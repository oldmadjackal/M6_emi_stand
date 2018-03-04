/*********************************************************************/
/*                                                                   */
/*                   DATA CONVERTION LANGUAGE                        */
/*                                                                   */
/*                       ������� ���������                           */
/*                                                                   */
/*********************************************************************/

#if defined(AIX) || defined(LINUX)
#ifndef UNIX
#define  UNIX
#endif
#endif


#include <memory.h>
#ifndef UNIX
#include <conio.h>
#endif

#define  _DCL_MAIN_
#define   DCL_INSIDE

#include "dcl.h"

/********************************************************************/
/*								    */
/*		       ����������� ����������			    */

  class Lang_DCL *Lang_DCL::ActiveInstance ;


/*********************************************************************/
/*                                                                   */
/*                          �����������                              */

     Lang_DCL::Lang_DCL(void)

{
             mExt_lib     =NULL ;

             nInit_flag   =  0 ;

             nX_types     =  0 ;
             nX_cnt       =  0 ;

             nAll_vars    =NULL ;

             nCall_list   =NULL ;
             nCall_stack  =NULL ;

             nT_blocks    =NULL ;
             nT_blocks_cnt=  0 ;

             nPars_cnt    =  0 ;
             nPars_cnt_all=  0 ;
             nTransit     =NULL ;
             nTransit_cnt =  0 ;
             nTransit_num =  0 ;
     memset(&nResult, 0, sizeof(nResult)) ;
     memset(&nReturn, 0, sizeof(nReturn)) ;
             nRet_data    =NULL ;

     memset(&nVoid, 0, sizeof(nVoid)) ;
     memset(&nTmp , 0, sizeof(nTmp )) ;

    iIgnoreSizeCorrection =  0 ;

           ActiveInstance = this ;
}


/*********************************************************************/
/*                                                                   */
/*                          ����������                               */

     Lang_DCL::~Lang_DCL(void)

{
            vFree() ;
}


/*********************************************************************/
/*                                                                   */
/*                       ������������ ��������                       */

    void  Lang_DCL::vFree(void)

{
            iCall_free() ;
             iMem_free() ;
}


/*********************************************************************/
/*                                                                   */
/*                       ���������� ���������                        */

    void  Lang_DCL::vProcess(void)

{
             nSeg_row   = 0 ;
             nSeg_start = 0 ;

             nInit_flag = 0 ;

   if(mProgramFile)  Interpritator(_DCL_FILE,   mProgramFile, (Dcl_decl **)mVars) ;
   if(mProgramMem )  Interpritator(_DCL_MEMORY, mProgramMem,  (Dcl_decl **)mVars) ;
}


/*********************************************************************/
/*                                                                   */
/*              ��������� ������ ������ �� ����                      */

    char *Lang_DCL::vDecodeError(int  code)

{
 static struct {
                  int  code ;
                 char *text ;
               } errors[]={
                            {  _DCLE_FILE_OPEN     , "������ �������� �����"                                },
                            {  _DCLE_STRING_LEN    , "������� ������� ������"                               },
                            {  _DCLE_NOCLOSE_STRING, "���������� �������"                                   },
                            {  _DCLE_MARK_SIMBOL   , "������������ �����"                                   },
                            {  _DCLE_MARK_LEN      , "������� ������� �����"                                },
                            {  _DCLE_IF_CLOSE      , "�������� ����.������ ��������� IF"                    },
                            {  _DCLE_FOR_CLOSE     , "�������� ����.������ ��������� �����"                 },
                            {  _DCLE_RETURN_CLOSE  , "�������� ����.������ ��������� RETURN"                },
                            {  _DCLE_FOR_TTL_SEL   , "������� ����������� ������ ���������"                 },
                            {  _DCLE_FREE_OPEN     , "�������� '����������' �����"                          },
                            {  _DCLE_FREE_CLOSE    , "�������� ����������� �����"                           },
                            {  _DCLE_FREE_CONTINUE , "'���������' �������� CONTINUE"                        },
                            {  _DCLE_FREE_BREAK    , "'���������' �������� BREAK"                           },
                            {  _DCLE_FREE_ELSE     , "ELSE ��� IF-�"                                        },
                            {  _DCLE_OPEN          , "���������� ��������� � �������"                       },
                            {  _DCLE_CLOSE         , "������ ����������� ������"                            },
                            {  _DCLE_UNKNOWN_NAME  , "����������� ���������� ��� ���������"                 },
                            {  _DCLE_SINTAX_NAME   , "������������ ������� �����"                           },
                            {  _DCLE_NOINDEX       , "������ ��� ���������� ��������� ���������"            },
                            {  _DCLE_INDEXED_VAR   , "�������������� ������� ����������"                    },
                            {  _DCLE_INDEX_TYPE    , "������������ ��� �������"                             },
                            {  _DCLE_INDEX_FRG     , "������������� �������������� ����������� �������"     },
                            {  _DCLE_INVALID_CHR   , "������������ ������ � ���������� ���������"           },
                            {  _DCLE_BAD_OPERATION , "������������ ��������"                                },
                            {  _DCLE_BAD_LEFT      , "������������ ����� �������"                           },
                            {  _DCLE_BAD_RIGHT     , "������������ ������ �������"                          },
                            {  _DCLE_EMPTY_LEFT    , "����������� ����� �������"                            },
                            {  _DCLE_EMPTY_RIGHT   , "����������� ������ �������"                           },
                            {  _DCLE_EMPTY_BOTH    , "�������� ��� ���������"                               },
                            {  _DCLE_BINARY_OPER   , "�������� �������� � ����� ���������"                  },
                            {  _DCLE_UNARY_OPER    , "������� �������� � ����� ����������"                  },
                            {  _DCLE_PREFIX_OPER   , "�������� ������ ���� ����������"                      },
                            {  _DCLE_EMPTY_DEST    , "��� �������� ����������� '��������'"                  },
                            {  _DCLE_DIF_TYPE      , "��� '���������' �� �����.���� ����������"             },
                            {  _DCLE_CONST_DEST    , "'��������' �������� ����������������"                 },
                            {  _DCLE_PROTOTYPE     , "��� ��������� �� ������������� ���������"             },
                            {  _DCLE_TYPEDEF_ELEM  , "����������� ������� ������������ �������"             },
                            {  _DCLE_TYPEDEF_METH  , "����������� ����� ������������ �������"               },
                            {  _DCLE_TYPEDEF_SORT  , "���� ���������� ����� ��������� ���"                  },
                            {  _DCLE_TYPEDEF_AREA  , "������������ ������ � ����������� �������"            },
                            {  _DCLE_TBLOCK_CLOSE  , "�� �������� ��������� ����"                           },
                            {  _DCLE_TBLOCK_SIZE   , "������� ������� ��������� ����"                       },

                            {  _DCLE_PRAGMA_UNK    , "����������� ������"                                   },

                            {  _DCLE_BAD_PREF      , "������������ �������"                                 },
                            {  _DCLE_DBL_MOD       , "������������ ��������"                                },
                            {  _DCLE_NO_TYPE       , "�������� �� ������ ������� ���"                       },
                            {  _DCLE_PTR_AREA      , "������ ���������� ��������"                           },
                            {  _DCLE_BLOCK_DECL    , "������ BLOCK ������ ���� ������� ��������"            },
                            {  _DCLE_TYPEDEF_IN    , "��������� �������� typedef"                           },
                            {  _DCLE_TYPEDEF_STR   , "������������ ��������� ��������� typedef"             },
                            {  _DCLE_TYPEDEF_RDF   , "��������������� ����"                                 },
                            {  _DCLE_TYPEDEF_NAME  , "��� ���� �� ������������� �����������"                },
                            {  _DCLE_NAME_INUSE    , "��� ���������� ��� ������������"                      },

                            {  _DCLE_CALL_INSIDE   , "������-����������� ������"                            },

                            {  _DCLE_USER_DEFINED  , "������ ��������� ����������"                          },

                            {  _DCLE_ELIB_INIT     , "������ ����.������� ������� ���������"                },
                            {  _DCLE_ELIB_DECL     , "������������ �������� ������� �� ������� ����������"  },
                            {  _DCLE_ELIB_LOAD     , "��������� �������� ������� �� ������� ����������"     },
                            {  _DCLE_MEMORY        , "���������� ������"                                    },
                            {  _DCLE_MEMORY_VT     , "���������� ������ ��� ���������� ����������"          },
                            {  _DCLE_MEMORY_VA     , "���������� ������ ��� ����������"                     },
                            {  _DCLE_LOAD_LIST     , "������ ����� ������������ ����������"                 },
                            {  _DCLE_STACK_OVR     , "������������ �����"                                   },
                            {  _DCLE_STACK_UND     , "���������� ������� �����"                             },
                            {  _DCLE_STACK_MEM     , "������������ ������ ��� �����"                        },
                            {  _DCLE_UNKNOWN       , "����� ��� � �������"                                  },
                            {  _DCLE_EMPTY         , "������ ������� ���������"                             },
                            {  _DCLE_WORK_FAULT    , "���� ��������� ��������� ������� ������"              },
                            { 0, NULL }
                         };
          int  i ;      

    for(i=0 ; errors[i].text!=NULL ; i++)
      if(errors[i].code==code)  return(errors[i].text) ;

    return("����������� ������") ;
}