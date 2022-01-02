/*********************************************************************/
/*                                                                   */
/*                  Утилита внешнего моделирования                   */
/*                                                                   */
/*                         Модель MISSILE                            */
/*                                                                   */
/*********************************************************************/

#include <windows.h>
#include <commctrl.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <tlhelp32.h>

#include <io.h>
#include <direct.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <time.h>
#include <ctype.h>
#include <process.h>
#include <errno.h>
#include <sys\timeb.h>
#include <sys\types.h>
#include <sys\stat.h>


#include "controls.h"
#include "resource.h"


#include "ExternalModel.h"

#pragma warning(disable : 4996)
#pragma warning(disable : 4267)

#define   _GRD_TO_RAD   0.017453


/********************************************************************/
/*                                                                  */
/*                          Расчет модели                           */

     int  EM_model_missile(Object *data)

{
   double  dt ;
   double  azim ;
   double  elev ;

#define    _V     300

/*------------------------------------------------------- Подготовка */

            dt=data->t2-data->t1 ;

/*---------------------------------------------- Неуправляемый полет */

   if(data->target[0]==0) {
                                   azim   =data->a_azim*_GRD_TO_RAD ;
                                   elev   =data->a_elev*_GRD_TO_RAD ;
           
                                data->v_x =_V*dt*cos(elev)*sin(azim) ;
                                data->v_y =_V*dt*sin(elev) ;
                                data->v_z =_V*dt*cos(elev)*cos(azim) ;

                                data->x  +=data->v_x ;
                                data->y  +=data->v_y ;
                                data->z  +=data->v_z ;

                                        return(0) ;
                          }
/*---------------------------------------------------- Самонаведение */


/*-------------------------------------------------------------------*/

    return(0) ;
}

