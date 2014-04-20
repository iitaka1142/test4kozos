#ifndef _KOZOS_MSG_H_INCLUDED_
#define _KOZOS_MSG_H_INCLUDED_

#include "../misc/defines.h" 
#include "kozos.h"


/**
   set reciever to msg_box
 */
int set_reciever(kz_msgbox * msg_box, kz_thread * thread);

/**
   check message in msg_box
 */
Bool msg_in(kz_msgbox * msg_box);

/**
   recieve message from msg_box
 */
kz_thread_id_t recvmsg(kz_msgbox * msg_box);

#endif
