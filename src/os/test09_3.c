#include "../misc/defines.h"
#include "../misc/lib.h"
#include "kozos.h"

extern kz_thread_id_t test09_1_id;
extern kz_thread_id_t test09_2_id;

int test09_3_main(int argc, char * arvg[])
{
  puts(__func__); puts(" started.\n");

  puts(__func__); puts(" wakeup (test09_1) in.\n");
  kz_wakeup(test09_1_id);
  puts(__func__); puts(" wakeup (test09_1) out.\n");

  puts(__func__); puts(" wakeup (test09_2) in.\n");
  kz_wakeup(test09_2_id);
  puts(__func__); puts(" wakeup (test09_2) out.\n");

  puts(__func__); puts(" wait in.\n");
  kz_wait();
  puts(__func__); puts(" wait out.\n");
  
  puts(__func__); puts(" exit in.\n");
  kz_exit();
  puts(__func__); puts(" exit out.\n");
  
  return EXIT_SUCCESS;
}
