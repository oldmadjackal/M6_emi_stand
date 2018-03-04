/*   Bolotov P.D.  version  29.04.1994   */
/*     0 warnings at warning level 1     */

/*********************************************************************/
/*                                                                   */
/*                   DATA CONVERTION LANGUAGE                        */
/*                                                                   */
/*                ПОДСИСТЕМА РАБОТЫ С ПРОЦЕДУРАМИ                    */
/*                                                                   */
/*********************************************************************/

#if defined(AIX) || defined(LINUX)
#ifndef UNIX
#define  UNIX
#endif
#endif


#include  <stddef.h>
#include  <stdlib.h>
#include  <string.h>
#include  <malloc.h>
#include  <ctype.h>
#include  <math.h>
#include  <fcntl.h>
#include  <stdio.h>
#include  <errno.h>
#include  <sys/types.h>
#include  <sys/stat.h>

#ifdef UNIX
#include  <unistd.h>
#define   stricmp  strcasecmp
#else
#include  <io.h>
#endif

#define DCL_INSIDE

#include "dcl.h"


#pragma warning(disable : 4996)

/*********************************************************************/
/*                                                                   */
/*            Инициализация системы подгрузки DCL-процедуры          */

    void  Lang_DCL::iCall_init(void)

{
/*------------------------------ Формирование описния main-процедуры */

         memset(&nCall_main, 0, sizeof(nCall_main)) ;

         strcpy(nCall_main.name,    "main") ;
                nCall_main.path     =mProgram_name ;
                nCall_main.mem_flag =!mFile_flag ;
                nCall_main.file_addr=mByte_cnt ;
                nCall_main.mem_addr =mFile_mem ;

         strcpy(nCall_main.decl.name, nCall_main.name) ;
                nCall_main.decl.func_flag=   1 ;
                nCall_main.decl.work_nmb = DCL_WORK_NUM(0, _DCL_MAIN_P) ;
                nCall_main.decl.addr     =_DCL_LOAD ;

/*--------------------------------------------- Иниц.списка процедур */

       nCall_list    =(Dcl_call *)
                       calloc(_CALL_MAX, sizeof(Dcl_call)) ;

       nCall_list[0]=nCall_main ;

       nCall_list_cnt=  1 ;

/*-------------------------------------- Иниц.стека вызовов процедур */

       nCall_stack    =(Dcl_call **)
                        calloc(_STACK_MAX, sizeof(*nCall_stack)) ;
       nCall_stack_ptr=  0 ;

       nCall_stack[0]=&nCall_list[0] ;

/*-------------------------------------------------------------------*/

}


/*********************************************************************/
/*                                                                   */
/*       Освобождение ресурсов системы подгрузки DCL-процедуры       */

    void  Lang_DCL::iCall_free(void)

{
   if(nCall_list !=NULL)  free(nCall_list) ;
                               nCall_list=NULL ;

   if(nCall_stack!=NULL)  free(nCall_stack) ;
                               nCall_stack=NULL ;

       nCall_stack_ptr= 0 ;
}


/*********************************************************************/
/*                                                                   */
/*                    Поиск DCL-процедуры                            */
/*                                                                   */
/*   Номер в описания в таблице адресации DCL-процедур передается    */
/*    через ячейку .work_num .                                       */

  Dcl_decl *Lang_DCL::iCall_find(char *name)

{
 Dcl_call *call_addr ;  /* Описание адресации */
      int  ctlg ;
      int  n ;
      int  i ;

/*---------------------------------------------------- Инициализация */

                    mError_code=  0 ;

                      call_addr=NULL ;

/*---------------------- Поиск в таблице "внутри-модульных" процедур */

#define  CURRENT_MODULE   nCall_stack[nCall_stack_ptr]

    for(i=0 ; i<CURRENT_MODULE->proc_cnt ; i++)
      if(!strcmp(CURRENT_MODULE->proc_list[i].name, name))
       if(CURRENT_MODULE->proc_list[i].v_flag==0) {                 /* Если локальная процедура - выдаем на выход */  
                      return(&CURRENT_MODULE->proc_list[i].decl) ;
                                                  }
       else                                       {                 /* Если виртуальная процедура - ищем верхнюю по стеку */
             for(n=0 ; n<=nCall_stack_ptr ; n++)
               if(!strcmp(nCall_stack[n]->proc_list[i].name, name) &&
                          nCall_stack[n]->proc_list[i].v_flag==1     )
                                  return(&nCall_stack[n]->proc_list[i].decl) ;
                                                  }

#undef   CURRENT_MODULE

/*--------------------------- Поиск среди ранее вызывавшихся модулей */

    for(i=0 ; i<nCall_list_cnt ; i++)                               /* Ищем процедуру по кэш-листу модулей */
       if(!strcmp(nCall_list[i].decl.name, name))                   /*  Если процедура найдена в списке -  */
			    return(&nCall_list[i].decl) ;           /*  - выдаем ее описание на выход      */

/*------------------------------------------ Поиск в текущем разделе */

   if(call_addr==NULL &&                                            /* Если процедура НЕ найдена */
      mWorkDir !=NULL   ) {
                            call_addr=iCall_file(name, mWorkDir) ;  /* Ищем процедуру сначала в виде            */
       if(call_addr==NULL)  call_addr=iCall_lib (name, mWorkDir) ;  /*  отдельного файла, а затем в библиотеках */
                          } 
/*------------------------------------ Поиск в библиотечных разделах */

   if(call_addr==NULL &&                                            /* Если процедура НЕ найдена      */
      mLibDirs !=NULL   ) {                                         /*  и заданы библиотечные разделы */

     for(ctlg=0 ; mLibDirs[ctlg]!=NULL &&                           /* Ищем процедуру в каждом библиотечном разделе          */
		  call_addr     ==NULL   ; ctlg++) {                /* сначала в виде отдельного файла, а затем в библиотеках */
			   call_addr=iCall_file(name, mLibDirs[ctlg]) ;
      if(call_addr==NULL)  call_addr=iCall_lib (name, mLibDirs[ctlg]) ;
                                                   }
                          }
/*-------------------------------- Регистрация процедуры в кэш-листе */

    if(call_addr!=NULL) {                                           /* Если процедура найдена ... */

      if(nCall_list_cnt>=_CALL_MAX) {  mError_code=_DCLE_MEMORY ;   /* Если переполнение списка процедур... */
                                         return(NULL) ;           }

#define  NEW_CALL   nCall_list[nCall_list_cnt]

	NEW_CALL               = *call_addr ;                       /* Заносим общие данные адесации */

	 strcpy(NEW_CALL.decl.name,     NEW_CALL.name) ;            /* Формируем описание процедуры */
	        NEW_CALL.decl.func_flag=   1 ;
	        NEW_CALL.decl.prototype= NULL ;
	        NEW_CALL.decl.work_nmb = DCL_WORK_NUM(nCall_list_cnt, _DCL_MAIN_P) ;
	        NEW_CALL.decl.addr     =_DCL_LOAD ;

#undef   NEW_CALL
                               nCall_list_cnt++;
			}
/*-------------------------------------------------------------------*/

   if(call_addr==NULL)  return(NULL) ;
   else                 return(&nCall_list[nCall_list_cnt-1].decl) ;
}


/*********************************************************************/
/*                                                                   */
/*                       Поиск DCL-файла                             */

  Dcl_call *Lang_DCL::iCall_file(char *name, char *folder)

{
 static Dcl_call  call_addr ;            /* Описание адресации */
	    char  path[FILENAME_MAX] ;   /* Имя DCL-файла */

/*------------------------------------------------------ Поиск файла */

          sprintf(path, "%s//%s.dcl", folder, name) ;

	if(access(path, 0x04))  return(NULL) ;                      /* Если файл недоступен */

/*-------------------------------------------- Регистрация процедуры */

     strncpy(call_addr.name, name, sizeof(call_addr.name)-1) ;      /* Имя процедуры */
     strncpy(call_addr.path, path, sizeof(call_addr.path)-1) ;      /* Путь файла */
	     call_addr.file_addr= 0L ;                              /* Стартовый адрес */

/*-------------------------------------------------------------------*/

  return(&call_addr) ;
}


/*********************************************************************/
/*                                                                   */
/*                  Поиск процедуры в DCL-библиотеке                 */

  Dcl_call *Lang_DCL::iCall_lib(char *name, char *folder)

{
  return(NULL) ;
}


/*********************************************************************/
/*                                                                   */
/*         Составление списка локальных и виртуальных процедур       */
/*           и текстовых блоков                                      */

   void  Lang_DCL::iCall_proc_list(Dcl_call *main)

{
    char  work[_BUFF_SIZE] ;  /* Рабочий буфер */
     int  work_cnt ;          /* Счетчик текущего буфера ввода */
     int  proc_cnt ;          /* Счетчик процедур */
     int  virtual_flag ;
     int  tblock_flag ;       /* Флаг прохождения по текстовому блоку */
     int  module_idx ;
    char *tmp ;
     int  i ;
     int  j ;

/*------------------------------------------------- Входной контроль */

    if(DCL_WORK2P(main->decl.work_nmb)!=_DCL_MAIN_P)  return ;      /* Если это не основная процедура модуля... */

    if(main->proc_list!=NULL)  return ;                             /* Если список процедур уже построен... */

          module_idx=DCL_WORK2M(main->decl.work_nmb) ;

/*-------------------------------- Построение списка входов процедур */

                 mByte_cnt =0 ;                                     /* Иниц. счетчика байтов */
                mError_code=0 ;

            main->proc_cnt= 0 ;
                  proc_cnt =0 ;

                tblock_flag=0 ;

  do {                                                              /* LOOP - Сканирование по строкам */
/*- - - - - - - - - - - - - - - - - - -  Считывание очередной строки */
	 work_cnt=iSysin(work, mByte_cnt, _BUFF_SIZE) ;             /* Считываем очередную строку */
      if(work_cnt==0)  break ;                                      /* Если все прошли... */

	 tmp=(char *)memchr(work, '\n', work_cnt) ;                 /* Ищем конец строки <ВК> */

      if(tmp==NULL)                                                 /* Если конец строки не найден... */
	if(work_cnt==_BUFF_SIZE) tmp=work+_BUFF_SIZE-1 ;            /*  Считаем концом строки последний введенный символ */
	else                     tmp=work+work_cnt ;                /*     или границу буфера                            */

		    *tmp=0 ;                                        /* Устан.терминатор строки */
	      memset(tmp, 0, _BUFF_SIZE+work-tmp-2) ;               /* Заполняем хвост буфера нулями */

	      mByte_cnt+=tmp-work+1 ;                               /* Устан.счетчик байт на след. строку */

	       tmp=strchr(work, '\r') ;                             /* Подавляем возможную    */
	    if(tmp!=NULL)  *tmp='\0' ;                              /*   комбинацию <RET><NL> */
/*- - - - - - - - - - - - - - - - - - - Регистрация текстового блока */
#define  TEXT  nT_blocks[nT_blocks_cnt]

   if(tblock_flag) {

     if(memcmp(work, "text_end", strlen("text_end"))==0) {          /* Если метка конца текстового блока... */

               TEXT.data=(char *)realloc(TEXT.data, strlen(TEXT.data)+8) ;

                                nT_blocks_cnt++ ;
                                   tblock_flag=0 ;
                                       continue ;
                                                         }
     else                                                {          /* Если содержимое текстового блока... */

              if(strlen(TEXT.data)+strlen(work)+2>=_TBLOCK_SIZE) {
                                          mError_code=_DCLE_TBLOCK_SIZE ;
                                                 break ;
                                                                 }

                              strcat(work, "\r\n") ;
                              strcat(TEXT.data, work) ;
                                      continue ;
                                                         }
                   }
   else            {

     if(memcmp(work, "text_start", strlen("text_start"))==0) {

            tmp=strstr(work, "text_start")+strlen("text_start") ;   /* Проходим на имя текстового блока */
         while(*tmp==' ')  tmp++ ;

                nT_blocks=(Dcl_tblock *)                            /* Переразмещаем список текстовых блоков */ 
                            realloc(nT_blocks, 
                                    (nT_blocks_cnt+1)*sizeof(*nT_blocks)) ;

                 strncpy(TEXT.name, tmp, sizeof(TEXT.name)-1) ;     /* Регистрируем вход */
                         TEXT.data=(char *)calloc(1, _TBLOCK_SIZE) ;

                                          tblock_flag=1 ;
                                              continue ;
                                                             }
                   }

#undef   TEXT
/*- - - - - - - - - - - - - - - - - - -  Ужатие пробелов и табуляций */
     for(i=0, j=0 ; work[i] ; i++, j++) 
       if(work[i]== ' ' ||                                          /* При обнаружении пробела       */
	  work[i]=='\t'   )      j-- ;                              /*  или табуляции - поджимаем их */
       else                 work[j]=work[i] ;

                            work[j]= 0 ;                            /* Терминируем строку */
/*- - - - - - - - - - - - - - - - - - - - - -  Обработка комментария */
	 if(work[0]=='/' &&
	    work[1]=='/'   )  continue ;
/*- - - - - - - - - - - - - - - - - - -  Регистрация входа процедуры */
     if(memcmp(work,        "procedure", 
              strlen(       "procedure"))==0 ||
        memcmp(work, "virtualprocedure", 
              strlen("virtualprocedure"))==0   ) {

          virtual_flag=(memcmp(work, "virtual", 
                              strlen("virtual"))==0)?1:0 ;

            tmp=strstr(work, "procedure")+strlen("procedure") ;     /* Проходим на имя процедуры */

        if(stricmp(tmp, "main"     )==0 ||
           stricmp(tmp,  main->name)==0   )  continue ;             /* Вход "основной" процедуры не регистрируем */

           main->proc_list=(Dcl_call *)                             /* Переразмещаем список процедур */ 
                            realloc(main->proc_list, 
                                     (proc_cnt+1)*sizeof(main->proc_list[0])) ;

#define  CALL  main->proc_list[proc_cnt]
#define  DECL  main->proc_list[proc_cnt].decl

          strncpy(CALL.name, tmp, sizeof(CALL.name)-1) ;            /* Регистрируем вход */
                  CALL.path     =main->path ;
                  CALL.mem_flag =main->mem_flag ;
                  CALL.file_addr=mByte_cnt ;
                  CALL.mem_addr =main->mem_addr ;
                  CALL.v_flag   = virtual_flag ;

           strcpy(DECL.name,     CALL.name) ;                       /* Формируем описание процедуры */
                  DECL.func_flag=   1 ;
 	          DECL.prototype= NULL ;
                  DECL.work_nmb = DCL_WORK_NUM(module_idx, proc_cnt) ;
                  DECL.addr     =_DCL_LOAD ;

#undef   CALL
#undef   DECL

                                 proc_cnt++ ;                       /* Модиф.счетчик процедур */
                  main->proc_cnt=proc_cnt ;
                                                 }
/*- - - - - - - - - - - - - - - - - - - - - - - - -  Цикл по строкам */
     } while(1) ;                                                   /* ENDLOOP - Сканирование по строкам */

/*------------------------------------------------ Анализ результата */

       if(tblock_flag) {                                            /* Если остался незакрытый текстовой блок */ 
                           mError_code=_DCLE_TBLOCK_CLOSE ;
                                 return ;
                       }

      if(main->proc_list==NULL)                                     /* Если модуль не содержит локальных и виртуальных */
                 main->proc_list=(Dcl_call *)0x0001 ;               /*  подпрограмм - ставим заглушку для исключения   */
                                                                    /*  повторного построения списка подпрограмм       */
/*-------------------------------------------------------------------*/

  return ;
}


/*********************************************************************/
/*                                                                   */
/*                    Исполнение DCL-процедуры                       */

   Dcl_decl *Lang_DCL::iCall_exec(int decl_num)

{
   Dcl_call *entry ;
        int  m_idx ;
        int  p_idx ;
        int  par_base ;    /* Базовый номер параметров */
        int  pars_cnt ;    /* Счетчик параметров */
        int  i ;

//#define  CALL   nCall_list[decl_num]

/*---------------------------------- Определение ссылки на процедуру */

            m_idx=DCL_WORK2M(decl_num) ;
            p_idx=DCL_WORK2P(decl_num) ;

     if(p_idx==_DCL_MAIN_P)  entry=&nCall_list[m_idx] ;
     else                    entry=&nCall_list[m_idx].proc_list[p_idx] ;

/*---------------------------------------- Модификация стека вызовов */

     if(nCall_stack_ptr>=_STACK_MAX) {  mError_code=_DCLE_STACK_OVR ;
                                           return(NULL) ;             }

                   nCall_stack_ptr++ ;
       nCall_stack[nCall_stack_ptr]=entry ;

/*-------------------------------------- Создание транзитной таблицы */

  if(nPars_cnt) {                                                   /* Если имеются параметры */
        nTransit=(Dcl_decl *)calloc(nPars_cnt, sizeof(Dcl_decl)) ;  /* Выделяем память под транзитную таблицу */
     if(nTransit==NULL)  return(NULL) ;

	    par_base=nPars_cnt_all-nPars_cnt ;                      /* Oпр.базовый номер параметров */

	for(i=0 ; i<nPars_cnt ; i++)                                /* Формируем транзитную таблицу параметров */
	     nTransit[i]=*nPars_table[par_base+i] ;
		}
/*------------------------------------------------- Собственно вызов */

		    pars_cnt=nPars_cnt ;                            /* Фикс.число перед.параметров */

	   Stack(_DCL_PUSH) ;                                       /* Сохр.текущее состояние процесса  */
                                                                    /*   и реиниц. нужные управл.данные */
   if(mError_code) return(NULL) ;                                   /* Если были ошибки... */ 

	     nTransit_cnt=pars_cnt ;                                /* Уст.число передаваемых параметров */

               nSeg_start=entry->file_addr ;
                   nLocal++ ;

   if(entry->mem_flag)                                              /* Выполняем DCL-процедуру */
           Interpritator(_DCL_MEMORY, entry->mem_addr, nVars) ;
   else    Interpritator(_DCL_FILE,   entry->path,     nVars) ;

                   Stack(_DCL_POP) ;                                /* Восст.исходное состояние процесса */

/*------------------------------------------------ Завершение вызова */

    if(nTransit!=NULL)  free(nTransit) ;                            /* Освоб.память транзитной таблицы */

/*-------------------------------------------- Возврат стека вызовов */

                   nCall_stack_ptr-- ;
                            nLocal-- ;

/*-------------------------------------------------------------------*/

 return(&nReturn) ;
}


/*********************************************************************/
/*                                                                   */
/*       Исполнение процедур финального освобождения ресурсов        */

   void  Lang_DCL::iCall_final(void)

{
   Dcl_decl *vars ;       /* Текущий массив описаний переменных и процедур */
        int  i ;

   void (*final_proc)(void) ;

/*---------------------------- Выполнение $Final$-процедур библиотек */

   for(i=0 ;  ; i++) {                                              /* LOOP - Перебираем библиотеки */

                        if(nVars[i]==NULL) break ;

     for(vars=nVars[i] ; vars->name[0]!=0 ; vars++) {               /* В библиотеке ищем и исплняем процедуру  */ 
                                                                    /*      с именем $Final$                   */
          if(      ! vars->func_flag       )  continue ;
          if(stricmp(vars->name, "$Final$"))  continue ;

              final_proc=(void (*)(void))vars->addr ;
              final_proc() ;
                                                    }


                    }                                              /* END LOOP - Перебираем библиотеки */
/*-------------------------------------------------------------------*/

   return ;
}

