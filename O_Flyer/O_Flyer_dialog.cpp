/********************************************************************/
/*								    */
/*		������ ���������� �������� "�����"  		    */
/*								    */
/*                   ���������� ���������                           */
/*                                                                  */
/********************************************************************/

#include <windows.h>
#include <stdio.h>
#include <io.h>

#include "..\Emi_root\controls.h"

#include "..\RSS_Feature\RSS_Feature.h"
#include "..\RSS_Object\RSS_Object.h"
#include "..\RSS_Unit\RSS_Unit.h"
#include "..\RSS_Kernel\RSS_Kernel.h"
#include "..\RSS_Model\RSS_Model.h"

#include "O_Flyer.h"

#pragma warning(disable : 4996)

#define  SEND_ERROR(text)    SendMessage(RSS_Kernel::kernel_wnd, WM_USER,  \
                                         (WPARAM)_USER_ERROR_MESSAGE,      \
                                         (LPARAM) text) ;

#include "resource.h"


/*********************************************************************/
/*								     */
/* 	     ���������� ��������� ����������� ���� HELP	             */

  INT_PTR CALLBACK  Object_Flyer_Help_dialog(  HWND hDlg,     UINT Msg, 
                                             WPARAM wParam, LPARAM lParam) 
{
 RSS_Module_Flyer  Module ;
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
/* 	     ���������� ��������� ����������� ���� CREATE            */

  INT_PTR CALLBACK  Object_Flyer_Create_dialog(  HWND hDlg,     UINT Msg, 
                                               WPARAM wParam, LPARAM lParam) 
{
        RSS_Module_Flyer  Module ;
 static   RSS_Model_data *data ;
              RSS_Object *object ;
 static             char  models_list[4096] ;
 static             RECT  PictureFrame ;
                 HBITMAP  hBitmap ;
//               HBITMAP  hBitmap_prv ;
                     int  elm ;               /* ������������� �������� ������� */
                     int  status ;
                    char  library[FILENAME_MAX] ;
                    char  value[512] ;
                     int  assigned ;
                    char *end ;
                     int  i ;
     
/*------------------------------------------------- ������� �������� */

  switch(Msg) {

/*---------------------------------------------------- ������������� */

    case WM_INITDIALOG: {

                   GetWindowRect(ITEM(IDC_PICTURE), &PictureFrame) ;

              data=(RSS_Model_data *)lParam ;
/*- - - - - - - - - - - - - - - - - - - ������������� ������ ������� */
           RSS_Model_list(data->lib_path, models_list,
                                   sizeof(models_list)-1, "BODY") ;

                                          CB_CLEAR(IDC_MODEL) ;
     for(end=models_list ; *end ; ) {
                                      CB_ADD_LIST(IDC_MODEL, end) ;
                                         end+=strlen(end)+1 ;
                                         end+=strlen(end)+1 ;
                                    }
/*- - - - - - - - - - - - - - - - - - - - ���������� �������� ������ */
     while(data->model[0]!=0) {

        for(end=models_list ; *end ; ) {                            /* ���� ������ �� ������ */
                        if(!stricmp(data->model, end))  break ;
                                         end+=strlen(end)+1 ;
                                         end+=strlen(end)+1 ;
                                       }

           if(*end==0) {  data->model[0]=0 ;                        /* ���� ����� ������ ���... */
                             break ;          }

                      sprintf(data->path, "%s\\%s", data->lib_path, end) ;
           RSS_Model_ReadPars(data) ;                               /* ��������� ��������� ������ */

                                  break ;
                              }
/*- - - - - - - - - - - - - - - - - - - - -   ������������� �������� */
     while(data->picture[0]!=0) {
    
          status=RSS_Model_Picture(data, &hBitmap, &PictureFrame) ;
       if(status)  break ;

               SendMessage(ITEM(IDC_PICTURE), 
                              STM_SETIMAGE,
                               (WPARAM)IMAGE_BITMAP,
                                (LPARAM)hBitmap     ) ;

                                      break ;
                                }
/*- - - - - - - - - - - - - - - - -  ������������� ������ ���������� */
       for(i=0 ; i<5 ; i++) {
                                       SETs(IDC_PAR_NAME_1+i, data->pars[i].text) ;
         if(data->pars[i].text[0]!=0)  SHOW(IDC_PAR_VALUE_1+i) ;
         else                          HIDE(IDC_PAR_VALUE_1+i) ;
                            }
/*- - - - - - - - - - - - - - - - - - - - -  ������������� ��������� */
          DISABLE(IDC_CREATE) ;

            SETs(IDC_NAME,         data->name) ;
            SETs(IDC_LIBRARY_PATH, data->lib_path) ;
            SETc(IDC_MODEL,        data->model) ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  			  return(FALSE) ;
  			     break ;
  			}

/*------------------------------------------------ ��������� ������� */

    case WM_COMMAND:    {

	status=HIWORD(wParam) ;
	   elm=LOWORD(wParam) ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - ����� ���������� */
     if(elm==IDC_LIBRARY_PATH) {

        if(status!=EN_UPDATE)  return(FALSE) ;

              GETs(IDC_LIBRARY_PATH, library) ;

           RSS_Model_list(library, models_list,
                            sizeof(models_list)-1, "BODY") ;

                                          CB_CLEAR(IDC_MODEL) ;
      for(end=models_list ; *end ; ) {
                                       CB_ADD_LIST(IDC_MODEL, end) ;
                                          end+=strlen(end)+1 ;
                                          end+=strlen(end)+1 ;
                                     }

                                 DISABLE(IDC_CREATE) ;

                                      return(FALSE) ;
                               }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - ����� ������ */
     if(elm==IDC_MODEL) {
                              if(status!=CBN_SELCHANGE)  break ;

                         GETs(IDC_LIBRARY_PATH, data->lib_path) ;
                         GETc(elm, data->model ) ;                  /* ��������� �������� ������ */

        for(end=models_list ; *end ; ) {                            /* ���� ������ �� ������ */
                        if(!stricmp(data->model, end))  break ;
                                         end+=strlen(end)+1 ;
                                         end+=strlen(end)+1 ;
                                       }

                                    end+=strlen(end)+1 ;            /* ��������� ��� ����� */

                      sprintf(data->path, "%s\\%s", data->lib_path, end) ;
           RSS_Model_ReadPars(data) ;                               /* ��������� ��������� ������ */

        for(i=0 ; i<5 ; i++) {                                      /* ������ ������ ���������� ������ */
                                       SETs(IDC_PAR_NAME_1+i, data->pars[i].text) ;
         if(data->pars[i].text[0]!=0)  SHOW(IDC_PAR_VALUE_1+i) ;
         else                          HIDE(IDC_PAR_VALUE_1+i) ;
                             }

            status=RSS_Model_Picture(data, &hBitmap, &PictureFrame) ;
//       if(status)  break ;

               SendMessage(ITEM(IDC_PICTURE), STM_SETIMAGE,
                               (WPARAM)IMAGE_BITMAP, (LPARAM)hBitmap) ;

                             DISABLE(IDC_CREATE) ;
                        }
/*- - - - - - - - - - - - - - - - - - - - -  ������� ���������� ���� */
     if(elm==IDC_PAR_VALUE_1 ||
        elm==IDC_PAR_VALUE_2 ||
        elm==IDC_PAR_VALUE_3 ||
        elm==IDC_PAR_VALUE_4 ||
        elm==IDC_PAR_VALUE_5   ) {

                    if(status!=EN_UPDATE)  return(FALSE) ;

           for(assigned=1, i=0 ; i<5 ; i++) {
               if(!VISIBLE(IDC_PAR_VALUE_1+i))  break ;

                  GETsl(IDC_PAR_VALUE_1+i, value, sizeof(value)-1) ;
               if(*value==0) {  assigned=0 ;  break ;  } 
                                            }

               if(assigned)  ENABLE(IDC_CREATE) ;

                                      return(FALSE) ;
                                }
/*- - - - - - - - - - - - - - - - - - - - - - - - - �������� ������� */
     if(elm==IDC_CREATE) {

                  GETs(IDC_NAME,         data->name) ;
                  GETs(IDC_LIBRARY_PATH, data->lib_path ) ;
                  GETc(IDC_MODEL,        data->model) ;

          for(i=0 ; i<5 ; i++)
                  GETs(IDC_PAR_VALUE_1+i, data->pars[i].value) ;  

                object=Module.vCreateObject(data) ;
             if(object!=NULL)  EndDialog(hDlg, 0) ;

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


/*********************************************************************/
/*								     */
/* 	     ���������� ��������� ����������� ���� UNITS             */

  INT_PTR CALLBACK  Object_Flyer_Units_dialog(  HWND hDlg,     UINT Msg, 
                                              WPARAM wParam, LPARAM lParam) 
{
           static  HFONT  font ;         /* ����� */
 static RSS_Module_Flyer *Module ;
 static RSS_Object_Flyer *object ;
                RSS_Unit *unit ;
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

              object=(RSS_Object_Flyer *)lParam ;
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
         SendMessage(ITEM(IDC_LIST), WM_SETFONT, (WPARAM)font, 0) ;
/*- - - - - - - - - - - - - - - - - ������������� ������ ����������� */
#define  UNIT  object->Units.List[i].object

                                          LB_CLEAR(IDC_LIST) ;

     for(i=0 ; i<object->Units.List_cnt ; i++) {

                 sprintf(text, "%-10.10s %s", UNIT->Name, UNIT->Decl) ;
              LB_ADD_ROW(IDC_LIST, text) ;
                                               }

#undef   UNIT
/*- - - - - - - - - - - - - - ������������� ������ ����� ����������� */
#define   MODULES       RSS_Kernel::kernel->modules 
#define   MODULES_CNT   RSS_Kernel::kernel->modules_cnt 


                                          CB_CLEAR(IDC_TYPE) ;

   for(i=0 ; i<MODULES_CNT ; i++) 
     if(MODULES[i].entry->category      !=NULL &&
        MODULES[i].entry->identification!=NULL   )
      if(!stricmp("Unit", MODULES[i].entry->category)) {

             MODULES[i].entry->vGetParameter("$$MODULE_NAME", text) ;

                   CB_ADD_LIST(IDC_TYPE, text) ;
                                                       }

#undef    MODULES
#undef    MODULES_CNT
/*- - - - - - - - - - - - - - - - - - - - -  ������������� ��������� */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  			  return(FALSE) ;
  			     break ;
  			}

/*------------------------------------------------ ��������� ������� */

    case WM_COMMAND:    {

	status=HIWORD(wParam) ;
	   elm=LOWORD(wParam) ;
/*- - - - - - - - - - - - - - - - - - - - - - ���������� ����������� */
     if(elm==IDC_ADD) {

                 GETsl(IDC_NAME, unit_name, sizeof(unit_name)) ;
                  GETc(IDC_TYPE, unit_type) ;

       if(unit_name[0]==0) {
              SEND_ERROR("�� ������ ��� ����������") ;
                                return(FALSE) ;
                           }
       if(unit_type[0]==0) {
              SEND_ERROR("�� ����� ��� ����������") ;
                                return(FALSE) ;
                           }

          end=strchr(unit_type, ' ') ;
       if(end!=NULL)  *end=0 ;

          unit=Module->AddUnit(object, unit_name, unit_type, text) ;
       if(unit==NULL) {
                        SEND_ERROR(text) ;
                          return(FALSE) ;
                      }

                 sprintf(text, "%-10.10s %s", unit->Name, unit->Decl) ;
              LB_ADD_ROW(IDC_LIST, text) ;

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


