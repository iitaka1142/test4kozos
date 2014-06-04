#include "../misc/defines.h"
#include "../misc/interrupt.h"
#include "../misc/lib.h"
#include "kozos.h"

/// システムタスクとユーザスレッドの起動
static int start_threads(int argc, char * argv[])
{
  kz_run(consdrv_main, "consdrv", 1, 0x100, 0, NULL);
  kz_run(command_main, "command", 8, 0x100, 0, NULL);

  kz_chpri(15);

  INTR_ENABLE;
  while(True) {
    asm volatile ("sleep");
  }
  
  return EXIT_SUCCESS;
}

int main(void)
{
  INTR_DISABLE; // disable interrupt

  puts("kozos boot succeeded!\n");
  
  // start OS
  kz_start(start_threads, "idle", 0, 0x100, 0, NULL);
  // no return
  
  return 0;
}

