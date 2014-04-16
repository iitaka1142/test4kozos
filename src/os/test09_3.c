#include "../misc/defnes.h"
#include "kozos.h"
#include "lib.h"

int test09_3_main(int argc, char * arvg[])
{
  puts("test09_3 started.\n");

  puts("test09_3 wakeup \"test09_1\" in.\n");
  kz_wakeup(test09_1_id);
  puts("test09_3 wakeup \"test09_1\" out.\n");

  puts("test09_3 wakeup \"test09_2\" in.\n");
  kz_wakeup(test09_2_id);
  puts("test09_3 wakeup \"test09_2\" out.\n");

  puts("test09_3 wait in.\n");
  kz_wait();
  puts("test09_3 wait out.\n");
  
  puts("test09_3 exit in.\n");
  kz_exit();
  puts("test09_3 exit out.\n");
  
  return EXIT_SUCCESS;
}
