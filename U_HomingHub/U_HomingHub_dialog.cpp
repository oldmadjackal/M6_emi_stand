/********************************************************************/
/*								    */
/*       ������ ���������� ����������� "��������������� ���"        */
/*								    */
/*                   ���������� ���������                           */
/*                                                                  */
/********************************************************************/

#include <windows.h>
#include <stdio.h>
#include <io.h>
#include <math.h>

#include "..\Emi_root\controls.h"

#include "..\RSS_Feature\RSS_Feature.h"
#include "..\RSS_Object\RSS_Object.h"
#include "..\RSS_Unit\RSS_Unit.h"
#include "..\RSS_Kernel\RSS_Kernel.h"
#include "..\RSS_Model\RSS_Model.h"

#include "U_HomingHub.h"

#pragma warning(disable : 4996)

#define  SEND_ERROR(text)    SendMessage(RSS_Kernel::kernel_wnd, WM_USER,  \
                                         (WPARAM)_USER_ERROR_MESSAGE,      \
                                         (LPARAM) text) ;

#include "resource.h"


/*********************************************************************/
/*								     */
/* 	     ���������� ��������� ����������� ���� HELP	             */

  INT_PTR CALLBACK  Unit_HomingHub_Help_dialog(  HWND hDlg,     UINT Msg, 
                                               WPARAM wParam, LPARAM lParam) 
{
      RSS_Module_HomingHub  Module ;
                       int  elm ;         /* ������������� �������� ������� */
                   LRESULT  status ;
                   LRESULT  index ;
                       int  insert_flag ;
                      char *help ;
                      char  text[512] ;
                      char *end ;
                       int  i ; 

/*------------------------------------------------- ������� �������� */

  switch(Msg) {

/*---------------------------------------------------- ������������� */

    case WM_INITDIALOG: {
/*- - - - - - - - - - - - - - - - - - - - -  ������������� ��������� */
   for(i=0 ; Module.mInstrList[i].name_full!=NULL ; i++) {

     if(Module.mInstrList[i].help_row==NULL)  continue ;

         index=LB_ADD_ROW (IDC_HELP_LIST, Module.mInstrList[i].help_row) ;
               LB_SET_ITEM(IDC_HELP_LIST, index, Module.mInstrList[i].help_full) ;
                                                         }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  			  return(FALSE) ;
  			     break ;
  			}

/*------------------------------------------------ ��������� ������� */

    case WM_COMMAND:    {

	status=HIWORD(wParam) ;
	   elm=LOWORD(wParam) ;
/*- - - - - - - - - - - - - - - - - - - - - - - -  ��������� ������� */
       if(elm==IDC_HELP_LIST) {

        if(status==LBN_DBLCLK) {

             index=        LB_GET_ROW (elm) ;
              help=(char *)LB_GET_ITEM(elm) ;
                           LB_GET_TEXT(elm, index, text) ;
           if(text[0]!='#' || 
              help   ==NULL  )  return(FALSE) ;

               status=LB_GET_TEXT(elm, index+1, text) ;
           if( status==LB_ERR ||
              text[0]==  '#'    )  insert_flag=1 ;
           else                    insert_flag=0 ;

         do {
                   memset(text, 0, sizeof(text)) ;
                  strncpy(text, help, sizeof(text)-1) ;
               end=strchr(text, '\n') ;
           if( end!=NULL)  *end=0 ;

           if(insert_flag) {  LB_INS_ROW(elm, index+1, text) ;
                                              index++ ;        }
              help=strchr(help, '\n') ;
           if(help==NULL)  break ;
              help++ ;
              
            } while(1) ;
                               }

			            return(FALSE) ;
                              }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
			  return(FALSE) ;
			     break ;
			}
/*--------------------------------------------------------- �������� */

    case WM_CLOSE:      {
                            EndDialog(hDlg, 0) ;
  			       return(FALSE) ;
			              break ;
			}
/*----------------------------------------------------------- ������ */

    default :        {
			  return(FALSE) ;
			    break ;
		     }
/*-------------------------------------------------------------------*/
	      }
/*-------------------------------------------------------------------*/

    return(TRUE) ;
}


/*********************************************************************/
/*								     */
/* 	     ���������� ��������� ����������� ���� CONFIG            */

  INT_PTR CALLBACK  Unit_HomingHub_Config_dialog(  HWND hDlg,     UINT Msg, 
                                                   WPARAM wParam, LPARAM lParam) 
{
 static                  HFONT  font ;         /* ����� */
          RSS_Module_HomingHub *Module ;
 static     RSS_Unit_HomingHub *hub ;
                    RSS_Object *object ;
               RSS_Unit_Homing *unit ;
 static                    int  n1 ;
 static                    int  n2 ;
                           int  elm ;          /* ������������� �������� ������� */
                           int  status ;
                          char  unit_name[1024] ;
                          char  unit_type[1024] ;
                          char  text[1024] ;
                          char *end ;
                           int  i ;
     
/*------------------------------------------------- ������� �������� */

  switch(Msg) {

/*---------------------------------------------------- ������������� */

    case WM_INITDIALOG: {

              hub=(RSS_Unit_HomingHub   *)lParam ;
           Module=(RSS_Module_HomingHub *)hub->Module ;
/*- - - - - - - - - - - - - - - - - - - - - - - - -  ������� ������� */
        if(font==NULL)
           font=CreateFont(14, 0, 0, 0, FW_THIN, 
                                 false, false, false,
                                  ANSI_CHARSET,
                                   OUT_DEFAULT_PRECIS,
                                    CLIP_DEFAULT_PRECIS,
                                     DEFAULT_QUALITY,
                                      VARIABLE_PITCH,
                                       "Courier New Cyr") ;
//         SendMessage(ITEM(IDC_LIST), WM_SETFONT, (WPARAM)font, 0) ;
/*- - - - - - - - - - - - -  ������������� ������� ����� ����������� */
#define   MODULES       RSS_Kernel::kernel->modules 
#define   MODULES_CNT   RSS_Kernel::kernel->modules_cnt 

                                          CB_CLEAR(IDC_TYPE_1_1) ;
                                          CB_CLEAR(IDC_TYPE_1_2) ;
                                          CB_CLEAR(IDC_TYPE_1_3) ;
                                          CB_CLEAR(IDC_TYPE_2_1) ;
                                          CB_CLEAR(IDC_TYPE_2_2) ;
                                          CB_CLEAR(IDC_TYPE_2_3) ;

   for(i=0 ; i<MODULES_CNT ; i++) 
     if(MODULES[i].entry->category      !=NULL &&
        MODULES[i].entry->identification!=NULL   )
      if(!stricmp("Unit", MODULES[i].entry->category)) {

        if(MODULES[i].entry->lego_type==NULL)  continue ;

        if(!stricmp(MODULES[i].entry->identification, Module->identification))  continue ;

             MODULES[i].entry->vGetParameter("$$MODULE_NAME", text) ;

        if(strstr(MODULES[i].entry->lego_type, "Homing" )!=NULL) {
                                                                    CB_ADD_LIST(IDC_TYPE_1_1, text) ;
                                                                    CB_ADD_LIST(IDC_TYPE_1_2, text) ;
                                                                    CB_ADD_LIST(IDC_TYPE_1_3, text) ;
                                                                    CB_ADD_LIST(IDC_TYPE_2_1, text) ;
                                                                    CB_ADD_LIST(IDC_TYPE_2_2, text) ;
                                                                    CB_ADD_LIST(IDC_TYPE_2_3, text) ;
                                                                 }
                                                       }

#undef    MODULES
#undef    MODULES_CNT
/*- - - - - - - - - - - - - - - - - - - - - -  ������������� ������� */
                 DISABLE(IDC_TYPE_1_1) ;
                 DISABLE(IDC_NAME_1_2) ;
                 DISABLE(IDC_TYPE_1_2) ;
                 DISABLE(IDC_NAME_1_3) ;
                 DISABLE(IDC_TYPE_1_3) ;

                 DISABLE(IDC_NAME_2_1) ;
                 DISABLE(IDC_TYPE_2_1) ;
                 DISABLE(IDC_NAME_2_2) ;
                 DISABLE(IDC_TYPE_2_2) ;
                 DISABLE(IDC_NAME_2_3) ;
                 DISABLE(IDC_TYPE_2_3) ;
/*- - - - - - - - - - - - - - - - - - - - -  ������������� ��������� */
#define U(p)  hub->units_1[p-1]

                                   n1=0 ;

        if(U(1)!=NULL) {
                            SETs(IDC_NAME_1_1, U(1)->Name) ;
                                               U(1)->Module->vGetParameter("$$MODULE_NAME", text) ;
                            SETc(IDC_TYPE_1_1, text) ;
                                   n1=1 ;

                         DISABLE(IDC_TYPE_1_1) ;
                         DISABLE(IDC_NAME_1_1) ;
                          ENABLE(IDC_NAME_1_2) ;
                          ENABLE(IDC_NAME_2_1) ;
                       }
        if(U(2)!=NULL) {
                            SETs(IDC_NAME_1_2, U(2)->Name) ;
                                               U(2)->Module->vGetParameter("$$MODULE_NAME", text) ;
                            SETc(IDC_TYPE_1_2, text) ;
                                   n1=2 ;

                         DISABLE(IDC_TYPE_1_2) ;
                         DISABLE(IDC_NAME_1_2) ;
                          ENABLE(IDC_NAME_1_3) ;
                       }
        if(U(3)!=NULL) {
                            SETs(IDC_NAME_1_3, U(3)->Name) ;
                                               U(3)->Module->vGetParameter("$$MODULE_NAME", text) ;
                            SETc(IDC_TYPE_1_3, text) ;
                                   n1=3 ;

                         DISABLE(IDC_TYPE_1_3) ;
                         DISABLE(IDC_NAME_1_3) ;
                       }

#undef  U
#define U(p)  hub->units_2[p-1]

                                   n2=0 ;

        if(U(1)!=NULL) {
                            SETs(IDC_NAME_2_1, U(1)->Name) ;
                                               U(1)->Module->vGetParameter("$$MODULE_NAME", text) ;
                            SETc(IDC_TYPE_2_1, text) ;
                                   n2=1 ;

                         DISABLE(IDC_TYPE_2_1) ;
                         DISABLE(IDC_NAME_2_1) ;
                          ENABLE(IDC_NAME_2_2) ;
                       }
        if(U(2)!=NULL) {
                            SETs(IDC_NAME_2_2, U(2)->Name) ;
                                               U(2)->Module->vGetParameter("$$MODULE_NAME", text) ;
                            SETc(IDC_TYPE_2_2, text) ;
                                   n2=2 ;

                         DISABLE(IDC_TYPE_2_2) ;
                         DISABLE(IDC_NAME_2_2) ;
                          ENABLE(IDC_NAME_2_3) ;
                       }
        if(U(3)!=NULL) {
                            SETs(IDC_NAME_2_3, U(3)->Name) ;
                                               U(3)->Module->vGetParameter("$$MODULE_NAME", text) ;
                            SETc(IDC_TYPE_2_3, text) ;
                                   n2=3 ;

                         DISABLE(IDC_TYPE_2_3) ;
                         DISABLE(IDC_NAME_2_3) ;
                       }

#undef  U
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  			  return(FALSE) ;
  			     break ;
  			}

/*------------------------------------------------ ��������� ������� */

    case WM_COMMAND:    {

	status=HIWORD(wParam) ;
	   elm=LOWORD(wParam) ;
/*- - - - - - - - - - - - - - - - - - - - - -  ���������� ���������� */
     if(status==EN_UPDATE) {

          if(elm==IDC_NAME_1_1) {
                                      ENABLE(IDC_TYPE_1_1) ;
                                } 
          if(elm==IDC_NAME_1_2) {
                                      ENABLE(IDC_TYPE_1_2) ;
                                } 
          if(elm==IDC_NAME_1_3) {
                                      ENABLE(IDC_TYPE_1_3) ;
                                } 
          if(elm==IDC_NAME_2_1) {
                                      ENABLE(IDC_TYPE_2_1) ;
                                } 
          if(elm==IDC_NAME_2_2) {
                                      ENABLE(IDC_TYPE_2_2) ;
                                } 
          if(elm==IDC_NAME_2_3) {
                                      ENABLE(IDC_TYPE_2_3) ;
                                } 

                                      return(FALSE) ;
                        }
/*- - - - - - - - - - - - - - - - - - - - - -  ����� ���� ���������� */
     if(status==CBN_SELCHANGE) {

          if(elm==IDC_TYPE_1_1) {
                                      ENABLE(IDC_NAME_1_2) ;
                                      ENABLE(IDC_NAME_2_1) ;
                                } 
          if(elm==IDC_TYPE_1_2) {
                                      ENABLE(IDC_NAME_1_3) ;
                                } 
          if(elm==IDC_TYPE_2_1) {
                                      ENABLE(IDC_NAME_2_2) ;
                                      ENABLE(IDC_SWITCH_1_2) ;
                                } 
          if(elm==IDC_TYPE_2_2) {
                                      ENABLE(IDC_NAME_2_3) ;
                                } 

                                      return(FALSE) ;
                               }
/*- - - - - - - - - - - - - - - - - - - - - - ���������� ����������� */
     if(elm==IDC_SET) {

                Module=(RSS_Module_HomingHub *)hub->Module ;
                object=                        hub->Owner ;

        for(i=n1 ; i<3 ; i++) {

          if(i==0)  {   GETs(IDC_NAME_1_1, unit_name) ;
                        GETc(IDC_TYPE_1_1, unit_type) ;   }
          if(i==1)  {   GETs(IDC_NAME_1_2, unit_name) ;
                        GETc(IDC_TYPE_1_2, unit_type) ;   }
          if(i==2)  {   GETs(IDC_NAME_1_3, unit_name) ;
                        GETc(IDC_TYPE_1_3, unit_type) ;   }

          if(unit_name[0]==0 ||
             unit_type[0]==0   )  continue ;

             end=strchr(unit_type, ' ') ;
          if(end!=NULL)  *end=0 ;

             unit=Module->AddUnit(object, unit_name, unit_type, text) ;
          if(unit==NULL) {
                            SEND_ERROR(text) ;
                              return(FALSE) ;
                         }

                hub->units_1[i]=unit ;
                            }

        for(i=n2 ; i<3 ; i++) {

          if(i==0)  {   GETs(IDC_NAME_2_1, unit_name) ;
                        GETc(IDC_TYPE_2_1, unit_type) ;   }
          if(i==1)  {   GETs(IDC_NAME_2_2, unit_name) ;
                        GETc(IDC_TYPE_2_2, unit_type) ;   }
          if(i==2)  {   GETs(IDC_NAME_2_3, unit_name) ;
                        GETc(IDC_TYPE_2_3, unit_type) ;   }

          if(unit_name[0]==0 ||
             unit_type[0]==0   )  continue ;

             end=strchr(unit_type, ' ') ;
          if(end!=NULL)  *end=0 ;

             unit=Module->AddUnit(object, unit_name, unit_type, text) ;
          if(unit==NULL) {
                            SEND_ERROR(text) ;
                              return(FALSE) ;
                         }

                hub->units_2[i]=unit ;
                            }

                            EndDialog(hDlg, 0) ;

                              return(FALSE) ;
                      }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
			  return(FALSE) ;
			     break ;
			}
/*--------------------------------------------------------- �������� */

    case WM_CLOSE:      {
                            EndDialog(hDlg, -1) ;
  			       return(FALSE) ;
			              break ;
			}
/*----------------------------------------------------------- ������ */

    default :        {
			  return(FALSE) ;
			    break ;
		     }
/*-------------------------------------------------------------------*/
	      }
/*-------------------------------------------------------------------*/

    return(TRUE) ;
}

