#include "../misc/defines.h"
#include "../misc/lib.h"
#include "kozos.h"

#define PROMPT "> "

int test08_1_main(int argc, char * argv[])
{
  static char buf[32];
  static Bool exit = false;
  
  puts("test08_1_main started\n");
  
  do {
    puts(PROMPT); 
    gets(buf);
  
    if (strncmp(buf, "echo ", 5) == Same) {
      puts(&(buf[5]));
      puts("\n");
    }
    else if (strncmp(buf, "exit", 4) == Same) {
      exit = true;
    }
    else {
      puts("unknown.\n");
    }
    
  } while(! exit);
  
  puts("test08_1 exit.\n");
  
  return EXIT_SUCCESS;
}

