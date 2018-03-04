/*********************************************************************/
/*                                                                   */
/*                   DATA CONVERTION LANGUAGE                        */
/*                                                                   */
/*                 ���������� ��������� �������                      */
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

/*---------------------------------------------------- ������� ����� */

          extern double  dcl_errno ;            /* ��������� ��������� ������ -> DCL_SLIB.CPP */
          extern   char  dcl_err_details[512] ; /* ��������� ��������� ������������ �������� ������ -> DCL_SLIB.CPP */

/*------------------------------------------------ "�������" ������� */

	        double  dcl_errno ;             /* ��������� ��������� ������ */
	          char  dcl_err_details[512] ;  /* ��������� ��������� ������������ �������� ������ */

  Dcl_decl *Dcl_version     (Lang_DCL *, Dcl_decl **, int) ;    /* ������ ������ */
  Dcl_decl *Dcl_set_error   (Lang_DCL *, Dcl_decl **, int) ;    /* ��������� ������ */
  Dcl_decl *Dcl_get_errtext (Lang_DCL *, Dcl_decl **, int) ;    /* ������ ������������ �������� ������ */
  Dcl_decl *Dcl_sizeof      (Lang_DCL *, Dcl_decl **, int) ;    /* ������ ������� ������� */
  Dcl_decl *Dcl_buffof      (Lang_DCL *, Dcl_decl **, int) ;    /* ������ ������� ������ ������� */
  Dcl_decl *Dcl_setsize     (Lang_DCL *, Dcl_decl **, int) ;    /* ��������� ������� ������� */
  Dcl_decl *Dcl_setconstant (Lang_DCL *, Dcl_decl **, int) ;    /* ��������� ������� ��� ������������� */
  Dcl_decl *Dcl_stod        (Lang_DCL *, Dcl_decl **, int) ;    /* �������������� ����� � ������ */
  Dcl_decl *Dcl_dtos        (Lang_DCL *, Dcl_decl **, int) ;    /* �������������� ������ � ������ */
  Dcl_decl *Dcl_dtod        (Lang_DCL *, Dcl_decl **, int) ;    /* �������������� ������ ������ ����� */
  Dcl_decl *Dcl_atos        (Lang_DCL *, Dcl_decl **, int) ;    /* �������������� ������ � ������ */
  Dcl_decl *Dcl_atot        (Lang_DCL *, Dcl_decl **, int) ;    /* �������������� ����������� ������� � ������ ����/����� */
  Dcl_decl *Dcl_time        (Lang_DCL *, Dcl_decl **, int) ;    /* ������ ��������� ���������� */
  Dcl_decl *Dcl_elapsed_time(Lang_DCL *, Dcl_decl **, int) ;    /* ������ ��������� ����� */
  Dcl_decl *Dcl_trim        (Lang_DCL *, Dcl_decl **, int) ;    /* �������� ��������� � �������� �������� */
  Dcl_decl *Dcl_replace     (Lang_DCL *, Dcl_decl **, int) ;    /* ������ ����������� ��������� �� ������ */
  Dcl_decl *Dcl_replace_char(Lang_DCL *, Dcl_decl **, int) ;    /* ������ �������� �� ������ �� ���������� �������� */
  Dcl_decl *Dcl_transpose   (Lang_DCL *, Dcl_decl **, int) ;    /* �������������� ��������� ����� */
  Dcl_decl *Dcl_upper       (Lang_DCL *, Dcl_decl **, int) ;    /* ������� �������� � ������� ������� */
  Dcl_decl *Dcl_cp_convert  (Lang_DCL *, Dcl_decl **, int) ;    /* ������������� ������� �������� ������ */
  Dcl_decl *Dcl_system      (Lang_DCL *, Dcl_decl **, int) ;    /* ���������� shell-������� */
  Dcl_decl *Dcl_sleep       (Lang_DCL *, Dcl_decl **, int) ;    /* ����� � ���������� */
  Dcl_decl *Dcl_log_file    (Lang_DCL *, Dcl_decl **, int) ;    /* ������� ������ � ���-���� */
  Dcl_decl *Dcl_log_event   (Lang_DCL *, Dcl_decl **, int) ;    /* ������� ������ � ��������� ��� ������� */
  Dcl_decl *Dcl_signal      (Lang_DCL *, Dcl_decl **, int) ;    /* �������� ������� */
  Dcl_decl *Dcl_md5         (Lang_DCL *, Dcl_decl **, int) ;    /* ���������� md5-���� ������ */
  Dcl_decl *Dcl_f_md5       (Lang_DCL *, Dcl_decl **, int) ;    /* ���������� md5-���� ������ �� ����� */

/*------------------------------------------------ �������� �������� */

#define   SE_BADFORMAT   1.
#define   SE_STRSIZE     2.
#define   SE_BADSTRING   3.
#define   SE_DATABUFF    4.

	 static double  se_badformat=SE_BADFORMAT ;   /* �������� ����� ������ */
	 static double  se_sizestr  =SE_STRSIZE ;
	 static double  se_badstring=SE_BADSTRING ;
	 static double  se_databuff =SE_DATABUFF ;

/*--------------------------------------------------- ������� ������ */

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
/*----------------------------------------------- ������� ���������� */

#undef   _BUFF_SIZE
#define  _BUFF_SIZE   128

 static  double  dgt_value ;          /* ����� ��������� �������� */
 static    char  buff[_BUFF_SIZE] ;   /* ����� ���������� ����� */

 static Dcl_decl  void_return={_NULL_PTR, 0,0,0,"",    NULL,    NULL, 0, 0} ;
 static Dcl_decl   dgt_return={ _DGT_VAL, 0,0,0,"", &dgt_value, NULL, 1, 1} ;
 static Dcl_decl   chr_return={ _CHR_PTR, 0,0,0,"",    NULL,    NULL, 0, 0} ;

 extern struct Dcl_vars_decl  dcl_result ;     /* �������� ���������� */


/*****************************************************************/
/*                                                               */
/*                   ������ ������                               */
/*                                                               */
/*          char *version(void) ;                                */
/*                                                               */
/*      ������ ������ � ��������������� ������                   */

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
/*                ��������� ������                               */
/*                                                               */
/*            void  set_error(error_code) ;                      */
/*                                                               */
/*              double  error_code  - ��� ������                 */

    Dcl_decl *Dcl_set_error(Lang_DCL *Kernel, Dcl_decl **pars, int  pars_cnt)

{
  double  error ;


      error=Kernel->iDgt_get(pars[0]->addr, pars[0]->type) ;        /* ��������� ��� ������ */

                 dcl_errno=error ;
      Kernel->mError_code =error ;

  return(NULL) ;
}


/*****************************************************************/
/*                                                               */
/*              ������ ������������ �������� ������              */
/*                                                               */
/*          char *get_errtext(void) ;                            */
/*                                                               */
/*      ������ ������ � ��������������� ������                   */

    Dcl_decl *Dcl_get_errtext(Lang_DCL *Kernel, Dcl_decl **pars, int  pars_cnt)

{
	   chr_return.addr=       dcl_err_details ;
	   chr_return.size=strlen(dcl_err_details) ;
	   chr_return.buff=strlen(dcl_err_details) ;

  return(&chr_return) ;
}


/*****************************************************************/
/*                                                               */
/*                   ������ ������� �������                      */
/*                                                               */
/*          double  sizeof(object) ;                             */
/*                                                               */
/*      ������ ������ ������� � ������                           */

    Dcl_decl *Dcl_sizeof(Lang_DCL *Kernel, Dcl_decl **pars, int  pars_cnt)

{
    int  type ;   /* ������� ��� ���������� */


	  type=t_base(pars[0]->type) ;                              /* ��������� ������� ��� */

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
/*             ������ ������� ������ �������                     */
/*                                                               */
/*          double  buffof(var) ;                                */
/*                                                               */
/*      ������ ������ ������ ���������� � ������                 */

    Dcl_decl *Dcl_buffof(Lang_DCL *Kernel, Dcl_decl **pars, int  pars_cnt)

{
    int  type ;    /* ������� ��� ���������� */


	  type=t_base(pars[0]->type) ;                              /* ��������� ������� ��� */

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
/*                ��������� ������� �������                      */
/*                                                               */
/*            void  setsize(object, size) ;                      */
/*                                                               */
/*              double  size  - ������ �������, ����             */
/*                                                               */
/*      ������������� ������ ������� � ������. ��������� ������  */
/*  ��� �������� ��������.                                       */

    Dcl_decl *Dcl_setsize(Lang_DCL *Kernel, Dcl_decl **pars, int  pars_cnt)

{
  int  size ;


      size=(int)Kernel->iDgt_get(pars[1]->addr, pars[1]->type) ;    /* '���������' ������ */

   if(size<pars[0]->buff)  pars[0]->size=         size ;            /* ���.������ ������� � ������ */
   else                    pars[0]->size=pars[0]->buff ;            /*   �������������� ������     */

          Kernel->iSize_correct(pars[0]) ;                          /* �������� �������� ���������     */
                						    /*   �������� ���������� �� ������ */
  return(NULL) ;
}


/*****************************************************************/
/*                                                               */
/*     ��������� ������� �������� ����������� (READ ONLY)        */
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
/*                 �������������� ����� � ������                 */
/*                                                               */
/*           char *stod(format, string, data) ;                  */
/*                                                               */
/*              char *format  - ������ ��������������            */
/*              char *string  - ������������� ������             */
/*              ...   data    - ����� ������                     */
/*                                                               */
/*   ��������� �������: [pref_chr]%[u][pref]type[.point][>]      */
/*                                                               */
/*               type - ������� ���:  d -  int/short/long        */
/*                                    f -  float/double    ;     */
/*               pref - ������� ����: h -  short                 */
/*                                    l -  long/double      ;    */
/*                  u - ������������ unsigned ;                  */
/*              point - ��������� �������, ���� ��� �������  ;   */
/*           pref_chr - ������� ������,                          */
/*                       ��� ��� �����  ............444444       */
/*                                  �  -............444444       */
/*                                 ������� ������ ����� '.' ;    */
/*                  > - ������� ������������������ ���������     */
/*                       ����������. ��� ���������� ����������.  */
/*                         ����� �������. ������ SE_BADSTRING .  */
/*                                                               */
/*   ���������� ��������� �� ��������� �� ���������              */
/*       ������������� ��������.                                 */
/*                                                               */
/*   ��� ������:                                                 */
/*        SE_BADFORMAT  -  ������������ ������                   */
/*        SE_STRSIZE    -  ������� ������� ����������� ������    */
/*        SE_BADSTRING  -  ������ �� ��������� �����������       */
/*        SE_DATABUFF   -  ������� �������� ����� ������         */

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
  int  pref_flag ;  /* ���� �������� */
  int  pref_char ;  /* �������-������ */
 char  type_base ;  /* ������� ���: D - �����, F - ��������� */
 char  type_mode ;  /* ����������� ����: H - short, L - long/double */
 char  type_sign ;  /* ����������� �����: S - signed, U - unsigned */
  int  point ;      /* ��������� ����� */
  int  scan_flag ;  /* ���� ������ ���������� */
  int  size ;       /* ����� ����������� ������ */
 void *data ;       /* ����� ��������������� ������ */
  int  data_size ;  /* ����������� ������ ������ ������ */
  int  data_buff ;  /* ������������� ������ ������ ������ */
  int  type2 ;      /* ������� ��� ��������-'���������' */
 char *addr ;
 char *end ;        /* ���������� ���������� */
  int  sp_char ;    /* ������ ��� "�����������" �������� */
  int  len ;

	     int  d_int ;
  unsigned   int  d_intu ;
	    long  d_long ;
  unsigned  long  d_longu ;
	   short  d_short ;
  unsigned short  d_shortu ;
	   float  d_float ;
	  double  d_double ;

/*------------------------------------------------- ������� �������� */

  if(pars_cnt      <  3  ||
     pars[0]->addr==NULL ||
     pars[1]->addr==NULL ||
     pars[2]->addr==NULL   )  return(&void_return) ;

/*---------------------------------------------------- ������������� */

                   data     =NULL ;
                   data_size=  0 ;
                   pref_char=  0 ;

/*----------------------------------------------- ���������� ������� */

      if(pars[0]->size>=_BUFF_SIZE) {  dcl_errno=SE_BADFORMAT ;
					  return(&void_return) ;  }

		 addr=(char *)pars[0]->addr ;
/*- - - - - - - - - - - - - - - - - - - - - - ������ �������-������� */
       if(addr[0]!='%') {  pref_flag= 1 ;                           /* ���� ����� �������-������ -         */
			   pref_char=addr[0] ;                      /*  �������� ���, ���. ���� �������    */
				addr++ ;       }                    /*   �������-������� � �������� ������ */
       else                pref_flag= 0 ;
/*- - - - - - - - - - - - - - - - - - - - ���������� ������� ������� */
      memset(buff, 0, _BUFF_SIZE) ;
      memcpy(buff, addr, pars[0]->size-pref_flag) ;                 /* ������� ������ � ������� ������ */
      strupr(buff) ;                                                /* ��������� ������ � ������� ������� */

       if(buff[0]!='%') {  dcl_errno=SE_BADFORMAT ;                 /* ���� ����������� �������.������ */
			      return(&void_return) ;  }
/*- - - - - - - - - - - - - - - - - - - - - - -  ��������� ��������� */
		   type_base= 0 ;                                   /* ��������� ��������� */
		   type_mode='N' ;
		   type_sign='S' ;
		       point= 0 ;
		   scan_flag= 0 ;
/*- - - - - - - - - - - - - - - - - - - - - - - - ���������� ������� */
	  addr=buff+1 ;

      if(*addr=='U'              ) {  type_sign=addr[0] ;  addr++ ;  }
      if(*addr=='H' || *addr=='L') {  type_mode=addr[0] ;  addr++ ;  }
      if(*addr=='D' || *addr=='F') {  type_base=addr[0] ;  addr++ ;  }

      if(*addr=='.') {
			point=strtol(addr+1, &addr, 10) ;
		     }
      if(*addr=='>') {  scan_flag=1 ;  addr++ ;  }
/*- - - - - - - - - - - - - - - - - -  ������ ����������� ���������� */
      if( *addr   !=0 ||                                            /* ���� ������ �� ���������       */
	 type_base==0   ) {  dcl_errno=SE_BADFORMAT ;               /*  ���������� ��� � ��� �� ����� */
			      return(&void_return) ;  }             /*   ������� ��� ...              */

/*------------------------------------ ���������� ����������� ������ */

      if(pars[0]->size>=_BUFF_SIZE-1) {  dcl_errno=SE_STRSIZE ;
					   return(&void_return) ;  }

	    size=pars[1]->size ;                                    /* ������� ������ ������ */

      memset(buff, 0, _BUFF_SIZE) ;
      memcpy(buff, pars[1]->addr, size) ;                           /* ������� ������ � ������� ������ */

/*---------------------------------------- ��������� �������-������� */

    if(pref_flag) {

       if(buff[0]=='+' || buff[0]=='-') {  sp_char='0' ;            /* �����.������-������������ �   */
					      addr=buff+1 ;  }      /*   ������ ����������� �������� */
       else                             {  sp_char=' ' ;
					      addr=buff ;    }

	   for( ; *addr==pref_char ; addr++)  *addr=sp_char ;       /* ���������� ������� */
		  }
/*--------------------------------------- ��������� ���������� ����� */

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
/*--------------------------------------------------- ���������� INT */

  if(type_base=='D' && type_mode=='N') {

   if(type_sign=='U') {  d_intu=strtoul(buff, &end, 10) ;
			   data=&d_intu ;                 }
   else               {  d_int = strtol(buff, &end, 10) ;
			   data=&d_int  ;                 }

		     data_size=sizeof(int)  ;
				       }
/*------------------------------------------------- ���������� SHORT */

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
/*-------------------------------------------------- ���������� LONG */

  else
  if(type_base=='D' && type_mode=='L') {

   if(type_sign=='U') {  d_longu=strtoul(buff, &end, 10) ;
			    data=&d_longu ;                }
   else               {  d_long = strtol(buff, &end, 10) ;
			    data=&d_long ;                 }

		      data_size=sizeof(long)  ;
				       }
/*------------------------------------------------- ���������� FLOAT */

  else
  if(type_base=='F' && type_mode!='L') {

			d_float=(float)strtod(buff, &end) ;
			   data=&d_float ;
		      data_size=sizeof(float)  ;
				       }
/*------------------------------------------------ ���������� DOUBLE */

  else
  if(type_base=='F' && type_mode=='L') {

		       d_double=strtod(buff, &end) ;
			   data=&d_double ;
		      data_size=sizeof(double)  ;
				       }
/*------------------------------------------- ����������� ���������� */
  else                                 {
                                          dcl_errno=SE_BADFORMAT ;
                                           return(&void_return) ;
				       }
/*-------------------------------------------- ����� ������ �� ����� */

   if(!scan_flag && end!=buff+size) {  dcl_errno=SE_BADSTRING ;     /* �������� ������� ���������� */
					return(&void_return) ;  }

	  type2=t_base(pars[2]->type) ;                             /* ��������� ������� ��� */

			    data_buff =pars[2]->buff ;              /* ���.������������� ������ */
    if(type2==_DGT_VAL ||                                           /*  ������ ������ � ������  */
       type2==_DGT_PTR ||
       type2==_DGT_AREA  )  data_buff*=Kernel->iDgt_size(pars[2]->type) ;

   if(data_buff<data_size) {  dcl_errno=SE_DATABUFF ;               /* �������� ������ ������ */
				return(&void_return) ;  }

		      memcpy(pars[2]->addr, data, data_size) ;      /* �������� � ����� ������ */

    if(type2!=_DGT_VAL &&                                           /* ��� �������� ���������� */
       type2!=_DGT_PTR &&                                           /*    ���.������ �������   */
       type2!=_DGT_AREA  )  pars[2]->size=data_size ;

		      len =end-buff ;
	   chr_return.addr=(char *)pars[1]->addr+len ;
	   chr_return.buff=        pars[1]->buff-len ;
	   chr_return.size=        pars[1]->size-len ;

/*-------------------------------------------------------------------*/

		    Kernel->iSize_correct(pars[2]) ;                /* �������� �������� ���������     */
								    /*   �������� ���������� �� ������ */
          dcl_errno=0 ;

  return(&chr_return) ;
}


/*****************************************************************/
/*                                                               */
/*                 �������������� ������ � ������                */
/*                                                               */
/*           char *dtos(format, string, data) ;                  */
/*                                                               */
/*              char *format  - ������ ��������������            */
/*              char *string  - ����� ������                     */
/*              ...   data    - ������������� ������             */
/*                                                               */
/*  ��������� �������: [+][pref_chr]%[u][pref]type<area>[.point] */
/*                                                               */
/*        <area> - ����� '���� ������'                           */
/*          type - ������� ���:  d -  int/short/long             */
/*                               f -  float/double (F-������) ;  */
/*          pref - ������� ����: h -  short                      */
/*                               l -  long/double      ;         */
/*             u - ������������ unsigned ;                       */
/*         point - ����� ������ ����� ������� -                  */
/*                   ���� ������� ������������� �����, ��        */
/*                    ������� ���������� ;                       */
/*      pref_chr - ������� ������,                               */
/*                ��� ��� �����  ............444444              */
/*                           �  -............444444              */
/*                              ������� ������ ����� '.' ;       */
/*             + - ������� ��������� ����� ����� �������-        */
/*                     ��������.                                 */
/*                                                               */
/*   ���������� ��������� �� ���������� ����� ����������.        */
/*                                                               */
/*   ��� ������:                                                 */
/*        SE_BADFORMAT  -  ������������ ������                   */
/*        SE_STRSIZE    -  ������� �������� ����� ��� ������     */
/*        SE_BADSTRING  -  ������ �� ��������� � ���� ������     */
/*        SE_DATABUFF   -  ������ ������ ������ �� �����. ����   */

    Dcl_decl *Dcl_dtos(Lang_DCL *Kernel, Dcl_decl **pars, int  pars_cnt)

{
	 int  pref_flag ;  /* ���� �������� */
	 int  pref_char ;  /* �������-������ */
	 int  sign_flag ;  /* ���� ��������� ����� */
	char  type_base ;  /* ������� ���: D - �����, F - ���������(F-������) */
	char  type_mode ;  /* ����������� ����: H - short, L - long/double */
	char  type_sign ;  /* ����������� �����: S - signed, U - unsigned */
	 int  point ;      /* ��������� ����� */
	 int  point_flag ; /* ���� ������� ����������� '.' � ������� */
	 int  data_size ;  /* ����������� ������ ������ ������ */
	char *addr ;
	 int  area ;       /* ������ ���� ������ */
	 int  dec ;        /* ��������� ������� ����� FCVT */
	 int  sign ;       /* ��� ����� ����� FCVT */
 static char *sign_char="+-" ;
	 int  shift ;      /* ����� �������-���� */
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

/*------------------------------------------------- ������� �������� */

  if(pars_cnt      <  3  ||
     pars[0]->addr==NULL ||
     pars[1]->addr==NULL ||
     pars[2]->addr==NULL   )  return(&void_return) ;

/*---------------------------------------------------- ������������� */

                           len=0 ;
                          sign=0 ;
                     pref_char=0 ;

/*----------------------------------------------- ���������� ������� */

      if(pars[0]->size>=_BUFF_SIZE) {  dcl_errno=SE_BADFORMAT ;
					  return(&void_return) ;  }

		 addr=(char *)pars[0]->addr ;
/*- - - - - - - - - - - - - - - - - �������� ������� ��������� ����� */
       if(addr[0]=='+') {  sign_flag= 1 ;                           /* ���� ����� ����� ��������� ����� - */
				addr++ ;       }                    /*   ��������� ��� � �������� ������  */
       else                sign_flag= 0 ;
/*- - - - - - - - - - - - - - - - - - - - - - ������ �������-������� */
       if(addr[0]!='%') {  pref_flag= 1 ;                           /* ���� ����� �������-������ -         */
			   pref_char=addr[0] ;                      /*  �������� ���, ���. ���� �������    */
				addr++ ;       }                    /*   �������-������� � �������� ������ */
       else                pref_flag= 0 ;
/*- - - - - - - - - - - - - - - - - - - - ���������� ������� ������� */
      memset(buff, 0, _BUFF_SIZE) ;
      memcpy(buff, addr, pars[0]->size-pref_flag) ;                 /* ������� ������ � ������� ������ */
      strupr(buff) ;                                                /* ��������� ������ � ������� ������� */

       if(buff[0]!='%') {  dcl_errno=SE_BADFORMAT ;                 /* ���� ����������� �������.������ */
			      return(&void_return) ;  }
/*- - - - - - - - - - - - - - - - - - - - - - -  ��������� ��������� */
		   type_base= 0 ;                                   /* ��������� ��������� */
		   type_mode='N' ;
		   type_sign='S' ;
		       point= 0 ;
		  point_flag= 0 ;
/*- - - - - - - - - - - - - - - - - - - - - - - - ���������� ������� */
	  addr=buff+1 ;

      if(*addr=='U'              ) {  type_sign=addr[0] ;  addr++ ;  }
      if(*addr=='H' || *addr=='L') {  type_mode=addr[0] ;  addr++ ;  }
      if(*addr=='D' || *addr=='F') {  type_base=addr[0] ;  addr++ ;  }

			 area=strtol(addr, &addr, 10) ;

      if(*addr=='.') {
			point_flag= 1 ;
			point     =strtol(addr+1, &addr, 10) ;
		     }
/*- - - - - - - - - - - - - - - - - -  ������ ����������� ���������� */
      if( *addr   !=0 ||                                            /* ���� ������ �� ���������       */
	 type_base==0   ) {  dcl_errno=SE_BADFORMAT ;               /*  ���������� ��� � ��� �� ����� */
			      return(&void_return) ;  }             /*   ������� ��� ...              */

      if(     area < 1 ||                                           /* ���� �� ������ ���� ������ */
	 abs(point)>20   ) {  dcl_errno=SE_BADFORMAT ;              /*  ��� ������ ������� �����  */
			       return(&void_return) ;  }            /*   ���� ����� ������� ...   */

/*----------------------------------------------- ���������� ������� */

	   a_int   =(           int *)pars[2]->addr ;
	   a_intu  =(unsigned   int *)pars[2]->addr ;
	   a_long  =(          long *)pars[2]->addr ;
	   a_longu =(unsigned  long *)pars[2]->addr ;
	   a_short =(         short *)pars[2]->addr ;
	   a_shortu=(unsigned short *)pars[2]->addr ;
	   a_float =(         float *)pars[2]->addr ;
	   a_double=(        double *)pars[2]->addr ;

/*--------------------------------- �������������� INT, SHORT � LONG */

  if(type_base=='D') {                                              /* IF.1 */

	    if(type_mode=='N') {    d_long =*a_int ;                /* ��������� ���������� �� ������ */
				    d_longu=*a_intu ;               /*   ������ � ������� ������ �    */
				  data_size=sizeof(int) ;  }        /*    ������������ � ����� ������ */
       else if(type_mode=='H') {    d_long =*a_short ;
				    d_longu=*a_shortu ;
				  data_size=sizeof(short) ;  }
       else                    {    d_long =*a_long ;
				    d_longu=*a_longu ;
				  data_size=sizeof(long) ;  }

	 if(type_sign=='U')  sprintf(buff, "%lu", d_longu) ;        /* �������� �������� ��� ����������� */
	 else                sprintf(buff, "%ld", d_long) ;         /*   ��������������� � ������        */

       if(sign_flag)                                                /* ���� ������ ��������� ����� -  */
	 if(buff[0]=='-') {  strcpy(buff, buff+1) ;                 /*  ���.��� ����� � �������       */
			       sign=1 ;             }               /*   ����� � �������������� ����� */
	 else                  sign=0 ;

			  len=strlen(buff) ;
		     }                                              /* END.1 */
/*------------------------- �������������� FLOAT � DOUBLE � ������ F */

  else
  if(type_base=='F') {                                              /* IF.2 */

	    if(type_mode=='N') {   d_double=*a_float ;              /* ��������� ���������� �� ������ */
				  data_size=sizeof(float) ;  }      /*   ������ � ������� ������ �    */
	    else               {   d_double=*a_double ;             /*    ������������ � ����� ������ */
				  data_size=sizeof(double) ;  }

			  len=abs(point) ;                          /* ���.����� '����� �������' */

		addr=fcvt(d_double, len, &dec, &sign) ;             /* ��������� ����������� */
		   strcpy(buff, addr) ;                             /* �������� ��������� � ������� ����� */
	       len=strlen(buff) ;                                   /* ���.����� ������ */

	if(dec<1) {                                                 /* ���� ����� <1, ��           */
		     memmove(buff-dec+1, buff, len+1) ;             /*  ��������� ����������� ���� */
		      memset(buff, '0', -dec+1) ;                   /*   �������                   */
			 len+=(-dec+1) ;
			 dec = 1 ;
		  }

	if(point_flag && point>=0) {                                /* ���� ���� ������� ����� - */
		       memmove(buff+dec+1, buff+dec, len-dec+1) ;   /*  ��������� ��             */
				    buff[dec]='.' ;
					 len++ ;
				   }

	if(sign && !sign_flag) {                                    /* ���� ����� ������������� */
		     memmove(buff+1, buff, len+1) ;                 /*  � ���� �� ��������� -   */
				     buff[0]='-' ;                  /*   ��������� ���� �����   */
				      len++ ;
			       }
		     }                                              /* END.2 */
/*--------------------------------------- ����������� �������������� */

  else               {
                           dcl_errno=SE_BADFORMAT ;
                              return(&void_return) ;
                     }
/*----------------------------------- �������� ������� ������ ������ */

// if(data_size>pars[2]->size) {  dcl_errno=SE_DATABUFF ;           /* �������� ������� ������ ������ ... */
//                                  return(&void_return) ;  }

/*---------------------------------------- ��������� �������-������� */

      if(len+sign*sign_flag>area) {   dcl_errno=SE_BADSTRING ;      /* ���� ������ �� ��������� */
				       return(&void_return) ;  }    /*   � ���� ������ ...      */

    if(pref_flag) {                                                 /* ���� ������ ����������  */
			shift=area-len-sign_flag ;                  /*   �������-�������� ...  */
		      memmove(buff+shift, buff, len+1) ;
		       memset(buff, pref_char, shift) ;
			 len+=shift ;
		  }
/*-------------------------------------------------- ��������� ����� */

    if(sign_flag) {
		       memmove(buff+1, buff, len+1) ;
			       buff[0]=sign_char[sign] ;
				len++ ;
		  }
/*-------------------------------------------- ����� ������ �� ����� */

   if(pars[1]->buff<len) {  dcl_errno=SE_STRSIZE ;                  /* �������� ������� ��������� ������ */
			     return(&void_return) ;  }

	    memcpy(pars[1]->addr, buff, len) ;
		   pars[1]->size=len ;

		    Kernel->iSize_correct(pars[1]) ;                /* �������� �������� ���������     */
								    /*   �������� ���������� �� ������ */
	   chr_return.addr= buff ;
	   chr_return.buff=_BUFF_SIZE ;
	   chr_return.size= len ;

/*-------------------------------------------------------------------*/

          dcl_errno=0 ;

  return(&chr_return) ;
}


/*****************************************************************/
/*                                                               */
/*             �������������� ������ ������ ��������             */
/*                                                               */
/*           void  dtod(format, data_in, data_out) ;             */
/*                                                               */
/*              char *format   - ������ ��������������           */
/*              ...   data_in  - ������������� ������            */
/*              ...   data_out - ��������������� ������          */
/*                                                               */
/*   ��������� �������: var_in%var_out ,                         */
/*         ���  var_in � var_out - �������� ������� � ��������   */
/*                                   ����������.                 */
/*    �������� �������� ����������: [u][pref]type                */
/*                                                               */
/*          type - ������� ���:   d -  int/short/long            */
/*                                f -  float/double   ;          */
/*          pref - ������s ����:  h -  short                     */
/*                                l -  long/double ;             */
/*             u - ������������ unsigned.                        */
/*                                                               */
/*   ��� ������:                                                 */
/*        SE_BADFORMAT  -  ������������ ������                   */
/*        SE_DATABUFF   -  ������ ������ ������ �� �����. ����   */

    Dcl_decl *Dcl_dtod(Lang_DCL *Kernel, Dcl_decl **pars, int  pars_cnt)

{
	char  type_base ;  /* ������� ���: D - �����, F - ���������(F-������) */
	char  type_mode ;  /* ����������� ����: H - short, L - long/double */
	char  type_sign ;  /* ����������� �����: S - signed, U - unsigned */
	 int  data_size ;  /* ����������� ������ ������ ������ */
	char *addr ;

    union Dcl_types_addr  a_type ;
		  double  d_double ;

/*------------------------------------------------- ������� �������� */

  if(pars_cnt      <  3  ||
     pars[0]->addr==NULL ||
     pars[1]->addr==NULL ||
     pars[2]->addr==NULL   )  return(&void_return) ;

/*--------------------------- ���������� �������: ������� ���������� */

      if(pars[0]->size>=_BUFF_SIZE) {  dcl_errno=SE_BADFORMAT ;
					  return(&void_return) ;  }

	memset(buff, 0, _BUFF_SIZE) ;
	memcpy(buff, pars[0]->addr, pars[0]->size) ;                /* ������� ������ � ������� ������ */
	strupr(buff) ;                                              /* ��������� ������ � ������� ������� */

	  addr=buff ;
/*- - - - - - - - - - - - - - - - - - - - - - -  ��������� ��������� */
		   type_base= 0 ;
		   type_mode='N' ;
		   type_sign='S' ;
/*- - - - - - - - - - - - - - - - - - - - - - - - ���������� ������� */
      if(*addr=='U'              ) {  type_sign=addr[0] ;  addr++ ;  }
      if(*addr=='H' || *addr=='L') {  type_mode=addr[0] ;  addr++ ;  }
      if(*addr=='D' || *addr=='F') {  type_base=addr[0] ;  addr++ ;  }
/*- - - - - - - - - - - - - - - - - -  ������ ����������� ���������� */
       if(*addr!='%') {  dcl_errno=SE_BADFORMAT ;                   /* ���� �� ��������� ����������� ... */
			  return(&void_return) ;  }

      if(type_base==0) {  dcl_errno=SE_BADFORMAT ;                  /* ���� �� ����� ������� ��� ... */
			      return(&void_return) ;  }

/*---------------------------------- ������� �������������� � DOUBLE */

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

// if(data_size>pars[1]->size) {  dcl_errno=SE_DATABUFF ;           /* �������� ������� ������ ������ ... */
//                                  return(&void_return) ;  }

/*-------------------------- ���������� �������: �������� ���������� */

/*- - - - - - - - - - - - - - - - - - - - - - -  ��������� ��������� */
		   type_base= 0 ;
		   type_mode='N' ;
		   type_sign='S' ;
/*- - - - - - - - - - - - - - - - - - - - - - - - ���������� ������� */
      if(*addr=='U'              ) {  type_sign=addr[0] ;  addr++ ;  }
      if(*addr=='H' || *addr=='L') {  type_mode=addr[0] ;  addr++ ;  }
      if(*addr=='D' || *addr=='F') {  type_base=addr[0] ;  addr++ ;  }
/*- - - - - - - - - - - - - - - - - -  ������ ����������� ���������� */
       if(*addr!= 0 ) {  dcl_errno=SE_BADFORMAT ;                   /* ���� ������ ���������� �� ���� ... */
			  return(&void_return) ;  }

      if(type_base==0) {  dcl_errno=SE_BADFORMAT ;                  /* ���� �� ����� ������� ��� ... */
			      return(&void_return) ;  }

/*-------------------------------- �������� �������������� �� DOUBLE */

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

// if(data_size>pars[2]->buff) {  dcl_errno=SE_DATABUFF ;           /* �������� ������� ������ ������ ... */
//                                  return(&void_return) ;  }

/*----------------------------------------------- ��������� �������� */

				     pars[2]->size=data_size ;      /* ���.�������� ������ */
               Kernel->iSize_correct(pars[2]) ;                     /* �������� �������� ���������     */
								    /*   �������� ���������� �� ������ */
/*-------------------------------------------------------------------*/

           dcl_errno=0 ;

  return(&void_return) ;
}


/*****************************************************************/
/*                                                               */
/*    �������������� ������ � ������ � ����������� � �����       */
/*                                                               */
/*           char *�tos(data) ;                                  */
/*                                                               */
/*              ...   data    - ������������� ������             */
/*                                                               */
/*    ��� ��������������:                                        */
/*                                                               */
/*   ���������� ��������� �� ���������� ����� ����������.        */

    Dcl_decl *Dcl_atos(Lang_DCL *Kernel, Dcl_decl **pars, int  pars_cnt)

{
     int  type_base ;

/*------------------------------------------------- ������� �������� */

  if(pars_cnt      <  1  ||
     pars[0]->addr==NULL   )  return(&void_return) ;

/*----------------------------------------------- ���������� ������� */

          type_base=t_base(pars[0]->type) ;

/*----------------------------------- �������������� ��������� ����� */

  if(type_base==_DGT_VAL) {
                               Kernel->iNumToStr(pars[0], buff) ;
                          } 
/*-------------------------------------------- ����� ������ �� ����� */

	   chr_return.addr= buff ;
	   chr_return.buff=_BUFF_SIZE ;
	   chr_return.size= strlen(buff) ;

/*-------------------------------------------------------------------*/

  return(&chr_return) ;
}


/*****************************************************************/
/*                                                               */
/*    �������������� ����������� ������� � ������ ����/�����     */
/*                                                               */
/*           char *�tot(data) ;                                  */
/*                                                               */
/*              ...   data    - ����� � �������� � 01.01.1970    */
/*                                                               */
/*    ��� ��������������:                                        */
/*                                                               */
/*   ���������� ��������� �� ���������� ����� ����������.        */

    Dcl_decl *Dcl_atot(Lang_DCL *Kernel, Dcl_decl **pars, int  pars_cnt)

{
        int  type_base ;
     time_t  time_abs ;
  struct tm *hhmmss ;

/*------------------------------------------------- ������� �������� */

  if(pars_cnt      <  1  ||
     pars[0]->addr==NULL   )  return(&void_return) ;

/*----------------------------------------------- ���������� ������� */

          type_base=t_base(pars[0]->type) ;

/*----------------------------------- �������������� ��������� ����� */

  if(type_base==_DGT_VAL) {

     time_abs=(time_t)Kernel->iDgt_get(pars[0]->addr, pars[0]->type) ;

                 hhmmss=localtime(&time_abs) ;

           sprintf(buff, "%02d:%02d:%02d %02d.%02d.%04d",
                            hhmmss->tm_hour, hhmmss->tm_min,   hhmmss->tm_sec,
                            hhmmss->tm_mday, hhmmss->tm_mon+1, hhmmss->tm_year+1900) ;
                          } 
/*-------------------------------------------- ����� ������ �� ����� */

	   chr_return.addr= buff ;
	   chr_return.buff=_BUFF_SIZE ;
	   chr_return.size= strlen(buff) ;

/*-------------------------------------------------------------------*/

  return(&chr_return) ;
}


/*****************************************************************/
/*                                                               */
/*                   ������ �������� ������� � ����              */
/*                                                               */
/*            void  time(data) ;                                 */
/*                                                               */
/*              char *data    - �������� ������                  */
/*                                                               */
/*  ������ ������� ����� � ���� � ������� HH24:MI:SS DD.MM.YYYY  */

    Dcl_decl *Dcl_time(Lang_DCL *Kernel, Dcl_decl **pars, int  pars_cnt)

{
     time_t  time_abs ;
  struct tm *hhmmss ;
       char  data[32] ;   /* ����� ����������� ������ */
        int  size ;

 static  unsigned long  time_value ;
 static       Dcl_decl  time_return={_DCL_ULONG, 0,0,0,"", &time_value, NULL, 1, 1} ;

/*------------------------------------------------- ������� �������� */

  if(pars_cnt      <  1  ||
     pars[0]->addr==NULL   )  return(&void_return) ;

/*---------------------------------------------- ������������ ������ */

               time_abs=     time( NULL) ;
                 hhmmss=localtime(&time_abs) ;

          sprintf(data, "%02d:%02d:%02d %02d.%02d.%04d",
                           hhmmss->tm_hour, hhmmss->tm_min,   hhmmss->tm_sec,
                           hhmmss->tm_mday, hhmmss->tm_mon+1, hhmmss->tm_year+1900) ;

/*----------------------------------------------- ��������� �������� */

                              size=strlen(data) ;                   /* ������������ ������ ���������� ������ */
      if(size>pars[0]->buff)  size=pars[0]->buff ;

                    memcpy(pars[0]->addr, data, size) ;             /* �������� ������ */

                           pars[0]->size=size ;                     /* ���.�������� ������ */
     Kernel->iSize_correct(pars[0]) ;                               /* �������� �������� ��������� �������� ���������� �� ������ */

/*-------------------------------------------------------------------*/

          time_value=time_abs ;
  return(&time_return) ;
}


/*****************************************************************/
/*                                                               */
/*                   ������ ��������� �����                      */
/*                                                               */
/*      unsigned long  elapsed_time(clear) ;                     */
/*                                                               */
/*              double clear  - ������� ������ �������           */

    Dcl_decl *Dcl_elapsed_time(Lang_DCL *Kernel, Dcl_decl **pars, int  pars_cnt)

{
 static         time_t  time_0 ;
 static  unsigned long  time_value ;
 static       Dcl_decl  time_return={_DCL_ULONG, 0,0,0,"", &time_value, NULL, 1, 1} ;
                   int  clear ;


     clear=(int)Kernel->iDgt_get(pars[0]->addr, pars[0]->type) ;    /* '���������' ������� ������ ������� */


   if(time_0==0)  time_0    =time(NULL) ;
                  time_value=time(NULL)-time_0 ;

   if(clear    )  time_0    =time(NULL) ;

  return(&time_return) ;
}


/*****************************************************************/
/*                                                               */
/*            �������� ��������� � �������� ��������             */
/*                                                               */
/*           char *trim(string[, symbols]) ;                     */
/*                                                               */
/*              char *string  - �������������� ������            */
/*              char *symbols - �������� ��������� ��������      */
/*                               (�� ��������� - ������)         */
/*                                                               */
/*   ���������� ��������� �� ���������.                          */

    Dcl_decl *Dcl_trim(Lang_DCL *Kernel, Dcl_decl **pars, int  pars_cnt)

{
	char *trim_list ;      /* �������� ��������� �������� */
         int  trim_cnt ;  
	char *addr ;
	 int  size ;

/*------------------------------------------------- ������� �������� */

       if(pars_cnt      <  1  ||
          pars[0]->addr==NULL   )  return(&void_return) ;

       if(pars[0]->size==  0    )  return(&void_return) ;

       if(pars_cnt     < 2  ||                                      /* '���������' ������ ��������� �������� */
	  pars[1]->addr==NULL ) {  trim_list=            " " ;
                                   trim_cnt =             1 ;         }
       else                     {  trim_list=(char *)pars[1]->addr ;
                                   trim_cnt =        pars[1]->size ;  }

       if(trim_list==NULL || trim_cnt==0)  return(&void_return) ;

/*---------------------------------------------- ���������� �������� */

       addr=(char *)pars[0]->addr ;
       size=        pars[0]->size ;

     for( ; size>0 ; size--)
       if(memchr(trim_list, addr[size-1], trim_cnt)==NULL)  break ;

     for( ; size>0 ; addr++, size--)
       if(memchr(trim_list, *addr, trim_cnt)==NULL)  break ;

/*------------------------------------------------ ������ ���������� */

      if(size>0 && addr!=pars[0]->addr)
          memmove(pars[0]->addr, addr, size) ;

                 pars[0]->size=size ;

                Kernel->iSize_correct(pars[0]) ;                    /* �������� �������� ���������     */
								    /*   �������� ���������� �� ������ */
/*-------------------------------------------------------------------*/

          chr_return.addr=pars[0]->addr ;
          chr_return.size=pars[0]->size ;
          chr_return.buff=pars[0]->buff ;

  return(&chr_return) ;
}


/*****************************************************************/
/*                                                               */
/*            ������ ����������� ��������� �� ������             */
/*                                                               */
/*           void  replace(string, text_1, text_2) ;             */
/*                                                               */
/*              char *string  - �������������� ������            */
/*              char *text_1  - ���������� ��������              */
/*              char *text_2  - ���������� ��������              */

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

/*------------------------------------------------- ������� �������� */

  if(pars_cnt      <  3  ||
     pars[0]->addr==NULL ||
     pars[1]->addr==NULL ||
     pars[2]->addr==NULL   )  return(&void_return) ;

  if(pars[0]->size==  0  ||
     pars[1]->size==  0    )  return(&void_return) ;

/*------------------------------------------------------- ���������� */

       addr  =(char *)pars[0]->addr ;
       size  =        pars[0]->size ;
       buff  =        pars[0]->buff ;
       text_1=(char *)pars[1]->addr ;
       size_1=        pars[1]->size ;
       text_2=(char *)pars[2]->addr ;
       size_2=        pars[2]->size ;

/*------------------------------------------------ ���������� ������ */

     for(i=0 ; i<size-size_1+1 && i<buff ; i++)
       if(addr[i]==*text_1)
        if(        size_1==1              || 
           !memcmp(addr+i, text_1, size_1)  ) {

         if(size_1!=size_2) {
            if(i+size_2>buff) {                                     /* ��� ������ �� ������� ������ */
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
/*------------------------------------------------ ������ ���������� */

      if(size>0 && addr!=pars[0]->addr)
          memmove(pars[0]->addr, addr, size) ;

                 pars[0]->size=size ;

                Kernel->iSize_correct(pars[0]) ;                    /* �������� �������� ���������     */
								    /*   �������� ���������� �� ������ */
/*-------------------------------------------------------------------*/

  return(&void_return) ;
}


/*****************************************************************/
/*                                                               */
/*            ������ �������� �� ������ �� ��������              */
/*                                                               */
/*           void  replace_char(string, text_1, text_2) ;        */
/*                                                               */
/*              char *string  - �������������� ������            */
/*              char *text_1  - ����� ��������                   */
/*              char *text_2  - ���������� ��������              */

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

/*------------------------------------------------- ������� �������� */

  if(pars_cnt      <  3  ||
     pars[0]->addr==NULL ||
     pars[1]->addr==NULL ||
     pars[2]->addr==NULL   )  return(&void_return) ;

  if(pars[0]->size==  0  ||
     pars[1]->size==  0    )  return(&void_return) ;

/*------------------------------------------------------- ���������� */

       addr  =(char *)pars[0]->addr ;
       size  =        pars[0]->size ;
       buff  =        pars[0]->buff ;
       text_1=(char *)pars[1]->addr ;
       size_1=        pars[1]->size ;
       text_2=(char *)pars[2]->addr ;
       size_2=        pars[2]->size ;

/*------------------------------------------------ ���������� ������ */

     for(i=0 ; i<size && i<buff ; i++)
       if(memchr(text_1, addr[i], size_1)) {

            if(i+size_2>buff) {                                     /* ��� ������ �� ������� ������ */
                 memmove(addr+i, text_2, i+size_2-buff) ;
                                 size=buff ;
                                    break ;
                              }

                memmove(addr+i+size_2, addr+i+1, size-i-1) ;
                        size+=size_2-1 ;

         if(size_2>0)  memmove(addr+i, text_2, size_2) ;

                      i+=size_2-1 ;
                                           }
/*------------------------------------------------ ������ ���������� */

      if(size>0 && addr!=pars[0]->addr)
          memmove(pars[0]->addr, addr, size) ;

                 pars[0]->size=size ;

                Kernel->iSize_correct(pars[0]) ;                    /* �������� �������� ���������     */
								    /*   �������� ���������� �� ������ */
/*-------------------------------------------------------------------*/

  return(&void_return) ;
}


/*********************************************************************/
/*                                                                   */
/*         �������������� ��������� �����                            */
/*                                                                   */
/*    int  transpose(text_1, text_2, format_1, format_2, option)     */
/*                                                                   */
/*      char *text_1    -  �������� �����                            */
/*      char *text_2    -  ��������������� �����                     */
/*      char *format_1  -  ������ ��������� ������                   */
/*      char *format_2  -  ������ ���������������� ������            */
/*      char *options   -  ����� ��������������                      */
/*                                                                   */
/*   ����� ��������������:                                           */
/*      FIELD:      - ��������� ��� ������� - ������ � �����         */
/*                     ��������� ���� (��-��������� - {} )           */
/*      ERROR_RAISE - ��������� �� ��������� ������ �� �������       */
/*                     ��������������                                */
/*                                                                   */
/*   ��������� ����� �������� ��������� ������:                      */
/*     {Name}    - �� ����������� ��������                           */
/*     {Name:20} - �� �����                                          */
/*                                                                   */
/*   ��������� ����� �������� ���������������� ������:               */
/*     {Name}    - ������� �������                                   */
/*                                                                   */
/*   Return:   1 - ��������� ��������������                          */
/*             0 - ����� �� ������������� �������                    */
/*            -1 - ������ ���������                                  */

    Dcl_decl *Dcl_transpose(Lang_DCL *Kernel, Dcl_decl **pars, int  pars_cnt)

{

#define  _FIELDS_MAX  20
    Dcl_decl  fields[_FIELDS_MAX] ;
         int  fields_cnt ;
         int  size ;

/*------------------------------------------------- ������� �������� */

  if(pars_cnt      <  5  ||
     pars[0]->addr==NULL ||
     pars[1]->addr==NULL ||
     pars[2]->addr==NULL ||
     pars[3]->addr==NULL ||
     pars[4]->addr==NULL   ) {         dgt_value=0 ;
                               return(&dgt_return) ;  }

/*------------------------------------------ ������ ��������� ������ */

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
/*----------------------------- ������������ ���������������� ������ */

     size=Kernel->zFormByTemplate((char *)pars[1]->addr, pars[1]->buff,
                                  (char *)pars[3]->addr, pars[3]->size,
                                  (char *)pars[4]->addr, pars[4]->size, 
                                                  fields, fields_cnt     ) ;
  if(size<0) {
                          dgt_value=size ;
                  return(&dgt_return) ;
             }
/*------------------------------------------------ ������ ���������� */

                 pars[1]->size=size ;

                Kernel->iSize_correct(pars[1]) ;                    /* �������� �������� ���������     */
								    /*   �������� ���������� �� ������ */
/*-------------------------------------------------------------------*/

          dgt_value=1 ;
  return(&dgt_return) ;
}


/*****************************************************************/
/*                                                               */
/*            ������� �������� � ������� �������                 */
/*                                                               */
/*           void  upper(string, codepage) ;                     */
/*                                                               */
/*              char *string   - �������������� ������           */
/*              char *codepage - ������������ ���������          */

    Dcl_decl *Dcl_upper(Lang_DCL *Kernel, Dcl_decl **pars, int  pars_cnt)

{
           char  codepage[32] ;
  unsigned char *coding ;
  unsigned char *addr ;
            int  size ; 
            int  i ; 

/*------------------------------------------------- ������� �������� */

  if(pars_cnt      <  2  ||
     pars[0]->addr==NULL   )  return(&void_return) ;

  if(pars[1]->size==  0    )  return(&void_return) ;

/*-------------------------------- ����������� ������� ������������� */

                                size=pars[1]->size ;
    if(size>=sizeof(codepage))  size=sizeof(codepage)-1 ;

	  memset(codepage, 0, sizeof(codepage)) ;
	  memcpy(codepage, pars[1]->addr, size) ;                   /* ��������� ��������� */

        
         if(!stricmp(codepage, "BASIC"  ))  coding=dcl_char_upper_128 ;
    else if(!stricmp(codepage, "WINDOWS"))  coding=dcl_char_upper_cp1251 ;
    else if(!stricmp(codepage, "CP1251" ))  coding=dcl_char_upper_cp1251 ;
    else                                    coding=dcl_char_basic ;

/*---------------------------------------- ���������� �������������� */

       addr  =(unsigned char *)pars[0]->addr ;
       size  =                 pars[0]->size ;

     for(i=0 ; i<size ; i++)  addr[i]=coding[addr[i]] ;

/*-------------------------------------------------------------------*/

  return(&void_return) ;
}


/*****************************************************************/
/*                                                               */
/*            ������������� ������� �������� ������              */
/*                                                               */
/*       void  cp_convert(string, codepage1, codepage2) ;        */
/*                                                               */
/*              char *string    - �������������� ������          */
/*              char *codepage1 - �������� ���������             */
/*              char *codepage2 - �������������� ���������       */

    Dcl_decl *Dcl_cp_convert(Lang_DCL *Kernel, Dcl_decl **pars, int  pars_cnt)

{
           char  codepage1[32] ;
           char  codepage2[32] ;
           char *addr ;
            int  size ; 
            int  cnt ; 

/*------------------------------------------------- ������� �������� */

  if(pars_cnt      <  3  ||
     pars[0]->addr==NULL   )  return(&void_return) ;

  if(pars[1]->size==  0    )  return(&void_return) ;
  if(pars[2]->size==  0    )  return(&void_return) ;

/*-------------------------------- ����������� ������� ������������� */

                                 size=pars[1]->size ;
    if(size>=sizeof(codepage1))  size=sizeof(codepage1)-1 ;

	  memset(codepage1, 0, sizeof(codepage1)) ;
	  memcpy(codepage1, pars[1]->addr, size) ;                   /* ��������� �������� ��������� */

                                 size=pars[2]->size ;
    if(size>=sizeof(codepage2))  size=sizeof(codepage2)-1 ;

	  memset(codepage2, 0, sizeof(codepage2)) ;
	  memcpy(codepage2, pars[2]->addr, size) ;                   /* ��������� �������������� ��������� */
        
/*---------------------------------------- ���������� �������������� */

                               addr=(char *)pars[0]->addr ;
                               size=        pars[0]->size ;

      cnt=Kernel->zCodePageConvert(addr, size, codepage1, codepage2) ;
   if(cnt!=size) {
                                      pars[0]->size=cnt ;
                Kernel->iSize_correct(pars[0]) ;                    /* �������� �������� ���������     */
								    /*   �������� ���������� �� ������ */
                 }
/*-------------------------------------------------------------------*/

  return(&void_return) ;
}


/*****************************************************************/
/*                                                               */
/*                 ���������� SHELL-�������                      */
/*                                                               */
/*        double  system(command) ;                              */
/*                                                               */
/*              char *command - ����������� �������              */
/*                                                               */
/*     ����� �/� ��������� ������:  system(command)              */
/*                                                               */
/*   ����������:  0  - ��� ���������                             */
/*               -1  - ������ ������� ��.errno                   */
/*               >0  - ��� �������� ��� ����������               */

    Dcl_decl *Dcl_system(Lang_DCL *Kernel, Dcl_decl **pars, int  pars_cnt)

{
	char  command[2048] ;
	 int  size ;
	 int  status ;
         int  i ;
         int  j ;

/*------------------------------------------------- ������� �������� */

  if(pars_cnt      <  1  ||
     pars[0]->addr==NULL   ) {
                                        dcl_errno=ENOENT ;
                                        dgt_value=-1. ;
                                return(&dgt_return) ;
                             }
/*------------------------------------------------------- ���������� */

                                 size=pars[0]->size ;
      if(size>=sizeof(command))  size=sizeof(command)-1 ;

           memset(command, 0, sizeof(command)) ;
           memcpy(command, pars[0]->addr, size) ;

   for(i=0, j=0 ; command[i]!=0 ; i++) {                            /* ��������� ����-�������� - */
                                                                    /*  - ������� ����� ������   */
       if(command[i]=='\r')  continue ;
       if(command[i]=='\n')  continue ;
       if(command[i]=='\t')  command[i]=' ' ;

          command[j]=command[i] ;
                  j++ ;
                                       }
/*------------------------------------------------------- ���������� */


              status=system(command) ;
           dgt_value=status ;

           if(status<0)  dcl_errno=errno ;
           else          dcl_errno=  0. ;

/*-------------------------------------------------------------------*/

  return(&dgt_return) ;
}


/*****************************************************************/
/*                                                               */
/*                ��������� �����                                */
/*                                                               */
/*            void  sleep(pause) ;                               */
/*                                                               */
/*              double  pause  - ����� � ��������                */

    Dcl_decl *Dcl_sleep(Lang_DCL *Kernel, Dcl_decl **pars, int  pars_cnt)

{
  int  pause ;


     pause=(int)Kernel->iDgt_get(pars[0]->addr, pars[0]->type) ;    /* '���������' ������������ ����� */

#ifdef UNIX
                sleep(pause) ;
#else
                Sleep(pause*1000) ;
#endif

  return(NULL) ;
}


/*****************************************************************/
/*                                                               */
/*                  ������ ������ � ���-����                     */
/*                                                               */
/*        double  log_file(path, text) ;                         */
/*                                                               */
/*              char *path - ���� � ����� ����                   */
/*              char *text - ������������ ������                 */
/*                                                               */
/*   ����������:  0  - ��� ���������                             */
/*               -1  - ������ ������� ��.errno                   */

    Dcl_decl *Dcl_log_file(Lang_DCL *Kernel, Dcl_decl **pars, int  pars_cnt)

{
       time_t  time_abs ;
    struct tm *hhmmss ;
         char  path[1024] ;
         FILE *file ;
         char  prefix[1024] ;
          int  size ;

/*------------------------------------------------- ������� �������� */

                                        dgt_value=0 ;

  if(pars_cnt      <  1  ||
     pars[0]->addr==NULL   ) {
                                        dcl_errno=ENOENT ;
                                        dgt_value=-1. ;
                                return(&dgt_return) ;
                             }
/*------------------------------------- ������������ ��������� ����� */

                time_abs=     time( NULL) ;
                  hhmmss=localtime(&time_abs) ;
        
        sprintf(prefix, "%02d/%02d/%02d %02d:%02d:%02d ",
                             hhmmss->tm_mday,
                             hhmmss->tm_mon+1,
                             hhmmss->tm_year-100,
                             hhmmss->tm_hour,
                             hhmmss->tm_min,    
                             hhmmss->tm_sec ) ;

/*---------------------------------------------- �������� ����� ���� */

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
/*------------------------------------------------- ������ ��������� */

        fwrite(prefix, 1, strlen(prefix), file) ;

   if(pars[1]->addr!=NULL && 
      pars[1]->size!=  0    ) 
        fwrite(pars[1]->addr, 1, pars[1]->size, file) ;

        fwrite("\n", 1, strlen("\n"), file) ;

/*---------------------------------------------- �������� ����� ���� */

         fclose(file) ;	

/*-------------------------------------------------------------------*/

  return(&dgt_return) ;
}


/*****************************************************************/
/*                                                               */
/*              ������ ������ � ��������� ��� �������            */
/*                                                               */
/*        double  log_event(journal, source, category, text) ;   */
/*                                                               */
/*     char *journal  - ��� ������� �������                      */
/*     char *source   - ��� ��������� �������                    */
/*     char *category - ��������� �������: ERROR, WARNING, INFO  */
/*     char *text     - ����� ���������                          */
/*                                                               */
/*   ����������:  0  - ��� ���������                             */
/*               -1  - ������ ������� ��.errno                   */

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
          int  log_created ;        /* ������� �������� ������� */
#endif

/*------------------------------------------------- ������� �������� */

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
/*-------------------------------------------- ���������� ���������� */

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
/*------------------------------------------------------ ��� Windows */

#ifndef  UNIX
/*- - - - - - - - - - - - - - - - - - - - - - - - - ����������� ���� */
            log_created=0 ;

  do {                                                              /* BLOCK */
/*- - - - - - - - - - - - - - - - - - - -  ������ ��������� � ������ */
     do {

            if(!stricmp(category, "WARNING"))  type=EVENTLOG_WARNING_TYPE ;
       else if(!stricmp(category, "INFO"   ))  type=EVENTLOG_INFORMATION_TYPE ;
       else                                    type=EVENTLOG_ERROR_TYPE ;

          hSrc=RegisterEventSource(NULL, source) ;                  /* "���������" ������ */
       if(hSrc==NULL) {                                             /* ���� ������ �������� ������� -                     */
        if(log_created) {                                           /*  ���� ���� �� �������� �������,                    */
                                  dcl_errno =GetLastError() ;       /*  ���� (���� �� ��� ��� �������) ������� � �������  */
                                  dgt_value = -1. ;
                          return(&dgt_return) ;
                        }
        else                  break ;
                      }

                        text_ptr=text ;

        status=ReportEvent(hSrc, type, 0, 0, NULL, 1, strlen(text), /* ����� ������� */
                                   (const char **)&text_ptr, text  ) ;

         DeregisterEventSource(hSrc);                               /* "���������" ������ */
    
      if(status==0) {                                               /* ���� ������ ������ �������... */
                              dcl_errno =GetLastError() ;
                              dgt_value = -1. ;
                    }

                      return(&dgt_return) ;
        } while(0) ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - �������� ������� */
   do {                                                             /* BLOCK */
                                    hKey=NULL ;

        sprintf(key_path, "SYSTEM\\CurrentControlSet\\Services\\EventLog\\%s", journal) ;

       result=RegOpenKeyEx(HKEY_LOCAL_MACHINE, key_path,            /* ������� ������� ������������ ������ */
                              0, KEY_SET_VALUE, &hKey)  ;

    if(result!=ERROR_SUCCESS) {

         result=RegCreateKeyEx(HKEY_LOCAL_MACHINE, key_path,        /* ����������� ������� */
                                0, NULL, REG_OPTION_NON_VOLATILE,
                                 KEY_WRITE, NULL, &hKey, &status )  ;
      if(result!=ERROR_SUCCESS)  break ;

         result=RegSetValueEx(hKey, "Sources",                      /* ������� �������� ���������� */
                               0, REG_MULTI_SZ,             
                              (LPBYTE)source, (DWORD) strlen(source)+1) ;

      if(result!=ERROR_SUCCESS)  break ;
                              }

                  RegCloseKey(hKey) ;                               /* ��������� ������ */

        sprintf(key_path, "SYSTEM\\CurrentControlSet\\Services\\EventLog\\%s\\%s", journal, source) ;

       result=RegOpenKeyEx(HKEY_LOCAL_MACHINE, key_path,            /* ������� ������� ������������ �������� */
                              0, KEY_SET_VALUE, &hKey)  ;
    if(result!=ERROR_SUCCESS) {
 
         result=RegCreateKeyEx(HKEY_LOCAL_MACHINE, key_path,        /* ������� �������� � ������� ������� */
                                0, NULL, REG_OPTION_NON_VOLATILE,
                                 KEY_SET_VALUE, NULL, &hKey, &status )  ;
      if(result!=ERROR_SUCCESS)  break ;
         result = 0 ;

         status = EVENTLOG_ERROR_TYPE      |                          
                  EVENTLOG_WARNING_TYPE    |
                  EVENTLOG_INFORMATION_TYPE ; 
         result|=RegSetValueEx(hKey, "TypesSupported",              /* ����������� �������������� ���� ��������� */  
                                 0, REG_DWORD,         
                                 (LPBYTE)&status, sizeof(DWORD)) ;
                              }

                RegCloseKey(hKey) ;                                 /* ��������� �������� */

      } while(0) ;                                                  /* BLOCK */

    if(result!=ERROR_SUCCESS) {                                     /* ���� ��� ��������/�������� ������� ���� ������ */
                                   dcl_errno =result ;
                                   dgt_value = -1. ;
                           return(&dgt_return) ;
                              }

                            log_created=1 ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - ����������� ���� */
     } while(1) ;                                                   /* BLOCK */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
#endif

/*-------------------------------------------------------------------*/

  return(&dgt_return) ;
}


/*****************************************************************/
/*                                                               */
/*                  ������ ������ � ���-����                     */
/*                                                               */
/*        double  signal(type, address, status, info) ;          */
/*                                                               */
/*          char *type     -  ��� �������: ABTP                  */
/*          char *address  -  �������                            */
/*          char *signal   -  ��� �������                        */
/*          char *value    -  �������� �������                   */
/*          char *info     -  ���������� �� �������              */
/*                                                               */
/*   ����������:  0  - ��� ���������                             */
/*               -1  - ������ ��.errno                           */

    Dcl_decl *Dcl_signal(Lang_DCL *Kernel, Dcl_decl **pars, int  pars_cnt)

{
         char  address[FILENAME_MAX] ;   /*  */
         char  signal[128] ; 
         char  value[128] ; 
         char *info ; 
          int  size ;                    /* ������ ���������� */

   static   double  ret_value ;          /* ����� ��������� �������� */
   static Dcl_decl  dgt_return={_DGT_VAL, 0, 0, 0, "", &ret_value, NULL, 1, 1} ;

/*------------------------------------------------- ������� �������� */

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
/*-------------------------------------------- ���������� ���������� */

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

/*-------------------------------------------- �������� ABTP-������� */

    if(!memcmp(pars[0]->addr, "ABTP", pars[0]->size) && 
               pars[0]->size==strlen("ABTP")           ) {

       if(signal[0]==0)  strcpy(signal, "NULL") ;

            ret_value=Kernel->zAbtpSignal(address, signal, value, info, size) ;
                                                         }
/*------------------------------------------ ����������� ��� ������� */
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
/*                    ���������� md5-���� ������                 */
/*                                                               */
/*  unsigned char *md5(string[, notation]) ;                     */
/*                                                               */
/*     char *string  - �������������� ������                     */
/*     char *notation - ������ ������������� ����                */
/*          ��������� ��������:                                  */
/*          "16" - � ����������������� ������� ���������;        */
/*          "2"  - � �������� ������� ���������;                 */
/*
/*   ���������� ��������� �� ���������.                          */


Dcl_decl *Dcl_md5(Lang_DCL *Kernel, Dcl_decl **pars, int  pars_cnt)
{
	md5_processor proc;
	int size_digest;
	unsigned char *digest;
	unsigned char *digest_format;
	unsigned int size_input;
	unsigned char *input;
	int flag;


/*------------------------------------------------- ������� �������� */

	if(	pars_cnt < 1 || pars[0]->addr==NULL	)  
		return( &void_return ) ;

/*------------------------------------------------- ������������� ��������� ���������� */

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
	
/*------------------------------------------------- ����������� ������ */

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
/*                    ���������� md5-���� ������ �� �����        */
/*                                                               */
/*  unsigned char *f_md5(path[, notation]) ;                     */
/*                                                               */
/*     char *path  - ������ ���� � ����                          */
/*     char *notation - ������ ������������� ����                */
/*          ��������� ��������:                                  */
/*          "16" - � ����������������� ������� ���������;        */
/*          "2"  - � �������� ������� ���������;                 */
/*
/*   ���������� ��������� �� ���������.                          */

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

/*------------------------------------------------- ������� �������� */

	if(	pars_cnt < 1 || pars[0]->addr==NULL	)
		return( &void_return ) ;

/*------------------------------------------------- ������������� ��������� ���������� */

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

/*------------------------------------------------- �������� ����� ��� ������ */

	if( ( input_file = fopen( path, "rb" ) ) == NULL )
            return( &void_return );

/*------------------------------------------------- ����������� ������ �� ����� */

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


