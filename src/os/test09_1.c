#include "../misc/defines.h"
#include "../misc/lib.h"
#include "kozos.h"

int test09_1_main(int argc, char * arvg[])
{
  puts(__func__); puts(" started.\n");

  puts(__func__); puts(" sleep in.\n");
  kz_sleep();
  puts(__func__); puts(" sleep out.\n");

  puts(__func__); puts(" chpri in.\n");
  kz_chpri(3); // change priority of this thread from 3 to 1
  puts(__func__); puts(" chpri out.\n");
  
  puts(__func__); puts(" wait in.\n");
  kz_wait();
  puts(__func__); puts(" wait out.\n");
  
  puts(__func__); puts(" trap in.\n");
  asm volatile ("trapa #1");
  puts(__func__); puts(" trap out.\n");
  
  puts(__func__); puts(" exit.\n");
  
  return EXIT_SUCCESS;
}
