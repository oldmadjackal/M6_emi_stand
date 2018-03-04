/*   Bolotov P.D.  version  29.04.1994   */
/*     0 warnings at warning level 1     */

/*********************************************************************/
/*                                                                   */
/*                   DATA CONVERTION LANGUAGE                        */
/*                                                                   */
/*                        �������������                              */
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


#pragma warning(disable : 4996)

/*--------------------------- �������� ������� �������� � ���������� */

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

/*--------------------------------------------------- ������ ������� */

//           int   Lang_DCL::nPass_sts ;      /* ������ ������� */

#define                    _ACTIVE_PASS   0    /* �������� ������ */
#define                      _ELSE_PASS   1    /* �������� ������ ��� ELSE */
#define                   _PASSIVE_PASS   2    /* ��������� ������ */
#define                     _BREAK_PASS   3    /* ��������� ����� �� ����� */

/*-------------------------------------------------------------------*/

//#define  _DEFINE_MAX    100
//   struct Dcl_define      dcl_define[_DEFINE_MAX] ;   /* ������ DEFINE-�� */
// int      dcl_def_cnt ;               /* ����� DEFINE-�� */

/*------------------------------------------------------ ����������� */

//#define DGT_SZ     sizeof(double)
#define CHR_SZ     sizeof(char)

/*-------------------------------------------------------------------*/


/************************************************************************/
/*                                                                      */
/*        ������������� ����� DCL (Data Convertion Language)            */
/*                                                                      */
/*   control  -   ����������� �������:                                  */
/*                    _DCL_MEMORY - ��������� � ������                  */
/*                    _DCL_FILE   - ��������� � �����                   */
/*                    _DCL_INIT   - ����� � �������                     */
/*     input  -   ��� ���� ��� ����� � ����������                       */
/*      vars  -   ������� ������� ����������                            */

    int  Lang_DCL::Interpritator(     int  control, 
                                     char *input, 
                                 Dcl_decl **vars   )
{
	 char  work[_BUFF_SIZE+2] ;  /* ������� ����� */
	  int  work_cnt ;            /* ������� �������� ������ ����� */
	 char *mark ;                /* ����� ������� ������ */
	 char *mark_target ;         /* ������� ����� */
	  int  mark_flag ;           /* ���� ������� � ������ �����-���� */
	  int  string_flag ;         /* ���� ���������� ������ ���������� ������ */
	 long  byte_prv ;            /* ������ ���������� �������� ������ ����� */
	  int  oper_type ;           /* ��� ��������� */
	 char  chr_save ;            /* ������ ���������� ������� */
	 char *for_addr[4] ;         /* ������ �������� FOR-��������� */
	  int  block_exp_flag ;      /* ���� �������� ����� */
	  int  any_flag ;            /* ���� �� IF-ELSE � �� FOR ��������� */
	  int  close_flag ;          /* ���� �������� IF-ELSE � FOR ���������� */
	  int  ret_flag ;            /* ���� ��������� ��������� RETURN */
	  int  end_flag ;            /* ���� ����� ����� */
	  int  else_flag ;           /* ���� ������� �� ELSE ��� ENDIF ��������� */

	 char *text ;
	 char *tmp ;
	 char *addr ;
         char  msg[128] ;
	  int  size ;
	  int  len ;
	  int  i ;
	  int  j ;
	  int  n ;

/*----------------------------------- ���������� ����������� ������� */

				     mFile_flag= 0 ;                /* ���.����� ������: ���� ��� ������ */
	    if(control & _DCL_FILE)  mFile_flag= 1 ;

                    if(mFile_flag)  mError_file=mProgram_name ;
                                    mError_code= 0 ;                /* ����� ���������� ������ */

                                    mError_details[0]=0 ;

	   strncpy(mProgram_name, input, sizeof(mProgram_name)-1) ; /* ��������� ������������� ��������� */

/*--------------------------------------- ��������� ������ � ������� */

  if(control==_DCL_INIT) {                                          /* ���� ������ ������������� ... */
			    iMem_free() ;                           /*   �������� ������� ���������� ������� */
				return(0) ;
			 }
/*----------------------------------------- �������� ����a ��������� */

  if(mFile_flag) {
                      mFile_hdl=open(input, O_BINARY | O_RDWR) ;    /* �������� ����� */
                   if(mFile_hdl==-1) {                              /* ���� ������... */
                                       mError_code=_DCLE_FILE_OPEN ;
                                             return(-1) ;
                                     }
	         }
  else                mFile_mem=input ;

/*---------------------------------------------------- ������������� */

  if(nInit_flag==0) {                                               /* ��� ������ � �������. 1-�� ������ */
                          iCall_init() ;                            /*   ����.������� DCL-������� */
                      if(mError_code)  return(-1) ;

                           nLocal=1 ;
                    }

     nInit_flag=1 ;

/*-------------------------------- ���������� ������ �������� ������ */

          iCall_proc_list(nCall_stack[nCall_stack_ptr]) ;
       if(mError_code)  return(-1) ;

/*-------------------------------- ������������� � ���������� ������ */

		getcwd(nCur_path, sizeof(nCur_path)) ;              /* ������ �������� ������� */

//	    dcl_def_cnt=0 ;                                         /* ����� �������� DEFINE-�� */

	   nVars=iMem_init(vars) ;                                  /* ����. ������� ���������� ������� */
	if(mError_code)  return(0) ;                                /* �����.�������� ������ */

       memcpy(&nReturn, &nVoid, sizeof(nResult)) ;                  /* ����� ��������� �������� */
								    /*    � ��������� VOID      */
	if(nRet_data!=NULL)  free(nRet_data) ;                      /* ����������� ������� RETURN-����� */

/*-------------------------------------------------- �������� ������ */
/*-------------------------------------------------------------------*/

		       mRow=nSeg_row ;                              /* ����. ������ ������ */
		  mByte_cnt=nSeg_start ;                            /* ����. �������� ������ */
		mark_target= NULL ;                                 /* ����� ������ �� ����� */
		 nNext_flag=  0 ;                                   /* ����� ����� ���������� ������� ����� */
		nId_stk_cnt=  0 ;                                   /* ����� IF-FOR ����� */
                  nPass_sts=_ACTIVE_PASS ;                          /* �������� � ��������� ������� */
	     block_exp_flag=  0 ;                                   /* ����� ����� �������� ����� */
		 nOper_next=  0 ;

		   end_flag=0 ;                                     /* ����� ����� ����� ����� */
		  else_flag=0 ;
		   ret_flag=0 ;
		mError_code=0 ;

  do {                                                              /* CIRCLE.1 - ������������ �� ������� */
		      mRow++ ;                                      /* �������. ������� ����� */

#ifndef UNIX

       if(_heapchk()!=_HEAPOK) {
                                  printf("--- MEMORY CRASH\n") ;
                               }
#endif
/*-------------------------------------- ���������� ��������� ������ */

                  memset(work, 0, sizeof(work)) ;
	 work_cnt=iSysin(work, mByte_cnt, _BUFF_SIZE) ;

      if(work_cnt==0)  {  end_flag=1 ;  break ;  }                  /* ���� ������ ��� - ���.���� ����� ����� */

	 tmp=(char *)memchr(work, '\n', work_cnt) ;                 /* ���� ����� ������ <��> */

      if(tmp==NULL)                                                 /* ���� ����� ������ �� ������... */
	if(work_cnt==_BUFF_SIZE) {  mError_code=_DCLE_STRING_LEN ;  /*   ���� ����� ����. ����� - ������      */
						  break ;         } /*     �����.�� ������ � �����.���������� */
	else              tmp=work+work_cnt ;                       /*   ���� ����� �� ���������� - ������� */
								    /*      ������ ������ ��������� ������  */
		    *tmp=0 ;                                        /* �����.���������� ������ */
	      memset(tmp, 0, _BUFF_SIZE+work-tmp-2) ;               /* ��������� ����� ������ ������ */

	       byte_prv =mByte_cnt  ;                               /* ���������� �������� ������ */
	      mByte_cnt+=tmp-work+1 ;                               /* �����.������� ���� �� ����. ������ */

	       tmp=strchr(work, '\r') ;                             /* ��������� ���������    */
	    if(tmp!=NULL)  *tmp='\0' ;                              /*   ���������� <RET><NL> */

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
/*----------------- ������ �������� � ���������, ������� ����������� */

	     string_flag=0 ;

     for(i=0, j=0 ; work[i] ; i++, j++) {                           /* CIRCLE.2 - ��������� ������ ... */
/*- - - - - - - - - - - - - - - - - - - ����������� ���������� ����� */
      if(work[i]=='"')  if(string_flag) {                           /* ������������� � ��������� */
			  if(work[i-1]!='\\' ||                     /*   �������� � ��������     */
			     work[i-2]=='\\'   )  string_flag=0 ;   /*    ���������� ������      */
					}
                        else                      string_flag=1 ;

      if(string_flag) {  work[j]=work[i] ;  continue ;  }           /* ������ ����.������ */
/*- - - - - - - - - - - - - - - - - - - - - -  ��������� ����������� */
	 if(work[i  ]=='/' &&
	    work[i+1]=='/'   )  break ;
/*- - - - - - - - - - - - - - - - - - -  ������ �������� � ��������� */
       if(work[i]== ' ' ||                                          /* ��� ����������� �������       */
	  work[i]=='\t'   )      j-- ;                              /*  ��� ��������� - ��������� �� */
       else                 work[j]=work[i] ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
					}                           /* CONTINUE.2 */

         if(string_flag) {                                          /* ���� ������ �� �������... */
                            mError_code=_DCLE_NOCLOSE_STRING ;
                                break ;
                         }

		      work[j]=0 ;                                   /* ����������� ������ */

	 if(work[0]==0)  continue ;

/*-------------------------------------------------- ��������� ����� */
  
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
/*- - - - - - - - - - - - - - - - - - - - - - - - ����������� ������ */
       else                                            {
		    mError_code=_DCLE_PRAGMA_UNK ;
			   break ;
                                                       } 
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                        continue ;
                                                     }
/*-------------------------------------------------- ��������� ����� */

     if(mark_target!=NULL)  nPass_sts=_PASSIVE_PASS ;               /* ����� ����� ��� GOTO - ������� */
								    /*   ������� � ��������� ������   */

	mark_flag=  0 ;                                             /* ����� ���������� �����-���� � ������ */
	mark     =work ;
	addr     =iFind_close(mark, ':') ;                          /* ���� ����������� ����� */

     if(addr!=NULL) {                                               /* IF.1 - ���� ������������� ����� ... */
		       *addr= 0 ;                                   /* ����������� ����� */
			 len=addr-mark ;                            /* �����.����� ����� */

	for(i=0 ; i<len; i++)  if(!NAME_CHAR(mark[i])) break ;      /* �����. ������ �������� ����� */

      if(i==len) {                                                  /* IF.2 - ���� ��������� ����� ����. ... */
       if(        mark_target!=NULL &&                              /* ���� ����� ����� �� ����� */
	  !strcmp(mark_target, mark)  ) {                           /*   � ����� ��������� ...   */
					   free(mark_target) ;      /*  �����. ������ ������� ����� */
					mark_target=NULL ;          /*  ���������� ����� ������ �� ����� */
					mark_flag  =  1 ;           /*  ����.������� �����-���� � ������ */
					  nPass_sts=_ACTIVE_PASS ;  /*  ���.�������� ������ */
					}

		       strcpy(work, work+len+1) ;                   /* �������� ������ �� ����� */
		 }                                                  /* ELSE.2 */
      else       {                                                  /* ELSE.2 - ���� ��������� ������. ... */
		    mError_code=_DCLE_MARK_SIMBOL ;
			   break ;
		 }                                                  /* END.2 */
		    }                                               /* ELSE.1 */
     else           {                                               /* ELSE.1 */
//                     if(mark_target!=NULL)  continue ;            /* ���� ����� ����� �� ����� ... */
		    }                                               /* END.1 */
/*----------------------------------- ���������� ������ �� ��������� */

	    nSubrow_cnt=1 ;                                         /* ����. ������� �������� */

    for(addr=work ; ; addr++) {                                     /* ��������� ������ � ������� */
								    /*   ����������� ����������   */
			addr=iFind_close(addr, ';') ;               /*    � ��� ��� �����������   */
								    /*   ��������� 0-����������   */
		     if(addr!=NULL) {   nSubrow_cnt++ ;
					      *addr=0 ;  }
		     else              break ;
			      }
/*----------------------------------- ���������� ������ �� ��������� */

	     nOper_cnt= 0 ;                                         /* ���������� ������� �������� */
		  text=work ;

    for(text=work ; nSubrow_cnt>0 ; text++, nSubrow_cnt--) {        /* CIRCLE.3 - ������.������ �� ���������� */

     do {                                                           /* CIRCLE.4 - ��������� ��������� */
	  if(nOper_cnt) nOper[nOper_cnt-1].end=text ;               /* ��������� ������ � ����� ��������� */
		        nOper[nOper_cnt  ].beg=text ;
/*- - - - - - - - - - - - - - - - - -  ��������� �������� ���������� */
         if( iDecl_detect(text) ) {
			nOper[nOper_cnt].type =_DECLARE_ ;
				     break ;
  				  }
/*- - - - - - - - - - - - - - - - - - - - - ��������� �������� ����� */
    else if(text[0]==_OPEN_BLOCK) {
				    nOper[nOper_cnt].type=_OPEN_ ;
					     text++ ;
				  }
/*- - - - - - - - - - - - - - - - - - - - - ��������� �������� ����� */
    else if(text[0]==_CLOSE_BLOCK) {
				     nOper[nOper_cnt].type=_CLOSE_ ;
					     text++ ;
				   }
/*- - - - - - - - - - - - - - - - - - - - - ��������� IF - ��������� */
    else if(!memcmp(text, _IF_LEX, _IF_SIZE)) {
		      addr=iFind_close(text+_IF_SIZE, ')') ;        /* ����� ����������� ������ ��������� */
		   if(addr==NULL) {  mError_code=_DCLE_IF_CLOSE ;   /* ���� ������� ��� - ������ */
					     break ;             }
			 nOper[nOper_cnt].type=_IF_ ;
					  text=addr+1 ;
					      }
/*- - - - - - - - - - - - - - - - - - - - ��������� ELSE - ��������� */
    else if(!memcmp(text, _ELSE_LEX, _ELSE_SIZE)) {
			 nOper[nOper_cnt].type =_ELSE_ ;
					  text+=_ELSE_SIZE ;
						  }
/*- - - - - - - - - - - - - - - - - - - -  ��������� FOR - ��������� */
    else if(!memcmp(text, _CIRCLE_LEX, _CIRCLE_SIZE)) {
		   addr=iFind_close(text+_CIRCLE_SIZE, ')') ;       /* ����� ����������� ������ ��������� */
		if(addr==NULL) {  mError_code=_DCLE_FOR_CLOSE ;     /* ���� ������� ��� - ������ */
					  break ;                }
				 nOper[nOper_cnt].type=_CIRCLE_ ;
						  text=addr+1 ;
						      }
/*- - - - - - - - - - - - - - - - - - - ��������� RETURN - ��������� */
    else if(!memcmp(text, _RETURN_LEX, _RETURN_SIZE)) {
			nOper[nOper_cnt].type =_RETURN_ ;

	     if(text[_RETURN_SIZE]=='(') {                          /* ���� RETURN � ���������� ... */
		   addr=iFind_close(text+_RETURN_SIZE+1, ')') ;     /*   ����� ����������� ������ ��������� */
		if(addr==NULL) {  mError_code=_DCLE_RETURN_CLOSE ;  /*   ���� ������� ��� - ������ */
					  break ;                 }
					    text=addr+1 ;
					 }
	else if(text[_RETURN_SIZE]== 0 )    text+=_RETURN_SIZE ;    /* ���� RETURN ��� ��������� ... */
	else                                  break ;               /* ���� ��� �� RETURN ... */
						      }
/*- - - - - - - - - - - - - - - - - - -  ��������� BREAK - ��������� */
    else if(!strcmp(text, _BREAK_LEX)) {
			  nOper[nOper_cnt].type =_BREAK_ ;
					   text+=_BREAK_SIZE ;
				       }
/*- - - - - - - - - - - - - - - - - - ��������� CONTINUE - ��������� */
    else if(!strcmp(text, _CONTINUE_LEX)) {
			     nOper[nOper_cnt].type =_CONTINUE_ ;
					      text+=_CONTINUE_SIZE ;
					  }
/*- - - - - - - - - - - - - - - - - - - - ��������� GOTO - ��������� */
    else if(!memcmp(text, _GOTO_LEX, _GOTO_SIZE)) {
			nOper[nOper_cnt].type =_GOTO_ ;
				   break ;
						  }
/*- - - - - - - - - - - - - - - - - -  ��������� �������� ���������� */
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
	 if(*text==0)  break ;                                      /* ���� ��������� ����������� - */
								    /*   - ��������� � ���������    */
	      nOper_cnt++ ;                                         /* ��������� �������� */

	} while(1) ;                                                /* CONTINUE.4 */

		  if(mError_code)  break ;

		      text=text+strlen(text) ;                      /* ���. ��������� �� ����� ������ */
      nOper[nOper_cnt].end=text+strlen(text) ;                      /* ���.����� ����������            */
	    nOper_cnt++ ;                                           /*  ���������� ��������� ��������� */

							   }        /* CONTINUE.3 */

//               oper_cnt++ ;

/*-------------------------------------------------- ���� ���������� */

  for(nOper_crn=0 ; nOper_crn<nOper_cnt ; nOper_crn++) {            /* CIRCLE.5 - ����. ����� �� ���������� */

		   oper_type= nOper[nOper_crn].type ;               /* ���. ��� �������� ��������� */
			text= nOper[nOper_crn].beg ;                /* ��������� ��������� �� ���������    */
	 if(nOper_crn) *text= chr_save ;                            /*  �������� ��������� ������ � ������. */
		    chr_save=*nOper[nOper_crn].end ;                /*   ��� ��������������� '����� ����'   */

       if(nNext_flag && nOper_crn<nOper_next)  continue ;           /* ������ '������' ����������    */
								    /*    ������ ����� � NEXT-������ */
	 *nOper[nOper_crn].end=  0 ;                                /* ����������� ��������� */

	    if(*text==0)  break ;                                   /* ������������ ������ ��������� */

	 if(block_exp_flag)  block_exp_flag-- ;                     /* '���������' ����� �������� ����� */

		  close_flag= 0 ;                                   /* ����.������ �������         */
		    any_flag= 0 ;                                   /*    IF-ELSE � FOR ���������� */

/*-------------------------------------- ������������� ������� ����� */

		    iNext_title(_CLEAR_WORK_VAR) ;                  /* ����� �������� ������� ����� */
		     iAlloc_var(_CLEAR_WORK_VAR, NULL, 0) ;         /* ������� ������� ������� */

/*----------------------------------------- ��������� ��������� ELSE */

    if(oper_type==_ELSE_) {                                         /* IF.7 - ���� �����.�������� ELSE ... */

      if(!else_flag) {  mError_code=_DCLE_FREE_ELSE ;               /* ��� "�����������" ��������� - */
			           break ;             }            /*   ���.������ � �������.�����. */

	      nId_stk[nId_stk_cnt].else_flag =1 ;                   /* ������������ ������ � IF_DO ����� */
	      nId_stk[nId_stk_cnt].block_flag=0 ;

	     if(nPass_sts!=_BREAK_PASS)
		  nPass_sts=nId_stk[nId_stk_cnt].pass_sts ;         /* ����.������ ������� ��� IF */

	     if(nPass_sts==_ACTIVE_PASS)  nPass_sts=  _ELSE_PASS ;  /* �����. ������ ������� ��� ELSE */
	else if(nPass_sts==  _ELSE_PASS)  nPass_sts=_ACTIVE_PASS ;

		nId_stk[nId_stk_cnt].pass_sts=nPass_sts ;           /* ������� ������ ������� */

				  else_flag=0 ;                     /* ���������� ELSE-���� */
			     block_exp_flag=2 ;                     /* ���.���� �������� ����� */
			  }                                         /* END.7 */
/*---------------------------------- ��������� "��������������" ELSE */

   if(else_flag) {

      for( ; nId_stk_cnt>0 ; nId_stk_cnt--) {                       /* ������. ��� "����������" IF, ELSE � DO */
	  if(nId_stk[nId_stk_cnt].block_flag) break ;               /*  ������������� ���� � �������������    */
	          iIef_close(nId_stk_cnt) ;                         /*   ���������� �������� �����            */

	      if(nNext_flag)  break ;
					    }

		    if(mark_flag)  nPass_sts=_ACTIVE_PASS ;         /* �����.������������ ��������     */
								    /*  IF-��������� ��� GOTO �������� */
		 }
		      else_flag=0 ;
		      mark_flag=0 ;

/*------------------------------------------ ��������� ��������� FOR */

    if(oper_type==_CIRCLE_) {                                       /* IF.5 - ���� �������� FOR ... */
/*- - - - - - - - - - - - - - ������������ ������ ����������� ������ */
		text+=_CIRCLE_SIZE ;
	  for_addr[0]= text ;
	  for_addr[3]= nOper[nOper_crn].end-1 ;
	 *for_addr[3]= 0 ;
/*- - - - - - - - - - - - - - -  ����� �������� � "��������" ������� */
      for(i=2 ; i>0 ; i--) {                                        /* ���� ����� "��������",          */
	  for_addr[i]=strrchr(text, ';') ;                          /*  ��� �� ���������� -            */
       if(for_addr[i]==NULL) {  mError_code=_DCLE_FOR_TTL_SEL ;     /*   ���.������ � �������.�����.,  */
					break ;                 }   /* � ��� ����������� - ����������� */
       else                  {  *for_addr[i]=0 ;                    /*  ������ � ������ �����          */
				 for_addr[i]++ ;           }
			    }

		  if(mError_code)  break ;                          /* ���� ���� ������ - �����.������ */
/*- - - - - - - - - - - - - - - ��������� �����.������ � IF-FOR ���� */
      if(!nNext_flag) {                                             /* ���� ������ ������ ����� ... */
			       nId_stk_cnt++ ;                      /*   ��������� �� ����� ������ */
		       nId_stk[nId_stk_cnt].if_for_flag= 1 ;        /*   ��������� ������ � IF_DO ����� */
		       nId_stk[nId_stk_cnt].block_flag = 0 ;
		       nId_stk[nId_stk_cnt].addr       =byte_prv ;
		       nId_stk[nId_stk_cnt].oper       =nOper_crn ;
		       nId_stk[nId_stk_cnt].row        =mRow ;

	if(nPass_sts!=_ACTIVE_PASS)                                 /* ���� ������ �� �������� - */
		       nId_stk[nId_stk_cnt].pass_sts=_PASSIVE_PASS; /*    ���.��������� ����     */
		      }
/*- - - - - - - - - - - - - - - - -  ��������� ����. � �����. ������ */
   if(nPass_sts==_ACTIVE_PASS) {                                    /* ���� ��������� ������ ... */

	     if(nNext_flag)  text=for_addr[2] ;                     /*   ���.����� ������������� ����� */
	     else            text=for_addr[0] ;

       while(1) {                                                   /*   �����. ����� �� ���������� � ������������ ',' ... */
		     addr=iFind_close(text, ',') ;                  /*     ���� ����������� */
		  if(addr!=NULL)  *addr=0 ;                         /*     ����.�������� */

			     iLeet(text) ;                          /*     ������������ �������� */

                  if(mError_code==_DCLE_EMPTY)  mError_code=0 ;     /*     ������ ��������� �� ��������� ������� */

		  if(mError_code )  break ;                         /*     ���� ���� ������ - ����� */
		  if(addr==NULL)  break ;                           /*     ���� ��������� �������� ... */

		     text=addr+1 ;
		}

				     if(mError_code)  break ;
			       }
/*- - - - - - - - - - - - - - -  ��������� ����������-�������� ����� */
   if(nPass_sts==_ACTIVE_PASS) {                                    /* ���� ��������� ������ ... */
     if((for_addr[1])[0]!=0)  iCulculate(for_addr[1]) ;             /*   ���� ���� �������� ��������� -       */
     else                       nIf_ind=1 ;                         /*    ��������� ���, � ��� ��� ���������� */
								    /*       ��������� �������� �������       */
	    if(mError_code)  break ;                                /*   ���� ���� ������ - ����� */

              if(nIf_ind)  nPass_sts=_ACTIVE_PASS ;                 /*   �����. ��� ������� */
              else         nPass_sts= _BREAK_PASS ;

	 nId_stk[nId_stk_cnt].pass_sts=nPass_sts ;                  /*   ������� ������ ������� */
			       }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
				   block_exp_flag= 2 ;              /* ���.���� �������� ����� */
                                       nNext_flag= 0 ;
			    }                                       /* END.5 */
/*------------------------------------------- ��������� ��������� IF */

    if(oper_type==_IF_) {                                           /* IF.6 - ���� �������� IF ... */
			      text+=_IF_SIZE ;                      /* �������� � ����������� */
			      addr =nOper[nOper_crn].end-1 ;        /*   �������� ���������   */
			     *addr =  0 ;

			     nId_stk_cnt++ ;                        /* ��������� ����� ������ */
		      nId_stk[nId_stk_cnt].if_for_flag= 0 ;         /* ��������� ������ � IF_DO ����� */
		      nId_stk[nId_stk_cnt].else_flag  = 0 ;
		      nId_stk[nId_stk_cnt].block_flag = 0 ;

   if(nPass_sts!=_BREAK_PASS)
     if(nPass_sts==_ACTIVE_PASS) {                                  /* ���� ���� �������������� ������ ... */
	                           iCulculate(text) ;               /*   ��������� �������� ��������� */
                               if(mError_code)  break ;             /*   ���� ���� ������ - ����� */

               if(nIf_ind)  nPass_sts=_ACTIVE_PASS ;                /*   ���.����������� IF ��� ELSE */
               else         nPass_sts=  _ELSE_PASS ;
				 }
     else                   nPass_sts=_PASSIVE_PASS ;               /* ���� ��������� ������... */

	    nId_stk[nId_stk_cnt].pass_sts=nPass_sts ;               /* ������� ������ ������� */

			   block_exp_flag=2 ;                       /* ���.���� �������� ����� */
			}                                           /* END.6 */
/*----------------------------------------- ��������� �������� ����� */

    if(oper_type==_OPEN_) {                                         /* IF.8 - ���� �������� OPEN ... */

      if(block_exp_flag) {
			     nId_stk[nId_stk_cnt].block_flag=1 ;
			 }
      else               {
			       mError_code=_DCLE_FREE_OPEN ;        /* ���� '���������' ���� - */
				     break ;                        /*   - ����� ���������     */
			 }
			  }                                         /* END.8 */
/*----------------------------------------- ��������� �������� ����� */

					 nId_num=0 ;                /* �����.����� ��������� ����� */
    if(oper_type==_CLOSE_) {                                        /* IF.9 - ���� ����� ����� ... */

      for(i=0 ; i<=nId_stk_cnt ; i++)                               /* ���� '�������' �������� */
	       if(nId_stk[i].block_flag)  nId_num=i ;

      if(nId_num==0) {  mError_code=_DCLE_FREE_CLOSE ;              /* ��� "�����������" ��������� - */
			       break ;                 }            /*   ���.������ � �������.�����. */

			     close_flag=1 ;                         /* ��� ���� �������� */
			   }                                        /* END.5 */
/*------------------------------------- ��������� ��������� CONTINUE */

    if(oper_type==_CONTINUE_) {                                     /* IF.9 - ���� �������� CONTINUE ... */

      if(nPass_sts==_ACTIVE_PASS) {                                 /* ���� �������� ������ ... */

	for(n=0, i=1 ; i<=nId_stk_cnt ; i++)                        /*   ���� ������ ����������� ����� */
		  if(nId_stk[i].if_for_flag)  n=i ;

	 if(n==0) {  mError_code=_DCLE_FREE_CONTINUE ;              /*   ��� ���������� ����� -        */
			     break ;                   }            /*     ���.������ � �������.�����. */

		  mByte_cnt=nId_stk[n].addr  ;
		 nOper_next=nId_stk[n].oper ;
		       mRow=nId_stk[n].row-1 ;
		nId_stk_cnt=        n ;
		 nNext_flag=        1 ;
			break ;
				  }

				any_flag=1 ;
			      }                                     /* END.9 */
/*---------------------------------------- ��������� ��������� BREAK */

    if(oper_type==_BREAK_) {                                        /* IF.8 - ���� �����.�������� BREAK ... */

      if(nPass_sts==_ACTIVE_PASS) {                                 /* ���� �������� ������ ... */
	for(n=0,i=1 ; i<=nId_stk_cnt ; i++)                         /*   ���� ������ ����������� ����� */
		   if(nId_stk[i].if_for_flag)  n=i ;

	 if(n==0) {  mError_code=_DCLE_FREE_BREAK ;                 /*   ��� ���������� ����� -        */
			     break ;                }               /*     ���.������ � �������.�����. */

		    nPass_sts=_BREAK_PASS ;                         /*   ����� - ����. ������ �� ����� ����� */
	  nId_stk[n].pass_sts=_BREAK_PASS ;                         /*   ������� ������ ������� */
				  }

				any_flag=1 ;
			   }                                        /* END.8 */
/*----------------------------------------- ��������� ��������� GOTO */

    if(oper_type==_GOTO_) {                                         /* IF.5 - ���� �������� GOTO ... */

      if(nPass_sts==_ACTIVE_PASS) {                                 /* ���� �������� ������ ... */
		    text+=_GOTO_SIZE ;                              /* �����.��������� �� ����� */
		     len =strlen(text) ;                            /* �����.����� ����� */

	      mark_target=(char *)calloc(len+1, 1) ;                /* �������� ����� ������� ����� */
	   if(mark_target==NULL) {  mError_code=_DCLE_MEMORY ;      /* ���� ������ ��������� - */
					    break ;            }    /*   - ����� ���������     */

			       strcpy(mark_target, text) ;          /* ������� ����� � ����� */
                                       mByte_cnt=nSeg_start ;       /* ���.����� � ������ �����.�������� */
					    mRow=nSeg_row ;
				     nId_stk_cnt=  0 ;              /* ����� ��-����� */
				       else_flag=  0 ;
                                       nPass_sts=_PASSIVE_PASS ;
					      break ;               /* ������� �� ���� ����.������ ������ */
				  }
				       any_flag=1 ;
			  }                                         /* END.5 */
/*--------------------------------------- ��������� ��������� RETURN */

    if(oper_type==_RETURN_) {                                       /* IF.6 - ���� �����.�������� RETURN ... */

      if(nPass_sts==_ACTIVE_PASS) {                                 /* IF.7 - ���� �������� ������ ... */
				     ret_flag=1 ;                   /* ���.���������� RETURN */

			     text+=_RETURN_SIZE ;

		   if(text[0]==0)  break ;                          /* ���� RETURN-��������� ����������� */

	          iCulculate(text) ;                                /* ��������� RETURN-��������� */
	     if(mError_code)  break ;                               /* ���� ���� ������ - ����� */

	   memcpy(&nReturn, &nResult, sizeof(nResult)) ;            /* ����.��������� �������� */
		   nReturn.work_nmb=  0 ;

	if(nResult.type!=_CHR_PTR &&                                /* ���� ������������ �� ��������� ... */
	   nResult.type!=_DGT_PTR   ) { 
					    size=nResult.buff ;     /*  ���.������ ������ RETURN-������� */
	    if(nResult.type!=_CHR_AREA)
			   size*=iDgt_size(nResult.type) ;

		     nRet_data=calloc(size, 1) ;                    /*  ��������� ����� RETURN-������� */
	      memcpy(nRet_data, nResult.addr, size) ;               /*  �������� RETURN-������ � ����� */
		     nReturn.addr=nRet_data ;                       /*  ���.����� ������ RETURN-������� */
                                      }

				      break ;
				  }                                 /* ELSE.7 */

      else                          any_flag=1 ;                    /* ELSE.7 - ���� ��������� ������ ... */
								    /* END.7 */
			    }                                       /* END.6 */
/*------------------------------------ ��������� �������� ���������� */

    if(oper_type==_DECLARE_) {                                      /* IF.5 - ���� �������� ���������� ... */

      if(nPass_sts==_ACTIVE_PASS) {                                 /* ���� �������� ������ ... */
				      iDecl_maker(text, NULL) ;     /*  �����.�������� */
				    if(mError_code)  break ;        /*  ���� ���� ������ ... */
		 		  }
				       any_flag=1 ;
			     }                                      /* END.5 */
/*----------------------------------- ��������� ��������� ���������� */

   if(oper_type==_LEET_) {

	if(nPass_sts==_ACTIVE_PASS)  iLeet(text) ;

			if(mError_code)  break ;

			     any_flag=1 ;
			 }
/*------------------- ����������� ��������� IF-ELSE � FOR ���������� */

   if(!close_flag)
    if(any_flag && nId_stk_cnt)                                     /* �������� ���������� ���������� */
	if(!nId_stk[nId_stk_cnt].block_flag)  close_flag=1 ;

/*---------------------- ��������� �������� IF-ELSE � FOR ���������� */

    if(close_flag) {
		       if(nId_num==0)  nId_num=nId_stk_cnt ;

				       iIef_close(nId_num) ;        /* ��������� �������� */

	  if(nId_stk[nId_num].if_for_flag ||                        /* ��� ���������� ELSE, FOR... */
	     nId_stk[nId_num].else_flag     ) {
//???????????                                 nId_stk_cnt=nId_num ;
	      if(nNext_flag)  break ;

		      nId_stk_cnt=nId_num-1 ;

	     for( ; nId_stk_cnt>0 ; nId_stk_cnt--) {                /*   ��������� ��� "����������" ELSE � DO */
		 if( nId_stk[nId_stk_cnt].block_flag   )  break ;   /*    ������������� ���� � �������������  */
		 if(!nId_stk[nId_stk_cnt].if_for_flag &&            /*     ���������� �������� �����          */
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

		       } while(mError_code==0 && !ret_flag) ;       /* CONTINUE.1 - ���� ��� ������ */

/*-------------------------------------------- �������������� ������ */


/*-------------------------- �������� ������ � ������������ �������� */
/*-------------------------------------------------------------------*/


  if(nLocal==1) {
                         iCall_final() ;                            /* ���������� �������� ���������� ������������ �������� ��������� */

       for(i=0 ; i<nT_blocks_cnt ; i++)  free(nT_blocks[i].data) ;  /* ������������ ������ ��������� ������ */

                    if(nT_blocks!=NULL)  free(nT_blocks) ;
                } 
       
  if(!nFix_flag)  iMem_free() ;                                     /* �������� ������� ���������� ������� */

//    for(i=0 ; i<dcl_def_cnt ; i++)   free(dcl_define[i].name) ;   /* ������������ ������ DEFINE-�� */

  if(mFile_flag)  close(mFile_hdl) ;

/*-------------------------------------------------------------------*/

    return(mError_code) ;
}


/*********************************************************************/
/*                                                                   */
/*                 ��������� ��������� ����������                    */
/*                                                                   */
/*      �������������� ��������� ���� ��������� ����������:          */
/*               =    -  ����������                                  */
/*              <==   -  �����������                                 */
/*              <++   -  ���������� (����������� � �����)            */
/*              @=    -  ����������-�������� ����������              */
/*                                                                   */
/*      ���������� ���� ��������� ���������� -                       */
/*  (���� ������� �������� �����������:                              */
/*             d - �����                                             */
/*             a - ������ �����                                      */
/*            pa - ��������� �� �����                                */
/*             c - ������ (�������������� ������)                    */
/*             s - ���������� ������                                 */
/*            ps - ��������� �� ���������� ������ )                  */
/*                                                                   */
/*       �������� ���������� :   d={ d | c }                         */
/*                              pa={ a | s | ps | pa }               */
/*                               c={ d }                             */
/*                              ps={ a | s | ps | pa }               */
/*                                                                   */
/*       �������� ����������� :        d  <=={ s | ps }              */
/*                              { a | pa }<=={ a | pa | s | ps }     */
/*                              { s | ps }<=={ d | a | pa | s | ps } */
/*               ����������� ����� � �������� �������                */
/*          ������������ �� '����������' ������� ����� - 8 ����.     */
/*                                                                   */
/*       �������� ��-���������� :  { d | s | ps }@= d                */
/*                                                                   */

    char *Lang_DCL::iLeet(char *string)
{
  Dcl_decl  dest ;          /* �������� '���������' */
  Dcl_decl *var ;           /* ����� ��������� ������� ���������� */
      char *leet ;          /* ��������� �� �������� ����������/����������� */
      char *culc ;          /* ����������� ��������� */
       int  oper_type ;     /* ��� ��������:         */
#define       _EQU_OPER  0  /*   ������� ����������             */
#define      _COPY_OPER  1  /*   �����������                    */
#define      _APND_OPER  2  /*   ����������                     */
#define      _CNUM_OPER  3  /*   ����������-�������� ���������� */
       int  insert_flag ;   /* ���� ������ ������� */
       int  v_type ;        /* ������� ��� ���������� ��������� */
       int  d_type ;        /* ������� ��� '���������' */
       int  r_type ;        /* ������� ��� ���������� */
       int  d_mode ;        /* ���������� ��� '���������' */
       int  r_mode ;        /* ���������� ��� ���������� */
      char *addr ;
       int  buff ;
       int  size ;
      char *addr_c ;
    double *addr_d ;
    double  value ;
      char *incl_begin ;    /* ������ INCLUDE-������� */
      char *incl_end  ;     /* ����� INCLUDE-������� */
       int  incl_shift ;    /* ����� ������ ��� INCLUDE �������� */
       int  len ;
      char  conv[128] ; 
      char *end ;

/*---------------------------------------------------- ������������� */

   if(string==NULL) {  nIf_ind=0 ;                                  /* ��������� ������ �������� */
			return(NULL) ;  }

/*---------------------------------- ��������� ���������� ���������� */

                     oper_type=-1 ;
                        d_type= 0 ;
                          culc="" ;
/*- - - - - - - - - - - - - - - - - - - - ����� ��������a ���������� */
     for(leet=string ; ; leet++) {                                  /* ��������� ������ � �������   */
	       leet=iFind_close(leet, '=') ;                        /*  ��������� ���������� = ,    */
	    if(leet==NULL)  break ;                                 /*    ��������� ��� ����        */
								    /*  ������ ��������� ���������� */
	    if(!OPER_CHAR(leet[ 1]) &&                              /*  ������ '='                  */
	       !OPER_CHAR(leet[-1])   )   break ;
				 }

	if(leet!=NULL) {  oper_type=_EQU_OPER ;                     /* ���� �������� ���������� ������� - */
			       culc=leet+1 ;    }                   /*   ���.�����. ��� ��������          */
/*- - - - - - - - - - - - - - - - - - -  ����� ��������� ����������� */
   if(leet==NULL) {                                                 /* IF.0 - ���� ��.�����. ����������� ... */
     for(leet=string ; ; leet++) {                                  /* ��������� ������ � ������� */
	       leet=iFind_close(leet, '<') ;                        /*  ��������� ����������� <== */
	    if(leet==NULL)  break ;

	    if(leet[ 1]=='=' && leet[2]=='=')  break ;
				 }

	if(leet!=NULL) {  oper_type=_COPY_OPER ;                    /* ���� �������� ����������� ������� - */
			       culc=leet+3 ;     }                  /*   ���.�����. ��� ��������           */
		  }                                                 /* END.0 */
/*- - - - - - - - - - - - - - - - - - -  ����� ��������� ����������� */
   if(leet==NULL) {                                                 /* IF.0 - ���� ��.�����. ����������� ... */
     for(leet=string ; ; leet++) {                                  /* ��������� ������ � ������� */
	       leet=iFind_close(leet, '<') ;                        /*  ��������� ����������� <++ */
	    if(leet==NULL)  break ;

	    if(leet[ 1]=='+' && leet[2]=='+')  break ;
				 }

	if(leet!=NULL) {  oper_type=_APND_OPER ;                    /* ���� �������� ����������� ������� - */
			       culc=leet+3 ;     }                  /*   ���.�����. ��� ��������           */
		  }                                                 /* END.0 */
/*- - - - - - - - - - - - - - - - - -  ����� ��������� ��-���������� */
   if(leet==NULL) {                                                 /* IF.1 - ���� ��.�����. ����������� ... */
     for(leet=string ; ; leet++) {                                  /* ��������� ������ � ������� */
	       leet=iFind_close(leet, '@') ;                        /*  ��������� ��-���������� @= */
	    if(leet==NULL)  break ;

		if(leet[ 1]=='=')  break ;
				 }

	if(leet!=NULL) {  oper_type=_CNUM_OPER ;                    /* ���� �������� ��-���������� ������� - */
			       culc=leet+2 ;  }                     /*   ���.�����. ��� ��������             */
		  }                                                 /* END.1 */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
	  if(leet==NULL)  culc=string ;                             /* ���� ��������� ����������    */
								    /*   ��� ����������� �� ������� */
/*-------------------------------------------- ��������� '���������' */

  if(leet!=NULL) {                                                  /* ���� ������ �������� ���������� ... */
    if(string==leet) {  mError_code=_DCLE_EMPTY_DEST ;              /*   ���� ��� '���������' � �������� - */
			          return(NULL) ;        }           /*     ������ � ����� �� ���������     */

	var=iComplex_var(string, leet-1, &dest) ;                   /*   '���������' �������� */

		      if(mError_code)  return(NULL) ;               /*   �����.������... */

	    v_type=t_base(var->type) ;                              /*   ����.�������� ���� ���������� ��������� */
	    d_type=t_base(dest.type) ;                              /*   ���������� �������� �           */
	    d_mode=t_mode(dest.type) ;                              /*     ����������� ����� '���������' */

       if(nFragm_flag)  insert_flag=1 ;                             /*   ���� �������� �������� ������������� */
       else             insert_flag=0 ;                             /*    ������ - ���.����� �������          */
		 }
/*---------------------------------- ���������� ��������� ���������� */

	   iCulculate(culc) ;                                       /* ��������� ��������� */

      if(mError_code)  return(NULL) ;                               /* �����.������... */

      if(leet==NULL)  return(NULL) ;                                /* ���� �������� �� ������ ... */

	    r_type=t_base(nResult.type) ;                           /*   ���������� �������� �          */
	    r_mode=t_mode(nResult.type) ;                           /*     ����������� ����� ���������� */

/*------------------------- �������� ����������-��������� ���������� */
/*-------------------------------------------------------------------*/

 if(oper_type==_CNUM_OPER) {                                        /* IF.2 */

/*------------------------------------- �������������� ������->����� */

    if( d_type==_DGT_VAL     && 
       (r_type==_CHR_AREA ||
        r_type==_CHR_PTR    )  ) {

          if(nResult.size>=sizeof(conv))  nResult.size=sizeof(conv) ;

                       memset(conv, 0, sizeof(conv)) ;
                       memcpy(conv, nResult.addr, nResult.size) ;
              value=iStrToNum(conv, &end) ;

					r_type=_DGT_VAL ;           /* �����.������� ������������ */
					r_mode=_DCLT_DOUBLE ;       /*  �������� ����������       */
			          nResult.addr= &value ;
			          nResult.size=   1 ;
			          nResult.buff=   1 ;
        
                                     oper_type=_EQU_OPER ;          /* ����� ��������� ������� �������� ���������� */
			         }
/*-------------------------------------- ���������� ���������� ����� */

    else 
    if( r_type==_DGT_VAL     && 
       (d_type==_CHR_AREA ||
        d_type==_CHR_PTR    )  ) {

         if(nResult.addr!=NULL &&
	       dest.addr!=NULL   ) {

			len= sizeof(conv)-3 ;                       /* ����������� �������� ���������   */
		      value=iDgt_get(nResult.addr, r_mode) ;        /*  � ��� ����������� ������������� */
                                gcvt(value, len, conv) ;

                           len=strlen(conv) ;                       /* ������� ����� � ����� ����� */
                   if(conv[len-1]=='.')  conv[len-1]=0 ;

			          nResult.addr=       conv ;        /* �����.������� ������������ */
			          nResult.size=strlen(conv) ;       /*  �������� ����������       */
			          nResult.buff=strlen(conv) ;
					r_type=_CHR_AREA ;

				     oper_type=_COPY_OPER ;         /* ����� ��������� �������� ����������� */
				   }
				  }
/*-------------------------------------------------------------------*/

    else                           mError_code=_DCLE_DIF_TYPE ;
			   }                                        /* END.2 */

/*---------------------------------- �������� �����������/���������� */
/*-------------------------------------------------------------------*/

 if(oper_type==_COPY_OPER ||                                        /* IF.3 */
    oper_type==_APND_OPER   ) {

  if(nResult.addr!=NULL &&                                          /* IF.4 - ���� ��������� � �������� */
        dest.addr!=NULL   ) {                                       /*           ���������� ...         */

/*-------------------------- ������. ��������� ���������� � �������� */

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
/*--------------------------------------------- ����������� �������� */

    if(insert_flag) {                                               /* IF.5 */

      if(oper_type==_APND_OPER) {
                              mError_code=_DCLE_BAD_OPERATION ;
                                        return(NULL) ;
                                }

		  incl_begin=(char *)dest.addr ;                    /* ��������� ��������� INCLUDE-������� */
		  incl_end  =(char *)dest.addr+dest.size ;
		  incl_shift=     nResult.size-dest.size ;

	 if(dest.size>=nResult.size) {                              /* ����� '������' �������        */
	      memmove((char *)dest.addr+nResult.size,               /*  ��� ������ ������� �         */
		      (char *)dest.addr+dest.size,                  /*   ��������� ��� ������������� */
			      dest.buff-dest.size       ) ;         /*     ������������ �� ������    */
				     }                              /*       ��������� ������        */
	 else                        {
	   if(nResult.size>dest.buff) nResult.size=dest.buff ;

		       size=dest.buff-nResult.size ;
	    if(size)
	      memmove((char *)dest.addr+nResult.size,
		      (char *)dest.addr+dest.size, size ) ;
				     }

		  memmove(dest.addr, nResult.addr,                  /* ���������� ����������� */
				     nResult.size*CHR_SZ) ;
			  dest.size+=nResult.size-dest.size ;       /* ��������� ������� ������� */
                    }                                               /* ESLE.5 */
/*----------------------------------- ������� �����������/���������� */

    else            {                                               /* ESLE.5 */
/*- - - - - - - - - - - - - - - - -  ��������� ������������ �������� */
       if( var->prototype!=NULL &&                                  /* ���� ������������ ������... */
          *var->prototype== 'D'   ) {

         if(oper_type==_APND_OPER)  buff=dest.size+nResult.size ;
         else                       buff=nResult.size ; 

         if(buff>dest.buff) {                                       /* �������������� ������  */
                                iBuff_realloc(var, &dest, buff) ;
		      if(mError_code)  return(NULL) ;
                            }
                                    }
/*- - - - - - - - - - - - - - - - - - - - - - -  ���������� �������� */
       if(oper_type==_APND_OPER) {
                                     addr=(char *)dest.addr+dest.size ; 
                                     buff=        dest.buff-dest.size ;
                                 }
       else                      {
                                     addr=(char *)dest.addr ; 
                                     buff=        dest.buff ;
                                 }

       if(nResult.size>buff)  size=buff ;                           /* ������ ����������� ���������        */
       else                   size=nResult.size ;                   /*  �� ������ ��������� ������� ������ */

       if(size>0) {
                        memmove(addr, nResult.addr, size*CHR_SZ) ;  /* ���������� ����������� */
                    if(oper_type==_APND_OPER)  dest.size+=size ;    /* ���.������ ������� */
                    else                       dest.size =size ;
                  }
       else       {
		    if(oper_type==_COPY_OPER)   dest.size=0 ;       /* ���.������ ������� */
                  }      

                    }                                               /* END.5 */
/*--------------------------- ��������� �������� �������� ���������� */

      if(d_type==_CHR_AREA ||
	 d_type==_CHR_PTR    )
			  if(insert_flag)                      ;
			  else             var->size=dest.size ;

/*-------------------------------------------------------------------*/
			    }                                       /* END.4 */
			      }                                     /* END.3 */
/*---------------------------------------------- �������� ���������� */
/*-------------------------------------------------------------------*/

 if(oper_type== _EQU_OPER) {                                        /* IF.6 */

/*------------------------------------------------- ���������� ����� */

	 if(r_type==_DGT_VAL &&
	    d_type==_DGT_VAL   ) {

	      if(nResult.addr!=NULL &&
		    dest.addr!=NULL   ) {

 	        value=iDgt_get(nResult.addr, r_mode) ;
	 	      iDgt_set(value, dest.addr, d_mode) ;
                                      dest.size=1 ;
					}
				 }
/*----------------------------------- �������������� ����� -> ������ */

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
/*----------------------------------- �������������� ������ -> ����� */

    else if(      r_type==_CHR_AREA &&
	          d_type==_DGT_VAL  &&
	    nResult.buff==  1         ) {

				    addr_c=(  char *)nResult.addr ;
				    addr_d=(double *)   dest.addr ;
				     value=*addr_c ;
	      if(addr_d!=NULL &&
		 addr_c!=NULL   ) iDgt_set(value, addr_d, d_mode) ;

					}
/*-------------------------------------------- ���������� ���������� */

    else if((r_type== _DGT_AREA ||
	     r_type== _DGT_PTR  ||
	     r_type== _CHR_AREA ||
	     r_type== _CHR_PTR  ||
	     r_type==_NULL_PTR    ) &&
	    (d_type== _DGT_PTR  ||
	     d_type== _CHR_PTR    ) &&
	     v_type==  d_type         ) {


	 if(r_type==_DGT_AREA ||
	    r_type==_DGT_PTR    ) {                                 /* ���� ��������� - �������� ���������, */
		         nResult.size*=iDgt_size(r_mode) ;          /*  �� ��������� ��� � ����������       */
		         nResult.buff*=iDgt_size(r_mode) ;
				  }

				   var->addr=nResult.addr ;         /* ����������� ��������� */
				   var->size=nResult.size ;
				   var->buff=nResult.buff ;

	 if(v_type==_DGT_PTR) {                                     /* ���� �������� - �������� ���������, */
		    var->size/=iDgt_size(var->type) ;               /*  �� �����. ������������ ���         */
		    var->buff/=iDgt_size(var->type) ;
			      }
					}
/*-------------------------------------------------------------------*/

    else if(d_type==_DGT_AREA ||
	    d_type==_CHR_AREA   )  mError_code=_DCLE_CONST_DEST ;

    else                           mError_code=_DCLE_DIF_TYPE ;
			   }                                        /* END.6 */

/*----------------------------- �������� ��������� �������� �������� */
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
/*         �/� ����� ������ ��������� �� ����� ��� ������               */
/*                                                                      */
/*    text  -   ����� �����                                             */
/*    addr  -   �����                                                   */
/*    size  -   ������� �����                                           */

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
/*              �������� IF, ELSE � FOR ����������                      */
/*                                                                      */
/*   pnt  -  ��������� ������ �����                                     */

  int  Lang_DCL::iIef_close(int  pnt)

{
//   int  n ;

/*------------------------------------------- �������� ��������� FOR */

       if(nId_stk[pnt].if_for_flag) {

	 if(nPass_sts==_ACTIVE_PASS) {                              /* ���� �������� ������ - */
				 mByte_cnt =nId_stk[pnt].addr  ;    /*  ������������ �������� */
				 nOper_next=nId_stk[pnt].oper ;     /*   CONTINUE             */
				      mRow =nId_stk[pnt].row-1 ;
				 nNext_flag=       1 ;
				     }
    else if(nPass_sts== _BREAK_PASS) {                              /* ���� ����� �� ����� ����� - */
	    if(nId_stk[pnt].pass_sts==_BREAK_PASS)                  /*  - �������� �������� ������ */
				  nPass_sts=_ACTIVE_PASS ;
		 		     }
				    }
/*------------------------------------------ �������� ��������� ELSE */

  else if(nId_stk[pnt].else_flag) {
//         if(nPass_sts==_ELSE_PASS)  nPass_sts=_ACTIVE_PASS ;

	 if(nPass_sts!=_BREAK_PASS) {
                                      nPass_sts=nId_stk[pnt].pass_sts ;
	   if(nPass_sts==_ELSE_PASS)  nPass_sts=_ACTIVE_PASS ;
				   }

				  }
/*-------------------------------------------- �������� ��������� IF */

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
/*                  ���������� ���������� ��������������             */

  typedef struct {
                     char   program[FILENAME_MAX] ;  /* ������������� ��������� */
                      int   fix_flag ;               /* ���� ���������� ������ �������������� */
               Dcl_if_for   id_stk[_IFFOR_MAX] ;     /* ���� IF-DO ���������� */
                      int   id_stk_cnt ;             /* ��������� ����� IF-DO ���������� */
                      int   id_num ;                 /* ��������� "�������� �����" IF_DO ����� */
                      int   subrow_cnt ;             /* ������� �������� */
                 Dcl_oper   oper[_OPER_MAX] ;        /* ������ �������� ���������� */
                      int   oper_cnt ;               /* ������� ���������� */
                      int   oper_crn ;               /* ����� ������� ��������� */
                      int   oper_next ;              /* ����� ��������� ��� �������� �� ������ ����� */
                     long   seg_start ;              /* ��������� ����� �������� */
                      int   seg_row  ;               /* ��������� ������ �������� */
                      int   pass_sts ;               /* ������ ������� */
                      int   file_flag ;              /* ���� ���������� ��������� � ����� */
                      int   file ;                   /* ��������� ����� ��������� */
                     char  *memory ;                 /* ����� ��������� � ������ */
                     long   byte_cnt ;               /* ������� ������ ����� */
                      int   next_flag ;              /* ���� ���������� ������� ����� */
                 Dcl_decl **vars ;                   /* �������� ���������� � �������� */
               Dcl_tblock  *tblock ;                 /* ������ ��������� ������ */
                      int   tblock_cnt ;
		 } Dcl_stack ;


      int  Lang_DCL::Stack(int  oper_code)

{
         Dcl_stack *addr ;

  static Dcl_stack *stack[_STACK_MAX] ;
  static       int  stack_cnt ;

/*--------------------------------------------- ������ ������ � ���� */

  if(oper_code==_DCL_PUSH) {
/*- - - - - - - - - - - - - - - - - - - - - - - - - ���������� ����� */
     if(stack_cnt==_STACK_MAX) {  mError_code=_DCLE_STACK_OVR ;     /* ���� ���� ��������... */
						return(-1) ;    }

	   addr=(Dcl_stack *)calloc(sizeof(*addr), 1) ;             /* �������� ������ ��� ���� */
	if(addr==NULL) {  mError_code=_DCLE_STACK_MEM ;             /* ���� ������ ���... */
				   return(-1) ;         }

	   stack[stack_cnt]=addr ;                                  /* ������� ����� ������ � ���� */
		 stack_cnt++    ;
/*- - - - - - - - - - - - - - - - - - - - -  ��������� ������ � ���� */
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
/*- - - - - - - - - - - - - - - - - - - - - - - ������������� ������ */
		        nFix_flag=0 ;                               /* ���� ���������� ������ �������������� */
			  nId_num=0 ;                               /* ����."�������� �����" IF_DO ����� */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
		       }
/*--------------------------------------- ���������� ������ �� ����� */

  if(oper_code==_DCL_POP) {
/*- - - - - - - - - - - - - - - - - - - - - - - - - ���������� ����� */
       if(stack_cnt<=0) {  mError_code=_DCLE_STACK_UND ;            /* ���� ���� ����... */
                                        return(-1) ;      }

                      stack_cnt-- ;
	   addr=stack[stack_cnt] ;                                  /* ��������� ����� ������ �� ���� */
/*- - - - - - - - - - - - - - - - - - �������������� ������ �� ����� */
   if(mError_code==0)  memcpy(mProgram_name, addr->program,         /* �������.��������� �����. ������ */
					 sizeof(mProgram_name)) ;   /*     ��� ���������� ������       */

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
/*- - - - - - - - - - - - - - - - - - - - - - -  ������������ ������ */
			     free(addr) ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
		      }
/*-------------------------------------------------------------------*/

		   iCulc_stack(oper_code) ;
  if(!mError_code)  iMem_stack(oper_code) ;

    return(0) ;
}
