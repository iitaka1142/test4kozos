#include "defines.h"
#include "intr.h"
#include "interrupt.h"

/**
 */
int softvec_init(void)
{
  softvec_type_t type;
  for (type = 0; type < SOFTVEC_TYPE_NUM; type++ ) {
    softvec_setintr(type, NULL);
  }

  return EXIT_SUCCESS;
}

/**
   意図的なMISRA-C違反: 「returnは1つ」に違反。異常系は関数の最後でなくともreturnする。
   可読性のため、この方が良いという考えもある。
   参考: 「リファクタリング」 M.Fawler
 */
int softvec_setintr(softvec_type_t type, softvec_handler_t handler)
{
  if( type < 0 || SOFTVEC_TYPE_NUM <= type ) {
    return -1;
  }

  SOFTVECS[type] = handler;

  return EXIT_SUCCESS;
}

/**
   @see interrupt.h
 */
void interrupt(softvec_type_t type, stack_ptr_t sp)
{
  if( type < 0 || SOFTVEC_TYPE_NUM <= type ) {
    return;
  }

  softvec_handler_t handler = SOFTVECS[type];

  if( handler != NULL ) {
    handler(type, sp);
  }
  
  return;
}
  
