
/*********************************************************************/
/*                                                                   */
/*                   DATA CONVERTION LANGUAGE                        */
/*                                                                   */
/*                 БИБЛИОТЕКА РАБОТЫ С SQL-ЗАПРОСАМИ                 */
/*                                                                   */
/*********************************************************************/

#if defined(AIX) || defined(LINUX)
#ifndef UNIX
#define  UNIX
#endif
#endif


#include <errno.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#ifdef UNIX
#include  <unistd.h>
#define   stricmp  strcasecmp
#define   O_BINARY    0
#else
#include  <io.h>
#include  <direct.h>
#endif

#include "sql_common.h"
#include "sql_direct.h"

#define   DCL_INSIDE
#include "dcl.h"


#pragma warning(disable : 4996)

/*---------------------------------------------------- Внешние связи */

      extern    double  dcl_errno ;               /* Системный указатель ошибок -> DCL_SLIB.CPP */

               SQL_link *dcl_sql_connect ;        /* Интерфейс SQL-запросов по-умолчанию */ 
       static       int  dcl_sql_inner ;          /* Флаг внутреннего выделения интерфейса по-умолчанию */
       static      char  dcl_sql_error[512] ;     /* Буфер ошибки для освобождаемых интерфейсов */
                   char  dcl_sql_output_ignore ;  /* Флаг игнорирования выходных параметров SQL */
       static      char  dcl_sql_cp_data[32] ;    /* Кодовая страница SQL-клиента */
       static      char  dcl_sql_cp_out[32] ;     /* Кодовая страница выходных данных */
       static       int  dcl_sql_cp_convert ;     /* Флаг необходимости выполнения преобразования кодовой страницы выходных данных */

/*------------------------------------------------ "Внешние" объекты */

    Dcl_decl *Dcl_sql_codepages   (Lang_DCL *, Dcl_decl **, int) ;      /* Задание преобразования кодовых страниц */
    Dcl_decl *Dcl_sql_connect     (Lang_DCL *, Dcl_decl **, int) ;      /* Выполнение операции CONNECT */
    Dcl_decl *Dcl_sql_disconnect  (Lang_DCL *, Dcl_decl **, int) ;      /* Выполнение операции DISCONNECT */
    Dcl_decl *Dcl_sql_alloc_cursor(Lang_DCL *, Dcl_decl **, int) ;      /* Выполнение операции ALLOC_CURSOR */
    Dcl_decl *Dcl_sql_commit      (Lang_DCL *, Dcl_decl **, int) ;      /* Выполнение операции COMMIT */
    Dcl_decl *Dcl_sql_rollback    (Lang_DCL *, Dcl_decl **, int) ;      /* Выполнение операции ROLLBACK */
    Dcl_decl *Dcl_sql_execute     (Lang_DCL *, Dcl_decl **, int) ;      /* Исполнение SQL-оператора */
    Dcl_decl *Dcl_sql_writeLOB    (Lang_DCL *, Dcl_decl **, int) ;      /* Занесение LOB-значения */
    Dcl_decl *Dcl_sql_fetch2csv   (Lang_DCL *, Dcl_decl **, int) ;      /* Выборка в CSV-файл */
    Dcl_decl *Dcl_sql_fetch2dbf   (Lang_DCL *, Dcl_decl **, int) ;      /* Выборка в DBF-файл */
    Dcl_decl *Dcl_sql_open        (Lang_DCL *, Dcl_decl **, int) ;      /* Открытие выборки */
    Dcl_decl *Dcl_sql_close       (Lang_DCL *, Dcl_decl **, int) ;      /* Закрытие выборки */
    Dcl_decl *Dcl_sql_error       (Lang_DCL *, Dcl_decl **, int) ;      /* Получение текста ошибки */

        void  Dcl_sql_final       (void) ;                              /* Процедура финального освобождения ресурсов */

      double  Dcl_sql_fetch       (Lang_DCL *, Dcl_decl *, int, int) ;  /* Выполнение SELECT-выборки */

/*------------------------------------------------ Эмуляция констант */

	 static double      E_CALL       = -1 ;        /* Эмуляция кодов ошибок */
	 static double      E_TYPE       = -2 ;
	 static double      E_SQL        = -3 ;
	 static double      E_HDC        = -4 ;
	 static double      E_CURSOR_MAX = -5 ;
	 static double      E_HCURSOR    = -6 ;
	 static double      E_UNK_IFACE  = -7 ;
	 static double      E_HDC_IN_USE = -8 ;
	 static double      E_FETCH_FILE = -9 ;
	 static double      E_NO_LOB     =-10 ;
	 static double      E_DBF_FILE   =-11 ;
	 static double      E_UNK_CP     =-12 ;

/*--------------------------------------------------- Таблица ссылок */

     Dcl_decl  dcl_sql_lib[]={

               {0,         0, 0, 0, "$PassiveData$",         NULL,          "qlib", 0, 0},
               {0, _DCL_CALL, 0, 0, "$Final$",       (void *)Dcl_sql_final,  NULL,  0, 0},

	       {_DGT_VAL, 0, 0, 0, "E_CALL",            &E_CALL,        NULL, 1, 1},
	       {_DGT_VAL, 0, 0, 0, "E_PAR_TYPE",        &E_TYPE,        NULL, 1, 1},
	       {_DGT_VAL, 0, 0, 0, "E_SQL_ERROR",       &E_SQL,         NULL, 1, 1},
	       {_DGT_VAL, 0, 0, 0, "E_CONNECT_HEADER",  &E_HDC,         NULL, 1, 1},
	       {_DGT_VAL, 0, 0, 0, "E_CURSOR_MAX",      &E_CURSOR_MAX,  NULL, 1, 1},
	       {_DGT_VAL, 0, 0, 0, "E_CURSOR_HEADER",   &E_HCURSOR,     NULL, 1, 1},
	       {_DGT_VAL, 0, 0, 0, "E_UNKNOWN_IFACE",   &E_UNK_IFACE,   NULL, 1, 1},
	       {_DGT_VAL, 0, 0, 0, "E_CONNECT_IN_USE",  &E_HDC_IN_USE,  NULL, 1, 1},
	       {_DGT_VAL, 0, 0, 0, "E_FETCH_FILE",      &E_FETCH_FILE,  NULL, 1, 1},
	       {_DGT_VAL, 0, 0, 0, "E_NO_LOB",          &E_NO_LOB,      NULL, 1, 1},
	       {_DGT_VAL, 0, 0, 0, "E_DBF_FILE",        &E_DBF_FILE,    NULL, 1, 1},
	       {_DGT_VAL, 0, 0, 0, "E_UNK_CP",          &E_UNK_CP,      NULL, 1, 1},

	       {_DGT_VAL, _DCL_CALL, 0, 0, "sql_codepages",    (void *)Dcl_sql_codepages,     "",  0, 0},
	       {_DGT_VAL, _DCL_CALL, 0, 0, "sql_connect",      (void *)Dcl_sql_connect,       "",  0, 0},
	       {_DGT_VAL, _DCL_CALL, 0, 0, "sql_disconnect",   (void *)Dcl_sql_disconnect,    "",  0, 0},
//             {_DGT_VAL, _DCL_CALL, 0, 0, "sql_alloc_cursor", (void *)Dcl_sql_alloc_cursor,  "",  0, 0},
	       {_DGT_VAL, _DCL_CALL, 0, 0, "sql_commit",       (void *)Dcl_sql_commit,        "",  0, 0},
	       {_DGT_VAL, _DCL_CALL, 0, 0, "sql_rollback",     (void *)Dcl_sql_rollback,      "",  0, 0},
	       {_DGT_VAL, _DCL_CALL, 0, 0, "sql_execute",      (void *)Dcl_sql_execute,       "",  0, 0},
	       {_DGT_VAL, _DCL_CALL, 0, 0, "sql_writeLOB",     (void *)Dcl_sql_writeLOB,      "",  0, 0},
	       {_DGT_VAL, _DCL_CALL, 0, 0, "sql_fetch2csv",    (void *)Dcl_sql_fetch2csv,     "",  0, 0},
	       {_DGT_VAL, _DCL_CALL, 0, 0, "sql_fetch2dbf",    (void *)Dcl_sql_fetch2dbf,     "",  0, 0},
               {_DGT_VAL, _DCL_CALL, 0, 0, "sql_open",         (void *)Dcl_sql_open,          "",  0, 0},
               {_DGT_VAL, _DCL_CALL, 0, 0, "sql_close",        (void *)Dcl_sql_close,         "",  0, 0},
               {_DGT_VAL, _DCL_CALL, 0, 0, "sql_error",        (void *)Dcl_sql_error,         "",  0, 0},

	       {0, 0, 0, 0, "", NULL, NULL, 0, 0}
                               } ;

/*------------------------------------------------ Системные объекты */

#define   _CURSORS_INI       2
#define   _CURSORS_MAX    1000

 static struct {
                   SQL_link *connect ;
                 SQL_cursor *cursor ;
               } dcl_sql_cursors[_CURSORS_MAX] ;                    /* Таблица рабочих курсоров */

/*------------------------------------------------ Рабочие процедуры */

  double  iDcl_sql_parform  (Dcl_decl *, SQL_parameter *) ;      /* Формирование SQL-параметра по DCL-параметру */
     int  iDcl_sql_getcursor(int, SQL_link **, SQL_cursor **) ;  /* Получение данных соединения и курсора по дескриптору курсора */


/*********************************************************************/
/*                                                                   */
/*            Процедура финального освобождения ресурсов             */

        void  Dcl_sql_final(void)
{

/*--------------------------------- Закрытие соединения по-умолчанию */

  if(dcl_sql_connect!=NULL &&
     dcl_sql_inner  ==  1    ) {

               dcl_sql_connect->FreeCursors() ;                     /* Освобождение курсоров */
               dcl_sql_connect->Disconnect() ;                      /* Отсоединение от БД */

        delete dcl_sql_connect ;                                    /* Освобождение ресурсов */
               dcl_sql_connect=NULL ;

                               }
/*-------------------------------------------------------------------*/

   return ;
}


/*********************************************************************/
/*                                                                   */
/*            Задание преобразования кодовых страниц                 */
/*                                                                   */
/*   int  sql_codepages(cp_sql, cp_out) ;                            */
/*                                                                   */
/*     char *cp_sql  -  кодовая страница SQL-клиента                 */
/*     char *cp_out  -  кодовая страница выходного результата        */
/*                                                                   */
/*    Возврашает 0 при успешном завершении или код ошибки:           */
/*       Е_CALL            -  некорректная структура вызова          */
/*       Е_UNK_CPL         -  неизвестная кодовая страница           */

  Dcl_decl *Dcl_sql_codepages(Lang_DCL *Kernel, Dcl_decl **pars, int  pars_cnt)

{
              char  *db_par ;
               int   db_size ;
               int   status ;
               int   type ;
               int   size ;
               int   i ;

   static   double  ret_value ;          /* Буфер числового значения */
   static Dcl_decl  dgt_return={_DGT_VAL, 0, 0, 0, "", &ret_value, NULL, 1, 1} ;

/*-------------------------------------------- Извлечение параметров */

  if(pars_cnt<2)  {          dcl_errno=E_CALL ;
                             ret_value=E_CALL ;
                     return(&dgt_return) ;       }

  for(i=0 ; ; i++ ) {

         if(i==0)  {  db_par=dcl_sql_cp_data ;   db_size=sizeof(dcl_sql_cp_data) ;  }
    else if(i==1)  {  db_par=dcl_sql_cp_out ;    db_size=sizeof(dcl_sql_cp_out ) ;  }
    else                  break ;

               memset(db_par, 0, db_size) ;

         if(i>=pars_cnt)  continue ;                                /* Если список переданных параметров закончился... */

    if(pars[i]->addr==NULL)  {          dcl_errno=E_CALL ;
                                        ret_value=E_CALL ;
                                return(&dgt_return) ;       }

       type=t_base(pars[i]->type) ;
    if(type!=_DCLT_CHR_AREA &&                                       /* Проверка типа операнда */
       type!=_DCLT_CHR_PTR    ) {        dcl_errno=E_CALL ;
                                         ret_value=E_CALL ;
                                 return(&dgt_return) ;       }

                            size=pars[i]->size ;                    /* Опр.допустимого размера данных */
         if(size>=db_size)  size=db_size-1 ;

          memcpy(db_par, pars[i]->addr, size) ;                     /* Извлекаем значение параметра */

                    }
/*------------------------------------- Проверка значений параметров */

                dcl_sql_cp_convert=0 ;

    if(stricmp(dcl_sql_cp_data, dcl_sql_cp_out))  {

       status=Kernel->zCodePageConvert("", 0, dcl_sql_cp_data, dcl_sql_cp_out) ;
    if(status) {
                                 dcl_errno=E_UNK_CP ; 
                                 ret_value=E_UNK_CP ;
                         return(&dgt_return) ;       
               }

                dcl_sql_cp_convert=1 ;
                                                   }
/*-------------------------------------------------------------------*/

          ret_value=0 ;
  return(&dgt_return) ;
}


/*********************************************************************/
/*                                                                   */
/*                Выполнение операции CONNECT                        */
/*                                                                   */
/*   int  sql_connect([hdc], db_type, SID, login[, par_1, par_2]) ;  */
/*                                                                   */
/*      int  hdc     -  дескриптор соединения                        */
/*     char *db_type -  тип используемых драйверов                   */
/*     char *SID     -  идентификатор базы                           */
/*     char *login   -  пользователь/пароль                          */
/*     char *par_1   -  параметр соединения 1                        */
/*     char *par_2   -  параметр соединения 2                        */
/*                                                                   */
/*    Возврашает 0 при успешном завершении или код ошибки:           */
/*       Е_CALL            -  некорректная структура вызова          */
/*       Е_CONNECT_HEADER  -  некорректный дескриптор соединения     */
/*       E_SQL_ERROR       -  ошибка исполнения SQL-оператора        */

  Dcl_decl *Dcl_sql_connect(Lang_DCL *Kernel, Dcl_decl **pars, int  pars_cnt)

{
          SQL_link **connect ;
              char   db_type[32] ;
              char   db_sid[32] ;
              char   db_login[128] ;
              char  *db_pass ;
              char   db_par_1[1024] ;
              char   db_par_2[1024] ;
              char  *db_par ;
               int   db_size ;
               int   hdc ;
               int   status ;
               int   inner_def_iface ;
               int   type ;
               int   size ;
               int   i ;

   static   double  ret_value ;          /* Буфер числового значения */
   static Dcl_decl  dgt_return={_DGT_VAL, 0, 0, 0, "", &ret_value, NULL, 1, 1} ;

/*------------------------------------------- Определение соединения */

                inner_def_iface=0 ;

  if(pars_cnt     <   1  ||
     pars[0]->addr==NULL   )  {          dcl_errno=E_CALL ;
                                         ret_value=E_CALL ;
                                 return(&dgt_return) ;       }

      type=t_base(pars[0]->type) ;                                  /* Если первый параметр имеет числовой тип - */
   if(type==_DGT_VAL) {                                             /*  - считаем его дискриптором соединения    */
                         hdc=(int)Kernel->iDgt_get(pars[0]->addr, pars[0]->type) ;
                     connect=  NULL ;
                         pars++ ;                                   /*  Проходим первый параметр */
                         pars_cnt-- ;

                      }
   else               {                                             /* Соединение по-умолчанию */
                                 connect=&dcl_sql_connect ;
                         inner_def_iface=  1 ;
                      }

   if(connect==NULL) {
                                dcl_errno=E_HDC ;
                                ret_value=E_HDC ;
                        return(&dgt_return) ;
                     }
/*------------------------- Проверка перекрытия активного соединения */

    if(*connect!=NULL) {
                                  dcl_errno=E_HDC_IN_USE ;
                                  ret_value=E_HDC_IN_USE ;
                          return(&dgt_return) ;
                       }
/*-------------------------------------------- Извлечение параметров */

  if(pars_cnt<3)  {          dcl_errno=E_CALL ;
                             ret_value=E_CALL ;
                     return(&dgt_return) ;       }

  for(i=0 ; ; i++ ) {

         if(i==0)  {  db_par=db_type ;   db_size=sizeof(db_type ) ;  }
    else if(i==1)  {  db_par=db_sid ;    db_size=sizeof(db_sid  ) ;  }
    else if(i==2)  {  db_par=db_login ;  db_size=sizeof(db_login) ;  }
    else if(i==3)  {  db_par=db_par_1 ;  db_size=sizeof(db_par_1) ;  }
    else if(i==4)  {  db_par=db_par_2 ;  db_size=sizeof(db_par_2) ;  }
    else                  break ;

               memset(db_par, 0, db_size) ;

         if(i>=pars_cnt)  continue ;                                /* Если список переданных параметров закончился... */

    if(pars[i]->addr==NULL)  {          dcl_errno=E_CALL ;
                                        ret_value=E_CALL ;
                                return(&dgt_return) ;       }

       type=t_base(pars[i]->type) ;
    if(type!=_DCLT_CHR_AREA &&                                       /* Проверка типа операнда */
       type!=_DCLT_CHR_PTR    ) {        dcl_errno=E_CALL ;
                                         ret_value=E_CALL ;
                                 return(&dgt_return) ;       }

                            size=pars[i]->size ;                    /* Опр.допустимого размера данных */
         if(size>=db_size)  size=db_size-1 ;

          memcpy(db_par, pars[i]->addr, size) ;                     /* Извлекаем значение параметра */

                    }
/*----------------------------------- Создание интерфейса по типу БД */

       *connect=(*connect)->IfaceByType(db_type) ; 
    if(*connect==NULL) {
                                 dcl_errno=E_UNK_IFACE ; 
                                 ret_value=E_UNK_IFACE ;
                         return(&dgt_return) ;       
                       }

                          dcl_sql_inner=inner_def_iface ;

/*-------------------------------------------------- Соединение с БД */

                         db_pass=strchr(db_login, '/') ;
    if(db_pass==NULL)    db_pass= "" ;
    else             {  *db_pass=0  ;
                         db_pass++ ;   }

       status=(*connect)->Connect(db_login, db_pass, db_sid,
                                           db_par_1, db_par_2 ) ;
    if(status) {
                                 dcl_errno=E_SQL ; 
                                 ret_value=E_SQL ;
                         return(&dgt_return) ;       
               }
/*------------------------------------- Задание атрибутов соединения */

       status=(*connect)->SetAutoCommit(0) ;                        /* Отключаем авто-коммит */
    if(status) {
                                 dcl_errno=E_SQL ; 
                                 ret_value=E_SQL ;
                         return(&dgt_return) ;       
               }
/*----------------------------------------------- Выделение курсоров */

       status=(*connect)->AllocCursors(_CURSORS_INI) ; 
    if(status) {
                                 dcl_errno=E_SQL ; 
                                 ret_value=E_SQL ;
                         return(&dgt_return) ;       
               }
/*-------------------------------------------------------------------*/

          ret_value=0 ;
  return(&dgt_return) ;
}


/*********************************************************************/
/*                                                                   */
/*                Выполнение операции DISCONNECT                     */
/*                                                                   */
/*          int  sql_disconnect([hdc]) ;                             */
/*                                                                   */
/*              int  hdc - дескриптор соединения                     */
/*                                                                   */
/*    Возврашает 0 при успешном завершении или код ошибки:           */
/*       Е_CONNECT_HEADER  -  некорректный дескриптор соединения     */
/*       E_SQL_ERROR       -  ошибка исполнения SQL-оператора        */

  Dcl_decl *Dcl_sql_disconnect(Lang_DCL *Kernel, Dcl_decl **pars, int  pars_cnt)

{
          SQL_link **connect ;
               int   hdc ;
               int   status ;  
               int   release_iface ;  

   static   double  ret_value ;          /* Буфер числового значения */
   static Dcl_decl  dgt_return={_DGT_VAL, 0, 0, 0, "", &ret_value, NULL, 1, 1} ;

/*------------------------------------------- Определение соединения */

               release_iface=0 ;

   if(pars_cnt     >=  1  &&
      pars[0]->addr!=NULL   ) {
                         hdc=(int)Kernel->iDgt_get(pars[0]->addr, pars[0]->type) ;
                     connect=  NULL ;
               release_iface=    1 ;
                              }
   else                       {
                                     connect=&dcl_sql_connect ;
            if(dcl_sql_inner)  release_iface=  1 ;
                              }      

   if(connect==NULL) {
                                dcl_errno=E_HDC ;
                                ret_value=E_HDC ;
                        return(&dgt_return) ;
                     }
/*---------------------------------- Проверка активности дескриптора */

    if(*connect==NULL) {
                                  ret_value=0 ;
                          return(&dgt_return) ;
                       }
/*-------------------------------------------- Освобождение курсоров */

              (*connect)->FreeCursors() ; 

/*----------------------------------------------- Отсоединение от БД */

       status=(*connect)->Disconnect() ;
    if(status)  ret_value=E_SQL ;
    else        ret_value=   0 ;

/*-------------------------------------------- Освобождение ресурсов */

   if(release_iface) {

             strncpy(dcl_sql_error, (*connect)->error_text,         /* Сохраняем текст ошибки */
                                       sizeof(dcl_sql_error)-1) ;

                           delete (*connect) ;
                                  (*connect)=NULL ;
                     }
/*-------------------------------------------------------------------*/

           dcl_errno=ret_value ;
  
   return(&dgt_return) ;
}


/*********************************************************************/
/*                                                                   */
/*                Выполнение операции COMMIT                         */
/*                                                                   */
/*          int  sql_commit([hdc]) ;                                 */
/*                                                                   */
/*              int  hdc - дескриптор соединения                     */
/*                                                                   */
/*    Возврашает 0 при успешном завершении или код ошибки:           */
/*       Е_CONNECT_HEADER  -  некорректный дескриптор соединения     */
/*       E_SQL_ERROR       -  ошибка исполнения SQL-оператора        */

  Dcl_decl *Dcl_sql_commit(Lang_DCL *Kernel, Dcl_decl **pars, int  pars_cnt)

{
          SQL_link *connect ;
               int  hdc ;
               int  status ;  

   static   double  ret_value ;          /* Буфер числового значения */
   static Dcl_decl  dgt_return={_DGT_VAL, 0, 0, 0, "", &ret_value, NULL, 1, 1} ;

/*------------------------------------------- Определение соединения */

   if(pars_cnt     >=  1  &&
      pars[0]->addr!=NULL   ) {
               hdc=(int)Kernel->iDgt_get(pars[0]->addr, pars[0]->type) ;
           connect=  NULL ;
                              }
   else                       {
                                   connect=dcl_sql_connect ;
                              }      

   if(connect==NULL) {
                                dcl_errno=E_HDC ;
                                ret_value=E_HDC ;
                        return(&dgt_return) ;
                     }
/*---------------------------------------------- Выполнение операции */

       status=connect->Commit() ;
    if(status)  ret_value=E_SQL ;
    else        ret_value=   0 ;

/*-------------------------------------------------------------------*/

       dcl_errno=ret_value ;

   return(&dgt_return) ;
}


/*********************************************************************/
/*                                                                   */
/*                Выполнение операции ROLLBACK                       */
/*                                                                   */
/*          int  sql_rollback([hdc]) ;                               */
/*                                                                   */
/*              int  hdc - дескриптор соединения                     */
/*                                                                   */
/*    Возврашает 0 при успешном завершении или код ошибки:           */
/*     Е_CONNECT_HEADER  -  некорректный дескриптор соединения       */
/*     E_SQL_ERROR       -  ошибка исполнения SQL-оператора          */

  Dcl_decl *Dcl_sql_rollback(Lang_DCL *Kernel, Dcl_decl **pars, int  pars_cnt)

{
          SQL_link *connect ;
               int  hdc ;
               int  status ;  

   static   double  ret_value ;          /* Буфер числового значения */
   static Dcl_decl  dgt_return={_DGT_VAL, 0, 0, 0, "", &ret_value, NULL, 1, 1} ;

/*------------------------------------------- Определение соединения */

   if(pars_cnt     >=  1  &&
      pars[0]->addr!=NULL   ) {
               hdc=(int)Kernel->iDgt_get(pars[0]->addr, pars[0]->type) ;
           connect=  NULL ;
                              }
   else                       {
                                   connect=dcl_sql_connect ;
                              }      

   if(connect==NULL) {
                                 dcl_errno=E_HDC ;
                                 ret_value=E_HDC ;
                         return(&dgt_return) ;
                     }
/*---------------------------------------------- Выполнение операции */

       status=connect->Rollback() ;
    if(status)  ret_value=E_SQL ;
    else        ret_value=   0 ;

/*-------------------------------------------------------------------*/

           dcl_errno=ret_value ;

   return(&dgt_return) ;
}


/*********************************************************************/
/*                                                                   */
/*       Выполнение модифицирующего SQL-оператора или SQL-блока      */
/*                                                                   */
/*        int  sql_execute([hdc], sql[, value1, ..., valueN]) ;      */
/*                                                                   */
/*            int  hdc    - дескриптор соединения                    */
/*           char *sql    - SQL-оператор                             */
/*              ?  value? - Подставляемая переменная                 */
/*                                                                   */
/*  Возврашает 0 при успешном завершении или код ошибки:             */
/*     Е_CALL            -  некорректная структура вызова            */
/*     Е_PAR_TYPE        -  неподдерживаемый тип переменной          */
/*     Е_CONNECT_HEADER  -  некорректный дескриптор соединения       */
/*     Е_CURSOR_MAX      -  нет свободных курсоров                   */
/*     E_SQL_ERROR       -  ошибка исполнения SQL-оператора          */

  Dcl_decl *Dcl_sql_execute(Lang_DCL *Kernel, Dcl_decl **pars, int  pars_cnt)

{
        SQL_cursor *cursor ;
          SQL_link *connect ;
               int  hdc ;
              char *sql ;                /* SQL-выражение */
     SQL_parameter  sql_pars[200] ;      /* Список SQL-параметров */
               int  sql_pars_cnt ;
          Dcl_decl *par ;
               int  type ;
               int  status ;  
               int  size ;
               int  i ;

   static   double  ret_value ;          /* Буфер числового значения */
   static Dcl_decl  dgt_return={_DGT_VAL, 0, 0, 0, "", &ret_value, NULL, 1, 1} ;

/*------------------------------------------- Определение соединения */

  if(pars_cnt     <   1  ||
     pars[0]->addr==NULL   )  {          dcl_errno=E_CALL ;
                                         ret_value=E_CALL ;
                                 return(&dgt_return) ;       }

      type=t_base(pars[0]->type) ;                                  /* Если первый параметр имеет числовой тип - */
   if(type==_DGT_VAL) {                                             /*  - считаем его дискриптором соединения    */
                         hdc=(int)Kernel->iDgt_get(pars[0]->addr, pars[0]->type) ;
                     connect=  NULL ;

                         pars++ ;                                   /*  Проходим первый параметр */
                         pars_cnt-- ;

                      }
   else               {                                             /* Соединение по-умолчанию */
                         connect=dcl_sql_connect ;
                      }      

   if(connect==NULL) {
                                dcl_errno=E_HDC ;
                                ret_value=E_HDC ;
                        return(&dgt_return) ;
                     }

              connect->error_text[0]=0 ;                            /* Сброс ошибки */

/*----------------------------------------- Извлечение SQL-оператора */

  if(pars_cnt     <   1  ||
     pars[0]->addr==NULL   )  {          dcl_errno=E_CALL ;
                                         ret_value=E_CALL ;
                                 return(&dgt_return) ;       }

      type=t_base(pars[0]->type) ;
   if(type!=_DCLT_CHR_AREA &&                                       /* Проверка типа операнда */
      type!=_DCLT_CHR_PTR    ) {         dcl_errno=E_CALL ;
                                         ret_value=E_CALL ;
                                 return(&dgt_return) ;       }


                  sql=(char *)calloc(1, pars[0]->size+8) ;          /* Выделяем память по pars[0]->size */
          memcpy( sql, pars[0]->addr, pars[0]->size) ;              /* Извлекаем SQL-оператор */

/*-------------------------------- Извлечение параметров подстановки */

   for(sql_pars_cnt=0, i=1 ; i<pars_cnt ; i++) {

        type= t_base(pars[i]->type) ;                               /* Извлекаем базовый тип */
/*- - - - - - - - - - - - - - - - - - - - - Объект комплексного типа */
     if(type==_XTP_OBJ) {
                        }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - Простой объект */
     else               {

              ret_value=iDcl_sql_parform(pars[i], &sql_pars[sql_pars_cnt])  ;
           if(ret_value<0) {
                                     dcl_errno=ret_value ;
                             return(&dgt_return) ;
                           }

                                       sql_pars_cnt++ ;
                        }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                                               } 

/*---------------------------------------------- Выполнение операции */

       cursor=connect->LockCursor("Dcl_sql_execute") ;              /* Захватываем курсор */
    if(cursor==NULL) {          dcl_errno=E_CURSOR_MAX ;
                                ret_value=E_CURSOR_MAX ;
                        return(&dgt_return) ;             }

       status=connect->SqlExecute(cursor, sql,                      /* Исполняем SQL-оператор */
                                          sql_pars, sql_pars_cnt) ;
    if(status) {                                                    /* Если ошибка - фиксируем на контексте соединения */
                         ret_value=E_SQL ;
                 strncpy(connect->error_text, 
                          cursor->error_text, sizeof(connect->error_text)-1) ;  
               }
    else                 ret_value=  0. ;

              connect->UnlockCursor(cursor) ;                       /* Освобождаем курсор */

/*----------------------------------- Извлечение выходных параметров */

  if(!dcl_sql_output_ignore)                                        /* Если не игнорируем выьодные параметры... */
   for(i=0 ; i<sql_pars_cnt ; i++) {

        par=(Dcl_decl *)sql_pars[i].ref ;
     if(par==NULL)  continue ;

        type= t_base(par->type) ;                                   /* Извлекаем базовый тип */
/*- - - - - - - - - - - - - - - - - - - - - Объект комплексного типа */
     if(type==_XTP_OBJ) {
                        }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - Простой объект */
     else               {
   
                            size=strlen(sql_pars[i].buff) ;
        if(par->buff<size)  size=par->buff ;

                        memset(par->addr, 0, par->buff) ;
                        memcpy(par->addr, sql_pars[i].buff, size) ;
                               par->size=size ;
         Kernel->iSize_correct(par) ;                               /* Проводим следящую коррекцию размеров указателей на обьект */

                        }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                                   }
/*-------------------------------------------- Освобождение ресурсов */

                                        free(sql) ;

   for(i=0 ; i<sql_pars_cnt ; i++) {
                                        free(sql_pars[i].name) ;
                                        free(sql_pars[i].buff) ;
                                   } 
/*-------------------------------------------------------------------*/

           dcl_errno=ret_value ;

   return(&dgt_return) ;
}


/*********************************************************************/
/*                                                                   */
/*                   Занесение LOB-значения                          */
/*                                                                   */
/*        int  sql_writeLOB([hdc], sql[, value1, ..., valueN]) ;     */
/*                                                                   */
/*            int  hdc    - дескриптор соединения                    */
/*           char *sql    - SQL-оператор                             */
/*              ?  value? - Подставляемая переменная                 */
/*                                                                   */
/*  Возврашает 0 при успешном завершении или код ошибки:             */
/*     Е_CALL            -  некорректная структура вызова            */
/*     Е_PAR_TYPE        -  неподдерживаемый тип переменной          */
/*     Е_CONNECT_HEADER  -  некорректный дескриптор соединения       */
/*     Е_CURSOR_MAX      -  нет свободных курсоров                   */
/*     E_SQL_ERROR       -  ошибка исполнения SQL-оператора          */
/*     E_NO_LOB          -  среди параметров нет LOB-объектов        */

  Dcl_decl *Dcl_sql_writeLOB(Lang_DCL *Kernel, Dcl_decl **pars, int  pars_cnt)

{
        SQL_cursor *cursor ;
          SQL_link *connect ;
               int  hdc ;
              char *sql ;                /* SQL-выражение */
     SQL_parameter  sql_pars[200] ;      /* Список SQL-параметров */
               int  sql_pars_cnt ;
               int  type ;
               int  blobs ;
               int  status ;  
               int  i ;

   static   double  ret_value ;          /* Буфер числового значения */
   static Dcl_decl  dgt_return={_DGT_VAL, 0, 0, 0, "", &ret_value, NULL, 1, 1} ;

/*------------------------------------------- Определение соединения */

  if(pars_cnt     <   1  ||
     pars[0]->addr==NULL   )  {          dcl_errno=E_CALL ;
                                         ret_value=E_CALL ;
                                 return(&dgt_return) ;       }

      type=t_base(pars[0]->type) ;                                  /* Если первый параметр имеет числовой тип - */
   if(type==_DGT_VAL) {                                             /*  - считаем его дискриптором соединения    */
                         hdc=(int)Kernel->iDgt_get(pars[0]->addr, pars[0]->type) ;
                     connect=  NULL ;

                         pars++ ;                                   /*  Проходим первый параметр */
                         pars_cnt-- ;

                      }
   else               {                                             /* Соединение по-умолчанию */
                         connect=dcl_sql_connect ;
                      }      

   if(connect==NULL) {
                                dcl_errno=E_HDC ;
                                ret_value=E_HDC ;
                        return(&dgt_return) ;
                     }

              connect->error_text[0]=0 ;                            /* Сброс ошибки */

/*----------------------------------------- Извлечение SQL-оператора */

  if(pars_cnt     <   1  ||
     pars[0]->addr==NULL   )  {          dcl_errno=E_CALL ;
                                         ret_value=E_CALL ;
                                 return(&dgt_return) ;       }

      type=t_base(pars[0]->type) ;
   if(type!=_DCLT_CHR_AREA &&                                       /* Проверка типа операнда */
      type!=_DCLT_CHR_PTR    ) {         dcl_errno=E_CALL ;
                                         ret_value=E_CALL ;
                                 return(&dgt_return) ;       }


                  sql=(char *)calloc(1, pars[0]->size+8) ;          /* Выделяем память по pars[0]->size */
          memcpy( sql, pars[0]->addr, pars[0]->size) ;              /* Извлекаем SQL-оператор */

/*-------------------------------- Извлечение параметров подстановки */

   for(sql_pars_cnt=0, blobs=0, i=1 ; i<pars_cnt ; i++) {

        type= t_base(pars[i]->type) ;                               /* Извлекаем базовый тип */
/*- - - - - - - - - - - - - - - - - - - - - Объект комплексного типа */
     if(type==_XTP_OBJ) {
                        }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - Простой объект */
     else               {

              ret_value=iDcl_sql_parform(pars[i], &sql_pars[sql_pars_cnt])  ;
           if(ret_value<0) {
                                     dcl_errno=ret_value ;
                             return(&dgt_return) ;
                           }

                       blobs+=sql_pars[sql_pars_cnt].blob ;  
                                       sql_pars_cnt++ ;
                        }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                                                        } 

    if(blobs==0) {         dcl_errno=E_NO_LOB ;
                           ret_value=E_NO_LOB ;
                   return(&dgt_return) ;         }

/*---------------------------------------------- Выполнение операции */

       cursor=connect->LockCursor("Dcl_sql_writeLOB") ;             /* Захватываем курсор */
    if(cursor==NULL) {          dcl_errno=E_CURSOR_MAX ;
                                ret_value=E_CURSOR_MAX ;
                        return(&dgt_return) ;             }

       status=connect->WriteLOB(cursor, sql,                        /* Исполняем SQL-оператор */
                                        sql_pars, sql_pars_cnt) ;
    if(status) {                                                    /* Если ошибка - фиксируем на контексте соединения */
                         ret_value=E_SQL ;
                 strncpy(connect->error_text, 
                          cursor->error_text, sizeof(connect->error_text)-1) ;  
               }
    else                 ret_value=  0. ;

              connect->UnlockCursor(cursor) ;                       /* Освобождаем курсор */

/*-------------------------------------------- Освобождение ресурсов */

                                        free(sql) ;

   for(i=0 ; i<sql_pars_cnt ; i++) {
                                        free(sql_pars[i].name) ;
                                        free(sql_pars[i].buff) ;
                                   }
/*-------------------------------------------------------------------*/

           dcl_errno=ret_value ;

   return(&dgt_return) ;
}


/*********************************************************************/
/*                                                                   */
/*       Выборка по SELECT-запросу в CSV-файл                        */
/*                                                                   */
/*      int  sql_fetch2csv([hdc], path, sep,                         */
/*                                 sql[, value1, ..., valueN]) ;     */
/*                                                                   */
/*         int  hdc    - дескриптор соединения                       */
/*        char *path   - путь к файлу результата                     */
/*        char *sep    - разделитель полей                           */
/*        char *sql    - SQL-оператор                                */
/*           ?  value? - Подставляемая переменная                    */
/*                                                                   */
/*  Возврашает число выбранных строк данных или -1 при ошибке        */
/*   или код ошибки:                                                 */
/*     Е_CALL            -  некорректная структура вызова            */
/*     Е_PAR_TYPE        -  неподдерживаемый тип переменной          */
/*     Е_CONNECT_HEADER  -  некорректный дескриптор соединения       */
/*     Е_CURSOR_MAX      -  нет свободных курсоров                   */
/*     E_SQL_ERROR       -  ошибка исполнения SQL-оператора          */
/*     E_FETCH_FILE      -  ошибка при работе с файлом               */

  Dcl_decl *Dcl_sql_fetch2csv(Lang_DCL *Kernel, Dcl_decl **pars, int  pars_cnt)

{
        SQL_cursor *cursor ;
          SQL_link *connect ;
               int  hdc ;
              char  path[FILENAME_MAX] ;  /* Путь к файлу результата */ 
              char  sep[32] ;             /* Разделитель */ 
              char *sql ;                 /* SQL-выражение */
     SQL_parameter  sql_pars[200] ;       /* Список SQL-параметров */
               int  sql_pars_cnt ;
          Dcl_decl *par ;
               int  type ;
              FILE *file ;
               int  status ;
              char  row_cut[16] ;         /* Завершение строки */
            size_t  cnt ;
               int  size ;
               int  n ;
               int  i ;
               int  j ;

   static   double  ret_value ;          /* Буфер числового значения */
   static Dcl_decl  dgt_return={_DGT_VAL, 0, 0, 0, "", &ret_value, NULL, 1, 1} ;

/*---------------------------------------------------- Инициализация */

                       ret_value=0 ;
                               n=0 ;

/*------------------------------------------- Определение соединения */

  if(pars_cnt     <   1  ||
     pars[0]->addr==NULL   )  {          dcl_errno=E_CALL ; 
                                         ret_value=E_CALL ;
                                 return(&dgt_return) ;       }

      type=t_base(pars[0]->type) ;                                  /* Если первый параметр имеет числовой тип - */
   if(type==_DGT_VAL) {                                             /*  - считаем его дискриптором соединения    */
                         hdc=(int)Kernel->iDgt_get(pars[0]->addr, pars[0]->type) ;
                     connect=  NULL ;

                         pars++ ;                                   /*  Проходим первый параметр */
                         pars_cnt-- ;

                      }
   else               {                                             /* Соединение по-умолчанию */
                         connect=dcl_sql_connect ;
                      }      

   if(connect==NULL) {
                                dcl_errno=E_HDC ;
                                ret_value=E_HDC ;
                        return(&dgt_return) ;
                     }

              connect->error_text[0]=0 ;                            /* Сброс ошибки */

/*------------------------------- Извлечение пути к файлу результата */

  if(pars_cnt     <   1  ||
     pars[0]->addr==NULL   )  {          dcl_errno=E_CALL ;
                                         ret_value=E_CALL ;
                                 return(&dgt_return) ;       }

      type=t_base(pars[0]->type) ;
   if(type!=_DCLT_CHR_AREA &&                                       /* Проверка типа операнда */
      type!=_DCLT_CHR_PTR    ) {         dcl_errno=E_CALL ;
                                         ret_value=E_CALL ;
                                 return(&dgt_return) ;       }

    if(pars[0]->size>=sizeof(path))  pars[0]->size=sizeof(path)-1 ; /* Контроль размеров */

	  memset(path, 0, sizeof(path)) ;
	  memcpy(path, pars[0]->addr, pars[0]->size) ;              /* Занесение пути в рабочий буфер */

                         pars++ ;                                   /* Проходим параметр */
                         pars_cnt-- ;

/*------------------------------------- Извлечение разделителя полей */

  if(pars_cnt     <   1  ||
     pars[0]->addr==NULL   )  {          dcl_errno=E_CALL ;
                                         ret_value=E_CALL ;
                                 return(&dgt_return) ;       }

      type=t_base(pars[0]->type) ;
   if(type!=_DCLT_CHR_AREA &&                                       /* Проверка типа операнда */
      type!=_DCLT_CHR_PTR    ) {         dcl_errno=E_CALL ;
                                         ret_value=E_CALL ;
                                 return(&dgt_return) ;       }

    if(pars[0]->size>=sizeof(sep))  pars[0]->size=sizeof(sep)-1 ;   /* Контроль размеров */

	  memset(sep, 0, sizeof(sep)) ;
	  memcpy(sep, pars[0]->addr, pars[0]->size) ;               /* Занесение разделителя в рабочий буфер */

                         pars++ ;                                   /* Проходим параметр */
                         pars_cnt-- ;
/*- - - - - - - - - - - - - - - - - - - - Разборка разделителя строк */
                        strcpy(row_cut, "\r\n") ;

        if(!memicmp(sep, "UNIX:", 5)) {                             /* Если разделитель строк для UNIX */
                                          strcpy(row_cut, "\n") ;
                                          strcpy(sep, sep+5) ;
                                      }
/*----------------------------------------- Извлечение SQL-оператора */

  if(pars_cnt     <   1  ||
     pars[0]->addr==NULL   )  {          dcl_errno=E_CALL ;
                                         ret_value=E_CALL ;
                                 return(&dgt_return) ;       }

      type=t_base(pars[0]->type) ;
   if(type!=_DCLT_CHR_AREA &&                                       /* Проверка типа операнда */
      type!=_DCLT_CHR_PTR    ) {         dcl_errno=E_CALL ;
                                         ret_value=E_CALL ;
                                 return(&dgt_return) ;       }

                  sql=(char *)calloc(1, pars[0]->size+8) ;          /* Выделяем память по pars[0]->size */
          memcpy( sql, pars[0]->addr, pars[0]->size) ;              /* Извлекаем SQL-оператор */

/*-------------------------------- Извлечение параметров подстановки */

   for(sql_pars_cnt=0, i=1 ; i<pars_cnt ; i++) {

        type= t_base(pars[i]->type) ;                               /* Извлекаем базовый тип */
/*- - - - - - - - - - - - - - - - - - - - - Объект комплексного типа */
     if(type==_XTP_OBJ) {
                        }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - Простой объект */
     else               {

              ret_value=iDcl_sql_parform(pars[i], &sql_pars[sql_pars_cnt])  ;
           if(ret_value<0) {
                                     dcl_errno=ret_value ;
                             return(&dgt_return) ;
                           }

                                       sql_pars_cnt++ ;
                        }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                                               } 
/*---------------------------------------------- Выполнение операции */

#define  NAME   (char *)cursor->columns[j].name
#define  VALUE  (char *)cursor->columns[j].value

   do {                                                             /* BLOCK.1 */

       cursor=connect->LockCursor("Dcl_sql_fetch2csv") ;            /* Захватываем курсор */
    if(cursor==NULL) {  ret_value=E_CURSOR_MAX ;
                               break ;           }

       status=connect->SelectOpen(cursor, sql,                      /* Открываем выборку */
                                          sql_pars, sql_pars_cnt) ;
    if(status) {  ret_value=E_SQL ;
                        break ;      }

       file=fopen(path, "wb") ;                                     /* Открываем файл результата */
    if(file==NULL) {  ret_value=E_FETCH_FILE ;  
                              break ;           }

    for(n=0 ; ; n++) {                                              /* LOOP - Сканируем выборку */ 

           status=connect->SelectFetch(cursor) ;                    /* Извлекаем очередную строку */
        if(status==_SQL_NO_DATA)   break ;                          /* Если все данные извлечены... */
        if(status) {                                                /* Если ошибка выборки */
                     ret_value=E_SQL ;
                      break ; 
                   }

      for(j=0 ; j<cursor->columns_cnt ; j++) {                      /* Пишем в файл данные полей */
        if(         j   >0 &&
           strlen(sep)  >0   ) {
                         cnt=fwrite(sep, 1, strlen(sep), file) ;
                      if(cnt!=strlen(sep)) {  ret_value=E_FETCH_FILE ;
                                                     break ;            }
                               }

        if(strlen(VALUE)>0   ) {  
                         cnt=fwrite(VALUE, 1, strlen(VALUE), file) ;
                      if(cnt!=strlen(VALUE)) {  ret_value=E_FETCH_FILE ;
                                                     break ;              }
                               }
                                             }

                         cnt=fwrite(row_cut, 1, strlen(row_cut), file) ;
                      if(cnt!=strlen(row_cut)) {  ret_value=E_FETCH_FILE ;
                                                      break ;              }

                                 if(ret_value)  break ;
                     }                                              /* ENDLOOP - Сканируем выборку */ 

       status=fclose(file) ;                                        /* Закрываем файл результата */
    if(status) {  ret_value=E_FETCH_FILE ;  
                          break ;           }

      } while(0) ;                                                  /* BLOCK.1 */

#undef  NAME
#undef  VALUE

    if(ret_value==E_SQL)                                            /* Если ошибка - фиксируем на контексте соединения */
             strncpy(connect->error_text, 
                      cursor->error_text, sizeof(connect->error_text)-1) ;  

/*----------------------------------- Извлечение выходных параметров */

   for(i=0 ; i<sql_pars_cnt ; i++) {

        par=(Dcl_decl *)sql_pars[i].ref ;
     if(par==NULL)  continue ;

        type= t_base(par->type) ;                                   /* Извлекаем базовый тип */
/*- - - - - - - - - - - - - - - - - - - - - Объект комплексного типа */
     if(type==_XTP_OBJ) {
                        }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - Простой объект */
     else               {
   
                            size=strlen(sql_pars[i].buff) ;
        if(par->buff<size)  size=par->buff ;

                        memset(par->addr, 0, par->buff) ;
                        memcpy(par->addr, sql_pars[i].buff, size) ;
                               par->size=size ;
         Kernel->iSize_correct(par) ;                               /* Проводим следящую коррекцию размеров указателей на обьект */

                        }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                                   }
/*-------------------------------------------- Освобождение ресурсов */

   if(cursor!=NULL)  connect->UnlockCursor(cursor) ;

                                        free(sql) ;

   for(i=0 ; i<sql_pars_cnt ; i++) {
                                        free(sql_pars[i].name) ;
                                        free(sql_pars[i].buff) ;
                                   } 
/*-------------------------------------------------------------------*/

                       dcl_errno=ret_value ;

     if(ret_value==0)  ret_value=n ;

   return(&dgt_return) ;
}


/*********************************************************************/
/*                                                                   */
/*       Выборка по SELECT-запросу в DBF-файл                        */
/*                                                                   */
/*      int  sql_fetch2dbf([hdc], path, dbf_tmpl,                    */
/*                                 sql[, value1, ..., valueN]) ;     */
/*                                                                   */
/*         int  hdc      - дескриптор соединения                     */
/*        char *path     - путь к файлу результата                   */
/*        char *dbf_tmpl - путь к файлу DBF-шаблона                  */
/*        char *sql      - SQL-оператор                              */
/*           ?  value?   - Подставляемая переменная                  */
/*                                                                   */
/*  Возврашает число выбранных строк данных или -1 при ошибке        */
/*   или код ошибки:                                                 */
/*     Е_CALL            -  некорректная структура вызова            */
/*     Е_PAR_TYPE        -  неподдерживаемый тип переменной          */
/*     Е_CONNECT_HEADER  -  некорректный дескриптор соединения       */
/*     Е_CURSOR_MAX      -  нет свободных курсоров                   */
/*     E_SQL_ERROR       -  ошибка исполнения SQL-оператора          */
/*     E_DBF_FILE        -  ошибка переноса DBF-заголовка            */
/*     E_FETCH_FILE      -  ошибка при работе с файлом результата    */

  Dcl_decl *Dcl_sql_fetch2dbf(Lang_DCL *Kernel, Dcl_decl **pars, int  pars_cnt)

{
    SQL_direct_DBF  Table ;
        SQL_cursor *cursor ;
          SQL_link *connect ;
               int  hdc ;
              char  path[FILENAME_MAX] ;  /* Путь к файлу результата */ 
              char  tmpl[FILENAME_MAX] ;  /* Путь к файлу dbf-заголовка */ 
              char *sql ;                 /* SQL-выражение */
     SQL_parameter  sql_pars[200] ;       /* Список SQL-параметров */
               int  sql_pars_cnt ;
               int  map[1024] ;           /* Таблица меппинга колонок SQL->DBF */
              char  dummy[1024] ;         /* Заглушка пустыx полей */
          Dcl_decl *par ;
               int  type ;
               int  status ;
               int  size ;
              char *value ; 
              char *end ; 
              char  tmp[1024] ;
               int  n ;
               int  i ;
               int  j ;

   static   double  ret_value ;          /* Буфер числового значения */
   static Dcl_decl  dgt_return={_DGT_VAL, 0, 0, 0, "", &ret_value, NULL, 1, 1} ;

/*---------------------------------------------------- Инициализация */

           memset(dummy, 0, sizeof(dummy)) ;

                       ret_value=0 ;
                               n=0 ;

/*------------------------------------------- Определение соединения */

  if(pars_cnt     <   1  ||
     pars[0]->addr==NULL   )  {          dcl_errno=E_CALL ; 
                                         ret_value=E_CALL ;
                                 return(&dgt_return) ;       }

      type=t_base(pars[0]->type) ;                                  /* Если первый параметр имеет числовой тип - */
   if(type==_DGT_VAL) {                                             /*  - считаем его дискриптором соединения    */
                         hdc=(int)Kernel->iDgt_get(pars[0]->addr, pars[0]->type) ;
                     connect=  NULL ;

                         pars++ ;                                   /*  Проходим первый параметр */
                         pars_cnt-- ;

                      }
   else               {                                             /* Соединение по-умолчанию */
                         connect=dcl_sql_connect ;
                      }      

   if(connect==NULL) {
                                dcl_errno=E_HDC ;
                                ret_value=E_HDC ;
                        return(&dgt_return) ;
                     }

              connect->error_text[0]=0 ;                            /* Сброс ошибки */

/*------------------------------- Извлечение пути к файлу результата */

  if(pars_cnt     <   1  ||
     pars[0]->addr==NULL   )  {          dcl_errno=E_CALL ;
                                         ret_value=E_CALL ;
                                 return(&dgt_return) ;       }

      type=t_base(pars[0]->type) ;
   if(type!=_DCLT_CHR_AREA &&                                       /* Проверка типа операнда */
      type!=_DCLT_CHR_PTR    ) {         dcl_errno=E_CALL ;
                                         ret_value=E_CALL ;
                                 return(&dgt_return) ;       }

    if(pars[0]->size>=sizeof(path))  pars[0]->size=sizeof(path)-1 ; /* Контроль размеров */

	  memset(path, 0, sizeof(path)) ;
	  memcpy(path, pars[0]->addr, pars[0]->size) ;              /* Занесение пути в рабочий буфер */

                         pars++ ;                                   /* Проходим параметр */
                         pars_cnt-- ;

/*------------------------------ Извлечение пути к файлу DBF-шаблона */

  if(pars_cnt     <   1  ||
     pars[0]->addr==NULL   )  {          dcl_errno=E_CALL ;
                                         ret_value=E_CALL ;
                                 return(&dgt_return) ;       }

      type=t_base(pars[0]->type) ;
   if(type!=_DCLT_CHR_AREA &&                                       /* Проверка типа операнда */
      type!=_DCLT_CHR_PTR    ) {         dcl_errno=E_CALL ;
                                         ret_value=E_CALL ;
                                 return(&dgt_return) ;       }

    if(pars[0]->size>=sizeof(tmpl))  pars[0]->size=sizeof(tmpl)-1 ; /* Контроль размеров */

	  memset(tmpl, 0, sizeof(tmpl)) ;
	  memcpy(tmpl, pars[0]->addr, pars[0]->size) ;              /* Занесение пути в рабочий буфер */

                         pars++ ;                                   /* Проходим параметр */
                         pars_cnt-- ;

/*----------------------------------------- Извлечение SQL-оператора */

  if(pars_cnt     <   1  ||
     pars[0]->addr==NULL   )  {          dcl_errno=E_CALL ;
                                         ret_value=E_CALL ;
                                 return(&dgt_return) ;       }

      type=t_base(pars[0]->type) ;
   if(type!=_DCLT_CHR_AREA &&                                       /* Проверка типа операнда */
      type!=_DCLT_CHR_PTR    ) {         dcl_errno=E_CALL ;
                                         ret_value=E_CALL ;
                                 return(&dgt_return) ;       }

                  sql=(char *)calloc(1, pars[0]->size+8) ;          /* Выделяем память по pars[0]->size */
          memcpy( sql, pars[0]->addr, pars[0]->size) ;              /* Извлекаем SQL-оператор */

/*-------------------------------- Извлечение параметров подстановки */

   for(sql_pars_cnt=0, i=1 ; i<pars_cnt ; i++) {

        type= t_base(pars[i]->type) ;                               /* Извлекаем базовый тип */
/*- - - - - - - - - - - - - - - - - - - - - Объект комплексного типа */
     if(type==_XTP_OBJ) {
                        }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - Простой объект */
     else               {

              ret_value=iDcl_sql_parform(pars[i], &sql_pars[sql_pars_cnt])  ;
           if(ret_value<0) {
                                     dcl_errno=ret_value ;
                             return(&dgt_return) ;
                           }

                                       sql_pars_cnt++ ;
                        }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                                               }
/*-------------------------------------------- Перенос DBF-заголовка */

        status=Table.Open(tmpl, NULL) ;                             /* Открытие файла DBF-шаблона */
     if(status) {
                  strncpy(connect->error_text, Table.error_text, sizeof(connect->error_text)-1) ;  
                            dcl_errno=E_DBF_FILE ;
                            ret_value=E_DBF_FILE ;
                    return(&dgt_return) ;
                }

        status=Table.Clone(path) ;                                  /* Запись заголовка в файл результата */
     if(status) {
                  strncpy(connect->error_text, Table.error_text, sizeof(connect->error_text)-1) ;  
                            dcl_errno=E_DBF_FILE ;
                            ret_value=E_DBF_FILE ;
                    return(&dgt_return) ;
                }

               Table.iFree() ;

/*----------------------------------------------- Выполнение выборки */

#define  NAME   (char *)cursor->columns[j].name
#define  VALUE  (char *)cursor->columns[j].value

   do {                                                             /* BLOCK.1 */
/*- - - - - - - - - - - - - - - - - - - - - - - - - Открытие выборки */
       cursor=connect->LockCursor("Dcl_sql_fetch2csv") ;            /* Захватываем курсор */
    if(cursor==NULL) {  ret_value=E_CURSOR_MAX ;
                               break ;           }

       status=connect->SelectOpen(cursor, sql,                      /* Открываем выборку */
                                          sql_pars, sql_pars_cnt) ;
    if(status) {  ret_value=E_SQL ;
                        break ;      }

        status=Table.Open(path, NULL) ;                             /* Открытие файла результата */
     if(status) {
                            dcl_errno=E_DBF_FILE ;
                            ret_value=E_DBF_FILE ;
                    return(&dgt_return) ;
                }
/*- - - - - - - - - - - - - - - - - - - - - - - - Построчная выборка */
    for(n=0 ; ; n++) {                                              /* LOOP - Сканируем выборку */ 

           status=connect->SelectFetch(cursor) ;                    /* Извлекаем очередную строку */
        if(status==_SQL_NO_DATA)   break ;                          /* Если все данные извлечены... */
        if(status) {                                                /* Если ошибка выборки */
                     ret_value=E_SQL ;
                      break ; 
                   }
/*- - - - - - - - - - - - - -  Привязка полей выборки к DBF-колонкам */
     if(n==0) {

      for(j=0 ; j<cursor->columns_cnt ; j++) {

        for(i=0 ; i<Table.fields_cnt ; i++)
          if(!strcmp(NAME, Table.fields[i].name )) {  map[j]=i ;  break ;  }

          if(i>=Table.fields_cnt) {                                 /* Если поле не найдено в DBF*/
                  sprintf(connect->error_text, "Field %s miseed in DBF file", cursor->columns[j].name) ;  
                            ret_value=E_DBF_FILE ;
                                        break ;
                                  }
                                             }
              }
/*- - - - - - - - - - - - - - - - - - - - Извлечение данных по полям */
      for(i=0 ; i<Table.fields_cnt    ; i++)  Table.fields[    i ].value=dummy ;

      for(j=0 ; j<cursor->columns_cnt ; j++)  Table.fields[map[j]].value=VALUE ;

/*- - - - - - - - - - - - - - - - - - - - -  Контроль значений полей */
      for(i=0 ; i<Table.fields_cnt ; i++) {

         if(Table.fields[i].value[0]==0)  continue ;

         if(Table.fields[i].format[0]=='C') {
                                               continue ;
                                            }
         else
         if(Table.fields[i].format[0]=='N') {

                 strtod(Table.fields[i].value, &end) ;

             if(*end!=0) {
                           sprintf(connect->error_text,
                                    "Illegal number format for DBF-field %s : %s", 
                                                             Table.fields[i].name,
                                                             Table.fields[i].value ) ;
                               ret_value=E_DBF_FILE ;
                                  break ;
                         }
                                            }
         else
         if(Table.fields[i].format[0]=='D') {

                     value=Table.fields[i].value ;

                if((value[0] <'0' || value[0]>'3'                  ) ||
                   (value[1] <'0' || value[1]>'9'                  ) ||
                   (value[2]!='.' && value[2]!='/' && value[2]!='-') ||
                   (value[3] <'0' || value[3]>'1'                  ) ||
                   (value[4] <'0' || value[4]>'9'                  ) ||
                   (value[5]!='.' && value[5]!='/' && value[5]!='-') ||
                   (value[6] <'0' || value[6]>'9'                  ) ||
                   (value[7] <'0' || value[7]>'9'                  ) ||
                   (value[8] <'0' || value[8]>'9'                  ) ||
                   (value[9] <'0' || value[9]>'9'                  )   ) {
                           sprintf(connect->error_text,
                                    "Illegal date format for DBF-field %s : %s", 
                                                             Table.fields[i].name,
                                                             Table.fields[i].value ) ;
                               ret_value=E_DBF_FILE ;
                                  break ;
                                                                         }

                                            }
         else                               {
                           sprintf(connect->error_text,
                                    "Unknown format %s for DBF-field %s : %s", 
                                                             Table.fields[i].format,
                                                             Table.fields[i].name,
                                                             Table.fields[i].value ) ;
                               ret_value=E_DBF_FILE ;
                                  break ;
                                            }

                                          }

                                 if(ret_value)  break ;
/*- - - - - - - - - - - - - - - - -  Приведение данных к DBF-формату */
      for(i=0 ; i<Table.fields_cnt ; i++) {

         if(Table.fields[i].value [0]== 0 )  continue ;

         if(Table.fields[i].format[0]=='C') {

              if(dcl_sql_cp_convert)
                   Kernel->zCodePageConvert(       Table.fields[i].value,
                                            strlen(Table.fields[i].value),
                                                dcl_sql_cp_data, dcl_sql_cp_out) ;
                                            }
         else
         if(Table.fields[i].format[0]=='N')   continue ;
         else
         if(Table.fields[i].format[0]=='D') {

                            tmp[0]=Table.fields[i].value[6] ;
                            tmp[1]=Table.fields[i].value[7] ;
                            tmp[2]=Table.fields[i].value[8] ;
                            tmp[3]=Table.fields[i].value[9] ;
                            tmp[4]=Table.fields[i].value[3] ;
                            tmp[5]=Table.fields[i].value[4] ;
                            tmp[6]=Table.fields[i].value[0] ;
                            tmp[7]=Table.fields[i].value[1] ;
                            tmp[8]= 0  ;

                strcpy(Table.fields[i].value, tmp) ;
                                            }

                                          }
/*- - - - - - - - - - - - - - - - - - - - - - - Запись данных в файл */
            status=Table.Insert() ;
         if(status) {
                       strncpy(connect->error_text, Table.error_text, sizeof(connect->error_text)-1) ;  
                             ret_value=E_DBF_FILE ;
                                   break ;
                    }
/*- - - - - - - - - - - - - - - - - - - - - - - - Построчная выборка */
                     }                                              /* ENDLOOP - Сканируем выборку */ 
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
      } while(0) ;                                                  /* BLOCK.1 */

#undef  NAME
#undef  VALUE

    if(ret_value==E_SQL)                                            /* Если SQL-ошибка - фиксируем на контексте соединения */
             strncpy(connect->error_text, 
                      cursor->error_text, sizeof(connect->error_text)-1) ;  

/*----------------------------------- Извлечение выходных параметров */

   for(i=0 ; i<sql_pars_cnt ; i++) {

        par=(Dcl_decl *)sql_pars[i].ref ;
     if(par==NULL)  continue ;

        type= t_base(par->type) ;                                   /* Извлекаем базовый тип */
/*- - - - - - - - - - - - - - - - - - - - - Объект комплексного типа */
     if(type==_XTP_OBJ) {
                        }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - Простой объект */
     else               {
   
                            size=strlen(sql_pars[i].buff) ;
        if(par->buff<size)  size=par->buff ;

                        memset(par->addr, 0, par->buff) ;
                        memcpy(par->addr, sql_pars[i].buff, size) ;
                               par->size=size ;
         Kernel->iSize_correct(par) ;                               /* Проводим следящую коррекцию размеров указателей на обьект */

                        }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                                   }
/*-------------------------------------------- Освобождение ресурсов */

   if(cursor!=NULL)  connect->UnlockCursor(cursor) ;

                                        free(sql) ;

   for(i=0 ; i<sql_pars_cnt ; i++) {
                                        free(sql_pars[i].name) ;
                                        free(sql_pars[i].buff) ;
                                   } 

                Table.iFree() ;

/*-------------------------------------------------------------------*/

                       dcl_errno=ret_value ;

     if(ret_value==0)  ret_value=n ;

   return(&dgt_return) ;
}


/*********************************************************************/
/*                                                                   */
/*       Подготовка к выборке по SELECT-запросу                      */
/*                                                                   */
/*        int  sql_open([hdc], sql[, value1, ..., valueN]) ;         */
/*                                                                   */
/*            int  hdc    - дескриптор соединения                    */
/*           char *sql    - SQL-оператор                             */
/*              ?  value? - Подставляемая переменная                 */
/*                                                                   */
/*  Возврашает дескриптор курсора (при успешном завершении)          */
/*   или код ошибки:                                                 */
/*     Е_CALL            -  некорректная структура вызова            */
/*     Е_PAR_TYPE        -  неподдерживаемый тип переменной          */
/*     Е_CONNECT_HEADER  -  некорректный дескриптор соединения       */
/*     Е_CURSOR_MAX      -  нет свободных курсоров                   */
/*     E_SQL_ERROR       -  ошибка исполнения SQL-оператора          */

  Dcl_decl *Dcl_sql_open(Lang_DCL *Kernel, Dcl_decl **pars, int  pars_cnt)

{
        SQL_cursor *cursor ;
               int  cursor_idx ;
          SQL_link *connect ;
               int  hdc ;
              char *sql ;                /* SQL-выражение */
     SQL_parameter  sql_pars[200] ;      /* Список SQL-параметров */
               int  sql_pars_cnt ;
          Dcl_decl *par ;
               int  type ;
               int  status ;  
               int  size ;
               int  i ;

   static   double  ret_value ;          /* Буфер числового значения */
   static Dcl_decl  dgt_return={_DGT_VAL, 0, 0, 0, "", &ret_value, NULL, 1, 1} ;

/*------------------------------------------- Определение соединения */

  if(pars_cnt     <   1  ||
     pars[0]->addr==NULL   )  {          dcl_errno=E_CALL ; 
                                         ret_value=E_CALL ;
                                 return(&dgt_return) ;       }

      type=t_base(pars[0]->type) ;                                  /* Если первый параметр имеет числовой тип - */
   if(type==_DGT_VAL) {                                             /*  - считаем его дискриптором соединения    */
                         hdc=(int)Kernel->iDgt_get(pars[0]->addr, pars[0]->type) ;
                     connect=  NULL ;

                         pars++ ;                                   /*  Проходим первый параметр */
                         pars_cnt-- ;

                      }
   else               {                                             /* Соединение по-умолчанию */
                         connect=dcl_sql_connect ;
                      }      

   if(connect==NULL) {
                                dcl_errno=E_HDC ;
                                ret_value=E_HDC ;
                        return(&dgt_return) ;
                     }

              connect->error_text[0]=0 ;                            /* Сброс ошибки */

/*----------------------------------------- Извлечение SQL-оператора */

  if(pars_cnt     <   1  ||
     pars[0]->addr==NULL   )  {          dcl_errno=E_CALL ;
                                         ret_value=E_CALL ;
                                 return(&dgt_return) ;       }

      type=t_base(pars[0]->type) ;
   if(type!=_DCLT_CHR_AREA &&                                       /* Проверка типа операнда */
      type!=_DCLT_CHR_PTR    ) {         dcl_errno=E_CALL ;
                                         ret_value=E_CALL ;
                                 return(&dgt_return) ;       }

                  sql=(char *)calloc(1, pars[0]->size+8) ;          /* Выделяем память по pars[0]->size */
          memcpy( sql, pars[0]->addr, pars[0]->size) ;              /* Извлекаем SQL-оператор */

/*-------------------------------- Извлечение параметров подстановки */

   for(sql_pars_cnt=0, i=1 ; i<pars_cnt ; i++) {

        type= t_base(pars[i]->type) ;                               /* Извлекаем базовый тип */
/*- - - - - - - - - - - - - - - - - - - - - Объект комплексного типа */
     if(type==_XTP_OBJ) {
                        }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - Простой объект */
     else               {

              ret_value=iDcl_sql_parform(pars[i], &sql_pars[sql_pars_cnt])  ;
           if(ret_value<0) {
                                     dcl_errno=ret_value ;
                             return(&dgt_return) ;
                           }

                                       sql_pars_cnt++ ;
                        }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                                               } 
/*----------------------------------- Фиксация курсора на дескриптор */

       cursor=connect->LockCursor("Dcl_sql_execute") ;              /* Захватываем курсор */
    if(cursor==NULL) {          dcl_errno=E_CURSOR_MAX ;
                                ret_value=E_CURSOR_MAX ;
                        return(&dgt_return) ;             }

       for(i=0 ; i<_CURSORS_MAX ; i++)                              /* Ищем свободный слот для курсора */
         if(dcl_sql_cursors[i].connect==NULL)  break ;

         if(i>=_CURSORS_MAX) {                                      /* Если все занято... */
                                 connect->UnlockCursor(cursor) ; 
                                      dcl_errno=E_CURSOR_MAX ;   
                                      ret_value=E_CURSOR_MAX ;   
                               return(&dgt_return) ;   
                             }

                             cursor_idx=i ;
             dcl_sql_cursors[cursor_idx].connect=connect ;          /* Фиксируем курсор на дескриптор */
             dcl_sql_cursors[cursor_idx].cursor =cursor ;

/*--------------------------------------------- Выполнение операции */

       status=connect->SelectOpen(cursor, sql,                      /* Исполняем SELECT-оператор */
                                          sql_pars, sql_pars_cnt) ;
    if(status)  ret_value=     E_SQL ;
    else        ret_value=cursor_idx ;

    if(status) {                                                    /* При ошибке освобождаем курсор и дескриптор */
                  dcl_sql_cursors[cursor_idx].connect=NULL ;
                  dcl_sql_cursors[cursor_idx].cursor =NULL ;

                        connect->UnlockCursor(cursor) ;
               } 
/*----------------------------------- Извлечение выходных параметров */

   for(i=0 ; i<sql_pars_cnt ; i++) {

        par=(Dcl_decl *)sql_pars[i].ref ;
     if(par==NULL)  continue ;

        type= t_base(par->type) ;                                   /* Извлекаем базовый тип */
/*- - - - - - - - - - - - - - - - - - - - - Объект комплексного типа */
     if(type==_XTP_OBJ) {
                        }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - Простой объект */
     else               {
   
                            size=strlen(sql_pars[i].buff) ;
        if(par->buff<size)  size=par->buff ;

                        memset(par->addr, 0, par->buff) ;
                        memcpy(par->addr, sql_pars[i].buff, size) ;
                               par->size=size ;
         Kernel->iSize_correct(par) ;                               /* Проводим следящую коррекцию размеров указателей на обьект */
                        }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                                   }
/*-------------------------------------------- Освобождение ресурсов */

                                        free(sql) ;

   for(i=0 ; i<sql_pars_cnt ; i++) {
                                        free(sql_pars[i].name) ;
                                        free(sql_pars[i].buff) ;
                                   } 
/*-------------------------------------------------------------------*/

    if(ret_value==E_SQL)                                            /* Если ошибка - фиксируем на контексте соединения */
             strncpy(connect->error_text, 
                      cursor->error_text, sizeof(connect->error_text)-1) ;  

           dcl_errno=ret_value ;

   return(&dgt_return) ;
}


/*********************************************************************/
/*                                                                   */
/*           Закрытие заданного или всех курсоров                    */
/*                                                                   */
/*        int  sql_close([hcursor]) ;                                */
/*                                                                   */
/*            int  hcursor - дескриптор курсора                      */
/*                                                                   */
/*  Возврашает 0 при успешном завершении или код ошибки:             */
/*     Е_CURSOR_HEADER  -  некорректный дескриптор соединения        */

  Dcl_decl *Dcl_sql_close(Lang_DCL *Kernel, Dcl_decl **pars, int  pars_cnt)

{
               int  idx ;
               int  type ;
               int  i ;

   static   double  ret_value ;          /* Буфер числового значения */
   static Dcl_decl  dgt_return={_DGT_VAL, 0, 0, 0, "", &ret_value, NULL, 1, 1} ;

/*---------------------------------- Определение дескриптора курсора */

   if(pars_cnt>=1) {                                                /* Если задан десцриптор курсора */
                      type=t_base(pars[0]->type) ;

      if(pars[0]->addr==  NULL   ||                                 /*  Проверка типа операнда */
                  type!=_DGT_VAL   ) {        dcl_errno=E_CALL ; 
                                              ret_value=E_CALL ;
                                       return(&dgt_return) ;      }

               idx=(int)Kernel->iDgt_get(pars[0]->addr, pars[0]->type) ;

      if(idx<0 || idx>=_CURSORS_MAX) {                              /*  Проверка значения десцриптора курсора */
                                               dcl_errno=E_CALL ;
                                               ret_value=E_CALL ;
                                        return(&dgt_return) ;      
                                     }

      if(dcl_sql_cursors[idx].connect==NULL) {                      /*  Проверка актуальности десцриптора курсора */
                                               dcl_errno=E_HCURSOR ;
                                               ret_value=E_HCURSOR ;
                                       return(&dgt_return) ;      
                                             }
                   }                     
   else            {
                                   idx= -1 ;
                   }  
/*------------------------------------------------ Для всех курсоров */

    if(idx==-1) {

       for(i=0 ; i<_CURSORS_MAX ; i++)
         if(dcl_sql_cursors[i].connect!=NULL) {

             dcl_sql_cursors[i].connect->UnlockCursor(dcl_sql_cursors[i].cursor) ; 

                  dcl_sql_cursors[i].connect=NULL ;
                  dcl_sql_cursors[i].cursor =NULL ;

                                              }
                } 
/*------------------------------------------- Для выбранного курсора */

    else        {

             dcl_sql_cursors[idx].connect->UnlockCursor(dcl_sql_cursors[idx].cursor) ; 

             dcl_sql_cursors[idx].connect=NULL ;
             dcl_sql_cursors[idx].cursor =NULL ;

                } 
/*-------------------------------------------------------------------*/

           dcl_errno=ret_value ;

   return(&dgt_return) ;
}


/*********************************************************************/
/*                                                                   */
/*                Выдача текста последней SQL-ошибки                 */
/*                                                                   */
/*          int  sql_error([hdc], text) ;                            */
/*                                                                   */
/*              int  hdc  - дескриптор соединения                    */
/*             char *text - буфер для текста ошибки                  */
/*                                                                   */
/*    Возврашает 0 при успешном завершении или код ошибки:           */
/*     Е_CALL            -  некорректная структура вызова            */
/*     Е_CONNECT_HEADER  -  некорректный дескриптор соединения       */

  Dcl_decl *Dcl_sql_error(Lang_DCL *Kernel, Dcl_decl **pars, int  pars_cnt)

{
          SQL_link *connect ;
               int  hdc ;
               int  type ;
              char *error ;

   static   double  ret_value ;          /* Буфер числового значения */
   static Dcl_decl  dgt_return={_DGT_VAL, 0, 0, 0, "", &ret_value, NULL, 1, 1} ;

/*------------------------------------------- Определение соединения */

  if(pars_cnt     <   1  ||
     pars[0]->addr==NULL   )  {          dcl_errno=E_CALL ;
                                         ret_value=E_CALL ;
                                 return(&dgt_return) ;       }

      type=t_base(pars[0]->type) ;                                  /* Если первый параметр имеет числовой тип - */
   if(type==_DGT_VAL) {                                             /*  - считаем его дискриптором соединения    */
                         hdc=(int)Kernel->iDgt_get(pars[0]->addr, pars[0]->type) ;
                     connect=  NULL ;

                         pars++ ;                                   /*  Проходим первый параметр */
                         pars_cnt-- ;

                      }
   else               {                                             /* Соединение по-умолчанию */
                         connect=dcl_sql_connect ;
                      }      

/*------------------------------------- Определение источника ошибки */

   if(connect==NULL)  error= dcl_sql_error ;                        /* Если интерфейс уже освобожден - используем резервный буфер */
   else               error=connect->error_text ;

/*---------------------------------------------------- Выдача ошибки */

  if(pars_cnt     <   1  ||
     pars[0]->addr==NULL   )  {          dcl_errno=E_CALL ;
                                         ret_value=E_CALL ;
                                 return(&dgt_return) ;       }

      type=t_base(pars[0]->type) ;
   if(type!=_DCLT_CHR_AREA &&                                       /* Проверка типа операнда */
      type!=_DCLT_CHR_PTR    ) {         dcl_errno=E_CALL ;
                                         ret_value=E_CALL ;
                                 return(&dgt_return) ;       }


      memset(        pars[0]->addr, 0, pars[0]->buff) ;             /* Выдаем ошибку */
     strncpy((char *)pars[0]->addr, error, pars[0]->buff-1) ;

                     pars[0]->size=strlen((char *)pars[0]->addr) ;

           Kernel->iSize_correct(pars[0]) ;                         /*  Проводим следящую коррекцию     */
								    /*    размеров указателей на обьект */

/*-------------------------------------------------------------------*/

           dcl_errno=ret_value ;

   return(&dgt_return) ;
}


/*********************************************************************/
/*                                                                   */
/*       Формирование SQL-параметра по DCL-параметру                 */

  double  iDcl_sql_parform(Dcl_decl *par, SQL_parameter *sql)

{
   int  type ;

/*----------------------------------------------- Формирование имени */

                     sql->name=(char *)calloc(1, 128) ;             /* Формируем подстановочное имя параметра */
             sprintf(sql->name, ":%s", par->name) ;                 /*    добавлением впереди двоеточия       */ 
                     sql->blob     =0 ;
                     sql->blob_size=0 ;

                 type=t_base(par->type) ;                           /* Извлекаем базовый тип */

/*--------------------------------------------------- NULL-указатели */

       if(     type==_NULL_PTR ||                                   /* Для NULL-указателей */
          par->addr== NULL       ) {

                  sql->buff=(char *)calloc(1, par->buff+8) ;
                  sql->size= par->buff+1 ;
                  sql->ref = NULL ;

                                   }  
/*---------------------------------------------- Строчные переменные */

       else
       if(type==_DCLT_CHR_AREA ||                                   /* Для строчных переменных */
          type==_DCLT_CHR_PTR    ) {

                     sql->buff=(char *)calloc(1, par->buff+8) ;
              memcpy(sql->buff, par->addr, par->size) ;
                     sql->size= par->buff+1 ;
                     sql->ref = par ;

         if(  type==_DCLT_CHR_PTR ||                                /* BLOB-объектами счетаем динамические массивы и указатели */
            ( par->prototype!=NULL &&
             *par->prototype=='D'    ) ) {
                             sql->blob     = 1 ;
                             sql->blob_size=par->size ;  
                                         }

                                   }
/*-------------------------------------------- Необрабатываемые типы */

       else                        {                                /* Для необрабатываемы типов */
                                           return(E_CALL) ; 
                                   }
/*-------------------------------------------------------------------*/

   return(0.) ;
}


/*********************************************************************/
/*                                                                   */
/*   Получение данных соединения и курсора по дескриптору курсора    */

    int  iDcl_sql_getcursor(       int   idx, 
                              SQL_link **connect, 
                            SQL_cursor **cursor  )

{
      if(idx<0 || idx>=_CURSORS_MAX) {                              /*  Проверка значения десцриптора курсора */
                                           *connect=NULL ;
                                            *cursor=NULL ;
                                              return(-1) ;
                                     }

   
            *connect=dcl_sql_cursors[idx].connect ;
             *cursor=dcl_sql_cursors[idx].cursor ;  

     if(*connect==NULL ||
         *cursor==NULL   ) {  dcl_errno=E_HCURSOR ;
                                   return(-1) ;      }

   return(0) ;
}

/*********************************************************************/
/*                                                                   */
/*              Выполнение SELECT-выборки                            */

   double  Dcl_sql_fetch(Lang_DCL *Kernel, Dcl_decl *source, 
                                                int  cursor_idx, 
                                                int  fetch_cnt  )

{
          SQL_link *connect ;
        SQL_cursor *cursor ;
               int  status ;
               int  size ;
               int  i ;
               int  j ;

          Dcl_decl *rec_data ;
  Dcl_complex_type  rec_template={ "sql", 0, 0, 0} ;

#define  NAME   (char *)cursor->columns[j].name
#define  VALUE  (char *)cursor->columns[j].value

/*----------------------------------------------- Подготовка курсора */

      if(cursor_idx<       0     ||                                 /* Проверка значения десцриптора курсора */
         cursor_idx>=_CURSORS_MAX  ) {  dcl_errno=E_HCURSOR ;
                                               return(-1) ;    }

         connect=dcl_sql_cursors[cursor_idx].connect ;
          cursor=dcl_sql_cursors[cursor_idx].cursor ;  

      if(connect==NULL ||
          cursor==NULL   ) {  dcl_errno=E_HCURSOR ;
                                   return(-1) ;      }

              connect->error_text[0]=0 ;                            /* Сброс ошибки */

/*------------------------------------ Формирование структуры записи */

         rec_data=(Dcl_decl *)                                      /* Размещаем описание полей */
                   calloc(cursor->columns_cnt, sizeof(*rec_data)) ;

    for(j=0 ; j<cursor->columns_cnt ; j++) {                        /* Формируем описание полей */

                  rec_data[j].type=_CHR_AREA ;
          strncpy(rec_data[j].name, NAME, sizeof(rec_data[j].name)-1) ;
                                           }
/*------------------------------------------------ Извлечение данных */

    for(i=0 ; fetch_cnt<=0 || i<fetch_cnt ; i++) {                  /* LOOP - Сканируем выборку */ 

           status=connect->SelectFetch(cursor) ;                    /* Извлекаем очередную строку */
        if(status==_SQL_NO_DATA)   break ;                          /* Если все данные извлечены... */
        if(status) {                                                /* Если ошибка выборки */
                     dcl_errno=E_SQL ;
                      break ; 
                   }

      for(size=0, j=0 ; j<cursor->columns_cnt ; j++) {                      /* Привязываем данные полей */
                    rec_data[j].addr     =(void *)size ;
                    rec_data[j].prototype=        VALUE ;
                    rec_data[j].size     = strlen(VALUE) ;
                    rec_data[j].buff     = strlen(VALUE)+1 ;
                                size    +=rec_data[j].buff ;
                                                     }

                    rec_template.list    =rec_data ;                /* Готовим шаблон записи */
                    rec_template.list_cnt=cursor->columns_cnt ;
                    rec_template.size    =  size ;

             status=Kernel->iXobject_add(source, &rec_template) ;   /* Добавляем запись */
          if(status) break ;
          
                                                 }                  /* ENDLOOP - Сканируем выборку */ 
/*-------------------------------------------- Освобождение ресурсов */

                          free(rec_data) ;

/*-------------------------------------------------------------------*/

#undef  NAME
#undef  VALUE

   return(0) ;
}
