/*********************************************************************/
/*                                                                   */
/*                   DATA CONVERTION LANGUAGE                        */
/*                                                                   */
/*                 БИБЛИОТЕКА ОТЛАДОЧНЫХ ФУНКЦИЙ                     */
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


#define DCL_INSIDE

#include "dcl.h"


#pragma warning(disable : 4996)

/*------------------------------------------------ "Внешние" объекты */

  Dcl_decl *Dcl_message  (Lang_DCL *, Dcl_decl **, int) ;    /* Выдача сообщения на экран */
  Dcl_decl *Dcl_varsts   (Lang_DCL *, Dcl_decl **, int) ;    /* Выдача описания переменной */
  Dcl_decl *Dcl_varsts_  (Lang_DCL *, Dcl_decl **, int) ;    /* Выдача описания переменной (с паузой) */
  Dcl_decl *Dcl_varshow  (Lang_DCL *, Dcl_decl **, int) ;    /* Выдача значения переменной */
  Dcl_decl *Dcl_varshow_ (Lang_DCL *, Dcl_decl **, int) ;    /* Выдача значения переменной (с паузой) */
  Dcl_decl *Dcl_timestamp(Lang_DCL *, Dcl_decl **, int) ;    /* Выдача данных даты/времени */

/*------------------------------------------------ Эмуляция констант */

/*--------------------------------------------------- Таблица ссылок */

   Dcl_decl  dcl_debug_lib[]={

	 {0, 0, 0, 0, "$PassiveData$", NULL, "dlib", 0, 0},

	 {_DGT_VAL, _DCL_CALL, 0, 0, "message",   (void *)Dcl_message,   "s",   0, 0},
	 {_DGT_VAL, _DCL_CALL, 0, 0, "variable",  (void *)Dcl_varsts,    "a",   0, 0},
	 {_DGT_VAL, _DCL_CALL, 0, 0, "variable_", (void *)Dcl_varsts_,   "a",   0, 0},
	 {_DGT_VAL, _DCL_CALL, 0, 0, "show",      (void *)Dcl_varshow,   "a",   0, 0},
	 {_DGT_VAL, _DCL_CALL, 0, 0, "show_",     (void *)Dcl_varshow_,  "a",   0, 0},
	 {_DGT_VAL, _DCL_CALL, 0, 0, "timestamp", (void *)Dcl_timestamp, "",    0, 0},

	 {0, 0, 0, 0, "", NULL, NULL, 0, 0}
                           } ;

/*----------------------------------------------- Рабочие переменные */

#define   DCL_DEBUG  (Kernel->mDebug_path[0]!=0)


/*****************************************************************/
/*                                                               */
/*                 Выдача сообщения на экран                     */
/*                                                               */
/*        void  message(text) ;                                  */
/*                                                               */
/*          char *text  - выводимый текст                        */
/*                                                               */
/*   Выводит текст на экран, после чего переводит строку.        */

    Dcl_decl *Dcl_message(Lang_DCL *Kernel, Dcl_decl **pars, int  pars_cnt)

{
   char  buff[1024] ;
    int  i ;

    if(pars[0]->size>=sizeof(buff))                                 /* Контроль размеров сообщения */
                  pars[0]->size=sizeof(buff)-2 ;                  

	 memcpy(buff, pars[0]->addr, pars[0]->size) ;               /* Занесение сообщения в рабочий буфер */

	       i   =pars[0]->size ;
	  buff[i  ]='\n' ;                                          /* Терминируем сообщение */
	  buff[i+1]='\0' ;

          CharToOem(buff, buff) ;
	     printf(buff) ;

  return(NULL) ;
}


/*****************************************************************/
/*                                                               */
/*                 Выдача соостояния переменной                  */
/*                                                               */
/*        void  variable (var) ;                                 */
/*        void  variable_(var) ; ( с паузой )                    */
/*                                                               */
/*   Выводит на экран параметры переменной в формате :           */
/*           для числовой:                                       */
/*  NAME TYPE ADDR (SIZE_B/BUFF_B) (SIZE/BUFF) VALUE1 VALUE2 ... */
/*           для бинарной:                                       */
/*  NAME TYPE ADDR (SIZE_B/BUFF_B) "STRING"                      */
/*                                                               */
/*     где  NAME    - имя переменной (кроме рабочих)             */
/*          TYPE    - тип переменной                             */
/*          ADDR    - адрес буфера                               */
/*          SIZE    - размер обьекта в единицах обьекта          */
/*          BUFF    - размер буфера обьекта в единицах обьекта   */
/*          SIZE_B  - размер обьекта в байтах                    */
/*          BUFF_B  - размер буфера обьекта в байтах             */
/*         VALUE    - числовое значение (значения - для массива) */
/*        STRING    - содержимое буфера                          */

    Dcl_decl *Dcl_varsts(Lang_DCL *Kernel, Dcl_decl **pars, int  pars_cnt)

{
	  int  type ;            /* Базовый тип переменной */
	  int  mode ;            /* Уточняющий тип переменной */
       double  value ;           /* Значение численной переменной */
	  int  size ;            /* Размер переменной */
	  int  buff ;            /* Размер буфера переменной */
	  int  elem ;            /* Размер 'ячейки' переменной */
	 char *addr ;            /* Адрес буфера переменной */
	 char  save ;            /* Адрес буфера переменной */
	  int  save_idx ;
         char  tmp[1024] ;
	  int  i ;
  static char *types[]={            /* Мнемоники базовых типов */
			     "UNKNOWN",
			      NULL,
			     "DIGITAL",
			      NULL,
			     "DGT.AREA",
			     "DGT.PTR",
			      NULL,
			      NULL,
			     "CHAR.AREA",
			     "CHAR.PTR"
                       } ;

  static char *modes[]={            /* Мнемоники уточняющих типов */
			     "unknown",
			     "double",
			     "short",
			     "u_short",
			     "long",
			     "u_long",
			     "float"
		       } ;

/*------------------------------------------------------ Общая часть */

       if(pars[0]->name[0]==0)                                      /* Уст.имя для временной ячейки */
                     strcpy(pars[0]->name, "Work Variable") ;

        type= t_base(pars[0]->type) ;                               /* Извлекаем базовый тип */

     if(type==_XTP_OBJ) {
                              mode=t_mode(pars[0]->type)>>8 ;       /*  Извлекаем уточняющий тип */
                        }
     else               {
                           if(type<0 || type>9 ||                   /*  Контроль базового типа */
	                      types[type]==NULL  )  type=0 ;

                              mode=(t_mode(pars[0]->type)>>8)+1 ;   /*  Извлекаем уточняющий тип */
                           if(mode<1 || mode>6)  mode=0 ;           /*  Контроль уточняющего типа */
                        }

                               addr =(char *)pars[0]->addr ;
		               size =        pars[0]->size ;
		               buff =        pars[0]->buff ;

/*---------------------------------- Отображение комплекных объектов */

   if(type==_XTP_OBJ    ) {

#ifdef UNIX
      sprintf(tmp, " % 15s typedef %s %p  %d  %d ",
#else
      sprintf(tmp, " % 15s typedef %s %8Fx  %d  %d ",
#endif
                pars[0]->name, Kernel->nX_types[mode].name,
                              addr, size, buff) ;                               

            if(DCL_DEBUG)  Kernel->iLog("VARIABLE>", tmp) ;
            else                 printf("\n%s",      tmp) ;
                          }
/*---------------------------------- Отображение числовых переменных */

   else
   if(type==_DGT_VAL  ||                                        /* Отображение            */
      type==_DGT_AREA ||                                        /*    числовых переменных */
      type==_DGT_PTR    ) {
				elem=Kernel->iDgt_size(pars[0]->type) ;

#ifdef UNIX
     sprintf(tmp, " % 15s %-9s %8p (%7s) (%2d/%2d)(%2d/%2d) ",
#else
     sprintf(tmp, " % 15s %-9s %8Fx (%7s) (%2d/%2d)(%2d/%2d) ",
#endif
	      pars[0]->name, types[type], addr, modes[mode],
	       size, buff, size*elem, buff*elem             ) ;

            if(DCL_DEBUG)  Kernel->iLog("VARIABLE>", tmp) ;
            else                 printf("\n%s",      tmp) ;

	 if(addr!=NULL)
	   for(i=0 ; i<pars[0]->size ; i++) {                       /* Выводим значения переменной */
		   value=Kernel->iDgt_get(addr+i*elem, pars[0]->type) ;
	       sprintf(tmp, "%lf ", value) ;
            if(DCL_DEBUG)  Kernel->iLog("VARIABLE>", tmp) ;
            else                 printf("%s",        tmp) ;
					    }
                          }
/*-------------------------------- Отображение символьных переменных */

   else                   {

#ifdef UNIX
	       sprintf(tmp, " % 15s %-9s %8p (%2d/%2d)",
#else
	       sprintf(tmp, " % 15s %-9s %8Fx (%2d/%2d)",
#endif
			 pars[0]->name, types[type], addr,
				 size, buff) ;

            if(DCL_DEBUG)  Kernel->iLog("VARIABLE>", tmp) ;
            else                 printf("\n%s",      tmp) ;

	 if(addr!=NULL) {
                              if(size<buff)  save_idx=size  ;       /* Обеспечиваем невыход за буфер */
                              else           save_idx=buff-1 ;

				       save     =addr[save_idx] ;
				  addr[save_idx]= 0 ;

            if(DCL_DEBUG)  Kernel->iLog("VARIABLE>", addr) ;
            else                 printf("\n%s",      addr) ;

				  addr[save_idx]=save ;
                        }
                          }
/*-------------------------------------------------------------------*/

            if(!DCL_DEBUG)   printf("\n") ;

  return(NULL) ;
}


    Dcl_decl *Dcl_varsts_(Lang_DCL *Kernel, Dcl_decl **pars, int  pars_cnt)

{
       Dcl_varsts(Kernel, pars, pars_cnt) ;
	    getch() ;

   return(NULL) ;
}


/*****************************************************************/
/*                                                               */
/*                 Выдача значения переменной                    */
/*                                                               */
/*        void  show (var1, var2, ...) ;                         */
/*        void  show_(var1, var2, ...) ;  ( с паузой )           */
/*                                                               */
/*   Выводит на экран значения переменных - число или строку.    */

    Dcl_decl *Dcl_varshow(Lang_DCL *Kernel, Dcl_decl **pars, int  pars_cnt)

{
	  int  type ;            /* Базовый тип переменной */
	  int  elem ;            /* Размер 'ячейки' переменной */
       double  value ;           /* Значение численной переменной */
	 char *addr ;            /* Адрес буфера переменной */
	 char  save ;            /* Адрес буфера переменной */
	  int  size ;
         char  tmp[128] ;
	  int  i ;
	  int  j ;

/*----------------------------------------------- Цикл по переменным */

    for(i=0 ; i<pars_cnt ; i++) {                                   /* CIRCLE.1 - Скан. таблицу параметров */

		      addr=(char *)pars[i]->addr ;
		      size=        pars[i]->size ;
		      type= t_base(pars[i]->type) ;                 /* Извлекаем базовый тип */

/*----------------------------------------- Объект комплексного типа */
           if(type==_XTP_OBJ    ) {

//             for(i=0 ; i<size ; i++) 

                                  }
/*------------------------------------------------------------- NULL */

      else if(addr==NULL        ) {

            if(DCL_DEBUG)  Kernel->iLog("SHOW>", "NULL-ptr") ;
            else                 printf(         "NULL-ptr") ;

                                  }  
/*------------------------------------------------- Числовые объекты */

      else if(type==_DGT_VAL  ||
	      type==_DGT_AREA ||
	      type==_DGT_PTR    ) {                                 /* Отображение числовой переменной */

				elem=Kernel->iDgt_size(pars[i]->type) ;

	   for(j=0 ; j<size ; j++) {
		   value=Kernel->iDgt_get(addr+j*elem, pars[i]->type) ;
                                sprintf(tmp, "%lf ", value) ;
            if(DCL_DEBUG)  Kernel->iLog("SHOW>", tmp) ;
            else                 printf("%s",    tmp) ;
				   }

				  }
/*----------------------------------------------- Символьные объекты */

      else                        {
                                            save =addr[size] ;
                                       addr[size]= 0 ;
                                     
            if(DCL_DEBUG)  Kernel->iLog("SHOW>", addr) ;
            else                 printf("%s",    addr) ;

                                       addr[size]=save ;
				  }
/*----------------------------------------------- Цикл по переменным */
				}                                   /* CONTINUE.1 */
/*-------------------------------------------------------------------*/

  return(NULL) ;
}


    Dcl_decl *Dcl_varshow_(Lang_DCL *Kernel, Dcl_decl **pars, int  pars_cnt)

{
       Dcl_varshow(Kernel, pars, pars_cnt) ;
	     getch() ;

   return(NULL) ;
}


/*****************************************************************/
/*                                                               */
/*                   Выдача на экран данных даты/времени         */
/*                                                               */
/*            void  timestamp(void) ;                            */
/*                                                               */
/*      Выдает на экран текущее время и дату,                    */
/*   а также время прошедшее после предыдущего вызова.           */

    Dcl_decl *Dcl_timestamp(Lang_DCL *Kernel, Dcl_decl **pars, int  pars_cnt)

{
  static time_t  time_prv ;   /* Время предыдущего вызова */
	 time_t  time_crn ;   /* Время текущего вызова */
	 time_t  time_d  ;    /* Разница времен */
	   char *string ;     /* Текущее время-дата в виде строки */
           char  tmp[256] ;


		   time(&time_crn) ;

     if(time_prv!=0)  time_d=time_crn-time_prv ;
     else             time_d=  0 ;

		      string=ctime(&time_crn) ;

                          sprintf(tmp, "Current time:%s  Pause:%ld", string, time_d) ;

      if(DCL_DEBUG)  Kernel->iLog("SHOW>", tmp) ;
      else                 printf("\n %s",    tmp) ;

       time_prv=time_crn ;

  return(NULL) ;
}


