
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
/*           Формирование списка файлов моделей в разделе            */

 RSS_MODEL_API int  RSS_Model_list(char *lib_path, 
                                   char *list, int  list_max, 
                                   char *types                )

{
                 int  group ;                   /* Дескриптор поисковой группы файлов */
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

/*------------------------------------------------------- Подготовка */

             memset(list, 0, list_max) ;

                    next=list ;

/*---------------------------------- Сканирование раздела библиотеки */

	            sprintf(mask, "%s\\*.obj", lib_path) ;          /* Строим маску */ 
           group=_findfirst(mask, &file_info);			    /* Начинаем поиск */
        if(group<0)  return(-1) ;                                   /* Если флагов нет... */

    while(1) {                                                      /* CIRCLE.1 */

/*--------------------------------------- Считывание названия модели */

      do {                                                          /* BLOCK.0 */
/*- - - - - - - - - - - - - - - - - - - - - - Считывание шапки файла */
              sprintf(path, "%s\\%s", lib_path, file_info.name) ;   /* Формируем путь к файлу */
           file=fopen(path, "rt") ;                                 /* Открываем файл */
        if(file==NULL)  break ;
        
               memset(buff, 0, sizeof(buff)) ;
                fread(buff, 1, sizeof(buff)-1, file) ;              /* Считываем шапку */
               fclose(file) ;                                       /* Закрываем файл */
/*- - - - - - - - - - - - - - - - - - - - - -  Разводка вида объекта */
             entry=strstr(buff, "#OBJECT=") ;                       /* Позиц.вида объекта */
          if(entry==NULL)  break ;
             entry+=strlen("#OBJECT=") ;

                 memset(object, 0, sizeof(object)) ;
                strncpy(object, entry, sizeof(object)-1) ;          /* Извлекаем вид объекта */
             end=strchr(object, '\n') ;                             /* Терминируем вид объекта */
          if(end==NULL)  break ;
            *end=0 ;

          if(strstr(types, object)==NULL)  break ;                  /* Контроль вида объекта... */
/*- - - - - - - - - - - - - - - - - - - - - Разводка названия модели */
             entry=strstr(buff, "#MODEL=") ;                        /* Позиц.название модели */
          if(entry==NULL)  break ;
             entry+=strlen("#MODEL=") ;

             end=strchr(entry, '\n') ;                              /* Терминируем название модели */
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

/*---------------------------------------------- Ищем следующий флаг */

		     status=_findnext(group, &file_info) ;          
		  if(status!=0)  break ;

/*---------------------------------- Сканирование раздела библиотеки */
             }                                                      /* CONTINUE.1 */
/*------------------------------------------------------- Завершение */

		        _findclose(group) ;                         /* Закрываем поиск */

                             next[0]=0 ;
                             next[1]=0 ;            

/*-------------------------------------------------------------------*/

   return(0) ;
}


/*********************************************************************/
/*								     */
/*                  Считывание параметров модели                     */ 

 RSS_MODEL_API int  RSS_Model_ReadPars(RSS_Model_data *data)

{
   FILE *file ;
   char  text[4096] ;
   char *end ;
    int  section_flag ;           /* Флаг работы с секцией данных */
    int  n_sect ;                  
    int  i ;

/*----------------------------------------------------- Инициализация */

              memset(data->object,  0, sizeof(data->object )) ;
              memset(data->model,   0, sizeof(data->model  )) ;
              memset(data->picture, 0, sizeof(data->picture)) ;

       for(i=0 ; i<_MODEL_PARS_MAX ; i++)
              memset(data->pars[i].text, 0, sizeof(data->pars[i].text)) ;

/*---------------------------------------------------- Открытие файла */

           file=fopen(data->path, "rt") ;                            /* Открываем файл */
        if(file==NULL) {
                            sprintf(text, "File open error: ", data->path) ;
                         MessageBox(NULL, text, "Считывание модели", 0) ;
                           return(-1) ;
                       }
/*------------------------------------------------- Считывание файла */

                  section_flag=0 ;
                        n_sect=0 ;

                      memset(text, 0, sizeof(text)) ;

     while(1) {                                                     /* CIRCLE.1 - Построчно читаем файл */
/*- - - - - - - - - - - - - - - - - - -  Считывание очередной строки */
                   end=fgets(text, sizeof(text)-1, file) ;          /* Считываем строку */
                if(end==NULL)  break ;

            if(text[0]==';')  continue ;                            /* Проходим комментарий */

                end=text+strlen(text)-1 ;                           /* Удаляем символ конца строки */
            if(*end=='\n')  *end=0 ;
/*- - - - - - - - - - - - - - - - - - - - - - Параметр - ВИД ОБЪЕКТА */
            if(!memicmp(text, "#OBJECT=", strlen("#OBJECT="))) {

                  strncpy(data->object, text+strlen("#OBJECT="), 
                                              sizeof(data->object)) ;
                                                               }
/*- - - - - - - - - - - - - - - - - - - - - - - -  Параметр - МОДЕЛЬ */
       else if(!memicmp(text, "#MODEL=", strlen("#MODEL="))) {

                  strncpy(data->model, text+strlen("#MODEL="), 
                                              sizeof(data->model)) ;
                                                             }
/*- - - - - - - - - - - - - - - - - - - - - - -  Параметр - КАРТИНКА */
       else if(!memicmp(text, "#PICTURE=", strlen("#PICTURE="))) {

                  strncpy(data->picture, text+strlen("#PICTURE="), 
                                                sizeof(data->picture)) ;
                                                                 }
/*- - - - - - - - - - - - - - - - - - - - - - - Параметр - ПАРАМЕТРЫ */
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
/*--------------------------------------------------- Закрытие файла */

                fclose(file) ;

/*-------------------------------------------------------------------*/

    return(0) ;
}


/*********************************************************************/
/*								     */
/*               Считывание секций описаний модели                   */ 

 RSS_MODEL_API int  RSS_Model_ReadSect(RSS_Model_data *data)

{
   FILE *file ;
   char  text[4096] ;
   char *end ;
    int  section_flag ;           /* Флаг работы с секцией данных */
    int  n_sect ;                  
    int  i ;

/*----------------------------------------------------- Инициализация */

   for(i=0 ; i<_MODEL_SECT_MAX ; i++) {
                               data->sections[i].title[0]= 0 ;

         if(data->sections[i].decl!=NULL)
                        delete data->sections[i].decl ;
                               data->sections[i].decl=new std::string ;
                                      }
/*---------------------------------------------------- Открытие файла */

        if(data->path[0]==0)  return(0) ;

           file=fopen(data->path, "rt") ;                            /* Открываем файл */
        if(file==NULL) {
                            sprintf(text, "File open error: ", data->path) ;
                         MessageBox(NULL, text, "Считывание модели", 0) ;
                           return(-1) ;
                       }
/*------------------------------------------------- Считывание файла */

                  section_flag=0 ;
                        n_sect=0 ;

     while(1) {                                                     /* CIRCLE.1 - Построчно читаем файл */
/*- - - - - - - - - - - - - - - - - - -  Считывание очередной строки */
                      memset(text, 0, sizeof(text)) ;
                   end=fgets(text, sizeof(text)-1, file) ;          /* Считываем строку */
                if(end==NULL)  break ;

            if(text[0]==';')  continue ;                            /* Проходим комментарий */

                end=text+strlen(text)-1 ;                           /* Удаляем символ конца строки */
            if(*end=='\n')  *end=0 ;
/*- - - - - - - - - - - - - - - - - - - - - - -  Начало/конец секции */
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
/*--------------------------------------------------- Закрытие файла */

                fclose(file) ;

/*-------------------------------------------------------------------*/

    return(0) ;
}


/*********************************************************************/
/*								     */
/*                  Считывание картинки модели                       */ 

 RSS_MODEL_API int  RSS_Model_Picture(RSS_Model_data *data, 
                                             HBITMAP *hBitmap,
                                                RECT *Frame  )

{
     char  path[1024] ;
   
/*---------------------------------------------- Считывание картинки */

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
