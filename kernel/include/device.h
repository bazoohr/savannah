#ifndef __DEVICE_H__
#define __DEVICE_H__

struct input_device {
  int id_id;
  int (*id_read)(void);
  void (*id_flush)(void);
  struct proc *id_waiting_proc;
};

struct output_device {
  int od_id;
  int (*od_write)(const char *buf);
};

union device {
  struct input_device  idev;
  struct output_device odev;
};

#define DEV_KBD  0
#define DEV_SCR  1

#define IN_DEVS  1
#define OUT_DEVS 1

#define NDEV  (IN_DEVS + OUT_DEVS)

void device_register (int devno, union device *dev);
void device_signal_data_ready (int idevno);
void device_wait_for_data (int idevno);
int device_read (int idevno, char * const buf);

#endif /* __DEVICE_H__ */
