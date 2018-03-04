/*   Bolotov P.D.  version  29.04.1994   */
/*     0 warnings at warning level 1     */

/*********************************************************************/
/*                                                                   */
/*                   DATA CONVERTION LANGUAGE                        */
/*                                                                   */
/*                     ����������� ���������                         */
/*                                                                   */
/*********************************************************************/

#include  <errno.h>
#include  <stddef.h>
#include  <stdlib.h>
#include  <string.h>
#include  <ctype.h>
#include  <math.h>
#include  <malloc.h>


#define DCL_INSIDE

#include "dcl.h"

/*-------------------------------------------- ����������� ��������� */

#define    _SYS_BASE      '0'
#define      _Q_SHIFT    0.49    /* ����� ����������� */

/*------------------------------------------------ �������� �������� */

#define   _ADD_OPER  101
#define   _SUB_OPER  102
#define   _MUL_OPER  103
#define   _DEV_OPER  104
#define   _POW_OPER  105
#define   _LNK_OPER  106
#define   _INC_OPER  111
#define   _DEC_OPER  112
#define   _NEG_OPER  121

#define    _LT_OPER   201
#define    _LE_OPER   202
#define    _EQ_OPER   203
#define    _NE_OPER   204
#define    _GE_OPER   205
#define    _GT_OPER   206

#define    _OR_OPER   301
#define   _AND_OPER   302
#define   _INV_OPER   303

#define   _AOR_OPER   401

typedef struct {                  /* �������� �������� ���������� */
                 char *name ;    /* ��� �������� */
                  int  len ;     /* ����� ����� �������� */
                  int  code ;    /* ��� �������� */
                  int  type ;    /* ��� �������� */
               } Dcl_proc ;

#define  _BINARY_OT   1     /* �������� �������� */
#define   _UNARY_OT   2     /* ������� �������� */
#define  _PREFIX_OT   4     /* ���������� ������� �������� */

/*  ��������! ������� �������� ������ �������� �� 1 ��� 2   */
/* ��������. ����� ������ ������� ������ � ������ OPER_CHAR */
/* � ����� DCL.H .                                          */

#define  _PROC_MAX  18
 static Dcl_proc dcl_proc[_PROC_MAX]={  /* �������� ���������� �������� */
       "++",       2,  _INC_OPER,  _UNARY_OT,
       "--",       2,  _DEC_OPER,  _UNARY_OT,
       "!",        1,  _INV_OPER,  _UNARY_OT | _PREFIX_OT,
       "^",        1,  _POW_OPER, _BINARY_OT,
       "/",        1,  _DEV_OPER, _BINARY_OT,                          /* ��������! ��������� ������� ������ */
       "*",        1,  _MUL_OPER, _BINARY_OT,                          /*   ���� ������ ���������� ��������� */
       "-",        1,  _SUB_OPER, _BINARY_OT | _UNARY_OT | _PREFIX_OT, /* ��������! ��������� ��������� ������ */
       "+",        1,  _ADD_OPER, _BINARY_OT,                          /*      ���� ������ ���������� �������� */
       "@",        1,  _LNK_OPER, _BINARY_OT,
       "==",       2,   _EQ_OPER, _BINARY_OT,
       "!=",       2,   _NE_OPER, _BINARY_OT,
       "<=",       2,   _LE_OPER, _BINARY_OT,
       ">=",       2,   _GE_OPER, _BINARY_OT,
       "<",        1,   _LT_OPER, _BINARY_OT,
       ">",        1,   _GT_OPER, _BINARY_OT,
       "&&",       2,  _AND_OPER, _BINARY_OT,
       "||",       2,   _OR_OPER, _BINARY_OT,
       "|",        1,  _AOR_OPER, _BINARY_OT
                                     } ;

/*-------------------------------------------- ���������� ���������� */

//       int   dcl_result_dst ;  /* ������ ���������� ����������: */
	  		         /*             0 - dcl_result    */
	                         /*             1 - 1-�� �������  */
                                 /*             2 - 2-�� �������  */

/*---------------------------------------- ������� ������� ��������� */

#define _EXT_LOAD  (void *)0x0000ffffL   /* '�����' ������������� ������� */

#ifdef  _EXT_LIBS

#define  _LOAD_MAX  20

    Dcl_decl *dcl_ext_list[_LOAD_MAX] ;        /* ������ ���������� ������������ �������� */
    int       dcl_ext_cnt ;                    /* ������� ������������ �������� */

#endif

/*------------------------------------------------------ ����������� */

//#define DGT_SZ     sizeof(double)
#define CHR_SZ     sizeof(char)
#define TTL_SZ     sizeof(nResult)

#define O_TYPE     operand->type
#define O_ADDR     operand->addr
#define O_SIZE     operand->size
#define O_BUFF     operand->buff
#define O_WORK     operand->work_nmb
#define O1_TYPE    operand1->type
#define O1_ADDR    operand1->addr
#define O1_SIZE    operand1->size
#define O1_BUFF    operand1->buff
#define O2_TYPE    operand2->type
#define O2_ADDR    operand2->addr
#define O2_SIZE    operand2->size
#define O2_BUFF    operand2->buff

#define R_TYPE     nResult.type
#define R_ADDR     nResult.addr
#define R_SIZE     nResult.size
#define R_BUFF     nResult.buff

#define P_TYPE       result->type
#define P_ADDR       result->addr
#define P_SIZE       result->size
#define P_BUFF       result->buff
#define P_RESULT     result

/*-------------------------------------------------------------------*/


/*********************************************************************/
/*                                                                   */
/*                      ��������� ���������                          */

  char *Lang_DCL::iCulculate(char *string)

{
	int  sub_flag ;     /* ���� ����������� ������� ������������ � ������� */
	int  string_flag ;  /* ���� ���������� ������ ���������� ������ */
       char *beg ;          /* ��������� �� ����������� ������ */
       char *end ;          /* ��������� �� ����������� ������ */
	int  cnt ;          /* ����� ������ */
	int  end_len ;      /* ����� �������� ������ */
	int  len ;
       char *addr ;

/*---------------------------------------------------- ������������� */

   if(string==NULL) {  nIf_ind=0 ;                                  /* ��������� ������ �������� */
			return(NULL) ;  }

		    iNext_title(_CLEAR_WORK_VAR) ;                  /* ����� �������� ������� ����� */
		     iAlloc_var(_CLEAR_WORK_VAR, NULL, 0) ;         /* ������� ������� ������� */

#ifdef  _EXT_LIBS
      for( ; dcl_ext_cnt ; dcl_ext_cnt--)                           /* ����� ����� ������������ �������� */
	dcl_ext_list[dcl_ext_cnt-1]->addr=_EXT_LOAD ;
#endif

/*-------------------------------------------------- �������� ������ */

       string_flag=0 ;
	  sub_flag=0 ;
	       cnt=0 ;

   for(addr=string ; *addr ; addr++) {                              /* CIRCLE.1 - ��������� ������ */
/*- - - - - - - - - - - - - - - - - - - ����������� ���������� ����� */
    if(*addr=='"')  if(string_flag) {                               /* ������������� � ��������� */
		     if(*(addr-1)!='\\' ||
			*(addr-2)=='\\'   ) string_flag=0 ;         /*   �������� � ��������     */
				    }                               /*    ���������� ������      */
		    else                    string_flag=1 ;

    if(string_flag)  continue ;                                     /* ������ ����.������ */
/*- - - - - - - - - - - - - - - - - - - -  ����������� ������� ����� */
      if(*addr=='?')  {  addr+=1 ;  continue ;  }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - ������� ������ */
	      if(*addr=='(') {  cnt++ ;  sub_flag=1 ;  }            /* ������������ ������ */
	      if(*addr==')') {  cnt-- ;  sub_flag=1 ;  }

	       if(cnt<0)  break ;                                   /* ���� ����������� ������ - ������ */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
				     }                              /* CONTINUE.1 */

     if(cnt>0) {  mError_code=_DCLE_OPEN ;   return(NULL) ;  }      /* ��������� �������������� ������ */
     if(cnt<0) {  mError_code=_DCLE_CLOSE ;  return(NULL) ;  }

/*--------------------------------- ��������� ������������ � ������� */

	       len=strlen(string) ;

 if(sub_flag)                                                       /* IF.1 - ���� �������� ������������ ... */
  for(end=string ; ; end++) {                                       /* CIRCLE.2 - ���� �� ����� ������ */
	      end=iFind_close(end, _ANY_CLOSE) ;                    /* ���� ����� ���������� ����.������ */
	   if(end==NULL)  break ;                                   /* ���� ������� ������ ��� - ����� */

// ??????  if(end[-1]=='?') continue ;

	      beg=iFind_open(string, end-1, '(') ;                  /* ���� ������ ������ */

	 if(beg!=string)
	   if(NAME_CHAR(*(beg-1)))  continue ;                      /* ���� ����� ������� ���������    */
								    /*  ���-����.������ - ��� �������, */
								    /*   �� �� ����������              */
		 *end=0 ;                                           /* ����������� ������������ � ������� */

	       addr=iSimple_string(beg+1) ;                         /* �������.��������� � ������� */
	   if(mError_code)  return(NULL) ;                          /* ���� ���� ������ - _QUIT-����� */

	       end_len=strlen(end+1)+1 ;                            /* ���������� ������ "������",  */
		   len=strlen(addr) ;                               /* ������������� ���������      */

		memmove(beg+len, end+1, end_len) ;                  /* "���������" �������� ��������� */
		memmove(beg, addr, len) ;                           /* ��������� �����.�����. � �������� */

		  end=beg+len-1 ;                                   /* ��������� ��������� ����.������ */
	   }                                                        /* CONTINUE.2 */
								    /* END.1 */
/*--------------------------------- ��������� "����������" ��������� */

	      iSimple_string(string) ;                              /* ������������ �������� ��������� */
	   if(mError_code)  return(NULL) ;                          /* ���� ���� ������ - _QUIT-����� */

/*-------------------------------------------------------------------*/

  return(NULL) ;
}


/*********************************************************************/
/*                                                                   */
/*                ��������� ������� �����                            */
/*  Return: ��� ������ ���������� ��������� ��� NULL - ��� ������    */

   char *Lang_DCL::iSimple_string(char *string)

{
      Dcl_decl  operand1 ;  /* �������� ����������/��������� */
      Dcl_decl  operand2 ;  /* �������� ����������/��������� */
      Dcl_decl *work ;      /* �������� ������� ������ */

	   int  oper_flag ;    /* ���� ������� �������� */
	   int  code ;         /* ��� ��������, ��������������� */
	   int  oper_code ;    /* ��� ��������, ������������� */
	  char *oper_name ;    /* ��� �������� */
	   int  oper_len ;     /* ����� ����� �������� */
	   int  type ;         /* ��� �������� */
	  char *first ;        /* ������ ������� */
	  char *second ;       /* ������ ������� */
	  char *end ;          /* O�������� ������� �������� */
	  char *oper_pos ;     /* ��������� �������� �������� */
	   int  dst_nmb ;      /* ����� ������� ������ ���������� */
	   int  str_len ;      /* '���������' ����� ������ */

	  char *addr ;
//	  char  next ;
	   int  len ;
	   int  i ;
//	   int  j ;

   static char  work_name[]={'?', '0', 0} ;

#define  _WORK_SIZE   2

/*---------------------------------------------- ����� ������������� */

      if(*string==0) {  mError_code=_DCLE_EMPTY ;  return(NULL) ; } /* ��������� ������ ������ */

	 addr=iFind_close(string, _ANY_OPER) ;                      /* ���� ����� ������ �������� */
      if(addr!=NULL)  oper_flag=1 ;                                 /* �� ���������� ������ ���. */
      else            oper_flag=0 ;                                 /*   ���� ������� ��������   */

          memset(&operand1, 0, sizeof(operand1)) ;
          memset(&operand2, 0, sizeof(operand2)) ;

/*----------------------------------------------- ��������� �������� */

      if(oper_flag) {                                               /* IF.1 - ��� ������� �������� */

	for(i=0 ; i<_PROC_MAX ; i++) {                              /* CIRCLE.1 - ��������� �� ��������� */

			    code=dcl_proc[i].code ;                 /* ���.��� ������� �������� */
		       oper_name=dcl_proc[i].name ;                 /* ���.��� ������� �������� */
			oper_len=dcl_proc[i].len ;                  /* ���.����� ����� ������� �������� */
			    type=dcl_proc[i].type ;                 /* ���.��� ������� �������� */

	 for(oper_pos=string ; ; oper_pos++) {                      /* CIRCLE.2 - �������� ������� �������� */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - ����� �������� */
		 oper_pos=iFind_close(oper_pos, oper_name[0]) ;     /* ���� ���� ������� ������� ����� */
	      if(oper_pos==NULL)  break ;                           /* ��� ���������� - ����.�������� */

	      if(oper_len==1) {
		   if((  oper_pos!=string &&
		       OPER_CHAR(oper_pos[-1])) ||
		       OPER_CHAR(oper_pos[ 1])    )  continue ;
			      }
	      else            {
		    if(oper_pos[1]!=oper_name[1]) continue ;
			      }

                       mError_position=oper_pos ;                   /* ���.��������� ������ �� �������� */
			       str_len=strlen(string)+1 ;           /* ���.����� ������ */
/*- - - - - - - - - - - - - - - - - - - - ��������� ������� �������� */
       if(oper_pos!=string) {                                       /* IF.2 - ���� ��������� 1-� ������� ... */
	    first=iFind_open(string, oper_pos-1, _ANY_OPER_OR_END); /* ���� ������ �������� */
					         
	 if(first==NULL) {  mError_code=_DCLE_BAD_LEFT ;            /* ���� ������ �� ������� - ������ */
				   return(NULL) ;        }
//          first++ ;                                               /* ������������ ��������� �� ������ */

	 if(first==oper_pos)  first=NULL ;                          /* ���� ������� ����������� */
			    }                                       /* ELSE.2 */
       else                 {                                       /* ELSE.2 - ���� 1-�� �������� ��� ... */
			      first=NULL ;
			    }                                       /* END.2 */
/*- - - - - - - - - - - - - - - - - - - - ��������� ������� �������� */
	   second=oper_pos+oper_len ;                               /* ���.������ 2-�� �������� */
	      end=iFind_close(second, _ANY_OPER_OR_END) ;           /* ���� ��������� �������� */
	   if(end==NULL) {  mError_code=_DCLE_BAD_RIGHT ;           /* ���� ��������� �� ������� - ������ */
				   return(NULL) ;         }

	   if(end<second)  second=NULL ;  //??????????              /* ���� ������� ����������� ... */
/*- - - - - - - - - - - - - - - - - - - - - - �������� ���� �������� */
	if(first==NULL && second==NULL) mError_code=_DCLE_EMPTY_BOTH; /* ���� ��������� ��� ������ - ������ */
   else if(first==NULL || second==NULL) {                           /* ���� ������� �������� ... */
	 if(type & _UNARY_OT) {                                     /*   ���� ������� �������� ��������� ... */
	  if((type & _PREFIX_OT) &&                                 /*     ���� �������� �����������,  */
	      second==NULL         )  mError_code=_DCLE_PREFIX_OPER;  /*      � �/� ������ ����������... */
			      }
	 else                         mError_code=_DCLE_BINARY_OPER ; /*   ���� �/� �������� ��������... */
					}
   else                                 {                           /* ���� �������� �������� ... */
	    if(!(type & _BINARY_OT))  mError_code=_DCLE_UNARY_OPER ;  /*   ���� �/� ������� ��������... */
					}

	 if(mError_code)  return(NULL) ;
/*- - - - - - - - - - - - - - - - - - ��������� ������� �������� '-' */
    if( code==_SUB_OPER &&
       first==  NULL      )  oper_code=_NEG_OPER ;
    else                     oper_code=  code ;
/*- - - - - - - - - - - - - - - - - - - - - - -  ���������� �������� */
  if(first!=NULL)  {                                                /* '����������' ���������, */
		     iComplex_var(first, oper_pos-1, &operand1) ;   /*   ������, ���� �������  */
  if(!mError_code)                                                  /*    ������ ����, �� ��   */
   if(second!=NULL)  iComplex_var(second,       end, &operand2) ;   /*   ����������� � ������  */
		   }                                                /*      �������            */
  else             {
                     iComplex_var(second,       end, &operand1) ;
                   }

	     if(mError_code)  return(NULL) ;                        /* ���� ���� ������ ... */

	         iProcessor(oper_code, &operand1, &operand2) ;      /* �������� �������� */

	     if(mError_code)  return(NULL) ;                        /* ���� ���� ������ ... */

	if(nResult_dst==1) {                                        /* ���� ��������� ������� */
				   dst_nmb=operand1.work_nmb ;      /*  � ���� �� ��������� - */
				      work=iGet_work(dst_nmb) ;     /*   �������� �� � �����. */
			       memcpy(work, &operand1, TTL_SZ) ;    /*     ������� ������     */
			   }
   else if(nResult_dst==2) {
				   dst_nmb=operand2.work_nmb ;
				      work=iGet_work(dst_nmb) ;
			       memcpy(work, &operand2, TTL_SZ) ;
			   }
   else                    {                                        /* ���� ��������� - � ����.������ ...  */
				  work=iNext_title(_WORK_VAR) ;     /*   �������� ����� ��������� ��������� */
				if(mError_code)  return(NULL) ;     /*   �����.���������� ������ */
			       dst_nmb=work->work_nmb ;             /*   ���.����� ������� ������ �        */
		   memcpy(work, &nResult, sizeof(nResult)) ;        /*    ������� ���� �������� ���������� */
			work->work_nmb=dst_nmb ;                    /*   �����.����� ������� ������ */
			   }

			if(mError_code)  return(NULL) ;

			work_name[1]=_SYS_BASE+dst_nmb ;            /* ���.��� ������ ���������� */

     if(first==NULL)  first=oper_pos ;                              /* ���� ������� �������� ��� ... */
			len=str_len-(end-string) ;
		      memmove(first+_WORK_SIZE, end+1, len) ;       /* ��������� ������ ��������� */
		      memmove(first, work_name, _WORK_SIZE) ;       /* ��������� ��� ���������� */

		   oper_pos=first+_WORK_SIZE-1 ;                    /* ���.������ �����.������� */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
					     }                      /* CONTINUE.2 */

				     }                              /* CONTINUE.1 */
		    }                                               /* END.1 */
/*--------------------------- ��������� '���������' ������ ��������� */

		len=strlen(string) ;
	   iComplex_var(string, string+len-1, &operand1) ;          /* ����������� '���������' ������ */

	 if(mError_code)  return(NULL) ;                            /* ���� ���� ������ ... */

	    memcpy(&nResult, &operand1, sizeof(nResult)) ;          /* ��������� ��������� � ������ ���. */

      if(nResult.work_nmb)   dst_nmb=nResult.work_nmb ;             /* ���� ������ - ������� ... */
      else                 {                                        /* ���� ������ �� ������� ... */
				  work=iNext_title(_WORK_VAR) ;     /*   �������� ����� ��������� ��������� */
				if(mError_code)  return(NULL) ;     /*   �����.���������� ������ */
			       dst_nmb=nWork_cnt ;                  /*    ������� ������ � ������� ����     */
		   memcpy(work, &nResult, sizeof(nResult)) ;        /*      �������� ����������             */
			work->work_nmb=nWork_cnt ;                  /*   �����.����� ������� ������ */
			   }

			work_name[1]=_SYS_BASE+dst_nmb ;            /* ���.��� ������ ���������� */

/*-------------------------------------------------------------------*/

  return(work_name) ;
}


/************************************************************************/
/*                                                                      */
/*           ��������� ������������ ���������� ��� ���������            */
/*                                                                      */
/*    beg       -  ������ �����. ���������                              */
/*    end       -  ��������� �����. ���������                           */
/*    operand   -  �������� ��������                                    */
/*                                                                      */
/*    123.45          - �������� ���������                              */
/*    "..."           - ���������� ������ - ���������                   */
/*    NULL            - NULL-���������                                  */
/*    ?N? (3 �������) - ��������������� ���������� ����� N              */
/*    Name            - ������� ����������                              */
/*                                                                      */
/*    Name[Index]     - ������� ������� ��� ������:                     */
/*                        Name-������ ��� ���������, Index-�����        */
/*    Name[Index]     - ��������� �� ������:                            */
/*                        Name-������, Index-������                     */
/*                                                                      */
/*    Name[Begin<Sb>...<Se>End] - ��������� �� �������� ������ :        */
/*           Begin - ��������� ������ ��� ������ � �������              */
/*                     ���������� ��������                              */
/*                      (��� ���������� - � ������ ������)              */
/*             End - ��������� ������ ��� ������ �������                */
/*                     ����������� ��������                             */
/*                      (��� ���������� - �� ����� ������)              */
/*            <Sb> - ������� ������������ ���������: +/-                */
/*                    ������������� ���������� �� ��������� ������      */
/*                     � ������ ���������                               */
/*                      (��� ���������� ��������� ��� +)                */
/*            <Se> - �������� ������������ ���������: +/-               */
/*                    ������������� ���������� �� ��������              */
/*                     � ������ ���������                               */
/*                      (��� ���������� ��������� ��� +)                */
/*                                                                      */
/*    �������� ��� ��������� ������������ ����:                         */
/*                                                                      */
/*    Name[Index].Elem  - �������� ������ � �������� ������ Index       */
/*    Name.Elem         - �������� ������ � �������� ������� ������     */
/*                                                                      */
/*    ��������� - �������� �������� !!!                                 */
/*     ��������� �������� ����������� �������� ��� ����������           */
/*      ��������� �������� !                                            */
/*                                                                      */
/*   ���������� ����� ��������� ������� ����������                      */

   Dcl_decl *Lang_DCL::iComplex_var(char *beg, char *end, Dcl_decl *operand)

{
           Dcl_decl  *head ;       /* �������� ����������/��������� */
           Dcl_decl  *par ;        /* �������� ��������� */
           Dcl_decl **pars_table ; /* ������ ������� ������� ���������� */
           Dcl_decl  *var ;        /* ��������� ������������ ���������� */
           Dcl_decl  *x_object ;   /* ������ �� X-������, ������������ ����� */
 Dcl_complex_record  *x_record ;   /* ������ �� ������ X-������� */
                int   chr_save ;   /* ������ ���������� ������� */
                int   name_len ;   /* ����� ����� */
                int   type ;       /* ������� ��� ���������� ��� ��������� */
                int   mode ;       /* ���������� ��� ���������� ��� ��������� */
                int   r_type ;     /* ������� ��� ���������� */
                int   fragm_flag ; /* ���� �������������� ������� */
                int   inc1_flag ;  /* ������� ������������ ��������� */
                int   inc2_flag ;  /* �������� ������������ ��������� */
               char  *index ;      /* ������ ���������� ��������� */
               char  *pars ;       /* ������ ���������� ��������� */
                int   pars_cnt ;   /* ������� ���������� */
                int   proto_flag ; /* ���� �������� ����� ���������� */
                int   proto_type ; /* �������������� ��� ��������� */
               char  *f_sep ;      /* ����������� '���������������' ������� */
           Dcl_decl   start ;      /* �������� ������� ��������� */
               char  *begin ;      /* ����� ����� ��������� */
               char  *addr ;
               char  *stop ;
             double   value ;
             double  *dbl  ;
                int   up_len ;     /* ����� ����-�� ����� �������� */
                int   len ;
                int   cnt ;
                int   i ;
                int   j ;

   Dcl_decl *(*dcl_proc)(Lang_DCL *, Dcl_decl **, int) ;

/*---------------------------------------------------- ������������� */
/*-------------------------------------------------------------------*/

	 mError_position= beg ;
		     var=&nTmp ;
		   f_sep= NULL ;
	      fragm_flag=   0 ;

	  *operand->name=   0 ;

/*------------------------------------ ���������� ������ - ��������� */
/*-------------------------------------------------------------------*/

       if(*beg=='"') {                                              /* IF.1 - ���� ���������� ������ ... */
			   head=iNext_title(_WORK_VAR) ;            /* �������� ����� ��������� ��������� */
								    /*     ������� ����������             */
			if(mError_code)  return(NULL) ;             /* �����.���������� ������ */

				  len= end-beg-1 ;
			   head->type=_CHR_AREA ;                   /* ������ ��� � ������ */
			   head->buff= len ;                        /*  ���������          */

		      addr=(char *)iAlloc_var(_WORK_VAR, head, 0) ; /* �������� ������ ��� ��������� */
                   if(mError_code)  return(NULL) ;                  /* ��������� ���������� ������ */

	  for(i=1, j=0 ; i<=len ; i++, j++) {                       /* CIRCLE.1 - ��������� ��������� ... */
	      if(beg[i]=='\\') {                                    /* ���� �����.����-������ ... */
			   i++  ;                                   /*   ������� ��������� ������ */
		    if(beg[i]=='n' )  addr[j]='\n' ;                /*   ��������� NL */
	       else if(beg[i]=='r' )  addr[j]='\r' ;                /*   ��������� RET */
	       else if(beg[i]=='t' )  addr[j]='\t' ;                /*   ��������� TAB */
	       else if(beg[i]=='\'')  addr[j]='\'' ;                /*   ��������� ������� ' */
	       else if(beg[i]=='\"')  addr[j]='\"' ;                /*   ��������� ������� " */
	       else if(beg[i]=='\\')  addr[j]='\\' ;                /*   ��������� ������� \ */
	       else if(beg[i]=='x') {                               /*   ��������� 16-������� ���� �������, */
		       addr[j]=(char)strtol(beg+i+1, &stop, 16) ;   /*    ��� �������� ������ ���������     */
		   if(stop==beg+i+1 ||                              /*     1 ��� 2 �����                    */
		      stop>=beg+i+4   )  mError_code=_DCLE_INVALID_CHR ;
			i=stop-beg-1 ;
				    }
	       else                  {                              /*   ��������� 8-������� ���� �������, */
		       addr[j]=(char)strtol(beg+i, &stop, 8) ;      /*    ��� �������� ������ ���������    */
		   if(stop==beg+i  ||                               /*     1, 2 ��� 3 �����                */
		      stop>=beg+i+4  )  mError_code=_DCLE_INVALID_CHR ;
			i=stop-beg-1 ;
				     }
			       }
	      else                    addr[j]=beg[i] ;              /* ��������� '��������' ������� */

               if(mError_code)  return(NULL) ;                      /* ��� ����������� ������ �����.��������� */
					    }                       /* CONTINUE.1 */

			  O_TYPE=_CHR_AREA ;                        /* �����. ���, �����  */
			  O_ADDR= addr ;                            /*   � ����� �������� */
			  O_BUFF= len ;
			  O_SIZE=  j ;
			  O_WORK=  0 ;
		     }                                              /* END.1 */
/*--------------------------------------- ��������������� ���������� */
/*-------------------------------------------------------------------*/

  else if(*beg=='?') {                                              /* IF.2 - ���� ����.���������� ... */
			       i=beg[1]-_SYS_BASE ;                 /* ��������� ����� ������� ������ */
			    head=iGet_work(i) ;                     /* �������� ����� �� ��������� */
			memcpy(operand, head, sizeof(*head)) ;      /* �������� �������� ���������� �� ����� */
		     }                                              /* END.2 */
/*---------------------------------------- ������� ������������ ���� */
/*-------------------------------------------------------------------*/

  else if(iXobject_check(beg, end, &index)) {

                        *index= 0 ;                                 /* ����������� ��� ������� */

/*------------------------------------------------------ ����� CLEAR */

     if(        end-index==strlen("clear")  &&
        !memcmp(index+1, "clear", end-index)  ) {
/*- - - - - - - - - - - - - - - - - - - - - -  ������������� ������� */
             head=iFind_var(beg, 0) ;                               /* ���� ���������� */
         if(mError_code)  return(NULL) ;

         if(head==NULL) {  mError_code=_DCLE_UNKNOWN ;              /* ���� ����� ���������� ���...*/
                                return(NULL) ;          }
         
         if(t_base(head->type)!=_XTP_OBJ) {                         /* ���� ��� �� ������ ������������ ���� */
                                   mError_code=_DCLE_BAD_LEFT ;
                                             return(NULL) ;                    
                                          }
/*- - - - - - - - - - - - - - - - - - - - ���������� ������� ������� */
              iXobject_clear(head) ;

         if(mError_code)  return(NULL) ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                                                }
/*----------------------------------------------------- ����� DELETE */

     else
     if(        end-index==strlen("delete")  &&
        !memcmp(index+1, "delete", end-index)  ) {
/*- - - - - - - - - - - - - - - - - - - - - -  ������������� ������� */
             head=iFind_var(beg, 0) ;                               /* ���� ���������� */
         if(mError_code)  return(NULL) ;

         if(head==NULL) {  mError_code=_DCLE_UNKNOWN ;              /* ���� ����� ���������� ���...*/
                                return(NULL) ;          }
         
         if(t_base(head->type)!=_XTP_OBJ) {                         /* ���� ��� �� ������ ������������ ���� */
                                   mError_code=_DCLE_BAD_LEFT ;
                                             return(NULL) ;                    
                                          }
/*- - - - - - - - - - - - - - - - - - -  ���������� ������� �������� */
                        head=NULL ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                                                 }
/*-------------------------------------------------------- ����� ADD */

     else
     if(        end-index==strlen("add")  &&
        !memcmp(index+1, "add", end-index)  ) {
/*- - - - - - - - - - - - - - - - - - - - - -  ������������� ������� */
             head=iFind_var(beg, 0) ;                               /* ���� ���������� */
         if(mError_code)  return(NULL) ;

         if(head==NULL) {  mError_code=_DCLE_UNKNOWN ;              /* ���� ����� ���������� ���...*/
                                return(NULL) ;          }
         
         if(t_base(head->type)!=_XTP_OBJ) {                         /* ���� ��� �� ������ ������������ ���� */
                                   mError_code=_DCLE_BAD_LEFT ;
                                             return(NULL) ;                    
                                          }
/*- - - - - - - - - - - - - - - - - - - ���������� ���������� ������ */
              iXobject_add(head, NULL) ;
         if(mError_code)  return(NULL) ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                                              }
/*------------------------------------------------------ ����� COUNT */

     else
     if(        end-index==strlen("count")  &&
        !memcmp(index+1, "count", end-index)  ) {
/*- - - - - - - - - - - - - - - - - - - - - -  ������������� ������� */
             head=iFind_var(beg, 0) ;                               /* ���� ���������� */
         if(mError_code)  return(NULL) ;

         if(head==NULL) {  mError_code=_DCLE_UNKNOWN ;              /* ���� ����� ���������� ���...*/
                                return(NULL) ;          }
         
         if(t_base(head->type)!=_XTP_OBJ) {                         /* ���� ��� �� ������ ������������ ���� */
                                   mError_code=_DCLE_BAD_LEFT ;
                                             return(NULL) ;                    
                                          }
/*- - - - - - - - - - - - - - - - - - - - -  ������ �������� ������� */
                       value=head->buff ;

                  head=iNext_title(_WORK_VAR) ;                     /* �������� ����� ��������� ��������� ������� ���������� */
          if(mError_code)  return(NULL) ;                           /* �����.���������� ������ */

                        head->type=_DGT_VAL ;                       /* ������ ��� � ������ ���������� */
                        head->addr=&value ;
                        head->size=  1 ;
                        head->buff=  1 ;

                     iAlloc_var(_WORK_VAR, head, 1) ;               /* �������� ������ ��� ��������� */
          if(mError_code)  return(NULL) ;                           /* ��������� ���������� ������ */

                 memcpy(operand, head, sizeof(*head)) ;             /* ������ ������ �� ����� */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                                                }
/*----------------------------------------------- ��������� �������� */
     else                                        {
/*- - - - - - - - - - - - - - - - - - - - ��������� ������ �� ������ */
                head=iComplex_var(beg, index-1, operand) ;          /* ��������� ��������� �� ������ */
            if(mError_code)  return(NULL) ;

            if((operand->type & _DCLT_XTP_REC)==0) {                /* ���� ������� ������ �� ������� ������� ������ */
                                         i=head->size ;
                   iXobject_index(head, &i, operand) ;
                                                   }

              x_record=(Dcl_complex_record *)operand->addr ;

/*- - - - - - - - - - - - - - - - - - - -  ��������� FIELD$-�������� */
      if(                end-index>=6  &&                           /* ���� �������� ��� ����� (field$-��������) */
         !memcmp(index+1, "field$", 6)   ) {

              var=iXobject_field(x_record, index+1, end-index) ;
            if(mError_code)  return(NULL) ;
                                           }
/*- - - - - - - - - - - - - - - - - - - - -  ��������� ���� �� ����� */
      else                                 {                        /* ���� ������ �� ���� */

                   var=x_record->elems ;
                   cnt=x_record->elems_cnt ;

        for(i=0 ; i<cnt ; var++, i++)                               /* ���� ���� ������ �� ����� */
          if(!memcmp(var->name, index+1, strlen(var->name)) &&
              strlen(var->name)==end-index                    )  break ; 

          if(i>=cnt) {  mError_code=_DCLE_TYPEDEF_ELEM ;            /* ���� ������ ���� ���... */
                                    return(NULL) ;       }
                                           }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                     memcpy(operand, var, sizeof(*var)) ;
                                                 }
/*-------------------------------------------------------------------*/
                                            }
/*---------------------------- �������� � ��������� ����� � �������� */
/*-------------------------------------------------------------------*/

  else if(*end==']') {                                              /* IF.3 - ���� ������� ������� ... */

	do {                                                        /* BLOCK.1 */

/*---------------------------------------- ��������� ��������� ����� */

      for(index=end-1, i=1 ; index>=beg ; index--)                  /* ���� ���� ������� */
             if(*index=='[') {     i-- ;
                               if(!i) break ;  }
        else if(*index==']')       i++ ;

        if(*index!='[') {  mError_code=_DCLE_NOINDEX ;  break ; }   /* ������ ��� ���������� ������ - ������ */

	     *index= 0 ;                                            /* ����������� ��� */

           head=iComplex_var(beg, index-1, operand) ;               /* ��������� ��� ������� */
//         head=iFind_var(beg, 0) ;                                 /* ���� ��� � ������ */
	if(mError_code)      break ;
	if(head==NULL) {  mError_code=_DCLE_UNKNOWN ;  break ;  }   /* ��� ���������� ���������� � ������� - */
								    /*    ���.������ � ���������� �����.     */
                            type=t_base(head->type) ;               /* ��������� ��� ���������� */
                            mode=t_mode(head->type) ;

        if(type==_DGT_VAL) {  mError_code=_DCLE_INDEXED_VAR ;       /* ���� ��� ������� ���������� -     */
					break ;               }     /*    ���.������ � ���������� �����. */
/*------------------------------------------- ���������� ��� ������� */

	     *end=0 ;                                               /* ����������� ��������� ��������� */

	for(f_sep=index+1 ; ; f_sep++) {                            /* ��������� ��������� ��������� */
			    f_sep=iFind_close(f_sep, '.') ;         /*  �� ������� '��������������'  */
			 if(f_sep==NULL)  break ;                   /*   ����������� - '...'         */

			 if(f_sep[1]=='.'  &&
			    f_sep[2]=='.'    )  break ;
				       }
/*---------------------------------------------- ���� ������� ������ */

	if(f_sep==NULL) {                                           /* IF.4 - ���� ��� ����������� ... */
			     iSimple_string(index+1) ;              /* ������������ ��������� ��������� */
			 if(mError_code)  break ;                   /* ���� ���� ������ ... */

			  O_WORK= 0 ;                               /* ����� �������� ������� ������ */
			     dbl=(double *)nResult.addr ;           /* ��������� ����� ���������� ������� */
			  r_type=t_base(R_TYPE) ;                   /* ��������� ������� ��� ������� */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - ������ - ����� */
	   if(r_type==_DGT_VAL) {                                   /* IF.5 - ���� �������� ������ */

                value=iDgt_get(dbl, R_TYPE) ;                       /* '���������' ������ */
             if(value>0)  i=(int)(value+_Q_SHIFT) ;
             else         i=(int)(value-_Q_SHIFT) ;
           
             if(type==_XTP_OBJ)  iXobject_index(head, &i, operand); /* ���� ������ ������������ ���� */
             else
             if(type==_DGT_AREA ||                                  /* � ����������� �� ���� ������� */
                type==_DGT_PTR    ) {                               /*  ��� ��������� �������������  */
			   O_TYPE=_DGT_VAL | mode ;                 /*   ���, ����� � ����� �������� */
			   O_ADDR=(char *)head->addr+i*iDgt_size(mode) ;
			   O_SIZE= 1 ;
			   O_BUFF= 1 ;
                                    }
             else                   {
			   O_TYPE=_CHR_AREA ;
			   O_ADDR=(char *)head->addr+i ;
			   O_SIZE= 1 ;
			   O_BUFF= 1 ;
                                    }

             if(i<     0    ||                                      /* ������������ ���� �� ������� */
                i>=head->buff ) {  O_ADDR=NULL ;  break ;  }        /*     ������� ��� ������       */

				}                                   /* END.5 */
/*- - - - - - - - - - - - - - - - - - - - - - - - -  ������ - ������ */
	 else if(r_type==_CHR_PTR ||                                /* IF.6 - ���� ������ - ������ */
		 r_type==_CHR_AREA  ) {

	       if(type!=_CHR_AREA &&                                /* ���� ������ ����������� ���   */
		  type!=_CHR_PTR    ) {                             /*   �������������� �� ��������� */
                                mError_code=_DCLE_INDEX_TYPE;       /*   ������� - ������            */
						break ;         
                                      }

		O_TYPE=_CHR_PTR ;                                   /* ���.��� �������� */
		O_ADDR=iFind_index(head, &nResult) ;                /* �����.����� �������� */
             if(O_ADDR!=NULL) {
                      up_len=(char *)O_ADDR-(char *)head->addr ;    /* ���.����� '����� �������' */
                      O_SIZE=head->size-up_len ;                    /* �����.������ �������� */
                      O_BUFF=head->buff-up_len ;                    /* �����.������ �������������� ������ */
                              }
             else             {
                                   O_SIZE=0 ;
                                   O_BUFF=0 ;
                              }
				      }                             /* END.6 */
/*- - - - - - - - - - - - - - - - - - - - - - -  ������������ ������ */
	 else                         {                             /* ���� ��� ���������� ��������� */
				   mError_code=_DCLE_INDEX_TYPE ;   /*   ����������� - ���. ������   */
						  break ;
				      }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
		       }                                            /* ELSE.4 */
/*--------------------------------------------- ���� ������-�������� */
	else           {                                            /* ELSE.4 - ��� �����.����������� ... */
				     fragm_flag=1 ;                 /* ���.���� �������������� ������� */

	  if(type!=_CHR_AREA &&                                     /* ���� '�������������' �������������� */
	     type!=_CHR_PTR    ) {  mError_code=_DCLE_INDEX_FRG ;   /*   ��� ����������� ������� - ������  */
					    break ;               }

			      var=head ;

			   O_WORK= 0 ;                              /* ����� �������� ������� ������ */
			   O_TYPE=_CHR_PTR ;                        /* ���.��� �������� */
/*- - - - - - - - - - - - - - - - - - - -  ��������� ������� ������� */
	      if(f_sep[-1]=='-') { inc1_flag=0 ; f_sep[-1]=0 ;  }   /* �����.�������������        */
	 else if(f_sep[-1]=='+') { inc1_flag=1 ; f_sep[-1]=0 ;  }   /*  ��������� ��������� ����� */
	 else                      inc1_flag=1 ;                    /*   � ���������� ��������    */

				 f_sep[0]=0 ;                       /* ����������� ������ ������ */

	 if(index[1]!=0) {                                          /* IF.7 - ���� ������ ������ ����� ... */
			     iSimple_string(index+1) ;              /* �����. ������ ��������� ��������� */
			 if(mError_code)  break ;                   /* ���� ���� ������ ... */

			   O_TYPE=_CHR_PTR ;                        /* ���.��� �������� */

			      dbl=(double *)nResult.addr ;          /* ��������� ����� ���������� ������� */
			   r_type=t_base(R_TYPE) ;                  /* ��������� ������� ��� ������� */

	      if(r_type==_DGT_VAL   ) {                             /* ���� �������� ������ ... */
		  i = (int)(iDgt_get(dbl, R_TYPE)+_Q_SHIFT) ;       /*   '���������' ������ */
	       if(i<     0    ||                                    /*   �����. ���� �� ������� */
		  i>=head->buff ) {  O_ADDR=NULL ;  break ;  }      /*     ������ �������       */
	  else if(i>=head->size ) {   fragm_flag=0 ;                /*   �����.���� �� ������� ������� */
				      head->size=i ;  }

		 start.addr=(char *)head->addr+i ;                  /*   ���.��������� ����� ��������� */
		      begin=(char *)start.addr ;
		 start.size=head->size-i ;                          /*   ���.����� ��������� */
		 start.buff=head->buff-i ;                          /*   ���.����� ��������� */
				      }
	 else if(r_type==_CHR_PTR ||
		 r_type==_CHR_AREA  ) {                             /* ���� ������ - ������ ... */
		   begin=iFind_index(head, &nResult) ;              /*   �����.��������� ����� ��������� */
		if(begin==NULL) {  O_ADDR=NULL ;  break ;  }        /*   ���� ������ �� ������... */
		    start.addr=begin+nResult.size ;                 /*   �����.����� ������� */
                        up_len=nResult.size+begin-(char *)head->addr ;
		    start.size=head->size-up_len ;                  /*   �����.����� ������� */
		    start.buff=head->buff-up_len ;                  /*   �����.������ ����������� ����� */
				      }
	 else                         {                             /* ���� ��� ���������� ��������� */
				     mError_code=_DCLE_INDEX_TYPE ; /*   ����������� - ���. ������   */
						  break ;
				      }
			 }                                          /* ELSE.7 */
	 else            {                                          /* ELSE.7 - ���� ������ ������ ������ */
			      start.addr=head->addr ;               /* ���.��������� ����� ��������� */
				   begin=(char *)start.addr ;
			      start.size=head->size ;               /* ���.����� ��������� */
			      start.buff=head->buff ;               /* ���.����� �������������� ������ */
			 }                                          /* END.7 */
/*- - - - - - - - - - - - - - - - - - - - ��������� ������ ��������� */
	  if(inc1_flag) {  O_ADDR=begin ;                           /* ���.��������� ����� ��������� */
			   O_SIZE=start.size                        /*  � ��� ��������������� �����  */
				 +(char *)start.addr-begin ;        /*   � ����������� �� ��������   */
			   O_BUFF=start.buff                        /*    ������������� ���������    */
				 +(char *)start.addr-begin ; }
	  else          {  O_ADDR=start.addr ;
			   O_SIZE=start.size ;
			   O_BUFF=start.buff ;                   }
/*- - - - - - - - - - - - - - - - - - - -  ��������� ������� ������� */
	      if(*(f_sep+3)=='-') {  inc2_flag=0 ;  f_sep++ ;  }    /* �����.�������������        */
	 else if(*(f_sep+3)=='+') {  inc2_flag=1 ;  f_sep++ ;  }    /*  ��������� ��������� ����� */
	 else                        inc2_flag=1 ;                  /*   � ���������� ��������    */

	 if(f_sep[3]!=0) {                                          /* IF.8 - ���� ������ ������ ����� ... */
			     iSimple_string(f_sep+3) ;              /* �����. ������ ��������� ��������� */
			 if(mError_code)  break ;                   /* ���� ���� ������ ... */

			  O_TYPE=_CHR_PTR ;                         /* ���.��� �������� */

			     dbl=(double *)nResult.addr ;           /* ��������� ����� ���������� ������� */
			  r_type=t_base(R_TYPE) ;                   /* ��������� ������� ��� ������� */

	      if(r_type==_DGT_VAL   ) {                             /* ���� �������� ������ ... */
		  i = (int)(iDgt_get(dbl, R_TYPE)+_Q_SHIFT) ;       /*   '���������' ������ */
	       if(i< head->size-start.size ||                       /*   �����. ���� �� ������� */
		  i>=head->buff              ) {  O_ADDR=NULL ;     /*     ������ �������       */
						       break ;  }
	  else if(i>=head->size              ) {  i=head->size-1 ;  /*   �����.���� �� ������� ������� */
                                                 fragm_flag=0 ;   } 

		    O_SIZE=(char *)head->addr+i-(char *)O_ADDR+1 ;  /*   ���.����� ��������� */
				      }
	 else if(r_type==_CHR_PTR ||
		 r_type==_CHR_AREA  ) {                             /* ���� ������ - ������ ... */
		      addr=iFind_index(&start, &nResult) ;          /*   �����.�������� ����� ��������� */
		   if(addr==NULL) {  O_ADDR=NULL ;  break ;  }      /*   ���� ������ �� ������... */
              if(inc1_flag)  O_SIZE =addr-(char *)O_ADDR ;          /*   �����.����� ��������� */
              else           O_SIZE =addr-(char *)start.addr ;
	      if(inc2_flag)  O_SIZE+=nResult.size ;                 /*   ��������� ����� ���������        */
				      }                             /*    ��� ���.������������� ��������� */
	 else                         {                             /* ���� ��� ���������� ��������� */
				     mError_code=_DCLE_INDEX_TYPE ; /*   ����������� - ���. ������   */
						  break ;
				      }
			 }                                          /* END.8 */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
		       }                                            /* END.4 */
/*-------------------------------------------------------------------*/

	   } while(0) ;                                             /* BLOCK.1 */
		     }                                              /* END.3 */
/*-------------------------------------------------------- ��������� */
/*-------------------------------------------------------------------*/

  else if(*end==')') {                                              /* IF.9 - ���� ������� ��������� ... */
	do {                                                        /* BLOCK.2 */

/*---------------------------------------- ��������� ��������� ����� */

             x_object=NULL ;

                pars=(char *)memchr(beg, '(', end-beg+1) ;          /* ���� ������ ���������� */
	       *pars= 0 ;                                           /* ����������� ��� */

       if(iXobject_check(beg, pars-1, &index)) {                    /* ���� ����� X-�������... */
                          *index= 0 ;

/*???*/
               if( beg[strlen(beg) - 1] == ']' )    /* ���� ������� ������� � ����� X-������� */
               {
                   char *ind_xobj;  /* ������ ������ */
                   char *pars_ind;  /* ��������� �� ������ ���������� */

                   ind_xobj = (char *)memchr( beg, '[', strlen(beg) );  /* �������� ��������� ��������� */
                   beg[ strlen(beg) - strlen(ind_xobj) ] = 0;
                   pars_ind = (char *)memchr( (pars + 1), ')', strlen(pars + 1) );  /* ���������� ������ � ����� ������ ���������� ������ */
                   if( *(pars+1) != ')' )                               /* ���� ���� ������� ���������, ������ �������  */
                   {
                       (*pars_ind) = ','; 
                       pars_ind++;
                   }
                   (*pars_ind) = '"'; pars_ind++;
                   memcpy( (char *)pars_ind, (char *)(ind_xobj+1), strlen(ind_xobj+1) );
                   pars_ind += strlen(ind_xobj+1)-1;
                   (*pars_ind) = '"'; pars_ind++;
                   (*pars_ind) = ')'; 
                   end = pars_ind;
               }

                            head=iXobject_method(index+1, pars-1) ; /*   �������������� ����� */
                        x_object=      iFind_var(beg, 0) ;          /*   �������������� X-������ */
              if(       x_object       == NULL   )  mError_code=_DCLE_UNKNOWN ;
         else if(t_base(x_object->type)!=_XTP_OBJ)  mError_code=_DCLE_BAD_LEFT ;
                                               }
       else                 head=iFind_var(beg, 1) ;                /* ���� ������� ��������� - ���� ��� � ������ �������� */

	if(mError_code)  break ;
	if(head==NULL ) {  mError_code=_DCLE_UNKNOWN ;  break ;  }  /* ��� ���������� ���������� � ������� - */
								    /*    ���.������ � ���������� �����.     */
	 type=head->type ;                                          /* ��������� ��� ���������� */

/*--------------------------------------------- ��������� ���������� */

		 proto_flag=  1 ;                                   /* ���.���� �������� ����� ���������� */
		 pars_table=&nPars_table[nPars_cnt_all] ;           /* ���.����� ������� ���������� */
		   pars_cnt=  0 ;                                   /* ���������� ������� ���������� */
		       *end=  0 ;                                   /* ����������� ������ ���������� */
		       pars++ ;                                     /* ���.��������� �� ������ �������� */

     if(pars[0]!=0) {                                               /* IF.10 - ���� ���� ��������� ... */

	 do {                                                       /* CIRCLE.2 - ����.������ ���������� ... */
/*- - - - - - - - - - - - - - - - - - - - - - - ���������� ��������� */
		 addr=iFind_close(pars, ',') ;                      /* ���� ����������� ���������� */
	      if(addr!=NULL)  *addr=0 ;                             /* ����������� �������� */

		    iSimple_string(pars) ;                          /* ��������� �������� */
		if(mError_code)  break ;                            /* ���� ���� ������ ... */
/*- - - - - - - - - - - - - - - �������� ���� ��������� �� ��������� */
        if(head->prototype!=NULL)
                 proto_type=head->prototype[pars_cnt] ;             /* �������� �������������� ��� */
        else     proto_type=  0 ;

	if(proto_type==0) proto_flag=0 ;                            /* ���� ������ ���������� ��������� -    */
								    /*  �����.���� �������� ����� ���������� */
	      r_type=t_base(nResult.type) ;                         /* ��������� ������� ��� ���������� */

	 if(proto_flag)                                             /* ���� ������� �������p -     */
	  if((proto_type==_F_DGT_VAL &&                             /*  ������������ ��� ��������� */
		  r_type!=_DGT_VAL     ) ||
	     (proto_type==_F_DGT_PTR &&
	      (   r_type!=_DGT_AREA &&
		  r_type!=_DGT_PTR    )) ||
	     (proto_type==_F_CHR_PTR &&
	      (   r_type!=_CHR_AREA &&
		  r_type!=_CHR_PTR    ))   ) {
				    mError_code=_DCLE_PROTOTYPE ;
                                                  return(NULL) ;
					     }
/*- - - - - - - - - - - - - - - - - -  ��������� ��������� � ������� */
		 par=iNext_title(_WORK_VAR) ;                       /* �������� ����� ��������� ��������� */
								    /*     ������� ����������             */
              if(mError_code)  return(NULL) ;                       /* �����.���������� ������ */

		 memcpy(par, &nResult, sizeof(nResult));            /* ������� ����� ���������� */

	       pars_table[pars_cnt]=par ;                           /* ������� ����� ��������� � ������� */
			  pars_cnt++ ;                              /* �����.������� ���������� */
		     nPars_cnt_all++ ;                              /* �����.��������� ������� ���������� */

			      pars=addr+1 ;                         /* ��� ��������� �� ����.�������� */

		if(addr==NULL)  break ;                             /* ���� ��������� ��������� ... */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
	    } while(1) ;                                            /* CONTINUE.2 */

		    }                                               /* END.10 */
/*-------------------------------------------------- ����� ��������� */
/*- - - - - - - - - - - - - - - - - - - - - - - -  ���������� ������ */
           nPars_cnt=pars_cnt ;                                     /* ���.������� ���������� */
            dcl_proc=(Dcl_decl *(*)(Lang_DCL *,                     /* ���.����� DCL-��������� */
                                    Dcl_decl **, int))head->addr ;  
/*- - - - - - - - - - - - - - - - - - - - - - -  ����� ��������� DCL */
     if(head->func_flag==_DCL_CALL  ||                            /* �������� ��������� ��� ����� � �����.DCL ... */
        head->func_flag==_DCL_METHOD  ) {

            if(x_object!=  NULL   )  
                      head=iXobject_invoke(x_object, head,          /*   ����� ������ X-������� */
                                                 pars_table, pars_cnt) ;
       else if(dcl_proc==_DCL_LOAD) 
                           head=iCall_exec(head->work_nmb) ;        /*   ����� DCL-��������� */
       else                head=  dcl_proc(this, pars_table,        /*   ���������� DCL-����� */
                                                 pars_cnt   ) ;  

                 if(mError_code)  return(NULL) ;                    /*   �����.������ ������ */

		 if(head==NULL)  head=&nVoid ;                      /*   ��������� NULL-�������� */

                        var=iNext_title(_WORK_VAR) ;                /*   �������� ����� ��������� ��������� ������� ���������� */
                 if(mError_code)  return(NULL) ;                    /*   ��������� ���������� ������ */

		      memcpy(var, head, sizeof(*var)) ;             /*   ��������� �� ����� ��������� */
                                  head=var ;

	                   iAlloc_var(_WORK_VAR, var, var->size) ;  /*   �������� ������ ��� RETURN-������ */
                 if(mError_code)  return(NULL) ;                    /*   ��������� ���������� ������ */
				        }
/*- - - - - - - - - - - - - - - - - - - - - - - -  ���������� ������ */
	     nPars_cnt_all-=nPars_cnt ;                             /* ����. �������. ������� ���������� */

	    memcpy(operand, head, sizeof(*head)) ;                  /* �������� ������� �� ����� */

/*-------------------------------------------------------------------*/

	   } while(0) ;                                             /* BLOCK.2 */
		     }                                              /* END.9 */
/*----------------------------------------- ��������� ��������� NULL */
/*-------------------------------------------------------------------*/

  else if((beg[0]=='N' || beg[0]=='n') &&
	  (beg[1]=='U' || beg[1]=='u') &&
	  (beg[2]=='L' || beg[2]=='l') &&
	  (beg[3]=='L' || beg[3]=='l')   ) {
					      O_TYPE=_NULL_PTR ;
					      O_ADDR= NULL ;
					      O_WORK=  0  ;
					   }
/*-------------- ��������� ������� ���������� ��� �������� ��������� */
/*-------------------------------------------------------------------*/

  else               {                                              /* IF.2 - ���� ������� �����. ... */
/*------------------------------ ���������� �������������� ��������� */

			  name_len=end-beg+1 ;
			  chr_save=end[1] ;                         /* ����������� ��� ���������� */
			    end[1]= 0 ;

/*------------------------------------- ��������� �������� ��������� */

			 value=iStrToNum(beg, &stop) ;              /* �������� ����������� ����� */

   if(stop==end+1) {                                                /* IF.11 - ���� �������� - ����� ... */
			   head=iNext_title(_WORK_VAR) ;            /* �������� ����� ��������� ��������� */
								    /*     ������� ����������             */
                        if(mError_code)  return(NULL) ;             /* �����.���������� ������ */

			   head->type=_DGT_VAL ;                    /* ������ ��� � ������ */
			   head->addr=&value ;                      /*    ���������        */
			   head->size=  1 ;
			   head->buff=  1 ;

			      iAlloc_var(_WORK_VAR, head, 1) ;      /* �������� ������ ��� ��������� */
                          if(mError_code)  return(NULL) ;           /* ��������� ���������� ������ */

		      memcpy(operand, head, sizeof(*head)) ;        /* ������ ������ �� ����� */
		   }                                                /* ELSE.11 */
/*------------------------------------- ��������� ������� ���������� */

   else            {                                                /* ELSE.11 - ���� �������� - �����. ... */
                      head=iFind_var(beg, 0) ;                      /* ���� ���������� � ������ */
              if(mError_code)  return(NULL) ;

             if(head==NULL)  mError_code=_DCLE_UNKNOWN ;            /* ��� ���������� ���������� � �������    */
             else           memcpy(operand, head, sizeof(*head)) ;  /*  ���. ������, � ��� ������� - �������� */

		         var=head ;
		   }                                                /* ELSE.11 */
								    /*   ��������� �� �����                   */
/*----------------------------------------- �������������� ��������� */

			if(!mError_code)  end[1]=chr_save ;         /* �������.��������� ��������� */
		     }                                              /* END.2 */
/*-------------------------------------- �������� ������������ ����� */
/*-------------------------------------------------------------------*/

     if(mError_code==_DCLE_UNKNOWN) {                               /* ���� ��� �� ������� ��   */
								    /*  ����������� ������� ... */
				  len=strlen(mError_position) ;
	for(i=0 ; i<len; i++)                                       /*   �����. ������ �������� */
          if(!NAME_CHAR(mError_position[i])) break ;   

	       if(i==len)  mError_code=_DCLE_UNKNOWN_NAME ;         /* ���. ��� ������ */
	       else        mError_code=_DCLE_SINTAX_NAME ;
				    }
/*----------------------------------------- ��������� NULL-��������� */
/*-------------------------------------------------------------------*/

    if(O_ADDR==NULL) {  O_SIZE=0 ;
                        O_BUFF=0 ;  }

/*------------------------------------- ��������� ���������� ������� */
/*-------------------------------------------------------------------*/

  if(t_base(O_TYPE)==_DGT_VAL ) {
			    dbl =(double *)O_ADDR ;
		if(         dbl         ==NULL)  nIf_ind=0 ;
	   else if(iDgt_get(dbl, O_TYPE)== 0. )  nIf_ind=0 ;
	   else                                  nIf_ind=1 ;
				}
  else                          {
                               if(O_ADDR==NULL)  nIf_ind=0 ;
                               else              nIf_ind=1 ;
				}
/*-------------------------------------------------------------------*/
/*-------------------------------------------------------------------*/

	  nFragm_flag=fragm_flag ;

  return(var) ;
}


/************************************************************************/
/*                                                                      */
/*              ����� ���������� ��� ��������� �� �����                 */
/*                                                                      */
/*   name       -  ��� ���������� ��� ���������                         */
/*   func_flag  -  ���� ������ � ����������                             */

   Dcl_decl *Lang_DCL::iFind_var(char *name, int  func_flag)

{
   Dcl_decl *target  ;    /* �������� ������ ���������� � ��������� */
   Dcl_decl *vars ;       /* ������� ������ �������� ���������� � �������� */
        int  f_flag ;     /* ���� ������� */
        int  i ;


/*-------------------------------------------- ������������� */

		   target=NULL ;

/*----------------------------- ������ � ���������� �������� */

  for(i=0 ; i<nTransit_cnt ; i++)
    if(!strcmp(nTransit[i].name, name)) {
                                     target=&nTransit[i] ;
				                break ;
                                        }
/*------------------------- ������ � ����������� ����������� */

   if(!func_flag) {                                         /* ���� ������ ����������... */

     for(vars=nInt_page ; vars->name[0]!=0 ; vars++)
       if(!strcmp(vars->name, name)) {  target=vars ;  
                                             break ;  }
		  }
/*---------------------------- ������ � �������� ����������� */

   for(i=0 ; target==NULL ; i++) {

	if(nVars[i]==NULL) break ;

    for(vars=nVars[i] ; vars->name[0]!=0 ; vars++) {

     if(vars->local_idx!=   0   &&                          /* ������ ���� ��������� */
        vars->local_idx!=nLocal   )  continue ; 

		    f_flag=vars->func_flag ;                /* ���.���� ������� � 1        */
	if(f_flag)  f_flag=1 ;                              /*  ���������� �� ���� ������� */

     if( f_flag==func_flag &&         
	!strcmp(vars->name, name)) {  target=vars ;  break ;  }
						   }
				 }
/*----------------------------- ������ � ������� ����������� */

#ifdef  _EXT_LIBS

   if(target     ==NULL &&                                  /* ���� ������ �� ������ � ������             */
      dcl_ext_lib!=NULL   ) {                               /*  ��������� ����� � ������� ����������� ... */

		    if(func_flag) obj_type=_LOAD_FNC ;      /*  ���.��� ������� */
		    else          obj_type=_LOAD_VAR ;

	      target=dcl_ext_lib(obj_type, name) ;          /*  ���� ������ �� ������� ���������� */
	 if(dcl_ext_error) {  mError_code=dcl_ext_error ;
				    return(target) ;      }
			    }

#endif

/*--------------------------- ��������� ������ DCL-��������� */

   if(target==NULL && func_flag) {                          /* ���� ������ �� ������ */
							    /*   � ��� ������� ...   */
		        target=iCall_find(name) ;           /*  ���� ����� DCL-�������� */
                                 }
/*------------------------- ��������� ������������ ��������� */

#ifdef  _EXT_LIBS

   if(target      !=    NULL  &&                            /* ���� ������ ����������,      */
      target->addr==_EXT_LOAD &&                            /*  ������ ��� ������������     */
      dcl_ext_load!=    NULL    ) {                         /*   ������ ��������� ��������� */

       if(func_flag) obj_type=_LOAD_FNC ;                   /*  ���.��� ������� */
       else          obj_type=_LOAD_VAR ;

	status=dcl_ext_load(obj_type, target->name) ;       /*  ����� ��������� ��������� */

	      target->addr=dcl_ext_addr ;                   /*  ����� ������ ������������� ������� */

      if(dcl_ext_cnt==_LOAD_MAX) {                          /*  �������� �������� ��������� */
			      mError_code=_DCLE_LOAD_LIST ;
				      return(target) ;
				 }

       dcl_ext_list[dcl_ext_cnt]=target ;                   /*  ��������� ������� � ������ */
		    dcl_ext_cnt++ ;                         /*    ������������             */
				   }

#endif

/*-----------------------------------------------------------*/

  return(target) ;
}


/************************************************************************/
/*                                                                      */
/*         ����� ��������� ������ � ������� ������                      */
/*                                                                      */
/*     base   -  ������� ������                                         */
/*     index  -  ��������� ������                                       */
/*                                                                      */
/*    ��� ����������� ������� ���������� ����� �����, � � ���������     */
/*     ������ - NULL.                                                   */

     char *Lang_DCL::iFind_index(Dcl_decl *base, Dcl_decl *index)

{
   int  find_flag ;
  char *b_str ;
  char *i_str ;
   int  len ;
   int  i ;
   int  j ;


    if(index->addr==   NULL ||                                      /* ���� ���.������ �������������� ��� */
       index->size> base->size )  return(NULL) ;                    /*  ���.������ ������� ������� -      */
								    /*   - ��������� �����������          */
    if(index->size==0)  return((char *)base->addr) ;                /* ���� ���.������ - ������, ��     */
								    /*   ��������� - ��� ������� ������ */
	     find_flag=  0 ;                                        /* �����.���� ����������� */
		 b_str=(char *)base->addr ;                         /* ���.������ ������� � */
		 i_str=(char *)index->addr ;                        /*   ��������� �����    */
		   len=base->size-index->size+1 ;                   /* ��� ����� ������ ����� ������� */

   for(i=0 ; i<len ; b_str++, i=b_str-(char *)base->addr) {         /* CIRCLE.1 - ��������� ������ */

       b_str=(char *)memchr(b_str, i_str[0], len-i) ;               /* ���� ���� ��������� ������ */
    if(b_str==NULL)  break ;                                        /* ��� ��� ���������� - ����� ������ */

     for(j=0 ; j<index->size ; j++) if(b_str[j]!=i_str[j]) break ;  /* ��������� '����������' ����� */
								    /*      ��������� ������        */
	if(j==index->size) {  find_flag=1 ;                         /* ���� ���� '��������' - ���. */
				 break ;   }                        /*  ���� �����. � �����.�����  */
                                                          }         /* CONTINUE.1 */

           len=b_str-(char *)base->addr+index->size ;               /* ���� ���.������ ����������� ��    */
	if(len>base->size)  find_flag=0 ;                           /*   ��������� ������� ������ - ...  */

       if(find_flag)  return(b_str) ;
       else           return(NULL) ;
}


/************************************************************************/
/*                                                                      */
/*                            ���������                                 */
/*                                                                      */
/*    oper_code  -  ��� ��������                                        */
/*    operand1   -  �������� 1-�� ��������                              */
/*    operand2   -  �������� 2-�� ��������                              */
/*                                                                      */
/*   ���������� �������� :                                              */
/*                                                                      */
/*      ��� ���� ����� ��������� :                                      */
/*                            @  ������� � ���������� ������            */
/*                                                                      */
/*      ������ - ������ (CHR_AREA | CHR_PTR | NULL_PTR):                */
/*                            -  �������� �������                       */
/*                                ��� �������� ���������                */
/*                            !  ��������                               */
/*                                                                      */
/*      ������ - �����  (CHR_AREA | CHR_PTR | NULL_PTR + DGT_VAL):      */
/*                            +  �������� ��������� �� ������ ������    */
/*                            -  �������� ��������� �� ������ �����     */
/*                                 (������ ������ - �����)              */
/*                            *  ������������ ������                    */
/*              ����������: ��� ������ � NULL ��������� ������ - NULL   */
/*                                                                      */
/*      ������ - ������ (DGT_AREA | DGT_PTR | NULL_PTR):                */
/*                            -  �������� �������                       */
/*                                                                      */
/*      ������ - �����  (DGT_AREA | DGT_PTR | NULL_PTR + DGT_VAL):      */
/*                            +  �������� ��������� �� ������ ������    */
/*                            -  �������� ��������� �� ������ �����     */
/*                                 (������ ������ - �����)              */
/*              ����������: ��� ������ � NULL ��������� ������ - NULL   */
/*                                                                      */
/*      ����� - �����  (DGT_VAL):                                       */
/*                                4 �������������� ��������: +,-,*,/    */
/*                            ^  ���������� � �������                   */
/*                           ++  �������������                          */
/*                           --  �������������                          */
/*                                ��� �������� ���������                */
/*                            -  ������� �����                          */
/*                            !  ��������                               */
/*                           &&  ���������� �                           */
/*                           ||  ���������� ���                         */
/*                            |  �������������� ���                     */
/*                                                                      */

   int  Lang_DCL::iProcessor(int  oper_code, Dcl_decl *operand1, 
                                             Dcl_decl *operand2 )

{
    int      cmpl_type ;  /* ���������� ��� ��������� */
    int      oper_type ;  /* ��� �������� */
#define    _CC_TYPE     _CHR_AREA              /* ������ - ������             */
#define    _CD_TYPE    (_CHR_AREA|_DGT_VAL)    /* ������ - �����              */
#define    _AA_TYPE     _DGT_AREA              /* ������ ����� - ������ ����� */
#define    _AD_TYPE    (_DGT_AREA|_DGT_VAL)    /* ������ ����� - �����        */
#define    _DD_TYPE     _DGT_VAL               /* ����� - �����               */

    int      status ;     /*  */
 double      value ;      /* �������� �������� */
    int      cnt ;        /* ������� */
//  int      size ;
    int      size1 ;      /* ������ 1-�� �������� */
    int      size2 ;      /* ������ 2-�� �������� */
    int      buff1 ;      /* ������ ������ 1-�� �������� */
    int      buff2 ;      /* ������ ������ 2-�� �������� */
    int      if_mask[6]={1, 3, 2, 5, 6, 4} ;  /*  ������ IF-�����: 0 ��� -  < */
					      /*                   1 ��� - == */
					      /*                   2 ��� -  > */

        Dcl_decl *result ;   /* ����� �������� ���������� */
	  double  dgt1 ;     /* �������� �������� ��������� */
	  double  dgt2 ;
   unsigned long  int1 ;
   unsigned long  int2 ;
   unsigned char *addr1 ;    /* ������ ������ ��������� */
   unsigned char *addr2 ;
	     int  type1 ;    /* ������� ���� ��������� */
	     int  type2 ;
	     int  mode1 ;    /* ���������� ���� ��������� */
	     int  mode2 ;

   unsigned char *addr ;
        Dcl_decl *tmp_v ;
    unsigned int  tmp_i ;
	     int  i ;

/*--------------------------------------- ��������� ������� �������� */

      if(oper_code==_INV_OPER ||                                    /* ��� ������� ���������          */
	 oper_code==_INC_OPER ||                                    /*   ������ ������� ������������� */
	 oper_code==_DEC_OPER ||
	 oper_code==_NEG_OPER   )  operand2=operand1 ;

/*---------------------------------------------------- ������������� */

                addr1=(unsigned char *)O1_ADDR ;                    /* ��������� ������ ������ */
                addr2=(unsigned char *)O2_ADDR ;
                type1= t_base(O1_TYPE) ;                            /* ��������� ������� ���� ������ */
                type2= t_base(O2_TYPE) ;
                mode1= t_mode(O1_TYPE) ;                            /* ��������� ���������� ���� ������ */
                mode2= t_mode(O2_TYPE) ;

/*---------------------------------------- ����������� ���� �������� */

	   cmpl_type=type1 | type2 ;                                /* �����.���������� ��� ��������� */
	   oper_type=cmpl_type & 0x000e ;                           /* �����.��� �������� */

	 if((cmpl_type & _NULL_PTR) &&                              /* ���.������� ������:      */
	     oper_type ==_DD_TYPE      )  oper_type=0 ;             /*   ����� - NULL-��������� */

/*------------------------------------ ����������� ������ ���������� */

	 if(operand1->work_nmb) {  nResult_dst=  1 ;                /* �������� ���������� ��������� */
				        result=operand1 ;  }        /*   � ������� ������, � ��� ��  */
    else if(operand2->work_nmb) {  nResult_dst=  2 ;                /*    ���������� � ���������     */
				        result=operand2 ;  }        /*      ������ ����������        */
    else                        {  nResult_dst=  0 ;
				        result=&nResult ;  }

/*-------------------------------------  ������� � ���������� ������ */

	  if(oper_code==_LNK_OPER) {
				     buff1=O1_BUFF ;                /* ��������� ������� ������� ����� */
				     buff2=O2_BUFF ;
	    if(addr1!=NULL)          size1=O1_SIZE ;                /* ������ �������������   */
	    else                     size1= 0 ;                     /*   ����� NULL-��������� */
	    if(addr2!=NULL)          size2=O2_SIZE ;
	    else                     size2= 0 ;

	  if(type1==_DGT_VAL  ||                                    /* �������� ������ ���������          */
	     type1==_DGT_AREA ||                                    /*  ��������� � �������� ������������ */
	     type1==_DGT_PTR    ) {  size1*=iDgt_size(mode1) ;
				     buff1*=iDgt_size(mode1) ;  }
	  if(type2==_DGT_VAL  ||
	     type2==_DGT_AREA ||
	     type2==_DGT_PTR    ) {  size2*=iDgt_size(mode2) ;
				     buff2*=iDgt_size(mode2) ;  }

				    P_TYPE=_CHR_AREA ;              /* ���.��� ���������� */

	     if(addr1==NULL &&
		addr2==NULL   ) {                                   /* ���� ��� NULL-������ ... */
					   P_ADDR=NULL ;            /* ������ ��������� - NULL-������ */
				}
	     else               {                                   /* ���� ���� ���������� ������ ... */
		       P_SIZE=size1+size2 ;                         /*   �����.������ ���������� */
		       P_BUFF=P_SIZE ;                              /*   �����.������ ������ ���������� */
			 addr=(unsigned char *)                     /*   �������� ������ */
                               iAlloc_var(_WORK_VAR, P_RESULT, 0) ;
		      if(mError_code)  return(0) ;                  /*   ��������� ���������� ������ */

		   memcpy(        P_ADDR,       addr1, size1) ;     /*   ��������� ��������� */
		   memcpy((char *)P_ADDR+size1, addr2, size2) ;
				}
				   }
/*------------------------------------------- �������� ������-������ */

   else if(oper_type==_CC_TYPE) {

			       buff1=O1_BUFF ;                      /* ��������� ������� ������� ����� */
			       buff2=O2_BUFF ;
	    if(addr1!=NULL)    size1=O1_SIZE ;                      /* ������ �������������   */
	    else               size1= 0 ;                           /*   ����� NULL-��������� */
	    if(addr2!=NULL)    size2=O2_SIZE ;
	    else               size2= 0 ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - �������� ������� */
	  if(oper_code==_SUB_OPER) {
				       value=addr1-addr2 ;          /* ��������� �������� ������� */
				      P_TYPE=_DGT_VAL ;             /* ���.��� ���������� */
				      P_ADDR=&value ;               /* ���.����� '���������' */
				      P_SIZE=  1 ;                  /* ���.������ ���������� */
				      P_BUFF=  1 ;                  /* ���.������ ������ ���������� */

			 addr=(unsigned char *)                     /* �������� ������ ��� ��������� */
                               iAlloc_var(_WORK_VAR, P_RESULT, 1) ;
		      if(mError_code)  return(0) ;                  /* ��������� ���������� ������ */
				   }
/*- - - - - - - - - - - - - - - - - - - - - - - - -  ��������� ����� */
     else if(oper_code>=_LT_OPER &&
	     oper_code<=_GT_OPER   ) {

	   if(size1>size2)   status= 4 ;                            /* ���������� '�����������' �����   */
      else if(size1<size2)   status= 1 ;                            /*  ������ �� : 1) �� �����         */
      else                 {                                        /*              2) �������          */
	      if(size1)    {                                        /*              3) �������          */
	                       status=memcmp(addr1, addr2, size1) ;
	                    if(status>0)  status=4 ;
                       else if(status<0)  status=1 ;
                       else               status=2 ; 
                           }                                        /*   � ��������� ������ ����������� */
	 else if(addr1!=NULL &&                                     /*    ������ �������(NULL) �        */
		 addr2==NULL   )  status= 4 ;                       /*     ���������� ������            */
	 else if(addr1==NULL &&
		 addr2!=NULL   )  status= 1 ;
	 else                     status= 2 ;
			   }

	    nIf_ind=status & if_mask[oper_code-_LT_OPER] ;          /* �����.��������� ��������� */
	   nDgt_val=nIf_ind ;

			 P_TYPE=_DGT_VAL ;                          /* ���.���, ������ �      */
			 P_ADDR=&nDgt_val ;                         /*  '��������' ���������� */
			 P_SIZE= 1 ;
			 P_BUFF= 1 ;

		addr=(unsigned char *)                              /* ��������� ��������� */
                      iAlloc_var(_WORK_VAR, P_RESULT, 1) ;
	     if(mError_code)  return(0) ;                           /* ��������� ���������� ������ */
				     }
/*- - - - - - - - - - - - - - - - - - - - - - - - �������� ��������� */
     else if(oper_code==_INV_OPER) {
				      P_TYPE=_CHR_PTR ;             /* ���.��� ���������� */
                     if(addr1==NULL)  P_ADDR=(void *)2L ;           /* ���. '���������' ����� */
		     else             P_ADDR= NULL ;
				      P_SIZE=  0  ;                 /* ���.������ ���������� */
				      P_BUFF=  0  ;                 /* ���.������ ������ ���������� */
				   }
/*- - - - - - - - - - - - - - - - - - - - - - - ����������� �������� */
     else              mError_code=_DCLE_BAD_OPERATION ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
				}
/*-------------------------------------------- �������� ������-����� */

   else if(oper_type==_CD_TYPE) {
		if(type1==_DGT_VAL) {    tmp_v =operand1 ;          /* �������� ������� ��������� */
				       operand1=operand2 ;          /*    � ������� ������-�����  */
				       operand2= tmp_v  ;
					 tmp_i =mode2 ;
					  mode2=mode1 ;
					  mode1= tmp_i ;    }

				 size1=O1_SIZE ;
				 buff1=O1_BUFF ;
				  dgt2=iDgt_get(addr2, mode2) ;     /* ��������� �������� 2-�� �������� */
/*- - - - - - - - - - - - - - - - - - - - - - ����� ��������� ������ */
	  if(oper_code==_ADD_OPER ||
	     oper_code==_SUB_OPER   ) {
					 cnt=(int)(dgt2+_Q_SHIFT) ; /* ��������� ����� */
	       if(oper_code==_SUB_OPER)  cnt=-cnt ;                 /* �������� ��������� � �������� */

				  P_TYPE=_CHR_PTR ;                 /* ���.��� ���������� */

	     if(addr1==NULL)      P_ADDR=NULL ;                      /* ���� NULL-������ ... */
	     else            {                                       /* ���� ���������� ������ ... */
				  P_ADDR=addr1+cnt ;                 /*   �����.�������������� ��������� */
		  if(size1>=cnt)  P_SIZE=size1-cnt ;                 /*   �����.�������������� ������ */
		  else            P_SIZE= 0 ;
		  if(buff1>=cnt)  P_BUFF=buff1-cnt ;                 /*   �����. ������ ������ */
		  else            P_BUFF= 0 ;

			     }
				      }
/*- - - - - - - - - - - - - - - - - - - - - - -  ������������ ������ */
     else if(oper_code==_MUL_OPER) {

				      cnt=(int)(dgt2+_Q_SHIFT) ;    /* ��� ������� ����������� */
				   P_TYPE=_CHR_AREA ;               /* ���.��� ���������� */

	      if( cnt <= 0 ||                                       /* �����.�������� ����� ������� <=0 */
		 addr1==NULL )   P_ADDR=NULL ;                      /*  ��� ����������� ������ - NULL   */
	      else             {                                    /* ���� �������� ��������� ... */
		       P_SIZE=size1*cnt ;                           /*   �����.������ ���������� */
		       P_BUFF=size1*cnt ;
		       P_ADDR=addr1 ;                               /*   ���.�������� ������� '�������' */
			 addr=(unsigned char *)                     /*   �������� ������ */
                                iAlloc_var(_WORK_VAR, P_RESULT, 0) ;
		      if(mError_code)  return(0) ;                  /*   �����.���������� ������ */

		  for(i=0 ; i<cnt ; i++)                            /*   ��������� ��������� */
			  memcpy(addr+size1*i, addr1, size1) ;
			       }
				   }
/*- - - - - - - - - - - - - - - - - - - - - - - ����������� �������� */
     else              mError_code=_DCLE_BAD_OPERATION ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
				}
/*------------------------------------------- �������� ������-������ */

   else if(oper_type==_AA_TYPE) {

/*- - - - - - - - - - - - - - - - - - - - - - - - - �������� ������� */
	  if(oper_code==_SUB_OPER) {
		       value=(addr1-addr2)/iDgt_size(mode1) ;       /* ��������� �������� �������    */
								    /*   � �������� ������� �������� */
		      P_TYPE=_DGT_VAL ;                             /* ���.��� ���������� */
		      P_ADDR=&value ;                               /* ���.����� '���������' */
		      P_SIZE=  1 ;                                  /* ���.������ ���������� */
		      P_BUFF=  1 ;

			addr=(unsigned char *)                      /* �������� ������ ��� ��������� */
                               iAlloc_var(_WORK_VAR, P_RESULT, 1) ;
		     if(mError_code)  return(0) ;                   /* ��������� ���������� ������ */
				   }
/*- - - - - - - - - - - - - - - - - - - - - - - ����������� �������� */
     else              mError_code=_DCLE_BAD_OPERATION ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
				}
/*-------------------------------------------- �������� ������-����� */

   else if(oper_type==_AD_TYPE) {
		if(type1==_DGT_VAL) {    tmp_v =operand1 ;          /* �������� ������� ��������� */
				       operand1=operand2 ;          /*    � ������� ������-�����  */
				       operand2= tmp_v  ;
					 tmp_i =mode2 ;
					  mode2=mode1 ;
					  mode1= tmp_i ;    }

				size1=O1_SIZE ;
				buff1=O1_BUFF ;
				 dgt2=iDgt_get(addr2, mode2) ;      /* ��������� �������� 2-�� �������� */
/*- - - - - - - - - - - - - - - - - - - - - - ����� ��������� ������ */
	  if(oper_code==_ADD_OPER ||
	     oper_code==_SUB_OPER   ) {
					 cnt=(int)(dgt2+_Q_SHIFT) ; /* ��������� ����� */
	       if(oper_code==_SUB_OPER)  cnt=-cnt ;                 /* �������� ��������� � �������� */

				P_TYPE=_DGT_PTR | mode1 ;           /* ���.��� ���������� */

	     if(addr1==NULL)    P_ADDR=NULL ;                       /* ���� NULL-������ ... */
	     else            {                                      /* ���� ���������� ������ ... */
				P_ADDR=addr1+cnt*iDgt_size(mode1) ; /*   �����.�������������� ��������� */				     
		if(size1>=cnt)  P_SIZE=size1-cnt ;                  /*   �����.�������������� ������ */
		else            P_SIZE= 0 ;
		if(buff1>=cnt)  P_BUFF=buff1-cnt ;                  /*   �����. ������ ������ */
		else            P_BUFF= 0 ;
			     }
				      }
/*- - - - - - - - - - - - - - - - - - - - - - - ����������� �������� */
     else              mError_code=_DCLE_BAD_OPERATION ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
				}
/*--------------------------------------------- �������� �����-����� */

   else if(oper_type==_DD_TYPE) {
				    P_TYPE=_DGT_VAL ;               /* ���.��� � ������ ���������� */
				    P_SIZE=  1 ;
				    P_BUFF=  1 ;
				    P_ADDR=&nDgt_val ;              /* ���.���������' ���������� */

	       if(addr1==NULL ||                                    /* ��������� ������� NULL-������� */
		  addr2==NULL   ) {  P_ADDR=NULL ;  return(0) ;  }  /*    ���������                   */

				   dgt1=iDgt_get(addr1, mode1) ;    /* ��������� ����� ��������� */
				   dgt2=iDgt_get(addr2, mode2) ;
				   int1=(int)(dgt1+_Q_SHIFT) ;      /* ��������� ������������� �������� */
				   int2=(int)(dgt2+_Q_SHIFT) ;
/*- - - - - - - - - - - - - - - - - - - - -  �������������� �������� */
	  if(oper_code==_NEG_OPER)   nDgt_val=-dgt1 ;
     else if(oper_code==_INC_OPER)   nDgt_val= dgt1+1 ;
     else if(oper_code==_DEC_OPER)   nDgt_val= dgt1-1 ;
     else if(oper_code==_ADD_OPER)   nDgt_val= dgt1+dgt2 ;
     else if(oper_code==_SUB_OPER)   nDgt_val= dgt1-dgt2 ;
     else if(oper_code==_MUL_OPER)   nDgt_val= dgt1*dgt2 ;
     else if(oper_code==_DEV_OPER) {
		       if(dgt2!=0.)  nDgt_val=dgt1/dgt2 ;
		       else          nDgt_val=  0. ;
				   }
     else if(oper_code==_POW_OPER) {
		       if(dgt1<=0.)  nDgt_val= 0. ;
		       else          nDgt_val=pow(dgt1,dgt2) ;
				   }
     else if(oper_code==_AOR_OPER)   nDgt_val=int1|int2 ;
/*- - - - - - - - - - - - - - - - - - - - - - - - -  ��������� ����� */
     else if(oper_code>=_LT_OPER &&
	     oper_code<=_GT_OPER   ) {

	   if(dgt1>dgt2)   status = 4 ;                             /* ���������� '�����������' ����� */
      else if(dgt1<dgt2)   status = 1 ;
      else                 status = 2 ;

	    nIf_ind=status & if_mask[oper_code-_LT_OPER] ;          /* �����.��������� ��������� */
	   nDgt_val=nIf_ind ;
				     }
/*- - - - - - - - - - - - - - - - - - - - - - -  ���������� �������� */
     else if(oper_code>= _OR_OPER &&
	     oper_code<=_INV_OPER   ) {

	if(oper_code== _OR_OPER) if(int1 || int2)  nDgt_val=1. ;    /* �������� '���������� ���' */
				 else              nDgt_val=0. ;

	if(oper_code==_AND_OPER) if(int1 && int2)  nDgt_val=1. ;    /* �������� '���������� ���' */
				 else              nDgt_val=0. ;

	if(oper_code==_INV_OPER) if(int1        )  nDgt_val=0. ;    /* �������� '���������� ��' */
				 else              nDgt_val=1. ;
				      }
/*- - - - - - - - - - - - - - - - - - - - - - - ����������� �������� */
     else              mError_code=_DCLE_BAD_OPERATION ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
   if(!mError_code) {
    if(oper_code==_INC_OPER || oper_code==_DEC_OPER)                /* ��� �������� ++ � -- ���������       */
		    iDgt_set(nDgt_val, addr1, mode1) ;              /*  ����������� � '���������', ���      */
                                                                    /*   ��������� � ������ ������� ������� */
                  iAlloc_var(_WORK_VAR, P_RESULT, 1) ; 

		       if(mError_code)  return(0) ;                 /* ��������� ���������� ������ */
		    }

			     nIf_ind=(int)(nDgt_val+_Q_SHIFT) ;     /* ���.��������� ������� */

				}
/*---------------------------------------- ����������� ���� �������� */

   else                         {
				   mError_code=_DCLE_BAD_OPERATION ;
				}
/*-------------------------------------------------------------------*/

  return(0) ;
}


/*********************************************************************/
/*                                                                   */
/*               ���������� ���������� ����������                    */

typedef struct {
		  Dcl_decl *pars_table[_PARS_MAX] ;  /* ������� ���������� */
		       int  pars_cnt ;               /* ������� ���������� �������� ������ */
		       int  allpars_cnt ;            /* ��������� ������� ���������� */
		  Dcl_decl *transit ;                /* ������� ���������� ���������� */
		       int  transit_cnt ;            /* ����� ���������� ���������� */
		       int  transit_num ;            /* ����� ����������� ��������� */
		  Dcl_decl  tmp ;                    /* ������-������ */
		  Dcl_decl  result ;                 /* �������� ���������� */
		       int  result_dst ;             /* ������ ���������� ����������: */
		       int  if_ind ;                 /* ��������� ������� */
		    double  dgt_val ;                /* �������� �������� */
		      char *chr_pnt ;                /* '������' �������� ������ */
		       int  work_cnt ;               /* ������� ������� ���������� */
		       int  fragm_flag ;             /* ���� ���������� � ������������� �������� */
#ifdef  _EXT_LIBS
		  Dcl_decl *ext_list[_LOAD_MAX] ;    /* ������ ���������� ������������ �������� */
		       int  ext_cnt ;                /* ������� ������������ �������� */
#endif
               } Dcl_cstack ;


   int  Lang_DCL::iCulc_stack(int  oper_code)

{
   Dcl_cstack *addr ;

  static Dcl_cstack *cstack[_STACK_MAX] ;
  static        int  cstack_cnt ;

/*--------------------------------------------- ������ ������ � ���� */

  if(oper_code==_DCL_PUSH) {
/*- - - - - - - - - - - - - - - - - - - - - - - - - ���������� ����� */
     if(cstack_cnt==_STACK_MAX) {  mError_code=_DCLE_STACK_OVR ;    /* ���� ���� ��������... */
						return(-1) ;      }

	   addr=(Dcl_cstack *)calloc(1, sizeof(*addr)) ;            /* �������� ������ ��� ���� */	                      
	if(addr==NULL) {  mError_code=_DCLE_STACK_MEM ;             /* ���� ������ ���... */
				 return(-1) ;           }

	       cstack[cstack_cnt]=addr ;                            /* ������� ����� ������ � ���� */
		      cstack_cnt++    ;
/*- - - - - - - - - - - - - - - - - - - - -  ��������� ������ � ���� */
   memcpy(addr->pars_table, nPars_table, sizeof(nPars_table)) ;
	  addr->pars_cnt   =nPars_cnt ;
	  addr->allpars_cnt=nPars_cnt_all ;
	  addr->transit    =nTransit ;
	  addr->transit_cnt=nTransit_cnt ;
	  addr->transit_num=nTransit_num ;
	  addr->tmp        =nTmp ;
	  addr->result     =nResult ;
	  addr->result_dst =nResult_dst ;
	  addr->if_ind     =nIf_ind ;
	  addr->dgt_val    =nDgt_val ;
          addr->chr_pnt    =nChr_pnt ;
          addr->work_cnt   =nWork_cnt ;
          addr->fragm_flag =nFragm_flag ;
#ifdef  _EXT_LIBS
   memcpy(addr->ext_list, dcl_ext_list, sizeof(dcl_ext_list)) ;
	  addr->ext_cnt    =dcl_ext_cnt ;
#endif
/*- - - - - - - - - - - - - - - - - - - - - - - ������������� ������ */
	       nPars_cnt =  0 ;
	   nPars_cnt_all =  0 ;
	     nTransit_num=  0 ;
	     nResult_dst =  0 ;
	         nIf_ind =  0 ;
	       nWork_cnt =  0 ;
	      nFragm_flag=  0 ;
#ifdef  _EXT_LIBS
	     dcl_ext_cnt =  0 ;
#endif
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
		           }
/*--------------------------------------- ���������� ������ �� ����� */

  if(oper_code==_DCL_POP ) {
/*- - - - - - - - - - - - - - - - - - - - - - - - - ���������� ����� */
	 if(cstack_cnt<=0) {  mError_code=_DCLE_STACK_UND ;         /* ���� ���� ����... */
					    return(-1) ;        }

		       cstack_cnt-- ;
	   addr=cstack[cstack_cnt] ;                                /* ��������� ����� ������ �� ���� */
/*- - - - - - - - - - - - - - - - - - �������������� ������ �� ����� */
   memcpy(nPars_table,  addr->pars_table, sizeof(nPars_table)) ;
	  nPars_cnt    =addr->pars_cnt    ;
	  nPars_cnt_all=addr->allpars_cnt ;
	  nTransit     =addr->transit     ;
	  nTransit_cnt =addr->transit_cnt ;
	  nTransit_num =addr->transit_num ;
	  nTmp         =addr->tmp         ;
	  nResult      =addr->result      ;
	  nResult_dst  =addr->result_dst  ;
	  nIf_ind      =addr->if_ind      ;
	  nDgt_val     =addr->dgt_val     ;
	  nChr_pnt     =addr->chr_pnt     ;
	  nWork_cnt    =addr->work_cnt     ;
	  nFragm_flag  =addr->fragm_flag  ;
#ifdef  _EXT_LIBS
   memcpy(dcl_ext_list, addr->ext_list, sizeof(dcl_ext_list)) ;
	  dcl_ext_cnt  =addr->ext_cnt     ;
#endif
/*- - - - - - - - - - - - - - - - - - - - - - -  ������������ ������ */
                                 free(addr) ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
		           }
/*-------------------------------------------------------------------*/

  return(0) ;
}

