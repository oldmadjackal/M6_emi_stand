/*   Bolotov P.D.  version  24.06.1993   */
/*     2 warnings at warning level 1     */

#include  <stddef.h>
#include  <stdlib.h>
#include  <string.h>
#include  <ctype.h>
#include  <math.h>
#include  <malloc.h>


#define  _CCL_1_MAIN
#include "lang_std.h"
#include "ccl_1.h"

#pragma warning(disable : 4244)
#pragma warning(disable : 4267)
#pragma warning(disable : 4996)


/*********************************************************************/
/*                                                                   */
/*             ����������� ��������������� �������                   */
/*                                                                   */
/*********************************************************************/


/*********************************************************************/
/*                                                                   */
/*                      � � � � � � � � !!!                          */
/*                                                                   */
/*    ��������������� ����������� ��� ��������� :                    */
/*       ����� �������� ������      - 128   _LEN_MAX                 */
/*       �����                      -  20   _GOTO_MAX                */
/*       GOTO ���������             -  20   _GOTO_MAX                */
/*       IF-DO �����������          -  20   _IFDO_MAX                */
/*       ���������� ����������      - 100   _INT_VAR_MAX             */
/*       ������� ����������         - 100   _WRK_VAR_MAX             */
/*       ���������� � ���������     -  50   _LINE_VARS_MAX           */
/*       ���������� �������         -  20   _PARS_MAX                */
/*       �������� �������� � ������ -  20   _TEXT_MAX                */
/*       �������� � ������          -  20   _INDEX_MAX               */
/*                                                                   */
/*  ��������������� ����������� ���� ����.                           */
/*                                                                   */
/*********************************************************************/

/*---------------------------------------------- ��������� ��������� */

//#define  __INSIDE_DEBUGER__

#define  _LEN_MAX            128
#define  _GOTO_MAX            20
#define  _IFDO_MAX            20
#define  _INT_VAR_MAX        100       /* �� ����� 255 */
#define  _WRK_VAR_MAX        100       /* �� ����� 255 */
#define  _LINE_VARS_MAX       50
#define  _PARS_MAX            20
#define  _INDEX_MAX           20
#define  _TEXT_MAX            20
#define  _TEXT_BUFF    (SYS_WRK_MAX*sizeof(double)/2)

/*--------------------------------------- ������� ����� � ���������� */

#define  _QUIT  -1

/*--------------------------------------- �������� ������������ ���� */

#define  _ENTRY_MAX   10
 static    struct L_entry  entry_list[_ENTRY_MAX] ;   /* ������ ������ */
 static               int  entry_cnt ;
 static               int  entry_max=_ENTRY_MAX ;


 struct L_control {                     /* �������� ���.���� */
		      int  oper_code ;   /* ��� �������� */
		   double *operand ;     /* ����� �������� */
		      int  addr ;        /* ����� ���� */
		      int  text_row ;    /* ����� ��������� ������ */
		  } ;

#define     _NOP_OPER  0
#define     _SET_OPER  1
#define    _CULC_OPER  2
#define      _IF_OPER  3
#define  _RETURN_OPER  4
#define   _ALLOC_OPER  5

/*------------------------------------ �������� ��������������� ���� */

 struct L_culc {
		      int  oper_code ;  /* ��� �������� */
		   double *addr1 ;      /* ����� 1-�� �������� */
		   double *addr2 ;      /* ����� 2-�� �������� */
	       } ;

 struct L_oper {                      /* �������� �������� */
		     char *name ;       /* ��� �������� */
		      int  len ;        /* ����� ����� �������� */
		      int  code ;       /* ��� �������� */
		      int  pref_flag ;  /* ���� ���������� �������� */
	       } ;


#define _CULC_FRAME   sizeof(struct L_culc)

				/* ��� ������� ��������:        */
#define   _PNTR_OP1      0      /*   ������ ����� ��������      */
#define   _DATA_OP1      1      /*   ���������������� ������    */
#define   _SHFT_OP1      2      /*   ������=����+����������     */
#define   _LONG_OP1      4      /*   ������� ������ �� �������� */

#define   _MOVE_OPER  0010      /* ��� �������� �/� ������� 8 !!! */
#define   _PUSH_OPER  0020
#define   _CALL_OPER  0030
#define  _CALLX_OPER  0040
#define  _CALLE_OPER  0050
#define  _POWER_OPER  0060
#define   _MULT_OPER  0070
#define    _DEV_OPER  0100
#define   _DEVC_OPER  0110
#define   _DEVQ_OPER  0120
#define    _SUB_OPER  0130
#define    _ADD_OPER  0140
#define     _EQ_OPER  0150
#define     _NE_OPER  0160
#define     _LE_OPER  0170
#define     _GE_OPER  0200
#define     _LT_OPER  0210
#define     _GT_OPER  0220
#define    _AND_OPER  0230
#define     _OR_OPER  0240
#define   _TSEG_OPER  0250     /* ���������� ������� ������ */
#define   _IND1_OPER  0260     /* ���������� ������ 1-�� �������� */
#define   _IND2_OPER  0270     /* ���������� ������ 2-�� �������� */
#define   _STOP_OPER  0770

#define  _OPER_MAX  16
 static  struct L_oper lng_oper[_OPER_MAX]={ "^",  1, _POWER_OPER, 0, /* �������� ���������� �������� */
					     "%%", 2,  _DEVC_OPER, 0,
					     "%",  1,  _DEVQ_OPER, 0,
					     "/",  1,   _DEV_OPER, 0, /* ��������! ��������� ������� ������ */
					     "*",  1,  _MULT_OPER, 0, /*   ���� ������ ���������� ��������� */
					     "-",  1,   _SUB_OPER, 1, /* ��������! ��������� ��������� ������ */
					     "+",  1,   _ADD_OPER, 0, /*      ���� ������ ���������� �������� */
					     "==", 2,    _EQ_OPER, 0,
					     "!=", 2,    _NE_OPER, 0,
					     "<=", 2,    _LE_OPER, 0,
					     ">=", 2,    _GE_OPER, 0,
					     "<",  1,    _LT_OPER, 0,
					     ">",  1,    _GT_OPER, 0,
					     "&&", 2,   _AND_OPER, 0,
					     "||", 2,    _OR_OPER, 0,
					     "=",  1,  _MOVE_OPER, 0  } ;

/*------------------------------------ �������� ��������������� ���� */
/*                                                                   */
/*      {TNI}  -   T - ��� �������                                   */
/*                 N - '�����' �������                               */
/*                 I - (>0) ����� ������������� ������               */
/*                                                                   */
/*   ��� ������� :                                                   */
/*         F - �������                -  vars_func                   */
/*         P - ��������� �������      -  lng_table, vars_tbl         */
/*         R - ENTRY-����             -  entry_list                  */
/*         E - ������� ����������     -  lng_vars                    */
/*         T - ��������� ����������   -  lng_table, vars_tbl         */
/*         I - ���������� ����������  -  SYS_INT_VARS                */
/*         W - ������� ����������     -  vars_work                   */
/*         C - ���������              -  vars_cnst                   */
/*         Z - ����                                                  */

 static char  func[6]={'{', 'F', '0', '0', '}', 0} ;    /* ������������ ������� */
 static char  rslt[6]={'{', 'W', '0', '0', '}', 0} ;    /* ������������ ������� ������ */
 static char  cnst[6]={'{', 'C', '0', '0', '}', 0} ;    /* ������������ �������� ��������� */
 static char  ltrl[6]={'{', 'L', '0', '0', '}', 0} ;    /* ������������ ���������� ��������� */
 static char  zero[6]={'{', 'Z', '0', '0', '}', 0} ;    /* ������������ ���� */

 static char *index_list[_INDEX_MAX] ;     /* ��������� ������ */
 static  int  index_cnt  ;                 /* ������� �������� */
 static char  index_buff[_INDEX_MAX*6] ;   /* ����� ���������� ������ */

/*------------------------- ���������� ����������� ������������ ���� */

 struct L_marker {                           /* �������� ����� � ��������� */
		   char  name[_NAME_MAX+1] ;  /* ��� ����� */
		    int  row ;                /* ������ ������ ��� ���� */
		 } ;

 struct L_if_do {                       /* �������� �������� � �������� ���������� */
		    int  if_do_flag ;    /* ���� ��������� ��� ��������� ��������� */
		    int  else_flag ;     /* ���� ELSE-��������� */
		    int  block_flag ;    /* ���� ������ � ������� ���������� */
		    int  addr ;          /* ������� ����� */
		} ;

 static   struct L_marker  mark[_GOTO_MAX] ;    /* �������� ����� */
 static               int  mark_cnt ;           /* ������� ����� */
 static   struct L_marker  dir[_GOTO_MAX] ;     /* �������� ��������� */
 static               int  dir_cnt ;            /* ������� ��������� */

 static    struct L_if_do  id_stk[_IFDO_MAX] ;  /* ���� IF-DO ���������� */
 static               int  id_stk_cnt ;         /* ��������� ����� IF-DO ���������� */
 static               int  id_num ;             /* ��������� "�������� �����" IF_DO ����� */

 static              char  buff[_LEN_MAX] ;     /* ������� ����� */
 static               int  row ;                /* ����� ������ ��������� ������ */
 static               int  block_flag ;         /* ���� ������ ������ IF-DO ����� */
 static               int  any_flag ;           /* ���� ������� ����� ���������� ����� IF-ELSE � DO */
 static               int  else_flag ;          /* ���� �������� ELSE-��������� */

/*---------------------- ���������� ����������� ��������������� ���� */

 static            double  shft_base ;                /* ���� ��� ��������� ���� _SHIFT_OP2 */
 static              char *text_buff ;                /* ������� ����� �������� �������� */
 static               int  text_addr[_TEXT_MAX] ;     /* ������� '�������' �������� �������� */
 static               int  text_cnt ;                 /* ������� �������� �������� */
 static               int  text_size ;                /* ������� ���������� ��������� ������ */
 static               int  text_flag ;                /* ���� ������ �� ��������� ����������� */
 static              char  extrn_name[80] ;           /* ��� �������� ������� */
#define    _TEXT_BASE  sizeof(struct L_culc)

 static               int  func_flag ;        /* ���� ����������� ������� */
 static               int  func_type ;        /* ��� �������: F, P, R */
 static               int  func_num ;         /* ����������������� ����� ������� */
 static               int  func_std ;         /* �������� ������� */
 static               int  func_pars_cnt ;    /* ����� ���������� ������� */

/*----------------------------------------------------- ������ ����� */

 static  struct L_control *ctrl_code ;         /* ����������� ��������� */
 static               int  ctrl_code_size ;    /* ������ ������ ������������ ���������� */
 static               int  ctrl_code_cnt ;     /* ������� ������ ������������ ���������� */
 static               int  ctrl_code_max ;     /* ����.����� ������ ������������ ���������� */
 static     struct L_culc *culc_code ;         /* ������� ��������� */
 static               int  culc_code_size ;    /* ������ ������ ��������������� ���������� */
 static               int  culc_code_cnt ;     /* ������� ������ �������� ���������� */
 static               int  culc_code_max ;     /* ����.����� ������ �������� ���������� */

/*-------------------------------------- ������� ���������� �������� */

 static struct L_variable  vars_int[_INT_VAR_MAX] ;   /* �������� ���������� ���������� */
 static               int  vars_int_cnt ;             /* ������� ���������� ���������� */
 static            double  vars_work[_WRK_VAR_MAX] ;  /* ������ ������� ����� */
 static               int  vars_work_cnt ;            /* ������� ������� ����� */
 static            double *vars_cnst ;                /* ������ �������� */
 static               int  vars_cnst_cnt ;            /* ������� �������� */
 static            double  vars_tmp ;                 /* ���������� ���������� */
 static              void *vars_tbl[_LINE_VARS_MAX] ; /* ������ ��������� ����������/������� */
 static               int  vars_tbl_cnt ;             /* ������� ������ ��������� ����������/������� */

 static            double  vars_stack[_PARS_MAX] ;    /* ������� ���� ���������� */
 static               int  vars_stack_cnt ;           /* ������� �������� ����� */

 static            double *vars_iarea ;               /* ����� ���������� ���������� �������� */
 static               int  vars_iarea_size ;          /* ������� ������ ���������� �������� */
 static               int  vars_iarea_cnt ;           /* ������� ���������� ������ ���������� �������� */

/*------------------------------------- ���������� �������� �������� */

	 struct L_module *lng_system_work ;  /* �������� �������� ������������ ������ */
	 struct L_module *lng_system ;       /* �������� ������������ ������ */
 static  struct L_module  system_default ;   /* ... �� ��������� */

#define  SYS_CTRL_CODE  ((struct L_control *)lng_system_work->ctrl_buff)
#define  SYS_CTRL_SIZE   lng_system_work->ctrl_size
#define  SYS_CTRL_MAX   (lng_system_work->ctrl_size/sizeof(struct L_control))
#define  SYS_CTRL_CNT    lng_system_work->ctrl_cnt
#define  SYS_CTRL_USED   lng_system_work->ctrl_used

#define  SYS_CULC_CODE  ((struct L_culc *)lng_system_work->culc_buff)
#define  SYS_CULC_SIZE   lng_system_work->culc_size
#define  SYS_CULC_MAX   (lng_system_work->culc_size/sizeof(struct L_culc))
#define  SYS_CULC_CNT    lng_system_work->culc_cnt
#define  SYS_CULC_USED   lng_system_work->culc_used

#define  SYS_INT_VARS    lng_system_work->int_vars
#define  SYS_INT_MAX     lng_system_work->int_vars_max
#define  SYS_INT_CNT     lng_system_work->int_vars_cnt

#define  SYS_WRK_VARS    lng_system_work->work_vars
#define  SYS_WRK_MAX     lng_system_work->work_vars_max
#define  SYS_WRK_CNT     lng_system_work->work_vars_cnt

#define  SYS_ENTRY_LIST  lng_system_work->entry_list
#define  SYS_ENTRY_MAX   lng_system_work->entry_max
#define  SYS_ENTRY_CNT   lng_system_work->entry_cnt

#define  SYS_EXT_VARS    lng_system_work->ext_vars
#define  SYS_EXT_VCNT    lng_system_work->ext_vars_cnt
#define  SYS_EXT_FUNC    lng_system_work->ext_func
#define  SYS_EXT_FCNT    lng_system_work->ext_func_cnt
#define  SYS_EXT_TABLE   lng_system_work->ext_table

#define  SYS_GET_TEXT   (lng_system_work->get_text)
#define  SYS_TEST_NAME  (lng_system_work->test_name)
#define  SYS_EXT_LIB    (lng_system_work->ext_lib)
#define  SYS_EXT_EXEC   (lng_system_work->ext_exec)

/*-------------------------------------------------- ��������� ����� */

#define   LNG_CHAR(c)    (isalnum(c) || c=='_' || c=='$' || c=='[' || c==']')
#define   LNG_NAME(c)    (isalnum(c) || c=='_' || c=='$')

/*-------------------------------------------- ���������� ���������� */

 static               int  lng_start ;       /* ��������� ����� */

 static            double  vars_if_ind ;     /* ��������� �������� IF */
 static            double  vars_go_ind ;     /* ��������� �������� GOTO */

/*---------------------------------------------------- ��������� �/� */

   char *L_find_close(char *) ;              /* ����� ����������� ������ */
 double  L_call_entry(int, double *, int) ;  /* ����� ����������� ����� */
   void  L_debug_ctrl(int) ;                 /* ������� ������������ ���� */


/************************************************************************/
/************************************************************************/
/*                                                                      */
/*                 ���������� � �������� ���������                      */
/*                                                                      */
/************************************************************************/
/************************************************************************/

/************************************************************************/
/*                                                                      */
/*                            ���������� ������                         */

  int  L_compile(system)

    struct L_module *system ;  /* �������� �������� */

{
  int  status ;


       lng_system=system ;

	   status=L_ctrl_maker() ;

  return(status) ;
}


/************************************************************************/
/*                                                                      */
/*                    ���������� ������                                 */

  double  L_execute(system, entry, pars, pars_cnt)

    struct L_module *system ;    /* �������� �������� */
	       char *entry ;     /* ��� ����� */
	     double *pars ;      /* ������� ���������� */
		int  pars_cnt ;  /* ����� ���������� */

{
  double  value ;
     int  status ;


       lng_system_work=system ;

    if(entry!=NULL) {
		      status=L_decl_entry(entry, pars, pars_cnt) ;
		   if(status) {  lng_error=_EL_UNK_ENTRY ;
					 return(0.) ;       }
		    }

	   value=L_ctrl_exec() ;

  return(value) ;
}


/************************************************************************/
/*                                                                      */
/*                ������� ������� �������                               */

     int  L_ctrl_buff(addr, size)

      char *addr ;  /* ����� ������ */
       int  size ;  /* ������� ������, ���� */

{
     ctrl_code     =(struct L_control *)addr ;
     ctrl_code_size=size ;
     ctrl_code_max =size/sizeof(ctrl_code[0]) ;

 return(0) ;
}


     int  L_culc_buff(addr, size)

      char *addr ;  /* ����� ������ */
       int  size ;  /* ������� ������, ���� */

{
     culc_code    =(struct L_culc *)addr ;
     culc_code_max=size/sizeof(culc_code[0]) ;

 return(0) ;
}


     int  L_area_buff(addr, size)

      char *addr ;  /* ����� ������ */
       int  size ;  /* ������� ������, ���� */

{
     vars_iarea     =(double *)addr ;
     vars_iarea_size=size/sizeof(vars_iarea[0]) ;

 return(0) ;
}

/************************************************************************/
/*                                                                      */
/*              ������ ������ ������� ���������� ����������             */

    void *L_int_vars(name_cnt)

      int *name_cnt ;

{
   *name_cnt=vars_int_cnt ;
  return(vars_int) ;
}


/************************************************************************/
/************************************************************************/
/*                                                                      */
/*                       �������������� ����                            */
/*                                                                      */
/************************************************************************/
/************************************************************************/


/************************************************************************/
/*                                                                      */
/*                ���������� ������������ ����������                    */

  int  L_ctrl_maker(void)

{
       char  text_src[4096] ;     /* ������� ����� �������������� ������ */
       char *text ;
	int  string_rgm ;         /* ����� ������ �� �������� ���������� */
	int  mark_flag ;          /* ���� ������� � ����� ����� */
	int  ret_flag ;           /* ���� ������� � ���������� RETURN � ������ */
	int  close_flag ;
       char *subrow_beg[50] ;     /* ������ ���������� �� ������ �������� */
       char *subrow_end[50] ;     /* ������ ���������� �� ����� �������� */
	int  subrow_cnt ;         /* ������� �������� */
	int  subrow ;             /* ����� ������� ��������� */
       char  chr_save ;           /* ������ ���������� ������� */
       char *addr ;
       char *name ;
static char  do_slct[4]={'=', ';', ';', ')'} ;  /* "����� ��������" ��������� DO */
       char *do_addr[4] ;                       /* ������ "��������" ��������� DO */
//   double *var_addr ;
//      int  var_num ;
	int  base ;               /* ������� ����� */
	int  shift ;              /* �������� ������ */
	int  size ;
	int  len ;
	int  n ;
	int  i ;
	int  j ;

/*------------------------- ���������� ����������� �������� �������� */

	if(lng_system==NULL)  lng_system     =&system_default ;
			      lng_system_work= lng_system ;
/*- - - - - - - - - - - - - - - - ���������� �� '������' ����������� */
	       system_default.ctrl_buff    = ctrl_code ;
	       system_default.ctrl_size    = ctrl_code_size ;
	       system_default.culc_buff    = culc_code ;
	       system_default.culc_size    = culc_code_size ;

	       system_default.int_vars     = vars_int ;
	       system_default.int_vars_max =_INT_VAR_MAX ;
	       system_default.work_vars    = vars_work ;
	       system_default.work_vars_max=_WRK_VAR_MAX ;

	       system_default.entry_list   = entry_list ;
	       system_default.entry_max    = entry_max ;

	       system_default.ext_vars     =lng_vars ;
	       system_default.ext_vars_cnt =lng_vars_cnt ;
	       system_default.ext_func     =lng_func ;
	       system_default.ext_func_cnt =lng_func_cnt ;
	       system_default.ext_table    =lng_table ;

	       system_default.get_text     = L_get_text ;
	       system_default.test_name    = L_test_name ;
	       system_default.ext_lib      = lng_ext_lib ;
	       system_default.ext_exec     = lng_ext_exec ;

/*------------------------------------------------- ������� �������� */

    if(SYS_GET_TEXT==NULL) {   lng_error=_EL_GET_TEXT ;
				     return(_QUIT) ;     }
    if(SYS_EXT_FCNT > 255) {   lng_error=_EL_FUNC_OUT ;
				     return(_QUIT) ;     }
    if(SYS_EXT_FCNT > 255) {   lng_error=_EL_FUNC_OUT ;
				     return(_QUIT) ;     }

/*---------------------------------------------- ����� ������������� */

		vars_cnst=         SYS_WRK_VARS ;                   /* ��������� ������� �������� */
		text_buff=(char *)(SYS_WRK_VARS+SYS_WRK_MAX/2) ;    /*    � �������� ��������     */

		lng_error=0 ;
	     lng_warn_cnt=0 ;
		 mark_cnt=0 ;
		  dir_cnt=0 ;
	       id_stk_cnt=0 ;
	    SYS_ENTRY_CNT=0 ;
	     SYS_CTRL_CNT=0 ;
	     SYS_CULC_CNT=0 ;
	   vars_iarea_cnt=0 ;

	       close_flag=0 ;
	       block_flag=0 ;
		else_flag=0 ;
		 ret_flag=0 ;

    for(i=0 ; i<_INDEX_MAX ; i++) index_list[i]=&index_buff[i*6] ;

/*------------------------------------------ ���������� ������������ */

  for(row=1 ; ; row++) {                                            /* CIRCLE.1 - ��������� ����� �� ������� */
		      if(lng_error)  return(_QUIT) ;                /* "��������" ��������� ������ */

			  text=SYS_GET_TEXT() ;                     /* �������� ��������� ������ ������ */

		       if(text==NULL)  break ;                      /* ��� ����������� ����� ������ - */
								    /*    - ��������� ������          */
             memset(text_src, 0, sizeof(text_src)) ;                /* �������� ������� ������ � ������� ����� */
            strncpy(text_src, text, sizeof(text_src)-1) ;
               text=text_src ;

			 lng_row=row ;
		       mark_flag= 0 ;                               /* ����� ����� ����� */

/*-------------------------------------- ��������� �������� �������� */

	       text_flag=0 ;                                        /* ����� ����� ��������� �������� */
	       text_cnt =0 ;                                        /* ����� �������� ��������� �������� */
	       text_size=0 ;                                        /* ����� ������ ��������� �������� */
	      string_rgm=0 ;                                        /* ����.������ ��������� */

//           len=strlen(text) ;

     for(i=0 ; text[i] ; i++) {                                     /* CIRCLE.2 - ��������� ������ ... */
      if(text[i]=='"') {                                            /* ���� �������� ��������� ... */
/*- - - - - - - - - - - - - - - - - - - - - - -  '��������' �������� */
	 if(string_rgm==text[i]) {                                  /* IF.1 - ���� ��������� '�����������'... */

	     if(text[i-1]=='\\' && text[i-2]!='\\')  continue ;     /* �������� �� Escape-������ */

			       text[i]= 0 ;                         /* ����������� ��������� */

	  if(text_cnt>=_TEXT_MAX) {  lng_error=_EL_TEXT_MAX ;       /* �������� ������������ ������� */
					   return(_QUIT) ;      }   /*    �������� ��������          */

	 for(n=1, j=0 ; addr[n] ; n++, j++) {                      /* ������� � �������� ������������� */
	  if(addr[n]=='\\')  n+=L_escape_dcd(addr+n+1, addr+j) ;
	  else                       addr[j]=addr[n] ;
	       if(lng_error)  return(_QUIT) ;
					    }
			addr[j]=  0 ;                               /* ����������� ����������� ������ */
			     j++     ;
			     j+=(j&1) ;                             /* �������.�������� ������� */

	  if(text_size+j>=_TEXT_BUFF) {  lng_error=_EL_TEXT_MAX ;   /* �������� ������������ ������ */
					   return(_QUIT) ;        } /*    �������� ��������         */

		      text_addr[text_cnt]=_TEXT_BASE+text_size ;    /* ���.����� �������� �������� */

		    memmove(text_buff+text_size, addr, j) ;         /* �������� ������ � ����� */

			    ltrl[2]=text_cnt+1 ;                    /* ����.������.���������� ��������� */
		    memmove(addr+5, text+i+1, strlen(text+i+1)+1) ; /* ��������� ����� ��������� */
		    memmove(addr  , ltrl    ,  5     ) ;            /* ������.������.���������� ��������� */

				      shift =text-addr+i-1 ;        /* ���. ����� */
					  i-=shift ;                /* ����.�������� �������� */
//              			len-=shift ;                /* ����.����� ��������� */

				  string_rgm = 0  ;                 /* ����� ������ �������� ��������� */
				   text_size+=j ;                   /* �����.������� ������ �������� �������� */
				    text_cnt++ ;                    /* �����.������� �������� �������� */
				 }                                  /* ELSE.1 */
/*- - - - - - - - - - - - - - - - - - - - ����������� ����� �������� */
      else if(string_rgm==0)  {                                     /* ELSE.1 - ���� �����. '�����������'... */
				     text_flag=1 ;                  /* ���.����� ��������� �������� */
				    string_rgm=text[i] ;            /* ���.�����.����� */
					  addr=text+i ;             /* ��������� ����� ������ ��������� */
			      }                                     /* END.1 */
		       }
/*- - - - - - - - - - - - - - - - - - - - - -  ��������� ����������� */
      else             {
			  if( text[i  ]=='/' &&                     /* ���� ������� �����������          */
			      text[i+1]=='/' &&                     /*  �� ��������� ������� ��������� - */
			     string_rgm==  0    )  break ;          /*   ���������� ��������� ��������   */
		       }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
			      }                                     /* CONTINUE.2 */

       if(string_rgm!=0) {  lng_error=_EL_TEXT_OPEN ;               /* �������� ���������� ������ */
				  return(_QUIT) ;      }            /*  �� ��������� �����������  */

/*------------------------------- ���������� ������������ � �������� */

   for(i=0, j=0 ; text[j] ; j++) {                                  /* ��������� ������ */
		       if(text[j  ]=='/' &&                         /*   ��������� ����������� */
			  text[j+1]=='/'   )  break ;

		       if(text[j]==' ' ||                           /*   ���������� �������� � ��������� */
			  text[j]=='\t'  )  continue ;

                       if(text[j]<  0 ||
                          text[j]>127   ) { lng_error=_EL_CYR_CHAR; /*   ��� ����������� �������� ������� - */
                                                 return(_QUIT) ;   }/*      - ����� � �������               */

			      text[i]=text[j] ;                     /*   �������� ������ */
				   i++ ;
				 }

			  text[i]=0 ;                               /* ����������� ������������ ������ */

	       if(i>=_LEN_MAX) {  lng_error=_EL_LEN_MAX ;           /* ���� ������ ������� ������ - */
				       return(_QUIT) ;     }        /*    - ����� � �������         */

/*-------------------------------------------------- ��������� ����� */

	addr=strchr(text, ':') ;                                    /* ���� ����������� ����� */

     if(addr!=NULL) {                                               /* IF.1 - ���� ������������� ����� ... */
		       len=addr-text ;                              /* �����.����� ����� */

       for(i=0 ; i<len; i++)                                        /* ���������.������ �������� */
		if(!LNG_CHAR(text[i])) break ;                      /*   �������������� �����    */

      if(i==len)                                                    /* IF.2 - ���� ��� ��������. ����� ... */
	  if(len>_NAME_MAX) {                                       /* IF.3 - ���� ����� ������� ������� ... */
				lng_error=_EL_MARK_BAD ;            /*  ������������� ������ */
				    return(_QUIT) ;                 /*  ���������� ��������� */
			    }
	  else              {                                       /* ELSE.3 - ���� ����� ���������� ... */
				  text[len]=0 ;                     /*  ����������� ����� */

		for(i=0 ; i<mark_cnt ; i++)                         /*  ��������� ���������� ����� */
		    if(!strcmp(mark[i].name, text))  break ;        /*     � ����������� �������   */

		 if(i<mark_cnt) {  lng_error=_EL_MARK_DBL ;         /*  ��� ����������� ���������� ����� - */
					 return(_QUIT) ;    }       /*   - ���.������ � �������. ��������� */

	     if(mark_cnt==_GOTO_MAX-1) {  lng_error=_EL_GOTO_OVR ;  /* ���� ������� ����� ����� - */
					       return(_QUIT) ;   }  /*    - ����� � �������       */

		  strcpy(mark[mark_cnt].name, text) ;               /*  ������� ����� � ������ */
			 mark[mark_cnt].row=SYS_CTRL_CNT ;
			      mark_cnt++ ;
			      mark_flag=1 ;                         /*  ���.���� ������� ����� � ������ */

		  strcpy(text, text+len+1) ;                        /*  "�������" ����� */
			    }                                       /* END.3 */
								    /* END.2 */
		    }                                               /* END.1 */
/*----------------------------------- ����������� ��������� �������� */

     if(!memicmp(text, "CHAR",   4) ||
	!memicmp(text, "DOUBLE", 6) ||
	!memicmp(text, "ENTRY",  5) ||
	!memicmp(text, "EXTERN", 6)   ) {
					  L_decl_maker(text) ;
			    if(lng_error)  return(_QUIT) ;

					   continue ;
				        }
/*------------------------------------------ ��������� ��������� END */

     if(!stricmp(text, "END"))                                      /* IF.4 - ��� �������. ��������� END ... */
		 if(mark_flag) {                                    /* ���� ������ �������� ����� -       */
				  lng_error=_EL_END_MARK ;          /*  - ���.������ � �������. ��������� */
				  return(_QUIT) ;
			       }
		 else          {                                    /* ���� �������� ���������... */
				  break ;
			       }                                    /* END.4 */
/*----------------------------------- ���������� ������ �� ��������� */

	 subrow_cnt=0 ;

  do {
	 if(subrow_cnt) subrow_end[subrow_cnt-1]=text ;
			subrow_beg[subrow_cnt  ]=text ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
	 if(!memicmp(text, "ELSE", 4)) {
					if(text[4]=='{')  text+=5 ;
					else              text+=4 ;
		 	 	       }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    else if(!memicmp(text, "IF(", 3)) {
				   addr=L_find_close(text+3) ;      /* ����� ����������� ������ ��������� */
				if(addr[1]=='{')  text=addr+2 ;
				else              text=addr+1 ;
				      }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    else if(!memicmp(text, "DO(", 3)) {
				   addr=L_find_close(text+3) ;      /* ����� ����������� ������ ��������� */
				if(addr[1]=='{')  text=addr+2 ;
				else              text=addr+1 ;
				      }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    else if(!memicmp(text, "FOR(", 4)) {
				   addr=L_find_close(text+4) ;      /* ����� ����������� ������ ��������� */
				if(addr[1]=='{')  text=addr+2 ;
				else              text=addr+1 ;
				       }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    else if(!memicmp(text, "RETURN", 6)) {

      if(text[6]=='(') {
			     addr=L_find_close(text+7) ;            /* ����� ����������� ������ ��������� */
			     text=addr+1 ;
		       }
      else                  text+=6 ;
					 }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    else if(*text=='}') {
			     text++ ;
			}
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    else                     break ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

	    subrow_cnt++ ;

     } while(1) ;

	subrow_end[subrow_cnt]=text+strlen(text) ;
		   subrow_cnt++ ;

/*---------------------------------------------------- ���� �������� */

  for(subrow=0 ; subrow<subrow_cnt ; subrow++) {                    /* CIRCLE.2 - ��������� ����� �� ���������� */

			text= subrow_beg[subrow] ;
	    if(subrow) *text= chr_save ;
		    chr_save=*subrow_end[subrow] ;
	 *subrow_end[subrow]=  0 ;

	    if(*text==0)  break ;

		      close_flag= 0 ;
			any_flag= 0 ;

/*----------------------------------------- ��������� ��������� ELSE */

    if(!memicmp(text, "ELSE", 4)) {                                 /* IF.5 - ���� �����.�������� ELSE ... */

      if(!else_flag) {  lng_error=_EL_ELSE_UNK ;                    /* ��� "�����������" ��������� - */
			      return(_QUIT) ;    }                  /*   ���.������ � �������.�����. */

      if(mark_flag) {   lng_error=_EL_ELSE_MARK ;                   /* ��� ������������ ������� -    */
			      return(_QUIT) ;    }                  /*   ���.������ � �������.�����. */

	 if(text[4]=='{')  block_flag=1 ;                           /* �����. ������� IF(ELSE)-����� */
	 else              block_flag=0 ;
			    else_flag=0 ;                           /* ���������� ELSE-���� */

		L_ctrl_code(_IF_OPER, &vars_go_ind, 0) ;            /* ����.���������� ELSE-�������� */
		L_ied_close(id_stk_cnt-1) ;                         /* ��������� IF �������� */

	    id_stk[id_stk_cnt-1].else_flag = 1 ;                    /* ������������ ������ � IF_DO ����� */
	    id_stk[id_stk_cnt-1].block_flag=block_flag ;
	    id_stk[id_stk_cnt-1].addr      =SYS_CTRL_CNT-1 ;


				   text+=4+block_flag ;             /* "��������" IF-�������� */
				}                                   /* END.5 */
/*---------------------------------- ��������� "��������������" ELSE */

   if(else_flag) {
      for( ; id_stk_cnt>0 ; id_stk_cnt--) {                         /* ������. ��� "����������" IF, ELSE � DO */
	  if( id_stk[id_stk_cnt-1].block_flag) break ;              /*  ������������� ���� � �������������    */
								    /*   ���������� �������� �����            */
	if(!id_stk[id_stk_cnt-1].if_do_flag &&
				   any_flag   )  SYS_CTRL_CNT-- ;

				 L_ied_close(id_stk_cnt-1) ;

	if(!id_stk[id_stk_cnt-1].if_do_flag &&
				   any_flag   )  SYS_CTRL_CNT++ ;
					  }

		      else_flag=0 ;
		 }
/*------------------------------------------- ��������� ��������� IF */

    if(!memicmp(text, "IF(", 3)) {                                  /* IF.6 - ���� �����.�������� IF ... */

	   addr=L_find_close(text+3) ;                              /* ����� ����������� ������ ��������� */

	if(addr==NULL) {   lng_error=_EL_IF_FMT ;                   /* ���� ������� ��� - ������ */
			       return(_QUIT) ;    }
	else                     *addr =  0 ;                       /* ���� ���� - ���.����������� */

	if(id_stk_cnt==_GOTO_MAX-1) {  lng_error=_EL_IFDO_OVR ;     /* ���� IFDO ���� ���������� - */
					       return(_QUIT) ;   }  /*     - ����� � �������       */

	 if(addr[1]=='{')  block_flag=1 ;                           /* �����. ������� IF-����� */
	 else              block_flag=0 ;

	    id_stk[id_stk_cnt].if_do_flag= 0 ;                      /* ��������� ������ � IF_DO ����� */
	    id_stk[id_stk_cnt].else_flag = 0 ;
	    id_stk[id_stk_cnt].block_flag=block_flag ;
	    id_stk[id_stk_cnt].addr      =SYS_CTRL_CNT ;
		   id_stk_cnt++ ;

	   L_ctrl_code(_CULC_OPER, &vars_if_ind, SYS_CULC_CNT) ;    /* ������.�����. ������������ ���������� */
	   L_ctrl_code(  _IF_OPER, &vars_if_ind, 0) ;               /* ����.���������� IF-�������� */

	    addr=L_culc_maker(text+3) ;                             /* ������. ������������ ��������� */
	 if(addr==NULL)  return(_QUIT) ;

				   text=addr+1+block_flag ;         /* "��������" IF-�������� */
				}                                   /* END.6 */
/*------------------------------------------ ��������� ��������� FOR */

    if(!memicmp(text, "FOR(", 4)) {                                 /* ������ '���������' ��������� */
				     text++ ;                       /*       � FOR �� DO            */
				     text[0]='D' ;
				     text[1]='O' ;
				 }
/*------------------------------------------- ��������� ��������� DO */

    if(!memicmp(text, "DO(", 3)) {                                  /* IF.7 - ���� �����.�������� IF ... */
/*- - - - - - - - - - - - - ����� ����������� ������ ��������� ����� */
			     text+=3 ;
	   addr=L_find_close(text) ;                                /* ����� ����������� ������ ��������� */

	if(addr==NULL) {   lng_error=_EL_DO_FMT ;                   /* ���� ������� ��� - ������ */
				return(_QUIT) ;    }
	else           {     do_addr[3]=addr ;                      /* ���� ���� - ���.����������� */
				 *addr =  0 ;    }
/*- - - - - - - - - - - - - - -  ����� �������� � "��������" ������� */
#ifdef _FORTRAN_DO
      for(i=2 ; i>=0 ; i--) {                                       /* ���� ����� "��������",          */
#else
       for(i=2 ; i>0 ; i--) {                                       /* ���� ����� "��������",          */
#endif
		 do_addr[i]=strrchr(text, do_slct[i]) ;             /*  ��� �� ���������� -            */
	      if(do_addr[i]==NULL) {   lng_error=_EL_DO_FMT ;       /*   ���.������ � �������.�����.,  */
					    return(_QUIT) ;    }    /* � ��� ����������� - ����������� */
	      else                           *do_addr[i]=0 ;        /*  ������ � ������ �����          */
			    }
/*- - - - - - - - - - - - - - -  ��������� �����.������ � ID-DO ���� */
	 if(id_stk_cnt==_GOTO_MAX-1) {  lng_error=_EL_IFDO_OVR ;    /* ���� IFDO ���� ���������� - */
					       return(_QUIT) ;   }  /*     - ����� � �������       */

	 if((do_addr[3])[1]=='{')  block_flag=1 ;                   /* �����. ������� DO-����� */
	 else                      block_flag=0 ;

			       base=SYS_CTRL_CNT ;

	    id_stk[id_stk_cnt].if_do_flag= 1 ;                      /* ��������� ������ � IF_DO ����� */
//          id_stk[id_stk_cnt].else_flag = 1 ;
	    id_stk[id_stk_cnt].block_flag=block_flag ;
	    id_stk[id_stk_cnt].addr      =base ;
		   id_stk_cnt++ ;
/*- - - - - - - - - - - - - - - - - - - - -  ��������� ������� ����� */
#ifdef _FORTRAN_DO
			  *do_addr[0]=do_slct[0] ;                  /* �������.����.����� ��������� DO */
#endif

	   if(text[0]!=0) {                                         /* ���� ����.��������� �� ������... */
			addr=L_equ_maker(text) ;                    /*   ����.����.��������� ����� */
		     if(addr==NULL)  return(_QUIT) ;
			  }
	   else           {                                         /* ���� ����.��������� ������... */
			     L_ctrl_code(_NOP_OPER,0,0) ;            /*   ������.������ �������� */
			  }

		 L_ctrl_code(_IF_OPER, &vars_go_ind, base+3) ;      /* ����.������� �� ���� ����� */
/*- - - - - - - - - - - - - - - - -  ��������� ��������������� ����� */
#ifdef _FORTRAN_DO
			 name =text ;
		   *do_addr[0]=  0 ;                                /* ����������� ��� ��������� ����� */
#endif

		   strcpy(buff, "") ;                               /* ���������� ������� ����� */
#ifdef _FORTRAN_DO
		   strcat(buff, name) ;                             /* ��������� ��������������� ��������� */
		   strcat(buff, "=") ;
		   strcat(buff, name) ;
		   strcat(buff, "+") ;
#endif
		   strcat(buff, do_addr[2]+1) ;

	   if(buff[0]!=0) {                                         /* ���� ������.��������� �� ������... */
			       addr=L_equ_maker(buff) ;             /*   ����.�����.��������� ����� */
			    if(addr==NULL)  return(_QUIT) ;
			  }
	   else           {                                         /* ���� ������.��������� ������... */
			     L_ctrl_code(_NOP_OPER,0,0) ;           /*   ������.������ �������� */
			  }
/*- - - - - - - - - - - - - - -  ��������� ����������-�������� ����� */
		   strcpy(buff, "") ;                               /* ���������� ������� ����� */
#ifdef _FORTRAN_DO
		   strcat(buff, name) ;                             /* ��������� ��������������� ��������� */
		   strcat(buff, "<=") ;
#endif
		   strcat(buff, do_addr[1]+1) ;

	     L_ctrl_code(_CULC_OPER, &vars_if_ind, SYS_CULC_CNT) ;  /* ������.�����. ������������ ���������� */
	     L_ctrl_code(  _IF_OPER, &vars_if_ind, 0) ;             /* ����.���������� IF-�������� */

		    addr=L_culc_maker(buff) ;                       /* ������. ������������ ��������� */
		 if(addr==NULL)  return(_QUIT) ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
		  text=do_addr[3]+1+block_flag ;
				}                                   /* END.7 */
/*---------------------------------------- ��������� ��������� BREAK */

    if(!memicmp(text, "BREAK", 5)) {                                /* IF.5 - ���� �����.�������� RETURN ... */

      for(n=-1,i=0 ; i<id_stk_cnt ; i++)
		      if(id_stk[i].if_do_flag)  n=i ;

       if(n==-1) {   lng_error=_EL_EXT_CRC ;                        /* ��� ���������� �������� ����� - */
			  return(_QUIT) ;    }                      /*   ���.������ � �������.�����.   */

	 L_ctrl_code(_SET_OPER, &vars_if_ind, 0) ;                  /* ��������� ���������� ������� */
	 L_ctrl_code( _IF_OPER, &vars_go_ind, id_stk[n].addr+4) ;

		    any_flag =1 ;
			text+=5 ;
				  }                                 /* END.5 */
/*------------------------------------- ��������� ��������� CONTINUE */

    if(!memicmp(text, "CONTINUE", 8)) {                             /* IF.5 - ���� �����.�������� RETURN ... */

      for(n=-1,i=0 ; i<id_stk_cnt ; i++)
		      if(id_stk[i].if_do_flag)  n=i ;

       if(n==-1) {   lng_error=_EL_EXT_CRC ;                        /* ��� ���������� �������� ����� - */
			  return(_QUIT) ;    }                      /*   ���.������ � �������.�����.   */

	 L_ctrl_code( _IF_OPER, &vars_go_ind, id_stk[n].addr+2) ;

		    any_flag =1 ;
			text+=8 ;
				    }                               /* END.5 */
/*--------------------------------------- ��������� ��������� RETURN */

    if(!memicmp(text, "RETURN", 6)) {                               /* IF.6 - ���� �����.�������� RETURN ... */

      if(text[6]=='(') {
	  addr=L_find_close(text+7) ;                               /* ����� ����������� ������ ��������� */
       if(addr==NULL) {   lng_error=_EL_RET_FMT ;                   /* ��� ������������ ������� -    */
				return(_QUIT) ;    }                /*   ���.������ � �������.�����. */
       else              *addr=0 ;

	   L_ctrl_code(_CULC_OPER, &vars_tmp, SYS_CULC_CNT) ;       /* ������. RETURN-��������� */

	    addr=L_culc_maker(text+7) ;                             /* ������. ��������� RETURN-��������� */
	 if(addr==NULL)  return(_QUIT) ;
		       }

	   L_ctrl_code(_RETURN_OPER, &vars_tmp, 0) ;                /* �������� ��������������� ���������� */

		    ret_flag=1 ;                                    /* ���.���� ������� RETURN ��������� */
		    any_flag=1 ;
				    }                               /* END.6 */
/*----------------------------------------- ��������� ��������� GOTO */

    if(!memicmp(text, "GOTO", 4)) {                                 /* IF.7 - ���� �����.�������� GOTO ... */

			    text+=4 ;                               /* �����.��������� �� ����� */
			     len =strlen(text) ;                    /* �����.����� ����� */

       if(text[len-1]==';') {       len-- ;                         /* ���� �������� ������������ ';' - */
			       text[len]= 0 ;   }                   /*   - ������������ ����� �����     */

      if(len>_NAME_MAX) {                                           /* IF.8 - ���� ����� ������� ������� ... */
				lng_error=_EL_MARK_BAD ;            /*  ������������� ������ */
				    return(_QUIT) ;                 /*  ���������� ��������� */
			}
      else              {                                           /* ELSE.8 - ���� ����� ���������� ... */

	      if(dir_cnt==_GOTO_MAX-1) {  lng_error=_EL_GOTO_OVR ;  /* ���� ������� ����� ��������� - */
					       return(_QUIT) ;   }  /*     - ����� � �������          */

		      strcpy(dir[dir_cnt].name, text) ;             /*  ������� ����� � ������ */
			     dir[dir_cnt].row=SYS_CTRL_CNT ;
				 dir_cnt++ ;

			  L_ctrl_code(_IF_OPER, &vars_go_ind, 0) ;  /* �������� ��������������� ���������� */
			}                                           /* END.8 */

		    any_flag=1 ;
				 }                                  /* END.5 */
/*----------------------------------- ��������� ��������� ���������� */

   if(!any_flag && text[0]!=0 && text[0]!='}') {

			addr=L_equ_maker(text) ;

		     if(addr==NULL)  return(_QUIT) ;

		     if(addr!=text) {    any_flag=  1 ;
					     text=addr ;  }

					       }
/*---------------------------- ��������� ��������� '}' (����� �����) */

					 id_num=-1 ;
    if(*text=='}') {                                                /* IF.5 - ���� �����.����� ����� ... */
      for(i=0 ; i<id_stk_cnt ; i++)
	       if(id_stk[i].block_flag)  id_num=i ;

      if(id_num<0) {  lng_error=_EL_BLOCK_UNK ;                     /* ��� "�����������" ��������� - */
			     return(_QUIT) ;    }                   /*   ���.������ � �������.�����. */

			    close_flag=1 ;
				  text++ ;                          /* "��������" �������� ����� ����� */
		   }                                                /* END.5 */
/*--------------------------- ����������� ��������� IF-DO ���������� */

   if(!close_flag)
    if(any_flag && id_stk_cnt)
	if(!id_stk[id_stk_cnt-1].block_flag)  close_flag=1 ;

/*------------------------------ ��������� �������� IF-DO ���������� */

    if(close_flag) {
/*- - - - - - - - - - - - - - - - �������� �������������� ���������� */
	if(id_num>=0)                                               /* ���� ����������� ���� - ��������� */
	    for(i=id_num ; i<id_stk_cnt ; i++)  L_ied_close(i) ;    /*  ��� "����������" IF-DO ��������� */
	else     id_num=id_stk_cnt-1 ;

					 L_ied_close(id_num) ;      /* ��������� �������� */

	  if(id_stk[id_num].if_do_flag ||                           /* ��� ���������� ELSE, DO... */
	     id_stk[id_num].else_flag    ) {
					      id_stk_cnt=id_num ;

	     for( ; id_stk_cnt>0 ; id_stk_cnt--) {                  /*   ��������� ��� "����������" ELSE � DO */
		 if( id_stk[id_stk_cnt-1].block_flag   )  break ;   /*    ������������� ���� � �������������  */
		 if(!id_stk[id_stk_cnt-1].if_do_flag &&             /*     ���������� �������� �����          */
		    !id_stk[id_stk_cnt-1].else_flag    )  break ;

				 L_ied_close(id_stk_cnt-1) ;
						 }

	   if(id_stk_cnt)
	     if(!id_stk[id_stk_cnt-1].if_do_flag &&
		!id_stk[id_stk_cnt-1].else_flag    )  else_flag=1 ;
	     else                                     else_flag=0 ;
					   }
	  else                             {
			      id_stk[id_num].block_flag= 0 ;
					     id_stk_cnt=id_num+1 ;
					      else_flag= 1 ;
					   }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
		   }
/*-------------------------------------------------------------------*/
					       }                    /* CONTINUE.2 */
		       }                                            /* CONTINUE.1 */

/*----------------------------- ���������� ���������� GOTO �� ������ */

  for(i=0 ; i<dir_cnt ; i++) {                                      /* CIRCLE.3 - ��������� GOTO-������ */
				name=dir[i].name ;

     for(j=0 ; j<mark_cnt ; j++)                                    /* ��������� ���������� GOTO-����� */
       if(!strcmp(mark[j].name, name))  break ;                     /*      �� ������� �����           */

	 if(j==mark_cnt) {  lng_error=_EL_MARK_UNK ;                /* ��� ���������� ������ ����� -      */
				  return(_QUIT) ;    }              /*  - ���.������ � �������. ��������� */

		  SYS_CTRL_CODE[dir[i].row].addr=mark[j].row ;      /* ���.����� �������� � ���.���������� */
			     }                                      /* CONTINUE.3 */
/*------------------------------------------- ���������� ENTRY-����� */

  for(i=0 ; i<SYS_ENTRY_CNT ; i++) {                                /* CIRCLE.4 - ��������� ENTRY-������ */
				 name=SYS_ENTRY_LIST[i].name ;

     for(j=0 ; j<mark_cnt ; j++)                                    /* ��������� ���������� GOTO-����� */
       if(!strcmp(mark[j].name, name))  break ;                     /*      �� ������� �����           */

			      lng_bad=name ;

	 if(j==mark_cnt) {  lng_error=_EL_ENTRY_UNK ;               /* ��� ���������� ������ ����� -      */
				  return(_QUIT) ;    }              /*  - ���.������ � �������. ��������� */

			SYS_ENTRY_LIST[i].start=mark[j].row ;       /* ���.����� �������� � ���.���������� */
				   }                                /* CONTINUE.4 */
/*-------------------------------------------- �������������� ������ */

       if(!ret_flag) {   lng_error=_EL_RET_OUT ;                    /* ��� ��������� RETURN � ������ - */
			       return(_QUIT) ;    }                 /*       - ������������� ������    */

       if(id_stk_cnt) {   lng_error=_EL_UNCLOSE ;                   /* ��� ��������� RETURN � ������ - */
			       return(_QUIT) ;    }                 /*       - ������������� ������    */

/*---------------------------------------------- ���������� �������� */

	for(i=0 ; i<SYS_CTRL_CNT ; i++) {

	 if(SYS_CTRL_CODE[i].oper_code==_ALLOC_OPER) {
			    size =SYS_CTRL_CODE[i].addr ;
	   SYS_CTRL_CODE[i].addr =SYS_CTRL_CNT*sizeof(SYS_CTRL_CODE[0])
			       +vars_iarea_cnt*sizeof(double) ;
		  vars_iarea_cnt+=size ;
						     }
					}
/*--------------------------- ���������� ������ ���. � ������. ����� */

     SYS_CTRL_USED=SYS_CTRL_CNT*sizeof(SYS_CTRL_CODE[0])
		  +vars_iarea_cnt*sizeof(double) ;

  if(SYS_CTRL_USED>SYS_CTRL_SIZE)  lng_error=_EL_AREA_MAX ;

     SYS_CULC_USED=SYS_CULC_CNT*sizeof(SYS_CULC_CODE[0]) ;

/*------------------------------- ���������� �� '������' ����������� */

     lng_ctrl_used=SYS_CTRL_USED ;
     lng_culc_used=SYS_CULC_USED ;
      vars_int_cnt=SYS_INT_CNT ;
     vars_work_cnt=SYS_WRK_CNT ;
	 entry_cnt=SYS_ENTRY_CNT ;

/*-------------------------------------------------------------------*/

   return(0) ;
}


/************************************************************************/
/*                                                                      */
/*                       ��������� ��������                             */

    int  L_decl_maker(text)

      char *text ;

{
   int  var_num ;   /* ����� �����.���������� */
  char *var  ;
  char *name ;
  char *index ;
  char *pars_list ;
  char *end  ;
   int  size ;
   int  i ;

/*-------------------------------------- ��������� �������� �������� */

   if(!memicmp(text, "DOUBLE", 6)) {
					  text+=6 ;
/*- - - - - - - - - - - - - - - - - - - - - - -  '��������' �������� */
		      name=text ;                                   /* �������� ��� ������� */

		     index=strchr(name, '[') ;                      /* ����� ��������� ������� */
		  if(index==NULL)  return(0)  ;                     /* ���� ��� �� ������ */

		    *index=0 ;                                      /* ����������� ��� */
		     index++ ;

		       end=strchr(index, ']') ;                     /* ����� ����� ��������� ������� */
		    if(end==NULL) {  lng_error=_EL_INDEX ;
					    return(1) ;    }

		    if(end[1]!= 0 &&                                /* �������� �������� �������� */
		       end[1]!=';'  )  {  lng_error=_EL_DECL_FMT ;
						 return(1) ;       }

		      *end=0 ;                                      /* ����.��������� ��������� */
/*- - - - - - - - - - - - - - - - - - - - - -  ���������� ���������� */
			 var=L_simple_unit(name) ;                  /* ���� ���������� ����� ������������ */
		      if(var!=NULL) {  lng_error=_EL_NAME_USED ;    /* ���� ��� ��� ������������... */
					      return(1) ;         }

				lng_error=0 ;                       /* ����� ������ */

		       var_num=L_alloc_var(name) ;                  /* ��������� ���������� */
		    if(var_num==-1)  return(1) ;
/*- - - - - - - - - - - - - - - - - - - - - - - - ���������� ������� */
			 size=atoi(index) ;                         /* ��������� ������ ������� */
		      if(size<=0) {   lng_error=_EL_DECL_SIZE ;
					     return(1) ;         }

     L_ctrl_code(_ALLOC_OPER, &(SYS_INT_VARS[var_num].value), size);/* ��������� �������� ���������� ������� */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
				  }
/*------------------------------------------------- ��������� ������ */

   if(!memicmp(text, "ENTRY", 5)) {

#define  ENTRY_NAME      SYS_ENTRY_LIST[SYS_ENTRY_CNT].name
#define  ENTRY_PARS_CNT  SYS_ENTRY_LIST[SYS_ENTRY_CNT].pars_cnt
#define  ENTRY_PARS      SYS_ENTRY_LIST[SYS_ENTRY_CNT].pars

					  text+=5 ;
/*- - - - - - - - - - - - - - - - - - - �������� ������������ ������ */
    if(SYS_ENTRY_CNT>=SYS_ENTRY_MAX) {   lng_error=_EL_ENTRY_MAX ;
					       return(1) ;         }
/*- - - - - - - - - - - - - - - - - - - - ��������� � �������� ����� */
		 name=text ;
	    pars_list=strchr(name, '(') ;                           /* ���� ���������� ����� */
	 if(pars_list==NULL)  {  lng_error=_EL_DECL_FMT ;           /* ���� ��� ���... */
					return(1) ;       }
	   *pars_list=0 ;                                           /* ����������� ��� */
	    pars_list++ ;                                           /* �������� �� ������ ���������� */

	 if(strlen(name)>_NAME_MAX) {  lng_error=_EL_NAME_LEN ;     /* ���� ��� ������� �������... */
					      return(-1) ;      }
	 if(strlen(name)==   0    ) {  lng_error=_EL_NAME_EMPTY ;   /* ���� ������ ���... */
					      return(-1) ;      }

	       strcpy(ENTRY_NAME, name) ;                           /* ������� ��� ����� */
/*- - - - - - - - - - - - - - - - -  ���������� ��������� ���������� */
		   end=strchr(pars_list, ')') ;                     /* ����� ����������� ������ ���������� */
		if(end==NULL) {  lng_error=_EL_DECL_FMT ;
					    return(1) ;        }

		if(end[1]!= 0 &&                                    /* �������� �������� �������� */
		   end[1]!=';'  )  {  lng_error=_EL_DECL_FMT ;
					     return(1) ;       }

		  *end=0 ;                                          /* ����.������ ���������� */
/*- - - - - - - - - - - - - - - - - - - - - - - ��������� ���������� */
     if(pars_list!=end) {                                           /* IF.1 - ���� ��������� ����... */

	 for(i=0 ; ; i++) {
			    end=strchr(pars_list, ',') ;            /* ���� ����������� ���������� */
			 if(end!=NULL)  *end=0 ;

	   for(var_num=0 ; var_num<SYS_INT_CNT ; var_num++)         /* ������� ��� �� ����� �����.�����.    */
	    if(!strcmp(SYS_INT_VARS[var_num].name,                  /*  � ���� ����� ���� - ���.����� �����.*/
					 pars_list))  break ;

	      if(var_num==SYS_INT_CNT) {                            /* ���� ����� �����.���... */
			   var_num=L_alloc_var(pars_list) ;         /*   ��������� ���������� */
			if(var_num==-1)  return(1) ;
				       }

			    ENTRY_PARS[i]=var_num ;

			 if(end==NULL)  break ;

				pars_list=end+1 ;
			  }
			       ENTRY_PARS_CNT=i+1 ;
			}                                           /* ELSE.1 */
/*- - - - - - - - - - - - - - -  ��������� ������� ������ ���������� */
     else               {                                           /* ELSE.1 */
			       ENTRY_PARS_CNT=0 ;
			}                                           /* END.1 */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
#undef  ENTRY_NAME
#undef  ENTRY_PARS_CNT
#undef  ENTRY_PARS
					SYS_ENTRY_CNT++ ;
				 }
/*---------------------------------------- ��������� ������� ������� */

   if(!memicmp(text, "EXTERN", 6)) {
	 			   }
/*-------------------------------------------------------------------*/

   return(0) ;
}


/************************************************************************/
/*                                                                      */
/*              ������������� ������������ ����������                   */

   int  L_ctrl_code(code, oper, addr)

	 int  code ;   /* ��� �������� */
      double *oper ;   /* ����� �������� */
	 int  addr ;   /* ������� ����� */

{
    if(SYS_CTRL_CNT>=SYS_CTRL_MAX-8) {
					lng_error=_EL_CTRL_BUFF ;
					       return(-1) ;
				     }

       SYS_CTRL_CODE[SYS_CTRL_CNT].oper_code=code ;
       SYS_CTRL_CODE[SYS_CTRL_CNT].operand  =oper ;
       SYS_CTRL_CODE[SYS_CTRL_CNT].addr     =addr ;
       SYS_CTRL_CODE[SYS_CTRL_CNT].text_row =row ;
		     SYS_CTRL_CNT++ ;

  return(0) ;
}


/************************************************************************/
/*                                                                      */
/*              �������� IF, ELSE � DO ����������                       */

 void  L_ied_close(pnt)

     int  pnt ;   /* ��������� ������ ����� */

{
   int  n ;

/*- - - - - - - - - - - - - - - - - - - - - -  �������� ��������� DO */
	if(id_stk[pnt].if_do_flag) {

	  memmove(&SYS_CTRL_CODE[SYS_CTRL_CNT  ],
		  &SYS_CTRL_CODE[SYS_CTRL_CNT-1],
			      sizeof(SYS_CTRL_CODE[0])) ;

				 n=id_stk[pnt].addr ;

		      if(else_flag)  SYS_CTRL_CNT-- ;

		L_ctrl_code(_IF_OPER, &vars_go_ind, n+2) ;          /* ����.���������� ELSE-�������� */

		      SYS_CTRL_CODE[n+4].addr=SYS_CTRL_CNT ;

		    if(else_flag)  SYS_CTRL_CNT++ ;
				   }
/*- - - - - - - - - - - - - - - - - - - - -  �������� ��������� ELSE */
  else  if(id_stk[pnt].else_flag ) {
					  n =id_stk[pnt].addr ;
		       SYS_CTRL_CODE[n].addr=SYS_CTRL_CNT ;
				   }
/*- - - - - - - - - - - - - - - - - - - - - -  �������� ��������� IF */
  else                             {
					    n =id_stk[pnt].addr ;
		       SYS_CTRL_CODE[n+1].addr=SYS_CTRL_CNT ;
				   }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
}


/************************************************************************/
/*                                                                      */
/*              ��������� ��������� ����������                          */

  char *L_equ_maker(text)

    char *text ;

{
    char *addr ;
    char *var ;
     int  len ;
     int  i ;


/*------------------------------------ ���������� � ������� �������� */

			    addr=strchr(text, ';') ;                /* ����������� �� ; */
	   if(addr!=NULL)  *addr=0 ;

	  if(*text==0)  return(text) ;                              /* ��������� ������� ��������� */

/*---------------------- �������������� ���������� ������ ���������� */

	addr=strchr(text, '=') ;                                    /* ���� ����������� ���������� */

  while(addr!=NULL && addr[1]!='=') {                               /* IF.1 - ���� ������. ���������� ... */

		       len=addr-text ;                              /* �����.����� ����� */

	  if(len==0) {   lng_error=_EL_NAME_EMPTY ;                 /* ���� ��� ����������� -       */
				return(NULL) ;       }              /*  - ���.������ � �����.�����. */
/*- - - - - - - - - - - - - - - ��������� ����� ��������� ���������� */
       for(i=0 ; i<len; i++)                                        /* ���������.������ ��������  */
		if(!LNG_NAME(text[i]))  break ;                     /*  �������������� ���������� */

	    if(text[i]=='[')  break ;                               /* ������� ������������� �� ��������� */

      if(i==len) {                                                  /* IF.2 - ���� ��� ������.���������� ... */
	  if(len>_NAME_MAX) {   lng_error=_EL_NAME_LEN ;            /* ���� ��� ������� ������� -  */
				      return(NULL) ;      }         /*  - ���.������ � �����.�����. */
/*- - - - - - - - - - - - - - -  ����� ���������� ����� ������������ */
				 text[len]=  0 ;                    /* ����������� ��� */

			 var=L_simple_unit(text) ;                  /* ���� ���������� ����� ������������ */
		   lng_error=   0 ;                                 /* ����� ������ */

		if(var==NULL) {                                     /* ���������� ����� ���������� */
				    L_alloc_var(text) ;
				if(lng_error)  return(NULL) ;
			      }
/*- - - - - - - - - - - - - - - - - - - - - �������������� ��������� */
			      addr[0]='=' ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
		 }                                                  /* END.2 */
			break ;
				    }                               /* END.1 */
/*------------------------------ �������� ��������������� ���������� */

		L_ctrl_code(_CULC_OPER, NULL, SYS_CULC_CNT) ;       /* �������� ��������������� ���������� */
	  text=L_culc_maker(text) ;                                 /* �������� ��������������� ���� */
       if(text==NULL)  return(NULL) ;

/*-------------------------------------------------------------------*/

  return(text) ;
}


/************************************************************************/
/*                                                                      */
/*              ���������� ���������� ����������                        */
/*                                                                      */
/*   Return:  ����� �������� ��� NULL                                   */

  int  L_alloc_var(name)

     char  *name ;   /* ��� ���������� */

{
  int  i ;

/*--------------------------------------------------- �������� ����� */

	  if(name[0]==0) {  lng_error=_EL_NAME_EMPTY ;              /* ���� ������ ���... */
				    return(-1) ;         }

	      for(i=0 ; name[i]; i++)                               /* ���������.������ ��������  */
		if(!LNG_NAME(name[i]))  break ;                     /*  �������������� ���������� */

	  if(name[i]!=0) {  lng_error=_EL_NAME_SINTAX ;             /* ���� ������.������ �����... */
				    return(-1) ;         }

	  if(i>_NAME_MAX) {  lng_error=_EL_NAME_LEN ;               /* ���� ��� ������� �������... */
				    return(-1) ;        }

/*-------------------------------------------- ���������� ���������� */

		if(SYS_INT_CNT==SYS_INT_MAX) {                      /*  ���� ������� ����� ���������� - */
				      lng_error=_EL_INT_VAR_MAX ;   /*     - ����� � �������            */
						 return(-1) ;
					     }

	       strcpy(SYS_INT_VARS[SYS_INT_CNT].name, name) ;       /*  ������� ��� �� "����������" ������ */
				   SYS_INT_CNT++ ;

/*-------------------------------------------------------------------*/

  return(SYS_INT_CNT-1) ;
}


/************************************************************************/
/*                                                                      */
/*              ���������� ��������������� ����������                   */

  char *L_culc_maker(text)

    char *text ;

{
static char  work[_LEN_MAX] ;
       char *addr ;
       char *beg ;
       char *end ;
	int  beg_cnt ;    /* ����� ����������� ������ */
	int  end_cnt ;    /* ����� ����������� ������ */
	int  end_len ;
	int  str_len ;
	int  len ;

/*---------------------------------------------- ����� ������������� */

			strcpy(work, text) ;

	      text=text+strlen(work) ;                              /* ���.����. �� ���������� ������ */

	  addr=strchr(work, ';') ;                                  /* ���� ���������� ��������� */
       if(addr!=NULL)  *addr=0 ;                                    /* �������� ��������� */

		      vars_tbl_cnt= 0 ;                             /* ����� ��������� */
		       SYS_WRK_CNT= 0 ;
		     vars_cnst_cnt= 0 ;
			 index_cnt= 0 ;

/*-------------------------------------------------- �������� ������ */

	   beg_cnt=0 ;
	   end_cnt=0 ;

   for(addr=work ; *addr ; addr++) {                                /* CIRCLE.1 - ��������� ������ */

      if(*addr=='{') {  addr+=4 ;  continue ;  }                    /* �������� ������� ������ */

	      if(*addr=='(')  beg_cnt++ ;                           /* ������������ ������ */
	      if(*addr==')')  end_cnt++ ;

	       if(beg_cnt<end_cnt)  break ;
				   }                                /* CONTINUE.1 */

     if(beg_cnt>end_cnt) {   lng_error=_EL_OPEN ;                   /* ������� �������������� ������ */
				  return(NULL) ;    }
     if(beg_cnt<end_cnt) {   lng_error=_EL_CLOSE ;
				  return(NULL) ;    }

/*--------------------------------- ��������� ������������ � ������� */

	       len=strlen(work) ;
	       beg=&work[len-1] ;

  while(1) {                                                        /* CIRCLE.2 - ���� �� ����� ������ */

   for(addr=beg ; addr>=work ; addr--)                              /* ���� ����������� ������ ������ ������ */
	  if(*addr=='}')    addr-=4 ;                               /*   �������� ����.������� */
     else if(*addr=='(') {
		if(LNG_CHAR(*(addr-1)))  continue ;                 /*   ���� ����� ������� ���������    */
					   break ;                  /*    ���-����.������ - ��� �������, */
			 }                                          /*     �� �� ����������              */

		 if(*addr!='(')  break ;

	      beg=addr ;
	      end=L_find_close(beg+1) ;

		 *end=0 ;                                           /* ����������� ������������ � ������� */

	       addr=L_simple_string(beg+1) ;                        /* �������.������������ � ������� */

	   if(lng_error)  return(NULL) ;                            /* ���� ���� ������ - _QUIT-����� */

		     end_len=strlen(end+1)+1 ;                      /* ���������� ������ "������",  */
		     str_len=end-beg+1 ;                            /* ��������� ������.� ������� � */
			 len=strlen(addr) ;                         /* ������������� ���������      */

	   if(len!=str_len)  memmove(beg+len, end+1, end_len) ;     /* "���������" �������� ��������� */
			     memmove(beg, addr, len) ;              /* ��������� �����.�����. � �������� */
	   }                                                        /* CONTINUE.2 */
/*--------------------------------- ��������� "����������" ��������� */

	       L_simple_string(work) ;
		   L_culc_code(_STOP_OPER, NULL, NULL) ;            /* ��������� �������� STOP */

      if(lng_error)  return(NULL) ;

      if(SYS_INT_CNT>SYS_WRK_MAX) {                                 /* ���� ������� ����� ���������� - */
				      lng_error=_EL_WRK_VAR_MAX ;   /*    - ����� � �������            */
						 return(NULL) ;
				  }

/*-------------------------------------------------------------------*/

  return(text) ;
}


/*********************************************************************/
/*                                                                   */
/*                ��������� ������� �����                            */
/*                                                                   */
/*   ������� ������ �� �������� ������������ �� � �������,           */
/*    �� � ���������� ��������, �� � ��������                        */

   char *L_simple_string(string)

    char *string ;   /* �������������� ������ */

/* Return: ��������� �� ��������� ��������� ��� */
/*          NULL, ���� ���� ������              */
{
 struct L_culc *exe ;               /* ��������� ����� ��������. ���� */
   static char  work[2*_LEN_MAX] ;
	  char *pars[20] ;          /* ��������� �� ��������� */
	  char *name ;              /* ��� ������� */
	  char *addr ;
	  char *beg ;
	  char *end ;
	  char *tmp ;
	  char  next ;
	   int  oper_code ;
	   int  len ;
	   int  n ;
	   int  i ;
	   int  j ;

/*---------------------------------------------- ����� ������������� */

	      strcpy(work, string) ;

/*--------------------------------------- �������� ������� ��������� */

   for(addr=work ; *addr ; addr++)
      if((*addr>='0' && *addr<='9') ||
	 (*addr>='A' && *addr<='z')   )  break ;

     if(*addr==0) {  lng_error=_EL_EMPTY ;
		       return(NULL) ;      }

/*-------------------------------------- ��������� �������� �������� */

   if(text_flag) {
			n=text_size/_CULC_FRAME+1 ;                 /* �����.����� ������ ��� */
								    /*   ��������� �������    */
#pragma warning(disable : 4312)
		    L_culc_code(_TSEG_OPER, (char *)n, NULL) ;      /* ������������� ��������� ������� */
#pragma warning(default : 4312)

		       exe=&SYS_CULC_CODE[SYS_CULC_CNT] ;           /* ���.����� ���������� ������ */
		memcpy(exe, text_buff, n*_CULC_FRAME) ;             /* ��������� ��������� ������� */

			  SYS_CULC_CNT+=n ;                         /* ����.������� ������ */
			     text_flag =0 ;
		 }
/*------------------------------------------------ ��������� ������� */

    for(addr=work+1 ; ; addr++) {                                   /* CIRCLE.1 - ���� ������� ... */
/*- - - - - - - - - - - - - - - - - - - - -  ��������� ����� ������� */
	       addr=strchr(addr, '(') ;                             /* ���� ����.������ */
	    if(addr==NULL)  break ;                                 /* ���� ��� - ��������� ������ */

	    if(!LNG_NAME(addr[-1]))  continue ;                     /* ���� ����� ������ ������� ��       */
								    /*   ���-����.������ - ��� �� ������� */
	 for(name=addr-1 ; LNG_NAME(name[-1]) &&                    /* ������� �� ������ ����� */
				    name!=work-1 ; name--) ;

		 *addr=  0 ;                                        /* ����������� ��� */
	       lng_bad=name ;                                       /* ���.��������� '�������' ����� */
/*- - - - - - - - - - - - - - - - - - - - - -  ������������� ������� */
	     func_flag=0 ;                                          /* �����.���� ����������� ������� */

	 for(i=0 ; i<SYS_ENTRY_CNT ; i++)                           /* ���� ������� �� ������ ENTRY-������ */
	  if(!strcmp(name, SYS_ENTRY_LIST[i].name)) {
			  func_flag    =         1 ;
			  func_num     =         i ;
			  func_pars_cnt=SYS_ENTRY_LIST[i].pars_cnt ;
			  func_type    =        'R' ;
				      break ;
						    }

      if(!func_flag)
	 for(i=0 ; i<SYS_EXT_FCNT ; i++)                            /* ���� ������� �� �������� ������ */
	  if(!strcmp(name, SYS_EXT_FUNC[i].name)) {
			  func_flag    =         1 ;
			  func_num     =         i ;
			  func_pars_cnt=SYS_EXT_FUNC[i].pars_cnt ;
			  func_type    =        'F' ;
				      break ;
						  }

      if(!func_flag) {                                              /* ���� ������� �� ������� -    */
	   func_num=L_table_objects(_FIND_FUNC, name) ;             /*   ���� �� �� ������� ������� */
	if(func_num>=0) {
	  if(func_std!=_LANG_STD_FUNC) {  lng_error=_EL_STD_OUT ;   /*   ���� ������������ ���������� - */
					       return(NULL) ;     } /*    - ���.������ � �������        */
			    func_flag= 1 ;
			    func_type='P' ;
			}
		     }

			    strcpy(extrn_name, name) ;              /* ���.��� �������� ������� */

      if(!func_flag) {   lng_error=_EL_FUNC_UNK ;                   /* ���� ������� �� ������� - */
			     return(NULL) ;        }                /*  - ���.������ � �������   */
/*- - - - - - - - - - - - - - - - - - - - - -  ���������� ���������� */
		    *addr='(' ;                                     /* �����.������ */
		      beg=addr+1 ;                                  /* ���.������ ������ ���������� */
		      end=strchr(beg, ')') ;                        /* ���� ����� ������ ���������� */
		   if(end==NULL) {  lng_error=_EL_FUNC_FMT ;        /* ���� �������� ��� - ������ */
					    return(NULL) ;  }
		     *end=0 ;                                       /* ����������� ������ ���������� */

      if(beg!=end) {                                                /* ���� ���� ��������� ... */
	for(j=0 ; ; j++) {    pars[j]=strrchr(beg, ',') ;           /*   ���������� ����� ���������� */
			   if(pars[j]==NULL) break ;                /*    � ��������� ��             */
			     *pars[j]=0 ;                 }

			      pars[j]=beg-1 ;
		   }
      else              j=-1 ;                                      /* ���� ���������� ��� ... */

	   if(func_pars_cnt>=0)                                     /* ����� ���������� ������ ������... */
	     if(j+1!=func_pars_cnt) {                               /*  ���� ����� ���������� �� */
			 lng_warn[lng_warn_cnt].code=_WL_FUNC_PARS ;/*   ��������� � �������� -  */
			 lng_warn[lng_warn_cnt].row = row ;         /*    ������ ��������������  */

		  if(lng_warn_cnt<_LNG_WARN_MAX-1)  lng_warn_cnt++ ;
				    }
/*- - - - - - - - - - - - - - - ��������� ����� ������� � ���������� */
	for( ; j>=0 ; j--) {                                        /* �������� ��������� ���������� */
			     tmp=L_simple_unit(pars[j]+1) ;         /*      ������� � ����           */
	    if(tmp==NULL) {  lng_error=_EL_FUNC_SINTAX ;
					 return(NULL) ;  }
		  L_culc_code(_PUSH_OPER, tmp, NULL) ;
			   }

			func[1]=func_type ;                         /* ��������� ��� ������� */
			func[2]=func_num+1 ;
			rslt[2]=SYS_WRK_CNT+1 ;                     /* �������� ������� ������ ��� ���������� */
				SYS_WRK_CNT++ ;

		L_culc_code(_CALL_OPER, func, rslt) ;               /* ��������� ������.����� ������� */

		  memmove(name+5, end+1, strlen(end+1)+1) ;         /* ������.������ ���������� � ����� */
		  memmove(name, rslt, 5) ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
				}                                   /* CONTINUE.1 */
/*----------------------------------------------- ��������� �������� */

   for(addr=work ; *addr ; addr++) {                                /* CIRCLE.4 - ��������� ������ */

      if(*addr=='{') {  addr+=4 ;  continue ;  }                    /* �������� ������� ��������� */

	 if((*addr>='0' && *addr<='9') ||                           /* IF.2 - ���� �����, ���������� ����� */
	     *addr=='.'                ||                           /*          ��� ���� ...               */
	     *addr=='-'                ||
	     *addr=='+'                  ) {

      if(  addr    !=work         && 
           addr[-1]!='='          &&                                /* ���� ����� �� � ������ ������ -  */          
	 (*addr=='-' || *addr=='+') ) {  end=addr+1 ;               /* ��� �� ����� ���������� �� ����� */
					  continue ;  }

	       vars_cnst[vars_cnst_cnt]=strtod(addr, &end) ;        /* ��������� ����� */

      do {                                                          /* BLOCK.1 */
                   j=0 ;

       if(addr[-1]!='[' && end[0]!=']') {                           /* IF.3 - ���� �� ������... */

	 if(*end!=0)                                                /* ���� ����� �� ���������� ������ */
	    for(j=0 ; j<_OPER_MAX ; j++)                            /*  ��������� ������� �� �� ������ */
	      if(lng_oper[j].len==2) {                              /*    ���� ��������                */
		    if((lng_oper[j].name)[0]==*(end+0) &&
		       (lng_oper[j].name)[1]==*(end+1)   ) break ;
				     }
	      else  if((lng_oper[j].name)[0]==*(end+0)   ) break ;

		if(j==_OPER_MAX)  break ;                           /* ���� ��� - ��� �� ����� */

	 if(addr!=work) {                                           /* ���� ����� �� � ������ ������... */
	    for(j=0 ; j<_OPER_MAX ; j++)                            /*  ��������� ������������ �� ����� */
	      if(lng_oper[j].len==2) {                              /*    ���� ��������                 */
		    if((lng_oper[j].name)[0]==*(addr-2) &&
		       (lng_oper[j].name)[1]==*(addr-1)   ) break ;
				     }
	      else  if((lng_oper[j].name)[0]==*(addr-1)   ) break ;

		if(j==_OPER_MAX)  break ;                           /*   ���� ��� - ��� �� ����� */
			}
					}                           /* END.3 */

			   cnst[2]=vars_cnst_cnt+1 ;
			   cnst[3]='0' ;

		  memmove(addr+5, end, strlen(end)+1) ;             /* ������.������ ��������� � ����� */
		  memmove(addr, cnst, 5) ;
		      addr+=4 ;

		    vars_cnst_cnt++ ;                               /* �����.������� �������� */

	 } while(0) ;                                               /* BLOCK.1 */
		      if(*end==0)  break ;                          /* ���� ����� ������ - ����� ������� */
					   }                        /* END.2 */

				   }                                /* CONTINUE.4 */
/*--------------------------------------------- ��������� ���������� */

   for(addr=work ; *addr ; addr++) {                                /* CIRCLE.5 - ��������� ������ */

      if(*addr=='{') {  addr+=4 ;  continue ;  }                    /* �������� ������� ��������� */

     if(LNG_CHAR(*addr)) {                                          /* IF.4 - ���� ����� */

       for(end=addr ; LNG_CHAR(*end) ; end++) {                     /* ���� ����� ����� ���������� */
			   if(*end=='[')  end+=1 ;
			   if(*end=='{')  end+=4 ;
					      }
		    lng_bad=addr ;                                  /* ���.��������� ���������� ��������� */

			len=end-addr ;                              /* �����.����� ����� */
		       next=*end ;                                  /* ������� ��������� �� ������ ������ */
		       *end= 0 ;                                    /* ����������� ��� */

		     if(len>_NAME_MAX) {  lng_error=_EL_NAME_LEN ;  /* ���� ��� ������� ������� - ��� ������ */
					   return(NULL) ;  }

			     tmp=L_simple_unit(addr) ;              /* ������������ ��� */
			  if(tmp==NULL)  return(NULL) ;             /* ���� ��� ���������� - ������ */

			 *end=next ;                                /* �������.������ ����� ����� */

		  memmove(addr+5, end, strlen(end)+1) ;             /* ������.������ ���������� ����� */
		  memmove(addr, tmp, 5) ;
			  addr+=4 ;

//		      if(*end==0)  break ;                          /* ���� ����� ������ - ����� ������� */
			 }                                          /* END.4 */
				   }                                /* CONTINUE.5 */
/*--------------------------------------------- ���������� ��������� */
/*-------------------------------------------------------------------*/

		 exe=&SYS_CULC_CODE[SYS_CULC_CNT] ;

   for(i=0 ; i<_OPER_MAX ; i++)  do {                               /* CIRCLE.6 - �� ��������� */

		oper_code=lng_oper[i].code ;
		      len=lng_oper[i].len ;

    for(addr=work ; addr!=NULL ; addr+=len) {                       /* ���� ���� �������� */
			    addr=strstr(addr, lng_oper[i].name) ;
                         if(addr     ==NULL)  break ;
			 if(addr[len]!= '}')  break ;
					    }
       if(addr==NULL)  break ;                                      /* ���� ��� ��� - �����. � ����.�������� */

       if(addr[len]!='{') {  lng_error=_EL_SINTAX1 ;                /* ���� ����� �� ��������� ��� �������� - */
				    return(NULL) ;   }              /*  ��������� ������                      */

      if(oper_code!=_MOVE_OPER)
       if(addr[len+1]!='W') {     rslt[2]=SYS_WRK_CNT+1 ;           /* ���� ������ ������� -    */
			L_culc_code(_MOVE_OPER, addr+len, rslt) ;   /*  �� ������� ������, ��   */
								    /*   ������� ������ ������� */
				  addr[len+1]='W' ;                 /*  � ������� ������ �      */
				  addr[len+2]=SYS_WRK_CNT+1 ;       /*   �����.������� � ������ */
				  addr[len+3]='0' ;
					      SYS_WRK_CNT++ ;
						      exe++ ;  }

      if(addr==work) {                                              /* IF.5 - ���� ��������� �������� ... */
	 if(lng_oper[i].pref_flag) {  lng_error=_EL_SINTAX2 ;       /* ���� �������� �� ����� ����    */
					     return(NULL) ;   }     /*   ��������� - ��������� ������ */
		    L_culc_code(oper_code, zero, addr+len) ;        /* ����.��� �������� */
						 exe++ ;
		     }                                              /* ELSE.5 */
      else           {                                              /* ELSE.5 - ���� ������ �������� */
	if(oper_code==_MOVE_OPER)                                   /* ����.��� �������� */
			L_culc_code(oper_code, addr+len, addr-5) ;
	else            L_culc_code(oper_code, addr-5, addr+len) ;
					       exe++ ;
		     }                                              /* END.5 */

		       strcpy(addr-5, addr+len) ;                   /* "�������" ����� ������� */

				    } while(1) ;                    /* CONTINUE.6 */

/*---------------------------------------------- �������� "��������" */

	       len=strlen(work) ;

	 if(len!=5) {
			lng_error=_EL_SINTAX3 ;
			       return(0) ;
		    }
/*-------------------------------------------------- ������ �������� */

	 if(work[1]!='W') {                                         /* ���� ���������� ������� �� W - */
				  rslt[2]=SYS_WRK_CNT+1 ;           /*  ������� ��� �                 */
			L_culc_code(_MOVE_OPER, work, rslt) ;       /*   ������� ������, � �� ��      */
								    /*   ����� ����� �������������    */
				  work[1]='W' ;                     /*   �����. ������� ������        */
				  work[2]=SYS_WRK_CNT+1 ;
				  work[3]='0' ;
					  SYS_WRK_CNT++ ;
			  }
/*-------------------------------------------------------------------*/
/*-------------------------------------------------------------------*/

  return(work) ;
}


/*********************************************************************/
/*                                                                   */
/*           ����� ����������� ������                                */

  char *L_find_close(text)

     char *text ;

{
  int  n ;


  for(n=0 ; *text ; text++)
	 if(*text=='{')  text+=4 ;
    else if(*text==')') if(n==0)  break ;
			else       n-- ;
    else if(*text=='(')    n++ ;

      if(*text!=')')  text=NULL ;

  return(text) ;
}


/*********************************************************************/
/*                                                                   */
/*           ��������� ��������� ��������� ��� ����������            */

   char *L_simple_unit(name)

     char *name ;  /* ���������� ��������� ������� */

{
 static char  code[8]="{???}" ;
	char *index ;            /* ��������� ��������� */
	char *index_code ;       /* ��� ���������� ��������� */
	 int  index_num ;        /* '�����' ���������� ������������ */
	char *end ;
	 int  n  ;
	 int  i ;

/*--------------------------------------------- ��������� ���������� */

		       index=strchr(name, '[') ;                    /* ���� ���� ������� */

     if(index!=NULL) {                                              /* IF.1 - ���� ���� ������ */

	if(index_cnt>=_INDEX_MAX) {  lng_error=_EL_INDEX_MAX ;      /* �������� ������������ */
					    return(NULL) ;      }   /*    ������ ��������    */

					  *index=0 ;                /* ����������� ��� ������� */
					   index++ ;                /* �������� ������ */

				  n=strlen(index) ;                 /* ������������ ����.������ */
		 if(index[n-1]!=']') {  lng_error=_EL_INDEX ;
					      return(NULL) ;   }

			     index[n-1]=0 ;                         /* ����������� ������ */

			   index_code=L_simple_unit(index) ;        /* �����.���������� ��������� */
			if(index_code==NULL)  return(NULL) ;

		      strcpy(index_list[index_cnt], index_code) ;
					index_cnt++ ;
			      index_num=index_cnt  ;
		     }                                              /* ELSE.1 */
     else            {                                              /* ELSE.1 - ���� ������� ������ */
			      index_num= 0 ;
		     }                                              /* END.1 */
/*--------------------------------- ��������� ��� �������� ��������� */

	      vars_cnst[vars_cnst_cnt]=strtod(name, &end) ;         /* ��������� ����� */

   if(*end==0) {                                                    /* ���� ��� ������������� ��������� */
		  code[1]='C' ;
		  code[2]=vars_cnst_cnt+1 ;
		  code[3]=index_num+'0'  ;
			  vars_cnst_cnt++ ;

			    return(code) ;
	       }
/*------------------------------- ��������� ��� ���������� ��������� */

   if(name[0]=='{' &&
      name[1]=='L'   ) {
			  memcpy(code, name, 5) ;
			   code[3]=index_num+'0'  ;
				 return(code) ;
		       }

   if(name[0]=='{' &&
      name[1]=='C'   ) {
			  memcpy(code, name, 5) ;
			   code[3]=index_num+'0'  ;
				 return(code) ;
		       }
/*----------------------------------------- ��������� ��� ���������� */

			   code[3]=index_num+'0'  ;

					 n=-1 ;

	    for(i=0 ; i<SYS_EXT_VCNT && n<0 ; i++)                  /* ������� ��� �� ����� �����.�����.  */
		 if(!strcmp(SYS_EXT_VARS[i].name, name))  n=i ;     /*  � ���� ����� ���� ���.����� �����.*/

	  if(n>=0) {   code[1]='E' ;                                /* ���� ��� ������� ���������� */
		       code[2]= n+1 ;
			return(code) ;   }

	    for(i=0 ; i<SYS_INT_CNT && n<0 ; i++)                   /* ������� ��� �� ����� �����.�����.  */
		 if(!strcmp(SYS_INT_VARS[i].name, name))  n=i ;     /*  � ���� ����� ���� ���.����� �����.*/

	  if(n>=0) {   code[1]='I' ;                                /* ���� ��� ���������� ���������� */
		       code[2]= n+1 ;
			return(code) ;   }

		     n=L_table_objects(_FIND_VAR, name) ;           /* ���� ���������� �� ������� ������� */
	  if(n>=0) {  code[1]='T' ;                                 /* ���� ������� ������� ... */
		      code[2]= n+1 ;
		       return(code) ;   }

/*---------------------------------- ��������� ���������� ���������� */

    lng_bad=  name ;
  lng_error=_EL_NAME_UNK ;

  return(NULL) ;
}


/************************************************************************/
/*                                                                      */
/*           ������ � ��������� ������� ������� - LNG_TABLE             */
/*             � ������� ���������� LNG_EXT_LIB                         */

    int  L_table_objects(oper_code, name)

      int  oper_code ;   /* ��� ��������:                     */
			 /*     _FIND_VAR  - ����� ���������� */
			 /*     _FIND_FUNC - ����� �������    */
     char *name ;        /* ��� ������� */

{
 struct    L_variable *lang_vars ;   /* ���������� ��������� LANG */
 struct    L_function *lang_func ;   /* ������� ��������� LANG */
 struct    Ccl_object *ccl_obj ;     /* ������ ��������� CCL */
 struct Dcl_vars_decl *dcl_obj ;     /* ������ ��������� DCL */
		  int  func_flag ;   /* ���� ������ � �������� */
		  int  n_list ;      /* ����� ������ � ������� */
		  int  i ;           /* ����� ������� � ������ */
		 void *addr ;        /* ����� ������� */

/*--------------------------------- ������� �������� � ������������� */

       if(SYS_EXT_TABLE==NULL)  return(-1) ;

	    addr=NULL ;

       if(oper_code==_FIND_FUNC)  func_flag=1 ;
       else                       func_flag=0 ;

/*---------------------------------------------------- ����� ������� */

  for(n_list=0 ; SYS_EXT_TABLE[n_list].objects!=NULL ; n_list++) {  /* CIRCLE.1 - ��������� ������� ... */
/*- - - - - - - - - - - - - - - - - - - - - - ������ LANG-���������� */
	if(SYS_EXT_TABLE[n_list].type==_LANG_STD_VARS) {

	 if(func_flag)  continue ;

	     lang_vars=SYS_EXT_TABLE[n_list].objects ;

	   for(i=0 ; *lang_vars[i].name!=0 ; i++)
	    if(!strcmp(lang_vars[i].name, name)) {
			       addr=&(lang_vars[i].value) ;
				       break ;
						 }
						       }
/*- - - - - - - - - - - - - - - - - - - - - - -  ������ LANG-������� */
   else if(SYS_EXT_TABLE[n_list].type==_LANG_STD_FUNC) {

	 if(!func_flag)  continue ;

	     lang_func=SYS_EXT_TABLE[n_list].objects ;

	   for(i=0 ; *lang_func[i].name!=0 ; i++)
	    if(!strcmp(lang_func[i].name, name)) {
			       addr=lang_func[i].addr ;
		      func_pars_cnt=lang_func[i].pars_cnt ;
				       break ;
						 }
						       }
/*- - - - - - - - - - - - - - - - - - - - - - -  ������ CCL-�������� */
   else if(SYS_EXT_TABLE[n_list].type==_CCL_STD) {

	     ccl_obj=SYS_EXT_TABLE[n_list].objects ;

	   for(i=0 ; ccl_obj[i].name!=NULL ; i++)
	    if(( ccl_obj[i].func_flag &&  func_flag) ||
	       (!ccl_obj[i].func_flag && !func_flag)   )
		     if(!strcmp(ccl_obj[i].name, name)) {
			     addr=ccl_obj[i].addr  ;
				      break ;
							}
						 }
/*- - - - - - - - - - - - - - - - - - - - - - -  ������ DCL-�������� */
   else if(SYS_EXT_TABLE[n_list].type==_DCL_STD) {

	     dcl_obj=SYS_EXT_TABLE[n_list].objects ;
#if 0
	   for(i=0 ; dcl_obj[i].name!=NULL ; i++)
	    if(( dcl_obj[i].func_flag &&  func_flag) ||
	       (!dcl_obj[i].func_flag && !func_flag)   )
		     if(!strcmp(dcl_obj[i].name, name)) {
			     addr=dcl_obj[i].addr ;
				      break ;
							}
#endif
						 }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
	       if(addr!=NULL)  break ;
								 }  /* CONTINUE.1 */
/*------------------------------------- ������ � ������� ����������� */

   if(SYS_EXT_LIB!=NULL &&                                          /* ���� ������ ������� ���������� */
	     addr==NULL   ) {                                       /*     � ������ �� ������ ...     */

     if(func_flag) {                                                /* ���� ���� ������� -               */
		     lang_func=SYS_EXT_LIB(_LOAD_FNC, name) ;       /*  - ����������� ������� ���������� */
		  if(lang_func!=NULL) {                             /*   �� ����� ������� � ���� ���     */
					addr=lang_func->addr ;      /*  ������� - ��������� �� ����� �   */
			       func_pars_cnt=lang_func->pars_cnt ;  /*   ����� ����������                */
				      }
		   }
     else          {                                                /* ���� ���� ���������� -            */
		     lang_vars=SYS_EXT_LIB(_LOAD_VAR, name) ;       /*  - ����������� ������� ���������� */
		  if(lang_vars!=NULL)  addr=&(lang_vars->value) ;   /*   �� ����� ���������� � ���� ���  */
		   }                                                /*  ������� - ��������� �� �����     */
			    }
/*---------------------------- ��������� ������� � '�������' ������� */

   if(addr!=NULL) {
		      func_std=SYS_EXT_TABLE[n_list].type ;

		     vars_tbl[vars_tbl_cnt]=addr ;
			      vars_tbl_cnt++ ;
		       return(vars_tbl_cnt-1) ;
		  }
/*-------------------------------------------------------------------*/

    return(-1) ;
}


/************************************************************************/
/*                                                                      */
/*           ������������� ��������������� ����������                   */

  int  L_culc_code(code, oper1, oper2)

	 int  code ;    /* ��� �������� */
	char *oper1 ;   /* �������� 1-�� �������� */
	char *oper2 ;   /* �������� 2-�� �������� */

{
 union {  double  dbl ;
	   short  bin[4] ;  }  cvt ;  /* ������� �������������� */

    int  n1 ;
    int  n2 ;
    int  i ;
    int  j ;

/*----------------------------------------------- ��������� �������� */

      if(SYS_CULC_CNT>=SYS_CULC_MAX-8) {
					 lng_error=_EL_CULC_BUFF ;
						return(-1) ;
				       }
/*--------------------------------- ������������ ���������� �������� */

       if(code==_TSEG_OPER) {
				    i           =SYS_CULC_CNT ;
		      SYS_CULC_CODE[i].oper_code=code ;

#pragma warning(disable : 4311)
				      cvt.dbl=(long)oper1 ;
#pragma warning(default : 4311)

			    SYS_CULC_CODE[i].oper_code|=_DATA_OP1 ;
		   memmove(&SYS_CULC_CODE[i].addr1, &cvt.bin[2], 4) ;

				 SYS_CULC_CNT++ ;
				     return(0) ;
			    }
/*--------------------------------------------- ��������� ���������� */

    if(oper1!=NULL && oper1[3]!='0') {
      if(code==_IND2_OPER)
	L_culc_code(_IND2_OPER, index_list[oper1[3]-'0'-1], NULL) ;
       else
	L_culc_code(_IND1_OPER, index_list[oper1[3]-'0'-1], NULL) ;

				     }

    if(oper2!=NULL && oper2[3]!='0')
	L_culc_code(_IND2_OPER, index_list[oper2[3]-'0'-1], NULL) ;

/*-------------------------------------------------- ������ �������� */

		   if(oper1!=NULL)  n1=oper1[2]-1 ;
		   if(oper2!=NULL)  n2=oper2[2]-1 ;

				    i           =SYS_CULC_CNT ;
		      SYS_CULC_CODE[i].oper_code=code ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - ������ ������� */
       if(oper1 == NULL)   SYS_CULC_CODE[i].addr1=  NULL ;
  else if(oper1[1]=='F')   SYS_CULC_CODE[i].addr1=(void *)SYS_EXT_FUNC[n1].addr ;
  else if(oper1[1]=='W')   SYS_CULC_CODE[i].addr1=&(SYS_WRK_VARS[n1]) ;
  else if(oper1[1]=='I')   SYS_CULC_CODE[i].addr1=&(SYS_INT_VARS[n1].value) ;
  else if(oper1[1]=='E')   SYS_CULC_CODE[i].addr1=&(SYS_EXT_VARS[n1].value) ;
  else if(oper1[1]=='T')   SYS_CULC_CODE[i].addr1=  vars_tbl[n1] ;
  else if(oper1[1]=='P')   SYS_CULC_CODE[i].addr1=  vars_tbl[n1] ;
#pragma warning(disable : 4312)
  else if(oper1[1]=='R')   SYS_CULC_CODE[i].addr1= (double *)n1 ;
#pragma warning(default : 4312)
  else if(oper1[1]=='Z') {
				      cvt.dbl=0 ;
				SYS_CULC_CODE[i].oper_code|=_DATA_OP1 ;
		       memmove(&SYS_CULC_CODE[i].addr1, &cvt.bin[2], 4) ;
			 }
  else if(oper1[1]=='C') {
			       cvt.dbl=vars_cnst[n1] ;
	 if(cvt.bin[0]==0 &&
	    cvt.bin[1]==0   ) {
				SYS_CULC_CODE[i].oper_code|=_DATA_OP1 ;
		       memmove(&SYS_CULC_CODE[i].addr1, &cvt.bin[2], 4) ;
			      }
	 else                 {
				SYS_CULC_CODE[i].oper_code|=_LONG_OP1 ;
		       memmove(&SYS_CULC_CODE[i+1], cvt.bin, 8) ;
				 SYS_CULC_CNT++ ;
			      }
			 }
  else if(oper1[1]=='L') {
			   SYS_CULC_CODE[i].oper_code|=_SHFT_OP1 ;
			      cvt.dbl=text_addr[n1] ;
		  memmove(&SYS_CULC_CODE[i].addr1, &cvt.bin[2], 4) ;
			 }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - ������ ������� */
       if(oper2 == NULL)   SYS_CULC_CODE[i].addr2=  NULL ;
  else if(oper2[1]=='W')   SYS_CULC_CODE[i].addr2=&(SYS_WRK_VARS[n2]) ;
  else if(oper2[1]=='I')   SYS_CULC_CODE[i].addr2=&(SYS_INT_VARS[n2].value) ;
  else if(oper2[1]=='E')   SYS_CULC_CODE[i].addr2=&(SYS_EXT_VARS[n2].value) ;
  else if(oper2[1]=='T')   SYS_CULC_CODE[i].addr2=  vars_tbl[n2] ;
  else if(oper2[1]=='P')   SYS_CULC_CODE[i].addr2=  vars_tbl[n2] ;
//else                  {  SYS_CULC_CODE[i].addr2=  NULL ;
//                         SYS_CULC_CODE[i].value=vars_cnst[n2] ;  }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
			      SYS_CULC_CNT++ ;

/*------------------------------ ��������� ������� ��������� ������� */

  if(code==_CALL_OPER) {
/*- - - - - - - - - - - - - - - - - -  ����� ����������� ENTRY-����� */
	if(oper1[1]=='R') {
		    SYS_CULC_CODE[i].oper_code=_CALLE_OPER ;
			  }
/*- - - - - - - - - - - - - - - - - - - -  ����� '���������' ������� */
   else if(SYS_CULC_CODE[i].addr1==_EXT_EXEC) {                     /* ���� ��������� ������ ���      */
								    /*  ������������ - ������� �� ��� */
	for(j=0 ; j<SYS_INT_CNT ; j++)                              /*   � ��������� ������� ������   */
	  if(!strcmp(SYS_INT_VARS[j].name, extrn_name))  break ;    /*    � ���.��� ������ �������    */
								    /*     �������� '��������' ������ */
      if(j==SYS_INT_CNT) {
		    strcpy(SYS_INT_VARS[j].name, extrn_name) ;
			    SYS_INT_CNT++ ;
			 }

	 SYS_CULC_CODE[i].oper_code=_CALLX_OPER ;
	 SYS_CULC_CODE[i].addr1    =(double *)SYS_INT_VARS[j].name;
					      }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
		       }
/*-------------------------------------------------------------------*/

   return(0) ;
}


/************************************************************************/
/*                                                                      */
/*                ��������� Escape-�������������������                  */
/*                                                                      */
/*  Return:  ����� ������������ ��������                                */
/*                                                                      */
/*  ������:  _EL_INV_CHAR - ������������ Escape-������������������      */

    int  L_escape_dcd(text, chr)

     char *text ;    /* �������� ����� �������� */
     char *chr ;     /* ����� �������������� ������� */

{
  char *end ;   /* ��������� �� ������ �� ������������ ������ */
   int  len ;   /* ����� ������������� ��������� */

/*------------------------------- ��������� ����-���������� �������� */

			      len=1 ;                               /* ����.����� ���.���������  */

	    if(*text=='n' )  *chr='\n' ;                            /* ��������� NL */
       else if(*text=='r' )  *chr='\r' ;                            /* ��������� RET */
       else if(*text=='t' )  *chr='\t' ;                            /* ��������� TAB */
       else if(*text=='\'')  *chr='\'' ;                            /* ��������� ������� ' */
       else if(*text=='\"')  *chr='\"' ;                            /* ��������� ������� " */
       else if(*text=='\\')  *chr='\\' ;                            /* ��������� ������� \ */

/*---------------------------------------- ��������� 16-������� ���� */

       else if(*text=='x') {                                        /* ���� ���� ������� 16-���� ���� ... */
			     *chr=strtol(text, &end, 16) ;          /*  ��������� ��� */

			      len=end-text ;                        /*  �����.����� ���� (� ���������) */

			   if(len<2 ||                              /*  ��� ������ ��������� 1 ��� 2 */
			      len>3   )  lng_error=_EL_INV_CHAR ;   /*    �������, ����� - ������    */
			   }
/*----------------------------------------- ��������� 8-������� ���� */

       else                {                                        /* ��������� ������������� - */
								    /*    -   8-������ ��� ...   */
			     *chr=strtol(text, &end, 8) ;           /*  ��������� ��� */

			      len=end-text ;                        /*  �����.����� ���� */

			   if(len<1 ||                              /*  ��� ������ ��������� 1, 2 ��� 3 */
			      len>3   )  lng_error=_EL_INV_CHAR ;   /*    �������, ����� - ������    */
			   }
/*-------------------------------------------------------------------*/

   return(len) ;
}


/************************************************************************/
/************************************************************************/
/*                                                                      */
/*                          ����������� ����                            */
/*                                                                      */
/************************************************************************/
/************************************************************************/


/************************************************************************/
/*                                                                      */
/*                          '������' �����                              */
/*                                                                      */
/*   Return: 0 - ��� ���������,  -1 - ����������� ����                  */

     int  L_decl_entry(name, pars, pars_cnt)

	char *name ;      /* ��� ����� */
      double *pars ;      /* ������� ���������� */
	 int  pars_cnt ;  /* ����� ���������� */

{
    int  entry ;
    int  i ;

/*---------------------------------------------- ������������� ����� */

    for(entry=0 ; entry<SYS_ENTRY_CNT ; entry++)
      if(!strcmp(name, SYS_ENTRY_LIST[entry].name))  break ;

	  if(entry>=SYS_ENTRY_CNT)  return(-1) ;

/*-------------------------------------------- ���������� ���������� */

	 lng_start=SYS_ENTRY_LIST[entry].start ;                    /* ���.���������� ������ */

     for(i=0 ; i<                      pars_cnt &&                  /* ������� ��������� */
	       i<SYS_ENTRY_LIST[entry].pars_cnt    ; i++)
	 SYS_INT_VARS[SYS_ENTRY_LIST[entry].pars[i]].value=pars[i] ;

/*-------------------------------------------------------------------*/

  return(0) ;
}


/************************************************************************/
/*                                                                      */
/*                     ����� ����������� �����                          */

   double  L_call_entry(entry, pars, pars_cnt)

	 int  entry ;     /* ����� ����� */
      double *pars ;      /* ������� ���������� */
	 int  pars_cnt ;  /* ����� ���������� */

{
  double  result ;
    char *safe_area ;   /* ������� ���������� */
  double *ivars_area ;  /* ������� ����������, '�������' ���������� ���������� */
     int  i ;

#define  ENTRY_START     SYS_ENTRY_LIST[entry].start
#define  ENTRY_PARS_CNT  SYS_ENTRY_LIST[entry].pars_cnt
#define  ENTRY_PARS      SYS_ENTRY_LIST[entry].pars

/*------------------------------------ ���������� ��������� �������� */

      safe_area=calloc(SYS_WRK_CNT+ENTRY_PARS_CNT,                  /* �������� ������ */
				      sizeof(*SYS_WRK_VARS));

   if(safe_area==NULL)  {   lng_ctrl_abort=1 ;                      /* ��� ������� - �������� */
			    lng_culc_abort=1 ;                      /*   ����� ���������      */
				 return(0.) ;   }

      ivars_area=(double*)(safe_area+
			    SYS_WRK_CNT*sizeof(*SYS_WRK_VARS)) ;

	   memcpy(safe_area, SYS_WRK_VARS,                          /* ����.������� ���������� */
		   SYS_WRK_CNT*sizeof(*SYS_WRK_VARS)) ;

      for(i=0 ; i<ENTRY_PARS_CNT ; i++)                             /* ��������� ������ ���������� */
	   ivars_area[i]=SYS_INT_VARS[ENTRY_PARS[i]].value ;

/*-------------------------------------------- ���������� ���������� */

	 lng_start=ENTRY_START ;                                    /* ���.���������� ������ */

     for(i=0 ; i<      pars_cnt &&                                  /* ������� ��������� */
	       i<ENTRY_PARS_CNT    ; i++)
		   SYS_INT_VARS[ENTRY_PARS[i]].value=pars[i] ;

/*------------------------------------------------------- ���������� */

	       result=L_ctrl_exec() ;

/*-------------------------------- �������������� ��������� �������� */

	   memcpy(SYS_WRK_VARS, safe_area,                          /* �����.������� ���������� */
		   SYS_WRK_CNT*sizeof(*SYS_WRK_VARS)) ;

      for(i=0 ; i<ENTRY_PARS_CNT ; i++)                             /* �����.������ ���������� */
		   SYS_INT_VARS[ENTRY_PARS[i]].value=ivars_area[i] ;

	     free(safe_area) ;

/*-------------------------------------------------------------------*/

#undef  ENTRY_START
#undef  ENTRY_PARS_CNT
#undef  ENTRY_PARS

  return(result) ;
}


/************************************************************************/
/*                                                                      */
/*          ����������� ������ ������������ ����������                  */

#define   OPER_CODE  (SYS_CTRL_CODE[i].oper_code)
#define   SOURCE     (SYS_CTRL_CODE[i].operand)
#define   ADDR       (SYS_CTRL_CODE[i].addr)

  double  L_ctrl_exec(void)

{
  double  value ;
     int  i ;

/*---------------------------------------------------- ������������� */

			    lng_ctrl_abort=0 ;                      /* ����� ������ ���������� */
			    lng_culc_abort=0 ;

#ifdef  __INSIDE_DEBUGER__
    if(lng_user_debug==NULL)  lng_user_debug=L_debug_ctrl ;
#endif

/*------------------------------------------------ ���������� �/���� */

  for(i=lng_start ; ; ) {
			       lng_start=0 ;                        /* ����� ���������� ������ */

    if(lng_ctrl_abort)  break ;                                     /* ���� ������ '���������'          */
								    /*  ���������� ���������� ��������� */
    if(lng_debug)
     if(lng_user_debug!=NULL)  lng_user_debug(i) ;

	if(OPER_CODE==   _SET_OPER) {
				      *SOURCE=ADDR ;
					    i++ ;
				    }
   else if(OPER_CODE== _ALLOC_OPER) {
	      *SOURCE=L_set_pointer((char *)SYS_CTRL_CODE+ADDR) ;
					    i++ ;
				    }
   else if(OPER_CODE==  _CULC_OPER) {
					value=L_culc_exec(ADDR) ;
		    if(SOURCE!=NULL)  *SOURCE=   value ;
					   i++ ;
				    }
   else if(OPER_CODE==    _IF_OPER) {
				      if(*SOURCE<=0)  i=ADDR ;
				      else            i++ ;
				    }
   else if(OPER_CODE==_RETURN_OPER) {
				       return(*SOURCE) ;
				    }
   else                                     i++ ;
			}
/*-------------------------------------------------------------------*/

  return(0.) ;
}


/************************************************************************/
/*                                                                      */
/*          ����������� ������ ��������������� ����������               */

  double  L_culc_exec(start)

   int  start ;  /* ��������� ����� */

{
 struct L_culc *exe ;          /* ��������� ����� ��������. ���� */
	double  result ;
	double  oper1 ;
	double *addr1 ;
	double *addr2 ;
	double *oper2 ;
//	 float *tmp ;
	   int  oper ;
	   int  type ;         /* ��� ������� �������� */

	   int  index1  ;      /* ������ 1-�� �������� */
	   int  index2  ;      /* ������ 2-�� �������� */
	   int  index1_flag ;  /* ���� ���������� 1-�� �������� */
	   int  index2_flag ;  /* ���� ���������� 2-�� �������� */


    union {          void *pntr ;            /* ������ �������������� */
	    unsigned long  dgt ;  }  cvt ;

    union {        double  dbl ;             /* ������ �������������� */
		    short  bin[4] ; }  dbl ;

  double  (*function)(double *) ;


		 exe    =&SYS_CULC_CODE[start] ;
	  vars_stack_cnt=  0 ;
	  lng_ctrl_abort=  0 ;                                      /* ����� ����� ���������� */

	     index1_flag=0 ;
	     index2_flag=0 ;

/*================================================ ������������ ���� */

  for( ; exe->oper_code!=_STOP_OPER ; exe++) {

    if(lng_ctrl_abort | lng_culc_abort)  break ;                    /* ���� ������ '���������'          */
								    /*  ���������� ���������� ��������� */
								    /*   ��� ��������������� ���������  */
	      oper =exe->oper_code & 0770 ;                         /* ��������� ��� �������� */
	      type =exe->oper_code & 0007 ;                         /* ��������� ��� 1-�� �������� */

	      oper2=exe->addr2 ;                                    /* ��������� ����� 2-�� �������� */

/*---------------------------------- ��������� ���� ������� �������� */

	     memset(dbl.bin, 0, sizeof(dbl.bin)) ;

	 if(type==_DATA_OP1) {                                      /* �������� ������ ������� � */
		      memcpy(&dbl.bin[2], &exe->addr1, 4) ;         /*  ����������� �� ��� ����  */
		       oper1=dbl.dbl ;
			     }
    else if(type==_SHFT_OP1) {
		      memcpy(&dbl.bin[2], &exe->addr1, 4) ;
		       oper1=dbl.dbl+shft_base ;
			     }
    else if(type==_LONG_OP1) {
					      exe++ ;
			       memcpy(&oper1, exe, 8) ;
			     }
    else                     {
/*- - - - - - - - - - - - - - - - - - - - - - - ��������� ���������� */
	  if(index1_flag     &&
	      oper!=_IND2_OPER  ) {
					 index1_flag=0 ;

				     addr1 =L_get_pointer(*exe->addr1) ;
				     addr1+=   index1 ;
				  }
     else if(index2_flag     &&
	      oper==_IND2_OPER  ) {
				      addr1 =L_get_pointer(*exe->addr1) ;
				      addr1+=   index2 ;
				   }
     else                             addr1=exe->addr1 ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
		  if(oper!= _CALL_OPER &&
		     oper!=_CALLE_OPER &&
		     oper!=_CALLX_OPER   )  oper1=*addr1 ;
			     }

/*================================================ �������� �������� */

      switch(oper) {

/*------------------------------------------------------- ���������� */

       case _IND1_OPER: {
				  index1     =oper1 ;
				  index1_flag=1 ;
					break ; }

       case _IND2_OPER: {
				  index2     =oper1 ;
				  index2_flag=1 ;
					break ; }

/*-------------------------------------------- ����������� ��������� */

       case _TSEG_OPER : {   cvt.pntr =exe ;
			    shft_base =cvt.dgt+0.5 ;
				  exe+=(unsigned long)oper1 ;
					break ; }

       case _MOVE_OPER : {
			if(index2_flag) {
					   addr2 =L_get_pointer(*oper2) ;
					   addr2+=   index2 ;
					     index2_flag=0 ;
					}
			else               addr2=oper2 ;

					  *addr2=oper1 ;
					break ; }

       case _PUSH_OPER : {  vars_stack[vars_stack_cnt]=oper1 ;
				     vars_stack_cnt++ ;
					break ;  }

       case _CALL_OPER : {
			     function=(void *)exe->addr1 ;
			 lng_pars_cnt= vars_stack_cnt ;
			       *oper2=function(vars_stack) ;
				         vars_stack_cnt=0 ;
					   break ;  }

       case _CALLX_OPER : {
			      lng_pars_cnt=vars_stack_cnt ;

	   if(SYS_EXT_EXEC!=NULL)
		  *oper2=SYS_EXT_EXEC((char *)exe->addr1, vars_stack,
						 vars_stack_cnt) ;
	   else   *oper2=0. ;
					 vars_stack_cnt=0 ;
					   break ;  }

       case _CALLE_OPER : {

#pragma warning(disable : 4311)
	     *oper2=L_call_entry((int)exe->addr1,
				   vars_stack, vars_stack_cnt) ;
#pragma warning(default : 4311)
					 vars_stack_cnt=0 ;
					   break ;  }

/*----------------------------------------- �������������� ��������� */

       case _MULT_OPER : {  *oper2*=oper1 ;  break ;  }

       case  _DEV_OPER : {  if(*oper2==0.)  *oper2=0 ;
			    else            *oper2=oper1/(*oper2) ;
				   break ;  }

       case _DEVC_OPER : {  if(*oper2==0.)  *oper2=0 ;
			    else            *oper2=floor(oper1/(*oper2)) ;
				   break ;  }

       case _DEVQ_OPER : {  if(*oper2==0.)  *oper2=0 ;
			    else            *oper2=floor(oper1/(*oper2)+0.5) ;
				   break ;  }

       case  _ADD_OPER : {  *oper2+=oper1 ;  break ; }

       case  _SUB_OPER : {  *oper2 =oper1-(*oper2) ;  break ; }

       case _POWER_OPER : {    if(oper1<0.)   *oper2=0 ;
				else         {
				    *oper2=pow(oper1, *oper2) ;
				  if(!errno)  *oper2=0 ;
					     }
					break ;  }
/*--------------------------------------------- ���������� ��������� */

       case   _EQ_OPER : {  if(oper1==(*oper2)) *oper2=1. ;
			    else                *oper2=0. ;
				   break ; }

       case   _NE_OPER : {  if(oper1!=(*oper2)) *oper2=1. ;
			    else                *oper2=0. ;
				   break ; }

       case   _LE_OPER : {  if(oper1<=(*oper2)) *oper2=1. ;
			    else                *oper2=0. ;
				   break ; }

       case   _GE_OPER : {  if(oper1>=(*oper2)) *oper2=1. ;
			    else                *oper2=0. ;
				   break ; }

       case   _LT_OPER : {  if(oper1< (*oper2)) *oper2=1. ;
			    else                *oper2=0. ;
				   break ; }

       case   _GT_OPER : {  if(oper1> (*oper2)) *oper2=1. ;
			    else                *oper2=0. ;
				   break ; }

       case  _AND_OPER : {  if(oper1!=0. && (*oper2)!=0.) *oper2=1. ;
			    else                          *oper2=0. ;
				   break ; }

       case   _OR_OPER : {  if(oper1!=0. || (*oper2)!=0.) *oper2=1. ;
			    else                          *oper2=0. ;
				   break ; }

/*-------------------------------------------------------------------*/
		   }
					     }
/*===================================================================*/

	    result=(*oper2) ;

  return(result) ;
}


/************************************************************************/
/*                                                                      */
/*                ���������/���������� ������ �������                   */

    void  L_debug_set(flag)

       int  flag ;  /* ���� ������� */

{
     lng_debug=flag ;
}

#ifdef  __INSIDE_DEBUGER__

/************************************************************************/
/*                                                                      */
/*                ������� ������������ ����                             */

//#include <graph.h>
#include <conio.h>
#include <stdio.h>

#define  SETTEXTPOS      _settextposition
#define  PRINT            printf
#define  CLEARSCREEN()    system("cls")

#undef      _PAGE
#define     _PAGE   15
#define _LIST_MAX   10


    void  L_debug_ctrl(n)

      int  n ;   /* ����� ����� */

{
 struct L_variable *var ;            /* �������� ���������� */
	static int  list[_LIST_MAX]={-1,-1,-1,-1,-1,-1,-1,-1,-1,-1} ;
	static int  row_stop ;       /* ������ ��������� */
	static int  row_prv ;        /* ����� ���������� ������ */
	static int  pause_flag=1 ;   /* ���� ���������� ����� */
	static int  stop_flag ;
	       int  oper ;
	       int  row ;
	    double *addr ;
	       int  key ;
	       int  max ;
	       int  i ;
	       int  j ;


 static char *oper_name[]={  "NOP ",   /* �������� �������� */
			     "SET",
			     "CULC",
			     "IF  ",
			     "RTRN",
			     "ALLC" } ;

 static char *frame_fmt="R:%3d  F:%3d  %s  Oper:%lp  Addr:%d   " ;
 static char *  var_fmt="%3d  %15s  Value:%12.6le  Addr:%lp  " ;

 static char *continue_msg="��������� - ��� ������. ESC - �������.  " ;

#define  _HELP_MAX  14
  static char *help[]={
			    "������� ������ ���������:                ",
			    " R [N]  - ������ �� ������ N             ",
			    " S      - ������ ��������� ����������    ",
			    " E      - ������ ������� ����������      ",
			    " I      - ������ �������.����������      ",
			    " L      - ������ ������ ����������       ",
			    "+{E|I}N - �������� �������./�������      ",
			    "     ���������� ����� N �� �����.������  ",
			    "     � ������ ������ ����������          ",
			    " P+     - ������ � �������               ",
			    " P-     - ������ ��� ����                ",
			    " X      - ������ �� �����                ",
			    " Y      - ��������� ���������� ����      ",
			    " Z      - ��������� ���������            ",
			    " C      - �������� �����                 ",
			    "                                         "
		      } ;

/*---------------------------------------------------- ������������� */

	   oper=SYS_CTRL_CODE[n].oper_code ;
	   addr=SYS_CTRL_CODE[n].operand ;
	    row=SYS_CTRL_CODE[n].text_row ;

	 if(n==0) {  stop_flag= 1 ;
		      row_stop=-1 ;
		       row_prv=-1 ;  }

/*----------------------------------------------- ��������� �������� */

	      if(row_prv!=-1 &&
		 row_prv!=row  ) {
					 row_prv=-1 ;
		     if(row_stop==-1)  stop_flag= 1 ;
		     else              stop_flag= 0 ;
				 }

	      if(row_prv ==-1 &&
		 row_stop==row  ) {
					row_stop=-1 ;
				       stop_flag= 1 ;
				  }

		  if(!pause_flag)  stop_flag=0 ;
/*- - - - - - - - - - - - - - - - - - - - - - �������� ���� �������� */
	  if(oper<_NOP_OPER  ||
	     oper>_ALLOC_OPER  ) {
		    SETTEXTPOS(3, 1) ;
		         PRINT("�������� --- ������ ���� ��������  ") ;
				     stop_flag=1 ;
				 }
/*- - - - - - - - - - - - - - - - - - - - - �������� ������ �������� */
	 if(SYS_EXT_TABLE==NULL)
	  if(oper!=_NOP_OPER)
	      if((addr< &(SYS_EXT_VARS[0].value) ||
		  addr>=&(SYS_EXT_VARS[SYS_EXT_VCNT].value))    &&
		 (addr< &(SYS_INT_VARS[0].value) ||
		  addr>=&(SYS_INT_VARS[SYS_INT_MAX].value)) &&
		  addr!=&vars_if_ind                        &&
		  addr!=&vars_go_ind                        &&
		  addr!=&vars_tmp                             ) {
		    SETTEXTPOS(3, 1) ;
		         PRINT("�������� --- ������ ������ ��������  ") ;
			    stop_flag=1 ;
								}
/*-------------------------------- ����� ���������� � ������ ������� */

       SETTEXTPOS(2, 1) ;
	    PRINT(frame_fmt, row, n, oper_name[oper],
		     SYS_CTRL_CODE[n].operand, SYS_CTRL_CODE[n].addr) ;

        for(j=0 ; j<_LIST_MAX ; j++) {
	          if(list[j]==-1 )   continue ;
	     else if(list[j]< 300)  var=&SYS_EXT_VARS[list[j]] ;
	     else                   var=&SYS_INT_VARS[list[j]-10000] ;

		SETTEXTPOS(j+4, 1) ;
		     PRINT(var_fmt, j, var->name,
				       var->value,
				      &var->value ) ;
				     }


	       if(!stop_flag)  return ;

   do {
	     SETTEXTPOS(1, 1) ;
		  PRINT("Ctrl-debug(Help-?) >                  ") ;
	     SETTEXTPOS(1, 22) ;
		   gets(buff) ;

/*------------------------------------------------- ��������� ������ */

		  strupr(buff) ;                                    /* ������� � ������� ������� */

/*- - - - - - - - - - - - - - - - - - - - - - - - - -  ��������� ��� */
	if(buff[0]==0) {
			   return ;
		       }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - Help */
   else if(buff[0]=='?') {
	  for(i=0 ; i<_HELP_MAX ; i++) {  SETTEXTPOS(i+4, 1) ;
					       PRINT(help[i]) ;  }
			 }
/*- - - - - - - - - - - - - - - - - - - -  ���������/���������� ���� */
   else if(buff[0]=='P') {
			    if(buff[1]=='+')  pause_flag=1 ;
			    else              pause_flag=0 ;
			 }
/*- - - - - - - - - - - - - - - - - - - - - - - - -  ������ �� ����� */
   else if(buff[0]=='X') {
			      stop_flag=0 ;

			      return ;
			 }
/*- - - - - - - - - - - - - - - - - - - - - - -  ��������� ��������� */
   else if(buff[0]=='Y') {
			    lng_ctrl_abort=1 ;
			     return ;
			 }
/*- - - - - - - - - - - - - - - - - - - - - - -  ��������� ��������� */
   else if(buff[0]=='Z') {
			    _exit(77) ;
			 }
/*- - - - - - - - - - - - - - - - - - - - - - -  ��������� ��������� */
   else if(buff[0]=='C') {
			    CLEARSCREEN() ;
			 }
/*- - - - - - - - - - - - - - - - - - - - - - - - - ������ �� ������ */
   else if(buff[0]=='R') {
					  stop_flag= 0 ;
					   row_prv =row ;
			   if(buff[1]!=0)  row_stop=atoi(&buff[1]) ;

			      return ;
			 }
/*- - - - - - - - - - - - - - -  ����� �������� ��������� ���������� */
   else if(buff[0]=='S') {

		SETTEXTPOS(4, 1) ;
		     PRINT(var_fmt, 0, "IF indicator",
					 vars_if_ind,
					&vars_if_ind ) ;

		SETTEXTPOS(5, 1) ;
		     PRINT(var_fmt, 0, "GO indicator",
					 vars_go_ind,
					&vars_go_ind ) ;

		SETTEXTPOS(6, 1) ;
		     PRINT(var_fmt, 0, "Work value",
					 vars_tmp,
					&vars_tmp ) ;
			 }
/*- - - - - - - - - - - - - - - - - - - -  ����� �������� ���������� */
   else if(buff[0]=='E' ||
	   buff[0]=='L' ||
           buff[0]=='I'   ) {
		  if(buff[0]=='E')  max= SYS_EXT_VCNT ;
	     else if(buff[0]=='I')  max= SYS_INT_MAX ;
	     else                   max=_LIST_MAX ;

	    for(i=0 ; i<max ; i+=_PAGE) {
	      for(j=0 ; j<_PAGE && i+j<max ; j++) {

		  if(buff[0]=='E')  var=&SYS_EXT_VARS[i+j] ;
	     else if(buff[0]=='I')  var=&SYS_INT_VARS[i+j] ;
	     else if(list[j]==-1 )   continue ;
	     else if(list[j]< 300)  var=&SYS_EXT_VARS[list[j]] ;
	     else                   var=&SYS_INT_VARS[list[j]-10000] ;

		SETTEXTPOS(j+4, 1) ;
		     PRINT(var_fmt, i+j, var->name,
					 var->value,
					&var->value ) ;
						  }
	      if(i+j<max) {
		SETTEXTPOS(j+4, 10) ;
		     PRINT(continue_msg) ;
		     key=getch() ;
		  if(key==27) {  i=max ;  break ;  }
			  }
					}
			    }
/*- - - - - - - - - - - - - - - - -  ���� ���������� � ������ ������ */
   else if(buff[0]=='+') {

	  for(i=0 ; i<_LIST_MAX ; i++) if(list[i]==-1) break ;

	    if(i==_LIST_MAX) {
		    SETTEXTPOS(3, 1) ;
		         PRINT("ERROR --- LIST OVERFLOW  ") ;
			     }

			  j=atoi(&buff[2]) ;

                  if(buff[1]=='E')  list[i]=j  ;
		  else              list[i]=j+10000 ;
			 }
/*- - - - - - - - - - - - - - - - ����� ���������� �� ������� ������ */
   else if(buff[0]=='-') {
			        i =atoi(&buff[1]) ;
                           list[i]= -1  ;
			 }
/*- - - - - - - - - - - - - - - - - -  ��������� ����������� ������� */
   else                     {
			  SETTEXTPOS(3, 1) ;
			       PRINT("ERROR --- UNKNOWN COMMAND  ") ;
			    }
/*-------------------------------------------------------------------*/

      } while(1) ;

}
#endif  /*   __INSIDE_DEBUGER__  */


/************************************************************************/
/*                                                                      */
/*                   �����������/������������� ������                   */

     double  L_set_pointer(pntr)

       void *pntr ;

{
    union {          void *pntr ;            /* ������ �������������� */
	    unsigned long  dgt ;  }  cvt ;

	 cvt.pntr=pntr ;
  return(cvt.dgt) ;
}


       void *L_get_pointer(pntr)

	 double  pntr ;

{
    union {          void *pntr ;            /* ������ �������������� */
	    unsigned long  dgt ;  }  cvt ;

	 cvt.dgt=pntr ;
  return(cvt.pntr) ;
}

