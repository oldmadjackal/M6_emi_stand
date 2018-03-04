/*********************************************************************/
/*                                                                   */
/*                   DATA CONVERTION LANGUAGE                        */
/*                                                                   */
/*                 БИБЛИОТЕКА СИСТЕМНЫХ ФУНКЦИЙ                      */
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
#include <stdio.h>
#include <string.h>
#include <time.h>

#ifdef UNIX
#include <unistd.h>
#include <ctype.h>
#define   O_BINARY          0
#define   stricmp           strcasecmp
#define   CharToOem(a, b)   strcmp(a, b)
#define   strupr(t)       for(char *b=t ; *b ; b++)  *b=toupper(*b)
#define   getch()           time(NULL)
#else
#include <conio.h>
#include <windows.h>
#endif

#define   DCL_INSIDE
#include "dcl.h"
#include "dcl_chars.h"
#include "dcl_md5.h"


#pragma warning(disable : 4996)

/*---------------------------------------------------- Внешние связи */

          extern double  dcl_errno ;            /* Системный указатель ошибок -> DCL_SLIB.CPP */
          extern   char  dcl_err_details[512] ; /* Системный указатель расширенного описания ошибок -> DCL_SLIB.CPP */

/*------------------------------------------------ "Внешние" объекты */

	        double  dcl_errno ;             /* Системный указатель ошибок */
	          char  dcl_err_details[512] ;  /* Системный указатель расширенного описания ошибок */

  Dcl_decl *Dcl_version     (Lang_DCL *, Dcl_decl **, int) ;    /* Выдача версии */
  Dcl_decl *Dcl_set_error   (Lang_DCL *, Dcl_decl **, int) ;    /* Установка ошибки */
  Dcl_decl *Dcl_get_errtext (Lang_DCL *, Dcl_decl **, int) ;    /* Запрос расширенного ошисания ошибки */
  Dcl_decl *Dcl_sizeof      (Lang_DCL *, Dcl_decl **, int) ;    /* Выдача размера обьекта */
  Dcl_decl *Dcl_buffof      (Lang_DCL *, Dcl_decl **, int) ;    /* Выдача размера буфера обьекта */
  Dcl_decl *Dcl_setsize     (Lang_DCL *, Dcl_decl **, int) ;    /* Установка размера обьекта */
  Dcl_decl *Dcl_setconstant (Lang_DCL *, Dcl_decl **, int) ;    /* Установка обьекта как неизменяемого */
  Dcl_decl *Dcl_stod        (Lang_DCL *, Dcl_decl **, int) ;    /* Преобразование строк в данные */
  Dcl_decl *Dcl_dtos        (Lang_DCL *, Dcl_decl **, int) ;    /* Преобразование данных в строку */
  Dcl_decl *Dcl_dtod        (Lang_DCL *, Dcl_decl **, int) ;    /* Преобразование данные разных типов */
  Dcl_decl *Dcl_atos        (Lang_DCL *, Dcl_decl **, int) ;    /* Преобразование данных в строку */
  Dcl_decl *Dcl_atot        (Lang_DCL *, Dcl_decl **, int) ;    /* Преобразование абсолютного времени в строку дата/время */
  Dcl_decl *Dcl_time        (Lang_DCL *, Dcl_decl **, int) ;    /* Выдача временных параметров */
  Dcl_decl *Dcl_elapsed_time(Lang_DCL *, Dcl_decl **, int) ;    /* Выдача временных меток */
  Dcl_decl *Dcl_trim        (Lang_DCL *, Dcl_decl **, int) ;    /* Усечение начальных и конечных символов */
  Dcl_decl *Dcl_replace     (Lang_DCL *, Dcl_decl **, int) ;    /* Замена символьного фрагмента на другой */
  Dcl_decl *Dcl_replace_char(Lang_DCL *, Dcl_decl **, int) ;    /* Замена символов из набора на символьный фрагмент */
  Dcl_decl *Dcl_transpose   (Lang_DCL *, Dcl_decl **, int) ;    /* Преобразование форматных строк */
  Dcl_decl *Dcl_upper       (Lang_DCL *, Dcl_decl **, int) ;    /* Перевод символов в верхний регистр */
  Dcl_decl *Dcl_cp_convert  (Lang_DCL *, Dcl_decl **, int) ;    /* Пребразование кодовой страницы текста */
  Dcl_decl *Dcl_system      (Lang_DCL *, Dcl_decl **, int) ;    /* Выполнение shell-команды */
  Dcl_decl *Dcl_sleep       (Lang_DCL *, Dcl_decl **, int) ;    /* Пауза в исполнении */
  Dcl_decl *Dcl_log_file    (Lang_DCL *, Dcl_decl **, int) ;    /* Сделать запись в лог-файл */
  Dcl_decl *Dcl_log_event   (Lang_DCL *, Dcl_decl **, int) ;    /* Сделать запись в системный лог событий */
  Dcl_decl *Dcl_signal      (Lang_DCL *, Dcl_decl **, int) ;    /* Отправка сигнала */
  Dcl_decl *Dcl_md5         (Lang_DCL *, Dcl_decl **, int) ;    /* Вычисление md5-хэша строки */
  Dcl_decl *Dcl_f_md5       (Lang_DCL *, Dcl_decl **, int) ;    /* Вычисление md5-хэша данных из файла */

/*------------------------------------------------ Эмуляция констант */

#define   SE_BADFORMAT   1.
#define   SE_STRSIZE     2.
#define   SE_BADSTRING   3.
#define   SE_DATABUFF    4.

	 static double  se_badformat=SE_BADFORMAT ;   /* Эмуляция кодов ошибок */
	 static double  se_sizestr  =SE_STRSIZE ;
	 static double  se_badstring=SE_BADSTRING ;
	 static double  se_databuff =SE_DATABUFF ;

/*--------------------------------------------------- Таблица ссылок */

   Dcl_decl  dcl_std_lib[]={

         {0, 0, 0, 0, "$PassiveData$", NULL, "slib", 0, 0},

	 {_DGT_VAL,  0, 0, 0, "errno",   &dcl_errno,    NULL, 1, 1},

	 {_DGT_VAL,  0, 0, 0, "double",  &dcl_errno,  NULL, 1, 1},  /* ?????  */
	 {_CHR_AREA, 0, 0, 0, "char",    &dcl_errno,  NULL, 1, 1},  /* ?????  */

	 {_DGT_VAL,  0, 0, 0, "SE_BADFORMAT", &se_badformat, NULL, 1, 1},
	 {_DGT_VAL,  0, 0, 0, "SE_STRSIZE",   &se_sizestr,   NULL, 1, 1},
	 {_DGT_VAL,  0, 0, 0, "SE_BADSTRING", &se_badstring, NULL, 1, 1},
	 {_DGT_VAL,  0, 0, 0, "SE_DATABUFF",  &se_databuff,  NULL, 1, 1},

	 {_CHR_PTR, _DCL_CALL, 0, 0, "version",      (void *)Dcl_version,       "",      0, 0},
	 {_DGT_VAL, _DCL_CALL, 0, 0, "set_error",    (void *)Dcl_set_error,     "v",     0, 0},
	 {_CHR_PTR, _DCL_CALL, 0, 0, "get_errtext",  (void *)Dcl_get_errtext,   "",      0, 0},
	 {_DGT_VAL, _DCL_CALL, 0, 0, "sizeof",       (void *)Dcl_sizeof,        "a",     0, 0},
	 {_DGT_VAL, _DCL_CALL, 0, 0, "buffof",       (void *)Dcl_buffof,        "a",     0, 0},
	 {_DGT_VAL, _DCL_CALL, 0, 0, "setsize",      (void *)Dcl_setsize,       "sv",    0, 0},
	 {_DGT_VAL, _DCL_CALL, 0, 0, "constant",     (void *)Dcl_setconstant,   "a",     0, 0},
	 {_DGT_VAL, _DCL_CALL, 0, 0, "stod",         (void *)Dcl_stod,          "ssa",   0, 0},
	 {_CHR_PTR, _DCL_CALL, 0, 0, "dtos",         (void *)Dcl_dtos,          "ssa",   0, 0},
	 {_DGT_VAL, _DCL_CALL, 0, 0, "dtod",         (void *)Dcl_dtod,          "saa",   0, 0},
	 {_CHR_PTR, _DCL_CALL, 0, 0, "atos",         (void *)Dcl_atos,          "a",     0, 0},
	 {_CHR_PTR, _DCL_CALL, 0, 0, "atot",         (void *)Dcl_atot,          "a",     0, 0},
	 {_DGT_VAL, _DCL_CALL, 0, 0, "time",         (void *)Dcl_time,          "s",     0, 0},
	 {_DGT_VAL, _DCL_CALL, 0, 0, "elapsed_time", (void *)Dcl_elapsed_time,  "v",     0, 0},
	 {_CHR_PTR, _DCL_CALL, 0, 0, "trim",         (void *)Dcl_trim,          "s",     0, 0},
	 {_DGT_VAL, _DCL_CALL, 0, 0, "replace",      (void *)Dcl_replace,       "sss",   0, 0},
	 {_DGT_VAL, _DCL_CALL, 0, 0, "replace_char", (void *)Dcl_replace_char,  "sss",   0, 0},
	 {_DGT_VAL, _DCL_CALL, 0, 0, "transpose",    (void *)Dcl_transpose,     "sssss", 0, 0},
	 {_DGT_VAL, _DCL_CALL, 0, 0, "upper",        (void *)Dcl_upper,         "ss",    0, 0},
         {_DGT_VAL, _DCL_CALL, 0, 0, "cp_convert",   (void *)Dcl_cp_convert,    "sss",   0, 0},
	 {_DGT_VAL, _DCL_CALL, 0, 0, "system",       (void *)Dcl_system,        "s",     0, 0},
         {_DGT_VAL, _DCL_CALL, 0, 0, "sleep",        (void *)Dcl_sleep,         "v",     0, 0},
         {_DGT_VAL, _DCL_CALL, 0, 0, "log_file",     (void *)Dcl_log_file,      "ss",    0, 0},
         {_DGT_VAL, _DCL_CALL, 0, 0, "log_event",    (void *)Dcl_log_event,     "ssss",  0, 0},
         {_DGT_VAL, _DCL_CALL, 0, 0, "signal",       (void *)Dcl_signal,        "sssss", 0, 0},
	 {_CHR_PTR, _DCL_CALL, 0, 0, "md5",          (void *)Dcl_md5,           "ss",    0, 0},
	 {_CHR_PTR, _DCL_CALL, 0, 0, "f_md5",        (void *)Dcl_f_md5,         "ss",    0, 0},

	 {0, 0, 0, 0, "", NULL, NULL, 0, 0}
                           } ;
/*----------------------------------------------- Рабочие переменные */

#undef   _BUFF_SIZE
#define  _BUFF_SIZE   128

 static  double  dgt_value ;          /* Буфер числового значения */
 static    char  buff[_BUFF_SIZE] ;   /* Буфер символьных строк */

 static Dcl_decl  void_return={_NULL_PTR, 0,0,0,"",    NULL,    NULL, 0, 0} ;
 static Dcl_decl   dgt_return={ _DGT_VAL, 0,0,0,"", &dgt_value, NULL, 1, 1} ;
 static Dcl_decl   chr_return={ _CHR_PTR, 0,0,0,"",    NULL,    NULL, 0, 0} ;

 extern struct Dcl_vars_decl  dcl_result ;     /* Описание результата */


/*****************************************************************/
/*                                                               */
/*                   Выдача версии                               */
/*                                                               */
/*          char *version(void) ;                                */
/*                                                               */
/*      Выдает строку с идентификатором версии                   */

    Dcl_decl *Dcl_version(Lang_DCL *Kernel, Dcl_decl **pars, int  pars_cnt)

{
  static char  version[128] ;

                           strcpy(version, _VERSION) ;

	   chr_return.addr=       version ;
	   chr_return.size=strlen(version) ;
	   chr_return.buff=strlen(version) ;

  return(&chr_return) ;
}


/*****************************************************************/
/*                                                               */
/*                Установка ошибки                               */
/*                                                               */
/*            void  set_error(error_code) ;                      */
/*                                                               */
/*              double  error_code  - код ошибки                 */

    Dcl_decl *Dcl_set_error(Lang_DCL *Kernel, Dcl_decl **pars, int  pars_cnt)

{
  double  error ;


      error=Kernel->iDgt_get(pars[0]->addr, pars[0]->type) ;        /* Извлекаем код ошибки */

                 dcl_errno=error ;
      Kernel->mError_code =error ;

  return(NULL) ;
}


/*****************************************************************/
/*                                                               */
/*              Выдача расширенного описания ошибки              */
/*                                                               */
/*          char *get_errtext(void) ;                            */
/*                                                               */
/*      Выдает строку с идентификатором версии                   */

    Dcl_decl *Dcl_get_errtext(Lang_DCL *Kernel, Dcl_decl **pars, int  pars_cnt)

{
	   chr_return.addr=       dcl_err_details ;
	   chr_return.size=strlen(dcl_err_details) ;
	   chr_return.buff=strlen(dcl_err_details) ;

  return(&chr_return) ;
}


/*****************************************************************/
/*                                                               */
/*                   Выдача размера обьекта                      */
/*                                                               */
/*          double  sizeof(object) ;                             */
/*                                                               */
/*      Выдает размер обьекта в байтах                           */

    Dcl_decl *Dcl_sizeof(Lang_DCL *Kernel, Dcl_decl **pars, int  pars_cnt)

{
    int  type ;   /* Базовый тип переменной */


	  type=t_base(pars[0]->type) ;                              /* Извлекаем базовый тип */

   if(pars_cnt     <   1       ||
	       type==_NULL_PTR ||
      pars[0]->addr== NULL       )  dgt_value= 0. ;
   else                             dgt_value=pars[0]->size ;

   if(type==_DGT_VAL  ||
      type==_DGT_AREA ||
      type==_DGT_PTR    )  dgt_value*=Kernel->iDgt_size(pars[0]->type) ;

  return(&dgt_return) ;
}


/*****************************************************************/
/*                                                               */
/*             Выдача размера буфера обьекта                     */
/*                                                               */
/*          double  buffof(var) ;                                */
/*                                                               */
/*      Выдает размер буфера переменной в байтах                 */

    Dcl_decl *Dcl_buffof(Lang_DCL *Kernel, Dcl_decl **pars, int  pars_cnt)

{
    int  type ;    /* Базовый тип переменной */


	  type=t_base(pars[0]->type) ;                              /* Извлекаем базовый тип */

   if(pars_cnt     <   1       ||
	       type==_NULL_PTR ||
      pars[0]->addr== NULL       )  dgt_value= 0. ;
   else                             dgt_value=pars[0]->buff ;

   if(type==_DGT_VAL  ||
      type==_DGT_AREA ||
      type==_DGT_PTR    )  dgt_value*=Kernel->iDgt_size(pars[0]->type) ;

  return(&dgt_return) ;
}


/*****************************************************************/
/*                                                               */
/*                Установка размера обьекта                      */
/*                                                               */
/*            void  setsize(object, size) ;                      */
/*                                                               */
/*              double  size  - размер обьекта, байт             */
/*                                                               */
/*      Устанавливает размер обьекта в байтах. Действует только  */
/*  для бинарных обьектов.                                       */

    Dcl_decl *Dcl_setsize(Lang_DCL *Kernel, Dcl_decl **pars, int  pars_cnt)

{
  int  size ;


      size=(int)Kernel->iDgt_get(pars[1]->addr, pars[1]->type) ;    /* 'Извлекаем' размер */

   if(size<pars[0]->buff)  pars[0]->size=         size ;            /* Уст.размер обьекта с учетом */
   else                    pars[0]->size=pars[0]->buff ;            /*   распологаемого буфера     */

          Kernel->iSize_correct(pars[0]) ;                          /* Проводим следящую коррекцию     */
                						    /*   размеров указателей на обьект */
  return(NULL) ;
}


/*****************************************************************/
/*                                                               */
/*     Установка обьекту признака постоянного (READ ONLY)        */
/*                                                               */
/*            void  constant(object) ;                           */

    Dcl_decl *Dcl_setconstant(Lang_DCL *Kernel, Dcl_decl **pars, int  pars_cnt)

{
  Dcl_decl *vars ;


     for(vars=Kernel->nInt_page ; vars->name[0]!=0 ; vars++)
       if(!strcmp(vars->name, pars[0]->name))   break ;

       if(vars->name[0]==0) {
                               Kernel->mError_code=_DCLE_UNKNOWN_NAME ;
                                      return(NULL) ;
                            }

         vars->buff=-1 ;      

  return(NULL) ;
}


/*****************************************************************/
/*                                                               */
/*                 Преобразование строк в данные                 */
/*                                                               */
/*           char *stod(format, string, data) ;                  */
/*                                                               */
/*              char *format  - формат преобразования            */
/*              char *string  - преобразуемая строка             */
/*              ...   data    - буфер данных                     */
/*                                                               */
/*   Структура формата: [pref_chr]%[u][pref]type[.point][>]      */
/*                                                               */
/*               type - базовый тип:  d -  int/short/long        */
/*                                    f -  float/double    ;     */
/*               pref - префикс типа: h -  short                 */
/*                                    l -  long/double      ;    */
/*                  u - спецификатор unsigned ;                  */
/*              point - положение запятой, если она опущена  ;   */
/*           pref_chr - префикс символ,                          */
/*                       так для строк  ............444444       */
/*                                  и  -............444444       */
/*                                 префикс символ будет '.' ;    */
/*                  > - признак автодетектирования окончания     */
/*                       дешифровки. При откюченном автодетект.  */
/*                         может регистр. ошибка SE_BADSTRING .  */
/*                                                               */
/*   Возвращает указатель на следующий за последним              */
/*       дешифрованным символом.                                 */
/*                                                               */
/*   Код ошибки:                                                 */
/*        SE_BADFORMAT  -  некорректный формат                   */
/*        SE_STRSIZE    -  слишком длинная дешифруемая строка    */
/*        SE_BADSTRING  -  строка не полностью дешифрована       */
/*        SE_DATABUFF   -  слишком короткий буфер данных         */

 union Dcl_types {
		     char  d_char[sizeof(double)] ;
		      int  d_int ;
	   unsigned   int  d_intu ;
		     long  d_long ;
	   unsigned  long  d_longu ;
		    short  d_short ;
	   unsigned short  d_shortu ;
		    float  d_float ;
		   double  d_double ;
		 } ;

 union Dcl_types_addr {
			 void *a_void ;
			 char *a_char ;
			  int *a_int ;
	       unsigned   int *a_intu ;
			 long *a_long ;
	       unsigned  long *a_longu ;
			short *a_short ;
	       unsigned short *a_shortu ;
			float *a_float ;
		       double *a_double ;
		      } ;

    Dcl_decl *Dcl_stod(Lang_DCL *Kernel, Dcl_decl **pars, int  pars_cnt)

{
  int  pref_flag ;  /* Флаг префикса */
  int  pref_char ;  /* Префикс-символ */
 char  type_base ;  /* Базовый тип: D - целое, F - плавающее */
 char  type_mode ;  /* Модификатор типа: H - short, L - long/double */
 char  type_sign ;  /* Модификатор знака: S - signed, U - unsigned */
  int  point ;      /* Положение точки */
  int  scan_flag ;  /* Флаг режима автопоиска */
  int  size ;       /* Длина дешифруемой строки */
 void *data ;       /* Буфер конвертирования данных */
  int  data_size ;  /* Необходимый размер буфера данных */
  int  data_buff ;  /* Располагаемый размер буфера данных */
  int  type2 ;      /* Базовый тип операнда-'приемника' */
 char *addr ;
 char *end ;        /* Завершение дешифровки */
  int  sp_char ;    /* Символ для "замазывания" префикса */
  int  len ;

	     int  d_int ;
  unsigned   int  d_intu ;
	    long  d_long ;
  unsigned  long  d_longu ;
	   short  d_short ;
  unsigned short  d_shortu ;
	   float  d_float ;
	  double  d_double ;

/*------------------------------------------------- Входной контроль */

  if(pars_cnt      <  3  ||
     pars[0]->addr==NULL ||
     pars[1]->addr==NULL ||
     pars[2]->addr==NULL   )  return(&void_return) ;

/*---------------------------------------------------- Инициализация */

                   data     =NULL ;
                   data_size=  0 ;
                   pref_char=  0 ;

/*----------------------------------------------- Дешифровка формата */

      if(pars[0]->size>=_BUFF_SIZE) {  dcl_errno=SE_BADFORMAT ;
					  return(&void_return) ;  }

		 addr=(char *)pars[0]->addr ;
/*- - - - - - - - - - - - - - - - - - - - - - Анализ префикс-символа */
       if(addr[0]!='%') {  pref_flag= 1 ;                           /* Если задан префикс-символ -         */
			   pref_char=addr[0] ;                      /*  выделяем его, уст. флаг наличия    */
				addr++ ;       }                    /*   префикс-символа и проходим дальше */
       else                pref_flag= 0 ;
/*- - - - - - - - - - - - - - - - - - - - Подготовка анализа формата */
      memset(buff, 0, _BUFF_SIZE) ;
      memcpy(buff, addr, pars[0]->size-pref_flag) ;                 /* Заносим формат в рабочий массив */
      strupr(buff) ;                                                /* Переводим формат в верхний регистр */

       if(buff[0]!='%') {  dcl_errno=SE_BADFORMAT ;                 /* Если отсутствует идентиф.символ */
			      return(&void_return) ;  }
/*- - - - - - - - - - - - - - - - - - - - - - -  Установка умолчаний */
		   type_base= 0 ;                                   /* Установка имолчаний */
		   type_mode='N' ;
		   type_sign='S' ;
		       point= 0 ;
		   scan_flag= 0 ;
/*- - - - - - - - - - - - - - - - - - - - - - - - Дешифровка формата */
	  addr=buff+1 ;

      if(*addr=='U'              ) {  type_sign=addr[0] ;  addr++ ;  }
      if(*addr=='H' || *addr=='L') {  type_mode=addr[0] ;  addr++ ;  }
      if(*addr=='D' || *addr=='F') {  type_base=addr[0] ;  addr++ ;  }

      if(*addr=='.') {
			point=strtol(addr+1, &addr, 10) ;
		     }
      if(*addr=='>') {  scan_flag=1 ;  addr++ ;  }
/*- - - - - - - - - - - - - - - - - -  Анализ результатов дешифровки */
      if( *addr   !=0 ||                                            /* Если формат не полностью       */
	 type_base==0   ) {  dcl_errno=SE_BADFORMAT ;               /*  дешифрован или в нем не задан */
			      return(&void_return) ;  }             /*   базовый тип ...              */

/*------------------------------------ Подготовка дешивруемой строки */

      if(pars[0]->size>=_BUFF_SIZE-1) {  dcl_errno=SE_STRSIZE ;
					   return(&void_return) ;  }

	    size=pars[1]->size ;                                    /* Снимаем размер строки */

      memset(buff, 0, _BUFF_SIZE) ;
      memcpy(buff, pars[1]->addr, size) ;                           /* Заносим строку в рабочий массив */

/*---------------------------------------- Обработка префикс-символа */

    if(pref_flag) {

       if(buff[0]=='+' || buff[0]=='-') {  sp_char='0' ;            /* Опред.символ-замазыватель и   */
					      addr=buff+1 ;  }      /*   начало замазывания префикса */
       else                             {  sp_char=' ' ;
					      addr=buff ;    }

	   for( ; *addr==pref_char ; addr++)  *addr=sp_char ;       /* Замазываем префикс */
		  }
/*--------------------------------------- Установка десятичной точки */

			   len=strlen(buff) ;

	 if(point>0) {
			 point=len-point ;
		       memmove(buff+point+1, buff+point, len-point) ;
			       buff[point]='.' ;
				     size++ ;
		     }
    else if(point<0) {
			point=-point ;
		       memset(buff+len, '0', point) ;
			 size+=point ;
		     }
/*--------------------------------------------------- Дешифровка INT */

  if(type_base=='D' && type_mode=='N') {

   if(type_sign=='U') {  d_intu=strtoul(buff, &end, 10) ;
			   data=&d_intu ;                 }
   else               {  d_int = strtol(buff, &end, 10) ;
			   data=&d_int  ;                 }

		     data_size=sizeof(int)  ;
				       }
/*------------------------------------------------- Дешифровка SHORT */

  else
  if(type_base=='D' && type_mode=='N') {

   if(type_sign=='U') {  d_shortu=(unsigned short)
                                   strtoul(buff, &end, 10) ;
			     data=&d_shortu ;               }
   else               {  d_short =(short) 
                                   strtol(buff, &end, 10) ;
			     data=&d_short  ;               }

		     data_size=sizeof(short)  ;
				       }
/*-------------------------------------------------- Дешифровка LONG */

  else
  if(type_base=='D' && type_mode=='L') {

   if(type_sign=='U') {  d_longu=strtoul(buff, &end, 10) ;
			    data=&d_longu ;                }
   else               {  d_long = strtol(buff, &end, 10) ;
			    data=&d_long ;                 }

		      data_size=sizeof(long)  ;
				       }
/*------------------------------------------------- Дешифровка FLOAT */

  else
  if(type_base=='F' && type_mode!='L') {

			d_float=(float)strtod(buff, &end) ;
			   data=&d_float ;
		      data_size=sizeof(float)  ;
				       }
/*------------------------------------------------ Дешифровка DOUBLE */

  else
  if(type_base=='F' && type_mode=='L') {

		       d_double=strtod(buff, &end) ;
			   data=&d_double ;
		      data_size=sizeof(double)  ;
				       }
/*------------------------------------------- НЕИЗВЕСТНАЯ дешифровка */
  else                                 {
                                          dcl_errno=SE_BADFORMAT ;
                                           return(&void_return) ;
				       }
/*-------------------------------------------- Сброс данных на выход */

   if(!scan_flag && end!=buff+size) {  dcl_errno=SE_BADSTRING ;     /* Контроль полноты дешифровки */
					return(&void_return) ;  }

	  type2=t_base(pars[2]->type) ;                             /* Извлекаем базовый тип */

			    data_buff =pars[2]->buff ;              /* Опр.располагаемый размер */
    if(type2==_DGT_VAL ||                                           /*  буфера данных в байтах  */
       type2==_DGT_PTR ||
       type2==_DGT_AREA  )  data_buff*=Kernel->iDgt_size(pars[2]->type) ;

   if(data_buff<data_size) {  dcl_errno=SE_DATABUFF ;               /* Контроль буфера данных */
				return(&void_return) ;  }

		      memcpy(pars[2]->addr, data, data_size) ;      /* Копируем в буфер данных */

    if(type2!=_DGT_VAL &&                                           /* Для бинарных переменных */
       type2!=_DGT_PTR &&                                           /*    уст.размер обьекта   */
       type2!=_DGT_AREA  )  pars[2]->size=data_size ;

		      len =end-buff ;
	   chr_return.addr=(char *)pars[1]->addr+len ;
	   chr_return.buff=        pars[1]->buff-len ;
	   chr_return.size=        pars[1]->size-len ;

/*-------------------------------------------------------------------*/

		    Kernel->iSize_correct(pars[2]) ;                /* Проводим следящую коррекцию     */
								    /*   размеров указателей на обьект */
          dcl_errno=0 ;

  return(&chr_return) ;
}


/*****************************************************************/
/*                                                               */
/*                 Преобразование данных в строку                */
/*                                                               */
/*           char *dtos(format, string, data) ;                  */
/*                                                               */
/*              char *format  - формат преобразования            */
/*              char *string  - буфер строки                     */
/*              ...   data    - преобразуемые данные             */
/*                                                               */
/*  Структура формата: [+][pref_chr]%[u][pref]type<area>[.point] */
/*                                                               */
/*        <area> - длина 'поля вывода'                           */
/*          type - базовый тип:  d -  int/short/long             */
/*                               f -  float/double (F-формат) ;  */
/*          pref - префикс типа: h -  short                      */
/*                               l -  long/double      ;         */
/*             u - спецификатор unsigned ;                       */
/*         point - число знаков после запятой -                  */
/*                   если указано отрицательное число, то        */
/*                    запятая опускается ;                       */
/*      pref_chr - префикс символ,                               */
/*                так для строк  ............444444              */
/*                           и  -............444444              */
/*                              префикс символ будет '.' ;       */
/*             + - признак вынесения знака перед префикс-        */
/*                     символом.                                 */
/*                                                               */
/*   Возвращает указатель на внутренний буфер результата.        */
/*                                                               */
/*   Код ошибки:                                                 */
/*        SE_BADFORMAT  -  некорректный формат                   */
/*        SE_STRSIZE    -  слишком короткий буфер для строки     */
/*        SE_BADSTRING  -  строка не умещается в поле вывода     */
/*        SE_DATABUFF   -  размер буфера данных не соотв. типу   */

    Dcl_decl *Dcl_dtos(Lang_DCL *Kernel, Dcl_decl **pars, int  pars_cnt)

{
	 int  pref_flag ;  /* Флаг префикса */
	 int  pref_char ;  /* Префикс-символ */
	 int  sign_flag ;  /* Флаг вынесения знака */
	char  type_base ;  /* Базовый тип: D - целое, F - плавающее(F-формат) */
	char  type_mode ;  /* Модификатор типа: H - short, L - long/double */
	char  type_sign ;  /* Модификатор знака: S - signed, U - unsigned */
	 int  point ;      /* Положение точки */
	 int  point_flag ; /* Флаг наличия разделителя '.' в формате */
	 int  data_size ;  /* Необходимый размер буфера данных */
	char *addr ;
	 int  area ;       /* Размер поля вывода */
	 int  dec ;        /* Положение запятой после FCVT */
	 int  sign ;       /* Код знака после FCVT */
 static char *sign_char="+-" ;
	 int  shift ;      /* Длина префикс-зоны */
	 int  len ;

	     int *a_int ;
  unsigned   int *a_intu ;
	    long *a_long ;
  unsigned  long *a_longu ;
	   short *a_short ;
  unsigned short *a_shortu ;
	   float *a_float ;
	  double *a_double ;
	    long  d_long ;
  unsigned  long  d_longu ;
	  double  d_double ;

/*------------------------------------------------- Входной контроль */

  if(pars_cnt      <  3  ||
     pars[0]->addr==NULL ||
     pars[1]->addr==NULL ||
     pars[2]->addr==NULL   )  return(&void_return) ;

/*---------------------------------------------------- Инициализация */

                           len=0 ;
                          sign=0 ;
                     pref_char=0 ;

/*----------------------------------------------- Дешифровка формата */

      if(pars[0]->size>=_BUFF_SIZE) {  dcl_errno=SE_BADFORMAT ;
					  return(&void_return) ;  }

		 addr=(char *)pars[0]->addr ;
/*- - - - - - - - - - - - - - - - - Контроль символа вынесения знака */
       if(addr[0]=='+') {  sign_flag= 1 ;                           /* Если задан режим вынесения знака - */
				addr++ ;       }                    /*   фиксируем его и проходим дальше  */
       else                sign_flag= 0 ;
/*- - - - - - - - - - - - - - - - - - - - - - Анализ префикс-символа */
       if(addr[0]!='%') {  pref_flag= 1 ;                           /* Если задан префикс-символ -         */
			   pref_char=addr[0] ;                      /*  выделяем его, уст. флаг наличия    */
				addr++ ;       }                    /*   префикс-символа и проходим дальше */
       else                pref_flag= 0 ;
/*- - - - - - - - - - - - - - - - - - - - Подготовка анализа формата */
      memset(buff, 0, _BUFF_SIZE) ;
      memcpy(buff, addr, pars[0]->size-pref_flag) ;                 /* Заносим формат в рабочий массив */
      strupr(buff) ;                                                /* Переводим формат в верхний регистр */

       if(buff[0]!='%') {  dcl_errno=SE_BADFORMAT ;                 /* Если отсутствует идентиф.символ */
			      return(&void_return) ;  }
/*- - - - - - - - - - - - - - - - - - - - - - -  Установка умолчаний */
		   type_base= 0 ;                                   /* Установка имолчаний */
		   type_mode='N' ;
		   type_sign='S' ;
		       point= 0 ;
		  point_flag= 0 ;
/*- - - - - - - - - - - - - - - - - - - - - - - - Дешифровка формата */
	  addr=buff+1 ;

      if(*addr=='U'              ) {  type_sign=addr[0] ;  addr++ ;  }
      if(*addr=='H' || *addr=='L') {  type_mode=addr[0] ;  addr++ ;  }
      if(*addr=='D' || *addr=='F') {  type_base=addr[0] ;  addr++ ;  }

			 area=strtol(addr, &addr, 10) ;

      if(*addr=='.') {
			point_flag= 1 ;
			point     =strtol(addr+1, &addr, 10) ;
		     }
/*- - - - - - - - - - - - - - - - - -  Анализ результатов дешифровки */
      if( *addr   !=0 ||                                            /* Если формат не полностью       */
	 type_base==0   ) {  dcl_errno=SE_BADFORMAT ;               /*  дешифрован или в нем не задан */
			      return(&void_return) ;  }             /*   базовый тип ...              */

      if(     area < 1 ||                                           /* Если не задано поле вывода */
	 abs(point)>20   ) {  dcl_errno=SE_BADFORMAT ;              /*  или задано слишком много  */
			       return(&void_return) ;  }            /*   цифр после запятой ...   */

/*----------------------------------------------- Разведение адресов */

	   a_int   =(           int *)pars[2]->addr ;
	   a_intu  =(unsigned   int *)pars[2]->addr ;
	   a_long  =(          long *)pars[2]->addr ;
	   a_longu =(unsigned  long *)pars[2]->addr ;
	   a_short =(         short *)pars[2]->addr ;
	   a_shortu=(unsigned short *)pars[2]->addr ;
	   a_float =(         float *)pars[2]->addr ;
	   a_double=(        double *)pars[2]->addr ;

/*--------------------------------- Преобразование INT, SHORT и LONG */

  if(type_base=='D') {                                              /* IF.1 */

	    if(type_mode=='N') {    d_long =*a_int ;                /* Извлекаем содержимое из буфера */
				    d_longu=*a_intu ;               /*   данных в рабочую ячейку в    */
				  data_size=sizeof(int) ;  }        /*    соответствии с типом данных */
       else if(type_mode=='H') {    d_long =*a_short ;
				    d_longu=*a_shortu ;
				  data_size=sizeof(short) ;  }
       else                    {    d_long =*a_long ;
				    d_longu=*a_longu ;
				  data_size=sizeof(long) ;  }

	 if(type_sign=='U')  sprintf(buff, "%lu", d_longu) ;        /* Проводим знаковое или беззнаковое */
	 else                sprintf(buff, "%ld", d_long) ;         /*   конвертирование в строку        */

       if(sign_flag)                                                /* Если задано вынесение знака -  */
	 if(buff[0]=='-') {  strcpy(buff, buff+1) ;                 /*  уст.код знака и убираем       */
			       sign=1 ;             }               /*   минус у отрицательного числа */
	 else                  sign=0 ;

			  len=strlen(buff) ;
		     }                                              /* END.1 */
/*------------------------- Преобразование FLOAT и DOUBLE в формат F */

  else
  if(type_base=='F') {                                              /* IF.2 */

	    if(type_mode=='N') {   d_double=*a_float ;              /* Извлекаем содержимое из буфера */
				  data_size=sizeof(float) ;  }      /*   данных в рабочую ячейку в    */
	    else               {   d_double=*a_double ;             /*    соответствии с типом данных */
				  data_size=sizeof(double) ;  }

			  len=abs(point) ;                          /* Уст.длину 'после запятой' */

		addr=fcvt(d_double, len, &dec, &sign) ;             /* Первичная конвертация */
		   strcpy(buff, addr) ;                             /* Копируем результат в рабочий буфер */
	       len=strlen(buff) ;                                   /* Опр.длину строки */

	if(dec<1) {                                                 /* Если число <1, то           */
		     memmove(buff-dec+1, buff, len+1) ;             /*  добавляем недостающие нули */
		      memset(buff, '0', -dec+1) ;                   /*   спереди                   */
			 len+=(-dec+1) ;
			 dec = 1 ;
		  }

	if(point_flag && point>=0) {                                /* Если надо ставить точку - */
		       memmove(buff+dec+1, buff+dec, len-dec+1) ;   /*  вставляем ее             */
				    buff[dec]='.' ;
					 len++ ;
				   }

	if(sign && !sign_flag) {                                    /* Если числи отрицательное */
		     memmove(buff+1, buff, len+1) ;                 /*  и знак не выносится -   */
				     buff[0]='-' ;                  /*   вставляем знак минус   */
				      len++ ;
			       }
		     }                                              /* END.2 */
/*--------------------------------------- Неизвестное преобразование */

  else               {
                           dcl_errno=SE_BADFORMAT ;
                              return(&void_return) ;
                     }
/*----------------------------------- Контроль размера буфера данных */

// if(data_size>pars[2]->size) {  dcl_errno=SE_DATABUFF ;           /* Контроль размера буфера данных ... */
//                                  return(&void_return) ;  }

/*---------------------------------------- Обработка префикс-символа */

      if(len+sign*sign_flag>area) {   dcl_errno=SE_BADSTRING ;      /* Если строка не умещается */
				       return(&void_return) ;  }    /*   в поле вывода ...      */

    if(pref_flag) {                                                 /* Если задано расширение  */
			shift=area-len-sign_flag ;                  /*   префикс-символом ...  */
		      memmove(buff+shift, buff, len+1) ;
		       memset(buff, pref_char, shift) ;
			 len+=shift ;
		  }
/*-------------------------------------------------- Вынесение знака */

    if(sign_flag) {
		       memmove(buff+1, buff, len+1) ;
			       buff[0]=sign_char[sign] ;
				len++ ;
		  }
/*-------------------------------------------- Сброс данных на выход */

   if(pars[1]->buff<len) {  dcl_errno=SE_STRSIZE ;                  /* Контроль размера выходного буфера */
			     return(&void_return) ;  }

	    memcpy(pars[1]->addr, buff, len) ;
		   pars[1]->size=len ;

		    Kernel->iSize_correct(pars[1]) ;                /* Проводим следящую коррекцию     */
								    /*   размеров указателей на обьект */
	   chr_return.addr= buff ;
	   chr_return.buff=_BUFF_SIZE ;
	   chr_return.size= len ;

/*-------------------------------------------------------------------*/

          dcl_errno=0 ;

  return(&chr_return) ;
}


/*****************************************************************/
/*                                                               */
/*             Преобразование данных разных форматов             */
/*                                                               */
/*           void  dtod(format, data_in, data_out) ;             */
/*                                                               */
/*              char *format   - формат преобразования           */
/*              ...   data_in  - преобразуемые данные            */
/*              ...   data_out - преобразованные данные          */
/*                                                               */
/*   Структура формата: var_in%var_out ,                         */
/*         где  var_in и var_out - описания входной и выходной   */
/*                                   переменных.                 */
/*    Сруктура описания переменной: [u][pref]type                */
/*                                                               */
/*          type - базовый тип:   d -  int/short/long            */
/*                                f -  float/double   ;          */
/*          pref - префикs типа:  h -  short                     */
/*                                l -  long/double ;             */
/*             u - спецификатор unsigned.                        */
/*                                                               */
/*   Код ошибки:                                                 */
/*        SE_BADFORMAT  -  некорректный формат                   */
/*        SE_DATABUFF   -  размер буфера данных не соотв. типу   */

    Dcl_decl *Dcl_dtod(Lang_DCL *Kernel, Dcl_decl **pars, int  pars_cnt)

{
	char  type_base ;  /* Базовый тип: D - целое, F - плавающее(F-формат) */
	char  type_mode ;  /* Модификатор типа: H - short, L - long/double */
	char  type_sign ;  /* Модификатор знака: S - signed, U - unsigned */
	 int  data_size ;  /* Необходимый размер буфера данных */
	char *addr ;

    union Dcl_types_addr  a_type ;
		  double  d_double ;

/*------------------------------------------------- Входной контроль */

  if(pars_cnt      <  3  ||
     pars[0]->addr==NULL ||
     pars[1]->addr==NULL ||
     pars[2]->addr==NULL   )  return(&void_return) ;

/*--------------------------- Дешифровка формата: входная переменная */

      if(pars[0]->size>=_BUFF_SIZE) {  dcl_errno=SE_BADFORMAT ;
					  return(&void_return) ;  }

	memset(buff, 0, _BUFF_SIZE) ;
	memcpy(buff, pars[0]->addr, pars[0]->size) ;                /* Заносим формат в рабочий массив */
	strupr(buff) ;                                              /* Переводим формат в верхний регистр */

	  addr=buff ;
/*- - - - - - - - - - - - - - - - - - - - - - -  Установка умолчаний */
		   type_base= 0 ;
		   type_mode='N' ;
		   type_sign='S' ;
/*- - - - - - - - - - - - - - - - - - - - - - - - Дешифровка формата */
      if(*addr=='U'              ) {  type_sign=addr[0] ;  addr++ ;  }
      if(*addr=='H' || *addr=='L') {  type_mode=addr[0] ;  addr++ ;  }
      if(*addr=='D' || *addr=='F') {  type_base=addr[0] ;  addr++ ;  }
/*- - - - - - - - - - - - - - - - - -  Анализ результатов дешифровки */
       if(*addr!='%') {  dcl_errno=SE_BADFORMAT ;                   /* Если не обнаружен разделитель ... */
			  return(&void_return) ;  }

      if(type_base==0) {  dcl_errno=SE_BADFORMAT ;                  /* Если не задан базовый тип ... */
			      return(&void_return) ;  }

/*---------------------------------- Входное преобразование к DOUBLE */

		       a_type.a_void=pars[1]->addr ;

  if(type_base=='D') {
	    if(type_mode=='N') {
		   if(type_sign=='U')  d_double=*a_type.a_intu ;
		   else                d_double=*a_type.a_int ;
				      data_size=sizeof(int) ;
			       }
       else if(type_mode=='S') {
		   if(type_sign=='U')  d_double=*a_type.a_shortu ;
		   else                d_double=*a_type.a_short ;
				      data_size=sizeof(short) ;
			       }
       else                    {
		   if(type_sign=='U')  d_double=*a_type.a_longu ;
		   else                d_double=*a_type.a_long ;
				      data_size=sizeof(long) ;
			       }
		     }
  else               {
	    if(type_mode=='N') {   d_double=*a_type.a_float ;
				  data_size=sizeof(float) ;    }
	    else               {   d_double=*a_type.a_double ;
				  data_size=sizeof(double) ;   }
		     }

// if(data_size>pars[1]->size) {  dcl_errno=SE_DATABUFF ;           /* Контроль размера буфера данных ... */
//                                  return(&void_return) ;  }

/*-------------------------- Дешифровка формата: выходная переменная */

/*- - - - - - - - - - - - - - - - - - - - - - -  Установка умолчаний */
		   type_base= 0 ;
		   type_mode='N' ;
		   type_sign='S' ;
/*- - - - - - - - - - - - - - - - - - - - - - - - Дешифровка формата */
      if(*addr=='U'              ) {  type_sign=addr[0] ;  addr++ ;  }
      if(*addr=='H' || *addr=='L') {  type_mode=addr[0] ;  addr++ ;  }
      if(*addr=='D' || *addr=='F') {  type_base=addr[0] ;  addr++ ;  }
/*- - - - - - - - - - - - - - - - - -  Анализ результатов дешифровки */
       if(*addr!= 0 ) {  dcl_errno=SE_BADFORMAT ;                   /* Если формат дешифрован не весь ... */
			  return(&void_return) ;  }

      if(type_base==0) {  dcl_errno=SE_BADFORMAT ;                  /* Если не задан базовый тип ... */
			      return(&void_return) ;  }

/*-------------------------------- Выходное преобразование от DOUBLE */

		       a_type.a_void=pars[2]->addr ;

  if(type_base=='D') {
	    if(type_mode=='N') {
	     if(type_sign=='U')  *a_type.a_intu=(unsigned int)(d_double+0.5) ;
	     else                *a_type.a_int =(         int)(d_double+0.5) ;
				      data_size=sizeof(int) ;
			       }
       else if(type_mode=='S') {
	     if(type_sign=='U')  *a_type.a_shortu=(unsigned short)(d_double+0.5) ;
	     else                *a_type.a_short =(         short)(d_double+0.5) ;
					data_size=sizeof(short) ;
			       }
       else                    {
	     if(type_sign=='U')  *a_type.a_longu=(unsigned long)(d_double+0.5) ;
	     else                *a_type.a_long =(         long)(d_double+0.5) ;
				       data_size=sizeof(long) ;
			       }
		     }
  else               {
	   if(type_mode=='N') {  *a_type.a_float =(float)d_double ;
					data_size=sizeof(float) ;  }
	   else               {  *a_type.a_double=d_double ;
					data_size=sizeof(double) ;  }
		     }

// if(data_size>pars[2]->buff) {  dcl_errno=SE_DATABUFF ;           /* Контроль размера буфера данных ... */
//                                  return(&void_return) ;  }

/*----------------------------------------------- Коррекция размеров */

				     pars[2]->size=data_size ;      /* Уст.выходной размер */
               Kernel->iSize_correct(pars[2]) ;                     /* Проводим следящую коррекцию     */
								    /*   размеров указателей на обьект */
/*-------------------------------------------------------------------*/

           dcl_errno=0 ;

  return(&void_return) ;
}


/*****************************************************************/
/*                                                               */
/*    Преобразование данных в строку в соответсвии с типом       */
/*                                                               */
/*           char *аtos(data) ;                                  */
/*                                                               */
/*              ...   data    - преобразуемые данные             */
/*                                                               */
/*    Вид преобразования:                                        */
/*                                                               */
/*   Возвращает указатель на внутренний буфер результата.        */

    Dcl_decl *Dcl_atos(Lang_DCL *Kernel, Dcl_decl **pars, int  pars_cnt)

{
     int  type_base ;

/*------------------------------------------------- Входной контроль */

  if(pars_cnt      <  1  ||
     pars[0]->addr==NULL   )  return(&void_return) ;

/*----------------------------------------------- Разведение адресов */

          type_base=t_base(pars[0]->type) ;

/*----------------------------------- Преобразование одиночных чисел */

  if(type_base==_DGT_VAL) {
                               Kernel->iNumToStr(pars[0], buff) ;
                          } 
/*-------------------------------------------- Сброс данных на выход */

	   chr_return.addr= buff ;
	   chr_return.buff=_BUFF_SIZE ;
	   chr_return.size= strlen(buff) ;

/*-------------------------------------------------------------------*/

  return(&chr_return) ;
}


/*****************************************************************/
/*                                                               */
/*    Преобразование абсолютного времени в строку дата/время     */
/*                                                               */
/*           char *аtot(data) ;                                  */
/*                                                               */
/*              ...   data    - время в секундах с 01.01.1970    */
/*                                                               */
/*    Вид преобразования:                                        */
/*                                                               */
/*   Возвращает указатель на внутренний буфер результата.        */

    Dcl_decl *Dcl_atot(Lang_DCL *Kernel, Dcl_decl **pars, int  pars_cnt)

{
        int  type_base ;
     time_t  time_abs ;
  struct tm *hhmmss ;

/*------------------------------------------------- Входной контроль */

  if(pars_cnt      <  1  ||
     pars[0]->addr==NULL   )  return(&void_return) ;

/*----------------------------------------------- Разведение адресов */

          type_base=t_base(pars[0]->type) ;

/*----------------------------------- Преобразование одиночных чисел */

  if(type_base==_DGT_VAL) {

     time_abs=(time_t)Kernel->iDgt_get(pars[0]->addr, pars[0]->type) ;

                 hhmmss=localtime(&time_abs) ;

           sprintf(buff, "%02d:%02d:%02d %02d.%02d.%04d",
                            hhmmss->tm_hour, hhmmss->tm_min,   hhmmss->tm_sec,
                            hhmmss->tm_mday, hhmmss->tm_mon+1, hhmmss->tm_year+1900) ;
                          } 
/*-------------------------------------------- Сброс данных на выход */

	   chr_return.addr= buff ;
	   chr_return.buff=_BUFF_SIZE ;
	   chr_return.size= strlen(buff) ;

/*-------------------------------------------------------------------*/

  return(&chr_return) ;
}


/*****************************************************************/
/*                                                               */
/*                   Выдача текущего времени и даты              */
/*                                                               */
/*            void  time(data) ;                                 */
/*                                                               */
/*              char *data    - выходные данные                  */
/*                                                               */
/*  Выдает текущее время и дату в формате HH24:MI:SS DD.MM.YYYY  */

    Dcl_decl *Dcl_time(Lang_DCL *Kernel, Dcl_decl **pars, int  pars_cnt)

{
     time_t  time_abs ;
  struct tm *hhmmss ;
       char  data[32] ;   /* Время предыдущего вызова */
        int  size ;

 static  unsigned long  time_value ;
 static       Dcl_decl  time_return={_DCL_ULONG, 0,0,0,"", &time_value, NULL, 1, 1} ;

/*------------------------------------------------- Входной контроль */

  if(pars_cnt      <  1  ||
     pars[0]->addr==NULL   )  return(&void_return) ;

/*---------------------------------------------- Фромирование данных */

               time_abs=     time( NULL) ;
                 hhmmss=localtime(&time_abs) ;

          sprintf(data, "%02d:%02d:%02d %02d.%02d.%04d",
                           hhmmss->tm_hour, hhmmss->tm_min,   hhmmss->tm_sec,
                           hhmmss->tm_mday, hhmmss->tm_mon+1, hhmmss->tm_year+1900) ;

/*----------------------------------------------- Коррекция размеров */

                              size=strlen(data) ;                   /* Контролируем размер выдаваемых данных */
      if(size>pars[0]->buff)  size=pars[0]->buff ;

                    memcpy(pars[0]->addr, data, size) ;             /* Копируем данные */

                           pars[0]->size=size ;                     /* Уст.выходной размер */
     Kernel->iSize_correct(pars[0]) ;                               /* Проводим следящую коррекцию размеров указателей на обьект */

/*-------------------------------------------------------------------*/

          time_value=time_abs ;
  return(&time_return) ;
}


/*****************************************************************/
/*                                                               */
/*                   Выдача временных меток                      */
/*                                                               */
/*      unsigned long  elapsed_time(clear) ;                     */
/*                                                               */
/*              double clear  - Признак сброса отсчета           */

    Dcl_decl *Dcl_elapsed_time(Lang_DCL *Kernel, Dcl_decl **pars, int  pars_cnt)

{
 static         time_t  time_0 ;
 static  unsigned long  time_value ;
 static       Dcl_decl  time_return={_DCL_ULONG, 0,0,0,"", &time_value, NULL, 1, 1} ;
                   int  clear ;


     clear=(int)Kernel->iDgt_get(pars[0]->addr, pars[0]->type) ;    /* 'Извлекаем' признак сброса отсчета */


   if(time_0==0)  time_0    =time(NULL) ;
                  time_value=time(NULL)-time_0 ;

   if(clear    )  time_0    =time(NULL) ;

  return(&time_return) ;
}


/*****************************************************************/
/*                                                               */
/*            Усечение начальных и конечных символов             */
/*                                                               */
/*           char *trim(string[, symbols]) ;                     */
/*                                                               */
/*              char *string  - обрабатываемая строка            */
/*              char *symbols - перечень усекаемых символов      */
/*                               (по умолчанию - пробел)         */
/*                                                               */
/*   Возвращает указатель на результат.                          */

    Dcl_decl *Dcl_trim(Lang_DCL *Kernel, Dcl_decl **pars, int  pars_cnt)

{
	char *trim_list ;      /* Перечень усекаемых символов */
         int  trim_cnt ;  
	char *addr ;
	 int  size ;

/*------------------------------------------------- Входной контроль */

       if(pars_cnt      <  1  ||
          pars[0]->addr==NULL   )  return(&void_return) ;

       if(pars[0]->size==  0    )  return(&void_return) ;

       if(pars_cnt     < 2  ||                                      /* 'Извлекаем' список усекаемых символов */
	  pars[1]->addr==NULL ) {  trim_list=            " " ;
                                   trim_cnt =             1 ;         }
       else                     {  trim_list=(char *)pars[1]->addr ;
                                   trim_cnt =        pars[1]->size ;  }

       if(trim_list==NULL || trim_cnt==0)  return(&void_return) ;

/*---------------------------------------------- Собственно усечение */

       addr=(char *)pars[0]->addr ;
       size=        pars[0]->size ;

     for( ; size>0 ; size--)
       if(memchr(trim_list, addr[size-1], trim_cnt)==NULL)  break ;

     for( ; size>0 ; addr++, size--)
       if(memchr(trim_list, *addr, trim_cnt)==NULL)  break ;

/*------------------------------------------------ Выдача результата */

      if(size>0 && addr!=pars[0]->addr)
          memmove(pars[0]->addr, addr, size) ;

                 pars[0]->size=size ;

                Kernel->iSize_correct(pars[0]) ;                    /* Проводим следящую коррекцию     */
								    /*   размеров указателей на обьект */
/*-------------------------------------------------------------------*/

          chr_return.addr=pars[0]->addr ;
          chr_return.size=pars[0]->size ;
          chr_return.buff=pars[0]->buff ;

  return(&chr_return) ;
}


/*****************************************************************/
/*                                                               */
/*            Замена символьного фрагмента на другой             */
/*                                                               */
/*           void  replace(string, text_1, text_2) ;             */
/*                                                               */
/*              char *string  - обрабатываемая строка            */
/*              char *text_1  - заменяемый фрагмент              */
/*              char *text_2  - замещающий фрагмент              */

    Dcl_decl *Dcl_replace(Lang_DCL *Kernel, Dcl_decl **pars, int  pars_cnt)

{
	char *addr ;
	 int  size ;
	 int  buff ;
	char *text_1 ;
         int  size_1 ;  
	char *text_2 ;
         int  size_2 ;  
         int  i ; 

/*------------------------------------------------- Входной контроль */

  if(pars_cnt      <  3  ||
     pars[0]->addr==NULL ||
     pars[1]->addr==NULL ||
     pars[2]->addr==NULL   )  return(&void_return) ;

  if(pars[0]->size==  0  ||
     pars[1]->size==  0    )  return(&void_return) ;

/*------------------------------------------------------- Подготовка */

       addr  =(char *)pars[0]->addr ;
       size  =        pars[0]->size ;
       buff  =        pars[0]->buff ;
       text_1=(char *)pars[1]->addr ;
       size_1=        pars[1]->size ;
       text_2=(char *)pars[2]->addr ;
       size_2=        pars[2]->size ;

/*------------------------------------------------ Собственно замена */

     for(i=0 ; i<size-size_1+1 && i<buff ; i++)
       if(addr[i]==*text_1)
        if(        size_1==1              || 
           !memcmp(addr+i, text_1, size_1)  ) {

         if(size_1!=size_2) {
            if(i+size_2>buff) {                                     /* При выходе за границы буфера */
                 memmove(addr+i, text_2, i+size_2-buff) ;
                                 size=buff ;
                                    break ;
                              }

                memmove(addr+i+size_2, addr+i+size_1, size-i-size_1) ;
                    size+=size_2-size_1 ;
                            }

         if(size_2>0)  memmove(addr+i, text_2, size_2) ;

                             i+=(size_2-1) ;
                                              }
/*------------------------------------------------ Выдача результата */

      if(size>0 && addr!=pars[0]->addr)
          memmove(pars[0]->addr, addr, size) ;

                 pars[0]->size=size ;

                Kernel->iSize_correct(pars[0]) ;                    /* Проводим следящую коррекцию     */
								    /*   размеров указателей на обьект */
/*-------------------------------------------------------------------*/

  return(&void_return) ;
}


/*****************************************************************/
/*                                                               */
/*            Замена символов из набора на фрагмент              */
/*                                                               */
/*           void  replace_char(string, text_1, text_2) ;        */
/*                                                               */
/*              char *string  - обрабатываемая строка            */
/*              char *text_1  - набор символов                   */
/*              char *text_2  - замещающий фрагмент              */

    Dcl_decl *Dcl_replace_char(Lang_DCL *Kernel, Dcl_decl **pars, int  pars_cnt)

{
	char *addr ;
	 int  size ;
	 int  buff ;
	char *text_1 ;
         int  size_1 ;  
	char *text_2 ;
         int  size_2 ;  
         int  i ; 

/*------------------------------------------------- Входной контроль */

  if(pars_cnt      <  3  ||
     pars[0]->addr==NULL ||
     pars[1]->addr==NULL ||
     pars[2]->addr==NULL   )  return(&void_return) ;

  if(pars[0]->size==  0  ||
     pars[1]->size==  0    )  return(&void_return) ;

/*------------------------------------------------------- Подготовка */

       addr  =(char *)pars[0]->addr ;
       size  =        pars[0]->size ;
       buff  =        pars[0]->buff ;
       text_1=(char *)pars[1]->addr ;
       size_1=        pars[1]->size ;
       text_2=(char *)pars[2]->addr ;
       size_2=        pars[2]->size ;

/*------------------------------------------------ Собственно замена */

     for(i=0 ; i<size && i<buff ; i++)
       if(memchr(text_1, addr[i], size_1)) {

            if(i+size_2>buff) {                                     /* При выходе за границы буфера */
                 memmove(addr+i, text_2, i+size_2-buff) ;
                                 size=buff ;
                                    break ;
                              }

                memmove(addr+i+size_2, addr+i+1, size-i-1) ;
                        size+=size_2-1 ;

         if(size_2>0)  memmove(addr+i, text_2, size_2) ;

                      i+=size_2-1 ;
                                           }
/*------------------------------------------------ Выдача результата */

      if(size>0 && addr!=pars[0]->addr)
          memmove(pars[0]->addr, addr, size) ;

                 pars[0]->size=size ;

                Kernel->iSize_correct(pars[0]) ;                    /* Проводим следящую коррекцию     */
								    /*   размеров указателей на обьект */
/*-------------------------------------------------------------------*/

  return(&void_return) ;
}


/*********************************************************************/
/*                                                                   */
/*         Преобразование форматных строк                            */
/*                                                                   */
/*    int  transpose(text_1, text_2, format_1, format_2, option)     */
/*                                                                   */
/*      char *text_1    -  исходный текст                            */
/*      char *text_2    -  преобразованный текст                     */
/*      char *format_1  -  шаблон исходного текста                   */
/*      char *format_2  -  шаблон преобразованного текста            */
/*      char *options   -  опции преобразования                      */
/*                                                                   */
/*   Опции преобразования:                                           */
/*      FIELD:      - указывает два символа - начала и конца         */
/*                     описателя поля (по-умолчанию - {} )           */
/*      ERROR_RAISE - указывает на эскалацию ошибок на уровень       */
/*                     интерпритатора                                */
/*                                                                   */
/*   Описатели полей шаблонов исходного текста:                      */
/*     {Name}    - по позиционным границам                           */
/*     {Name:20} - по длине                                          */
/*                                                                   */
/*   Описатели полей шаблонов преобразованного текста:               */
/*     {Name}    - простая вставка                                   */
/*                                                                   */
/*   Return:   1 - проведено преобразование                          */
/*             0 - текст не соответствует шаблону                    */
/*            -1 - ошибка обработки                                  */

    Dcl_decl *Dcl_transpose(Lang_DCL *Kernel, Dcl_decl **pars, int  pars_cnt)

{

#define  _FIELDS_MAX  20
    Dcl_decl  fields[_FIELDS_MAX] ;
         int  fields_cnt ;
         int  size ;

/*------------------------------------------------- Входной контроль */

  if(pars_cnt      <  5  ||
     pars[0]->addr==NULL ||
     pars[1]->addr==NULL ||
     pars[2]->addr==NULL ||
     pars[3]->addr==NULL ||
     pars[4]->addr==NULL   ) {         dgt_value=0 ;
                               return(&dgt_return) ;  }

/*------------------------------------------ Разбор исходного текста */

     fields_cnt=Kernel->zParseByTemplate((char *)pars[0]->addr, pars[0]->size,
                                         (char *)pars[2]->addr, pars[2]->size,
                                         (char *)pars[4]->addr, pars[4]->size, 
                                                        fields, _FIELDS_MAX   ) ;
  if(fields_cnt==-1) {
                                dgt_value=-1 ;
                        return(&dgt_return) ;
                     }
  if(fields_cnt==-2) {
                                dgt_value= 0 ;
                        return(&dgt_return) ;
                     }
/*----------------------------- Формирование преобразованного текста */

     size=Kernel->zFormByTemplate((char *)pars[1]->addr, pars[1]->buff,
                                  (char *)pars[3]->addr, pars[3]->size,
                                  (char *)pars[4]->addr, pars[4]->size, 
                                                  fields, fields_cnt     ) ;
  if(size<0) {
                          dgt_value=size ;
                  return(&dgt_return) ;
             }
/*------------------------------------------------ Выдача результата */

                 pars[1]->size=size ;

                Kernel->iSize_correct(pars[1]) ;                    /* Проводим следящую коррекцию     */
								    /*   размеров указателей на обьект */
/*-------------------------------------------------------------------*/

          dgt_value=1 ;
  return(&dgt_return) ;
}


/*****************************************************************/
/*                                                               */
/*            Перевод символов в верхний регистр                 */
/*                                                               */
/*           void  upper(string, codepage) ;                     */
/*                                                               */
/*              char *string   - обрабатываемая строка           */
/*              char *codepage - используемая кодировка          */

    Dcl_decl *Dcl_upper(Lang_DCL *Kernel, Dcl_decl **pars, int  pars_cnt)

{
           char  codepage[32] ;
  unsigned char *coding ;
  unsigned char *addr ;
            int  size ; 
            int  i ; 

/*------------------------------------------------- Входной контроль */

  if(pars_cnt      <  2  ||
     pars[0]->addr==NULL   )  return(&void_return) ;

  if(pars[1]->size==  0    )  return(&void_return) ;

/*-------------------------------- Определение таблицы перекодировки */

                                size=pars[1]->size ;
    if(size>=sizeof(codepage))  size=sizeof(codepage)-1 ;

	  memset(codepage, 0, sizeof(codepage)) ;
	  memcpy(codepage, pars[1]->addr, size) ;                   /* Извлекаем кодировку */

        
         if(!stricmp(codepage, "BASIC"  ))  coding=dcl_char_upper_128 ;
    else if(!stricmp(codepage, "WINDOWS"))  coding=dcl_char_upper_cp1251 ;
    else if(!stricmp(codepage, "CP1251" ))  coding=dcl_char_upper_cp1251 ;
    else                                    coding=dcl_char_basic ;

/*---------------------------------------- Собственно преобразование */

       addr  =(unsigned char *)pars[0]->addr ;
       size  =                 pars[0]->size ;

     for(i=0 ; i<size ; i++)  addr[i]=coding[addr[i]] ;

/*-------------------------------------------------------------------*/

  return(&void_return) ;
}


/*****************************************************************/
/*                                                               */
/*            Пребразование кодовой страницы текста              */
/*                                                               */
/*       void  cp_convert(string, codepage1, codepage2) ;        */
/*                                                               */
/*              char *string    - обрабатываемая строка          */
/*              char *codepage1 - исходная кодировка             */
/*              char *codepage2 - результирующая кодировка       */

    Dcl_decl *Dcl_cp_convert(Lang_DCL *Kernel, Dcl_decl **pars, int  pars_cnt)

{
           char  codepage1[32] ;
           char  codepage2[32] ;
           char *addr ;
            int  size ; 
            int  cnt ; 

/*------------------------------------------------- Входной контроль */

  if(pars_cnt      <  3  ||
     pars[0]->addr==NULL   )  return(&void_return) ;

  if(pars[1]->size==  0    )  return(&void_return) ;
  if(pars[2]->size==  0    )  return(&void_return) ;

/*-------------------------------- Определение таблицы перекодировки */

                                 size=pars[1]->size ;
    if(size>=sizeof(codepage1))  size=sizeof(codepage1)-1 ;

	  memset(codepage1, 0, sizeof(codepage1)) ;
	  memcpy(codepage1, pars[1]->addr, size) ;                   /* Извлекаем исходную кодировку */

                                 size=pars[2]->size ;
    if(size>=sizeof(codepage2))  size=sizeof(codepage2)-1 ;

	  memset(codepage2, 0, sizeof(codepage2)) ;
	  memcpy(codepage2, pars[2]->addr, size) ;                   /* Извлекаем результирующую кодировку */
        
/*---------------------------------------- Собственно преобразование */

                               addr=(char *)pars[0]->addr ;
                               size=        pars[0]->size ;

      cnt=Kernel->zCodePageConvert(addr, size, codepage1, codepage2) ;
   if(cnt!=size) {
                                      pars[0]->size=cnt ;
                Kernel->iSize_correct(pars[0]) ;                    /* Проводим следящую коррекцию     */
								    /*   размеров указателей на обьект */
                 }
/*-------------------------------------------------------------------*/

  return(&void_return) ;
}


/*****************************************************************/
/*                                                               */
/*                 Исполнение SHELL-команды                      */
/*                                                               */
/*        double  system(command) ;                              */
/*                                                               */
/*              char *command - Исполняемая команда              */
/*                                                               */
/*     Вызов п/п адекватен вызову:  system(command)              */
/*                                                               */
/*   Возвращает:  0  - все нормально                             */
/*               -1  - ошибка запуска см.errno                   */
/*               >0  - код возврата при исполнении               */

    Dcl_decl *Dcl_system(Lang_DCL *Kernel, Dcl_decl **pars, int  pars_cnt)

{
	char  command[2048] ;
	 int  size ;
	 int  status ;
         int  i ;
         int  j ;

/*------------------------------------------------- Входной контроль */

  if(pars_cnt      <  1  ||
     pars[0]->addr==NULL   ) {
                                        dcl_errno=ENOENT ;
                                        dgt_value=-1. ;
                                return(&dgt_return) ;
                             }
/*------------------------------------------------------- Подготовка */

                                 size=pars[0]->size ;
      if(size>=sizeof(command))  size=sizeof(command)-1 ;

           memset(command, 0, sizeof(command)) ;
           memcpy(command, pars[0]->addr, size) ;

   for(i=0, j=0 ; command[i]!=0 ; i++) {                            /* Обработка спец-символов - */
                                                                    /*  - удаляем конец строки   */
       if(command[i]=='\r')  continue ;
       if(command[i]=='\n')  continue ;
       if(command[i]=='\t')  command[i]=' ' ;

          command[j]=command[i] ;
                  j++ ;
                                       }
/*------------------------------------------------------- Исполнение */


              status=system(command) ;
           dgt_value=status ;

           if(status<0)  dcl_errno=errno ;
           else          dcl_errno=  0. ;

/*-------------------------------------------------------------------*/

  return(&dgt_return) ;
}


/*****************************************************************/
/*                                                               */
/*                Отработка паузы                                */
/*                                                               */
/*            void  sleep(pause) ;                               */
/*                                                               */
/*              double  pause  - Пауза в секундах                */

    Dcl_decl *Dcl_sleep(Lang_DCL *Kernel, Dcl_decl **pars, int  pars_cnt)

{
  int  pause ;


     pause=(int)Kernel->iDgt_get(pars[0]->addr, pars[0]->type) ;    /* 'Извлекаем' длительность паузы */

#ifdef UNIX
                sleep(pause) ;
#else
                Sleep(pause*1000) ;
#endif

  return(NULL) ;
}


/*****************************************************************/
/*                                                               */
/*                  Запись данных в лог-файл                     */
/*                                                               */
/*        double  log_file(path, text) ;                         */
/*                                                               */
/*              char *path - Путь к файлу лога                   */
/*              char *text - Записываемые данные                 */
/*                                                               */
/*   Возвращает:  0  - все нормально                             */
/*               -1  - ошибка запуска см.errno                   */

    Dcl_decl *Dcl_log_file(Lang_DCL *Kernel, Dcl_decl **pars, int  pars_cnt)

{
       time_t  time_abs ;
    struct tm *hhmmss ;
         char  path[1024] ;
         FILE *file ;
         char  prefix[1024] ;
          int  size ;

/*------------------------------------------------- Входной контроль */

                                        dgt_value=0 ;

  if(pars_cnt      <  1  ||
     pars[0]->addr==NULL   ) {
                                        dcl_errno=ENOENT ;
                                        dgt_value=-1. ;
                                return(&dgt_return) ;
                             }
/*------------------------------------- Формирование временной метки */

                time_abs=     time( NULL) ;
                  hhmmss=localtime(&time_abs) ;
        
        sprintf(prefix, "%02d/%02d/%02d %02d:%02d:%02d ",
                             hhmmss->tm_mday,
                             hhmmss->tm_mon+1,
                             hhmmss->tm_year-100,
                             hhmmss->tm_hour,
                             hhmmss->tm_min,    
                             hhmmss->tm_sec ) ;

/*---------------------------------------------- Открытие файла лога */

                             size=pars[0]->size ;
     if(size>=sizeof(path))  size=sizeof(path)-1 ;

            memset(path, 0, sizeof(path)) ;
            memcpy(path, pars[0]->addr, size) ;

     if(*path==0) {
                             dcl_errno=ENOENT ;
                             dgt_value=-1. ;
                     return(&dgt_return) ;
                  }

        file=fopen(path, "ab") ;
     if(file==NULL) {
                               dcl_errno=errno ;
                               dgt_value=-1. ;
                       return(&dgt_return) ;
                    }  
/*------------------------------------------------- Запись сообщения */

        fwrite(prefix, 1, strlen(prefix), file) ;

   if(pars[1]->addr!=NULL && 
      pars[1]->size!=  0    ) 
        fwrite(pars[1]->addr, 1, pars[1]->size, file) ;

        fwrite("\n", 1, strlen("\n"), file) ;

/*---------------------------------------------- Закрытие файла лога */

         fclose(file) ;	

/*-------------------------------------------------------------------*/

  return(&dgt_return) ;
}


/*****************************************************************/
/*                                                               */
/*              Запись данных в системный лог событий            */
/*                                                               */
/*        double  log_event(journal, source, category, text) ;   */
/*                                                               */
/*     char *journal  - Имя журнала событий                      */
/*     char *source   - Имя источника событий                    */
/*     char *category - Категория события: ERROR, WARNING, INFO  */
/*     char *text     - Текст сообщения                          */
/*                                                               */
/*   Возвращает:  0  - все нормально                             */
/*               -1  - ошибка запуска см.errno                   */

    Dcl_decl *Dcl_log_event(Lang_DCL *Kernel, Dcl_decl **pars, int  pars_cnt)

{
         char  journal[128] ;
         char  source[128] ;
         char  category[128] ;
         char  text[2048] ;
         char *text_ptr ;
         char  key_path[1024] ;
          int  size ;

#ifdef UNIX
#else
         HKEY  hKey ;
         LONG  result ;
        DWORD  status ;
       HANDLE  hSrc ; 
         WORD  type ;
          int  log_created ;        /* Признак создания журнала */
#endif

/*------------------------------------------------- Входной контроль */

                                            dcl_errno=0 ;
                                            dgt_value=0. ;

  if(pars_cnt      <  4  ||
     pars[0]->addr==NULL ||
     pars[1]->addr==NULL ||
     pars[2]->addr==NULL ||
     pars[3]->addr==NULL   ) {
                                       dcl_errno=EINVAL ;
                                       dgt_value=-1. ;
                               return(&dgt_return) ;
                             }
/*-------------------------------------------- Извлечение параметров */

#define EXTRACT(par, buffer)                              size=pars[par]->size ;    \
                                if(size>=sizeof(buffer))  size=sizeof(buffer)-1 ;   \
                                           memset(buffer, 0, sizeof(buffer)) ;      \
                                           memcpy(buffer, pars[par]->addr, size) ;

    EXTRACT(0, journal ) ;
    EXTRACT(1, source  ) ;
    EXTRACT(2, category) ;
    EXTRACT(3, text    ) ;

#undef  EXTRACT

  if(journal [0]==0 ||
     source  [0]==0 ||
     category[0]==0 ||
     text    [0]==0   ) {
                                   dcl_errno=EINVAL ;
                                   dgt_value=-1. ;
                           return(&dgt_return) ;
                        }
/*------------------------------------------------------ Для Windows */

#ifndef  UNIX
/*- - - - - - - - - - - - - - - - - - - - - - - - - Управляющий блок */
            log_created=0 ;

  do {                                                              /* BLOCK */
/*- - - - - - - - - - - - - - - - - - - -  Запись сообщения в журнал */
     do {

            if(!stricmp(category, "WARNING"))  type=EVENTLOG_WARNING_TYPE ;
       else if(!stricmp(category, "INFO"   ))  type=EVENTLOG_INFORMATION_TYPE ;
       else                                    type=EVENTLOG_ERROR_TYPE ;

          hSrc=RegisterEventSource(NULL, source) ;                  /* "Открываем" журнал */
       if(hSrc==NULL) {                                             /* Если ошибка открытия журнала -                     */
        if(log_created) {                                           /*  идем либо на создание журнала,                    */
                                  dcl_errno =GetLastError() ;       /*  либо (если мы его уже создали) выходим с ошибкой  */
                                  dgt_value = -1. ;
                          return(&dgt_return) ;
                        }
        else                  break ;
                      }

                        text_ptr=text ;

        status=ReportEvent(hSrc, type, 0, 0, NULL, 1, strlen(text), /* Пишем событие */
                                   (const char **)&text_ptr, text  ) ;

         DeregisterEventSource(hSrc);                               /* "Закрываем" журнал */
    
      if(status==0) {                                               /* Если ошибка записи события... */
                              dcl_errno =GetLastError() ;
                              dgt_value = -1. ;
                    }

                      return(&dgt_return) ;
        } while(0) ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - Создание журнала */
   do {                                                             /* BLOCK */
                                    hKey=NULL ;

        sprintf(key_path, "SYSTEM\\CurrentControlSet\\Services\\EventLog\\%s", journal) ;

       result=RegOpenKeyEx(HKEY_LOCAL_MACHINE, key_path,            /* Попытка открыть существующий журнал */
                              0, KEY_SET_VALUE, &hKey)  ;

    if(result!=ERROR_SUCCESS) {

         result=RegCreateKeyEx(HKEY_LOCAL_MACHINE, key_path,        /* Регистрация журнала */
                                0, NULL, REG_OPTION_NON_VOLATILE,
                                 KEY_WRITE, NULL, &hKey, &status )  ;
      if(result!=ERROR_SUCCESS)  break ;

         result=RegSetValueEx(hKey, "Sources",                      /* Создаем название источников */
                               0, REG_MULTI_SZ,             
                              (LPBYTE)source, (DWORD) strlen(source)+1) ;

      if(result!=ERROR_SUCCESS)  break ;
                              }

                  RegCloseKey(hKey) ;                               /* Закрываем журнал */

        sprintf(key_path, "SYSTEM\\CurrentControlSet\\Services\\EventLog\\%s\\%s", journal, source) ;

       result=RegOpenKeyEx(HKEY_LOCAL_MACHINE, key_path,            /* Попытка открыть существующий источник */
                              0, KEY_SET_VALUE, &hKey)  ;
    if(result!=ERROR_SUCCESS) {
 
         result=RegCreateKeyEx(HKEY_LOCAL_MACHINE, key_path,        /* Создаем источник в реестре системы */
                                0, NULL, REG_OPTION_NON_VOLATILE,
                                 KEY_SET_VALUE, NULL, &hKey, &status )  ;
      if(result!=ERROR_SUCCESS)  break ;
         result = 0 ;

         status = EVENTLOG_ERROR_TYPE      |                          
                  EVENTLOG_WARNING_TYPE    |
                  EVENTLOG_INFORMATION_TYPE ; 
         result|=RegSetValueEx(hKey, "TypesSupported",              /* Прописываем поддерживаемые типы сообщений */  
                                 0, REG_DWORD,         
                                 (LPBYTE)&status, sizeof(DWORD)) ;
                              }

                RegCloseKey(hKey) ;                                 /* Закрываем источник */

      } while(0) ;                                                  /* BLOCK */

    if(result!=ERROR_SUCCESS) {                                     /* Если при создании/открытии журнала была ошибка */
                                   dcl_errno =result ;
                                   dgt_value = -1. ;
                           return(&dgt_return) ;
                              }

                            log_created=1 ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - Управляющий блок */
     } while(1) ;                                                   /* BLOCK */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
#endif

/*-------------------------------------------------------------------*/

  return(&dgt_return) ;
}


/*****************************************************************/
/*                                                               */
/*                  Запись данных в лог-файл                     */
/*                                                               */
/*        double  signal(type, address, status, info) ;          */
/*                                                               */
/*          char *type     -  Тип сигнала: ABTP                  */
/*          char *address  -  Адресат                            */
/*          char *signal   -  Имя сигнала                        */
/*          char *value    -  Значение сигнала                   */
/*          char *info     -  Информация по сигналу              */
/*                                                               */
/*   Возвращает:  0  - все нормально                             */
/*               -1  - ошибка см.errno                           */

    Dcl_decl *Dcl_signal(Lang_DCL *Kernel, Dcl_decl **pars, int  pars_cnt)

{
         char  address[FILENAME_MAX] ;   /*  */
         char  signal[128] ; 
         char  value[128] ; 
         char *info ; 
          int  size ;                    /* Размер считывания */

   static   double  ret_value ;          /* Буфер числового значения */
   static Dcl_decl  dgt_return={_DGT_VAL, 0, 0, 0, "", &ret_value, NULL, 1, 1} ;

/*------------------------------------------------- Входной контроль */

                                            dcl_errno= 0 ; 

  if(pars_cnt      <  5  ||
     pars[0]->addr==NULL ||
     pars[1]->addr==NULL ||
     pars[2]->addr==NULL ||
     pars[3]->addr==NULL ||
     pars[4]->addr==NULL   ) {
                                            dcl_errno=EINVAL ;
                                            ret_value=-1. ;
                                    return(&dgt_return) ;
                             }
/*-------------------------------------------- Извлечение параметров */

                                  size=pars[1]->size ;
       if(size>=sizeof(address))  size=sizeof(address)-1 ;

	  memset(address, 0, sizeof(address)) ;
	  memcpy(address, pars[1]->addr, size) ;

                                 size=pars[2]->size ;
       if(size>=sizeof(signal))  size=sizeof(signal)-1 ;

	  memset(signal, 0, sizeof(signal)) ;
	  memcpy(signal, pars[2]->addr, size) ;

                                size=pars[3]->size ;
       if(size>=sizeof(value))  size=sizeof(value)-1 ;

	  memset(value, 0, sizeof(value)) ;
	  memcpy(value, pars[3]->addr, size) ;

                 info=(char *)pars[4]->addr ;
                 size=        pars[4]->size ;

/*-------------------------------------------- Отправка ABTP-сигнала */

    if(!memcmp(pars[0]->addr, "ABTP", pars[0]->size) && 
               pars[0]->size==strlen("ABTP")           ) {

       if(signal[0]==0)  strcpy(signal, "NULL") ;

            ret_value=Kernel->zAbtpSignal(address, signal, value, info, size) ;
                                                         }
/*------------------------------------------ Неизвестный вид сигнала */
    else                                                 {

          strcpy(Kernel->mError_details, "Unknown signal type") ;
                              dcl_errno=_DCLE_CALL_INSIDE ;
                              ret_value=-1. ;
                      return(&dgt_return) ;

                                                         }
/*-------------------------------------------------------------------*/

  return(&dgt_return) ;
}

/*****************************************************************/
/*                                                               */
/*                    Вычисление md5-хэша строки                 */
/*                                                               */
/*  unsigned char *md5(string[, notation]) ;                     */
/*                                                               */
/*     char *string  - обрабатываемая строка                     */
/*     char *notation - способ представления хэша                */
/*          Возможные значения:                                  */
/*          "16" - в шестнадцатеричной системе счисления;        */
/*          "2"  - в двоичной системе счисления;                 */
/*
/*   Возвращает указатель на результат.                          */


Dcl_decl *Dcl_md5(Lang_DCL *Kernel, Dcl_decl **pars, int  pars_cnt)
{
	md5_processor proc;
	int size_digest;
	unsigned char *digest;
	unsigned char *digest_format;
	unsigned int size_input;
	unsigned char *input;
	int flag;


/*------------------------------------------------- Входной контроль */

	if(	pars_cnt < 1 || pars[0]->addr==NULL	)  
		return( &void_return ) ;

/*------------------------------------------------- Инициализация локальных переменных */

	memset( &proc, 0, sizeof( md5_processor ) ) ;
	md5_initialization( &proc ) ;
	
	size_digest = 17;
	digest = new unsigned char [ size_digest ];
	memset( digest, 0, sizeof(digest) ) ;
	
	size_input = ( pars[0]->size ) ;
	input = new unsigned char [ size_input + 1 ] ;
	memcpy( input, pars[0]->addr, (size_input+1) );

	digest_format = new unsigned char [_MD5_BUFF_SIZE];
        memset( digest_format, 0, sizeof(digest_format) ) ;
	
	if ( pars[1]->addr==NULL )
		flag = 0;
	else
		flag = atoi((char *)(pars[1]->addr));
	
/*------------------------------------------------- Хэширование данных */

        md5_start( &proc, input, size_input );	
	md5_finish( &proc, digest );	
	digest[ size_digest - 1 ] = '\0';
	
	md5_format( digest, digest_format, flag );

	memset( &proc, 0, sizeof(md5_processor) ) ;

	chr_return.addr = digest_format ;
	chr_return.buff = ( strlen((char *)digest_format) + 1) ;
	chr_return.size = ( strlen((char *)digest_format) + 1) ;
	return(&chr_return) ;
}


/*****************************************************************/
/*                                                               */
/*                    Вычисление md5-хэша данных из файла        */
/*                                                               */
/*  unsigned char *f_md5(path[, notation]) ;                     */
/*                                                               */
/*     char *path  - полный путь к фйлу                          */
/*     char *notation - способ представления хэша                */
/*          Возможные значения:                                  */
/*          "16" - в шестнадцатеричной системе счисления;        */
/*          "2"  - в двоичной системе счисления;                 */
/*
/*   Возвращает указатель на результат.                          */

Dcl_decl *Dcl_f_md5(Lang_DCL *Kernel, Dcl_decl **pars, int  pars_cnt)
{
	FILE *input_file;   
	size_t size_file;
        char *path;
	md5_processor proc; 
	int size_digest;
	unsigned char *digest;
	unsigned char *digest_format;
	unsigned int size_input;
	unsigned char *input;
	int flag;

/*------------------------------------------------- Входной контроль */

	if(	pars_cnt < 1 || pars[0]->addr==NULL	)
		return( &void_return ) ;

/*------------------------------------------------- Инициализация локальных переменных */

	memset( &proc, 0, sizeof( md5_processor ) ) ;
	md5_initialization( &proc ) ;

	size_digest = 17;
	digest = new unsigned char [ size_digest ];
	memset( digest, 0, sizeof(digest) ) ;
	
	size_input = 512 ;
	input = new unsigned char [ size_input ];

	digest_format = new unsigned char [_MD5_BUFF_SIZE];
        memset( digest_format, 0, sizeof(digest_format) ) ;

        input_file = NULL;

	if ( pars[1]->addr==NULL )
		flag = 0;
	else
		flag = atoi((char *)(pars[1]->addr));

        path = new char[ pars[0]->size + 1 ];
        memcpy(path,pars[0]->addr, pars[0]->size);
        path[ pars[0]->size ] = 0;

/*------------------------------------------------- Открытие файла для чтения */

	if( ( input_file = fopen( path, "rb" ) ) == NULL )
            return( &void_return );

/*------------------------------------------------- Хэширование данных из файла */

	while( ( size_file = fread( input, 1, sizeof(input), input_file ) ) > 0 )
		md5_start( &proc, input, (int)size_file );

	md5_finish( &proc, digest );
	digest[ size_digest - 1 ] = '\0';

	md5_format( digest, digest_format, flag );

	memset( &proc, 0, sizeof( md5_processor ) );
        fclose( input_file );

	chr_return.addr = digest_format ;
	chr_return.buff = ( strlen((char *)digest_format) + 1 ) ;
	chr_return.size = ( strlen((char *)digest_format) + 1 ) ;
	
	return(&chr_return) ;
}


