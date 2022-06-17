#ifndef _SERIAL_H_
#define _SERIAL_H_

int openAndSetTermiosDevice();
int getData(int fd, int * out);
int closeFD(int fd);

#endif  // _ADDON_H_