
#include <windows.h>
#include <stdio.h>
#include <io.h>

#include "RSS_model.h"

#pragma warning(disable : 4996)


BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
    switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
		case DLL_PROCESS_DETACH:
			break;
    }
    return TRUE;
}


/*********************************************************************/
/*								     */
/*           ������������ ������ ������ ������� � �������            */

 RSS_MODEL_API int  RSS_Model_list(char *lib_path, 
                                   char *list, int  list_max, 
                                   char *types                )

{
            intptr_t  group ;                   /* ���������� ��������� ������ ������ */
  struct _finddata_t  file_info ;
                char  mask[FILENAME_MAX] ;
                char  path[FILENAME_MAX] ;
                FILE *file ;
                char  buff[4096] ;
                char  object[256] ;
                char *entry ;
                char *end ;
                char *next ;
                 int  status ;

/*------------------------------------------------------- ���������� */

             memset(list, 0, list_max) ;

                    next=list ;

/*---------------------------------- ������������ ������� ���������� */

	            sprintf(mask, "%s\\*.obj", lib_path) ;          /* ������ ����� */ 
           group=_findfirst(mask, &file_info);			    /* �������� ����� */
        if(group<0)  return(-1) ;                                   /* ���� ������ ���... */

    while(1) {                                                      /* CIRCLE.1 */

/*--------------------------------------- ���������� �������� ������ */

      do {                                                          /* BLOCK.0 */
/*- - - - - - - - - - - - - - - - - - - - - - ���������� ����� ����� */
              sprintf(path, "%s\\%s", lib_path, file_info.name) ;   /* ��������� ���� � ����� */
           file=fopen(path, "rt") ;                                 /* ��������� ���� */
        if(file==NULL)  break ;
        
               memset(buff, 0, sizeof(buff)) ;
                fread(buff, 1, sizeof(buff)-1, file) ;              /* ��������� ����� */
               fclose(file) ;                                       /* ��������� ���� */
/*- - - - - - - - - - - - - - - - - - - - - -  �������� ���� ������� */
             entry=strstr(buff, "#OBJECT=") ;                       /* �����.���� ������� */
          if(entry==NULL)  break ;
             entry+=strlen("#OBJECT=") ;

                 memset(object, 0, sizeof(object)) ;
                strncpy(object, entry, sizeof(object)-1) ;          /* ��������� ��� ������� */
             end=strchr(object, '\n') ;                             /* ����������� ��� ������� */
          if(end==NULL)  break ;
            *end=0 ;

          if(strstr(types, object)==NULL)  break ;                  /* �������� ���� �������... */
/*- - - - - - - - - - - - - - - - - - - - - �������� �������� ������ */
             entry=strstr(buff, "#MODEL=") ;                        /* �����.�������� ������ */
          if(entry==NULL)  break ;
             entry+=strlen("#MODEL=") ;

             end=strchr(entry, '\n') ;                              /* ����������� �������� ������ */
          if(end==NULL)  break ;
            *end=0 ;

          if((next-list)+1+strlen(entry)
                          +strlen(file_info.name)+3>list_max)  break ;

             memcpy(next, entry, strlen(entry)+1) ;
                    next+=strlen(entry)+1 ;

             memcpy(next, file_info.name, strlen(file_info.name)+1) ;
                    next+=strlen(file_info.name)+1 ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
         } while(0) ;                                               /* BLOCK.0 */  

/*---------------------------------------------- ���� ��������� ���� */

		     status=_findnext(group, &file_info) ;          
		  if(status!=0)  break ;

/*---------------------------------- ������������ ������� ���������� */
             }                                                      /* CONTINUE.1 */
/*------------------------------------------------------- ���������� */

		        _findclose(group) ;                         /* ��������� ����� */

                             next[0]=0 ;
                             next[1]=0 ;            

/*-------------------------------------------------------------------*/

   return(0) ;
}


/*********************************************************************/
/*								     */
/*                  ���������� ���������� ������                     */ 

 RSS_MODEL_API int  RSS_Model_ReadPars(RSS_Model_data *data)

{
   FILE *file ;
   char  text[4096] ;
   char *end ;
    int  section_flag ;           /* ���� ������ � ������� ������ */
    int  n_sect ;                  
    int  i ;

/*----------------------------------------------------- ������������� */

              memset(data->object,  0, sizeof(data->object )) ;
              memset(data->model,   0, sizeof(data->model  )) ;
              memset(data->picture, 0, sizeof(data->picture)) ;

       for(i=0 ; i<_MODEL_PARS_MAX ; i++)
              memset(data->pars[i].text, 0, sizeof(data->pars[i].text)) ;

/*---------------------------------------------------- �������� ����� */

           file=fopen(data->path, "rt") ;                            /* ��������� ���� */
        if(file==NULL) {
                            sprintf(text, "File open error: %s", data->path) ;
                         MessageBox(NULL, text, "���������� ������", 0) ;
                           return(-1) ;
                       }
/*------------------------------------------------- ���������� ����� */

                  section_flag=0 ;
                        n_sect=0 ;

                      memset(text, 0, sizeof(text)) ;

     while(1) {                                                     /* CIRCLE.1 - ��������� ������ ���� */
/*- - - - - - - - - - - - - - - - - - -  ���������� ��������� ������ */
                   end=fgets(text, sizeof(text)-1, file) ;          /* ��������� ������ */
                if(end==NULL)  break ;

            if(text[0]==';')  continue ;                            /* �������� ����������� */

                end=text+strlen(text)-1 ;                           /* ������� ������ ����� ������ */
            if(*end=='\n')  *end=0 ;
/*- - - - - - - - - - - - - - - - - - - - - - �������� - ��� ������� */
            if(!memicmp(text, "#OBJECT=", strlen("#OBJECT="))) {

                  strncpy(data->object, text+strlen("#OBJECT="), 
                                              sizeof(data->object)) ;
                                                               }
/*- - - - - - - - - - - - - - - - - - - - - - - -  �������� - ������ */
       else if(!memicmp(text, "#MODEL=", strlen("#MODEL="))) {

                  strncpy(data->model, text+strlen("#MODEL="), 
                                              sizeof(data->model)) ;
                                                             }
/*- - - - - - - - - - - - - - - - - - - - - - -  �������� - �������� */
       else if(!memicmp(text, "#PICTURE=", strlen("#PICTURE="))) {

                  strncpy(data->picture, text+strlen("#PICTURE="), 
                                                sizeof(data->picture)) ;
                                                                 }
/*- - - - - - - - - - - - - - - - - - - - - - - �������� - ��������� */
       else if(!memicmp(text, "#PAR1=", strlen("#PAR1="))) {
                 strncpy(data->pars[0].text, text+strlen("#PAR1="), 
                                          sizeof(data->pars[0].text)) ;
                                                           }
       else if(!memicmp(text, "#PAR2=", strlen("#PAR2="))) {
                 strncpy(data->pars[1].text, text+strlen("#PAR2="), 
                                          sizeof(data->pars[1].text)) ;
                                                           }
       else if(!memicmp(text, "#PAR3=", strlen("#PAR3="))) {
                 strncpy(data->pars[2].text, text+strlen("#PAR3="), 
                                          sizeof(data->pars[2].text)) ;
                                                           }
       else if(!memicmp(text, "#PAR4=", strlen("#PAR4="))) {
                 strncpy(data->pars[3].text, text+strlen("#PAR4="), 
                                          sizeof(data->pars[3].text)) ;
                                                           }
       else if(!memicmp(text, "#PAR5=", strlen("#PAR5="))) {
                 strncpy(data->pars[4].text, text+strlen("#PAR5="), 
                                          sizeof(data->pars[4].text)) ;
                                                           }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
              }
/*--------------------------------------------------- �������� ����� */

                fclose(file) ;

/*-------------------------------------------------------------------*/

    return(0) ;
}


/*********************************************************************/
/*								     */
/*               ���������� ������ �������� ������                   */ 

 RSS_MODEL_API int  RSS_Model_ReadSect(RSS_Model_data *data)

{
   FILE *file ;
   char  text[4096] ;
   char *end ;
    int  section_flag ;           /* ���� ������ � ������� ������ */
    int  n_sect ;                  
    int  i ;

/*----------------------------------------------------- ������������� */

   for(i=0 ; i<_MODEL_SECT_MAX ; i++) {
                               data->sections[i].title[0]= 0 ;

         if(data->sections[i].decl!=NULL)
                        delete data->sections[i].decl ;
                               data->sections[i].decl=new std::string ;
                                      }
/*---------------------------------------------------- �������� ����� */

        if(data->path[0]==0)  return(0) ;

           file=fopen(data->path, "rt") ;                            /* ��������� ���� */
        if(file==NULL) {
                            sprintf(text, "File open error: %s", data->path) ;
                         MessageBox(NULL, text, "���������� ������", 0) ;
                           return(-1) ;
                       }
/*------------------------------------------------- ���������� ����� */

                  section_flag=0 ;
                        n_sect=0 ;

     while(1) {                                                     /* CIRCLE.1 - ��������� ������ ���� */
/*- - - - - - - - - - - - - - - - - - -  ���������� ��������� ������ */
                      memset(text, 0, sizeof(text)) ;
                   end=fgets(text, sizeof(text)-1, file) ;          /* ��������� ������ */
                if(end==NULL)  break ;

            if(text[0]==';')  continue ;                            /* �������� ����������� */

                end=text+strlen(text)-1 ;                           /* ������� ������ ����� ������ */
            if(*end=='\n')  *end=0 ;
/*- - - - - - - - - - - - - - - - - - - - - - -  ������/����� ������ */
            if(!memicmp(text, "#BEGIN ", strlen("#BEGIN "))) {

                    strcpy(data->sections[n_sect].title, 
                                    text+strlen("#BEGIN "))  ;

                                section_flag=1 ;
                                                             }
       else if(!memicmp(text, "#END",    strlen("#END"))   ) {

                                section_flag=0 ;
                                      n_sect++ ;
                                                             }
       else if(section_flag                                ) {

                   *data->sections[n_sect].decl+=text ;
                   *data->sections[n_sect].decl+="\n" ;
                                                             }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
              }
/*--------------------------------------------------- �������� ����� */

                fclose(file) ;

/*-------------------------------------------------------------------*/

    return(0) ;
}


/*********************************************************************/
/*								     */
/*                  ���������� �������� ������                       */ 

 RSS_MODEL_API int  RSS_Model_Picture(RSS_Model_data *data, 
                                             HBITMAP *hBitmap,
                                                RECT *Frame  )

{
     char  path[1024] ;
   
/*---------------------------------------------- ���������� �������� */

        sprintf(path, "%s\\%s", data->lib_path, data->picture) ;

      *hBitmap=(HBITMAP)LoadImage(NULL, path, IMAGE_BITMAP, 
                                              Frame->right -Frame->left,
                                              Frame->bottom-Frame->top,
                                              LR_CREATEDIBSECTION | 
//                                            LR_DEFAULTSIZE      |
//                                            LR_LOADTRANSPARENT  |
                                              LR_LOADFROMFILE      ) ;
   if(*hBitmap == NULL) return(-1) ;

/*-------------------------------------------------------------------*/

     return(0) ;
} 
