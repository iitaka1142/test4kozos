#include "../misc/defines.h"
#include "../misc/lib.h"
#include "../misc/serial.h"
#include "kozos.h"
#include "consdrv.h"

static int send_use(serial_port port)
{
  #define len 3
  char * buffer = kz_kmalloc(len);

  buffer[0] = '0';
  buffer[1] = CONSDRV_CMD_USE;
  buffer[2] = (char)('0' + port);

  kz_send(MSGBOX_ID_CONSOUTPUT, len, buffer); 

  #undef len
  return EXIT_SUCCESS;
}

static int send_write(cString str)
{
  char * buffer;
  int len = strlen(str) + 2;

  buffer = (char *)kz_kmalloc(len);
  buffer[0] = '0';
  buffer[1] = (char)CONSDRV_CMD_WRITE;

  memcpy(&(buffer[2]), str, len);

  kz_send(MSGBOX_ID_CONSOUTPUT, len, buffer); 
  
  return len;
}

int command_main(int argc, char * argv[])
{
  unsigned int size;
  char * str;

  send_use(SERIAL_DEFAULT_DEVICE);

  while(true) {
    send_write("command> ");

    kz_recv(MSGBOX_ID_CONSINPUT, &size, &str);
    str[size] = '\0';

    if( strncmp((cString)str, "echo ", 5) == Same ) {
      send_write((str + 5)); send_write("\n");
    }
    else {
      send_write("unkown\n");
    }

    kz_kmfree(str);
  }

  return EXIT_SUCCESS;
}
