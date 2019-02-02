/*********************************************************************/
/*                                                                   */
/*                   DATA CONVERTION LANGUAGE                        */
/*                                                                   */
/*          УТИЛИТА ПОСТРОЕНИЯ СПИСКА ФАЙЛОВ ПО МАСКЕ                */
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

#ifdef UNIX
#include  <unistd.h>
#include  <dirent.h>
#define   stricmp  strcasecmp
#define   O_BINARY    0
#else
#include  <io.h>
#include  <direct.h>
#include  <aclapi.h>
#endif

#define   DCL_INSIDE
#include "dcl.h"


#pragma warning(disable : 4244)
#pragma warning(disable : 4267)
#pragma warning(disable : 4996)

/*---------------------------------------------------- Внешние связи */

          extern double  dcl_errno ;       /* Системный указатель ошибок -> DCL_SLIB.CPP */

  char *GetMaskFiles(char *path) ;

/*********************************************************************/
/*                                                                   */
/*                Построение списка файлов по маске                  */

   int  Lang_DCL::zGetFilesByMask(Dcl_decl *source, char *mask)

{
 typedef struct {  char  decl[512] ;
		   char  name[512] ;
                    int  base_flag ;
                    int  copy_flag ;
                    int  names_flag ;
                   char *names ;
                    int  names_idx ; } Tree_decl ;

    Tree_decl *tree ;      /* Дерево пути */
          int  tree_cnt ;

         char  path[1024] ;
         char  name[1024] ;
  struct stat  file_attr ;
         char *work ;
         char *end ;
          int  new_branch ;     /* Флаг введения новой ветви */
          int  status ;
          int  i ;
          int  i_prv ;
          int  i_base ;
          int  j ;
          int  k ;
                           
/*------------------------------------------------------- Подготовка */

     for(i=0 ; mask[i] ; i++)  if(mask[i]=='\\')  mask[i]='/' ;     /* Замена разделителя ветвей */

                         dcl_errno=0 ;
                               
/*---------------------------------------- Инициализация дерева пути */

         tree=(Tree_decl *)calloc(512, sizeof(*tree)) ;		    /* Выделение памяти */

	           i_base=9999 ;

    for(work=mask, tree_cnt=0 ; ; work=end, tree_cnt++) {

         end=strchr(work+1, '/') ;                                  /* Ищем очередной разделитель */
      if(end!=NULL)  *end=0 ;

         strcpy(tree[tree_cnt].decl, work) ;                        /* Вырезаем ветвь дерева */
         strcpy(tree[tree_cnt].name, work) ;                        /* Вырезаем ветвь дерева */

      if(strchr(tree[tree_cnt].decl, '*') ||                        /* Определяем задана ли маска в ветви... */
         strchr(tree[tree_cnt].decl, '?')   ) {
             if(i_base==9999)  i_base=tree_cnt ;
                                 tree[tree_cnt].names_flag=1 ;
                                              }
      else                       tree[tree_cnt].names_flag=0 ;


				 tree[tree_cnt].base_flag =1 ;      /* Уст.флаг базовой ветви */

      if(end==NULL)  break;     				    /* Если все ветви пройдены */

                     *end='/' ;                                     /* Восстанавливаем разделитель */
                                                        }

			tree_cnt++ ;

/*-------------------------------------------- Проход по дереву пути */

                 k=0 ;
     
   for(i_prv=-1, i=0 ; i>=0 ; ) {				    /* CIRCLE.1 */

/*- - - - - - - - - - - - - - - - - -  Определение путей копирования */
                             strcpy(path, "") ;
      for(j=0 ; j<=i ; j++)  strcat(path, tree[j].name) ;
/*- - - - - - - - - - - - - - - - - - Создание списка объектов ветви */
      if(tree[i].names_flag && i>i_prv) {			    /* IF.1 Если приходим на ветвь-маску */
                                                                    /*        от КОРНЯ...                */
       if(tree[i].names!=NULL)  free(tree[i].names) ;               /* Освоб.предыдущий список объектов */
          tree[i].names=zGetMaskFiles(path) ;                       /* Получаем текущий список объектов */
       if(tree[i].names==NULL) {
                                  mError_code=_DCLE_MEMORY ;
                                    return(-1) ;
                               }

				  	   tree[i].names_idx=0 ;    /* Идем на первый объект ветви */
                                        }                           /* END.1 */
/*- - - - - - - - - - - - - - - - -  Проход по списку обьектов ветви */
      if(tree[i].names_flag)  {

	if(tree[i].names[tree[i].names_idx]==0) {                   /* Если все объекты ветви       */
                                                                    /*   просмотрены - идем к КОРНЮ */
                           strcpy(tree[i].name, tree[i].decl) ;

	 		       if(!tree[i].base_flag) tree_cnt-- ;  /* 'Обрубаем' пройденные небазовые ветви */

                                                     i_prv=i  ;
                                                           i-- ;

                                                       continue ;
	                                        }

        if(tree[i].decl[0]=='/')  strcpy(tree[i].name, "/") ;  
        else                      strcpy(tree[i].name, "") ;   

           strcat(tree[i].name, tree[i].names+tree[i].names_idx) ;  /* Выбираем очередной объект ветви */

		  tree[i].names_idx+=strlen(tree[i].names+          /* Переставляем указатель на следующий */
                                            tree[i].names_idx)+1 ;  /*  объект ветви                       */

                              }
/*- - - - - - - - - - - - - - - -  Переопределение путей копирования */
                            strcpy(path, "") ;
     for(j=0 ; j<=i ; j++)  strcat(path, tree[j].name) ;
/*- - - - - - - - - - - - - - - - Обработка движения вверх по дереву */
      if(tree[i].names_flag==0)  {    				    /* Если при движении вверх попадаем   */
                                                                    /*  на ветвь-НЕмаску - идем транзитом */
           if(i_prv>i) {
                            i_prv=i  ;
                                  i-- ;

	       if(!tree[i].base_flag) tree_cnt-- ;                  /* 'Обрубаем' пройденные небазовые ветви */

                              continue ;
	               }
                                 }
/*- - - - - - - - - - - - - - - - Регистрация файла/Создание раздела */
     if(i==tree_cnt-1) {
                             new_branch=0 ;

          status=stat(path, &file_attr) ;                           /* Получение аттрибутов файла */
// Как обработать отсутсвие или недоступность файла???

       if(status==0) 
        if(file_attr.st_mode & S_IFDIR) {                           /* Обработка раздела */
                                          new_branch=1 ;
                                               i_prv=i ;
                                            tree_cnt++ ;	    /*  Создаем новую ветвь */
                                                   i++ ;
 
                                       strcpy(tree[i].decl, "/*") ;
	                               strcpy(tree[i].name, "/*") ;
                                              tree[i].names_flag=1 ;
                                        }
        else                            {                           /* Регистрация файла */

          Dcl_decl  rec_data[2] ={
                                  {_CHR_AREA, 0, 0, 0, "path",            0, path, 512, 512},
                                  {_CHR_AREA, 0, 0, 0, "name",  (void *)512, name, 128, 128}
                                 } ;
  Dcl_complex_type  rec_template={ "file", 640, rec_data, 2} ;

                        strcpy(name, path) ;        
                   end=strrchr(name, ':' ) ; if(end!=NULL)  strcpy(name, end+1) ;
                   end=strrchr(name, '/' ) ; if(end!=NULL)  strcpy(name, end+1) ;
                   end=strrchr(name, '\\') ; if(end!=NULL)  strcpy(name, end+1) ;
                   end= strchr(name, '.' ) ; if(end!=NULL)   *end=0 ;

#ifdef UNIX
#else
                for(work=path ; *work ; work++)  if(*work=='/')  *work='\\' ;
#endif

                    rec_data[0].size=strlen(path) ;
                    rec_data[1].size=strlen(name) ;

                               status=iXobject_add(source, &rec_template) ;
                            if(status) break ;
                                        }

                            if(new_branch)  continue ;
    
                       }
/*- - - - - - - - - - - - Определение направления движения по дереву */
    if(tree[i].names_flag==0)  {  

              if(i==tree_cnt-1)  {  i_prv=i ;  i-- ;  continue ;  }

              if(i>i_prv      )  {  i_prv=i ;  i++ ;  continue ;  }
              else               {  i_prv=i ;  i-- ;  continue ;  }

                                    i_prv=i ;
                               }
    else                       {
             if(i<tree_cnt-1)    {  i_prv=i ;  i++ ;  continue ;  }

                                    i_prv=i ;
                               }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

                                }                                   /* CONTINUE.1 */
/*-------------------------------------------- Освобождение ресурсов */

      for(i=0 ; i<512 ; i++)
        if(tree[i].names!=NULL)  free(tree[i].names) ;

                                 free(tree) ;

/*-------------------------------------------------------------------*/

  return(0) ;
}


/*********************************************************************/
/*                                                                   */
/*                Построение списка файлов по маске                  */

   int  Lang_DCL::zGetFilesByFolder(Dcl_decl *source, char *mask)

{
         char *files ;
         char *file ;
         char  folder[1024] ;
         char  folder_sep ;
         char  path[1024] ;
         char  name[1024] ;
 struct _stat  file_info ;
unsigned long  file_size ;
unsigned long  file_ctime ;
         char  file_owner[256] ;

#ifdef UNIX
          int  status ;
#else
         PSID  file_PSID ;
         char  account[128] ;
         char  domain[128] ;
 SID_NAME_USE  account_type ;
        DWORD  status ;
        DWORD  a_size ;
        DWORD  d_size ;
#endif
         char *end ;

          Dcl_decl  rec_data[5] ={
                                  {_CHR_AREA,  0, 0, 0, "path",             0,          path,       512, 512},
                                  {_CHR_AREA,  0, 0, 0, "name",   (void *)512,          name,       128, 128},
                                  {_DCL_ULONG, 0, 0, 0, "size",   (void *)640, (char *)&file_size,    1,   1},
                                  {_CHR_AREA,  0, 0, 0, "owner",  (void *)644,          file_owner, 128, 128},
                                  {_DCL_ULONG, 0, 0, 0, "c_time", (void *)772, (char *)&file_ctime,   1,   1}
                                 } ;

  Dcl_complex_type  rec_template={ "file", 776, rec_data, 5} ;

/*------------------------------------------------------- Подготовка */

          if(strchr(mask, '\\')!=NULL)  folder_sep='\\' ;           /* Определяем разделитель пути */
          else                          folder_sep='/' ;

                                memset(folder,   0 , sizeof(folder)  ) ;
                               strncpy(folder, mask, sizeof(folder)-1) ;

                           end=strrchr(folder, folder_sep) ;     /* Выделение пути к папке */
            if(end==NULL)  end=strrchr(folder, ':') ;
            if(end!=NULL) *end=0 ;

                         dcl_errno=0 ;

/*------------------------------------------ Получение списка файлов */

          files=zGetMaskFiles(mask) ;                               /* Получаем текущий список объектов */
       if(files==NULL) {
                          mError_code=_DCLE_MEMORY ;
                             return(-1) ;
                       }
/*--------------------------------------------------- Перебор файлов */

             memset(name, 0, sizeof(name)) ;

   for(file=files ; *file!=0 ; file+=strlen(file)+1) {

                       file_size =0 ;
                       file_ctime=0 ;
                      *file_owner=0 ;

              sprintf(path, "%s%c%s", folder, folder_sep, file) ;
              strncpy(name, file, sizeof(name)-1) ;
/*- - - - - - - - - - - - - - - - - - - - - Извлекаем атрибуты файла */
         status=_stat(path, &file_info) ;
      if(status)  continue ;

                   file_size =file_info.st_size ;
                   file_ctime=file_info.st_mtime ;
/*- - - - - - - - - - - - - - - - - - - - Определяем владельца файла */
#ifdef UNIX
#else
              *file_owner=0 ;

     do {

          status=GetNamedSecurityInfo(path, SE_FILE_OBJECT, OWNER_SECURITY_INFORMATION, &file_PSID, NULL, NULL, NULL, NULL) ;
       if(status!=ERROR_SUCCESS)  break ;

                            a_size=sizeof(account)-1 ;
                            d_size=sizeof(domain)-1 ;

          status=LookupAccountSidA(NULL, file_PSID, account, &a_size, domain, &d_size, &account_type) ;
       if(status==0)  break ;

                 sprintf(file_owner, "%s/%s", domain, account) ;

        }  while(0) ;


#endif
/*- - - - - - - - - - - - - - - - - - - - - Добавляем запись о файле */
                    rec_data[0].size=strlen(path) ;
                    rec_data[1].size=strlen(name) ;
                    rec_data[3].size=strlen(file_owner) ;

               status=iXobject_add(source, &rec_template) ;
            if(status) break ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                                                     }
/*-------------------------------------------- Освобождение ресурсов */

               free(files) ;

/*-------------------------------------------------------------------*/

  return(0) ;
}


/*********************************************************************/
/*                                                                   */
/*               Получение списка объектов раздела                   */

  char *Lang_DCL::zGetMaskFiles(char *path)

{
#ifdef UNIX
                 DIR *dir_hdr ;
       struct dirent *file_info ;
#else
            intptr_t  dir_hdr ;
  struct _finddata_t  file_info ;
#endif
                char *names ;
                 int  names_size ;
                 int  status ;
                char  folder[1024] ;
                char  folder_sep ;
                char  mask[1024] ;
                 int  find_next ;
                char *end ;

/*---------------------------------------------------- Инициализация */

	names=(char *)calloc(1, 1024) ;
     if(names==NULL)  return(NULL) ;

        names_size=0 ;

     if(strchr(path, '\\')!=NULL)  folder_sep='\\' ;                /* Определяем разделитель пути */
     else                          folder_sep='/' ;

/*------------------------------------------------------- Подготовка */

                    strcpy(folder, path) ;		            /* Разборка полной маски           */
               end=strrchr(folder, folder_sep) ;                    /*   на путь раздела и маску файла */
            if(end!=NULL) {
                             strcpy(mask, end+1) ;
			        *end=0 ;
                          }
            else          {
                             strcpy(mask,   folder) ;
                             strcpy(folder, ".") ;
                          }
/*--------------------------------------------------- Анализ раздела */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - -  Для Solaris */
#ifdef UNIX
           dir_hdr=opendir(folder) ;
        if(dir_hdr==NULL)  return(names) ;

        do {
  		file_info=readdir(dir_hdr) ;
             if(file_info==NULL)  break ;

             if(file_info->d_name[0]=='.')  continue ;

             if(zTestMask(file_info->d_name, mask))  continue ;     /* Контроль на маску */

		names=(char *)
                       realloc(names, names_size+
                                      strlen(file_info->d_name)+8) ;

                         memset(names+names_size, 0, strlen(file_info->d_name)+8) ;
          if(*names==0)  strcpy(names,            file_info->d_name) ;
          else           strcpy(names+names_size, file_info->d_name) ;  

                           names_size+=strlen(file_info->d_name)+1 ;

           } while(1) ;

	                   closedir(dir_hdr) ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - Для MS Windows */
#else

           dir_hdr=_findfirst(path, &file_info) ;
        if(dir_hdr<0)  return(names) ;

                find_next=0 ;

        do {
             if(find_next) {
                     status=_findnext(dir_hdr, &file_info) ;
                  if(status)  break ; 
                           } 

                find_next++ ;

             if(file_info.name[0]=='.')  continue ;

             if(zTestMask(file_info.name, mask))  continue ;        /* Контроль на маску */

		names=(char *)
                       realloc(names, names_size+
                                      strlen(file_info.name)+8) ;

                         memset(names+names_size, 0, strlen(file_info.name)+8) ;
          if(*names==0)  strcpy(names,            file_info.name) ;
          else           strcpy(names+names_size, file_info.name) ;  

                           names_size+=strlen(file_info.name)+1 ;

           } while(1) ;

                         _findclose(dir_hdr) ;
#endif

/*-------------------------------------------------------------------*/

  return(names) ;
}


/*********************************************************************/
/*                                                                   */
/*                   Проверка фрагмента по маске                     */
/*                                                                   */
/*    Программа работает рекурсивно.                                 */

  int  Lang_DCL::zTestMask(char *text, char *mask)

{
/*--------------------------------------- Обработка начальной звезды */

   if(!strcmp(mask, "*"))  return(0) ;

   if(mask[0]=='*') {

      for(mask++ ; *text ; text++)  
          if(zTestMask(text, mask)==0)  return(0) ;
                                            
                         return(-1) ;
                    }
/*----------------------------------------------- Сравнение по маске */

    for( ; *mask ; text++, mask++) {

      if(*mask=='*') {
          if(zTestMask(text, mask)==0)  return( 0) ;
                                        return(-1) ;
                     } 

      if(*mask=='?')  continue ;

      if(*mask!=*text)  break ;   
                                   }
         
      if(*mask!=*text)  return(-1) ;

/*-------------------------------------------------------------------*/

    return( 0) ;
}
