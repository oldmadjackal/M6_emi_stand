
/*********************************************************************/
/*                                                                   */
/*                   DATA CONVERTION LANGUAGE                        */
/*                                                                   */
/*                 ���������� ������ � �������                       */
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


#pragma warning(disable : 4996)

/*---------------------------------------------------- ������� ����� */

          extern double  dcl_errno ;       /* ��������� ��������� ������ -> DCL_SLIB.CPP */

/*------------------------------------------------ "�������" ������� */

    Dcl_decl *Dcl_f_with      (Lang_DCL *, Dcl_decl **, int) ;    /* ����� ��������� �������� ����� */
    Dcl_decl *Dcl_f_open      (Lang_DCL *, Dcl_decl **, int) ;    /* �������� �������� ����� */
    Dcl_decl *Dcl_f_close     (Lang_DCL *, Dcl_decl **, int) ;    /* �������� �������� ����� */
    Dcl_decl *Dcl_f_seek      (Lang_DCL *, Dcl_decl **, int) ;    /* ���������������� �������� ����� */
    Dcl_decl *Dcl_f_tell      (Lang_DCL *, Dcl_decl **, int) ;    /* ������ ������� � ������� ����� */
    Dcl_decl *Dcl_f_read      (Lang_DCL *, Dcl_decl **, int) ;    /* ���������� �������� ����� */
    Dcl_decl *Dcl_f_readl     (Lang_DCL *, Dcl_decl **, int) ;    /* ���������� ������ �������� ����� */
    Dcl_decl *Dcl_f_readonce  (Lang_DCL *, Dcl_decl **, int) ;    /* ���������� ����� � ���� ����� */
    Dcl_decl *Dcl_f_write     (Lang_DCL *, Dcl_decl **, int) ;    /* ������ �������� ����� */
    Dcl_decl *Dcl_f_writeonce (Lang_DCL *, Dcl_decl **, int) ;    /* ������ ����� � ���� ����� */
    Dcl_decl *Dcl_f_unlink    (Lang_DCL *, Dcl_decl **, int) ;    /* �������� ����� */
    Dcl_decl *Dcl_f_exists    (Lang_DCL *, Dcl_decl **, int) ;    /* �������� ������������� ����� ��� ������� */
    Dcl_decl *Dcl_f_cp_convert(Lang_DCL *, Dcl_decl **, int) ;    /* �������������� ������� �������� ���������� ����� */
    Dcl_decl *Dcl_f_check     (Lang_DCL *, Dcl_decl **, int) ;    /* �������� ������� ��������� ��������� � ����� */

/*------------------------------------------------ �������� �������� */

	 static double      o_creat =O_CREAT ;   /* �������� ����������� ����� */
	 static double      o_excl  =O_EXCL ;
	 static double      o_trunc =O_TRUNC ;
	 static double      o_append=O_APPEND ;
	 static double      o_rdwr  =O_RDWR ;
	 static double      seek_set=SEEK_SET ;
	 static double      seek_cur=SEEK_CUR ;
	 static double      seek_end=SEEK_END ;

	 static double      eacces=EACCES ;      /* �������� ����� ������ */
	 static double      eexist=EEXIST ;
	 static double      emfile=EMFILE ;
	 static double      enoent=ENOENT ;
	 static double      enomem=ENOMEM ;
	 static double      einval=EINVAL ;
	 static double      enospc=ENOSPC ;
	 static double      ebadf =EBADF ;
	 static double      eof_=EOF ;

/*--------------------------------------------------- ������� ������ */

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

/*----------------------------------------------- ������� ���������� */

#undef   _BUFF_SIZE
#define  _BUFF_SIZE   8192

        static      int  dcl_rfile ;           /* ������� ���� */
        static      int  dcl_rfile_written ;   /* ����� ������� ������ � ������� ���� */


/*****************************************************************/
/*                                                               */
/*          ����� ��������� �������� �����                       */
/*                                                               */
/*        int  f_with(file) ;                                    */
/*                                                               */
/*            int  file - ���������� �����                       */
/*                                                               */
/*  ���������� ���������� ����������� ��������� �������� �����   */

  Dcl_decl *Dcl_f_with(Lang_DCL *Kernel, Dcl_decl **pars, int  pars_cnt)

{
   static   double  ret_value ;          /* ����� ��������� �������� */
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
/*                 �������� �������� �����                       */
/*                                                               */
/*   double  f_open(path [, attr]) ;                             */
/*                                                               */
/*           char *path - ��� �����                              */
/*         double  attr - �������� : O_CREAT, O_EXCL, O_TRUNC,   */
/*                                   O_RDWR, O_APPEND            */
/*                                                               */
/*     ���� attr ������ ���������� attr=0.                       */
/*                                                               */
/*     ����� �/� ��������� ������:                               */
/*        open(name, attr|O_BINARY, S_IREAD|S_IWRITE)            */
/*                                                               */
/*  ���������� ���������� �����, ���� ���� ������ ���������      */
/*   ��� -1 - � ��������� ������.                                */
/*  ��� ������ errno :                                           */
/*       EACCES, EEXIST, EMFILE, ENOENT - �������� C-���������   */

  Dcl_decl *Dcl_f_open(Lang_DCL *Kernel, Dcl_decl **pars, int  pars_cnt)

{
              char  path[_BUFF_SIZE] ;   /* ����� ���������� ����� */
               int  attr ;

   static   double  ret_value ;          /* ����� ��������� �������� */
   static Dcl_decl  dgt_return={_DGT_VAL, 0, 0, 0, "", &ret_value, NULL, 1, 1} ;

/*---------------------------------------- ���������� �������� ����� */

//     if(dcl_rfile>0)  Dcl_f_close(Kernel, NULL, 0) ;              /* ���� ������� ���� ��� ������ - */
//              						    /*   ��������� ���                */
/*--------------------------------------------------- �������� ����� */

            dcl_errno=0. ;

    if(pars[0]->size>=sizeof(path))  pars[0]->size=sizeof(path)-1 ; /* �������� �������� ����� ����� */

	  memset(path, 0, sizeof(path)) ;
	  memcpy(path, pars[0]->addr, pars[0]->size) ;              /* ��������� ����� � ������� ����� */

       if(pars_cnt     < 2  ||                                      /* '���������' �������� ����� */
	  pars[1]->addr==NULL )  attr= 0 ;
       else                      attr=(int)Kernel->iDgt_get(pars[1]->addr, pars[1]->type) ;

       if((attr & O_CREAT )==O_CREAT  ||                            /* ������������ �������� ������� */
          (attr & O_APPEND)==O_APPEND   )  attr|=O_RDWR ;           /*     ��� �������� ������       */

	dcl_rfile=open(path, attr | O_BINARY,                       /* ��������� ���� */
					S_IREAD | S_IWRITE) ;
     if(dcl_rfile==-1)  dcl_errno=errno ;                           /* ��� ������� - ��������� ��������� ������ */

/*--------------------------------------------------------- �������� */

          ret_value=dcl_rfile ;
   return(&dgt_return) ;
}


/*****************************************************************/
/*                                                               */
/*                 �������� �������� �����                       */
/*                                                               */
/*        void  f_close([file]) ;                                */
/*                                                               */
/*            int  file - ���������� �����                       */
/*                                                               */
/*  ���������� 0 ��� �������� �������� ��� -1 - ��� �������      */
/*  ��� ������ errno :  EBADF - �������� C-���������             */

  Dcl_decl *Dcl_f_close(Lang_DCL *Kernel, Dcl_decl **pars, int  pars_cnt)

{
               int  file ;
 
   static   double  ret_value ;          /* ����� ��������� �������� */
   static Dcl_decl  dgt_return={_DGT_VAL, 0, 0, 0, "", &ret_value, NULL, 1, 1} ;

/*------------------------------------------------- ������� �������� */

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
/*---------------------------------------- ���������� �������� ����� */

        ret_value=close(file) ;                                     /* ��������� ���� */
     if(ret_value==-1)  dcl_errno=errno ;                           /* ��� ������� - ��������� ��������� ������ */

/*-------------------------------------------------------------------*/

      return(&dgt_return) ;
}


/*****************************************************************/
/*                                                               */
/*            ���������������� �������� �����                    */
/*                                                               */
/*    double  f_seek(offset, base) ;                             */
/*                                                               */
/*      double  offset - �������� �������                        */
/*      double  base   - ��� ����: SEEK_SET, SEEK_CUR, SEEK_END  */
/*                                                               */
/*     ����� �/� ��������� ������:                               */
/*       lseek(..., offset, base)                                */
/*                                                               */
/*  ���������� ������� ������� ��� -1 - ��� �������              */
/*  ��� ������ errno :  EBADF, EINVAL - �������� C-���������     */

  Dcl_decl *Dcl_f_seek(Lang_DCL *Kernel, Dcl_decl **pars, int  pars_cnt)

{
             int  base ;
            long  offset ;
 static   double  ret_value ;          /* ����� ��������� �������� */
 static Dcl_decl  dgt_return={_DGT_VAL, 0, 0, 0, "", &ret_value, NULL, 1, 1} ;


                dcl_errno=0. ;

    if(dcl_rfile<=0) {         dcl_errno=EBADF ;    
                               ret_value= -1 ;
                       return(&dgt_return) ;      }

      offset=(long)Kernel->iDgt_get(pars[0]->addr, pars[0]->type) ; /* '�����������' ��� ���� � �������� */
        base=( int)Kernel->iDgt_get(pars[1]->addr, pars[1]->type) ;

      ret_value=lseek(dcl_rfile, offset, base) ;                    /* ������������� ������� ���� */
   if(ret_value<0)  dcl_errno=errno ;	                            /* ���� �������� - ��������� ��������� ������ */

  return(&dgt_return) ;
}


/*****************************************************************/
/*                                                               */
/*         ������ ������� ������� � ������� �����                */
/*                                                               */
/*    double  f_tell(void) ;                                     */
/*                                                               */
/*     ����� �/� ��������� ������:                               */
/*        tell(...)                                              */
/*                                                               */
/*  ���������� ������� ������� ��� -1 - ��� �������              */
/*  ��� ������ errno :  EBADF - �������� C-���������             */

  Dcl_decl *Dcl_f_tell(Lang_DCL *Kernel, Dcl_decl **pars, int  pars_cnt)

{
 static   double  ret_value ;          /* ����� ��������� �������� */
 static Dcl_decl  dgt_return={_DGT_VAL, 0, 0, 0, "", &ret_value, NULL, 1, 1} ;


                dcl_errno=0. ;

    if(dcl_rfile<=0) {         dcl_errno=EBADF ;    
                               ret_value= -1 ;
                       return(&dgt_return) ;      }

       ret_value=lseek(dcl_rfile, 0L, SEEK_CUR) ;                   /* ������ ������� ������� � ����� */
    if(ret_value<0)  dcl_errno=errno ;	                            /* ���� �������� - ��������� ��������� ������ */

  return(&dgt_return) ;
}


/*****************************************************************/
/*                                                               */
/*                ���������� �������� �����                      */
/*                                                               */
/*    double  f_read(buff [, cnt]) ;                             */
/*                                                               */
/*        void *buff - ����� ����������                          */
/*      double  cnt  - ������� ����������                        */
/*                                                               */
/*    ���� cnt ������, �� ���������� cnt=buffof(buff)            */
/*                                                               */
/*  ���������� ����� ��������� ����, -1 - ��� ������             */
/*  ��� ������ errno :  EBADF - �������� C-���������             */

  Dcl_decl *Dcl_f_read(Lang_DCL *Kernel, Dcl_decl **pars, int  pars_cnt)

{
             int  type ;   /* ������� ��� ������ */
             int  size ;   /* ������ ���������� */

 static   double  ret_value ;          /* ����� ��������� �������� */
 static Dcl_decl  dgt_return={_DGT_VAL, 0, 0, 0, "", &ret_value, NULL, 1, 1} ;


                                  dcl_errno=0 ;

       if(dcl_rfile<=0) {         dcl_errno=EBADF ;    
                                  ret_value= -1 ;
                          return(&dgt_return) ;      }

	  type=t_base(pars[0]->type) ;

       if(pars[0]      ==NULL ||                                    /* ���. ������� ���������� */
	  pars[0]->addr==NULL   )  size= 0 ;                        /*  � ����������� �� ����� */
  else if(pars_cnt      <  2  ||                                    /*   ����������            */
	  pars[1]->addr==NULL   )  size=pars[0]->buff ;
  else                             size=(int)Kernel->iDgt_get(pars[1]->addr, pars[1]->type) ;

   if(size>pars[0]->buff)  size=pars[0]->buff ;                     /* �������� ������������ ������ ������� */

	if(type==_DGT_VAL  ||                                       /* ���������� ��������    */
	   type==_DGT_AREA ||                                       /*    �������� ���������� */
	   type==_DGT_PTR    )  size*=Kernel->iDgt_size(pars[0]->type) ;

           ret_value=read(dcl_rfile, pars[0]->addr, size) ;         /* ��������� �� �������� ����� */
        if(ret_value<0) {                                           /* ���� ������... */
                            dcl_errno=errno ;
			    ret_value= -1 ;
			     return(&dgt_return) ;
                        }

                                 pars[0]->size=(int)ret_value ;     /* ���.������ ���������� ������� */             
           Kernel->iSize_correct(pars[0]) ;                         /* �������� �������� ���������     */
								    /*   �������� ���������� �� ������ */
  return(&dgt_return) ;
}


/*****************************************************************/
/*                                                               */
/*            ���������� ������ �� �������� �����                */
/*                                                               */
/*    double  f_readl(buff [, end]) ;                            */
/*                                                               */
/*        void *buff - ����� ����������                          */
/*        char *end  - ����� ����� ������                        */
/*                                                               */
/*     ���������� ������� �� ����������� ����� ����� ������,     */
/*  ��� �� ����� �����, ��� �� ����� ������. ���� end ������, �� */
/*  �/� �������� ���������� f_read(buff).                        */
/*                                                               */
/*  ����������: ����� ��������� ����, -1 - ������                */
/*  ��� ������ errno :  EBADF - �������� C-���������             */
/*  ��� ���������� ����� ���������� 0 � ��� ������ EOF           */

  Dcl_decl *Dcl_f_readl(Lang_DCL *Kernel, Dcl_decl **pars, int  pars_cnt)

{
            char *addr ;
            char *end ;
             int  end_size ;
             int  size ;
             int  cnt ;
             int  len ;
            long  pos ;
            char  buff[_BUFF_SIZE] ;   /* ����� ���������� ����� */
             int  i ;

 static   double  ret_value ;          /* ����� ��������� �������� */
 static Dcl_decl  dgt_return={_DGT_VAL, 0, 0, 0, "", &ret_value, NULL, 1, 1} ;

/*---------------------------------------------------- ������������� */

                            dcl_errno=0 ;

       if(dcl_rfile<=0) {         dcl_errno=EBADF ;                 /* �������� ������� �������� ����� */
                                  ret_value= -1 ;
                          return(&dgt_return) ;      }

       if(pars[0]      ==NULL ||                                    /* ��������� ���������� ���������� */
	  pars[0]->addr==NULL   ) {  ret_value= 0 ;
				    return(&dgt_return) ;  }

       if(pars_cnt     >=  2  &&                                    /* ���. �������� ����� */
	  pars[1]->addr!=NULL   ) {  end     =(char *)pars[1]->addr ;
				     end_size=        pars[1]->size ;  }
       else                       {  end     =NULL ;
                                     end_size= 0 ;                     }

/*------------------------------------------- ���������������� ����� */

           pos=lseek(dcl_rfile, 0L, SEEK_CUR) ;                     /* ������ ������� ������� */ 
           cnt= read(dcl_rfile, buff, _BUFF_SIZE) ;                 /* ��������� �� �������� ����� */
        if(cnt< 0) {                                                /* ���� ������... */
                      dcl_errno=errno ;
                      ret_value= -1 ;
		        return(&dgt_return) ;
                   }
        if(cnt==0) {                                                /* ���� ���� ����������... */
                      dcl_errno=EOF ;
                      ret_value= 0 ;
                        return(&dgt_return) ;
                   }
/*--------------------------------------------- ����� �������� ����� */

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

/*--------------------------------------------- ����������� �� ����� */

	      if(size>pars[0]->buff)  size=pars[0]->buff ;

          lseek(dcl_rfile, pos+size+end_size, SEEK_SET) ;           /* ���������.���� �� ��������� �������� */

		memcpy(pars[0]->addr, buff, size) ;

		      pars[0]->size=size ;

		    Kernel->iSize_correct(pars[0]) ;                /* �������� �������� ���������     */
								    /*   �������� ���������� �� ������ */
/*-------------------------------------------------------------------*/

       ret_value=size ;

  return(&dgt_return) ;
}


/*****************************************************************/
/*                                                               */
/*            ���������� ����� � ���� �����                      */
/*                                                               */
/*   double  f_readonce(path, buff [, cnt]) ;                    */
/*                                                               */
/*        char *path - ��� �����                                 */
/*        void *buff - ����� ����������                          */
/*      double  cnt  - ������� ����������                        */
/*                                                               */
/*  ���������� ����� �������� ���� � -1 - � ������ ������        */
/*                                                               */
/*  ��� ������ errno :                                           */
/*       EACCES, EMFILE, ENOENT - �������� C-���������           */

  Dcl_decl *Dcl_f_readonce(Lang_DCL *Kernel, Dcl_decl **pars, int  pars_cnt)

{
         char  path[_BUFF_SIZE] ;   /* ����� ���������� ����� */
 struct _stat  attr ;               /* ��������� ����� */ 
          int  file ;  
          int  type ;              /* ������� ��� ������ */
         long  size ;              /* ������ ���������� */
          int  status ;  

   static   double  ret_value ;          /* ����� ��������� �������� */
   static Dcl_decl  dgt_return={_DGT_VAL, 0, 0, 0, "", &ret_value, NULL, 1, 1} ;

/*------------------------------------------------- ������� �������� */

                                            dcl_errno= 0 ; 

  if(pars_cnt      <  2  ||
     pars[0]->addr==NULL ||
     pars[1]->addr==NULL   ) {
                                            dcl_errno=EINVAL ;
                                            ret_value=-1. ;
                                    return(&dgt_return) ;
                             }
/*------------------------------------------- ��������� ���� � ����� */

    if(pars[0]->size>=sizeof(path))  pars[0]->size=sizeof(path)-1 ; /* �������� �������� ����� ����� */

	  memset(path, 0, sizeof(path)) ;
	  memcpy(path, pars[0]->addr, pars[0]->size) ;              /* ��������� ����� � ������� ����� */

       status=_stat(path, &attr) ;
    if(status==-1) {                                                /* ���� ������... */
                              dcl_errno=errno ;
                              ret_value=-1. ;
                      return(&dgt_return) ;
                   }
/*------------------------------------- ���� ����� ������ ���������� */

   if(pars_cnt      <  3  ||
      pars[2]->addr==NULL   )  size=  0 ;
   else                        size=(int)Kernel->iDgt_get(pars[2]->addr, pars[2]->type) ;

/*------------------------------------ ���� ����� ������������ ����� */

  if( pars[1]->prototype!=NULL &&
     *pars[1]->prototype=='D'    ) {

       if(size==0)  size=attr.st_size ;                             /* ���� ��������� ���� ����... */
 
       if(size>pars[1]->buff) {                                     /* ���� ����������� ������... */
            Kernel->iBuff_realloc(pars[1], NULL, size) ;              
         if(Kernel->mError_code) { 
                                    dcl_errno=Kernel->mError_code ;
                                    ret_value=-1. ;
                            return(&dgt_return) ;
                                 }
                              }
                                   }
/*--------------------------------------------------- �������� ����� */

    if(size==0)  size=pars[1]->buff ;                               /* ���� ��������� �� ������� ������... */

       file=open(path, O_BINARY | O_RDONLY) ;                       /* ��������� ���� */
    if(file==-1) {                                                  /* ���� ������... */
                                  dcl_errno=errno ;
                                  ret_value=-1. ;
                          return(&dgt_return) ;
                 }
/*------------------------------------------------- ���������� ����� */

   if(size==0)  size=pars[1]->buff ;

   if(size>pars[1]->buff)  size=pars[1]->buff ;                     /* �������� ������������ ������ ������� */

      type=t_base(pars[1]->type) ;
   if(type==_DGT_VAL  ||                                            /* ���������� ��������    */
      type==_DGT_AREA ||                                            /*    �������� ���������� */
      type==_DGT_PTR    )  size*=Kernel->iDgt_size(pars[1]->type) ;

           ret_value=read(file, pars[1]->addr, size) ;              /* ��������� ������ �� ����� */
        if(ret_value<0)  dcl_errno=errno ;                          /* ���� ������... */

/*--------------------------------------------------- �������� ����� */

                close(file) ;

/*--------------------------------------------------------- �������� */

    if(ret_value>=0) {                                              /* ���� ������� ������... */ 
                                 pars[1]->size=(int)ret_value ;     /*  ���.������ ���������� ������� */             
           Kernel->iSize_correct(pars[1]) ;                         /*  �������� �������� ���������     */
								    /*    �������� ���������� �� ������ */
                     }

   return(&dgt_return) ;
}


/*****************************************************************/
/*                                                               */
/*                 ������ �������� �����                         */
/*                                                               */
/*    double  f_write(buff) ;                                    */
/*                                                               */
/*        void  buff - ����� ������                              */
/*                                                               */
/*  � ������� ���� ������������ ����� ���� �����. sizeof(buff).  */
/*                                                               */
/*  ���������� ����� ��������� ���� ��� -1 - ��� �������         */
/*  ��� ������ errno :  EBADF, ENOSPC - �������� C-���������     */

  Dcl_decl *Dcl_f_write(Lang_DCL *Kernel, Dcl_decl **pars, int  pars_cnt)

{
             int  type ;  /* ������� ��� ������ */
             int  size ;

 static   double  ret_value ;          /* ����� ��������� �������� */
 static Dcl_decl  dgt_return={_DGT_VAL, 0, 0, 0, "", &ret_value, NULL, 1, 1} ;


                                   dcl_errno= 0 ; 

        if(dcl_rfile<=0) {         dcl_errno=EBADF ;                /* �������� ������� �������� ����� */
                                   ret_value= -1 ;
                           return(&dgt_return) ;      }
 
	  type=t_base(pars[0]->type) ;


	if(pars[0]      ==NULL ||                                   /* ��������� ������ �������� */
	   pars[0]->addr==NULL   )  size= 0 ;
	else                        size=pars[0]->size ;

	if(type==_DGT_VAL  ||                                       /* ���������� ��������    */
	   type==_DGT_AREA ||                                       /*    �������� ���������� */
	   type==_DGT_PTR    )  size*=(int)Kernel->iDgt_size(pars[0]->type) ;

	   ret_value=write(dcl_rfile, pars[0]->addr, size) ;        /* ���������� � ������� ���� */
        if(ret_value<0)  dcl_errno=errno ;                          /* ���� �������� - ��������� ��������� ������ */

  return(&dgt_return) ;
}


/*****************************************************************/
/*                                                               */
/*            ������ ����� � ���� �����                          */
/*                                                               */
/*   double  f_writeonce(path, buff [, cnt]) ;                   */
/*                                                               */
/*        char *path - ��� �����                                 */
/*        void *buff - ����� ������������ ������                 */
/*      double  cnt  - ������� ������                            */
/*                                                               */
/*  ���������� ����� ���������� ���� � -1 - � ������ ������      */
/*                                                               */
/*  ��� ������ errno :                                           */
/*       EACCES, EMFILE, ENOENT, ENOSPC - �������� C-���������   */

  Dcl_decl *Dcl_f_writeonce(Lang_DCL *Kernel, Dcl_decl **pars, int  pars_cnt)

{
         char  path[_BUFF_SIZE] ;   /* ���� � ����� */
          int  file ;  
          int  type ;               /* ������� ��� ������ */
          int  size ;               /* ������ ���������� */

   static   double  ret_value ;          /* ����� ��������� �������� */
   static Dcl_decl  dgt_return={_DGT_VAL, 0, 0, 0, "", &ret_value, NULL, 1, 1} ;

/*------------------------------------------------- ������� �������� */

                                            dcl_errno= 0 ; 

  if(pars_cnt      <  2  ||
     pars[0]->addr==NULL ||
     pars[1]->addr==NULL   ) {
                                            dcl_errno=EINVAL ;
                                            ret_value=-1. ;
                                    return(&dgt_return) ;
                             }
/*--------------------------------------------------- �������� ����� */

    if(pars[0]->size>=sizeof(path))  pars[0]->size=sizeof(path)-1 ; /* �������� �������� ����� ����� */

	  memset(path, 0, sizeof(path)) ;
	  memcpy(path, pars[0]->addr, pars[0]->size) ;              /* ��������� ����� � ������� ����� */

       file=open(path, O_CREAT | O_TRUNC | O_BINARY | O_RDWR,       /* ��������� ���� */
                                            S_IREAD | S_IWRITE) ; 
    if(file==-1) {                                                  /* ���� ������... */
                                  dcl_errno=errno ;
                                  ret_value=-1. ;
                          return(&dgt_return) ;
                 }  
/*----------------------------------------------------- ������ ����� */

   if(pars_cnt      <  3  ||                                        /* ���������� ������ ���������� */
      pars[2]->addr==NULL   )  size=pars[1]->size ;
   else                        size=(int)Kernel->iDgt_get(pars[2]->addr, pars[2]->type) ;

      type=t_base(pars[1]->type) ;
   if(type==_DGT_VAL  ||                                            /* ���������� ��������    */
      type==_DGT_AREA ||                                            /*    �������� ���������� */
      type==_DGT_PTR    )  size*=Kernel->iDgt_size(pars[1]->type) ;

   if(size>0) {
                   ret_value=write(file, pars[1]->addr, size) ;     /* ����� ������ � ���� */
                if(ret_value==-1)  dcl_errno=errno ;
              }  
/*--------------------------------------------------- �������� ����� */

                close(file) ;

/*--------------------------------------------------------- �������� */

   return(&dgt_return) ;
}


/*****************************************************************/
/*                                                               */
/*                 �������� �����                                */
/*                                                               */
/*   double  f_unlink(path) ;                                    */
/*                                                               */
/*           char *path - ���� �����                             */
/*                                                               */
/*     ����� �/� ��������� ������:  unlink(path)                 */
/*                                                               */
/*  ���������� 0, ���� ���� ������ � -1 - � ��������� ������.    */
/*  ��� ������ errno :                                           */
/*       EACCES, ENOENT - �������� C-���������                   */

  Dcl_decl *Dcl_f_unlink(Lang_DCL *Kernel, Dcl_decl **pars, int  pars_cnt)

{
              char *path ;
	       int  size ;

   static   double  ret_value ;          /* ����� ��������� �������� */
   static Dcl_decl  dgt_return={_DGT_VAL, 0, 0, 0, "", &ret_value, NULL, 1, 1} ;

/*------------------------------------------------- ������� �������� */

                                        dcl_errno=  0. ;

  if(pars_cnt      <  1  ||
     pars[0]->addr==NULL   ) {
                                        dcl_errno=ENOENT ;
                                        ret_value=-1. ;
                                return(&dgt_return) ;
                             }
/*------------------------------------------------------- ���������� */

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
/*         �������� ������������� ����� ��� �������              */
/*                                                               */
/*   double  f_exists(path) ;                                    */
/*                                                               */
/*           char *path - ���� �����                             */
/*                                                               */
/*     ����� �/� ��������� ������:  access(path, 0x00)           */
/*                                                               */
/*  ���������� 0, ���� ���� ���������� ���                       */
/*             EACCES, ENOENT - �������� C-���������             */

  Dcl_decl *Dcl_f_exists(Lang_DCL *Kernel, Dcl_decl **pars, int  pars_cnt)

{
              char *path ;
	       int  size ;

   static   double  ret_value ;          /* ����� ��������� �������� */
   static Dcl_decl  dgt_return={_DGT_VAL, 0, 0, 0, "", &ret_value, NULL, 1, 1} ;

/*------------------------------------------------- ������� �������� */

                                        dcl_errno=  0. ;

  if(pars_cnt      <  1  ||
     pars[0]->addr==NULL   ) {
                                        dcl_errno=ENOENT ;
                                        ret_value=-1. ;
                                return(&dgt_return) ;
                             }
/*------------------------------------------------------- ���������� */

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
/*       �������������� ������� �������� ���������� �����        */
/*                                                               */
/*   double  f_cp_convert(path) ;                                */
/*                                                               */
/*           char *path_i - ���� � ��������� �����               */
/*           char *path_o - ���� � ���������������� �����        */
/*           char *cp_i   - �������� ���������                   */
/*           char *cp_o   - ������ ���������                     */
/*                                                               */
/*  ���������� 0, ���� ������� � -1 - � ��������� ������.        */
/*  ��� ������ errno :                                           */
/*       EACCES, ENOENT - �������� C-���������                   */

  Dcl_decl *Dcl_f_cp_convert(Lang_DCL *Kernel, Dcl_decl **pars, int  pars_cnt)

{
         char  i_path[_BUFF_SIZE] ;   /* ���� � ��������� ����� */
         char  o_path[_BUFF_SIZE] ;   /* ���� � ���������������� ����� */
         char  i_cp  [16] ;           /* �������� ��������� */
         char  o_cp  [16] ;           /* ������ ��������� */
         FILE *file_i ;
         FILE *file_o ;
          int  size ;
         char *buff ;
         char *end ;

   static   double  ret_value ;          /* ����� ��������� �������� */
   static Dcl_decl  dgt_return={_DGT_VAL, 0, 0, 0, "", &ret_value, NULL, 1, 1} ;

/*---------------------------------------------------- ������������� */

          ret_value=0 ;
          dcl_errno=0 ; 

/*------------------------------------------------- ������� �������� */


  if(pars_cnt      <  4  ||
     pars[0]->addr==NULL ||
     pars[1]->addr==NULL ||
     pars[2]->addr==NULL ||
     pars[3]->addr==NULL   ) {
                                            dcl_errno=EINVAL ;
                                            ret_value=-1. ;
                                    return(&dgt_return) ;
                             }
/*-------------------------------------------- ���������� ���������� */

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

/*-------------------------------------------------- �������� ������ */

        file_i=NULL ;   
        file_o=NULL ;   

  do {

        file_i=fopen(i_path, "rt") ;                                /* ��������� �������� ���� */
     if(file_i==NULL) {                                             /* ��� ������� - ��������� ��������� ������ */
                                dcl_errno=errno ;                   /*       � ��������� ������                 */
                                ret_value=-1. ;
                                  break ;
                      }

        file_o=fopen(o_path, "wt") ;                                /* ��������� ��������������� ���� */
     if(file_o==NULL) {                                             /* ��� ������� - ��������� ��������� ������ */
                                dcl_errno=errno ;                   /*       � ��������� ������                 */
                                ret_value=-1. ;
                                  break ;
                      }
/*---------------------------------------------------- ������������� */

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

/*-------------------------------------------------- �������� ������ */

  } while(0) ;

        if(file_i!=NULL)  fclose(file_i) ;
        if(file_o!=NULL)  fclose(file_o) ;

/*-------------------------------------------------------------------*/

  return(&dgt_return) ;
}


/*****************************************************************/
/*                                                               */
/*            �������� ������� ��������� ��������� � �����       */
/*                                                               */
/*   double  f_check(path, buff, key_1[, key_2...]) ;            */
/*                                                               */
/*        char *path  - ��� �����                                */
/*        void *buff  - ����� ����������                         */
/*        char *key_N - �������� ��������                        */
/*                                                               */
/*  ����������:  0 - �������� �� ������, 1 - �������� ������,    */
/*              -1 - ������                                      */
/*                                                               */
/*  ��� ������ errno :                                           */
/*       EACCES, EMFILE, ENOENT - �������� C-���������           */

  Dcl_decl *Dcl_f_check(Lang_DCL *Kernel, Dcl_decl **pars, int  pars_cnt)

{
         char  path[_BUFF_SIZE] ;   /* ����� ���������� ����� */
 struct _stat  attr ;               /* ��������� ����� */ 
         FILE *file ;  
         char *buff ;
         char *result ;
         char *entry ;
         char *key ;
         long  pos ;
       size_t  cnt ;
         long  tail ;
          int  type ;               /* ������� ��� ������ */
          int  keys_size ;          /* ������������ ����� ������ */
          int  status ;  
          int  i ;

   static   double  ret_value ;          /* ����� ��������� �������� */
   static Dcl_decl  dgt_return={_DGT_VAL, 0, 0, 0, "", &ret_value, NULL, 1, 1} ;

/*---------------------------------------------------- ������������� */

                       ret_value= 0 ;
                       dcl_errno= 0 ; 

/*------------------------------------------------- ������� �������� */

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
/*------------------------------------------- ��������� ���� � ����� */

    if(pars[0]->size>=sizeof(path))  pars[0]->size=sizeof(path)-1 ; /* �������� �������� ����� ����� */

	  memset(path, 0, sizeof(path)) ;
	  memcpy(path, pars[0]->addr, pars[0]->size) ;              /* ��������� ����� � ������� ����� */

/*---------------------------------------- ���������� �������� ����� */

       status=_stat(path, &attr) ;
    if(status==-1) {                                                /* ���� ������... */
                              dcl_errno=errno ;
                              ret_value=-1. ;
                      return(&dgt_return) ;
                   }
/*-------------------------------- ����������� ����������� ��������� */

                                   keys_size=0 ;

    for(i=2 ; i<pars_cnt ;  i++) 
      if(pars[i]->size>keys_size)  keys_size=pars[i]->size ;

/*--------------------------------------------------- �������� ����� */

       file=fopen(path, "rb") ;                                     /* ��������� ���� */
    if(file==NULL) {                                                /* ���� ������... */
                                  dcl_errno=errno ;
                                  ret_value=-1. ;
                          return(&dgt_return) ;
                   }
/*------------------------------------------------- ����� ���������� */

#define  _ROW_SIZE   128000

                buff=(char *)calloc(_ROW_SIZE, 1) ;

              result=NULL ;
                tail= 0 ;
                 pos= 0L ;

    do {
Kernel->iLog("Dcl_f_check> ", "do {") ;
/*- - - - - - - - - - - - - - - - - - -  ���������� ���������� ����� */
               fseek(file, pos, SEEK_SET) ;
           cnt=fread(buff, 1, _ROW_SIZE-1, file) ;
        if(cnt==0)  break; 
/*- - - - - - - - - - - - - - - - - - - - - -  ����� ������ �� ����� */
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
/*- - - - - - - - - - - - - - - - - - - -  ������ ����������� ������ */
          if(result!=NULL) {
Kernel->iLog("Dcl_f_check> ", "Find...") ;
                                pos+=result-buff ;
                               tail =attr.st_size-pos ;
                                  break ;
                           }
/*- - - - - - - - - - - - - - - - - - - - ������� � ���������� ����� */
               pos+=_ROW_SIZE-keys_size ;

Kernel->iLog("Dcl_f_check> ", "} while(...)") ;
       } while(cnt==_ROW_SIZE) ;

                free(buff) ;

#undef  _ROW_SIZE

/*----------------------------------- ���� �������� ����� �� ������� */

  if(result==NULL) {
                              fclose(file) ;

                               ret_value=0. ;
                       return(&dgt_return) ;
                   }
/*------------------------------------ ���� ����� ������������ ����� */
Kernel->iLog("Dcl_f_check> ", "1") ;

  if( pars[1]->prototype!=NULL &&
     *pars[1]->prototype=='D'    ) {

       if(tail>pars[1]->buff) {                                     /* ���� ����������� ������... */
            Kernel->iBuff_realloc(pars[1], NULL, tail) ;              
         if(Kernel->mError_code) { 
                                    dcl_errno=Kernel->mError_code ;
                                    ret_value=-1. ;
                            return(&dgt_return) ;
                                 }
                              }
                                   }
/*------------------------------------------------- ���������� ����� */
Kernel->iLog("Dcl_f_check> ", "2") ;

   if(tail>pars[1]->buff)  tail=pars[1]->buff ;                     /* �������� ������������ ������ ������� */

               fseek(file, pos, SEEK_SET) ;
           cnt=fread((char *)pars[1]->addr, 1, tail, file) ;

              fclose(file) ;

/*--------------------------------------------------------- �������� */
Kernel->iLog("Dcl_f_check> ", "3") ;

                                 pars[1]->size=cnt ;                /*  ���.������ ���������� ������� */             
           Kernel->iSize_correct(pars[1]) ;                         /*  �������� �������� ���������     */
								    /*    �������� ���������� �� ������ */
           ret_value=1. ;
   return(&dgt_return) ;
}
