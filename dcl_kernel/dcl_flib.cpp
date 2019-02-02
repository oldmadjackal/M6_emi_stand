
/*********************************************************************/
/*                                                                   */
/*                   DATA CONVERTION LANGUAGE                        */
/*                                                                   */
/*                 БИБЛИОТЕКА РАБОТЫ С ФАЙЛАМИ                       */
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
#include <malloc.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#ifdef UNIX
#include  <unistd.h>
#define   O_BINARY    0
#else
#include  <io.h>
#include  <direct.h>
#endif


#define DCL_INSIDE

#include "dcl.h"


#pragma warning(disable : 4244)
#pragma warning(disable : 4267)
#pragma warning(disable : 4996)

/*---------------------------------------------------- Внешние связи */

          extern double  dcl_errno ;       /* Системный указатель ошибок -> DCL_SLIB.CPP */

/*------------------------------------------------ "Внешние" объекты */

    Dcl_decl *Dcl_f_with      (Lang_DCL *, Dcl_decl **, int) ;    /* Смена активного рабочего файла */
    Dcl_decl *Dcl_f_open      (Lang_DCL *, Dcl_decl **, int) ;    /* Открытие рабочего файла */
    Dcl_decl *Dcl_f_close     (Lang_DCL *, Dcl_decl **, int) ;    /* Закрытие рабочего файла */
    Dcl_decl *Dcl_f_seek      (Lang_DCL *, Dcl_decl **, int) ;    /* Позиционирование рабочего файла */
    Dcl_decl *Dcl_f_tell      (Lang_DCL *, Dcl_decl **, int) ;    /* Запрос позиции в рабочем файле */
    Dcl_decl *Dcl_f_read      (Lang_DCL *, Dcl_decl **, int) ;    /* Считывание рабочего файла */
    Dcl_decl *Dcl_f_readl     (Lang_DCL *, Dcl_decl **, int) ;    /* Считывание строки рабочего файла */
    Dcl_decl *Dcl_f_readonce  (Lang_DCL *, Dcl_decl **, int) ;    /* Считывание файла в один прием */
    Dcl_decl *Dcl_f_write     (Lang_DCL *, Dcl_decl **, int) ;    /* Запись рабочего файла */
    Dcl_decl *Dcl_f_writeonce (Lang_DCL *, Dcl_decl **, int) ;    /* Запись файла в один прием */
    Dcl_decl *Dcl_f_unlink    (Lang_DCL *, Dcl_decl **, int) ;    /* Удаление файла */
    Dcl_decl *Dcl_f_exists    (Lang_DCL *, Dcl_decl **, int) ;    /* Проверка существования файла или раздела */
    Dcl_decl *Dcl_f_cp_convert(Lang_DCL *, Dcl_decl **, int) ;    /* Преобразование кодовой страницы текстового файла */
    Dcl_decl *Dcl_f_check     (Lang_DCL *, Dcl_decl **, int) ;    /* Проверка наличия ключевого фрагмента в файле */

/*------------------------------------------------ Эмуляция констант */

	 static double      o_creat =O_CREAT ;   /* Эмуляция управляющих кодов */
	 static double      o_excl  =O_EXCL ;
	 static double      o_trunc =O_TRUNC ;
	 static double      o_append=O_APPEND ;
	 static double      o_rdwr  =O_RDWR ;
	 static double      seek_set=SEEK_SET ;
	 static double      seek_cur=SEEK_CUR ;
	 static double      seek_end=SEEK_END ;

	 static double      eacces=EACCES ;      /* Эмуляция кодов ошибок */
	 static double      eexist=EEXIST ;
	 static double      emfile=EMFILE ;
	 static double      enoent=ENOENT ;
	 static double      enomem=ENOMEM ;
	 static double      einval=EINVAL ;
	 static double      enospc=ENOSPC ;
	 static double      ebadf =EBADF ;
	 static double      eof_=EOF ;

/*--------------------------------------------------- Таблица ссылок */

     Dcl_decl  dcl_file_lib[]={

               {0, 0, 0, 0, "$PassiveData$", NULL, "flib", 0, 0},

	       {_DGT_VAL, 0, 0, 0, "O_CREAT",  &o_creat,  NULL, 1, 1},
	       {_DGT_VAL, 0, 0, 0, "O_EXCL",   &o_excl,   NULL, 1, 1},
	       {_DGT_VAL, 0, 0, 0, "O_TRUNC",  &o_trunc,  NULL, 1, 1},
	       {_DGT_VAL, 0, 0, 0, "O_APPEND", &o_append, NULL, 1, 1},
	       {_DGT_VAL, 0, 0, 0, "O_RDWR",   &o_rdwr,   NULL, 1, 1},
	       {_DGT_VAL, 0, 0, 0, "SEEK_SET", &seek_set, NULL, 1, 1},
	       {_DGT_VAL, 0, 0, 0, "SEEK_CUR", &seek_cur, NULL, 1, 1},
	       {_DGT_VAL, 0, 0, 0, "SEEK_END", &seek_end, NULL, 1, 1},

	       {_DGT_VAL, 0, 0, 0, "EACCES", &eacces, NULL, 1, 1},
	       {_DGT_VAL, 0, 0, 0, "EEXIST", &eexist, NULL, 1, 1},
	       {_DGT_VAL, 0, 0, 0, "EMFILE", &emfile, NULL, 1, 1},
	       {_DGT_VAL, 0, 0, 0, "ENOENT", &enoent, NULL, 1, 1},
	       {_DGT_VAL, 0, 0, 0, "ENOMEM", &enomem, NULL, 1, 1},
	       {_DGT_VAL, 0, 0, 0, "EINVAL", &einval, NULL, 1, 1},
	       {_DGT_VAL, 0, 0, 0, "ENOSPC", &enospc, NULL, 1, 1},
	       {_DGT_VAL, 0, 0, 0, "EBADF",  &ebadf,  NULL, 1, 1},
	       {_DGT_VAL, 0, 0, 0, "EOF",    &eof_,   NULL, 1, 1},

	       {_DGT_VAL, _DCL_CALL, 0, 0, "f_with",       (void *)Dcl_f_with,       "v",    0, 0},
	       {_DGT_VAL, _DCL_CALL, 0, 0, "f_open",       (void *)Dcl_f_open,       "sv",   0, 0},
	       {_DGT_VAL, _DCL_CALL, 0, 0, "f_close",      (void *)Dcl_f_close,      "",     0, 0},
	       {_DGT_VAL, _DCL_CALL, 0, 0, "f_seek",       (void *)Dcl_f_seek,       "vv",   0, 0},
	       {_DGT_VAL, _DCL_CALL, 0, 0, "f_tell",       (void *)Dcl_f_tell,       "",     0, 0},
	       {_DGT_VAL, _DCL_CALL, 0, 0, "f_read",       (void *)Dcl_f_read,       "av",   0, 0},
	       {_DGT_VAL, _DCL_CALL, 0, 0, "f_readl",      (void *)Dcl_f_readl,      "as",   0, 0},
	       {_DGT_VAL, _DCL_CALL, 0, 0, "f_readonce",   (void *)Dcl_f_readonce,   "sa",   0, 0},
	       {_DGT_VAL, _DCL_CALL, 0, 0, "f_write",      (void *)Dcl_f_write,      "a",    0, 0},
	       {_DGT_VAL, _DCL_CALL, 0, 0, "f_writeonce",  (void *)Dcl_f_writeonce,  "sa",   0, 0},
	       {_DGT_VAL, _DCL_CALL, 0, 0, "f_unlink",     (void *)Dcl_f_unlink,     "s",    0, 0},
	       {_DGT_VAL, _DCL_CALL, 0, 0, "f_exists",     (void *)Dcl_f_exists,     "s",    0, 0},
	       {_DGT_VAL, _DCL_CALL, 0, 0, "f_cp_convert", (void *)Dcl_f_cp_convert, "ssss", 0, 0},
	       {_DGT_VAL, _DCL_CALL, 0, 0, "f_check",      (void *)Dcl_f_check,      "sas",  0, 0},

	       {0, 0, 0, 0, "", NULL, NULL, 0, 0}
                               } ;

/*----------------------------------------------- Рабочие переменные */

#undef   _BUFF_SIZE
#define  _BUFF_SIZE   8192

        static      int  dcl_rfile ;           /* Рабочий файл */
        static      int  dcl_rfile_written ;   /* Метка наличия записи в рабочий файл */


/*****************************************************************/
/*                                                               */
/*          Смена активного рабочего файла                       */
/*                                                               */
/*        int  f_with(file) ;                                    */
/*                                                               */
/*            int  file - дескриптор файла                       */
/*                                                               */
/*  Возврашает десцриптор предыдущего активного рабочего файла   */

  Dcl_decl *Dcl_f_with(Lang_DCL *Kernel, Dcl_decl **pars, int  pars_cnt)

{
   static   double  ret_value ;          /* Буфер числового значения */
   static Dcl_decl  dgt_return={_DGT_VAL, 0, 0, 0, "", &ret_value, NULL, 1, 1} ;


               ret_value=dcl_rfile ;

   if(pars_cnt     >=  1  &&
      pars[0]->addr!=NULL   ) {
               dcl_rfile=(int)Kernel->iDgt_get(pars[0]->addr, pars[0]->type) ;
                              }

   return(&dgt_return) ;
}


/*****************************************************************/
/*                                                               */
/*                 Открытие рабочего файла                       */
/*                                                               */
/*   double  f_open(path [, attr]) ;                             */
/*                                                               */
/*           char *path - имя файла                              */
/*         double  attr - атрибуты : O_CREAT, O_EXCL, O_TRUNC,   */
/*                                   O_RDWR, O_APPEND            */
/*                                                               */
/*     Если attr опущен полагается attr=0.                       */
/*                                                               */
/*     Вызов п/п адекватен вызову:                               */
/*        open(name, attr|O_BINARY, S_IREAD|S_IWRITE)            */
/*                                                               */
/*  Возврашает дескриптор файла, если файл открыт нормально      */
/*   или -1 - в противном случае.                                */
/*  Код ошибки errno :                                           */
/*       EACCES, EEXIST, EMFILE, ENOENT - согласно C-стандарту   */

  Dcl_decl *Dcl_f_open(Lang_DCL *Kernel, Dcl_decl **pars, int  pars_cnt)

{
              char  path[_BUFF_SIZE] ;   /* Буфер символьных строк */
               int  attr ;

   static   double  ret_value ;          /* Буфер числового значения */
   static Dcl_decl  dgt_return={_DGT_VAL, 0, 0, 0, "", &ret_value, NULL, 1, 1} ;

/*---------------------------------------- Подготовка открытие файла */

//     if(dcl_rfile>0)  Dcl_f_close(Kernel, NULL, 0) ;              /* Если рабочий файл уже открыт - */
//              						    /*   закрываем его                */
/*--------------------------------------------------- Открытие файла */

            dcl_errno=0. ;

    if(pars[0]->size>=sizeof(path))  pars[0]->size=sizeof(path)-1 ; /* Контроль размеров имени файла */

	  memset(path, 0, sizeof(path)) ;
	  memcpy(path, pars[0]->addr, pars[0]->size) ;              /* Занесение имени в рабочий буфер */

       if(pars_cnt     < 2  ||                                      /* 'Извлекаем' аттрибут файла */
	  pars[1]->addr==NULL )  attr= 0 ;
       else                      attr=(int)Kernel->iDgt_get(pars[1]->addr, pars[1]->type) ;

       if((attr & O_CREAT )==O_CREAT  ||                            /* Корректируем аттрибут доступа */
          (attr & O_APPEND)==O_APPEND   )  attr|=O_RDWR ;           /*     для операций записи       */

	dcl_rfile=open(path, attr | O_BINARY,                       /* Открываем файл */
					S_IREAD | S_IWRITE) ;
     if(dcl_rfile==-1)  dcl_errno=errno ;                           /* При неудаче - считываем системную ошибку */

/*--------------------------------------------------------- Концовка */

          ret_value=dcl_rfile ;
   return(&dgt_return) ;
}


/*****************************************************************/
/*                                                               */
/*                 Закрытие рабочего файла                       */
/*                                                               */
/*        void  f_close([file]) ;                                */
/*                                                               */
/*            int  file - дескриптор файла                       */
/*                                                               */
/*  Возврашает 0 при успещном закрытии или -1 - при неудаче      */
/*  Код ошибки errno :  EBADF - согласно C-стандарту             */

  Dcl_decl *Dcl_f_close(Lang_DCL *Kernel, Dcl_decl **pars, int  pars_cnt)

{
               int  file ;
 
   static   double  ret_value ;          /* Буфер числового значения */
   static Dcl_decl  dgt_return={_DGT_VAL, 0, 0, 0, "", &ret_value, NULL, 1, 1} ;

/*------------------------------------------------- Входной контроль */

                        dcl_errno=0 ;

   if(pars_cnt     >=  1  &&
      pars[0]->addr!=NULL   ) {
                                 file=(int)Kernel->iDgt_get(pars[0]->addr, pars[0]->type) ;
                              }
   else                       {
                                      file=dcl_rfile ;
                                 dcl_rfile= 0 ; 
                              } 

   if(file<=0) {
                          ret_value=0 ;
                  return(&dgt_return) ;
               }
/*---------------------------------------- Собственно закрытие файла */

        ret_value=close(file) ;                                     /* Закрываем файл */
     if(ret_value==-1)  dcl_errno=errno ;                           /* При неудаче - считываем системную ошибку */

/*-------------------------------------------------------------------*/

      return(&dgt_return) ;
}


/*****************************************************************/
/*                                                               */
/*            Позиционирование рабочего файла                    */
/*                                                               */
/*    double  f_seek(offset, base) ;                             */
/*                                                               */
/*      double  offset - смещение позиции                        */
/*      double  base   - код базы: SEEK_SET, SEEK_CUR, SEEK_END  */
/*                                                               */
/*     Вызов п/п адекватен вызову:                               */
/*       lseek(..., offset, base)                                */
/*                                                               */
/*  Возврашает текущую позицию или -1 - при неудаче              */
/*  Код ошибки errno :  EBADF, EINVAL - согласно C-стандарту     */

  Dcl_decl *Dcl_f_seek(Lang_DCL *Kernel, Dcl_decl **pars, int  pars_cnt)

{
             int  base ;
            long  offset ;
 static   double  ret_value ;          /* Буфер числового значения */
 static Dcl_decl  dgt_return={_DGT_VAL, 0, 0, 0, "", &ret_value, NULL, 1, 1} ;


                dcl_errno=0. ;

    if(dcl_rfile<=0) {         dcl_errno=EBADF ;    
                               ret_value= -1 ;
                       return(&dgt_return) ;      }

      offset=(long)Kernel->iDgt_get(pars[0]->addr, pars[0]->type) ; /* 'Вытаскиваем' код базы и смещение */
        base=( int)Kernel->iDgt_get(pars[1]->addr, pars[1]->type) ;

      ret_value=lseek(dcl_rfile, offset, base) ;                    /* Позиционируем рабочий файл */
   if(ret_value<0)  dcl_errno=errno ;	                            /* Если неудачно - считываем системную ошибку */

  return(&dgt_return) ;
}


/*****************************************************************/
/*                                                               */
/*         Запрос текущей позиции в рабочем файле                */
/*                                                               */
/*    double  f_tell(void) ;                                     */
/*                                                               */
/*     Вызов п/п адекватен вызову:                               */
/*        tell(...)                                              */
/*                                                               */
/*  Возврашает текущую позицию или -1 - при неудаче              */
/*  Код ошибки errno :  EBADF - согласно C-стандарту             */

  Dcl_decl *Dcl_f_tell(Lang_DCL *Kernel, Dcl_decl **pars, int  pars_cnt)

{
 static   double  ret_value ;          /* Буфер числового значения */
 static Dcl_decl  dgt_return={_DGT_VAL, 0, 0, 0, "", &ret_value, NULL, 1, 1} ;


                dcl_errno=0. ;

    if(dcl_rfile<=0) {         dcl_errno=EBADF ;    
                               ret_value= -1 ;
                       return(&dgt_return) ;      }

       ret_value=lseek(dcl_rfile, 0L, SEEK_CUR) ;                   /* Запрос текущей позиции в файла */
    if(ret_value<0)  dcl_errno=errno ;	                            /* Если неудачно - считываем системную ошибку */

  return(&dgt_return) ;
}


/*****************************************************************/
/*                                                               */
/*                Считывание рабочего файла                      */
/*                                                               */
/*    double  f_read(buff [, cnt]) ;                             */
/*                                                               */
/*        void *buff - буфер считывания                          */
/*      double  cnt  - счетчик считывания                        */
/*                                                               */
/*    Если cnt опущен, то полачается cnt=buffof(buff)            */
/*                                                               */
/*  Возврашает число считанных байт, -1 - при ошибке             */
/*  Код ошибки errno :  EBADF - согласно C-стандарту             */

  Dcl_decl *Dcl_f_read(Lang_DCL *Kernel, Dcl_decl **pars, int  pars_cnt)

{
             int  type ;   /* Базовый тип буфера */
             int  size ;   /* Размер считывания */

 static   double  ret_value ;          /* Буфер числового значения */
 static Dcl_decl  dgt_return={_DGT_VAL, 0, 0, 0, "", &ret_value, NULL, 1, 1} ;


                                  dcl_errno=0 ;

       if(dcl_rfile<=0) {         dcl_errno=EBADF ;    
                                  ret_value= -1 ;
                          return(&dgt_return) ;      }

	  type=t_base(pars[0]->type) ;

       if(pars[0]      ==NULL ||                                    /* Уст. счетчик считывания */
	  pars[0]->addr==NULL   )  size= 0 ;                        /*  в зависимости от числа */
  else if(pars_cnt      <  2  ||                                    /*   параметров            */
	  pars[1]->addr==NULL   )  size=pars[0]->buff ;
  else                             size=(int)Kernel->iDgt_get(pars[1]->addr, pars[1]->type) ;

   if(size>pars[0]->buff)  size=pars[0]->buff ;                     /* Контроль переполнения буфера данныех */

	if(type==_DGT_VAL  ||                                       /* Приведение размеров    */
	   type==_DGT_AREA ||                                       /*    числовых переменных */
	   type==_DGT_PTR    )  size*=Kernel->iDgt_size(pars[0]->type) ;

           ret_value=read(dcl_rfile, pars[0]->addr, size) ;         /* Считываем из рабочего файла */
        if(ret_value<0) {                                           /* Если ошибка... */
                            dcl_errno=errno ;
			    ret_value= -1 ;
			     return(&dgt_return) ;
                        }

                                 pars[0]->size=(int)ret_value ;     /* Уст.размер считанного обьекта */             
           Kernel->iSize_correct(pars[0]) ;                         /* Проводим следящую коррекцию     */
								    /*   размеров указателей на обьект */
  return(&dgt_return) ;
}


/*****************************************************************/
/*                                                               */
/*            Считывание строки из рабочего файла                */
/*                                                               */
/*    double  f_readl(buff [, end]) ;                            */
/*                                                               */
/*        void *buff - буфер считывания                          */
/*        char *end  - метка конца строки                        */
/*                                                               */
/*     Считывание ведется до обнаружения метки конца строки,     */
/*  или до конца файла, или до конца буфера. Если end опущен, то */
/*  п/п работает аналогични f_read(buff).                        */
/*                                                               */
/*  Возврашает: число считанных байт, -1 - ошибке                */
/*  Код ошибки errno :  EBADF - согласно C-стандарту             */
/*  При завершении файла возвращает 0 и код ошибки EOF           */

  Dcl_decl *Dcl_f_readl(Lang_DCL *Kernel, Dcl_decl **pars, int  pars_cnt)

{
            char *addr ;
            char *end ;
             int  end_size ;
             int  size ;
             int  cnt ;
             int  len ;
            long  pos ;
            char  buff[_BUFF_SIZE] ;   /* Буфер символьных строк */
             int  i ;

 static   double  ret_value ;          /* Буфер числового значения */
 static Dcl_decl  dgt_return={_DGT_VAL, 0, 0, 0, "", &ret_value, NULL, 1, 1} ;

/*---------------------------------------------------- Инициализация */

                            dcl_errno=0 ;

       if(dcl_rfile<=0) {         dcl_errno=EBADF ;                 /* Проверка наличия рабочего файла */
                                  ret_value= -1 ;
                          return(&dgt_return) ;      }

       if(pars[0]      ==NULL ||                                    /* Обработка отсутствия параметров */
	  pars[0]->addr==NULL   ) {  ret_value= 0 ;
				    return(&dgt_return) ;  }

       if(pars_cnt     >=  2  &&                                    /* Опр. конечную метку */
	  pars[1]->addr!=NULL   ) {  end     =(char *)pars[1]->addr ;
				     end_size=        pars[1]->size ;  }
       else                       {  end     =NULL ;
                                     end_size= 0 ;                     }

/*------------------------------------------- Позиционирование файла */

           pos=lseek(dcl_rfile, 0L, SEEK_CUR) ;                     /* Запрос текущей позиции */ 
           cnt= read(dcl_rfile, buff, _BUFF_SIZE) ;                 /* Считываем из рабочего файла */
        if(cnt< 0) {                                                /* Если ошибка... */
                      dcl_errno=errno ;
                      ret_value= -1 ;
		        return(&dgt_return) ;
                   }
        if(cnt==0) {                                                /* Если файл завершился... */
                      dcl_errno=EOF ;
                      ret_value= 0 ;
                        return(&dgt_return) ;
                   }
/*--------------------------------------------- Поиск концевой метки */

    if(end_size) {

	 for(addr=buff ; ; addr++) {
				    len=cnt-(addr-buff) ;
				   addr=(char *)memchr(addr, end[0], len) ;
				if(addr==NULL)  break ;

	     for(i=1 ; i<end_size && end[i]==addr[i] ; i++) ;

		    if(i==end_size)  break ;
				   }

		   if(addr!=NULL)  size=addr-buff ;
                   else            size=cnt ; 
		 }
    else                           size=cnt ;

	    if(size==_BUFF_SIZE)  {  dcl_errno=enomem ;
				     ret_value= 0 ;
				       return(&dgt_return) ;  }

/*--------------------------------------------- Копирование на выход */

	      if(size>pars[0]->buff)  size=pars[0]->buff ;

          lseek(dcl_rfile, pos+size+end_size, SEEK_SET) ;           /* Перепозиц.файл за считанный фрагмент */

		memcpy(pars[0]->addr, buff, size) ;

		      pars[0]->size=size ;

		    Kernel->iSize_correct(pars[0]) ;                /* Проводим следящую коррекцию     */
								    /*   размеров указателей на обьект */
/*-------------------------------------------------------------------*/

       ret_value=size ;

  return(&dgt_return) ;
}


/*****************************************************************/
/*                                                               */
/*            Считывание файла в один прием                      */
/*                                                               */
/*   double  f_readonce(path, buff [, cnt]) ;                    */
/*                                                               */
/*        char *path - имя файла                                 */
/*        void *buff - буфер считывания                          */
/*      double  cnt  - счетчик считывания                        */
/*                                                               */
/*  Возврашает число считаных байт и -1 - в случае ошибки        */
/*                                                               */
/*  Код ошибки errno :                                           */
/*       EACCES, EMFILE, ENOENT - согласно C-стандарту           */

  Dcl_decl *Dcl_f_readonce(Lang_DCL *Kernel, Dcl_decl **pars, int  pars_cnt)

{
         char  path[_BUFF_SIZE] ;   /* Буфер символьных строк */
 struct _stat  attr ;               /* Аттрибуты файла */ 
          int  file ;  
          int  type ;              /* Базовый тип буфера */
         long  size ;              /* Размер считывания */
          int  status ;  

   static   double  ret_value ;          /* Буфер числового значения */
   static Dcl_decl  dgt_return={_DGT_VAL, 0, 0, 0, "", &ret_value, NULL, 1, 1} ;

/*------------------------------------------------- Входной контроль */

                                            dcl_errno= 0 ; 

  if(pars_cnt      <  2  ||
     pars[0]->addr==NULL ||
     pars[1]->addr==NULL   ) {
                                            dcl_errno=EINVAL ;
                                            ret_value=-1. ;
                                    return(&dgt_return) ;
                             }
/*------------------------------------------- Извлекаем путь к файлу */

    if(pars[0]->size>=sizeof(path))  pars[0]->size=sizeof(path)-1 ; /* Контроль размеров имени файла */

	  memset(path, 0, sizeof(path)) ;
	  memcpy(path, pars[0]->addr, pars[0]->size) ;              /* Занесение имени в рабочий буфер */

       status=_stat(path, &attr) ;
    if(status==-1) {                                                /* Если ошибка... */
                              dcl_errno=errno ;
                              ret_value=-1. ;
                      return(&dgt_return) ;
                   }
/*------------------------------------- Если задан размер считывания */

   if(pars_cnt      <  3  ||
      pars[2]->addr==NULL   )  size=  0 ;
   else                        size=(int)Kernel->iDgt_get(pars[2]->addr, pars[2]->type) ;

/*------------------------------------ Если задан динамический буфер */

  if( pars[1]->prototype!=NULL &&
     *pars[1]->prototype=='D'    ) {

       if(size==0)  size=attr.st_size ;                             /* Если считываем весь файл... */
 
       if(size>pars[1]->buff) {                                     /* Если довыделение памяти... */
            Kernel->iBuff_realloc(pars[1], NULL, size) ;              
         if(Kernel->mError_code) { 
                                    dcl_errno=Kernel->mError_code ;
                                    ret_value=-1. ;
                            return(&dgt_return) ;
                                 }
                              }
                                   }
/*--------------------------------------------------- Открытие файла */

    if(size==0)  size=pars[1]->buff ;                               /* Если считываем по размеру буфера... */

       file=open(path, O_BINARY | O_RDONLY) ;                       /* Открываем файл */
    if(file==-1) {                                                  /* Если ошибка... */
                                  dcl_errno=errno ;
                                  ret_value=-1. ;
                          return(&dgt_return) ;
                 }
/*------------------------------------------------- Считывание файла */

   if(size==0)  size=pars[1]->buff ;

   if(size>pars[1]->buff)  size=pars[1]->buff ;                     /* Контроль переполнения буфера данныех */

      type=t_base(pars[1]->type) ;
   if(type==_DGT_VAL  ||                                            /* Приведение размеров    */
      type==_DGT_AREA ||                                            /*    числовых переменных */
      type==_DGT_PTR    )  size*=Kernel->iDgt_size(pars[1]->type) ;

           ret_value=read(file, pars[1]->addr, size) ;              /* Считываем данные из файла */
        if(ret_value<0)  dcl_errno=errno ;                          /* Если ошибка... */

/*--------------------------------------------------- Закрытие файла */

                close(file) ;

/*--------------------------------------------------------- Концовка */

    if(ret_value>=0) {                                              /* Если считаны даныые... */ 
                                 pars[1]->size=(int)ret_value ;     /*  Уст.размер считанного обьекта */             
           Kernel->iSize_correct(pars[1]) ;                         /*  Проводим следящую коррекцию     */
								    /*    размеров указателей на обьект */
                     }

   return(&dgt_return) ;
}


/*****************************************************************/
/*                                                               */
/*                 Запись рабочего файла                         */
/*                                                               */
/*    double  f_write(buff) ;                                    */
/*                                                               */
/*        void  buff - буфер записи                              */
/*                                                               */
/*  В рабочий файл записывается число байт соотв. sizeof(buff).  */
/*                                                               */
/*  Возврашает число записаных байт или -1 - при неудаче         */
/*  Код ошибки errno :  EBADF, ENOSPC - согласно C-стандарту     */

  Dcl_decl *Dcl_f_write(Lang_DCL *Kernel, Dcl_decl **pars, int  pars_cnt)

{
             int  type ;  /* Базовый тип буфера */
             int  size ;

 static   double  ret_value ;          /* Буфер числового значения */
 static Dcl_decl  dgt_return={_DGT_VAL, 0, 0, 0, "", &ret_value, NULL, 1, 1} ;


                                   dcl_errno= 0 ; 

        if(dcl_rfile<=0) {         dcl_errno=EBADF ;                /* Проверка наличия рабочего файла */
                                   ret_value= -1 ;
                           return(&dgt_return) ;      }
 
	  type=t_base(pars[0]->type) ;


	if(pars[0]      ==NULL ||                                   /* Обработка особой ситуации */
	   pars[0]->addr==NULL   )  size= 0 ;
	else                        size=pars[0]->size ;

	if(type==_DGT_VAL  ||                                       /* Приведение размеров    */
	   type==_DGT_AREA ||                                       /*    числовых переменных */
	   type==_DGT_PTR    )  size*=(int)Kernel->iDgt_size(pars[0]->type) ;

	   ret_value=write(dcl_rfile, pars[0]->addr, size) ;        /* Записываем в рабочий файл */
        if(ret_value<0)  dcl_errno=errno ;                          /* Если неудачно - считываем системную ошибку */

  return(&dgt_return) ;
}


/*****************************************************************/
/*                                                               */
/*            Запись файла в один прием                          */
/*                                                               */
/*   double  f_writeonce(path, buff [, cnt]) ;                   */
/*                                                               */
/*        char *path - имя файла                                 */
/*        void *buff - буфер записываемых данных                 */
/*      double  cnt  - счетчик записи                            */
/*                                                               */
/*  Возврашает число записанных байт и -1 - в случае ошибки      */
/*                                                               */
/*  Код ошибки errno :                                           */
/*       EACCES, EMFILE, ENOENT, ENOSPC - согласно C-стандарту   */

  Dcl_decl *Dcl_f_writeonce(Lang_DCL *Kernel, Dcl_decl **pars, int  pars_cnt)

{
         char  path[_BUFF_SIZE] ;   /* Путь к файлу */
          int  file ;  
          int  type ;               /* Базовый тип буфера */
          int  size ;               /* Размер считывания */

   static   double  ret_value ;          /* Буфер числового значения */
   static Dcl_decl  dgt_return={_DGT_VAL, 0, 0, 0, "", &ret_value, NULL, 1, 1} ;

/*------------------------------------------------- Входной контроль */

                                            dcl_errno= 0 ; 

  if(pars_cnt      <  2  ||
     pars[0]->addr==NULL ||
     pars[1]->addr==NULL   ) {
                                            dcl_errno=EINVAL ;
                                            ret_value=-1. ;
                                    return(&dgt_return) ;
                             }
/*--------------------------------------------------- Открытие файла */

    if(pars[0]->size>=sizeof(path))  pars[0]->size=sizeof(path)-1 ; /* Контроль размеров имени файла */

	  memset(path, 0, sizeof(path)) ;
	  memcpy(path, pars[0]->addr, pars[0]->size) ;              /* Занесение имени в рабочий буфер */

       file=open(path, O_CREAT | O_TRUNC | O_BINARY | O_RDWR,       /* Открываем файл */
                                            S_IREAD | S_IWRITE) ; 
    if(file==-1) {                                                  /* Если ошибка... */
                                  dcl_errno=errno ;
                                  ret_value=-1. ;
                          return(&dgt_return) ;
                 }  
/*----------------------------------------------------- Запись файла */

   if(pars_cnt      <  3  ||                                        /* Определяем размер считывания */
      pars[2]->addr==NULL   )  size=pars[1]->size ;
   else                        size=(int)Kernel->iDgt_get(pars[2]->addr, pars[2]->type) ;

      type=t_base(pars[1]->type) ;
   if(type==_DGT_VAL  ||                                            /* Приведение размеров    */
      type==_DGT_AREA ||                                            /*    числовых переменных */
      type==_DGT_PTR    )  size*=Kernel->iDgt_size(pars[1]->type) ;

   if(size>0) {
                   ret_value=write(file, pars[1]->addr, size) ;     /* Пишем данные в файл */
                if(ret_value==-1)  dcl_errno=errno ;
              }  
/*--------------------------------------------------- Закрытие файла */

                close(file) ;

/*--------------------------------------------------------- Концовка */

   return(&dgt_return) ;
}


/*****************************************************************/
/*                                                               */
/*                 Удаление файла                                */
/*                                                               */
/*   double  f_unlink(path) ;                                    */
/*                                                               */
/*           char *path - путь файла                             */
/*                                                               */
/*     Вызов п/п адекватен вызову:  unlink(path)                 */
/*                                                               */
/*  Возврашает 0, если файл удален и -1 - в противном случае.    */
/*  Код ошибки errno :                                           */
/*       EACCES, ENOENT - согласно C-стандарту                   */

  Dcl_decl *Dcl_f_unlink(Lang_DCL *Kernel, Dcl_decl **pars, int  pars_cnt)

{
              char *path ;
	       int  size ;

   static   double  ret_value ;          /* Буфер числового значения */
   static Dcl_decl  dgt_return={_DGT_VAL, 0, 0, 0, "", &ret_value, NULL, 1, 1} ;

/*------------------------------------------------- Входной контроль */

                                        dcl_errno=  0. ;

  if(pars_cnt      <  1  ||
     pars[0]->addr==NULL   ) {
                                        dcl_errno=ENOENT ;
                                        ret_value=-1. ;
                                return(&dgt_return) ;
                             }
/*------------------------------------------------------- Исполнение */

                               size=pars[0]->size ;
      if(size>=pars[0]->buff)  size=pars[0]->buff-1 ;

                            path=(char *)pars[0]->addr ;
                            path[size]=0 ;

         ret_value=unlink(path) ;
      if(ret_value<0)  dcl_errno=errno ;

/*-------------------------------------------------------------------*/

  return(&dgt_return) ;
}


/*****************************************************************/
/*                                                               */
/*         Проверка существования файла или раздела              */
/*                                                               */
/*   double  f_exists(path) ;                                    */
/*                                                               */
/*           char *path - путь файла                             */
/*                                                               */
/*     Вызов п/п адекватен вызову:  access(path, 0x00)           */
/*                                                               */
/*  Возврашает 0, если файл существует или                       */
/*             EACCES, ENOENT - согласно C-стандарту             */

  Dcl_decl *Dcl_f_exists(Lang_DCL *Kernel, Dcl_decl **pars, int  pars_cnt)

{
              char *path ;
	       int  size ;

   static   double  ret_value ;          /* Буфер числового значения */
   static Dcl_decl  dgt_return={_DGT_VAL, 0, 0, 0, "", &ret_value, NULL, 1, 1} ;

/*------------------------------------------------- Входной контроль */

                                        dcl_errno=  0. ;

  if(pars_cnt      <  1  ||
     pars[0]->addr==NULL   ) {
                                        dcl_errno=ENOENT ;
                                        ret_value=-1. ;
                                return(&dgt_return) ;
                             }
/*------------------------------------------------------- Исполнение */

                               size=pars[0]->size ;
      if(size>=pars[0]->buff)  size=pars[0]->buff-1 ;

                            path=(char *)pars[0]->addr ;
                            path[size]=0 ;

         ret_value=access(path, 0x00) ;
      if(ret_value<0)  dcl_errno=errno ;

/*-------------------------------------------------------------------*/

  return(&dgt_return) ;
}

/*****************************************************************/
/*                                                               */
/*       Преобразование кодовой страницы текстового файла        */
/*                                                               */
/*   double  f_cp_convert(path) ;                                */
/*                                                               */
/*           char *path_i - путь к исходному файлу               */
/*           char *path_o - путь к преобразованному файлу        */
/*           char *cp_i   - исходная кодировка                   */
/*           char *cp_o   - нужная кодировка                     */
/*                                                               */
/*  Возврашает 0, если успешно и -1 - в противном случае.        */
/*  Код ошибки errno :                                           */
/*       EACCES, ENOENT - согласно C-стандарту                   */

  Dcl_decl *Dcl_f_cp_convert(Lang_DCL *Kernel, Dcl_decl **pars, int  pars_cnt)

{
         char  i_path[_BUFF_SIZE] ;   /* Путь к исходному файлу */
         char  o_path[_BUFF_SIZE] ;   /* Путь к преобразованному файлу */
         char  i_cp  [16] ;           /* Исходная кодировка */
         char  o_cp  [16] ;           /* Нужная кодировка */
         FILE *file_i ;
         FILE *file_o ;
          int  size ;
         char *buff ;
         char *end ;

   static   double  ret_value ;          /* Буфер числового значения */
   static Dcl_decl  dgt_return={_DGT_VAL, 0, 0, 0, "", &ret_value, NULL, 1, 1} ;

/*---------------------------------------------------- Инициализация */

          ret_value=0 ;
          dcl_errno=0 ; 

/*------------------------------------------------- Входной контроль */


  if(pars_cnt      <  4  ||
     pars[0]->addr==NULL ||
     pars[1]->addr==NULL ||
     pars[2]->addr==NULL ||
     pars[3]->addr==NULL   ) {
                                            dcl_errno=EINVAL ;
                                            ret_value=-1. ;
                                    return(&dgt_return) ;
                             }
/*-------------------------------------------- Извлечение параметров */

#define   EXTRACT(value, n)  do {                                                 \
                                                          size=pars[n]->size ;    \
                                 if(size>=sizeof(value))  size=sizeof(value)-1 ;  \
                                          memset(value, 0, sizeof(value)) ;       \
                                          memcpy(value, pars[n]->addr, size) ;    \
                               } while(0) 

          EXTRACT(i_path, 0) ;
          EXTRACT(o_path, 1) ;
          EXTRACT(i_cp,   2) ;
          EXTRACT(o_cp,   3) ;

/*-------------------------------------------------- Открытие файлов */

        file_i=NULL ;   
        file_o=NULL ;   

  do {

        file_i=fopen(i_path, "rt") ;                                /* Открываем исходный файл */
     if(file_i==NULL) {                                             /* При неудаче - считываем системную ошибку */
                                dcl_errno=errno ;                   /*       и завершаем работу                 */
                                ret_value=-1. ;
                                  break ;
                      }

        file_o=fopen(o_path, "wt") ;                                /* Открываем преобразованный файл */
     if(file_o==NULL) {                                             /* При неудаче - считываем системную ошибку */
                                dcl_errno=errno ;                   /*       и завершаем работу                 */
                                ret_value=-1. ;
                                  break ;
                      }
/*---------------------------------------------------- Перекодировка */

#define  _ROW_SIZE   64000

                buff=(char *)calloc(_ROW_SIZE, 1) ;

    while(1) {
                  end=fgets(buff, _ROW_SIZE-1, file_i) ;
               if(end==NULL)  break ;

                   Kernel->zCodePageConvert(buff, strlen(buff), i_cp, o_cp) ;

                if(Kernel->mError_code) {
                                          ret_value=-1. ;
                                            break ;
                                        }

                     fwrite(buff, strlen(buff), 1, file_o) ;
             }

                free(buff) ;

#undef  _ROW_SIZE

/*-------------------------------------------------- Закрытие файлов */

  } while(0) ;

        if(file_i!=NULL)  fclose(file_i) ;
        if(file_o!=NULL)  fclose(file_o) ;

/*-------------------------------------------------------------------*/

  return(&dgt_return) ;
}


/*****************************************************************/
/*                                                               */
/*            Проверка наличия ключевого фрагмента в файле       */
/*                                                               */
/*   double  f_check(path, buff, key_1[, key_2...]) ;            */
/*                                                               */
/*        char *path  - имя файла                                */
/*        void *buff  - буфер считывания                         */
/*        char *key_N - ключевой фрагмент                        */
/*                                                               */
/*  Возврашает:  0 - фрагмент не найден, 1 - фрагмент найден,    */
/*              -1 - ошибка                                      */
/*                                                               */
/*  Код ошибки errno :                                           */
/*       EACCES, EMFILE, ENOENT - согласно C-стандарту           */

  Dcl_decl *Dcl_f_check(Lang_DCL *Kernel, Dcl_decl **pars, int  pars_cnt)

{
         char  path[_BUFF_SIZE] ;   /* Буфер символьных строк */
 struct _stat  attr ;               /* Аттрибуты файла */ 
         FILE *file ;  
         char *buff ;
         char *result ;
         char *entry ;
         char *key ;
         long  pos ;
       size_t  cnt ;
         long  tail ;
          int  type ;               /* Базовый тип буфера */
          int  keys_size ;          /* Максимальная длина ключей */
          int  status ;  
          int  i ;

   static   double  ret_value ;          /* Буфер числового значения */
   static Dcl_decl  dgt_return={_DGT_VAL, 0, 0, 0, "", &ret_value, NULL, 1, 1} ;

/*---------------------------------------------------- Инициализация */

                       ret_value= 0 ;
                       dcl_errno= 0 ; 

/*------------------------------------------------- Входной контроль */

  if(pars_cnt      <  3  ||
     pars[0]->addr==NULL ||
     pars[1]->addr==NULL ||
     pars[2]->addr==NULL   ) {
                                            dcl_errno=EINVAL ;
                                            ret_value=-1. ;
                                    return(&dgt_return) ;
                             }

      type=t_base(pars[1]->type) ;
   if(type!=_CHR_AREA &&
      type!=_CHR_PTR    ) {
                                            dcl_errno=EINVAL ;
                                            ret_value=-1. ;
                                    return(&dgt_return) ;
                          }   
/*------------------------------------------- Извлекаем путь к файлу */

    if(pars[0]->size>=sizeof(path))  pars[0]->size=sizeof(path)-1 ; /* Контроль размеров имени файла */

	  memset(path, 0, sizeof(path)) ;
	  memcpy(path, pars[0]->addr, pars[0]->size) ;              /* Занесение имени в рабочий буфер */

/*---------------------------------------- Определяем атрибуты файла */

       status=_stat(path, &attr) ;
    if(status==-1) {                                                /* Если ошибка... */
                              dcl_errno=errno ;
                              ret_value=-1. ;
                      return(&dgt_return) ;
                   }
/*-------------------------------- Определение наибольшего фрагмента */

                                   keys_size=0 ;

    for(i=2 ; i<pars_cnt ;  i++) 
      if(pars[i]->size>keys_size)  keys_size=pars[i]->size ;

/*--------------------------------------------------- Открытие файла */

       file=fopen(path, "rb") ;                                     /* Открываем файл */
    if(file==NULL) {                                                /* Если ошибка... */
                                  dcl_errno=errno ;
                                  ret_value=-1. ;
                          return(&dgt_return) ;
                   }
/*------------------------------------------------- Поиск фрагментов */

#define  _ROW_SIZE   128000

                buff=(char *)calloc(_ROW_SIZE, 1) ;

              result=NULL ;
                tail= 0 ;
                 pos= 0L ;

    do {
Kernel->iLog("Dcl_f_check> ", "do {") ;
/*- - - - - - - - - - - - - - - - - - -  Считывание очередного блока */
               fseek(file, pos, SEEK_SET) ;
           cnt=fread(buff, 1, _ROW_SIZE-1, file) ;
        if(cnt==0)  break; 
/*- - - - - - - - - - - - - - - - - - - - - -  Поиск ключей по блоку */
Kernel->iLog("Dcl_f_check> ", buff) ;
         for(i=2 ; i<pars_cnt ; i++)
           if(pars[i]->size>0) {

                       entry=buff ;
                        tail=cnt-pars[i]->size+1 ;
                         key=(char *)pars[i]->addr ;

             if(tail<0)  continue ;

             do {
                     entry=(char *)memchr(entry, key[0], tail-(entry-buff)) ;
                  if(entry==NULL)  break ;

                  if(!memcmp(entry, key, pars[i]->size))  break ;

                           entry++ ;
                  if(tail-(entry-buff)<=0)  {  entry=NULL ;  break ;  }

                } while(1) ;

             if(entry!=NULL)
              if(result==NULL || result>entry)  result=entry ;
                               }
/*- - - - - - - - - - - - - - - - - - - -  Анализ результатов поиска */
          if(result!=NULL) {
Kernel->iLog("Dcl_f_check> ", "Find...") ;
                                pos+=result-buff ;
                               tail =attr.st_size-pos ;
                                  break ;
                           }
/*- - - - - - - - - - - - - - - - - - - - Переход к следующему блоку */
               pos+=_ROW_SIZE-keys_size ;

Kernel->iLog("Dcl_f_check> ", "} while(...)") ;
       } while(cnt==_ROW_SIZE) ;

                free(buff) ;

#undef  _ROW_SIZE

/*----------------------------------- Если ключевые слова не найдены */

  if(result==NULL) {
                              fclose(file) ;

                               ret_value=0. ;
                       return(&dgt_return) ;
                   }
/*------------------------------------ Если задан динамический буфер */
Kernel->iLog("Dcl_f_check> ", "1") ;

  if( pars[1]->prototype!=NULL &&
     *pars[1]->prototype=='D'    ) {

       if(tail>pars[1]->buff) {                                     /* Если довыделение памяти... */
            Kernel->iBuff_realloc(pars[1], NULL, tail) ;              
         if(Kernel->mError_code) { 
                                    dcl_errno=Kernel->mError_code ;
                                    ret_value=-1. ;
                            return(&dgt_return) ;
                                 }
                              }
                                   }
/*------------------------------------------------- Считывание файла */
Kernel->iLog("Dcl_f_check> ", "2") ;

   if(tail>pars[1]->buff)  tail=pars[1]->buff ;                     /* Контроль переполнения буфера данныех */

               fseek(file, pos, SEEK_SET) ;
           cnt=fread((char *)pars[1]->addr, 1, tail, file) ;

              fclose(file) ;

/*--------------------------------------------------------- Концовка */
Kernel->iLog("Dcl_f_check> ", "3") ;

                                 pars[1]->size=cnt ;                /*  Уст.размер считанного обьекта */             
           Kernel->iSize_correct(pars[1]) ;                         /*  Проводим следящую коррекцию     */
								    /*    размеров указателей на обьект */
           ret_value=1. ;
   return(&dgt_return) ;
}
