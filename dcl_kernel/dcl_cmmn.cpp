/*   Bolotov P.D.  version  29.04.1994   */
/*     0 warnings at warning level 1     */

/*********************************************************************/
/*                                                                   */
/*                   DATA CONVERTION LANGUAGE                        */
/*                                                                   */
/*                    УТИЛИТЫ ОБЩЕГО НАЗНАЧЕНИЯ                      */
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
#include  <time.h>
#include  <sys/timeb.h>

#ifdef UNIX
#include  <unistd.h>
#else
#include <process.h>
#endif


#define DCL_INSIDE

#include "dcl.h"


#pragma warning(disable : 4244)
#pragma warning(disable : 4267)
#pragma warning(disable : 4996)

/*--------------------------------------------- Технические описания */

  union Dcl_v_conversion {
				  double  v_double ;
				   float  v_float ;
				    long  v_long ;
			   unsigned long  v_ulong ;
				   short  v_short ;
			  unsigned short  v_ushort ;
			 } ;

/*********************************************************************/
/*                                                                   */
/*           Поиск конечного символа не включенного в скобки         */
/*                с игнорированием символьных констант               */
/*                                                                   */
/*   text  -  Анализируемая строка                                   */
/*   end   -  Конечный символ или                                    */
/*             _ANY_OPER        -  любая операция                    */
/*             _ANY_OPER_OR_END - любая операция или граница строки  */
/*             _ANY_CLOSE       - любая закрывающая скобку           */

  char *Lang_DCL::iFind_close(char *text, int  end)

{
  int  string_flag ;   /* Флаг нахождения внутри символьной строки */
  int  n1 ;            /* Счетчик вложенности круглых скобок */
  int  n2 ;            /* Счетчик вложенности квадратных скобок */


	   string_flag=0 ;
		    n1=0 ;
		    n2=0 ;

  for( ; *text ; text++) {

/*------------------------------------- Прохождение символьных строк */

    if(*text=='"')  if(string_flag) {                               /* Идентификация и обработка */
		     if(*(text-1)!='\\' ||                          /*   открытия и закрытия     */
			*(text-2)=='\\'   )  string_flag=0 ;        /*    символьной строки      */
				    }
		    else                     string_flag=1 ;

    if(string_flag)  continue ;                                     /* Проход симв.строки */

/*--------------------------------------------------- Основной поиск */

	 if(*text=='?')  text+=1 ;                                  /* Обраб.особых фрагментов */
    else if((end ==_ANY_OPER ||                                     /* Поиск знаков операций */
	     end ==_ANY_OPER_OR_END) &&
	      OPER_CHAR(*text)         ) {
					   if(!n1 && !n2)  break ;
					 }
    else if(*text==end) {                                           /* Поиск стоп-символа */
			  if(!n1 && !n2)  break ;
			  if(end == ')')   n1-- ;
			}
    else if(*text==')') {                                           /* Обраб.закрытия скобок */
	    if(end==_ANY_CLOSE)  break ;
	    else                  n1-- ;
			}
    else if(*text=='(')   n1++ ;                                    /* Обраб.открытия скобок */
    else if(*text==']')   n2-- ;                                    /* Обраб.закрытия скобок */
    else if(*text=='[')   n2++ ;                                    /* Обраб.открытия скобок */

/*-------------------------------------------------------------------*/

			 }

       if(end==_ANY_OPER       ) {  if(*text== 0 )  text=NULL ;  }  /* Контроль обнаружения     */
  else if(end==_ANY_OPER_OR_END) {                  text-- ;     }  /*   того, что надо и       */
  else if(end==_ANY_CLOSE      ) {  if(*text!=')')  text=NULL ;  }  /*    позиционная коррекция */
  else                           {
				     if(*text!=end)  text=NULL ;
				     if(  n1 || n2)  text=NULL ; //??????
				 }


 return(text) ;
}


/*********************************************************************/
/*                                                                   */
/*          Поиск начального символа не включенного в скобки         */
/*                с игнорированием символьных констант               */
/*                                                                   */
/*   text   -  Анализируемая строка                                  */
/*   start  -  Точка входа                                           */
/*   end    -  Конечный символ или                                   */
/*              _ANY_OPER        -  любая операция                   */
/*              _ANY_OPER_OR_END - любая операция или граница строки */

  char *Lang_DCL::iFind_open(char *text, char *start, int  end)

{
 char *tmp ;           /* Рабочий указатель */
  int  string_flag ;   /* Флаг нахождения внутри символьной строки */
  int  n1 ;            /* Счетчик вложенности круглых скобок */
  int  n2 ;            /* Счетчик вложенности квадратных скобок */


	   string_flag=0 ;
		    n1=0 ;
		    n2=0 ;

  for(tmp=start ; tmp>=text ; tmp--) {

/*------------------------------------- Прохождение символьных строк */

    if(*tmp=='"')  if(string_flag) {                                /* Идентификация и обработка */
		     if(*(tmp-1)!='\\' ||                           /*   открытия и закрытия     */
			*(tmp-2)=='\\'   ) string_flag=0 ;          /*    символьной строки      */
				   }
		   else                 string_flag=1 ;

    if(string_flag)  continue ;                                     /* Проход симв.строки */

/*--------------------------------------------------- Основной поиск */

	 if( tmp[-1]=='?')  tmp-=1 ;                                /* Обраб.особых фрагментов */
    else if((end ==_ANY_OPER ||                                     /* Поиск знаков операций */
	     end ==_ANY_OPER_OR_END) &&
	      OPER_CHAR(*tmp)          ) {
					   if(!n1 && !n2)  break ;
					 }
    else if(*tmp==end) {                                            /* Поиск стоп-символа */
			  if(!n1 && !n2)  break ;
			  if(end == '(')   n1-- ;
		       }
    else if(*tmp==')')   n1++ ;                                     /* Обраб.закрытия скобок */
    else if(*tmp=='(')   n1-- ;                                     /* Обраб.открытия скобок */
    else if(*tmp==']')   n2++ ;                                     /* Обраб.закрытия скобок */
    else if(*tmp=='[')   n2-- ;                                     /* Обраб.открытия скобок */

/*-------------------------------------------------------------------*/

				     }

  if(end==_ANY_OPER_OR_END) {                   tmp++ ;     }
  else                         if(tmp <text  )  tmp=NULL ;

 return(tmp) ;
}


/*********************************************************************/
/*                                                                   */
/*          Определение длины числового операнда                     */
/*                                                                   */
/*   type  -  Тип данных                                             */

   int  Lang_DCL::iDgt_size(int  type)

{
    int  size ;

	   type=t_mode(type) ;                                      /* Выделение уточн.типа */

                                size=  0 ;
	if(type==_DCLT_DOUBLE)  size=sizeof(double) ;               /* Преобразование данных         */
   else if(type==_DCLT_FLOAT )  size=sizeof(float) ;                /*  к основному формату - DOUBLE */
   else if(type==_DCLT_LONG  )  size=sizeof(long) ;
   else if(type==_DCLT_ULONG )  size=sizeof(long);
   else if(type==_DCLT_SHORT )  size=sizeof(short);
   else if(type==_DCLT_USHORT)  size=sizeof(short) ;

 return(size) ;
}


/*********************************************************************/
/*                                                                   */
/*                Извлечение числового значения                      */
/*                                                                   */
/*   addr  -  Адрес данных                                           */
/*   type  -  Тип данных                                             */

   double  Lang_DCL::iDgt_get(void *addr, int  type)

{
  union Dcl_v_conversion  conv ;   /* Буфер выравнивания-преобразования */
		  double  value ;  /* 'Выходное' значение */


	       if(addr==NULL)  return(0.) ;                         /* Контроль адреса данных */

	     memcpy(&conv, addr, sizeof(conv)) ;                    /* Передача данных в буфер преобразования */

	   type=t_mode(type) ;                                      /* Выделение уточн.типа */

                                value=  0. ;
	if(type==_DCLT_DOUBLE)  value=conv.v_double ;               /* Преобразование данных         */
   else if(type==_DCLT_FLOAT )  value=conv.v_float ;                /*  к основному формату - DOUBLE */
   else if(type==_DCLT_LONG  )  value=conv.v_long ;
   else if(type==_DCLT_ULONG )  value=conv.v_ulong ;
   else if(type==_DCLT_SHORT )  value=conv.v_short ;
   else if(type==_DCLT_USHORT)  value=conv.v_ushort ;

 return(value) ;
}


/*********************************************************************/
/*                                                                   */
/*                Занесение числового значения                       */
/*                                                                   */
/*   value  -  Значение                                              */
/*   addr   -  Адрес данных                                          */
/*   type   -  Тип данных                                            */

   double  Lang_DCL::iDgt_set(double  value, void *addr, int  type)

{
  union Dcl_v_conversion  conv ;   /* Буфер выравнивания-преобразования */


	       if(addr==NULL)  return(0.) ;                         /* Контроль адреса данных */

	   type=t_mode(type) ;                                      /* Выделение уточн.типа */

	if(type==_DCLT_DOUBLE)  conv.v_double=value ;               /* Преобразование данных из основного формата - DOUBLE */
   else if(type==_DCLT_FLOAT )  conv.v_float =(         float)value ; 
   else if(type==_DCLT_LONG  )  conv.v_long  =(          long)value ;
   else if(type==_DCLT_ULONG )  conv.v_ulong =(unsigned  long)value ;
   else if(type==_DCLT_SHORT )  conv.v_short =(         short)value ;
   else if(type==_DCLT_USHORT)  conv.v_ushort=(unsigned short)value ;

	     memcpy(addr, &conv, iDgt_size(type)) ;                 /* Передача данных из буфер преобр. */

 return(value) ;
}


/*********************************************************************/
/*                                                                   */
/*         Следящая коррекция строчных обьектов                      */
/*                                                                   */
/*   base_var  -  Базовая переменная коррекции                       */
/*                                                                   */
/*  Среди переменных ищем те, которые указывают на базовый обьект    */
/*  или внутрь его и корректируем размер обьекта в соответствии      */  
/*  с базовым.                                                       */

   void  Lang_DCL::iSize_correct(Dcl_decl *base_var)

{
            Dcl_decl *vars ;       /* Текущий массив описаний переменных и процедур */
  Dcl_complex_record *record ;
                 int  type ;       /* Основной тип переменной */
                 int  i ;
                 int  j ;
                 int  k ;

/*------------------------------------------------------- Подготовка */

	type=t_base(base_var->type) ;
     if(type!=_CHR_AREA &&                                          /* Обрабатываются только */
	type!=_CHR_PTR    )  return ;                               /*    строчные обьекты   */

/*------------------------------------ Работа со списками переменных */

  for(i=0 ; nVars[i]!=NULL ; i++) {

     if(nVars[i]->addr==NULL)  continue ;                           /* Не смотрим "пассивные" списки объектов */

   for(vars=nVars[i] ; vars->name[0]!=0 ; vars++)
     if(vars->func_flag==0) {

          type=t_base(vars->type) ;
/*- - - - - - - - - - - - - - - - - - - - - - - - Комплексный объект */
       if(type==_XTP_OBJ) {

         if((vars->work_nmb & _DCL_XTRACE)!=_DCL_XTRACE) continue ; /* Только если отслеживание по объекту разрешено */

          for(record=(Dcl_complex_record *)vars->addr,
                             j=0 ; j<vars->buff ; j++, 
              record=(Dcl_complex_record *)record->next_record)
         for(k=0 ; k<record->elems_cnt ; k++) {
              type=t_base(record->elems[k].type) ;
           if(type==_CHR_AREA ||
              type==_CHR_PTR    )  iSize_correct(base_var, &record->elems[k]) ;
                                              }

                                     continue ;
                          }
/*- - - - - - - - - - - - - - - - - - - - - - - - Простая переменная */
       else
       if(type==_CHR_AREA ||
          type==_CHR_PTR    )  iSize_correct(base_var, vars) ;

                            }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                                  }
/*--------------------------------- Работа с внутренними переменными */

   for(vars=nInt_page ; vars->name[0]!=0 ; vars++) {

          type=t_base(vars->type) ;
/*- - - - - - - - - - - - - - - - - - - - - - - - Комплексный объект */
       if(type==_XTP_OBJ) {

          for(record=(Dcl_complex_record *)vars->addr,
                             j=0 ; j<vars->buff ; j++, 
              record=(Dcl_complex_record *)record->next_record)
         for(k=0 ; k<record->elems_cnt ; k++) {
              type=t_base(record->elems[k].type) ;
           if(type==_CHR_AREA ||
              type==_CHR_PTR    )  iSize_correct(base_var, &record->elems[k]) ;
                                              }

                                     continue ;
                          }
/*- - - - - - - - - - - - - - - - - - - - - - - - Простая переменная */
       else
       if(type==_CHR_AREA ||
          type==_CHR_PTR    )  iSize_correct(base_var, vars) ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                                                   }
/*------------------------------------- Работа с транзитной таблицей */

  for(i=0 ; i<nTransit_cnt ; i++) {

           vars=&nTransit[i] ;
           type=t_base(vars->type) ;
/*- - - - - - - - - - - - - - - - - - - - - - - - Комплексный объект */
       if(type==_XTP_OBJ) {

          for(record=(Dcl_complex_record *)vars->addr,
                             j=0 ; j<vars->buff ; j++, 
              record=(Dcl_complex_record *)record->next_record)
         for(k=0 ; k<record->elems_cnt ; k++) {
              type=t_base(record->elems[k].type) ;
           if(type==_CHR_AREA ||
              type==_CHR_PTR    )  iSize_correct(base_var, &record->elems[k]) ;
                                              }

                                     continue ;
                          }
 /*- - - - - - - - - - - - - - - - - - - - - - - - Простая переменная */
        else
	if(type==_CHR_AREA ||
	   type==_CHR_PTR    )  iSize_correct(base_var, vars) ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                                  }
/*-------------------------------------------------------------------*/
}


   void  Lang_DCL::iSize_correct(Dcl_decl *base, Dcl_decl *dest)

{
   char *addr_base ;  /* Базовый адрес */
   char *addr_end ;   /* Адрес конца буфера обьекта */
    int  size_base ;  /* Базовый размер */
    int  buff_base ;  /* Базовый буфер */

/*------------------------------------------------------- Подготовка */

	  addr_base=(char *)base->addr ;
	  size_base=        base->size ;
	  buff_base=        base->buff ;
	  addr_end =addr_base+buff_base ;

/*------------------------------------------- "Связанная" переменная */

  if((char *)dest->addr+dest->buff==addr_end ||
          (  dest->addr>=addr_base &&
             dest->addr< addr_end    )         ) {

         if(dest->buff<0) {
                             mError_code=_DCLE_READONLY ;
                                return ;
                          }

	    dest->size=addr_base-(char *)dest->addr+size_base ;
	 if(dest->size<   0      )  dest->size=    0 ;
	 if(dest->size>dest->buff)  dest->size=dest->buff ;
						 }
/*-------------------------------- "Зеркало" динамической переменной */

  else
  if(strcmp(base->name, dest->name)==0 &&
            base->prototype   !=NULL   &&
            base->prototype[0]=='D'    &&
            dest->prototype   !=NULL   &&
            dest->prototype[0]=='D'      ) {

         if(dest->buff<0) {
                             mError_code=_DCLE_READONLY ;
                                return ;
                          }
     
               dest->addr=base->addr ;
               dest->size=base->size ;
               dest->buff=base->buff ;
                                           }
/*-------------------------------------------------------------------*/
}


/*********************************************************************/
/*                                                                   */
/*   Следящая коррекция строчных обьектов при операции INCLUDE       */
/*                                                                   */
/*    base_var  -  Базовая переменная коррекции                      */
/*    begin     -  Арес начала INCLUDE-области                       */
/*    end       -  Адрес конца INCLUDE-области                       */
/*    shift     -  Направленный сдвиг 'хвоста'                       */

   void  Lang_DCL::iIncl_correct(Dcl_decl *base_var, 
                                     char *begin, 
                                     char *end, 
                                      int  shift)

{
  Dcl_decl *vars ;       /* Текущий массив описаний переменных и процедур */
      char *addr_base ;  /* Базовый адрес */
      char *addr_end ;   /* Адрес конца буфера обьекта */
       int  size_base ;  /* Базовый размер */
       int  buff_base ;  /* Базовый размер */
       int  i ;


     if(base_var->type!=_CHR_AREA &&                                /* Обрабатываются только */
	base_var->type!=_CHR_PTR    )  return ;                     /*    строчные обьекты   */

     if(shift==0)  return ;                                         /* Если сдвига хвоста не было ... */

	  addr_base=(char *)base_var->addr ;
	  size_base=        base_var->size ;
	  buff_base=        base_var->buff ;
	  addr_end =addr_base+buff_base ;

  for(i=0 ; nVars[i]!=NULL ; i++) {                                 

     if(nVars[i]->addr==NULL)  continue ;                           /* Не смотрим "пассивные" списки объектов */

   for(vars=nVars[i] ; vars->name!=NULL ; vars++)                   /* Среди переменных ищем те, которые            */
     if( vars->func_flag==    0         &&                          /*   указывают на базовый обьект или внутрь его */
	(vars->type     ==_CHR_AREA ||                              /*  (поиск ведется по совпадению конечных       */
	 vars->type     ==_CHR_PTR    )   )                         /*    адресов буферов обьектов с таковым        */
       if((char *)vars->addr+vars->buff==addr_end) {                /*       у базового обьекта)                    */

/*--------------------------------------- При сдвиге хвоста 'внутрь' */

		 if(vars->addr<=begin) {                            /* Если адрес обьекта до INCLUDE-области... */
					 vars->size+=shift ;        /*   Корректируем размер обьекта */
			if(vars->size>                              /*   Контроль непревышения      */
			   vars->buff )  vars->size =vars->buff ;   /*     обьектом размеров буфера */
				       }
	    else if(vars->addr<=end  ) {                            /* Если адрес обьекта внутри INCLUDE-области... */
					 vars->size+=shift ;        /*   Корректируем размер обьекта */
		       if(vars->size<0)  vars->size =  0  ;         /*   Контроль на 'коллапс' обьекта */
		       if(vars->size>                               /*   Контроль непревышения      */
			  vars->buff  )  vars->size =vars->buff ;   /*     обьектом размеров буфера */
				       }                            /* Если адрес обьекта после INCLUDE-области... */
	    else                       {
                           vars->addr =(char *)vars->addr+shift ;   /* Коррект.адрес обьекта и размер его буфера  */
                           vars->buff-=shift ;
				       }
/*-------------------------------------------------------------------*/
						   }
                                  }  
}


/*********************************************************************/
/*                                                                   */
/*               Переразмещение диманического массива                */
/*                                                                   */
/*    base_var  -  Базовая переменная коррекции                      */
/*    work_var  -  Рабочая переменная коррекции                      */
/*    size      -  Новый размер динамического массива                */

   void  Lang_DCL::iBuff_realloc(Dcl_decl *base_var, 
                                 Dcl_decl *work_var, 
                                   size_t  size)

{
#if 0
  Dcl_decl *vars ;       /* Текущий массив описаний переменных и процедур */
      char *addr_base ;  /* Базовый адрес */
      char *addr_end ;   /* Адрес конца буфера обьекта */
       int  size_base ;  /* Базовый размер */
       int  buff_base ;  /* Базовый размер */
       int  i ;
#endif

     if(base_var->type!=_CHR_AREA &&                                /* Обрабатываются только */
	base_var->type!=_CHR_PTR    )  return ;                     /*    строчные обьекты   */

                    size= size+_DYN_CHUNK ;
          base_var->addr=realloc(base_var->addr, size) ;
          base_var->buff= size ;

     if(work_var!=NULL) {
          work_var->addr=base_var->addr ;
          work_var->buff=base_var->buff ;
                        }

#if 0
	  addr_base=(char *)base_var->addr ;
	  size_base=        base_var->size ;
	  buff_base=        base_var->buff ;
	  addr_end =addr_base+buff_base ;

  for(i=0 ; nVars[i]!=NULL ; i++) {                                 

     if(nVars[i]->addr==NULL)  continue ;                           /* Не смотрим "пассивные" списки объектов */

   for(vars=nVars[i] ; vars->name!=NULL ; vars++)                   /* Среди переменных ищем те, которые            */
     if( vars->func_flag==    0         &&                          /*   указывают на базовый обьект или внутрь его */
	(vars->type     ==_CHR_AREA ||                              /*  (поиск ведется по совпадению конечных       */
	 vars->type     ==_CHR_PTR    )   )                         /*    адресов буферов обьектов с таковым        */
       if((char *)vars->addr+vars->buff==addr_end) {                /*       у базового обьекта)                    */

/*--------------------------------------- При сдвиге хвоста 'внутрь' */

		 if(vars->addr<=begin) {                            /* Если адрес обьекта до INCLUDE-области... */
					 vars->size+=shift ;        /*   Корректируем размер обьекта */
			if(vars->size>                              /*   Контроль непревышения      */
			   vars->buff )  vars->size =vars->buff ;   /*     обьектом размеров буфера */
				       }
	    else if(vars->addr<=end  ) {                            /* Если адрес обьекта внутри INCLUDE-области... */
					 vars->size+=shift ;        /*   Корректируем размер обьекта */
		       if(vars->size<0)  vars->size =  0  ;         /*   Контроль на 'коллапс' обьекта */
		       if(vars->size>                               /*   Контроль непревышения      */
			  vars->buff  )  vars->size =vars->buff ;   /*     обьектом размеров буфера */
				       }                            /* Если адрес обьекта после INCLUDE-области... */
	    else                       {
                           vars->addr =(char *)vars->addr+shift ;   /* Коррект.адрес обьекта и размер его буфера  */
                           vars->buff-=shift ;
				       }
/*-------------------------------------------------------------------*/
						   }
                                  }  
#endif
}


/*********************************************************************/
/*                                                                   */
/*           Перевод символьной строки в число.                      */
/*                                                                   */
/*   text  -  Обрабатываемый фрагмент                                */
/*   end   -  Последний обработанный символ фрагмента                */

  double  Lang_DCL::iStrToNum(char *text, char **end)

{
   char *addr ;
 double  value ;


	    addr=strchr(text, '.') ;                                /* Определяем наличие предпологаемой */
								    /*      десятичной точки             */
	 if(addr   !=NULL  )  value=strtod(text, end) ;             /* Интерпретируем как плавующее */
    else if(text[0]=='0' &&
	    text[1]=='x'   )  value=strtol(text+2, end, 16) ;       /* Интерпр. как 16-ричное целое */
    else if(text[0]=='0' &&
	    text[1]=='o'   )  value=strtol(text+2, end,  8) ;       /* Интерпр. как  8-ричное целое */
    else                      value=strtol(text  , end, 10) ;       /* Интерпр. как 10-тичное целое */

  return(value) ;
}


/*********************************************************************/
/*                                                                   */
/*              Перевод числа в символьную строку                    */
/*                                                                   */
/*   value -  Числовой объект                                        */
/*   text  -  Выходной буфер                                         */

  void   Lang_DCL::iNumToStr(Dcl_decl *value, char *text)

{
	     int  type_base ;
	     int  type_mode ;
	    char *tmp ;

	    long *a_long ;
  unsigned  long *a_longu ;
	   short *a_short ;
  unsigned short *a_shortu ;
	   float *a_float ;
	  double *a_double ;

/*------------------------------------------------- Входной контроль */

  if(value->addr==NULL) {
                            strcpy(text, "NULL") ;
                                   return ;
                        }
/*----------------------------------------------- Разведение адресов */

          type_base=t_base(value->type) ;
          type_mode=t_mode(value->type) ;

	   a_long  =(          long *)value->addr ;
	   a_longu =(unsigned  long *)value->addr ;
	   a_short =(         short *)value->addr ;
	   a_shortu=(unsigned short *)value->addr ;
	   a_float =(         float *)value->addr ;
	   a_double=(        double *)value->addr ;

/*----------------------------------- Преобразование одиночных чисел */

  if(type_base==_DGT_VAL) {
/*- - - - - - - - - - - - - - - - - - - - - - Типовое приобразование */
         if(type_mode==_DCLT_SHORT )  sprintf(text, "%hd",    *a_short) ;
    else if(type_mode==_DCLT_USHORT)  sprintf(text, "%hu",    *a_shortu) ;
    else if(type_mode==_DCLT_LONG  )  sprintf(text, "%ld",    *a_long) ;
    else if(type_mode==_DCLT_ULONG )  sprintf(text, "%lu",    *a_longu) ;
    else if(type_mode==_DCLT_FLOAT )  sprintf(text, "%.10f",  *a_float) ;
    else if(type_mode==_DCLT_DOUBLE)  sprintf(text, "%.10lf", *a_double) ;
/*- - - - - - - - - - - - - - - - - - - Снятие ноликов дробной части */
         if(type_mode==_DCLT_FLOAT  ||
            type_mode==_DCLT_DOUBLE   ) {

              for(tmp=text+strlen(text)-1 ; *tmp=='0' ; tmp--) *tmp=0 ;

                if(*tmp=='.')  *tmp=0 ;
                                        }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                          } 
/*-------------------------------------------------------------------*/

  return ;
}


/*********************************************************************/
/*                                                                   */
/*              Вывод информации в отладочный лог                    */
/*                                                                   */
/*   text  -  Выходной буфер                                         */

   void  Lang_DCL::iLog(char *text_1, char *text_2)

{
#ifdef UNIX
      struct timeb  time_ms ;
#else
  struct __timeb32  time_ms ;
#endif
              char  prefix[128] ;
              FILE *file ;

/*------------------------------------------------- Входной контроль */

    if(mDebug_path[0]==0)  return ;

/*-------------------------------------- Определение временной метки */

#ifdef UNIX
      ftime(&time_ms) ; 
    sprintf( prefix, "%d.%hu> ", time_ms.time, time_ms.millitm) ;
#else
   _ftime32(&time_ms) ; 
    sprintf( prefix, "%d.%hu> ", time_ms.time, time_ms.millitm) ;
#endif

/*------------------------------------------------- Запись лог-файла */

      file=fopen(mDebug_path, "at") ;                               /* Открытие файла лога */
   if(file==NULL)  return ;

                     fwrite(prefix, 1, strlen(prefix), file) ;
   if(text_1!=NULL)  fwrite(text_1, 1, strlen(text_1), file) ;
   if(text_2!=NULL)  fwrite(text_2, 1, strlen(text_2), file) ;
                     fwrite("\n",   1, strlen( "\n" ), file) ;

         fclose(file) ;	                                            /* Закрытие файла лога */

/*-------------------------------------------------------------------*/

  return ;
}
