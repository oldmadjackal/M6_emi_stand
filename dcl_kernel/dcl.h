#ifndef  _DCL_H_
#define  _DCL_H_

#ifndef UNIX
#pragma warning( 3 : 4701 )
#endif

#include "stddef.h"
#include "stdio.h"

//#include "..\\Lang_common\\lang_std.h"

#ifdef  _DCL_MAIN_
#define  _EXTERNAL
#else
#define  _EXTERNAL  extern
#endif

/*---------------------------------------------- �������� � �������� */

#ifdef UNIX

#define              stricmp  strcasecmp
#define              memicmp  memcmp
#define                _stat  stat
#define  GetCurrentProcessId  getpid
#else

#endif

/*--------------------------------- ��������� ������������ ��������� */

#ifndef _DCL_LOAD
#define _DCL_LOAD  (void *)0x0000fff1L  /* ����� DCL-��������� */
#endif

#ifndef _DCL_STD
#define _DCL_STD          3             /* ������� � ��������� DCL */
#endif

#ifndef _DCL_CALL
#define _DCL_CALL         1             /* �������� ����������  DCL */
#endif

#ifndef _DCL_METHOD
#define _DCL_METHOD       2             /* �������� �������  DCL */
#endif

/*---------------------------------------------- ��������� ��������� */

#define    _DCL_NAME_SIZE       32     /* ������������ ������ ����� */
#define    _DCL_PARS_MAX       100     /* ����� ���������� �/� */
#define    _DCL_IFFOR_MAX       20     /* ����� ����� ��������� IF � FOR ���������� */
#define    _DCL_OPER_MAX        10     /* ����� ������������� */


#ifdef  DCL_INSIDE

#define    _VERSION     "2.00.001 2016/05/19"

#define    _NAME_SIZE    _DCL_NAME_SIZE   
#define    _PARS_MAX     _DCL_PARS_MAX
#define   _IFFOR_MAX     _DCL_IFFOR_MAX
#define    _OPER_MAX     _DCL_OPER_MAX

#define    _CALL_MAX       500     /* ����� ����������� */
#define   _STACK_MAX        10     /* ������ ����� ������� */

#define   _TITLE_PAGE     1000     /* ������ �������� ���������� */
#define     _DYN_CHUNK   10000     /* ����������� ������ ����������� ��� ������������ �������� */

#define  _TBLOCK_SIZE    64000     /* ������������ ������ ���������� ����� */

#define    _BUFF_SIZE     2048

#endif

/*--------------------------------- ��������� �������� ��������� DCL */

typedef struct {                                 /* �������� � ��������� DCL */
	        unsigned int  type ;                  /* ��� ����������/��������� */
	        unsigned int  func_flag ;             /* ���� ���������(��� �������) */
	        unsigned int  work_nmb ;              /* ����� ������� ������ (0 - �� �������)  */
				                      /*  ��� "�����" � ������� DCL-�������     */
				                      /*  ��� ������������ ������������ ������� */
                         int  local_idx ;             /* ������ ���� ������: 0 - ���������� ���������� */      
			char  name[_DCL_NAME_SIZE] ;  /* ��� ����������/��������� */
			void *addr ;                  /* ����� ����������/��������� */
			char *prototype ;             /* ���� ���������� ��������� ��� ������� ������������ ���������� "D" */
			 int  size ;                  /* ������ ���������� */
			 int  buff ;                  /* ����� �����������, ���� -1 - ���������� �������� READ ONLY*/
	       } Dcl_decl ;

/*  ��� ������� ����� ��������� ���������:            */
/*                                                    */
/*        0...7  -  ������� ���                       */
/*        8...b  -  ���������� ����                   */

				/* ������� ��� - ������� �� ������� !!! */
#define  _DCLT_DGT_VAL   0x0002   /* ����� */
#define  _DCLT_DGT_AREA  0x0004   /* ������ ����� ��� ��������� �� ����� */
#define  _DCLT_DGT_PTR   0x0005   /* ��������� �� ������ ����� */
#define  _DCLT_CHR_AREA  0x0008   /* ������(������ ��������) */
#define  _DCLT_CHR_PTR   0x0009   /* ��������� �� ������ */
#define  _DCLT_NULL_PTR  0x0010   /* NULL - ��������� */
#define  _DCLT_XTP_OBJ   0x0020   /* ������ ������������ ���� */
#define  _DCLT_XTP_REC   0x0040   /* ������ ������� ������������ ���� */

				/* ���������� ��� */
#define   _DCLT_SHORT    0x0100   /*            short       */
#define   _DCLT_USHORT   0x0200   /*   unsigned short       */
#define   _DCLT_LONG     0x0300   /*            long        */
#define   _DCLT_ULONG    0x0400   /*   unsigned long        */
#define   _DCLT_FLOAT    0x0500   /*            float       */
#define   _DCLT_DOUBLE   0x0000   /*            double      */
#define   _DCLT_UNKNOWN  0xff00

			      /* ������ ���� */
#define   _DCL_SHORT       (_DCLT_DGT_VAL | _DCLT_SHORT )
#define   _DCL_USHORT      (_DCLT_DGT_VAL | _DCLT_USHORT)
#define   _DCL_LONG        (_DCLT_DGT_VAL | _DCLT_LONG  )
#define   _DCL_ULONG       (_DCLT_DGT_VAL | _DCLT_ULONG )
#define   _DCL_FLOAT       (_DCLT_DGT_VAL | _DCLT_FLOAT )
#define   _DCL_DOUBLE      (_DCLT_DGT_VAL | _DCLT_DOUBLE)
#define   _DCL_CHAR_AREA    _DCLT_CHR_AREA
#define   _DCL_CHAR_PTR     _DCLT_CHR_PTR
#define   _DCL_SHORT_PTR   (_DCLT_DGT_PTR | _DCLT_SHORT )
#define   _DCL_USHORT_PTR  (_DCLT_DGT_PTR | _DCLT_USHORT)
#define   _DCL_LONG_PTR    (_DCLT_DGT_PTR | _DCLT_LONG  )
#define   _DCL_ULONG_PTR   (_DCLT_DGT_PTR | _DCLT_ULONG )
#define   _DCL_FLOAT_PTR   (_DCLT_DGT_PTR | _DCLT_FLOAT )
#define   _DCL_DOUBLE_PTR  (_DCLT_DGT_PTR | _DCLT_DOUBLE)

#define   _DCLT_BASE_MASK    0x00ff
#define   _DCLT_MODE_MASK    0xff00

#define   DCL_WORK_NUM(m_idx, p_idx) (m_idx<<16 | p_idx)
#define   DCL_WORK2M(n)              (n>>16)
#define   DCL_WORK2P(n)              (n & 0x00ffff)

#define  _DCL_MAIN_P                  0x00fff

#define _DCL_LOAD  (void *)0x0000fff1L  /* ����� DCL-��������� */

/*  ������������� ������������ �������: */

#define  _DCL_MARKED_ONLY      0x0001    /* ������������ ����� ������ "MARKED ONLY" */
#define  _DCL_XTRACE           0x0002    /* �������� �������� ��������� */

/*---------------------------------------------- ��������� ��������� */

typedef struct {                   /* �������� ���������� �������� */
                    char *mnemo ;   /* ��������� */
                     int  code ;    /* ��� */
               } Dcl_decode ;

typedef struct {                            /* �������� ������������ ���� */
		   char  name[_DCL_NAME_SIZE] ;  /* ��������� ������������ ���� */
                    int  size ;              /* ����� ������ ���� */
               Dcl_decl *list ;              /* ������ ����������� */
                    int  list_cnt ;
	       } Dcl_complex_type ;

typedef struct {                            /* ��������� ������ ������������ ���� */
                     void *next_record ;     /* ������ �� ��������� ������ */
                 Dcl_decl *elems ;           /* ������ ��������� ������ */
                      int  elems_cnt ;
                      int  size ;            /* ����� ������ ������ */
                      int  find_mark ;       /* ����� ������ FIND */
                      int  exec_mark ;       /* ��������� ����� ��������� */
                 Dcl_decl  fld_name ;        /* ������ �� ��� �������� ���� - $name */
                 Dcl_decl  fld_value ;       /* ������ �� �������� �������� ���� - $value */
                      int  fld_idx ;
               } Dcl_complex_record ;

typedef struct Dcl_call {                       /* �������� ������ DCL-��������� */
                 Dcl_decl  decl ;                /* �������� ��� ��������� ������� */ 
		     char  name[_DCL_NAME_SIZE] ;    /* ��� ������� */
		     char *path ;                /* ��� ����� */
		      int  mem_flag ;            /* ���� ���������� � ������ */
		     long  file_addr ;           /* �������� ����� ������� */
		     char *mem_addr ;            /* ����� ������� � ������ */
          struct Dcl_call *proc_list ;           /* ������ ��������� � ����������� �������� */
                      int  proc_cnt ;
                      int  v_flag ;              /* ���� ����������� ��������� */
               } Dcl_call ;

typedef struct {                                /* �������� TEXT-����� */
		     char  name[_DCL_NAME_SIZE] ;    /* ��� TEXT-����� */
		     char *data ;                /* ������ */
               } Dcl_tblock ;

typedef struct {                                /* �������� ����� IF-FOR �������� */ 
		     char  if_for_flag ;         /* ���� ���� ���������: 1-FOR, 0-IF */
		     char  else_flag ;           /* ���� ����� IF ���������: 1-ELSE, 0-IF */
		     char  block_flag ;          /* ���� ������ � ������� ���������� */
		     char  pass_sts ;            /* ��� ������� ��������� */
		     long  addr ;                /* ������ ����� - ����� ������ */
		      int  oper ;                /* ������ ����� - �������� */
		      int  row ;                 /* ������ ����� - ������ */
               } Dcl_if_for ;

typedef struct {                               /* �������� ������� �������� � ���������� */
		     char *beg ;                /* ������ ��������� */
		     char *end ;                /* ��������� ��������� */
		      int  type ;               /* ��� ��������� */
               }  Dcl_oper ;

/*--------------------------------------- �������� ������ Lang_iface */

  class Lang_iface {

   public : 
                     int   mDebug_flag  ;                 /* ���� ������� */
                     int (*mDebug)(void) ;                /* ���������������� ��������� ������� */
                     int   mDebug_trace  ;                /* ���� ����������� */
                    char   mDebug_path[FILENAME_MAX] ;    /* ���� � ����� ���� */

                     int   mError_code ;                  /* ��� ������ */
                    char   mError_details[1024] ;         /* �������� ������ */
                    char  *mError_file ;                  /* ���� � ������� */
                    char  *mError_position ;              /* ��������� "�������" ����� */

                    char   mProgram_name[FILENAME_MAX] ;  /* �������� ��������� */
	             int   mFile_flag ;                   /* ���� ���������� ��������� � ����� */
	             int   mFile_hdl ;                    /* ��������� ����� ��������� */
                    char  *mFile_mem ;                    /* ����� ��������� � ������ */
                     int   mRow ;                         /* ������� ����� ��������� */
                    long   mByte_cnt ;                    /* ������� ������ ����� */

                    char  *mWorkDir ;                     /* ������� ������ */
                    char **mLibDirs ;                     /* �������� ��������� (NULL-���������������) */

                    void  *mVars ;                        /* ������ �� ������ ����������� �������� */

                    char  *mProgramFile ;                 /* ���� ��������� */
                    char  *mProgramMem ;                  /* ����� � ���������� � ������ */

            virtual void   vFree   (void){} ;             /* ������������ �������� */
            virtual void   vProcess(void){} ;             /* ���������� ��������� */
            virtual char  *vDecodeError(int){             /* ��������� ������ ������ �� ���� */
                                             return(NULL) ;
                                            } ;          

                           Lang_iface(){                  /* ����������� */  
                                          mError_file    =NULL ;
                                          mError_code    =  0 ; 
                                          mRow           =  0 ;
                                          mError_position=NULL ;
                                          mWorkDir       =NULL ;
                                          mLibDirs       =NULL ;
                                          mDebug_flag    =  0 ;
                                          mDebug         =NULL ;
                                          mDebug_trace   =  0 ;
                                         *mDebug_path    =  0 ;
                                       } ;               

                          ~Lang_iface(){} ;               /* ���������� */
                 } ;
/*---------------------------------------------- �������� ������ DCL */

  class Lang_DCL : public Lang_iface {

   public : 

              virtual void   vFree       (void) ;   /* ������������ �������� */
              virtual void   vProcess    (void) ;   /* ���������� ��������� */
              virtual char  *vDecodeError(int) ;    /* ��������� ������ ������ �� ���� */

	                     Lang_DCL() ;           /* ����������� */
	                    ~Lang_DCL() ;           /* ���������� */

    static  class Lang_DCL  *ActiveInstance ;
 
   public : 

    /* ������ ���������� ���������������� ������� mExt_lib �������� */
    /* ��� ������������� �������, � ������ - ��� ���������� ���.    */
    /* ���� ������ ������ - ������������ ��������� �� ��� ��������, */
    /* � ���� ��� - NULL.                                           */
             Dcl_decl *(*mExt_lib)(int, char *) ;     /* ������� ������� ���������� */
#define                  _DCL_LOAD_VAR    1            /* ���������� ���������� */
#define                  _DCL_LOAD_FNC    2            /* ���������� ������� */

   public : 
                  int   nInit_flag ;                  /* ���� ������������� */ 

     Dcl_complex_type  *nX_types ;                    /* �������� ����������� ����� */
                  int   nX_cnt ;

             Dcl_decl  *nPars_table[_DCL_PARS_MAX] ;      /* ������� ���������� */
                  int   nPars_cnt ;                   /* ������� ���������� �������� ������ */
                  int   nPars_cnt_all ;               /* ��������� ������� ���������� */
             Dcl_decl  *nTransit ;                    /* ������� ���������� ���������� */
                  int   nTransit_cnt ;                /* ����� ���������� ���������� */
                  int   nTransit_num ;                /* ����� ����������� ��������� */
             Dcl_decl   nReturn ;                     /* �������� �������� */
                 void  *nRet_data ;                   /* ����� ������ �������� */

             Dcl_call   nCall_main ;                  /* ������� main-��������� */
             Dcl_call  *nCall_list ;                  /* ���-���� ������� */
                  int   nCall_list_cnt ;
             Dcl_call **nCall_stack ;                 /* ���� ������� �������� */
                  int   nCall_stack_ptr ;             /* ��������� ����� ������� �������� */
                  int   nLocal ;                      /* ������ ������ */

             Dcl_decl **nVars ;                       /* �������� �������(?) ���������� � �������� */
                  int   nBuff_max ;                   /* ������������ ������ ������ ���������� */
             Dcl_decl   nResult ;                     /* �������� ���������� */
  	          int   nResult_dst ;                 /* ������ ���������� ���������� */
                  int   nIf_ind ;                     /* ��������� ������� */
               double   nDgt_val ;                    /* �������� �������� */
	         char  *nChr_pnt ;                    /* '������' �������� ������ */
//               char   nConv[128] ;                  /* ����� �������������� */
//           Dcl_decl  *nTarget  ;                    /* �������� ������ ���������� � ��������� */
             Dcl_decl   nTmp ;                        /* ������-������ */
             Dcl_decl   nVoid ;

             Dcl_decl **nAll_vars ;                   /* ����� ������� ������� ���������� */
             Dcl_decl  *nInt_page ;                   /* �������� ���������� �����.���������� */
                  int   nInt_cnt ;                    /* ������� ���������� ���������� ���������� */
             Dcl_decl  *nWork_page ;                  /* �������� ���������� ������� ���������� */
                 void **nWork_mem ;                   /* �������� ��������� ������ ������� ���������� */
                  int   nWork_cnt ;                   /* ������� ������� ���������� */  
                  int   nFix_flag ;                   /* ���� ���������� ������ �������������� ��� ������� ����� ���������� ��������� */
           Dcl_tblock  *nT_blocks ;                   /* ������ ��������� ������ */
                  int   nT_blocks_cnt ;

           Dcl_if_for   nId_stk[_DCL_IFFOR_MAX] ;     /* ���� IF-DO ���������� */
                  int   nId_stk_cnt ;                 /* ��������� ����� IF-DO ���������� */
                  int   nId_num ;                     /* ��������� "�������� �����" IF_DO ����� */
                  int   nNext_flag ;                  /* ���� ���������� ������� ����� */

                  int   nSubrow_cnt ;                 /* ������� �������� */
             Dcl_oper   nOper[_DCL_OPER_MAX] ;        /* ������ �������� ���������� */
                  int   nOper_cnt ;                   /* ������� ���������� */
                  int   nOper_crn ;                   /* ����� ������� ��������� */
                  int   nOper_next ;                  /* ����� ���������� ��������� ��� �������� �� ������ ����� */

		 long   nSeg_start ;                  /* ��������� ����� ������������ �������� */
		  int   nSeg_row  ;                   /* ��������� ������ ������������ �������� */
                  int   nPass_sts ;                   /* ������ ������� */

                  int   iIgnoreSizeCorrection ;       /* ������ ���������� ��������� �������� �� �����-������� */

                 char   nCur_path[FILENAME_MAX] ;     /* ������� ������ �� */
//                int   nCnum_flag ;                  /* ���� ����������-��������� ������ ��������� */
                  int   nIndex_flag ;                 /* ���� ���������� � �������� */
                  int   nFragm_flag ;                 /* ���� ���������� � ������������� �������� */

                                                                    /* ���� DCL_INT.C */
                  int   Interpritator  (int, char *, Dcl_decl **) ;  /* DCL-������������� */
                                       
                  int   Stack          (int) ;                       /* ���������� ���������� �������������� */
#define                  _DCL_POP           -1                      /*  ������� �� �����  */
#define                  _DCL_PUSH           1                      /*  ������� � ����    */
#define                  _DCL_CREATE_STACK   2                      /*  ������� ����      */
#define                  _DCL_DELETE_STACK  -2                      /*  ���������� ����   */
#define                  _DCL_CLEAR_STACK    0                      /*  �������� ����     */
                  int   iSysin         (char *, long, int) ;         /* �/� ����� ��������� */
                 char  *iLeet          (char *) ;                    /* �����.��������� ���������� */
                  int   iIef_close     (int) ;                       /* �������� IF, ELSE � FOR ���������� */

                                                                    /* ���� DCL_CULC.C */
                 char  *iCulculate     (char *) ;                    /* ���������� ��������� */
                 char  *iSimple_string (char *) ;                    /* ��������� ������� ����� */
                  int   iProcessor     (int, Dcl_decl *,             /* ��������� */
			                     Dcl_decl * ) ;
             Dcl_decl  *iComplex_var   (char *, char *, Dcl_decl *); /* ���. ������������ ���������� ��� ��������� */
             Dcl_decl  *iFind_var      (char *, int) ;               /* ����� ���������� ��� ��������� �� ����� */
                 char  *iFind_index    (Dcl_decl *, Dcl_decl *) ;    /* ����� ��������� ������ � ������� ������ */
                  int   iCulc_stack    (int) ;                       /* ���������� ���������� ���������� */

                                                                    /* ���� DCL_CMMN.C */
                 char  *iFind_close    (char *, int) ;               /* ����� ������.������� �� �����. � ������ */
                 char  *iFind_open     (char *, char *, int) ;       /* ����� �������.������� �� �����. � ������ */
#define                  _ANY_OPER          0
#define                  _ANY_OPER_OR_END   1
#define                  _ANY_CLOSE         2
                  int   iDgt_size      (int) ;                       /* ����������� ����� ��������� �������� */
               double   iDgt_get       (void *, int) ;               /* ���������� ��������� �������� */
               double   iDgt_set       (double, void *, int) ;       /* ��������� ��������� �������� */
                 void   iSize_correct  (Dcl_decl *) ;                /* �������� ��������� �������� �������� */
                 void   iSize_correct  (Dcl_decl *, Dcl_decl *) ;
                 void   iIncl_correct  (Dcl_decl *, char *,          /* �������� ��������� �������� �������� ��� �������� INCLUDE */
			                            char *, int) ;
                 void   iBuff_realloc  (Dcl_decl *,                  /* �������������� ������������� ������� */
                                        Dcl_decl *, size_t) ;
               double   iStrToNum      (char *, char **) ;           /* ������� ���������� ������ � ����� */
                 void   iNumToStr      (Dcl_decl *, char *) ;        /* ������� ����� � ���������� ������ */
                 void   iLog           (char *, char *) ;            /* ����� ���������� � ���������� ��� */

                                                                    /* ���� DCL_XOBJ.C */
                  int   iXobject_check (char *, char *, char **) ;   /* �������� �� ������� �������� "����������������" */
                  int   iXobject_clear (Dcl_decl *) ;                /* ������� �������� ������� ������������ ���� */
                  int   iXobject_add   (Dcl_decl *,                  /* ���������� ������ ��� ������� ������������ ���� */
                                        Dcl_complex_type *) ;
                  int   iXobject_index (Dcl_decl *, int *,           /* �������������� ������� ������������ ���� */
                                        Dcl_decl *        ) ;
                  int   iXobject_set   (Dcl_decl *,                  /* ������� �������� ������� ������ ��� ������� ������������ ���� */
                                        Dcl_decl *, int) ;
                  int   iXobject_delete(Dcl_decl *) ;                /* �������� ������� ������� ������� ������������ ���� */
                  int   iXobject_extend(Dcl_decl *, Dcl_decl *) ;    /* ���������� ���� � ��������� ������� ������� ������������ ���� */
                  int   iXobject_sort  (Dcl_decl *, char *) ;        /* ���������� ������� ������� ������������ ���� �� ��������� ���� */
                  int   iXobject_diff  (Dcl_decl *, Dcl_decl *,      /* ��������� ������� ������� �������� ������������ ���� */
                                        Dcl_decl *, char *, char *) ;
             Dcl_decl  *iXobject_field (Dcl_complex_record *,        /* �������� ��� ������ ������ ������� ������������ ���� */
                                                      char *, int) ;
             Dcl_decl  *iXobject_method(char *, char *) ;            /* �������� ������ ������ ������� ������������ ���� */
             Dcl_decl  *iXobject_invoke(Dcl_decl *, Dcl_decl *,      /* ���������� ������ ������� ������������ ���� */
                                        Dcl_decl **,  int       ) ;

                                                                    /* ���� DCL_DECL.C */
                  int   iDecl_detect   (char *) ;                    /* ����������� �������� ���������� */
                  int   iDecl_maker    (char *, Dcl_decl *) ;        /* ��������� �������� ���������� */
                  int   iType_define   (int, int, int, int) ;        /* ����������� ���� ���������� */
                 void  *iTest_var      (char *, int) ;               /* �������� ������� ���������� */
#define                  _DCL_INTERNAL  0
#define                  _DCL_EXTERNAL  1
                                                                    /* ���� DCL_CALL.C */
		 void   iCall_init     (void) ;                      /* ������������� ������� ��������� DCL-��������� */
		 void   iCall_free     (void) ;                      /* ������������ �������� ������� ��������� DCL-��������� */
             Dcl_decl  *iCall_find     (char *) ;                    /* ����� DCL-��������� */
             Dcl_decl  *iCall_exec     (int) ;                       /* ���������� DCL-��������� */
             Dcl_call  *iCall_file     (char *, char *) ;            /* ����� DCL-��������� � ������ */
             Dcl_call  *iCall_lib      (char *, char *) ;            /* ����� DCL-��������� � ����������� */
                 void   iCall_proc_list(Dcl_call *) ;                /* ����������� ������ ��������� � ����������� �������� */
                 void   iCall_final    (void) ;                      /* ���������� �������� ���������� ������������ �������� */

                                                                    /* ���� DCL_MEM.C */
             Dcl_decl **iMem_init      (Dcl_decl **) ;               /* ������������� ������� ���������� ������� */
                 void   iMem_free      (void) ;                      /* �������� ������� ���������� ������� */
             Dcl_decl  *iNext_title    (int) ;                       /* ������ ������ ���������� ��������� */
                 void  *iAlloc_var     (int, Dcl_decl *, int) ;      /* ���������� ���������� � ������������ ������ */
#define                        _WORK_VAR   1                         /* ������� ���������� */
#define                    _INTERNAL_VAR   2                         /* ���������� ���������� */
//#define                     _BLOCK_VAR   3                         /* ������ ���� BLOCK */
#define                  _CLEAR_WORK_VAR   4                         /* ���������� ������� ���������� */
              Dcl_decl *iGet_work      (int) ;                       /* ������ ��������� ������� ������ �� �� ������ */
                   int  iMem_stack     (int) ;                       /* ���� ���������� ���������� ������� */

                                                                    /* ���� DCL_Z_FILELIST.C */
                   int  zGetFilesByMask  (Dcl_decl *, char *) ;      /* ���������� ������ ������ �� ����� */
                   int  zGetFilesByFolder(Dcl_decl *, char *) ;      /* ���������� ������ ������ �� ����� */
                  char *zGetMaskFiles    (char *) ;                  /* ��������� ������ �������� ������� */
                   int  zTestMask        (char *, char *) ;          /* �������� ��������� �� ����� */

                                                                    /* ���� DCL_Z_SQL.C */
                   int  zSqlSelectOnce (Dcl_decl *,                  /* ������� ���������� SELECT-������� */
                                        Dcl_decl **, int) ; 
                   int  zSqlDmlExecute (Dcl_decl *,                  /* ���������� DML-��������� �� ���� ��������� ������� */
                                        Dcl_decl **, int) ; 
                   int  zSqlDescribe   (Dcl_decl *,                  /* ��������� �������� ���������� ������� */
                                        Dcl_decl **, int) ; 
                   int  zDbfRead       (Dcl_decl *,                  /* ���������� DBF-����� */
                                        Dcl_decl **, int) ; 

                                                                    /* ���� DCL_Z_MIDAS.C */
                   int  zMidasApiNew   (Dcl_decl *,                  /* �������� ������� ��������� MIDAS API */
                                        Dcl_decl **, int) ; 
                   int  zMidasApiSend  (Dcl_decl *,                  /* �������� ��������� MIDAS API */
                                        Dcl_decl **, int) ; 
                   int  zMidasApiErrors(Dcl_decl *,                  /* ���������� ������ MIDAS API */
                                        Dcl_decl **, int, char *, int) ; 

                                                                    /* ���� DCL_Z_QC.C */
                   int  zQC_ResultNew  (Dcl_decl *,                  /* ���������� ����� ������ QC-result */
                                        Dcl_decl **, int) ; 
                   int  zQC_ResultSend (Dcl_decl *,                  /* ��������� ������� QC-result */
                                        Dcl_decl **, int) ; 
                   int  zQC_Errors     (Dcl_decl *,                  /* ���������� ������ ������ � QC */
                                        Dcl_decl **, int, char *, int) ; 

                                                                    /* ���� DCL_Z_EMAIL.C */
                   int  zEMailNew      (Dcl_decl *,                  /* ���������� ����� ������ EMAIL */
                                        Dcl_decl **, int) ; 
                   int  zEMailSend     (Dcl_decl *,                  /* �������� Email-��������� */
                                        Dcl_decl **, int) ; 
                   int  zEMailErrors   (Dcl_decl *,                  /* ���������� ������ ������ � EMAIL */
                                        Dcl_decl **, int, char *, int) ; 

                                                                    /* ���� DCL_Z_ABTP.C */
                   int  zAbtpSignal    (char *, char *, char *,      /* �������� ��������� ABTP:SIGNAL */
                                                char *,  int   ) ;

                                                                    /* ���� DCL_Z_STRING.C */
                   int  zParseByTemplate(char *, int, char *, int,   /* ������ ������ �� ������� */
                                                      char *, int,
                                                  Dcl_decl *, int ) ;
                   int  zFormByTemplate (char *, int, char *, int,   /* ������������ ������ �� ������� */
                                                      char *, int,
                                                  Dcl_decl *, int ) ;
                   int  zCodePageConvert(char *, int,                /* �������������� ������� �������� */
                                                char *, char *) ;
                  void  zDosToWin       (char *, long) ;             /* �������������� ������� �������� DOS->WIN */
                  void  zWinToDos       (char *, long) ;             /* �������������� ������� �������� WIN->DOS */
                  long  zUTF8ToWin      (char *, long, char *) ;     /* �������������� ������� �������� UTF8->WIN */
                  long  zWinToUTF8      (char *, long, char *) ;     /* �������������� ������� �������� WIN->UTF8 */

                                   } ;

/*----------------------------------- �������� ���������� � �������� */

#define  _DGT_VAL     _DCLT_DGT_VAL     /* ����� */
#define  _DGT_AREA    _DCLT_DGT_AREA    /* ������ ����� ��� ��������� �� ����� */
#define  _DGT_PTR     _DCLT_DGT_PTR     /* ��������� �� ������ ����� */
#define  _CHR_AREA    _DCLT_CHR_AREA    /* ������(������ ��������) */
#define  _CHR_PTR     _DCLT_CHR_PTR     /* ��������� �� ������ */
#define  _XTP_OBJ     _DCLT_XTP_OBJ     /* ������ ������������ ���� */
#define  _XTP_REC     _DCLT_XTP_REC     /* ������ ������� ������������ ���� */
#define  _NULL_PTR    _DCLT_NULL_PTR    /* NULL - ��������� */

#define  t_base(a)   (a & _DCLT_BASE_MASK)
#define  t_mode(a)   (a & _DCLT_MODE_MASK)

/* ��������� ���������� ������� */
#define  _F_ANY_TYPE   'a'   /* ������������ ��� */
#define  _F_DGT_VAL    'v'   /* ����� */
#define  _F_DGT_PTR    'd'   /* ��������� �� ������ ����� */
#define  _F_CHR_PTR    's'   /* ��������� �� ������ */

/*------------------------------------------------- �������� ������� */

#define  _DCL_MEMORY   0
#define  _DCL_FILE     1
#define  _DCL_INIT     2

/*----------------------------------------------------------- ������ */

#define  _DCLE_FILE_OPEN       2    /* ������ �������� ����� */
#define  _DCLE_STRING_LEN      3    /* ������� ������� ������ */
#define  _DCLE_NOCLOSE_STRING  4    /* ���������� ������� */
#define  _DCLE_MARK_SIMBOL     5    /* ������������ ����� */
#define  _DCLE_MARK_LEN        6    /* ������� ������� ����� */
#define  _DCLE_IF_CLOSE        7    /* �������� ����.������ ��������� IF */
#define  _DCLE_FOR_CLOSE       8    /* �������� ����.������ ��������� ����� */
#define  _DCLE_RETURN_CLOSE    9    /* �������� ����.������ ��������� RETURN */
#define  _DCLE_FOR_TTL_SEL    10    /* ������� ����������� ������ ��������� */
#define  _DCLE_FREE_OPEN      11    /* �������� '����������' ����� */
#define  _DCLE_FREE_CLOSE     12    /* �������� ����������� ����� */
#define  _DCLE_FREE_CONTINUE  14    /* '���������' �������� CONTINUE */
#define  _DCLE_FREE_BREAK     15    /* '���������' �������� BREAK */
#define  _DCLE_FREE_ELSE      16    /* ELSE ��� IF-� */
#define  _DCLE_OPEN           17    /* ���������� ��������� � ������� */
#define  _DCLE_CLOSE          18    /* ������ ����������� ������ */
#define  _DCLE_UNKNOWN_NAME   19    /* ����������� ���������� ��� ��������� */
#define  _DCLE_SINTAX_NAME    20    /* ������������ ������� ����� */
#define  _DCLE_NOINDEX        21    /* ������ ��� ���������� ��������� ��������� */
#define  _DCLE_INDEXED_VAR    22    /* �������������� ������� ���������� */
#define  _DCLE_INDEX_TYPE     23    /* ������������ ��� ������� */
#define  _DCLE_INDEX_FRG      24    /* ������������� �������������� ����������� ������� */
#define  _DCLE_INVALID_CHR    25    /* ������������ ������ � ���������� ��������� */
#define  _DCLE_BAD_OPERATION  26    /* ������������ �������� */
#define  _DCLE_BAD_LEFT       27    /* ������������ ����� ������� */
#define  _DCLE_BAD_RIGHT      28    /* ������������ ������ ������� */
#define  _DCLE_EMPTY_LEFT     29    /* ����������� ����� ������� */
#define  _DCLE_EMPTY_RIGHT    30    /* ����������� ������ ������� */
#define  _DCLE_EMPTY_BOTH     31    /* �������� ��� ��������� */
#define  _DCLE_BINARY_OPER    32    /* �������� �������� � ����� ��������� */
#define  _DCLE_UNARY_OPER     33    /* ������� �������� � ����� ���������� */
#define  _DCLE_PREFIX_OPER    34    /* �������� ������ ���� ���������� */
#define  _DCLE_EMPTY_DEST     35    /* ��� �������� ����������� '��������' */
#define  _DCLE_DIF_TYPE       36    /* ��� '���������' �� �����.���� ���������� */
#define  _DCLE_CONST_DEST     37    /* '��������' �������� ���������������� */
#define  _DCLE_PROTOTYPE      38    /* ��� ��������� �� ������������� ��������� */
#define  _DCLE_TYPEDEF_ELEM   39    /* ����������� ������� ������������ ������� */
#define  _DCLE_TYPEDEF_METH   40    /* ����������� ����� ������������ ������� */
#define  _DCLE_TYPEDEF_SORT   41    /* ���� ���������� ����� ��������� ��� �� �������������� ��� */
#define  _DCLE_TYPEDEF_AREA   42    /* ������������ ������ � ����������� ������� */
#define  _DCLE_TBLOCK_CLOSE   43    /* �� �������� ��������� ���� */
#define  _DCLE_TBLOCK_SIZE    44    /* ������� ������� ��������� ���� */
#define  _DCLE_READONLY       45    /* ������� ��������� READONLY-���������� */

#define  _DCLE_PRAGMA_UNK    201    /* ����������� ������ */

#define  _DCLE_BAD_PREF      401    /* ������������ ������� */
#define  _DCLE_DBL_MOD       402    /* ������������ �������� */
#define  _DCLE_NO_TYPE       403    /* �������� �� ������ ������� ��� */
#define  _DCLE_PTR_AREA      404    /* ������ ���������� �������� */
#define  _DCLE_BLOCK_DECL    405    /* ������ BLOCK ������ ���� ������� �������� */
#define  _DCLE_TYPEDEF_IN    406    /* ��������� �������� typedef */
#define  _DCLE_TYPEDEF_STR   407    /* ������������ ��������� ��������� typedef */
#define  _DCLE_TYPEDEF_RDF   408    /* ��������������� ���� */
#define  _DCLE_TYPEDEF_NAME  409    /* ��� ���� �� ������������� ����������� */
#define  _DCLE_NAME_INUSE    410    /* ��� ���������� ��� ������������ */

#define  _DCLE_CALL_INSIDE   501    /* ������-����������� ������ */

#define  _DCLE_USER_DEFINED  601    /* ������ ��������� ���������� */

#define  _DCLE_ELIB_INIT     701    /* ������ ����.������� ������� ��������� */
#define  _DCLE_ELIB_DECL     702    /* ������������ �������� ������� �� ������� ���������� */
#define  _DCLE_ELIB_LOAD     703    /* ��������� �������� ������� �� ������� ���������� */

#define  _DCLE_MEMORY        801    /* ���������� ������ */
#define  _DCLE_MEMORY_VT     802    /* ���������� ������ ��� ���������� ���������� */
#define  _DCLE_MEMORY_VA     804    /* ���������� ������ ��� ���������� */
#define  _DCLE_LOAD_LIST     808    /* ������ ����� ������������ ���������� */

#define  _DCLE_STACK_OVR     809    /* ������������ ����� */
#define  _DCLE_STACK_UND     810    /* ���������� ������� ����� */
#define  _DCLE_STACK_MEM     811    /* ������������ ������ ��� ����� */

#define  _DCLE_UNKNOWN       901    /* ����� ��� � ������� */
#define  _DCLE_EMPTY         902    /* ������ ������� ��������� */
#define  _DCLE_WORK_FAULT    903    /* ���� ��������� ��������� ������� ������ */

/*--------------------------- �������� ����������� ������� ��������� */

#define  ALFANUM_CHAR(c)  (isalnum(c) || (c>='�' && c<='�') \
				      || (c>='�' && c<='�') \
				      || (c>='�' && c<='�'))

#define   DIGITS_CHAR(c)  (isdigit(c) || c=='+' || c=='-' \
				      || c=='e' || c=='E' \
				      || c=='.'          )

#define     NAME_CHAR(c)  (isalnum(c) || c=='_' || c=='$' \
						|| c=='#')

#define      VAR_CHAR(c)  (isalnum(c) || c=='_' || c=='$' \
				      || c=='[' || c==']' \
				      || c=='(' || c==')' \
				      || c==',' || c=='#')

#define     SPEC_CHAR(c)  (isalnum(c) || c=='_' || c=='$' \
				      || c=='[' || c==']' \
				      || c=='(' || c==')' \
				      || c==',' || c==':' \
				      || c=='+' || c=='-' \
				      || c=='#' || c=='.')

#define     OPER_CHAR(c)             (   c=='+' || c=='-' \
				      || c=='*' || c=='/' \
				      || c=='^' || c=='!' \
				      || c=='=' || c=='>' \
				      || c=='<' || c=='&' \
				      || c=='|' || c=='@' )

/*----------------------------------------------- ������� ���������� */

// extern Dcl_decl  dcl_debug_lib[] ;  /* ���������� ������ ������� */
// extern Dcl_decl  dcl_std_lib[] ;    /* ��������� ���������� */
// extern Dcl_decl  dcl_file_lib[] ;   /* ���������� ������ � ������� */
// extern Dcl_decl  dcl_sql_lib[] ;    /* ���������� ������ � SQL-��������� */

/*-------------------------------------------------------------------*/

#endif
