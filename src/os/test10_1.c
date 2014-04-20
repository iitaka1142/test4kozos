#include "../misc/defines.h"
#include "../misc/lib.h"
#include "kozos.h"

int test10_1_main(int argc, char * argv[])
{
  char *p1, *p2;

  puts("test10_1 started.\n");

  for(int i = 4; i <= 56; i += 4) {
    // メモリを獲得
    p1 = kz_kmalloc(i);
    p2 = kz_kmalloc(i);

    // 獲得した領域を特定のパターンで初期化する
    for (int j = 0; j < (i - 1); j++) {
      p1[j] = 'a';
      p2[j] = 'b';
    }
    p1[i - 1] = '\0';
    p2[i - 1] = '\0';

    putxval((unsigned long)p1, 8); puts(" "); puts(p1); puts("\n");
    putxval((unsigned long)p2, 8); puts(" "); puts(p2); puts("\n");

    // メモリ解放
    kz_kmfree(p1);
    kz_kmfree(p2);
  }

  puts("test10_1 exit.\n");
  
  return EXIT_SUCCESS;
}
