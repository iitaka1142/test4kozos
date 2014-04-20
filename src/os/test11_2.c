#include "../misc/defines.h"
#include "../misc/lib.h"
#include "kozos.h"

int test11_2_main(int argc, char * argv[])
{
  char * p;
  unsigned int size;
  
  puts("test11_2 started.\n");

  puts("test11_2 send in.\n");
  kz_send(MSGBOX_ID_MSGBOX1, 15, "static memory\n");
  puts("test11_2 send out.\n");
  
  p = kz_kmalloc(18);
  strcpy(p, "allocated memory\n");
  puts("test11_2 send in.\n");
  kz_send(MSGBOX_ID_MSGBOX1, 18, p);
  puts("test11_2 send out.\n");
  /// 開放は送信先で実施

  puts("test11_2 recv in.\n");
  kz_recv(MSGBOX_ID_MSGBOX2, &size, &p);
  puts("test11_2 recv out.\n");
  puts(p);

  puts("test11_2 recv in.\n");
  kz_recv(MSGBOX_ID_MSGBOX2, &size, &p);
  puts("test11_2 recv out.\n");
  puts(p);
  kz_kmfree(p);

  puts("test11_2 exit.\n");

  return EXIT_SUCCESS;
  
}
