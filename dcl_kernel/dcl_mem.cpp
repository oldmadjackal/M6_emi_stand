/*   Bolotov P.D.  version  29.04.1994   */
/*     0 warnings at warning level 1     */

/*********************************************************************/
/*                                                                   */
/*                   DATA CONVERTION LANGUAGE                        */
/*                                                                   */
/*                 РАЗМЕЩЕНИЕ ОБЪЕКТОВ В ПАМЯТИ                      */
/*                                                                   */
/*********************************************************************/

#include  <errno.h>
#include  <stddef.h>
#include  <stdlib.h>
#include  <string.h>
#include  <malloc.h>
#include  <ctype.h>
#include  <math.h>


#define   DCL_INSIDE

#include "dcl.h"


/*----------------------------------- Управление динамической памятью */
/*                                                                    */
/*                                         Страницы заголовков        */
/*  dcl_all_vars -> г=======¬<-vars[0] ->-----------¬  ->-----------¬ */
/*  (=dcl_vars)     ¦-------¦<-vars[1] ¦ ¦ Internal ¦  ¦ ¦   Work   ¦ */
/*                  ¦-------¦   ...    ¦ ¦          ¦  ¦ ¦          ¦ */
/*                  ¦-------¦<-vars[N] ¦ ¦          ¦  ¦ ¦          ¦ */
/*                  ¦=======¦ ---------- L-----------  ¦ L----------- */
/*                  ¦-------¦ --------------------------              */
/*                  ¦=======¦<-NULL                                   */
/*                  L=======-                                         */
/*                                                                    */

/*------------------------------------------------------ Обозначения */

//#define DGT_SZ     sizeof(double)
#define CHR_SZ     sizeof(char)

/*-------------------------------------------------------------------*/


/************************************************************************/
/*                                                                      */
/*              Инициализация системы управления памятью                */
/*                                                                      */
/*    vars  -   Таблица списков внешних переменных                      */
/*                                                                      */
/*    Return: Адрес общей таблицы списков переменных                    */

   Dcl_decl **Lang_DCL::iMem_init(Dcl_decl **vars)

{
  int  all_cnt ;  /* Общее число списков переменных */
  int  ext_cnt ;  /* Счетчик списков внешних переменных */
  int  i ;


			    iMem_free() ;                           /* Сброс системы управления памятью */

                         nInt_cnt=0 ;
                        nWork_cnt=0 ;

         nInt_page=(Dcl_decl *)                                     /* Размещаем страницу внутренних переменных */
                      calloc(_TITLE_PAGE, sizeof(*nInt_page)) ;
      if(nInt_page==NULL) {  mError_code=_DCLE_MEMORY ;             /* Обраб.нехватку памяти */
		                      return(NULL) ;      }
         nWork_page=(Dcl_decl *)                                    /* Размещаем страницу внутренних переменных */
                      calloc(_TITLE_PAGE, sizeof(*nWork_page)) ;
      if(nWork_page==NULL) {  mError_code=_DCLE_MEMORY ;            /* Обраб.нехватку памяти */
		                      return(NULL) ;      }
         nWork_mem =(void **)                                       /* Размещаем страницу памяти внутренних переменных */
                      calloc(_TITLE_PAGE, sizeof(*nWork_mem)) ;
      if(nWork_mem ==NULL) {  mError_code=_DCLE_MEMORY ;            /* Обраб.нехватку памяти */
		                      return(NULL) ;      }

      if(vars!=NULL)                                                /* Если есть внешние переменные - */
	    for(ext_cnt=0 ; vars[ext_cnt]!=NULL ; ext_cnt++) ;      /*   подсчитываем число списков   */
      else      ext_cnt=0 ;                                         /*     внешних переменных         */

	       all_cnt=ext_cnt+2+1 ;                                /* Учитываем страницы внутр. и рабочих переменных */

	   nAll_vars=(Dcl_decl **)                                  /* Размещаем общую таблицу списков */
                      calloc(all_cnt, sizeof(*nAll_vars)) ;
	if(nAll_vars==NULL) {  mError_code=_DCLE_MEMORY ;           /* Обраб.нехватку памяти */
		                      return(NULL) ;      }

    for(i=0 ; i<ext_cnt ; i++)  nAll_vars[i]=vars[i] ;              /* Копируем таблицу списков внешних */
								    /*   переменных в общую таблицу     */
    for(    ; i<all_cnt ; i++)  nAll_vars[i]=NULL ;                 /* Иниц. таблицу страниц заголовков   */
								    /*   внутр.переменных, а также ставим */
								    /*    метку конца суммарной таблицы   */  
          nAll_vars[ext_cnt]=nInt_page ;                            /* Включаем страницу заголовков внутренних переменных */

   return(nAll_vars) ;
}


/*********************************************************************/
/*                                                                   */
/*                  Закрытие системы управления памятью              */

       void Lang_DCL::iMem_free(void)

{
  int  i ;


   if(nAll_vars==NULL)  return ;                                    /* Контроль 'наличия' размещения */

    for(i=0 ; i<nInt_cnt ; i++)                                     /* Освобождаем рабочие переменные - кроме указателей */
     if((nInt_page[i].type & _DCLT_DGT_PTR)!=_DCLT_DGT_PTR &&
        (nInt_page[i].type & _DCLT_CHR_PTR)!=_DCLT_CHR_PTR   )
       if(nInt_page[i].addr!=NULL)  free(nInt_page[i].addr) ;

                                    free(nInt_page) ;

    for(i=0 ; i<nWork_cnt ; i++)                                    /* Освобождаем внутренние переменные */
     if(nWork_mem[i]!=NULL)  free(nWork_mem[i]) ;

                             free(nWork_mem) ;
                             free(nWork_page) ;

                             free(nAll_vars) ;                      /* Осв.память общей таблицы списков */
	                          nAll_vars=NULL ;                  /* 'Сброс' размещения */
}


/************************************************************************/
/*                                                                      */
/*                Выдача адреса свободного заголовка                    */
/*                                                                      */
/*   type  -  Тип ячейки: _WORK_VAR - рабочая переменная                */
/*                    _INTERNAL_VAR - внутр.переменная                  */
/*                  _CLEAR_WORK_VAR - сбросить рабочие переменные       */
/*                                                                      */
/*    Рабочие переменные (_WORK_VAR) размещаются на следующей странице  */
/*  за последней страницей, занятой заголовками под внутренние          */
/*  переменные(_INTERNAL_VAR).                                          */

    Dcl_decl *Lang_DCL::iNext_title(int  type)

{
  Dcl_decl *page  ;   /* Текущая страница заголовков */
  Dcl_decl *title ;   /* Выделяемый заголовок */
       int  i ;

/*---------------------------------------------------- Инициализация */

                          title=NULL ;

/*---------------------------------- Освобождение рабочих переменных */

  if(type==_CLEAR_WORK_VAR) {

       for(i=1 ; i<nWork_cnt ; i++)                                 /* Внимание! Рабочие переменные размещаются с индекса 1 */
         if(nWork_mem[i]!=NULL)  free(nWork_mem[i]) ;

                                      nWork_cnt=0 ;                 /* Сброс счетчика рабочих ячеек */
  
				 return(NULL) ;
			    }
/*---------------------- Размещение заголовков внутренних переменных */

  if(type==_INTERNAL_VAR) {

      if(nInt_cnt==_TITLE_PAGE) { mError_code=_DCLE_MEMORY_VT ;     /* Контроль переполнения */
					    return(NULL) ;      }

             page=      nInt_page ;                                 /* Уст.адрес текущей страницы */
            title=&page[nInt_cnt] ;                                 /* Выделяем заголовок */
                        nInt_cnt++ ;                                /* Модиф.счетчик заголовков внутр.переменных */

                 memset(title, 0, sizeof(*title)) ;                 /* Очистка заголовка */

			  }
/*------------------------- Размещение заголовков рабочих переменных */

  if(type==_WORK_VAR) {

     if(nWork_cnt==_TITLE_PAGE) { mError_code=_DCLE_MEMORY_VT ;     /* Контроль переполнения */
					    return(NULL) ;      }

		 page=nWork_page ;                                  /* Уст.адрес текущей страницы */

        if(nWork_cnt==0)  nWork_cnt=1 ;                             /* 'Нулевой' заголовок не использ */

	                 title=&page[nWork_cnt] ;                   /* Выделяем заголовок */

                  memset(title, 0, sizeof(*title)) ;                /* Очистка заголовка */
	                 title->name[0] =  0 ;                      /* Уст. 'рабочее' имя */
	                 title->work_nmb=nWork_cnt  ;               /* Уст.флаг рабочей ячейки */
                               
                               nWork_mem[nWork_cnt]=NULL ;          /* Сброс указателя собственной памяти */
                                         nWork_cnt++ ;              /* Модиф.счетчик рабочих заголовков */
		      }
/*-------------------------------------------------------------------*/

   return(title) ;
}


/************************************************************************/
/*                                                                      */
/*         Размещение переменной в динамической памяти                  */
/*                                                                      */
/*   type  -  Тип ячейки: _WORK_VAR - рабочая переменная                */
/*                    _INTERNAL_VAR - внутр.обьект                      */
/*                  _CLEAR_WORK_VAR - очистить место рабочих переменных */
/*    var  -  Описание переменной                                       */
/*   save  -  Размер переносимого буфера                                */
/*                                                                      */
/*   При выделении памяти под данные рабочей переменной(_WORK_VAR)      */
/*  в том случае, если в описании указано ненулевое значение addr,      */
/*  производится перенос данных со старого места на новое               */

    void *Lang_DCL::iAlloc_var(int  type, Dcl_decl *var, int save)

{
   int  var_size ;  /* Требуемое для данных место */
   int  var_type ;  /* Базовый тип переменной */
   int  cell ;      /* Размер "типовой" ячейки в байтах */
  char *addr ;

/*---------------------------------- Освобождение рабочих переменных */

  if(type==_CLEAR_WORK_VAR) {
				 return(NULL) ;
			    }
/*-------------------------------- Определение размера буфера данных */

                              var_type=t_base(var->type) ;          /* Извлекаем базовый тип переменной */

    if(var_type==_DGT_VAL  ||                                       /* Определяем размер "типовой" ячейки */
       var_type==_DGT_AREA   )    cell=iDgt_size(var->type) ;       /*   переменной                       */
    else                          cell=  1 ;

                              var_size =cell*var->buff ;
                                  save*=cell ;

/*----------------------------------- Размещение внутренних обьектов */

  if(type==_INTERNAL_VAR) {

          var->addr=calloc(var_size+1, 1) ;
       if(var->addr==NULL) {  mError_code=_DCLE_MEMORY_VA ;         /* Если памяти нет - уст.ошибку */
                                      return(NULL) ;         }
		          }
/*------------------------------------ Размещение рабочих переменных */

  if(type==_WORK_VAR) {
/*- - - - - - - - - - - - - - - - - - - - - - - - - Выделение памяти */
               addr=(char *)var->addr ;                             /* Фиксируем целевые данные */

          var->addr=calloc(var_size+1, 1) ;
       if(var->addr==NULL) {  mError_code=_DCLE_MEMORY_VA ;         /* Если памяти нет - уст.ошибку */
                                      return(NULL) ;         }
   
           nWork_mem[var->work_nmb]=var->addr ;                     /* Запись указателя собственной памяти */
/*- - - - - - - - - - - - - - - - - -  Восстановление целевых данных */
     if(addr!=NULL && save>0)  memmove(var->addr, addr, save) ;     /* Копируем данные в буфер */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
		      }
/*-------------------------------------------------------------------*/

   return(var->addr) ;
}


/************************************************************************/
/*                                                                      */
/*         Выдача адреса заголовка рабочей ячейки по ее номеру          */
/*                                                                      */
/*    n  -  Номер рабочей ячейки                                        */

   Dcl_decl *Lang_DCL::iGet_work(int  n)

{
  Dcl_decl *page  ;   /* Нужная страница заголовков */
  Dcl_decl *title ;   /* Нужный заголовок */

/*------------------------------- 1-страничное размещение заголовков */

		 page=nWork_page ;                                  /* Уст.адрес текущей страницы */
		title=    &page[n] ;                                /* Опред.адрес заголовка */

/*---------------------------------- Контроль правильности адресации */

     if(title->work_nmb!=n)  mError_code=_DCLE_WORK_FAULT ;

/*-------------------------------------------------------------------*/

   return(title) ;
}


/*********************************************************************/
/*                                                                   */
/*             Стекование параметров управления памятью              */

typedef  struct {
		  Dcl_decl **all_vars ;              /* Общая таблица списков переменных */
		  Dcl_decl  *int_page ;              /* Страница заголовков внутренних переменных */
		       int   int_cnt ;
		  Dcl_decl  *work_page ;             /* Страница заголовков рабочих переменных */
                      void **work_mem ;
		       int   work_cnt ;
		} Dcl_mstack ;

    int  Lang_DCL::iMem_stack(int  oper_code)

{
         Dcl_mstack *addr ;

  static Dcl_mstack *stack[_STACK_MAX] ;
  static        int  stack_cnt ;

/*--------------------------------------------- Запись данных в стек */

  if(oper_code==_DCL_PUSH) {
/*- - - - - - - - - - - - - - - - - - - - - - - - - Подготовка стека */
     if(stack_cnt==_STACK_MAX) {  mError_code=_DCLE_STACK_OVR ;     /* Если стек заполнен... */
					      return(-1) ;       }

	   addr=(Dcl_mstack *)calloc(sizeof(*addr), 1) ;            /* Выделяем память под стек */
	if(addr==NULL) {  mError_code=_DCLE_STACK_MEM ;             /* Если памяти нет... */
				  return(-1) ;          }

	        stack[stack_cnt]=addr ;                             /* Заносим адрес данных в стек */
		      stack_cnt++    ;
/*- - - - - - - - - - - - - - - - - - - - -  Занесение данных в стек */
                   addr->all_vars =nAll_vars ;
                   addr->int_page =nInt_page ;
    	           addr->int_cnt  =nInt_cnt   ;
                   addr->work_page=nWork_page ;
                   addr->work_mem =nWork_mem ;
    	           addr->work_cnt =nWork_cnt   ;
/*- - - - - - - - - - - - - - - - - - - - - - - Инициализация данных */
		         nAll_vars=NULL ;
		         nInt_page=NULL ;
		        nWork_page=NULL ;
                        nWork_mem =NULL ;
		         nInt_cnt =  0 ;
		        nWork_cnt =  0 ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
		       }
/*--------------------------------------- Извлечение данных из стека */

  if(oper_code==_DCL_POP) {
/*- - - - - - - - - - - - - - - - - - - - - - - - - Подготовка стека */
         if(stack_cnt<=0) {  mError_code=_DCLE_STACK_UND ;          /* Если стек пуст... */
	                                  return(-1) ;       }

                      stack_cnt-- ;
	   addr=stack[stack_cnt] ;                                  /* Извлекаем адрес данных из стек */
/*- - - - - - - - - - - - - - - - - - Восстановление данных из стека */
	      nAll_vars=addr->all_vars ;
	      nInt_page=addr->int_page ;
	      nInt_cnt =addr->int_cnt  ;
             nWork_page=addr->work_page ; 
             nWork_mem =addr->work_mem ;
             nWork_cnt =addr->work_cnt ;
/*- - - - - - - - - - - - - - - - - - - - - - -  Освобождение памяти */
			     free(addr) ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
		      }
/*-------------------------------------------------------------------*/

  return(0) ;
}
