/*********************************************************************/
/*                                                                   */
/*                   DATA CONVERTION LANGUAGE                        */
/*                                                                   */
/*                 ���������� ���������� �������                     */
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

/*------------------------------------------------ "�������" ������� */

  Dcl_decl *Dcl_message  (Lang_DCL *, Dcl_decl **, int) ;    /* ������ ��������� �� ����� */
  Dcl_decl *Dcl_varsts   (Lang_DCL *, Dcl_decl **, int) ;    /* ������ �������� ���������� */
  Dcl_decl *Dcl_varsts_  (Lang_DCL *, Dcl_decl **, int) ;    /* ������ �������� ���������� (� ������) */
  Dcl_decl *Dcl_varshow  (Lang_DCL *, Dcl_decl **, int) ;    /* ������ �������� ���������� */
  Dcl_decl *Dcl_varshow_ (Lang_DCL *, Dcl_decl **, int) ;    /* ������ �������� ���������� (� ������) */
  Dcl_decl *Dcl_timestamp(Lang_DCL *, Dcl_decl **, int) ;    /* ������ ������ ����/������� */

/*------------------------------------------------ �������� �������� */

/*--------------------------------------------------- ������� ������ */

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

/*----------------------------------------------- ������� ���������� */

#define   DCL_DEBUG  (Kernel->mDebug_path[0]!=0)


/*****************************************************************/
/*                                                               */
/*                 ������ ��������� �� �����                     */
/*                                                               */
/*        void  message(text) ;                                  */
/*                                                               */
/*          char *text  - ��������� �����                        */
/*                                                               */
/*   ������� ����� �� �����, ����� ���� ��������� ������.        */

    Dcl_decl *Dcl_message(Lang_DCL *Kernel, Dcl_decl **pars, int  pars_cnt)

{
   char  buff[1024] ;
    int  i ;

    if(pars[0]->size>=sizeof(buff))                                 /* �������� �������� ��������� */
                  pars[0]->size=sizeof(buff)-2 ;                  

	 memcpy(buff, pars[0]->addr, pars[0]->size) ;               /* ��������� ��������� � ������� ����� */

	       i   =pars[0]->size ;
	  buff[i  ]='\n' ;                                          /* ����������� ��������� */
	  buff[i+1]='\0' ;

          CharToOem(buff, buff) ;
	     printf(buff) ;

  return(NULL) ;
}


/*****************************************************************/
/*                                                               */
/*                 ������ ���������� ����������                  */
/*                                                               */
/*        void  variable (var) ;                                 */
/*        void  variable_(var) ; ( � ������ )                    */
/*                                                               */
/*   ������� �� ����� ��������� ���������� � ������� :           */
/*           ��� ��������:                                       */
/*  NAME TYPE ADDR (SIZE_B/BUFF_B) (SIZE/BUFF) VALUE1 VALUE2 ... */
/*           ��� ��������:                                       */
/*  NAME TYPE ADDR (SIZE_B/BUFF_B) "STRING"                      */
/*                                                               */
/*     ���  NAME    - ��� ���������� (����� �������)             */
/*          TYPE    - ��� ����������                             */
/*          ADDR    - ����� ������                               */
/*          SIZE    - ������ ������� � �������� �������          */
/*          BUFF    - ������ ������ ������� � �������� �������   */
/*          SIZE_B  - ������ ������� � ������                    */
/*          BUFF_B  - ������ ������ ������� � ������             */
/*         VALUE    - �������� �������� (�������� - ��� �������) */
/*        STRING    - ���������� ������                          */

    Dcl_decl *Dcl_varsts(Lang_DCL *Kernel, Dcl_decl **pars, int  pars_cnt)

{
	  int  type ;            /* ������� ��� ���������� */
	  int  mode ;            /* ���������� ��� ���������� */
       double  value ;           /* �������� ��������� ���������� */
	  int  size ;            /* ������ ���������� */
	  int  buff ;            /* ������ ������ ���������� */
	  int  elem ;            /* ������ '������' ���������� */
	 char *addr ;            /* ����� ������ ���������� */
	 char  save ;            /* ����� ������ ���������� */
	  int  save_idx ;
         char  tmp[1024] ;
	  int  i ;
  static char *types[]={            /* ��������� ������� ����� */
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

  static char *modes[]={            /* ��������� ���������� ����� */
			     "unknown",
			     "double",
			     "short",
			     "u_short",
			     "long",
			     "u_long",
			     "float"
		       } ;

/*------------------------------------------------------ ����� ����� */

       if(pars[0]->name[0]==0)                                      /* ���.��� ��� ��������� ������ */
                     strcpy(pars[0]->name, "Work Variable") ;

        type= t_base(pars[0]->type) ;                               /* ��������� ������� ��� */

     if(type==_XTP_OBJ) {
                              mode=t_mode(pars[0]->type)>>8 ;       /*  ��������� ���������� ��� */
                        }
     else               {
                           if(type<0 || type>9 ||                   /*  �������� �������� ���� */
	                      types[type]==NULL  )  type=0 ;

                              mode=(t_mode(pars[0]->type)>>8)+1 ;   /*  ��������� ���������� ��� */
                           if(mode<1 || mode>6)  mode=0 ;           /*  �������� ����������� ���� */
                        }

                               addr =(char *)pars[0]->addr ;
		               size =        pars[0]->size ;
		               buff =        pars[0]->buff ;

/*---------------------------------- ����������� ���������� �������� */

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
/*---------------------------------- ����������� �������� ���������� */

   else
   if(type==_DGT_VAL  ||                                        /* �����������            */
      type==_DGT_AREA ||                                        /*    �������� ���������� */
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
	   for(i=0 ; i<pars[0]->size ; i++) {                       /* ������� �������� ���������� */
		   value=Kernel->iDgt_get(addr+i*elem, pars[0]->type) ;
	       sprintf(tmp, "%lf ", value) ;
            if(DCL_DEBUG)  Kernel->iLog("VARIABLE>", tmp) ;
            else                 printf("%s",        tmp) ;
					    }
                          }
/*-------------------------------- ����������� ���������� ���������� */

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
                              if(size<buff)  save_idx=size  ;       /* ������������ ������� �� ����� */
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
/*                 ������ �������� ����������                    */
/*                                                               */
/*        void  show (var1, var2, ...) ;                         */
/*        void  show_(var1, var2, ...) ;  ( � ������ )           */
/*                                                               */
/*   ������� �� ����� �������� ���������� - ����� ��� ������.    */

    Dcl_decl *Dcl_varshow(Lang_DCL *Kernel, Dcl_decl **pars, int  pars_cnt)

{
	  int  type ;            /* ������� ��� ���������� */
	  int  elem ;            /* ������ '������' ���������� */
       double  value ;           /* �������� ��������� ���������� */
	 char *addr ;            /* ����� ������ ���������� */
	 char  save ;            /* ����� ������ ���������� */
	  int  size ;
         char  tmp[128] ;
	  int  i ;
	  int  j ;

/*----------------------------------------------- ���� �� ���������� */

    for(i=0 ; i<pars_cnt ; i++) {                                   /* CIRCLE.1 - ����. ������� ���������� */

		      addr=(char *)pars[i]->addr ;
		      size=        pars[i]->size ;
		      type= t_base(pars[i]->type) ;                 /* ��������� ������� ��� */

/*----------------------------------------- ������ ������������ ���� */
           if(type==_XTP_OBJ    ) {

//             for(i=0 ; i<size ; i++) 

                                  }
/*------------------------------------------------------------- NULL */

      else if(addr==NULL        ) {

            if(DCL_DEBUG)  Kernel->iLog("SHOW>", "NULL-ptr") ;
            else                 printf(         "NULL-ptr") ;

                                  }  
/*------------------------------------------------- �������� ������� */

      else if(type==_DGT_VAL  ||
	      type==_DGT_AREA ||
	      type==_DGT_PTR    ) {                                 /* ����������� �������� ���������� */

				elem=Kernel->iDgt_size(pars[i]->type) ;

	   for(j=0 ; j<size ; j++) {
		   value=Kernel->iDgt_get(addr+j*elem, pars[i]->type) ;
                                sprintf(tmp, "%lf ", value) ;
            if(DCL_DEBUG)  Kernel->iLog("SHOW>", tmp) ;
            else                 printf("%s",    tmp) ;
				   }

				  }
/*----------------------------------------------- ���������� ������� */

      else                        {
                                            save =addr[size] ;
                                       addr[size]= 0 ;
                                     
            if(DCL_DEBUG)  Kernel->iLog("SHOW>", addr) ;
            else                 printf("%s",    addr) ;

                                       addr[size]=save ;
				  }
/*----------------------------------------------- ���� �� ���������� */
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
/*                   ������ �� ����� ������ ����/�������         */
/*                                                               */
/*            void  timestamp(void) ;                            */
/*                                                               */
/*      ������ �� ����� ������� ����� � ����,                    */
/*   � ����� ����� ��������� ����� ����������� ������.           */

    Dcl_decl *Dcl_timestamp(Lang_DCL *Kernel, Dcl_decl **pars, int  pars_cnt)

{
  static time_t  time_prv ;   /* ����� ����������� ������ */
	 time_t  time_crn ;   /* ����� �������� ������ */
	 time_t  time_d  ;    /* ������� ������ */
	   char *string ;     /* ������� �����-���� � ���� ������ */
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


