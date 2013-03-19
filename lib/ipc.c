#include <ipc.h>
#include <string.h>
#include <misc.h>
#include <panic.h>
#include <bitmap.h>

/*
 * msg_send() - Send a message to another process.
 *
 * to: The receiver process number.
 * number: The number corresponding to the type of the request.
 * data: The structure that needs to be passed to the process.
 * size: The size of the data.
 *
 * This function can be called from all the processes and everyone can use this
 * function to send a message to one of the server. Just the servers can receive
 * this type of messages.
 *
 * Usually it is use in the library to perform functions like fork() or exec()
 * or read(), the typical system calls.
 *
 * Implementation: Fill the msg_input with the correct data (passed as
 * parameters) and set the flag ready to 'true' for the corresponding server.
 */
void
msg_send(const int to, const int number, const void *data, const int size)
{
  int id = cpuinfo->cpuid;

  cpuinfo->msg_output->from = id;
  cpuinfo->msg_output->number = number;
  memcpy(cpuinfo->msg_output->data, data, size);

  set_bit(&cpuinfo->msg_ready_bitmap[to], id);
}

/*
 * msg_receive() - Wait for a reply from a server.
 *
 * from: The cpuid of the process from where you are waiting the message. In
 *       some cases the special parameter ANY can be specified if you don't know
 *       from which CPU the message come from.
 *
 * This function it is usually called just after msg_send() and waits for the
 * reply from the server.
 * It can be also used from the drivers to receive messages from the servers.
 *
 * Implementation: Check if there is any message in the msg_input "box". When
 * the message will fill the box with the reply it returns. It return an
 * integer, which is the cpuid from where the message come from.
 * In the current implementation we don't have just one input box, but as many
 * as the number of CPUs in the system. This is to prevent race conditions
 * between two messages sent at the same time. If the from parameter is a
 * specific CPU, then this function will poll just that "box", otherwhise if the
 * parameter is ANY it will loop all the boxes checking for some messages.
 */
int
msg_receive(int from)
{
  static int i = 0;
  int ncpus;

  ncpus = cpuinfo->ncpus;

  while (1) {
    if (from == ANY) {
      while (i < ncpus) {
        if (cpuinfo->msg_input[i].number) {
          cpuinfo->msg_input[i].number = 0;
          from = i;
          i++;
          break;
        }
        i++;
      }

      if (i >= ncpus) {
        i = 0;
      }
      if (from != ANY)
        break;
    } else {
      if (cpuinfo->msg_input[from].number) {
        cpuinfo->msg_input[from].number = 0;
        break;
      }
    }
  }

  return from;
}

/*
 * msg_check() - Check if there is any message for the server.
 *
 * This function can be called just by the servers, if a tries to call this
 * function (and somehow bypass the check) it will get an EPT violation anyway.
 * The purpose of this function is to scan the msg_ready array of all the
 * processes and check if anyone has a message pending.
 *
 * Implementation: The msg_ready structure (and also the msg_input and
 * msg_output) is always inside one 2MB page, and it is splitted in 4KB "pieces"
 * where every 4KB there is the corresponding structure for that cpuid. For
 * example: lets consider the process with cpuid 4. The msg_ready pointer is
 * saved at (base + (4KB * 4)), where the "base" is the beginning of the 2MB
 * page. Knowing this it's important to understand all the pointer computations
 * in this function.
 * At the beginning it finds out the 'base_r', which is the base pointer for the
 * msg_ready structure. In the while loop it checkes for all the cpuids if there
 * is a message pending just for that particular server. If it is set the 'from'
 * variable and exit the loop. It then clean the 'msg_ready', find the
 * correct pointer for the 'msg_output' structure and returns it.
 *
 * NOTE: All the castings are for safety reasons.
 */
struct message *
msg_check()
{
  int id = cpuinfo->cpuid;
  static int i = 0;
  int from;
  int bit;
  uint64_t ncpus;

  if (! check_server()) {
    return cpuinfo->msg_input;;
  }

  ncpus = cpuinfo->ncpus;
  from = -1;
  while (1) {
    if (cpuinfo->msg_ready_bitmap[id] != 0)
      break;
  }

  /* TODO Find if there is a faster way to check which CPU has a message */
  for (i = 0 ; i < ncpus ; i++) {
    bit = check_bit(&cpuinfo->msg_ready_bitmap[id], i);
    if (bit == 1) {
      from = i;
      break;
    }
  }

  clear_bit(&cpuinfo->msg_ready_bitmap[id], from);

  struct message *base_m = (struct message *)((phys_addr_t)cpuinfo->msg_output - (_4KB_ * id));

  return (struct message *)((phys_addr_t)base_m + (_4KB_ * from));
}

/*
 * msg_reply() - Reply to a message.
 *
 * to: The receiver process number.
 * number: The number corresponding to the type of the request.
 * data: The structure that needs to be passed to the process.
 * size: The size of the data.
 *
 * This function is called just by the server. After receiving the message
 * (using msg_check()) and executing the request, it replies to the message.
 *
 * Implementation: The idea behind is the same of msg_check(), so it finds the
 * base pointer of the 'msg_input' 2MB page, then it finds the correct
 * 'msg_input' box of the process specified wit the 'to' parameter, and then set
 * the correct fields specified as parameters.
 * It does return void becuase there is nothing important to return, since the
 * process waiting for the message is polling if there is anything in the inbox
 * box, so at the moment this function fills its 'msg_input' box the
 * 'msg_receive' will return and the process can read the result.
 */
/* TODO : Change the name!!! */
void
msg_reply(const int from, const int to, const int number, const void *data, const int size)
{
  int id;
  struct message *base;
  struct message *inbox;

  if (! check_server()) {
    panic ("Only Servers can use msg_reply!");
    return;
  }

  id = cpuinfo->cpuid;

  base = (struct message *)(((phys_addr_t)cpuinfo->msg_input - (_4KB_ * id)));
  inbox = (struct message *)(((phys_addr_t)base + (_4KB_ * to)));

  inbox[from].from = from;
  memcpy(inbox[from].data, data, size);
  inbox[from].number = number;
}
