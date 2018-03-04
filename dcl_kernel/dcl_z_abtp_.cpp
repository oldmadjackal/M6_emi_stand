/*********************************************************************/
/*                                                                   */
/*                   DATA CONVERTION LANGUAGE                        */
/*                                                                   */
/*          УТИЛИТЫ ДЛЯ РАБОТЫ С СЕТЕВЫМ ПРОТОКОЛОМ ABTP             */
/*                                                                   */
/*********************************************************************/

#if defined(AIX) || defined(LINUX)
#ifndef UNIX
#define  UNIX
#endif
#endif


#include  <errno.h>
#include  <stddef.h>
#include  <stdlib.h>
#include  <string.h>
#include  <ctype.h>
#include  <math.h>
#include  <memory.h>
#include  <sys/types.h>
#include  <sys/stat.h>

#define   DCL_INSIDE
#include "dcl.h"


#pragma warning(disable : 4996)

/*------------------------------------------------- Общие переменные */


/*---------------------------------------------------- Внешние связи */

          extern double  dcl_errno ;       /* Системный указатель ошибок -> DCL_SLIB.CPP */
                            

/*********************************************************************/
/*                                                                   */
/*               Отправка сообщения SIGNAL                           */

   int  Lang_DCL::zAbtpSignal(char *address, char *sgn_name,
                                             char *sgn_value,
                                             char *sgn_info, 
                                              int  sgn_info_size)

{
  return(0) ;
}


