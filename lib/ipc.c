#include <ipc.h>
#include <fs.h>
#include <string.h>
#include <misc.h>

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
 * Implementation: It clean the msg_output and msg_input, then fill the
 * msg_input with the correct data (passed as parameters) and set the flag ready
 * to 'true' for the corresponding server.
 */
void
msg_send(const int to, const int number, const void *data, const int size)
{
  int id = cpuinfo->cpuid;

  memset(cpuinfo->msg_output, 0, sizeof(struct message));
  memset(cpuinfo->msg_input, 0, sizeof(struct message));

  cpuinfo->msg_output->from = id;
  cpuinfo->msg_output->number = number;
  memcpy(&cpuinfo->msg_output->data, data, size);

  cpuinfo->msg_ready[to] = true;
}

/*
 * msg_receive() - Wait for a reply from a server.
 *
 * This function it is usually called just after msg_send() and waits for the
 * reply from the server.
 *
 * Implementation: Check if there is any message in the msg_input "box". When
 * the message will fill the box with the reply it returns. It return a void
 * because when this function returns the message is already in msg_input.
 */
void
msg_receive()
{
  while (1)
    if (cpuinfo->msg_input->number)
      break;
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
  if (! check_server()) {
    return cpuinfo->msg_input;;
  }

  int id = cpuinfo->cpuid;
  int i;
  int from = -1;

  bool *base_r = (bool*)((phys_addr_t)cpuinfo->msg_ready - (_4KB_ * id));
  while (1) {
    for (i = 0 ; i < 8/*get_ncpus()*/ ; i++) {
      if (*(bool*)((phys_addr_t)base_r + (_4KB_ * i) + id)) {
        from = i;
	break;
      }
    }
    if (from != -1)
      break;
  }

  *(bool*)((phys_addr_t)base_r + (_4KB_ * from) + id) = false;

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
void
msg_reply(const int to, const int number, const void *data, const int size)
{
  if (! check_server()) {
    return;
  }

  int id = cpuinfo->cpuid;

  struct message *base = (struct message *)((phys_addr_t)cpuinfo->msg_input - (_4KB_ * id));
  struct message *inbox = (struct message *)((phys_addr_t)base + (_4KB_ * to));

  inbox->from = id;
  inbox->number = number;
  memcpy(&inbox->data, data, size);
}
