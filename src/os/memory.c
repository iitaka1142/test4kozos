#include "../misc/defines.h"
#include "../misc/lib.h"
#include "memory.h"
#include "kozos.h"

typedef struct _kzmem_block kzmem_block;

/**
   メモリブロック構造体

   確保されたメモリのヘッダ情報
*/
struct _kzmem_block {
  kzmem_block * next; ///< リンクトリスト実装用
  int size;
};

/**
   メモリプール
*/
typedef struct _kzmem_pool {
  int size;
  int num;
  kzmem_block *free;
} kzmem_pool;

/**
   メモリプールの定義
*/
static kzmem_pool pool[] = {
  {16, 8, NULL},
  {32, 8, NULL},
  {64, 4, NULL}
};
#define MEMORY_AREA_NUM (sizeof(pool)/sizeof(*pool))

/// メモリプールの初期化
static int kzmem_init_pool(kzmem_pool * pool)
{
  extern char freearea; ///< @see ld.scr
  static char * area = &freearea;

  // 内部関数 ※ Gcc固有
  kzmem_block * get_mem_block_at(int index)
  {
    return (kzmem_block *)(area + (pool->size) * index);
  }
  // 内部関数(ここまで)
  
  // 最終要素から手前へ構造体を定義していく
  // 最終要素はnextにNULLを持つ
  kzmem_block * next = NULL;
  for(int i = pool->num - 1; i >= 0; --i) {
    kzmem_block * mem_block = get_mem_block_at(i);
    
    mem_block->next = next;
    mem_block->size = pool->size;
    
    next = mem_block;
  }
  
  // 領域の先頭を開放済みリンクトリストへ接続する
  pool->free = (kzmem_block *)area;

  // 未使用エリアの先頭アドレスareaを更新する
  area += pool->size * pool->num;

  return EXIT_SUCCESS;
}

// メモリ領域の初期化
int kzmem_init(void)
{
  for(int i = 0; i < MEMORY_AREA_NUM; i++) {
    (void)kzmem_init_pool(&pool[i]);
  }

  return EXIT_SUCCESS;
}

void * kzmem_alloc(unsigned int size)
{
  kzmem_block * mem_block = NULL;
  
  for (int i = 0; (mem_block == NULL) && (i < MEMORY_AREA_NUM); ++i) {
    kzmem_pool * this_pool = &pool[i];

    if (this_pool->free == NULL) {
      // 空き領域のないブロックなので、次を探す
    }
    else if (size > (this_pool->size - sizeof(kzmem_block))) {
      // ブロックのサイズが小さいので、次を探す
    }
    else {
      // 使用できる領域を発見したので、確保する
      mem_block = this_pool->free;
      
      // こういう実際は1回しか行われない処理は、ループの外に書きたい
      this_pool->free = this_pool->free->next;
    }
  }

  if ( mem_block == NULL ) {
    kz_sysdown("no memory to allocate\n");
    // no return
  }
  else {
    mem_block->next = NULL;

    return mem_block + 1;
  }
}

int kzmem_free(void * p)
{
  kzmem_block * mem_block = ((kzmem_block *)p) - 1;//ヘッダを得るために領域の手前へアクセスする
  kzmem_pool * this_pool = NULL;
  
  // pのsizeを見てどのプールに戻すか決める
  for (int i = 0; (this_pool == NULL) && (i < MEMORY_AREA_NUM); i++) {
    if ((pool[i].size) == (mem_block->size)) {
      this_pool = &(pool[i]);
    }
    else {
      // 次のプールを見に行く
    }
  }

  if ( this_pool == NULL ) {
    // 見つからなかったら停止
    kz_sysdown("no memory to free\n");
  }
  else {
    // 決まったら、pを解放領域へcons
    mem_block->next = this_pool->free;
    this_pool->free = mem_block;
  }

  return EXIT_SUCCESS;
}
