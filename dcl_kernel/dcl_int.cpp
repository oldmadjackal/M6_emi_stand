/*   Bolotov P.D.  version  29.04.1994   */
/*     0 warnings at warning level 1     */

/*********************************************************************/
/*                                                                   */
/*                   DATA CONVERTION LANGUAGE                        */
/*                                                                   */
/*                        ИНТЕРПРИТАТОР                              */
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
#include  <malloc.h>
#include  <ctype.h>
#include  <math.h>
#include  <fcntl.h>
#include  <stdio.h>
#include  <sys/types.h>
#include  <sys/stat.h>

#ifdef UNIX
#include  <unistd.h>
#define   stricmp  strcasecmp
#define   O_BINARY    0
#else
#include  <io.h>
#include  <direct.h>
#endif

#define  _DCL_MAIN_
#define   DCL_INSIDE

#include "dcl.h"


#pragma warning(disable : 4244)
#pragma warning(disable : 4267)
#pragma warning(disable : 4996)

/*--------------------------- Описание системы подстрок и операторов */

#define        _IF_LEX       "if("
#define        _IF_SIZE        3
#define      _ELSE_LEX       "else"
#define      _ELSE_SIZE        4
#define    _CIRCLE_LEX       "for("
#define    _CIRCLE_SIZE        4
#define    _RETURN_LEX       "return"
#define    _RETURN_SIZE        6
#define     _BREAK_LEX       "break"
#define     _BREAK_SIZE        5
#define  _CONTINUE_LEX       "continue"
#define  _CONTINUE_SIZE        8
#define      _GOTO_LEX       "goto"
#define      _GOTO_SIZE        4

#define      _OPEN_BLOCK       '{'
#define     _CLOSE_BLOCK       '}'

#define      _LEET_       0
#define      _ELSE_       1
#define        _IF_       2
#define    _CIRCLE_       3
#define    _RETURN_       4
#define     _BREAK_       5
#define  _CONTINUE_       6
#define      _GOTO_       7
#define      _OPEN_       8
#define     _CLOSE_       9
#define   _DECLARE_      21

/*--------------------------------------------------- Статус прохода */

//           int   Lang_DCL::nPass_sts ;      /* Статус прохода */

#define                    _ACTIVE_PASS   0    /* Активный проход */
#define                      _ELSE_PASS   1    /* Активный проход для ELSE */
#define                   _PASSIVE_PASS   2    /* Пассивный проход */
#define                     _BREAK_PASS   3    /* Пассивный выход из цикла */

/*-------------------------------------------------------------------*/

//#define  _DEFINE_MAX    100
//   struct Dcl_define      dcl_define[_DEFINE_MAX] ;   /* Список DEFINE-ов */
// int      dcl_def_cnt ;               /* Число DEFINE-ов */

/*------------------------------------------------------ Обозначения */

//#define DGT_SZ     sizeof(double)
#define CHR_SZ     sizeof(char)

/*-------------------------------------------------------------------*/


/************************************************************************/
/*                                                                      */
/*        Интерпритатор языка DCL (Data Convertion Language)            */
/*                                                                      */
/*   control  -   Управляющая команда:                                  */
/*                    _DCL_MEMORY - программа в памяти                  */
/*                    _DCL_FILE   - программа в файле                   */
/*                    _DCL_INIT   - сброс и очистка                     */
/*     input  -   Имя файл или буфер с программой                       */
/*      vars  -   Таблица списков переменных                            */

    int  Lang_DCL::Interpritator(     int  control, 
                                     char *input, 
                                 Dcl_decl **vars   )
{
	 char  work[_BUFF_SIZE+2] ;  /* Рабочий буфер */
	  int  work_cnt ;            /* Счетчик текущего буфера ввода */
	 char *mark ;                /* Метка текущей строки */
	 char *mark_target ;         /* Искомая метка */
	  int  mark_flag ;           /* Флаг наличия в строке метки-цели */
	  int  string_flag ;         /* Флаг нахождения внутри символьной строки */
	 long  byte_prv ;            /* Ячейка сохранения счетчика байтов ввода */
	  int  oper_type ;           /* Тип оператора */
	 char  chr_save ;            /* Ячейка сохранения символа */
	 char *for_addr[4] ;         /* Адреса рассечки FOR-оператора */
	  int  block_exp_flag ;      /* Флаг ожидания блока */
	  int  any_flag ;            /* Флаг не IF-ELSE и не FOR оператора */
	  int  close_flag ;          /* Флаг закрытия IF-ELSE и FOR операторов */
	  int  ret_flag ;            /* Флаг отработки оператора RETURN */
	  int  end_flag ;            /* Флаг конца файла */
	  int  else_flag ;           /* Флаг прохода до ELSE или ENDIF оператора */

	 char *text ;
	 char *tmp ;
	 char *addr ;
         char  msg[128] ;
	  int  size ;
	  int  len ;
	  int  i ;
	  int  j ;
	  int  n ;

/*----------------------------------- Дешифровка управляющей команды */

				     mFile_flag= 0 ;                /* Уст.режим работы: файл или память */
	    if(control & _DCL_FILE)  mFile_flag= 1 ;

                    if(mFile_flag)  mError_file=mProgram_name ;
                                    mError_code= 0 ;                /* Сброс индикатора ошибки */

                                    mError_details[0]=0 ;

	   strncpy(mProgram_name, input, sizeof(mProgram_name)-1) ; /* Сохраняем идентификатор программы */

/*--------------------------------------- Обработка сброса и очистки */

  if(control==_DCL_INIT) {                                          /* Если задана инициализация ... */
			    iMem_free() ;                           /*   Закрытие системы управления памятью */
				return(0) ;
			 }
/*----------------------------------------- Открытие файлa программы */

  if(mFile_flag) {
                      mFile_hdl=open(input, O_BINARY | O_RDWR) ;    /* Открытие файла */
                   if(mFile_hdl==-1) {                              /* Если ошибка... */
                                       mError_code=_DCLE_FILE_OPEN ;
                                             return(-1) ;
                                     }
	         }
  else                mFile_mem=input ;

/*---------------------------------------------------- Инициализация */

  if(nInit_flag==0) {                                               /* При заходе в интерпр. 1-го уровня */
                          iCall_init() ;                            /*   Иниц.систему DCL-вызовов */
                      if(mError_code)  return(-1) ;

                           nLocal=1 ;
                    }

     nInit_flag=1 ;

/*-------------------------------- Построение списка процедур модуля */

          iCall_proc_list(nCall_stack[nCall_stack_ptr]) ;
       if(mError_code)  return(-1) ;

/*-------------------------------- Инициализация и размещение памяти */

		getcwd(nCur_path, sizeof(nCur_path)) ;              /* Запрос текущего раздела */

//	    dcl_def_cnt=0 ;                                         /* Сброс счетчика DEFINE-ов */

	   nVars=iMem_init(vars) ;                                  /* Иниц. системы управления памятью */
	if(mError_code)  return(0) ;                                /* Обраб.нехватку памяти */

       memcpy(&nReturn, &nVoid, sizeof(nResult)) ;                  /* Сброс заголовка возврата */
								    /*    в положение VOID      */
	if(nRet_data!=NULL)  free(nRet_data) ;                      /* Освобождаем прежний RETURN-буфер */

/*-------------------------------------------------- Основная работа */
/*-------------------------------------------------------------------*/

		       mRow=nSeg_row ;                              /* Иниц. номера строки */
		  mByte_cnt=nSeg_start ;                            /* Иниц. счетчика байтов */
		mark_target= NULL ;                                 /* Сброс поиска по метке */
		 nNext_flag=  0 ;                                   /* Сброс флага повторного прохода цикла */
		nId_stk_cnt=  0 ;                                   /* Сброс IF-FOR стека */
                  nPass_sts=_ACTIVE_PASS ;                          /* Начинаем с активного прохода */
	     block_exp_flag=  0 ;                                   /* Сброс флага ожидания блока */
		 nOper_next=  0 ;

		   end_flag=0 ;                                     /* Сброс флага конца файла */
		  else_flag=0 ;
		   ret_flag=0 ;
		mError_code=0 ;

  do {                                                              /* CIRCLE.1 - Сканирование по строкам */
		      mRow++ ;                                      /* Коррект. счетчик строк */

#ifndef UNIX

       if(_heapchk()!=_HEAPOK) {
                                  printf("--- MEMORY CRASH\n") ;
                               }
#endif
/*-------------------------------------- Считывание очередной строки */

                  memset(work, 0, sizeof(work)) ;
	 work_cnt=iSysin(work, mByte_cnt, _BUFF_SIZE) ;

      if(work_cnt==0)  {  end_flag=1 ;  break ;  }                  /* Если данных нет - уст.флаг конца файла */

	 tmp=(char *)memchr(work, '\n', work_cnt) ;                 /* Ищем конец строки <ВК> */

      if(tmp==NULL)                                                 /* Если конец строки не найден... */
	if(work_cnt==_BUFF_SIZE) {  mError_code=_DCLE_STRING_LEN ;  /*   Если буфер счит. полон - выдаем      */
						  break ;         } /*     сообщ.об ошибке и прекр.считывание */
	else              tmp=work+work_cnt ;                       /*   Если буфер не переполнен - считаем */
								    /*      концом строки последний символ  */
		    *tmp=0 ;                                        /* Устан.терминатор строки */
	      memset(tmp, 0, _BUFF_SIZE+work-tmp-2) ;               /* Заполняем хвост буфера нулями */

	       byte_prv =mByte_cnt  ;                               /* Сохранение счетчика байтов */
	      mByte_cnt+=tmp-work+1 ;                               /* Устан.счетчик байт на след. строку */

	       tmp=strchr(work, '\r') ;                             /* Подавляем возможную    */
	    if(tmp!=NULL)  *tmp='\0' ;                              /*   комбинацию <RET><NL> */

   if(mDebug_trace)
    if(nPass_sts==_ACTIVE_PASS) {
      if(mDebug_path[0]!=0) {
                                 sprintf(msg, "%3d ", mRow) ;
                                    iLog(msg , work) ;
                            }
      else                  {
                                 printf("%3d %s\n", mRow, work) ;
                            }
                                }
/*----------------- Ужатие пробелов и табуляций, отсечка комментария */

	     string_flag=0 ;

     for(i=0, j=0 ; work[i] ; i++, j++) {                           /* CIRCLE.2 - Сканируем строку ... */
/*- - - - - - - - - - - - - - - - - - - Прохождение символьных строк */
      if(work[i]=='"')  if(string_flag) {                           /* Идентификация и обработка */
			  if(work[i-1]!='\\' ||                     /*   открытия и закрытия     */
			     work[i-2]=='\\'   )  string_flag=0 ;   /*    символьной строки      */
					}
                        else                      string_flag=1 ;

      if(string_flag) {  work[j]=work[i] ;  continue ;  }           /* Проход симв.строки */
/*- - - - - - - - - - - - - - - - - - - - - -  Обработка комментария */
	 if(work[i  ]=='/' &&
	    work[i+1]=='/'   )  break ;
/*- - - - - - - - - - - - - - - - - - -  Ужатие пробелов и табуляций */
       if(work[i]== ' ' ||                                          /* При обнаружении пробела       */
	  work[i]=='\t'   )      j-- ;                              /*  или табуляции - поджимаем их */
       else                 work[j]=work[i] ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
					}                           /* CONTINUE.2 */

         if(string_flag) {                                          /* Если строка не закрыта... */
                            mError_code=_DCLE_NOCLOSE_STRING ;
                                break ;
                         }

		      work[j]=0 ;                                   /* Терминируем строку */

	 if(work[0]==0)  continue ;

/*-------------------------------------------------- Обработка прагм */
  
     if(!memcmp(work, "#pragma", strlen("#pragma"))) {

             text=work+strlen("#pragma") ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  TRACE */
       if(!memcmp(text, "trace", strlen("trace"))) {

                 text+=strlen("trace") ;
             if(*text=='0')  mDebug_trace=0 ;
             else            mDebug_trace=1 ; 
                                                   } 
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  LOG */
       else
       if(!memcmp(text, "log", strlen("log"))) {

                                  text+=strlen("log") ;
             strncpy(mDebug_path, text, sizeof(mDebug_path)-1) ;
                                               } 
/*- - - - - - - - - - - - - - - - - - - - - - - - Неизвестная прагма */
       else                                            {
		    mError_code=_DCLE_PRAGMA_UNK ;
			   break ;
                                                       } 
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                        continue ;
                                                     }
/*-------------------------------------------------- Обработка меток */

     if(mark_target!=NULL)  nPass_sts=_PASSIVE_PASS ;               /* Поиск меток при GOTO - проходе */
								    /*   ведется в пассивном режиме   */

	mark_flag=  0 ;                                             /* Сброс индикатора метки-цели в строке */
	mark     =work ;
	addr     =iFind_close(mark, ':') ;                          /* Ищем разделитель метки */

     if(addr!=NULL) {                                               /* IF.1 - Если подозревается метка ... */
		       *addr= 0 ;                                   /* Терминируем метку */
			 len=addr-mark ;                            /* Опред.длину метки */

	for(i=0 ; i<len; i++)  if(!NAME_CHAR(mark[i])) break ;      /* Контр. состав символов метки */

      if(i==len) {                                                  /* IF.2 - Если синтаксис метки корр. ... */
       if(        mark_target!=NULL &&                              /* Если задан поиск по метке */
	  !strcmp(mark_target, mark)  ) {                           /*   и метка совпадает ...   */
					   free(mark_target) ;      /*  Освоб. буфера искомой метки */
					mark_target=NULL ;          /*  Сбрасываем режим поиска по метке */
					mark_flag  =  1 ;           /*  Фикс.наличие метки-цели в строке */
					  nPass_sts=_ACTIVE_PASS ;  /*  Уст.активный проход */
					}

		       strcpy(work, work+len+1) ;                   /* Сдвигаем строки на метку */
		 }                                                  /* ELSE.2 */
      else       {                                                  /* ELSE.2 - Если синтаксис некорр. ... */
		    mError_code=_DCLE_MARK_SIMBOL ;
			   break ;
		 }                                                  /* END.2 */
		    }                                               /* ELSE.1 */
     else           {                                               /* ELSE.1 */
//                     if(mark_target!=NULL)  continue ;            /* Если задан поиск по метке ... */
		    }                                               /* END.1 */
/*----------------------------------- Разделение строки на подстроки */

	    nSubrow_cnt=1 ;                                         /* Иниц. счетчик подстрок */

    for(addr=work ; ; addr++) {                                     /* Сканируем строку в поисках */
								    /*   разделителя операторов   */
			addr=iFind_close(addr, ';') ;               /*    и при его обнаружении   */
								    /*   вставляем 0-терминатор   */
		     if(addr!=NULL) {   nSubrow_cnt++ ;
					      *addr=0 ;  }
		     else              break ;
			      }
/*----------------------------------- Разделение строки на операторы */

	     nOper_cnt= 0 ;                                         /* Сбрасываем счетчик подстрок */
		  text=work ;

    for(text=work ; nSubrow_cnt>0 ; text++, nSubrow_cnt--) {        /* CIRCLE.3 - Сканир.строку по подстрокам */

     do {                                                           /* CIRCLE.4 - Сканируем подстроку */
	  if(nOper_cnt) nOper[nOper_cnt-1].end=text ;               /* Фиксируем начало и конец подстроки */
		        nOper[nOper_cnt  ].beg=text ;
/*- - - - - - - - - - - - - - - - - -  Обработка описаний переменных */
         if( iDecl_detect(text) ) {
			nOper[nOper_cnt].type =_DECLARE_ ;
				     break ;
  				  }
/*- - - - - - - - - - - - - - - - - - - - - Обработка открытия блока */
    else if(text[0]==_OPEN_BLOCK) {
				    nOper[nOper_cnt].type=_OPEN_ ;
					     text++ ;
				  }
/*- - - - - - - - - - - - - - - - - - - - - Обработка открытия блока */
    else if(text[0]==_CLOSE_BLOCK) {
				     nOper[nOper_cnt].type=_CLOSE_ ;
					     text++ ;
				   }
/*- - - - - - - - - - - - - - - - - - - - - Обработка IF - оператора */
    else if(!memcmp(text, _IF_LEX, _IF_SIZE)) {
		      addr=iFind_close(text+_IF_SIZE, ')') ;        /* Поиск закрывающей скобки оператора */
		   if(addr==NULL) {  mError_code=_DCLE_IF_CLOSE ;   /* Если таковой нет - ошибка */
					     break ;             }
			 nOper[nOper_cnt].type=_IF_ ;
					  text=addr+1 ;
					      }
/*- - - - - - - - - - - - - - - - - - - - Обработка ELSE - оператора */
    else if(!memcmp(text, _ELSE_LEX, _ELSE_SIZE)) {
			 nOper[nOper_cnt].type =_ELSE_ ;
					  text+=_ELSE_SIZE ;
						  }
/*- - - - - - - - - - - - - - - - - - - -  Обработка FOR - оператора */
    else if(!memcmp(text, _CIRCLE_LEX, _CIRCLE_SIZE)) {
		   addr=iFind_close(text+_CIRCLE_SIZE, ')') ;       /* Поиск закрывающей скобки оператора */
		if(addr==NULL) {  mError_code=_DCLE_FOR_CLOSE ;     /* Если таковой нет - ошибка */
					  break ;                }
				 nOper[nOper_cnt].type=_CIRCLE_ ;
						  text=addr+1 ;
						      }
/*- - - - - - - - - - - - - - - - - - - Обработка RETURN - оператора */
    else if(!memcmp(text, _RETURN_LEX, _RETURN_SIZE)) {
			nOper[nOper_cnt].type =_RETURN_ ;

	     if(text[_RETURN_SIZE]=='(') {                          /* Если RETURN с параметром ... */
		   addr=iFind_close(text+_RETURN_SIZE+1, ')') ;     /*   Поиск закрывающей скобки оператора */
		if(addr==NULL) {  mError_code=_DCLE_RETURN_CLOSE ;  /*   Если таковой нет - ошибка */
					  break ;                 }
					    text=addr+1 ;
					 }
	else if(text[_RETURN_SIZE]== 0 )    text+=_RETURN_SIZE ;    /* Если RETURN без параметра ... */
	else                                  break ;               /* Если это не RETURN ... */
						      }
/*- - - - - - - - - - - - - - - - - - -  Обработка BREAK - оператора */
    else if(!strcmp(text, _BREAK_LEX)) {
			  nOper[nOper_cnt].type =_BREAK_ ;
					   text+=_BREAK_SIZE ;
				       }
/*- - - - - - - - - - - - - - - - - - Обработка CONTINUE - оператора */
    else if(!strcmp(text, _CONTINUE_LEX)) {
			     nOper[nOper_cnt].type =_CONTINUE_ ;
					      text+=_CONTINUE_SIZE ;
					  }
/*- - - - - - - - - - - - - - - - - - - - Обработка GOTO - оператора */
    else if(!memcmp(text, _GOTO_LEX, _GOTO_SIZE)) {
			nOper[nOper_cnt].type =_GOTO_ ;
				   break ;
						  }
/*- - - - - - - - - - - - - - - - - -  Обработка описаний переменных */
    else if( iDecl_detect(text) )                 {
			nOper[nOper_cnt].type =_DECLARE_ ;
				   break ;
						  }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    else                                          {
			nOper[nOper_cnt].type =_LEET_ ;
				   break ;
						  }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
	 if(*text==0)  break ;                                      /* Если подстрока закончилась - */
								    /*   - переходим к следующей    */
	      nOper_cnt++ ;                                         /* Сохраняем оператор */

	} while(1) ;                                                /* CONTINUE.4 */

		  if(mError_code)  break ;

		      text=text+strlen(text) ;                      /* Уст. указатель на конец строки */
      nOper[nOper_cnt].end=text+strlen(text) ;                      /* Уст.адрес завершения            */
	    nOper_cnt++ ;                                           /*  последнего оператора подстроки */

							   }        /* CONTINUE.3 */

//               oper_cnt++ ;

/*-------------------------------------------------- Цикл операторов */

  for(nOper_crn=0 ; nOper_crn<nOper_cnt ; nOper_crn++) {            /* CIRCLE.5 - Скан. текст по операторам */

		   oper_type= nOper[nOper_crn].type ;               /* Уст. тип текущего оператора */
			text= nOper[nOper_crn].beg ;                /* Сохраняем следующий за последним    */
	 if(nOper_crn) *text= chr_save ;                            /*  символом подстроки символ с послед. */
		    chr_save=*nOper[nOper_crn].end ;                /*   его восстановлением 'через круг'   */

       if(nNext_flag && nOper_crn<nOper_next)  continue ;           /* Проход 'лишних' операторов    */
								    /*    поиске цикла в NEXT-режиме */
	 *nOper[nOper_crn].end=  0 ;                                /* Терминируем подстроку */

	    if(*text==0)  break ;                                   /* Обрабатываем пустую подстроку */

	 if(block_exp_flag)  block_exp_flag-- ;                     /* 'Понижение' флага ожидания блока */

		  close_flag= 0 ;                                   /* Иниц.флагов анализа         */
		    any_flag= 0 ;                                   /*    IF-ELSE и FOR операторов */

/*-------------------------------------- Инициализация рабочих ячеек */

		    iNext_title(_CLEAR_WORK_VAR) ;                  /* Сброс счетчика рабочих ячеек */
		     iAlloc_var(_CLEAR_WORK_VAR, NULL, 0) ;         /* Очистка рабочих буферов */

/*----------------------------------------- Обработка оператора ELSE */

    if(oper_type==_ELSE_) {                                         /* IF.7 - Если обнар.оператор ELSE ... */

      if(!else_flag) {  mError_code=_DCLE_FREE_ELSE ;               /* При "неожиданном" появлении - */
			           break ;             }            /*   уст.ошибку и прекращ.обраб. */

	      nId_stk[nId_stk_cnt].else_flag =1 ;                   /* Корректируем запись в IF_DO стеке */
	      nId_stk[nId_stk_cnt].block_flag=0 ;

	     if(nPass_sts!=_BREAK_PASS)
		  nPass_sts=nId_stk[nId_stk_cnt].pass_sts ;         /* Извл.статус прохода для IF */

	     if(nPass_sts==_ACTIVE_PASS)  nPass_sts=  _ELSE_PASS ;  /* Опред. статус прохода для ELSE */
	else if(nPass_sts==  _ELSE_PASS)  nPass_sts=_ACTIVE_PASS ;

		nId_stk[nId_stk_cnt].pass_sts=nPass_sts ;           /* Заносим статус прохода */

				  else_flag=0 ;                     /* Сбрасываем ELSE-флаг */
			     block_exp_flag=2 ;                     /* Уст.флаг ожидания блока */
			  }                                         /* END.7 */
/*---------------------------------- Обработка "непоявившегося" ELSE */

   if(else_flag) {

      for( ; nId_stk_cnt>0 ; nId_stk_cnt--) {                       /* Закрыв. все "неблоковые" IF, ELSE и DO */
	  if(nId_stk[nId_stk_cnt].block_flag) break ;               /*  расположенные выше с одновременной    */
	          iIef_close(nId_stk_cnt) ;                         /*   коррекцией счетчика стека            */

	      if(nNext_flag)  break ;
					    }

		    if(mark_flag)  nPass_sts=_ACTIVE_PASS ;         /* Обраб.некорректное закрытие     */
								    /*  IF-оператора при GOTO переходе */
		 }
		      else_flag=0 ;
		      mark_flag=0 ;

/*------------------------------------------ Обработка оператора FOR */

    if(oper_type==_CIRCLE_) {                                       /* IF.5 - Если оператор FOR ... */
/*- - - - - - - - - - - - - - Установление границ закрывающей секции */
		text+=_CIRCLE_SIZE ;
	  for_addr[0]= text ;
	  for_addr[3]= nOper[nOper_crn].end-1 ;
	 *for_addr[3]= 0 ;
/*- - - - - - - - - - - - - - -  Общий контроль и "рассечка" формата */
      for(i=2 ; i>0 ; i--) {                                        /* Ищем точки "рассечки",          */
	  for_addr[i]=strrchr(text, ';') ;                          /*  при их отсутствии -            */
       if(for_addr[i]==NULL) {  mError_code=_DCLE_FOR_TTL_SEL ;     /*   уст.ошибку и прекращ.обраб.,  */
					break ;                 }   /* а при обнаружении - терминируем */
       else                  {  *for_addr[i]=0 ;                    /*  строку в данной точке          */
				 for_addr[i]++ ;           }
			    }

		  if(mError_code)  break ;                          /* Если были ошибки - прекр.работу */
/*- - - - - - - - - - - - - - - Занесение соотв.записи в IF-FOR стек */
      if(!nNext_flag) {                                             /* Если первый проход цикла ... */
			       nId_stk_cnt++ ;                      /*   Переходим на новую запись */
		       nId_stk[nId_stk_cnt].if_for_flag= 1 ;        /*   Формируем запись в IF_DO стеке */
		       nId_stk[nId_stk_cnt].block_flag = 0 ;
		       nId_stk[nId_stk_cnt].addr       =byte_prv ;
		       nId_stk[nId_stk_cnt].oper       =nOper_crn ;
		       nId_stk[nId_stk_cnt].row        =mRow ;

	if(nPass_sts!=_ACTIVE_PASS)                                 /* Если проход не активный - */
		       nId_stk[nId_stk_cnt].pass_sts=_PASSIVE_PASS; /*    уст.пассивный цикл     */
		      }
/*- - - - - - - - - - - - - - - - -  Обработка иниц. и модиф. частей */
   if(nPass_sts==_ACTIVE_PASS) {                                    /* Если активного проход ... */

	     if(nNext_flag)  text=for_addr[2] ;                     /*   Уст.адрес анализируемой части */
	     else            text=for_addr[0] ;

       while(1) {                                                   /*   Обраб. часть по фрагментам с разделителем ',' ... */
		     addr=iFind_close(text, ',') ;                  /*     Ищем разделитель */
		  if(addr!=NULL)  *addr=0 ;                         /*     Терм.фрагмент */

			     iLeet(text) ;                          /*     Обрабатываем фрагмент */

                  if(mError_code==_DCLE_EMPTY)  mError_code=0 ;     /*     Пустое выражение не считается ошибкой */

		  if(mError_code )  break ;                         /*     Если есть ошибка - выход */
		  if(addr==NULL)  break ;                           /*     Если последний фрагмент ... */

		     text=addr+1 ;
		}

				     if(mError_code)  break ;
			       }
/*- - - - - - - - - - - - - - -  Обработка контрольно-выходной части */
   if(nPass_sts==_ACTIVE_PASS) {                                    /* Если активного проход ... */
     if((for_addr[1])[0]!=0)  iCulculate(for_addr[1]) ;             /*   Если есть условное выражение -       */
     else                       nIf_ind=1 ;                         /*    вычисляем его, а при его отсутствии */
								    /*       эмулируем истинние условие       */
	    if(mError_code)  break ;                                /*   Если есть ошибка - выход */

              if(nIf_ind)  nPass_sts=_ACTIVE_PASS ;                 /*   Опред. тип прохода */
              else         nPass_sts= _BREAK_PASS ;

	 nId_stk[nId_stk_cnt].pass_sts=nPass_sts ;                  /*   Заносим статус прохода */
			       }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
				   block_exp_flag= 2 ;              /* Уст.флаг ожидания блока */
                                       nNext_flag= 0 ;
			    }                                       /* END.5 */
/*------------------------------------------- Обработка оператора IF */

    if(oper_type==_IF_) {                                           /* IF.6 - Если оператор IF ... */
			      text+=_IF_SIZE ;                      /* Выделяем и терминируем */
			      addr =nOper[nOper_crn].end-1 ;        /*   условное выражение   */
			     *addr =  0 ;

			     nId_stk_cnt++ ;                        /* Открываем новую запись */
		      nId_stk[nId_stk_cnt].if_for_flag= 0 ;         /* Формируем запись в IF_DO стеке */
		      nId_stk[nId_stk_cnt].else_flag  = 0 ;
		      nId_stk[nId_stk_cnt].block_flag = 0 ;

   if(nPass_sts!=_BREAK_PASS)
     if(nPass_sts==_ACTIVE_PASS) {                                  /* Если идет исполнительный проход ... */
	                           iCulculate(text) ;               /*   Вычисляем условное выражение */
                               if(mError_code)  break ;             /*   Если есть ошибка - выход */

               if(nIf_ind)  nPass_sts=_ACTIVE_PASS ;                /*   Опр.выполняется IF или ELSE */
               else         nPass_sts=  _ELSE_PASS ;
				 }
     else                   nPass_sts=_PASSIVE_PASS ;               /* Если пассивный проход... */

	    nId_stk[nId_stk_cnt].pass_sts=nPass_sts ;               /* Заносим статус прохода */

			   block_exp_flag=2 ;                       /* Уст.флаг ожидания блока */
			}                                           /* END.6 */
/*----------------------------------------- Обработка открытия блока */

    if(oper_type==_OPEN_) {                                         /* IF.8 - Если оператор OPEN ... */

      if(block_exp_flag) {
			     nId_stk[nId_stk_cnt].block_flag=1 ;
			 }
      else               {
			       mError_code=_DCLE_FREE_OPEN ;        /* Если 'свободный' блок - */
				     break ;                        /*   - конец обработки     */
			 }
			  }                                         /* END.8 */
/*----------------------------------------- Обработка закрытия блока */

					 nId_num=0 ;                /* Сбрас.номер закрытого блока */
    if(oper_type==_CLOSE_) {                                        /* IF.9 - Если конец блока ... */

      for(i=0 ; i<=nId_stk_cnt ; i++)                               /* Ищем 'блочный' оператор */
	       if(nId_stk[i].block_flag)  nId_num=i ;

      if(nId_num==0) {  mError_code=_DCLE_FREE_CLOSE ;              /* При "неожиданном" появлении - */
			       break ;                 }            /*   уст.ошибку и прекращ.обраб. */

			     close_flag=1 ;                         /* Уст флаг закрытия */
			   }                                        /* END.5 */
/*------------------------------------- Обработка оператора CONTINUE */

    if(oper_type==_CONTINUE_) {                                     /* IF.9 - Если оператор CONTINUE ... */

      if(nPass_sts==_ACTIVE_PASS) {                                 /* Если активный проход ... */

	for(n=0, i=1 ; i<=nId_stk_cnt ; i++)                        /*   Ищем запись близжайшего цикла */
		  if(nId_stk[i].if_for_flag)  n=i ;

	 if(n==0) {  mError_code=_DCLE_FREE_CONTINUE ;              /*   При отсутствии цикла -        */
			     break ;                   }            /*     уст.ошибку и прекращ.обраб. */

		  mByte_cnt=nId_stk[n].addr  ;
		 nOper_next=nId_stk[n].oper ;
		       mRow=nId_stk[n].row-1 ;
		nId_stk_cnt=        n ;
		 nNext_flag=        1 ;
			break ;
				  }

				any_flag=1 ;
			      }                                     /* END.9 */
/*---------------------------------------- Обработка оператора BREAK */

    if(oper_type==_BREAK_) {                                        /* IF.8 - Если обнар.оператор BREAK ... */

      if(nPass_sts==_ACTIVE_PASS) {                                 /* Если активный проход ... */
	for(n=0,i=1 ; i<=nId_stk_cnt ; i++)                         /*   Ищем запись близжайшего цикла */
		   if(nId_stk[i].if_for_flag)  n=i ;

	 if(n==0) {  mError_code=_DCLE_FREE_BREAK ;                 /*   При отсутствии цикла -        */
			     break ;                }               /*     уст.ошибку и прекращ.обраб. */

		    nPass_sts=_BREAK_PASS ;                         /*   Далее - пасс. проход до конца цикла */
	  nId_stk[n].pass_sts=_BREAK_PASS ;                         /*   Заносим статус прохода */
				  }

				any_flag=1 ;
			   }                                        /* END.8 */
/*----------------------------------------- Обработка оператора GOTO */

    if(oper_type==_GOTO_) {                                         /* IF.5 - Если оператор GOTO ... */

      if(nPass_sts==_ACTIVE_PASS) {                                 /* Если активный проход ... */
		    text+=_GOTO_SIZE ;                              /* Устан.указатель на метку */
		     len =strlen(text) ;                            /* Опред.длину метки */

	      mark_target=(char *)calloc(len+1, 1) ;                /* Выделяем буфер искомой метки */
	   if(mark_target==NULL) {  mError_code=_DCLE_MEMORY ;      /* Если памяти нехватает - */
					    break ;            }    /*   - конец обработки     */

			       strcpy(mark_target, text) ;          /* Заносим метку в буфер */
                                       mByte_cnt=nSeg_start ;       /* Уст.поиск с начала прогр.сигмента */
					    mRow=nSeg_row ;
				     nId_stk_cnt=  0 ;              /* Сброс УЦ-стека */
				       else_flag=  0 ;
                                       nPass_sts=_PASSIVE_PASS ;
					      break ;               /* Выходим на ввод след.строки текста */
				  }
				       any_flag=1 ;
			  }                                         /* END.5 */
/*--------------------------------------- Обработка оператора RETURN */

    if(oper_type==_RETURN_) {                                       /* IF.6 - Если обнар.оператор RETURN ... */

      if(nPass_sts==_ACTIVE_PASS) {                                 /* IF.7 - Если АКТИВНЫЙ проход ... */
				     ret_flag=1 ;                   /* Уст.выполнение RETURN */

			     text+=_RETURN_SIZE ;

		   if(text[0]==0)  break ;                          /* Если RETURN-выражение отсутствует */

	          iCulculate(text) ;                                /* Вычисляем RETURN-выражение */
	     if(mError_code)  break ;                               /* Если есть ошибка - выход */

	   memcpy(&nReturn, &nResult, sizeof(nResult)) ;            /* Форм.заголовок возврата */
		   nReturn.work_nmb=  0 ;

	if(nResult.type!=_CHR_PTR &&                                /* Если возвращается НЕ указатель ... */
	   nResult.type!=_DGT_PTR   ) { 
					    size=nResult.buff ;     /*  Опр.размер буфера RETURN-обьекта */
	    if(nResult.type!=_CHR_AREA)
			   size*=iDgt_size(nResult.type) ;

		     nRet_data=calloc(size, 1) ;                    /*  Размещаем буфер RETURN-обьекта */
	      memcpy(nRet_data, nResult.addr, size) ;               /*  Копируем RETURN-обьект в буфер */
		     nReturn.addr=nRet_data ;                       /*  Уст.адрес данных RETURN-обьекта */
                                      }

				      break ;
				  }                                 /* ELSE.7 */

      else                          any_flag=1 ;                    /* ELSE.7 - Если ПАССИВНЫЙ проход ... */
								    /* END.7 */
			    }                                       /* END.6 */
/*------------------------------------ Обработка описании переменных */

    if(oper_type==_DECLARE_) {                                      /* IF.5 - Если описание переменных ... */

      if(nPass_sts==_ACTIVE_PASS) {                                 /* Если активный проход ... */
				      iDecl_maker(text, NULL) ;     /*  Обраб.описание */
				    if(mError_code)  break ;        /*  Если есть ошибки ... */
		 		  }
				       any_flag=1 ;
			     }                                      /* END.5 */
/*----------------------------------- Обработка оператора присвоения */

   if(oper_type==_LEET_) {

	if(nPass_sts==_ACTIVE_PASS)  iLeet(text) ;

			if(mError_code)  break ;

			     any_flag=1 ;
			 }
/*------------------- Специальная обработка IF-ELSE и FOR операторов */

   if(!close_flag)
    if(any_flag && nId_stk_cnt)                                     /* Закрытие неблоковых операторов */
	if(!nId_stk[nId_stk_cnt].block_flag)  close_flag=1 ;

/*---------------------- Обработка закрытия IF-ELSE и FOR операторов */

    if(close_flag) {
		       if(nId_num==0)  nId_num=nId_stk_cnt ;

				       iIef_close(nId_num) ;        /* Закрываем оператор */

	  if(nId_stk[nId_num].if_for_flag ||                        /* Для операторов ELSE, FOR... */
	     nId_stk[nId_num].else_flag     ) {
//???????????                                 nId_stk_cnt=nId_num ;
	      if(nNext_flag)  break ;

		      nId_stk_cnt=nId_num-1 ;

	     for( ; nId_stk_cnt>0 ; nId_stk_cnt--) {                /*   Закрываем все "неблоковые" ELSE и DO */
		 if( nId_stk[nId_stk_cnt].block_flag   )  break ;   /*    расположенные выше с одновременной  */
		 if(!nId_stk[nId_stk_cnt].if_for_flag &&            /*     коррекцией счетчика стека          */
		    !nId_stk[nId_stk_cnt].else_flag     )  break ;

				 iIef_close(nId_stk_cnt) ;
			  if(nNext_flag) break ;
						   }

	     if(!nNext_flag                       &&
		!nId_stk[nId_stk_cnt].if_for_flag &&
		!nId_stk[nId_stk_cnt].else_flag     )  else_flag=1 ;
	     else                                      else_flag=0 ;
					     }
	  else                               {
                             nId_stk[nId_num].block_flag= 0 ;
					     nId_stk_cnt=nId_num ;
                                               else_flag= 1 ;
					     }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
		   }
/*-------------------------------------------------------------------*/
					               }            /* CONTINUE.5 */

		       } while(mError_code==0 && !ret_flag) ;       /* CONTINUE.1 - Пока нет ошибок */

/*-------------------------------------------- Заключительный анализ */


/*-------------------------- Закрытие файлов и освобождение ресурсов */
/*-------------------------------------------------------------------*/


  if(nLocal==1) {
                         iCall_final() ;                            /* Выполнение процедур финального освобождения ресурсов библиотек */

       for(i=0 ; i<nT_blocks_cnt ; i++)  free(nT_blocks[i].data) ;  /* Освобождение памяти текстовых блоков */

                    if(nT_blocks!=NULL)  free(nT_blocks) ;
                } 
       
  if(!nFix_flag)  iMem_free() ;                                     /* Закрытие системы управления памятью */

//    for(i=0 ; i<dcl_def_cnt ; i++)   free(dcl_define[i].name) ;   /* Освобождение памяти DEFINE-ов */

  if(mFile_flag)  close(mFile_hdl) ;

/*-------------------------------------------------------------------*/

    return(mError_code) ;
}


/*********************************************************************/
/*                                                                   */
/*                 Процессор выражений присвоения                    */
/*                                                                   */
/*      Обрабатываются следующие виды выражений присвоения:          */
/*               =    -  присвоение                                  */
/*              <==   -  копирование                                 */
/*              <++   -  добавление (копирование в конец)            */
/*              @=    -  литерально-числовое присвоение              */
/*                                                                   */
/*      Допустимые виды выражений присвоения -                       */
/*  (Ниже приняты следущие обозначения:                              */
/*             d - число                                             */
/*             a - массив чисел                                      */
/*            pa - Указатель на число                                */
/*             c - символ (односимвольная строка)                    */
/*             s - символьная строка                                 */
/*            ps - указатель на символьную строку )                  */
/*                                                                   */
/*       Операция присвоения :   d={ d | c }                         */
/*                              pa={ a | s | ps | pa }               */
/*                               c={ d }                             */
/*                              ps={ a | s | ps | pa }               */
/*                                                                   */
/*       Операция копирования :        d  <=={ s | ps }              */
/*                              { a | pa }<=={ a | pa | s | ps }     */
/*                              { s | ps }<=={ d | a | pa | s | ps } */
/*               копирование строк в числовые массивы                */
/*          производится по 'внутренней' границе числа - 8 байт.     */
/*                                                                   */
/*       Операция ЛЧ-присвоения :  { d | s | ps }@= d                */
/*                                                                   */

    char *Lang_DCL::iLeet(char *string)
{
  Dcl_decl  dest ;          /* Описание 'приемника' */
  Dcl_decl *var ;           /* Адрес заголовка простой переменной */
      char *leet ;          /* Указатель на оператор присвоения/копирования */
      char *culc ;          /* Вычисляемое выражение */
       int  oper_type ;     /* Тип операции:         */
#define       _EQU_OPER  0  /*   Простое присвоение             */
#define      _COPY_OPER  1  /*   Копирование                    */
#define      _APND_OPER  2  /*   Добавление                     */
#define      _CNUM_OPER  3  /*   Литерально-числовое присвоение */
       int  insert_flag ;   /* Флаг режима вставки */
       int  v_type ;        /* Базовый тип переменной приемника */
       int  d_type ;        /* Базовый тип 'приемника' */
       int  r_type ;        /* Базовый тип результата */
       int  d_mode ;        /* Уточняющий тип 'приемника' */
       int  r_mode ;        /* Уточняющий тип результата */
      char *addr ;
       int  buff ;
       int  size ;
      char *addr_c ;
    double *addr_d ;
    double  value ;
      char *incl_begin ;    /* Начало INCLUDE-области */
      char *incl_end  ;     /* Конец INCLUDE-области */
       int  incl_shift ;    /* Сдбиг хвоста при INCLUDE операции */
       int  len ;
      char  conv[128] ; 
      char *end ;

/*---------------------------------------------------- Инициализация */

   if(string==NULL) {  nIf_ind=0 ;                                  /* Обработка особой ситуации */
			return(NULL) ;  }

/*---------------------------------- Выделение операторов присвоения */

                     oper_type=-1 ;
                        d_type= 0 ;
                          culc="" ;
/*- - - - - - - - - - - - - - - - - - - - Поиск операторa присвоения */
     for(leet=string ; ; leet++) {                                  /* Сканируем строку в поисках   */
	       leet=iFind_close(leet, '=') ;                        /*  оператора присвоения = ,    */
	    if(leet==NULL)  break ;                                 /*    игнорируя при этом        */
								    /*  другие операторы содержащие */
	    if(!OPER_CHAR(leet[ 1]) &&                              /*  символ '='                  */
	       !OPER_CHAR(leet[-1])   )   break ;
				 }

	if(leet!=NULL) {  oper_type=_EQU_OPER ;                     /* Если операция присвоения найдена - */
			       culc=leet+1 ;    }                   /*   уст.соотв. тип операции          */
/*- - - - - - - - - - - - - - - - - - -  Поиск оператора копирования */
   if(leet==NULL) {                                                 /* IF.0 - Если оп.присв. отсутствует ... */
     for(leet=string ; ; leet++) {                                  /* Сканируем строку в поисках */
	       leet=iFind_close(leet, '<') ;                        /*  оператора копирования <== */
	    if(leet==NULL)  break ;

	    if(leet[ 1]=='=' && leet[2]=='=')  break ;
				 }

	if(leet!=NULL) {  oper_type=_COPY_OPER ;                    /* Если операция копирования найдена - */
			       culc=leet+3 ;     }                  /*   уст.соотв. тип операции           */
		  }                                                 /* END.0 */
/*- - - - - - - - - - - - - - - - - - -  Поиск оператора копирования */
   if(leet==NULL) {                                                 /* IF.0 - Если оп.присв. отсутствует ... */
     for(leet=string ; ; leet++) {                                  /* Сканируем строку в поисках */
	       leet=iFind_close(leet, '<') ;                        /*  оператора копирования <++ */
	    if(leet==NULL)  break ;

	    if(leet[ 1]=='+' && leet[2]=='+')  break ;
				 }

	if(leet!=NULL) {  oper_type=_APND_OPER ;                    /* Если операция копирования найдена - */
			       culc=leet+3 ;     }                  /*   уст.соотв. тип операции           */
		  }                                                 /* END.0 */
/*- - - - - - - - - - - - - - - - - -  Поиск оператора ЛЧ-присвоения */
   if(leet==NULL) {                                                 /* IF.1 - Если оп.присв. отсутствует ... */
     for(leet=string ; ; leet++) {                                  /* Сканируем строку в поисках */
	       leet=iFind_close(leet, '@') ;                        /*  оператора ЛЧ-присвоения @= */
	    if(leet==NULL)  break ;

		if(leet[ 1]=='=')  break ;
				 }

	if(leet!=NULL) {  oper_type=_CNUM_OPER ;                    /* Если операция ЛЧ-присвоения найдена - */
			       culc=leet+2 ;  }                     /*   уст.соотв. тип операции             */
		  }                                                 /* END.1 */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
	  if(leet==NULL)  culc=string ;                             /* Если операторы присвоения    */
								    /*   или копирования не найдены */
/*-------------------------------------------- Отработка 'приемника' */

  if(leet!=NULL) {                                                  /* Если задана операция присвоения ... */
    if(string==leet) {  mError_code=_DCLE_EMPTY_DEST ;              /*   Если нет 'приемника' у операции - */
			          return(NULL) ;        }           /*     ошибка и выход из обработки     */

	var=iComplex_var(string, leet-1, &dest) ;                   /*   'Извлекаем' приемник */

		      if(mError_code)  return(NULL) ;               /*   Обраб.ошибок... */

	    v_type=t_base(var->type) ;                              /*   Извл.базового типа переменной приемника */
	    d_type=t_base(dest.type) ;                              /*   Извлечение базового и           */
	    d_mode=t_mode(dest.type) ;                              /*     уточняющего типов 'приемника' */

       if(nFragm_flag)  insert_flag=1 ;                             /*   Если приемник содержит фрагментарный */
       else             insert_flag=0 ;                             /*    индекс - уст.режим вставки          */
		 }
/*---------------------------------- Вычисление выражения присвоения */

	   iCulculate(culc) ;                                       /* Вычисляем выражение */

      if(mError_code)  return(NULL) ;                               /* Обраб.ошибок... */

      if(leet==NULL)  return(NULL) ;                                /* Если приемник не указан ... */

	    r_type=t_base(nResult.type) ;                           /*   Извлечение базового и          */
	    r_mode=t_mode(nResult.type) ;                           /*     уточняющего типов результата */

/*------------------------- Операция литерально-числового присвоения */
/*-------------------------------------------------------------------*/

 if(oper_type==_CNUM_OPER) {                                        /* IF.2 */

/*------------------------------------- Приобразование строка->число */

    if( d_type==_DGT_VAL     && 
       (r_type==_CHR_AREA ||
        r_type==_CHR_PTR    )  ) {

          if(nResult.size>=sizeof(conv))  nResult.size=sizeof(conv) ;

                       memset(conv, 0, sizeof(conv)) ;
                       memcpy(conv, nResult.addr, nResult.size) ;
              value=iStrToNum(conv, &end) ;

					r_type=_DGT_VAL ;           /* Соотв.образом модифицируем */
					r_mode=_DCLT_DOUBLE ;       /*  описание результата       */
			          nResult.addr= &value ;
			          nResult.size=   1 ;
			          nResult.buff=   1 ;
        
                                     oper_type=_EQU_OPER ;          /* Далее выполняем простую операцию присвоения */
			         }
/*-------------------------------------- Присвоение символьных строк */

    else 
    if( r_type==_DGT_VAL     && 
       (d_type==_CHR_AREA ||
        d_type==_CHR_PTR    )  ) {

         if(nResult.addr!=NULL &&
	       dest.addr!=NULL   ) {

			len= sizeof(conv)-3 ;                       /* Преобразуем числовой результат   */
		      value=iDgt_get(nResult.addr, r_mode) ;        /*  в его литеральное представление */
                                gcvt(value, len, conv) ;

                           len=strlen(conv) ;                       /* Убираем точку у целых чисел */
                   if(conv[len-1]=='.')  conv[len-1]=0 ;

			          nResult.addr=       conv ;        /* Соотв.образом модифицируем */
			          nResult.size=strlen(conv) ;       /*  описание результата       */
			          nResult.buff=strlen(conv) ;
					r_type=_CHR_AREA ;

				     oper_type=_COPY_OPER ;         /* Далее выполняем операцию копирования */
				   }
				  }
/*-------------------------------------------------------------------*/

    else                           mError_code=_DCLE_DIF_TYPE ;
			   }                                        /* END.2 */

/*---------------------------------- Операция копирования/добавления */
/*-------------------------------------------------------------------*/

 if(oper_type==_COPY_OPER ||                                        /* IF.3 */
    oper_type==_APND_OPER   ) {

  if(nResult.addr!=NULL &&                                          /* IF.4 - Если результат и приемник */
        dest.addr!=NULL   ) {                                       /*           определены ...         */

/*-------------------------- Преобр. чиселовых переменных в строчные */

   if(r_type==_DGT_VAL  ||
      r_type==_DGT_AREA ||
      r_type==_DGT_PTR    ) {
                              nResult.size*=iDgt_size(r_mode) ;
                              nResult.buff*=iDgt_size(r_mode) ;
			    }

   if(d_type==_DGT_VAL  ||
      d_type==_DGT_AREA ||
      d_type==_DGT_PTR    ) {
			       dest.size*=iDgt_size(r_mode) ;
			       dest.buff*=iDgt_size(r_mode) ;
			    }
/*--------------------------------------------- Копирование вставкой */

    if(insert_flag) {                                               /* IF.5 */

      if(oper_type==_APND_OPER) {
                              mError_code=_DCLE_BAD_OPERATION ;
                                        return(NULL) ;
                                }

		  incl_begin=(char *)dest.addr ;                    /* Фиксируем параметры INCLUDE-области */
		  incl_end  =(char *)dest.addr+dest.size ;
		  incl_shift=     nResult.size-dest.size ;

	 if(dest.size>=nResult.size) {                              /* Сдвиг 'хвоста' обьекта        */
	      memmove((char *)dest.addr+nResult.size,               /*  под размер вставки с         */
		      (char *)dest.addr+dest.size,                  /*   усечением при необходимости */
			      dest.buff-dest.size       ) ;         /*     выступающего из буфера    */
				     }                              /*       фрагмента хвоста        */
	 else                        {
	   if(nResult.size>dest.buff) nResult.size=dest.buff ;

		       size=dest.buff-nResult.size ;
	    if(size)
	      memmove((char *)dest.addr+nResult.size,
		      (char *)dest.addr+dest.size, size ) ;
				     }

		  memmove(dest.addr, nResult.addr,                  /* Собственно копирование */
				     nResult.size*CHR_SZ) ;
			  dest.size+=nResult.size-dest.size ;       /* Коррекция размера обьекта */
                    }                                               /* ESLE.5 */
/*----------------------------------- Простое копирование/добавление */

    else            {                                               /* ESLE.5 */
/*- - - - - - - - - - - - - - - - -  Обработка динамических массивов */
       if( var->prototype!=NULL &&                                  /* Если динамический массив... */
          *var->prototype== 'D'   ) {

         if(oper_type==_APND_OPER)  buff=dest.size+nResult.size ;
         else                       buff=nResult.size ; 

         if(buff>dest.buff) {                                       /* Переразмещение памяти  */
                                iBuff_realloc(var, &dest, buff) ;
		      if(mError_code)  return(NULL) ;
                            }
                                    }
/*- - - - - - - - - - - - - - - - - - - - - - -  Выполнение операции */
       if(oper_type==_APND_OPER) {
                                     addr=(char *)dest.addr+dest.size ; 
                                     buff=        dest.buff-dest.size ;
                                 }
       else                      {
                                     addr=(char *)dest.addr ; 
                                     buff=        dest.buff ;
                                 }

       if(nResult.size>buff)  size=buff ;                           /* Размер копируемого фрагмента        */
       else                   size=nResult.size ;                   /*  не должен превышать размера буфера */

       if(size>0) {
                        memmove(addr, nResult.addr, size*CHR_SZ) ;  /* Собственно копирование */
                    if(oper_type==_APND_OPER)  dest.size+=size ;    /* Уст.размер обьекта */
                    else                       dest.size =size ;
                  }
       else       {
		    if(oper_type==_COPY_OPER)   dest.size=0 ;       /* Уст.размер обьекта */
                  }      

                    }                                               /* END.5 */
/*--------------------------- Занесение размеров строчных переменных */

      if(d_type==_CHR_AREA ||
	 d_type==_CHR_PTR    )
			  if(insert_flag)                      ;
			  else             var->size=dest.size ;

/*-------------------------------------------------------------------*/
			    }                                       /* END.4 */
			      }                                     /* END.3 */
/*---------------------------------------------- Операция присвоения */
/*-------------------------------------------------------------------*/

 if(oper_type== _EQU_OPER) {                                        /* IF.6 */

/*------------------------------------------------- Присвоение чисел */

	 if(r_type==_DGT_VAL &&
	    d_type==_DGT_VAL   ) {

	      if(nResult.addr!=NULL &&
		    dest.addr!=NULL   ) {

 	        value=iDgt_get(nResult.addr, r_mode) ;
	 	      iDgt_set(value, dest.addr, d_mode) ;
                                      dest.size=1 ;
					}
				 }
/*----------------------------------- Преобразование число -> символ */

    else if(   r_type==_DGT_VAL  &&
	       d_type==_CHR_AREA &&
	    dest.buff==  1         ) {

			   addr_d=(double *)nResult.addr ;
			   addr_c=(  char *)dest.addr ;

	   if(addr_d!=NULL &&
	      addr_c!=NULL   ) {
		      *addr_c=(char)iDgt_get(addr_d, r_mode) ;
		    dest.size=  1 ;
			       }
				     }
/*----------------------------------- Преобразование символ -> число */

    else if(      r_type==_CHR_AREA &&
	          d_type==_DGT_VAL  &&
	    nResult.buff==  1         ) {

				    addr_c=(  char *)nResult.addr ;
				    addr_d=(double *)   dest.addr ;
				     value=*addr_c ;
	      if(addr_d!=NULL &&
		 addr_c!=NULL   ) iDgt_set(value, addr_d, d_mode) ;

					}
/*-------------------------------------------- Присвоение указателей */

    else if((r_type== _DGT_AREA ||
	     r_type== _DGT_PTR  ||
	     r_type== _CHR_AREA ||
	     r_type== _CHR_PTR  ||
	     r_type==_NULL_PTR    ) &&
	    (d_type== _DGT_PTR  ||
	     d_type== _CHR_PTR    ) &&
	     v_type==  d_type         ) {


	 if(r_type==_DGT_AREA ||
	    r_type==_DGT_PTR    ) {                                 /* Если результат - числовой указатель, */
		         nResult.size*=iDgt_size(r_mode) ;          /*  то переводим его в символьный       */
		         nResult.buff*=iDgt_size(r_mode) ;
				  }

				   var->addr=nResult.addr ;         /* Присваиваем указатель */
				   var->size=nResult.size ;
				   var->buff=nResult.buff ;

	 if(v_type==_DGT_PTR) {                                     /* Если приемник - числовой указатель, */
		    var->size/=iDgt_size(var->type) ;               /*  то соотв. корректируем его         */
		    var->buff/=iDgt_size(var->type) ;
			      }
					}
/*-------------------------------------------------------------------*/

    else if(d_type==_DGT_AREA ||
	    d_type==_CHR_AREA   )  mError_code=_DCLE_CONST_DEST ;

    else                           mError_code=_DCLE_DIF_TYPE ;
			   }                                        /* END.6 */

/*----------------------------- Следящая коррекция размеров обьектов */
/*-------------------------------------------------------------------*/

  if(dest.addr!=NULL) {

   if(!mError_code)
    if(insert_flag)  iIncl_correct(&dest, incl_begin,
                                          incl_end, incl_shift) ;
    else             iSize_correct(&dest) ;

                      }
/*-------------------------------------------------------------------*/
/*-------------------------------------------------------------------*/

  return(NULL) ;
}


/************************************************************************/
/*                                                                      */
/*         П/п ввода текста программы из файла или памяти               */
/*                                                                      */
/*    text  -   Буфер ввода                                             */
/*    addr  -   Адрес                                                   */
/*    size  -   Счетчик ввода                                           */

   int  Lang_DCL::iSysin(char *text, long  addr, int  size)

{
  char *end ;
   int  cnt ;


     if(mDebug_flag && mDebug!=NULL)  mDebug() ;

     if(mFile_flag) {
			 lseek(mFile_hdl, addr, SEEK_SET) ;
		      cnt=read(mFile_hdl, text, size) ;
                    }
     else           {
                      if(mFile_mem==NULL)  return(0) ;

			            memmove(text, mFile_mem+addr, size) ;
			 end=(char *)memchr(text, 0, size) ;

		      if(end!=NULL)  cnt=end-text ;
		      else           cnt=size ;
                    }

 return(cnt) ;
}


/************************************************************************/
/*                                                                      */
/*              Закрытие IF, ELSE и FOR операторов                      */
/*                                                                      */
/*   pnt  -  Указатель строки стека                                     */

  int  Lang_DCL::iIef_close(int  pnt)

{
//   int  n ;

/*------------------------------------------- Закрытие оператора FOR */

       if(nId_stk[pnt].if_for_flag) {

	 if(nPass_sts==_ACTIVE_PASS) {                              /* Если активный проход - */
				 mByte_cnt =nId_stk[pnt].addr  ;    /*  отрабатываем операцию */
				 nOper_next=nId_stk[pnt].oper ;     /*   CONTINUE             */
				      mRow =nId_stk[pnt].row-1 ;
				 nNext_flag=       1 ;
				     }
    else if(nPass_sts== _BREAK_PASS) {                              /* Если выход из ЭТОГО цикла - */
	    if(nId_stk[pnt].pass_sts==_BREAK_PASS)                  /*  - включаем активный проход */
				  nPass_sts=_ACTIVE_PASS ;
		 		     }
				    }
/*------------------------------------------ Закрытие оператора ELSE */

  else if(nId_stk[pnt].else_flag) {
//         if(nPass_sts==_ELSE_PASS)  nPass_sts=_ACTIVE_PASS ;

	 if(nPass_sts!=_BREAK_PASS) {
                                      nPass_sts=nId_stk[pnt].pass_sts ;
	   if(nPass_sts==_ELSE_PASS)  nPass_sts=_ACTIVE_PASS ;
				   }

				  }
/*-------------------------------------------- Закрытие оператора IF */

  else                           {
	 if(nPass_sts!=_BREAK_PASS) {
                                      nPass_sts=nId_stk[pnt].pass_sts ;
	   if(nPass_sts==_ELSE_PASS)  nPass_sts=_ACTIVE_PASS ;
				    }
				 }
/*-------------------------------------------------------------------*/

  return(0) ;
}


/*********************************************************************/
/*                                                                   */
/*                  Стекование параметров интерпритатора             */

  typedef struct {
                     char   program[FILENAME_MAX] ;  /* Идентификатор программы */
                      int   fix_flag ;               /* Флаг сохранения систем интерпритатора */
               Dcl_if_for   id_stk[_IFFOR_MAX] ;     /* Стек IF-DO операторов */
                      int   id_stk_cnt ;             /* Указатель стека IF-DO операторов */
                      int   id_num ;                 /* Указатель "крайнего блока" IF_DO стека */
                      int   subrow_cnt ;             /* Счетчик подстрок */
                 Dcl_oper   oper[_OPER_MAX] ;        /* Массив описания операторов */
                      int   oper_cnt ;               /* Счетчик операторов */
                      int   oper_crn ;               /* Номер текущей оператора */
                      int   oper_next ;              /* Номер оператора при возврате на начало цикла */
                     long   seg_start ;              /* Начальный адрес сегмента */
                      int   seg_row  ;               /* Начальная строка сегмента */
                      int   pass_sts ;               /* Статус прохода */
                      int   file_flag ;              /* Флаг размещения программы в файле */
                      int   file ;                   /* Указатель файла программы */
                     char  *memory ;                 /* Буфер программы в памяти */
                     long   byte_cnt ;               /* Счетчик байтов ввода */
                      int   next_flag ;              /* Флаг повторного прохода цикла */
                 Dcl_decl **vars ;                   /* Описание переменных и процедур */
               Dcl_tblock  *tblock ;                 /* Список текстовых блоков */
                      int   tblock_cnt ;
		 } Dcl_stack ;


      int  Lang_DCL::Stack(int  oper_code)

{
         Dcl_stack *addr ;

  static Dcl_stack *stack[_STACK_MAX] ;
  static       int  stack_cnt ;

/*--------------------------------------------- Запись данных в стек */

  if(oper_code==_DCL_PUSH) {
/*- - - - - - - - - - - - - - - - - - - - - - - - - Подготовка стека */
     if(stack_cnt==_STACK_MAX) {  mError_code=_DCLE_STACK_OVR ;     /* Если стек заполнен... */
						return(-1) ;    }

	   addr=(Dcl_stack *)calloc(sizeof(*addr), 1) ;             /* Выделяем память под стек */
	if(addr==NULL) {  mError_code=_DCLE_STACK_MEM ;             /* Если памяти нет... */
				   return(-1) ;         }

	   stack[stack_cnt]=addr ;                                  /* Заносим адрес данных в стек */
		 stack_cnt++    ;
/*- - - - - - - - - - - - - - - - - - - - -  Занесение данных в стек */
     memcpy(addr->program, mProgram_name, sizeof(mProgram_name)) ;
	    addr->fix_flag  =nFix_flag ;
     memcpy(addr->id_stk, nId_stk, sizeof(nId_stk)) ;
	    addr->id_stk_cnt=nId_stk_cnt ;
	    addr->id_num    =nId_num ;
	    addr->subrow_cnt=nSubrow_cnt ;
     memcpy(addr->oper, nOper, sizeof(nOper)) ;
	    addr->oper_cnt  =nOper_cnt ;
	    addr->oper_crn  =nOper_crn ;
	    addr->oper_next =nOper_next ;
	    addr->seg_start =nSeg_start ;
	    addr->seg_row   =nSeg_row ;
	    addr->pass_sts  =nPass_sts ;
	    addr->file_flag =mFile_flag ;
	    addr->file      =mFile_hdl ;
	    addr->memory    =mFile_mem ;
	    addr->byte_cnt  =mByte_cnt ;
	    addr->next_flag =nNext_flag ;
	    addr->vars      =nVars ;
	    addr->tblock    =nT_blocks ;
	    addr->tblock_cnt=nT_blocks_cnt ;
/*- - - - - - - - - - - - - - - - - - - - - - - Инициализация данных */
		        nFix_flag=0 ;                               /* Флаг сохранения систем интерпритатора */
			  nId_num=0 ;                               /* Указ."крайнего блока" IF_DO стека */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
		       }
/*--------------------------------------- Извлечение данных из стека */

  if(oper_code==_DCL_POP) {
/*- - - - - - - - - - - - - - - - - - - - - - - - - Подготовка стека */
       if(stack_cnt<=0) {  mError_code=_DCLE_STACK_UND ;            /* Если стек пуст... */
                                        return(-1) ;      }

                      stack_cnt-- ;
	   addr=stack[stack_cnt] ;                                  /* Извлекаем адрес данных из стек */
/*- - - - - - - - - - - - - - - - - - Восстановление данных из стека */
   if(mError_code==0)  memcpy(mProgram_name, addr->program,         /* Идентиф.программы восст. только */
					 sizeof(mProgram_name)) ;   /*     при отсутствии ошибок       */

	      nFix_flag  =addr->fix_flag   ;
     memcpy(nId_stk,      addr->id_stk, sizeof(nId_stk)) ;
	    nId_stk_cnt  =addr->id_stk_cnt ;
	    nId_num      =addr->id_num     ;
	    nSubrow_cnt  =addr->subrow_cnt ;
     memcpy(nOper,        addr->oper, sizeof(nOper)) ;
	    nOper_cnt    =addr->oper_cnt   ;
	    nOper_crn    =addr->oper_crn   ;
	    nOper_next   =addr->oper_next  ;
	    nSeg_start   =addr->seg_start  ;
	    nSeg_row     =addr->seg_row    ;
	    nPass_sts    =addr->pass_sts   ;
	    mFile_flag   =addr->file_flag  ;
	    mFile_hdl    =addr->file       ;
	    mFile_mem    =addr->memory     ;
	    mByte_cnt    =addr->byte_cnt   ;
	    nNext_flag   =addr->next_flag  ;
	    nVars        =addr->vars       ;
	    nT_blocks    =addr->tblock     ;
	    nT_blocks_cnt=addr->tblock_cnt ;
/*- - - - - - - - - - - - - - - - - - - - - - -  Освобождение памяти */
			     free(addr) ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
		      }
/*-------------------------------------------------------------------*/

		   iCulc_stack(oper_code) ;
  if(!mError_code)  iMem_stack(oper_code) ;

    return(0) ;
}
