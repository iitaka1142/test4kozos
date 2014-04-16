#include "../misc/defines.h"
#include "../misc/interrupt.h"
#include "../misc/lib.h"
#include "kozos.h"

kz_thread_id_t test09_1_id;
kz_thread_id_t test09_2_id;
kz_thread_id_t test09_3_id;

static int start_threads(int argc, char * argv[])
{
  test09_1_id = kz_run(test09_1_main, "test09_1", 1, 0x100, 0, NULL);
  test09_2_id = kz_run(test09_2_main, "test09_2", 2, 0x200, 0, NULL);
  test09_3_id = kz_run(test09_3_main, "test09_3", 3, 0x300, 0, NULL);

  kz_chpri(15);
  INTER_ENABLE;
  while(true) {
    asm volatile ("sleep");
  }
  
  return EXIT_SUCCESS;
}

int main(void)
{
  INTR_DISABLE; // disable interrupt

  puts("kozos boot succeeded!\n");
  
  // start OS
  kz_start(start_threads, "idle", 0x100, 0, NULL);
  // no return
  
  return 0;
}

