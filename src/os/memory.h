#ifndef _KOZOS_MEMORY_H_INCLUDED_
#define _KOZOS_MEMORY_H_INCLUDED_

/// メモリ領域初期化
int kzmem_init(void);
/**<
   @return always EXIT_SUCCESS
*/

/// メモリ確保
void * kzmem_alloc(unsigned int size);
/**<
   @param size
   @return head address of allocated area
*/

/// メモリ開放
int kzmem_free(void * p); 
/**<
   @param p head address of area
   @return always EXIT_SUCCESS
*/

#endif
