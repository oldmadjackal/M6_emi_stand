
/*********************************************************************/
/*                                                                   */
/*                   DATA CONVERTION LANGUAGE                        */
/*                                                                   */
/*                 ���������� ������ � SQL-���������                 */
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
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#ifdef UNIX
#include  <unistd.h>
#define   stricmp  strcasecmp
#define   O_BINARY    0
#else
#include  <io.h>
#include  <direct.h>
#endif

#include "sql_common.h"
#include "sql_direct.h"

#define   DCL_INSIDE
#include "dcl.h"


#pragma warning(disable : 4996)

/*---------------------------------------------------- ������� ����� */

      extern    double  dcl_errno ;               /* ��������� ��������� ������ -> DCL_SLIB.CPP */

               SQL_link *dcl_sql_connect ;        /* ��������� SQL-�������� ��-��������� */ 
       static       int  dcl_sql_inner ;          /* ���� ����������� ��������� ���������� ��-��������� */
       static      char  dcl_sql_error[512] ;     /* ����� ������ ��� ������������� ����������� */
                   char  dcl_sql_output_ignore ;  /* ���� ������������� �������� ���������� SQL */
       static      char  dcl_sql_cp_data[32] ;    /* ������� �������� SQL-������� */
       static      char  dcl_sql_cp_out[32] ;     /* ������� �������� �������� ������ */
       static       int  dcl_sql_cp_convert ;     /* ���� ������������� ���������� �������������� ������� �������� �������� ������ */

/*------------------------------------------------ "�������" ������� */

    Dcl_decl *Dcl_sql_codepages   (Lang_DCL *, Dcl_decl **, int) ;      /* ������� �������������� ������� ������� */
    Dcl_decl *Dcl_sql_connect     (Lang_DCL *, Dcl_decl **, int) ;      /* ���������� �������� CONNECT */
    Dcl_decl *Dcl_sql_disconnect  (Lang_DCL *, Dcl_decl **, int) ;      /* ���������� �������� DISCONNECT */
    Dcl_decl *Dcl_sql_alloc_cursor(Lang_DCL *, Dcl_decl **, int) ;      /* ���������� �������� ALLOC_CURSOR */
    Dcl_decl *Dcl_sql_commit      (Lang_DCL *, Dcl_decl **, int) ;      /* ���������� �������� COMMIT */
    Dcl_decl *Dcl_sql_rollback    (Lang_DCL *, Dcl_decl **, int) ;      /* ���������� �������� ROLLBACK */
    Dcl_decl *Dcl_sql_execute     (Lang_DCL *, Dcl_decl **, int) ;      /* ���������� SQL-��������� */
    Dcl_decl *Dcl_sql_writeLOB    (Lang_DCL *, Dcl_decl **, int) ;      /* ��������� LOB-�������� */
    Dcl_decl *Dcl_sql_fetch2csv   (Lang_DCL *, Dcl_decl **, int) ;      /* ������� � CSV-���� */
    Dcl_decl *Dcl_sql_fetch2dbf   (Lang_DCL *, Dcl_decl **, int) ;      /* ������� � DBF-���� */
    Dcl_decl *Dcl_sql_open        (Lang_DCL *, Dcl_decl **, int) ;      /* �������� ������� */
    Dcl_decl *Dcl_sql_close       (Lang_DCL *, Dcl_decl **, int) ;      /* �������� ������� */
    Dcl_decl *Dcl_sql_error       (Lang_DCL *, Dcl_decl **, int) ;      /* ��������� ������ ������ */

        void  Dcl_sql_final       (void) ;                              /* ��������� ���������� ������������ �������� */

      double  Dcl_sql_fetch       (Lang_DCL *, Dcl_decl *, int, int) ;  /* ���������� SELECT-������� */

/*------------------------------------------------ �������� �������� */

	 static double      E_CALL       = -1 ;        /* �������� ����� ������ */
	 static double      E_TYPE       = -2 ;
	 static double      E_SQL        = -3 ;
	 static double      E_HDC        = -4 ;
	 static double      E_CURSOR_MAX = -5 ;
	 static double      E_HCURSOR    = -6 ;
	 static double      E_UNK_IFACE  = -7 ;
	 static double      E_HDC_IN_USE = -8 ;
	 static double      E_FETCH_FILE = -9 ;
	 static double      E_NO_LOB     =-10 ;
	 static double      E_DBF_FILE   =-11 ;
	 static double      E_UNK_CP     =-12 ;

/*--------------------------------------------------- ������� ������ */

     Dcl_decl  dcl_sql_lib[]={

               {0,         0, 0, 0, "$PassiveData$",         NULL,          "qlib", 0, 0},
               {0, _DCL_CALL, 0, 0, "$Final$",       (void *)Dcl_sql_final,  NULL,  0, 0},

	       {_DGT_VAL, 0, 0, 0, "E_CALL",            &E_CALL,        NULL, 1, 1},
	       {_DGT_VAL, 0, 0, 0, "E_PAR_TYPE",        &E_TYPE,        NULL, 1, 1},
	       {_DGT_VAL, 0, 0, 0, "E_SQL_ERROR",       &E_SQL,         NULL, 1, 1},
	       {_DGT_VAL, 0, 0, 0, "E_CONNECT_HEADER",  &E_HDC,         NULL, 1, 1},
	       {_DGT_VAL, 0, 0, 0, "E_CURSOR_MAX",      &E_CURSOR_MAX,  NULL, 1, 1},
	       {_DGT_VAL, 0, 0, 0, "E_CURSOR_HEADER",   &E_HCURSOR,     NULL, 1, 1},
	       {_DGT_VAL, 0, 0, 0, "E_UNKNOWN_IFACE",   &E_UNK_IFACE,   NULL, 1, 1},
	       {_DGT_VAL, 0, 0, 0, "E_CONNECT_IN_USE",  &E_HDC_IN_USE,  NULL, 1, 1},
	       {_DGT_VAL, 0, 0, 0, "E_FETCH_FILE",      &E_FETCH_FILE,  NULL, 1, 1},
	       {_DGT_VAL, 0, 0, 0, "E_NO_LOB",          &E_NO_LOB,      NULL, 1, 1},
	       {_DGT_VAL, 0, 0, 0, "E_DBF_FILE",        &E_DBF_FILE,    NULL, 1, 1},
	       {_DGT_VAL, 0, 0, 0, "E_UNK_CP",          &E_UNK_CP,      NULL, 1, 1},

	       {_DGT_VAL, _DCL_CALL, 0, 0, "sql_codepages",    (void *)Dcl_sql_codepages,     "",  0, 0},
	       {_DGT_VAL, _DCL_CALL, 0, 0, "sql_connect",      (void *)Dcl_sql_connect,       "",  0, 0},
	       {_DGT_VAL, _DCL_CALL, 0, 0, "sql_disconnect",   (void *)Dcl_sql_disconnect,    "",  0, 0},
//             {_DGT_VAL, _DCL_CALL, 0, 0, "sql_alloc_cursor", (void *)Dcl_sql_alloc_cursor,  "",  0, 0},
	       {_DGT_VAL, _DCL_CALL, 0, 0, "sql_commit",       (void *)Dcl_sql_commit,        "",  0, 0},
	       {_DGT_VAL, _DCL_CALL, 0, 0, "sql_rollback",     (void *)Dcl_sql_rollback,      "",  0, 0},
	       {_DGT_VAL, _DCL_CALL, 0, 0, "sql_execute",      (void *)Dcl_sql_execute,       "",  0, 0},
	       {_DGT_VAL, _DCL_CALL, 0, 0, "sql_writeLOB",     (void *)Dcl_sql_writeLOB,      "",  0, 0},
	       {_DGT_VAL, _DCL_CALL, 0, 0, "sql_fetch2csv",    (void *)Dcl_sql_fetch2csv,     "",  0, 0},
	       {_DGT_VAL, _DCL_CALL, 0, 0, "sql_fetch2dbf",    (void *)Dcl_sql_fetch2dbf,     "",  0, 0},
               {_DGT_VAL, _DCL_CALL, 0, 0, "sql_open",         (void *)Dcl_sql_open,          "",  0, 0},
               {_DGT_VAL, _DCL_CALL, 0, 0, "sql_close",        (void *)Dcl_sql_close,         "",  0, 0},
               {_DGT_VAL, _DCL_CALL, 0, 0, "sql_error",        (void *)Dcl_sql_error,         "",  0, 0},

	       {0, 0, 0, 0, "", NULL, NULL, 0, 0}
                               } ;

/*------------------------------------------------ ��������� ������� */

#define   _CURSORS_INI       2
#define   _CURSORS_MAX    1000

 static struct {
                   SQL_link *connect ;
                 SQL_cursor *cursor ;
               } dcl_sql_cursors[_CURSORS_MAX] ;                    /* ������� ������� �������� */

/*------------------------------------------------ ������� ��������� */

  double  iDcl_sql_parform  (Dcl_decl *, SQL_parameter *) ;      /* ������������ SQL-��������� �� DCL-��������� */
     int  iDcl_sql_getcursor(int, SQL_link **, SQL_cursor **) ;  /* ��������� ������ ���������� � ������� �� ����������� ������� */


/*********************************************************************/
/*                                                                   */
/*            ��������� ���������� ������������ ��������             */

        void  Dcl_sql_final(void)
{

/*--------------------------------- �������� ���������� ��-��������� */

  if(dcl_sql_connect!=NULL &&
     dcl_sql_inner  ==  1    ) {

               dcl_sql_connect->FreeCursors() ;                     /* ������������ �������� */
               dcl_sql_connect->Disconnect() ;                      /* ������������ �� �� */

        delete dcl_sql_connect ;                                    /* ������������ �������� */
               dcl_sql_connect=NULL ;

                               }
/*-------------------------------------------------------------------*/

   return ;
}


/*********************************************************************/
/*                                                                   */
/*            ������� �������������� ������� �������                 */
/*                                                                   */
/*   int  sql_codepages(cp_sql, cp_out) ;                            */
/*                                                                   */
/*     char *cp_sql  -  ������� �������� SQL-�������                 */
/*     char *cp_out  -  ������� �������� ��������� ����������        */
/*                                                                   */
/*    ���������� 0 ��� �������� ���������� ��� ��� ������:           */
/*       �_CALL            -  ������������ ��������� ������          */
/*       �_UNK_CPL         -  ����������� ������� ��������           */

  Dcl_decl *Dcl_sql_codepages(Lang_DCL *Kernel, Dcl_decl **pars, int  pars_cnt)

{
              char  *db_par ;
               int   db_size ;
               int   status ;
               int   type ;
               int   size ;
               int   i ;

   static   double  ret_value ;          /* ����� ��������� �������� */
   static Dcl_decl  dgt_return={_DGT_VAL, 0, 0, 0, "", &ret_value, NULL, 1, 1} ;

/*-------------------------------------------- ���������� ���������� */

  if(pars_cnt<2)  {          dcl_errno=E_CALL ;
                             ret_value=E_CALL ;
                     return(&dgt_return) ;       }

  for(i=0 ; ; i++ ) {

         if(i==0)  {  db_par=dcl_sql_cp_data ;   db_size=sizeof(dcl_sql_cp_data) ;  }
    else if(i==1)  {  db_par=dcl_sql_cp_out ;    db_size=sizeof(dcl_sql_cp_out ) ;  }
    else                  break ;

               memset(db_par, 0, db_size) ;

         if(i>=pars_cnt)  continue ;                                /* ���� ������ ���������� ���������� ����������... */

    if(pars[i]->addr==NULL)  {          dcl_errno=E_CALL ;
                                        ret_value=E_CALL ;
                                return(&dgt_return) ;       }

       type=t_base(pars[i]->type) ;
    if(type!=_DCLT_CHR_AREA &&                                       /* �������� ���� �������� */
       type!=_DCLT_CHR_PTR    ) {        dcl_errno=E_CALL ;
                                         ret_value=E_CALL ;
                                 return(&dgt_return) ;       }

                            size=pars[i]->size ;                    /* ���.����������� ������� ������ */
         if(size>=db_size)  size=db_size-1 ;

          memcpy(db_par, pars[i]->addr, size) ;                     /* ��������� �������� ��������� */

                    }
/*------------------------------------- �������� �������� ���������� */

                dcl_sql_cp_convert=0 ;

    if(stricmp(dcl_sql_cp_data, dcl_sql_cp_out))  {

       status=Kernel->zCodePageConvert("", 0, dcl_sql_cp_data, dcl_sql_cp_out) ;
    if(status) {
                                 dcl_errno=E_UNK_CP ; 
                                 ret_value=E_UNK_CP ;
                         return(&dgt_return) ;       
               }

                dcl_sql_cp_convert=1 ;
                                                   }
/*-------------------------------------------------------------------*/

          ret_value=0 ;
  return(&dgt_return) ;
}


/*********************************************************************/
/*                                                                   */
/*                ���������� �������� CONNECT                        */
/*                                                                   */
/*   int  sql_connect([hdc], db_type, SID, login[, par_1, par_2]) ;  */
/*                                                                   */
/*      int  hdc     -  ���������� ����������                        */
/*     char *db_type -  ��� ������������ ���������                   */
/*     char *SID     -  ������������� ����                           */
/*     char *login   -  ������������/������                          */
/*     char *par_1   -  �������� ���������� 1                        */
/*     char *par_2   -  �������� ���������� 2                        */
/*                                                                   */
/*    ���������� 0 ��� �������� ���������� ��� ��� ������:           */
/*       �_CALL            -  ������������ ��������� ������          */
/*       �_CONNECT_HEADER  -  ������������ ���������� ����������     */
/*       E_SQL_ERROR       -  ������ ���������� SQL-���������        */

  Dcl_decl *Dcl_sql_connect(Lang_DCL *Kernel, Dcl_decl **pars, int  pars_cnt)

{
          SQL_link **connect ;
              char   db_type[32] ;
              char   db_sid[32] ;
              char   db_login[128] ;
              char  *db_pass ;
              char   db_par_1[1024] ;
              char   db_par_2[1024] ;
              char  *db_par ;
               int   db_size ;
               int   hdc ;
               int   status ;
               int   inner_def_iface ;
               int   type ;
               int   size ;
               int   i ;

   static   double  ret_value ;          /* ����� ��������� �������� */
   static Dcl_decl  dgt_return={_DGT_VAL, 0, 0, 0, "", &ret_value, NULL, 1, 1} ;

/*------------------------------------------- ����������� ���������� */

                inner_def_iface=0 ;

  if(pars_cnt     <   1  ||
     pars[0]->addr==NULL   )  {          dcl_errno=E_CALL ;
                                         ret_value=E_CALL ;
                                 return(&dgt_return) ;       }

      type=t_base(pars[0]->type) ;                                  /* ���� ������ �������� ����� �������� ��� - */
   if(type==_DGT_VAL) {                                             /*  - ������� ��� ������������ ����������    */
                         hdc=(int)Kernel->iDgt_get(pars[0]->addr, pars[0]->type) ;
                     connect=  NULL ;
                         pars++ ;                                   /*  �������� ������ �������� */
                         pars_cnt-- ;

                      }
   else               {                                             /* ���������� ��-��������� */
                                 connect=&dcl_sql_connect ;
                         inner_def_iface=  1 ;
                      }

   if(connect==NULL) {
                                dcl_errno=E_HDC ;
                                ret_value=E_HDC ;
                        return(&dgt_return) ;
                     }
/*------------------------- �������� ���������� ��������� ���������� */

    if(*connect!=NULL) {
                                  dcl_errno=E_HDC_IN_USE ;
                                  ret_value=E_HDC_IN_USE ;
                          return(&dgt_return) ;
                       }
/*-------------------------------------------- ���������� ���������� */

  if(pars_cnt<3)  {          dcl_errno=E_CALL ;
                             ret_value=E_CALL ;
                     return(&dgt_return) ;       }

  for(i=0 ; ; i++ ) {

         if(i==0)  {  db_par=db_type ;   db_size=sizeof(db_type ) ;  }
    else if(i==1)  {  db_par=db_sid ;    db_size=sizeof(db_sid  ) ;  }
    else if(i==2)  {  db_par=db_login ;  db_size=sizeof(db_login) ;  }
    else if(i==3)  {  db_par=db_par_1 ;  db_size=sizeof(db_par_1) ;  }
    else if(i==4)  {  db_par=db_par_2 ;  db_size=sizeof(db_par_2) ;  }
    else                  break ;

               memset(db_par, 0, db_size) ;

         if(i>=pars_cnt)  continue ;                                /* ���� ������ ���������� ���������� ����������... */

    if(pars[i]->addr==NULL)  {          dcl_errno=E_CALL ;
                                        ret_value=E_CALL ;
                                return(&dgt_return) ;       }

       type=t_base(pars[i]->type) ;
    if(type!=_DCLT_CHR_AREA &&                                       /* �������� ���� �������� */
       type!=_DCLT_CHR_PTR    ) {        dcl_errno=E_CALL ;
                                         ret_value=E_CALL ;
                                 return(&dgt_return) ;       }

                            size=pars[i]->size ;                    /* ���.����������� ������� ������ */
         if(size>=db_size)  size=db_size-1 ;

          memcpy(db_par, pars[i]->addr, size) ;                     /* ��������� �������� ��������� */

                    }
/*----------------------------------- �������� ���������� �� ���� �� */

       *connect=(*connect)->IfaceByType(db_type) ; 
    if(*connect==NULL) {
                                 dcl_errno=E_UNK_IFACE ; 
                                 ret_value=E_UNK_IFACE ;
                         return(&dgt_return) ;       
                       }

                          dcl_sql_inner=inner_def_iface ;

/*-------------------------------------------------- ���������� � �� */

                         db_pass=strchr(db_login, '/') ;
    if(db_pass==NULL)    db_pass= "" ;
    else             {  *db_pass=0  ;
                         db_pass++ ;   }

       status=(*connect)->Connect(db_login, db_pass, db_sid,
                                           db_par_1, db_par_2 ) ;
    if(status) {
                                 dcl_errno=E_SQL ; 
                                 ret_value=E_SQL ;
                         return(&dgt_return) ;       
               }
/*------------------------------------- ������� ��������� ���������� */

       status=(*connect)->SetAutoCommit(0) ;                        /* ��������� ����-������ */
    if(status) {
                                 dcl_errno=E_SQL ; 
                                 ret_value=E_SQL ;
                         return(&dgt_return) ;       
               }
/*----------------------------------------------- ��������� �������� */

       status=(*connect)->AllocCursors(_CURSORS_INI) ; 
    if(status) {
                                 dcl_errno=E_SQL ; 
                                 ret_value=E_SQL ;
                         return(&dgt_return) ;       
               }
/*-------------------------------------------------------------------*/

          ret_value=0 ;
  return(&dgt_return) ;
}


/*********************************************************************/
/*                                                                   */
/*                ���������� �������� DISCONNECT                     */
/*                                                                   */
/*          int  sql_disconnect([hdc]) ;                             */
/*                                                                   */
/*              int  hdc - ���������� ����������                     */
/*                                                                   */
/*    ���������� 0 ��� �������� ���������� ��� ��� ������:           */
/*       �_CONNECT_HEADER  -  ������������ ���������� ����������     */
/*       E_SQL_ERROR       -  ������ ���������� SQL-���������        */

  Dcl_decl *Dcl_sql_disconnect(Lang_DCL *Kernel, Dcl_decl **pars, int  pars_cnt)

{
          SQL_link **connect ;
               int   hdc ;
               int   status ;  
               int   release_iface ;  

   static   double  ret_value ;          /* ����� ��������� �������� */
   static Dcl_decl  dgt_return={_DGT_VAL, 0, 0, 0, "", &ret_value, NULL, 1, 1} ;

/*------------------------------------------- ����������� ���������� */

               release_iface=0 ;

   if(pars_cnt     >=  1  &&
      pars[0]->addr!=NULL   ) {
                         hdc=(int)Kernel->iDgt_get(pars[0]->addr, pars[0]->type) ;
                     connect=  NULL ;
               release_iface=    1 ;
                              }
   else                       {
                                     connect=&dcl_sql_connect ;
            if(dcl_sql_inner)  release_iface=  1 ;
                              }      

   if(connect==NULL) {
                                dcl_errno=E_HDC ;
                                ret_value=E_HDC ;
                        return(&dgt_return) ;
                     }
/*---------------------------------- �������� ���������� ����������� */

    if(*connect==NULL) {
                                  ret_value=0 ;
                          return(&dgt_return) ;
                       }
/*-------------------------------------------- ������������ �������� */

              (*connect)->FreeCursors() ; 

/*----------------------------------------------- ������������ �� �� */

       status=(*connect)->Disconnect() ;
    if(status)  ret_value=E_SQL ;
    else        ret_value=   0 ;

/*-------------------------------------------- ������������ �������� */

   if(release_iface) {

             strncpy(dcl_sql_error, (*connect)->error_text,         /* ��������� ����� ������ */
                                       sizeof(dcl_sql_error)-1) ;

                           delete (*connect) ;
                                  (*connect)=NULL ;
                     }
/*-------------------------------------------------------------------*/

           dcl_errno=ret_value ;
  
   return(&dgt_return) ;
}


/*********************************************************************/
/*                                                                   */
/*                ���������� �������� COMMIT                         */
/*                                                                   */
/*          int  sql_commit([hdc]) ;                                 */
/*                                                                   */
/*              int  hdc - ���������� ����������                     */
/*                                                                   */
/*    ���������� 0 ��� �������� ���������� ��� ��� ������:           */
/*       �_CONNECT_HEADER  -  ������������ ���������� ����������     */
/*       E_SQL_ERROR       -  ������ ���������� SQL-���������        */

  Dcl_decl *Dcl_sql_commit(Lang_DCL *Kernel, Dcl_decl **pars, int  pars_cnt)

{
          SQL_link *connect ;
               int  hdc ;
               int  status ;  

   static   double  ret_value ;          /* ����� ��������� �������� */
   static Dcl_decl  dgt_return={_DGT_VAL, 0, 0, 0, "", &ret_value, NULL, 1, 1} ;

/*------------------------------------------- ����������� ���������� */

   if(pars_cnt     >=  1  &&
      pars[0]->addr!=NULL   ) {
               hdc=(int)Kernel->iDgt_get(pars[0]->addr, pars[0]->type) ;
           connect=  NULL ;
                              }
   else                       {
                                   connect=dcl_sql_connect ;
                              }      

   if(connect==NULL) {
                                dcl_errno=E_HDC ;
                                ret_value=E_HDC ;
                        return(&dgt_return) ;
                     }
/*---------------------------------------------- ���������� �������� */

       status=connect->Commit() ;
    if(status)  ret_value=E_SQL ;
    else        ret_value=   0 ;

/*-------------------------------------------------------------------*/

       dcl_errno=ret_value ;

   return(&dgt_return) ;
}


/*********************************************************************/
/*                                                                   */
/*                ���������� �������� ROLLBACK                       */
/*                                                                   */
/*          int  sql_rollback([hdc]) ;                               */
/*                                                                   */
/*              int  hdc - ���������� ����������                     */
/*                                                                   */
/*    ���������� 0 ��� �������� ���������� ��� ��� ������:           */
/*     �_CONNECT_HEADER  -  ������������ ���������� ����������       */
/*     E_SQL_ERROR       -  ������ ���������� SQL-���������          */

  Dcl_decl *Dcl_sql_rollback(Lang_DCL *Kernel, Dcl_decl **pars, int  pars_cnt)

{
          SQL_link *connect ;
               int  hdc ;
               int  status ;  

   static   double  ret_value ;          /* ����� ��������� �������� */
   static Dcl_decl  dgt_return={_DGT_VAL, 0, 0, 0, "", &ret_value, NULL, 1, 1} ;

/*------------------------------------------- ����������� ���������� */

   if(pars_cnt     >=  1  &&
      pars[0]->addr!=NULL   ) {
               hdc=(int)Kernel->iDgt_get(pars[0]->addr, pars[0]->type) ;
           connect=  NULL ;
                              }
   else                       {
                                   connect=dcl_sql_connect ;
                              }      

   if(connect==NULL) {
                                 dcl_errno=E_HDC ;
                                 ret_value=E_HDC ;
                         return(&dgt_return) ;
                     }
/*---------------------------------------------- ���������� �������� */

       status=connect->Rollback() ;
    if(status)  ret_value=E_SQL ;
    else        ret_value=   0 ;

/*-------------------------------------------------------------------*/

           dcl_errno=ret_value ;

   return(&dgt_return) ;
}


/*********************************************************************/
/*                                                                   */
/*       ���������� ��������������� SQL-��������� ��� SQL-�����      */
/*                                                                   */
/*        int  sql_execute([hdc], sql[, value1, ..., valueN]) ;      */
/*                                                                   */
/*            int  hdc    - ���������� ����������                    */
/*           char *sql    - SQL-��������                             */
/*              ?  value? - ������������� ����������                 */
/*                                                                   */
/*  ���������� 0 ��� �������� ���������� ��� ��� ������:             */
/*     �_CALL            -  ������������ ��������� ������            */
/*     �_PAR_TYPE        -  ���������������� ��� ����������          */
/*     �_CONNECT_HEADER  -  ������������ ���������� ����������       */
/*     �_CURSOR_MAX      -  ��� ��������� ��������                   */
/*     E_SQL_ERROR       -  ������ ���������� SQL-���������          */

  Dcl_decl *Dcl_sql_execute(Lang_DCL *Kernel, Dcl_decl **pars, int  pars_cnt)

{
        SQL_cursor *cursor ;
          SQL_link *connect ;
               int  hdc ;
              char *sql ;                /* SQL-��������� */
     SQL_parameter  sql_pars[200] ;      /* ������ SQL-���������� */
               int  sql_pars_cnt ;
          Dcl_decl *par ;
               int  type ;
               int  status ;  
               int  size ;
               int  i ;

   static   double  ret_value ;          /* ����� ��������� �������� */
   static Dcl_decl  dgt_return={_DGT_VAL, 0, 0, 0, "", &ret_value, NULL, 1, 1} ;

/*------------------------------------------- ����������� ���������� */

  if(pars_cnt     <   1  ||
     pars[0]->addr==NULL   )  {          dcl_errno=E_CALL ;
                                         ret_value=E_CALL ;
                                 return(&dgt_return) ;       }

      type=t_base(pars[0]->type) ;                                  /* ���� ������ �������� ����� �������� ��� - */
   if(type==_DGT_VAL) {                                             /*  - ������� ��� ������������ ����������    */
                         hdc=(int)Kernel->iDgt_get(pars[0]->addr, pars[0]->type) ;
                     connect=  NULL ;

                         pars++ ;                                   /*  �������� ������ �������� */
                         pars_cnt-- ;

                      }
   else               {                                             /* ���������� ��-��������� */
                         connect=dcl_sql_connect ;
                      }      

   if(connect==NULL) {
                                dcl_errno=E_HDC ;
                                ret_value=E_HDC ;
                        return(&dgt_return) ;
                     }

              connect->error_text[0]=0 ;                            /* ����� ������ */

/*----------------------------------------- ���������� SQL-��������� */

  if(pars_cnt     <   1  ||
     pars[0]->addr==NULL   )  {          dcl_errno=E_CALL ;
                                         ret_value=E_CALL ;
                                 return(&dgt_return) ;       }

      type=t_base(pars[0]->type) ;
   if(type!=_DCLT_CHR_AREA &&                                       /* �������� ���� �������� */
      type!=_DCLT_CHR_PTR    ) {         dcl_errno=E_CALL ;
                                         ret_value=E_CALL ;
                                 return(&dgt_return) ;       }


                  sql=(char *)calloc(1, pars[0]->size+8) ;          /* �������� ������ �� pars[0]->size */
          memcpy( sql, pars[0]->addr, pars[0]->size) ;              /* ��������� SQL-�������� */

/*-------------------------------- ���������� ���������� ����������� */

   for(sql_pars_cnt=0, i=1 ; i<pars_cnt ; i++) {

        type= t_base(pars[i]->type) ;                               /* ��������� ������� ��� */
/*- - - - - - - - - - - - - - - - - - - - - ������ ������������ ���� */
     if(type==_XTP_OBJ) {
                        }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - ������� ������ */
     else               {

              ret_value=iDcl_sql_parform(pars[i], &sql_pars[sql_pars_cnt])  ;
           if(ret_value<0) {
                                     dcl_errno=ret_value ;
                             return(&dgt_return) ;
                           }

                                       sql_pars_cnt++ ;
                        }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                                               } 

/*---------------------------------------------- ���������� �������� */

       cursor=connect->LockCursor("Dcl_sql_execute") ;              /* ����������� ������ */
    if(cursor==NULL) {          dcl_errno=E_CURSOR_MAX ;
                                ret_value=E_CURSOR_MAX ;
                        return(&dgt_return) ;             }

       status=connect->SqlExecute(cursor, sql,                      /* ��������� SQL-�������� */
                                          sql_pars, sql_pars_cnt) ;
    if(status) {                                                    /* ���� ������ - ��������� �� ��������� ���������� */
                         ret_value=E_SQL ;
                 strncpy(connect->error_text, 
                          cursor->error_text, sizeof(connect->error_text)-1) ;  
               }
    else                 ret_value=  0. ;

              connect->UnlockCursor(cursor) ;                       /* ����������� ������ */

/*----------------------------------- ���������� �������� ���������� */

  if(!dcl_sql_output_ignore)                                        /* ���� �� ���������� �������� ���������... */
   for(i=0 ; i<sql_pars_cnt ; i++) {

        par=(Dcl_decl *)sql_pars[i].ref ;
     if(par==NULL)  continue ;

        type= t_base(par->type) ;                                   /* ��������� ������� ��� */
/*- - - - - - - - - - - - - - - - - - - - - ������ ������������ ���� */
     if(type==_XTP_OBJ) {
                        }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - ������� ������ */
     else               {
   
                            size=strlen(sql_pars[i].buff) ;
        if(par->buff<size)  size=par->buff ;

                        memset(par->addr, 0, par->buff) ;
                        memcpy(par->addr, sql_pars[i].buff, size) ;
                               par->size=size ;
         Kernel->iSize_correct(par) ;                               /* �������� �������� ��������� �������� ���������� �� ������ */

                        }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                                   }
/*-------------------------------------------- ������������ �������� */

                                        free(sql) ;

   for(i=0 ; i<sql_pars_cnt ; i++) {
                                        free(sql_pars[i].name) ;
                                        free(sql_pars[i].buff) ;
                                   } 
/*-------------------------------------------------------------------*/

           dcl_errno=ret_value ;

   return(&dgt_return) ;
}


/*********************************************************************/
/*                                                                   */
/*                   ��������� LOB-��������                          */
/*                                                                   */
/*        int  sql_writeLOB([hdc], sql[, value1, ..., valueN]) ;     */
/*                                                                   */
/*            int  hdc    - ���������� ����������                    */
/*           char *sql    - SQL-��������                             */
/*              ?  value? - ������������� ����������                 */
/*                                                                   */
/*  ���������� 0 ��� �������� ���������� ��� ��� ������:             */
/*     �_CALL            -  ������������ ��������� ������            */
/*     �_PAR_TYPE        -  ���������������� ��� ����������          */
/*     �_CONNECT_HEADER  -  ������������ ���������� ����������       */
/*     �_CURSOR_MAX      -  ��� ��������� ��������                   */
/*     E_SQL_ERROR       -  ������ ���������� SQL-���������          */
/*     E_NO_LOB          -  ����� ���������� ��� LOB-��������        */

  Dcl_decl *Dcl_sql_writeLOB(Lang_DCL *Kernel, Dcl_decl **pars, int  pars_cnt)

{
        SQL_cursor *cursor ;
          SQL_link *connect ;
               int  hdc ;
              char *sql ;                /* SQL-��������� */
     SQL_parameter  sql_pars[200] ;      /* ������ SQL-���������� */
               int  sql_pars_cnt ;
               int  type ;
               int  blobs ;
               int  status ;  
               int  i ;

   static   double  ret_value ;          /* ����� ��������� �������� */
   static Dcl_decl  dgt_return={_DGT_VAL, 0, 0, 0, "", &ret_value, NULL, 1, 1} ;

/*------------------------------------------- ����������� ���������� */

  if(pars_cnt     <   1  ||
     pars[0]->addr==NULL   )  {          dcl_errno=E_CALL ;
                                         ret_value=E_CALL ;
                                 return(&dgt_return) ;       }

      type=t_base(pars[0]->type) ;                                  /* ���� ������ �������� ����� �������� ��� - */
   if(type==_DGT_VAL) {                                             /*  - ������� ��� ������������ ����������    */
                         hdc=(int)Kernel->iDgt_get(pars[0]->addr, pars[0]->type) ;
                     connect=  NULL ;

                         pars++ ;                                   /*  �������� ������ �������� */
                         pars_cnt-- ;

                      }
   else               {                                             /* ���������� ��-��������� */
                         connect=dcl_sql_connect ;
                      }      

   if(connect==NULL) {
                                dcl_errno=E_HDC ;
                                ret_value=E_HDC ;
                        return(&dgt_return) ;
                     }

              connect->error_text[0]=0 ;                            /* ����� ������ */

/*----------------------------------------- ���������� SQL-��������� */

  if(pars_cnt     <   1  ||
     pars[0]->addr==NULL   )  {          dcl_errno=E_CALL ;
                                         ret_value=E_CALL ;
                                 return(&dgt_return) ;       }

      type=t_base(pars[0]->type) ;
   if(type!=_DCLT_CHR_AREA &&                                       /* �������� ���� �������� */
      type!=_DCLT_CHR_PTR    ) {         dcl_errno=E_CALL ;
                                         ret_value=E_CALL ;
                                 return(&dgt_return) ;       }


                  sql=(char *)calloc(1, pars[0]->size+8) ;          /* �������� ������ �� pars[0]->size */
          memcpy( sql, pars[0]->addr, pars[0]->size) ;              /* ��������� SQL-�������� */

/*-------------------------------- ���������� ���������� ����������� */

   for(sql_pars_cnt=0, blobs=0, i=1 ; i<pars_cnt ; i++) {

        type= t_base(pars[i]->type) ;                               /* ��������� ������� ��� */
/*- - - - - - - - - - - - - - - - - - - - - ������ ������������ ���� */
     if(type==_XTP_OBJ) {
                        }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - ������� ������ */
     else               {

              ret_value=iDcl_sql_parform(pars[i], &sql_pars[sql_pars_cnt])  ;
           if(ret_value<0) {
                                     dcl_errno=ret_value ;
                             return(&dgt_return) ;
                           }

                       blobs+=sql_pars[sql_pars_cnt].blob ;  
                                       sql_pars_cnt++ ;
                        }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                                                        } 

    if(blobs==0) {         dcl_errno=E_NO_LOB ;
                           ret_value=E_NO_LOB ;
                   return(&dgt_return) ;         }

/*---------------------------------------------- ���������� �������� */

       cursor=connect->LockCursor("Dcl_sql_writeLOB") ;             /* ����������� ������ */
    if(cursor==NULL) {          dcl_errno=E_CURSOR_MAX ;
                                ret_value=E_CURSOR_MAX ;
                        return(&dgt_return) ;             }

       status=connect->WriteLOB(cursor, sql,                        /* ��������� SQL-�������� */
                                        sql_pars, sql_pars_cnt) ;
    if(status) {                                                    /* ���� ������ - ��������� �� ��������� ���������� */
                         ret_value=E_SQL ;
                 strncpy(connect->error_text, 
                          cursor->error_text, sizeof(connect->error_text)-1) ;  
               }
    else                 ret_value=  0. ;

              connect->UnlockCursor(cursor) ;                       /* ����������� ������ */

/*-------------------------------------------- ������������ �������� */

                                        free(sql) ;

   for(i=0 ; i<sql_pars_cnt ; i++) {
                                        free(sql_pars[i].name) ;
                                        free(sql_pars[i].buff) ;
                                   }
/*-------------------------------------------------------------------*/

           dcl_errno=ret_value ;

   return(&dgt_return) ;
}


/*********************************************************************/
/*                                                                   */
/*       ������� �� SELECT-������� � CSV-����                        */
/*                                                                   */
/*      int  sql_fetch2csv([hdc], path, sep,                         */
/*                                 sql[, value1, ..., valueN]) ;     */
/*                                                                   */
/*         int  hdc    - ���������� ����������                       */
/*        char *path   - ���� � ����� ����������                     */
/*        char *sep    - ����������� �����                           */
/*        char *sql    - SQL-��������                                */
/*           ?  value? - ������������� ����������                    */
/*                                                                   */
/*  ���������� ����� ��������� ����� ������ ��� -1 ��� ������        */
/*   ��� ��� ������:                                                 */
/*     �_CALL            -  ������������ ��������� ������            */
/*     �_PAR_TYPE        -  ���������������� ��� ����������          */
/*     �_CONNECT_HEADER  -  ������������ ���������� ����������       */
/*     �_CURSOR_MAX      -  ��� ��������� ��������                   */
/*     E_SQL_ERROR       -  ������ ���������� SQL-���������          */
/*     E_FETCH_FILE      -  ������ ��� ������ � ������               */

  Dcl_decl *Dcl_sql_fetch2csv(Lang_DCL *Kernel, Dcl_decl **pars, int  pars_cnt)

{
        SQL_cursor *cursor ;
          SQL_link *connect ;
               int  hdc ;
              char  path[FILENAME_MAX] ;  /* ���� � ����� ���������� */ 
              char  sep[32] ;             /* ����������� */ 
              char *sql ;                 /* SQL-��������� */
     SQL_parameter  sql_pars[200] ;       /* ������ SQL-���������� */
               int  sql_pars_cnt ;
          Dcl_decl *par ;
               int  type ;
              FILE *file ;
               int  status ;
              char  row_cut[16] ;         /* ���������� ������ */
            size_t  cnt ;
               int  size ;
               int  n ;
               int  i ;
               int  j ;

   static   double  ret_value ;          /* ����� ��������� �������� */
   static Dcl_decl  dgt_return={_DGT_VAL, 0, 0, 0, "", &ret_value, NULL, 1, 1} ;

/*---------------------------------------------------- ������������� */

                       ret_value=0 ;
                               n=0 ;

/*------------------------------------------- ����������� ���������� */

  if(pars_cnt     <   1  ||
     pars[0]->addr==NULL   )  {          dcl_errno=E_CALL ; 
                                         ret_value=E_CALL ;
                                 return(&dgt_return) ;       }

      type=t_base(pars[0]->type) ;                                  /* ���� ������ �������� ����� �������� ��� - */
   if(type==_DGT_VAL) {                                             /*  - ������� ��� ������������ ����������    */
                         hdc=(int)Kernel->iDgt_get(pars[0]->addr, pars[0]->type) ;
                     connect=  NULL ;

                         pars++ ;                                   /*  �������� ������ �������� */
                         pars_cnt-- ;

                      }
   else               {                                             /* ���������� ��-��������� */
                         connect=dcl_sql_connect ;
                      }      

   if(connect==NULL) {
                                dcl_errno=E_HDC ;
                                ret_value=E_HDC ;
                        return(&dgt_return) ;
                     }

              connect->error_text[0]=0 ;                            /* ����� ������ */

/*------------------------------- ���������� ���� � ����� ���������� */

  if(pars_cnt     <   1  ||
     pars[0]->addr==NULL   )  {          dcl_errno=E_CALL ;
                                         ret_value=E_CALL ;
                                 return(&dgt_return) ;       }

      type=t_base(pars[0]->type) ;
   if(type!=_DCLT_CHR_AREA &&                                       /* �������� ���� �������� */
      type!=_DCLT_CHR_PTR    ) {         dcl_errno=E_CALL ;
                                         ret_value=E_CALL ;
                                 return(&dgt_return) ;       }

    if(pars[0]->size>=sizeof(path))  pars[0]->size=sizeof(path)-1 ; /* �������� �������� */

	  memset(path, 0, sizeof(path)) ;
	  memcpy(path, pars[0]->addr, pars[0]->size) ;              /* ��������� ���� � ������� ����� */

                         pars++ ;                                   /* �������� �������� */
                         pars_cnt-- ;

/*------------------------------------- ���������� ����������� ����� */

  if(pars_cnt     <   1  ||
     pars[0]->addr==NULL   )  {          dcl_errno=E_CALL ;
                                         ret_value=E_CALL ;
                                 return(&dgt_return) ;       }

      type=t_base(pars[0]->type) ;
   if(type!=_DCLT_CHR_AREA &&                                       /* �������� ���� �������� */
      type!=_DCLT_CHR_PTR    ) {         dcl_errno=E_CALL ;
                                         ret_value=E_CALL ;
                                 return(&dgt_return) ;       }

    if(pars[0]->size>=sizeof(sep))  pars[0]->size=sizeof(sep)-1 ;   /* �������� �������� */

	  memset(sep, 0, sizeof(sep)) ;
	  memcpy(sep, pars[0]->addr, pars[0]->size) ;               /* ��������� ����������� � ������� ����� */

                         pars++ ;                                   /* �������� �������� */
                         pars_cnt-- ;
/*- - - - - - - - - - - - - - - - - - - - �������� ����������� ����� */
                        strcpy(row_cut, "\r\n") ;

        if(!memicmp(sep, "UNIX:", 5)) {                             /* ���� ����������� ����� ��� UNIX */
                                          strcpy(row_cut, "\n") ;
                                          strcpy(sep, sep+5) ;
                                      }
/*----------------------------------------- ���������� SQL-��������� */

  if(pars_cnt     <   1  ||
     pars[0]->addr==NULL   )  {          dcl_errno=E_CALL ;
                                         ret_value=E_CALL ;
                                 return(&dgt_return) ;       }

      type=t_base(pars[0]->type) ;
   if(type!=_DCLT_CHR_AREA &&                                       /* �������� ���� �������� */
      type!=_DCLT_CHR_PTR    ) {         dcl_errno=E_CALL ;
                                         ret_value=E_CALL ;
                                 return(&dgt_return) ;       }

                  sql=(char *)calloc(1, pars[0]->size+8) ;          /* �������� ������ �� pars[0]->size */
          memcpy( sql, pars[0]->addr, pars[0]->size) ;              /* ��������� SQL-�������� */

/*-------------------------------- ���������� ���������� ����������� */

   for(sql_pars_cnt=0, i=1 ; i<pars_cnt ; i++) {

        type= t_base(pars[i]->type) ;                               /* ��������� ������� ��� */
/*- - - - - - - - - - - - - - - - - - - - - ������ ������������ ���� */
     if(type==_XTP_OBJ) {
                        }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - ������� ������ */
     else               {

              ret_value=iDcl_sql_parform(pars[i], &sql_pars[sql_pars_cnt])  ;
           if(ret_value<0) {
                                     dcl_errno=ret_value ;
                             return(&dgt_return) ;
                           }

                                       sql_pars_cnt++ ;
                        }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                                               } 
/*---------------------------------------------- ���������� �������� */

#define  NAME   (char *)cursor->columns[j].name
#define  VALUE  (char *)cursor->columns[j].value

   do {                                                             /* BLOCK.1 */

       cursor=connect->LockCursor("Dcl_sql_fetch2csv") ;            /* ����������� ������ */
    if(cursor==NULL) {  ret_value=E_CURSOR_MAX ;
                               break ;           }

       status=connect->SelectOpen(cursor, sql,                      /* ��������� ������� */
                                          sql_pars, sql_pars_cnt) ;
    if(status) {  ret_value=E_SQL ;
                        break ;      }

       file=fopen(path, "wb") ;                                     /* ��������� ���� ���������� */
    if(file==NULL) {  ret_value=E_FETCH_FILE ;  
                              break ;           }

    for(n=0 ; ; n++) {                                              /* LOOP - ��������� ������� */ 

           status=connect->SelectFetch(cursor) ;                    /* ��������� ��������� ������ */
        if(status==_SQL_NO_DATA)   break ;                          /* ���� ��� ������ ���������... */
        if(status) {                                                /* ���� ������ ������� */
                     ret_value=E_SQL ;
                      break ; 
                   }

      for(j=0 ; j<cursor->columns_cnt ; j++) {                      /* ����� � ���� ������ ����� */
        if(         j   >0 &&
           strlen(sep)  >0   ) {
                         cnt=fwrite(sep, 1, strlen(sep), file) ;
                      if(cnt!=strlen(sep)) {  ret_value=E_FETCH_FILE ;
                                                     break ;            }
                               }

        if(strlen(VALUE)>0   ) {  
                         cnt=fwrite(VALUE, 1, strlen(VALUE), file) ;
                      if(cnt!=strlen(VALUE)) {  ret_value=E_FETCH_FILE ;
                                                     break ;              }
                               }
                                             }

                         cnt=fwrite(row_cut, 1, strlen(row_cut), file) ;
                      if(cnt!=strlen(row_cut)) {  ret_value=E_FETCH_FILE ;
                                                      break ;              }

                                 if(ret_value)  break ;
                     }                                              /* ENDLOOP - ��������� ������� */ 

       status=fclose(file) ;                                        /* ��������� ���� ���������� */
    if(status) {  ret_value=E_FETCH_FILE ;  
                          break ;           }

      } while(0) ;                                                  /* BLOCK.1 */

#undef  NAME
#undef  VALUE

    if(ret_value==E_SQL)                                            /* ���� ������ - ��������� �� ��������� ���������� */
             strncpy(connect->error_text, 
                      cursor->error_text, sizeof(connect->error_text)-1) ;  

/*----------------------------------- ���������� �������� ���������� */

   for(i=0 ; i<sql_pars_cnt ; i++) {

        par=(Dcl_decl *)sql_pars[i].ref ;
     if(par==NULL)  continue ;

        type= t_base(par->type) ;                                   /* ��������� ������� ��� */
/*- - - - - - - - - - - - - - - - - - - - - ������ ������������ ���� */
     if(type==_XTP_OBJ) {
                        }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - ������� ������ */
     else               {
   
                            size=strlen(sql_pars[i].buff) ;
        if(par->buff<size)  size=par->buff ;

                        memset(par->addr, 0, par->buff) ;
                        memcpy(par->addr, sql_pars[i].buff, size) ;
                               par->size=size ;
         Kernel->iSize_correct(par) ;                               /* �������� �������� ��������� �������� ���������� �� ������ */

                        }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                                   }
/*-------------------------------------------- ������������ �������� */

   if(cursor!=NULL)  connect->UnlockCursor(cursor) ;

                                        free(sql) ;

   for(i=0 ; i<sql_pars_cnt ; i++) {
                                        free(sql_pars[i].name) ;
                                        free(sql_pars[i].buff) ;
                                   } 
/*-------------------------------------------------------------------*/

                       dcl_errno=ret_value ;

     if(ret_value==0)  ret_value=n ;

   return(&dgt_return) ;
}


/*********************************************************************/
/*                                                                   */
/*       ������� �� SELECT-������� � DBF-����                        */
/*                                                                   */
/*      int  sql_fetch2dbf([hdc], path, dbf_tmpl,                    */
/*                                 sql[, value1, ..., valueN]) ;     */
/*                                                                   */
/*         int  hdc      - ���������� ����������                     */
/*        char *path     - ���� � ����� ����������                   */
/*        char *dbf_tmpl - ���� � ����� DBF-�������                  */
/*        char *sql      - SQL-��������                              */
/*           ?  value?   - ������������� ����������                  */
/*                                                                   */
/*  ���������� ����� ��������� ����� ������ ��� -1 ��� ������        */
/*   ��� ��� ������:                                                 */
/*     �_CALL            -  ������������ ��������� ������            */
/*     �_PAR_TYPE        -  ���������������� ��� ����������          */
/*     �_CONNECT_HEADER  -  ������������ ���������� ����������       */
/*     �_CURSOR_MAX      -  ��� ��������� ��������                   */
/*     E_SQL_ERROR       -  ������ ���������� SQL-���������          */
/*     E_DBF_FILE        -  ������ �������� DBF-���������            */
/*     E_FETCH_FILE      -  ������ ��� ������ � ������ ����������    */

  Dcl_decl *Dcl_sql_fetch2dbf(Lang_DCL *Kernel, Dcl_decl **pars, int  pars_cnt)

{
    SQL_direct_DBF  Table ;
        SQL_cursor *cursor ;
          SQL_link *connect ;
               int  hdc ;
              char  path[FILENAME_MAX] ;  /* ���� � ����� ���������� */ 
              char  tmpl[FILENAME_MAX] ;  /* ���� � ����� dbf-��������� */ 
              char *sql ;                 /* SQL-��������� */
     SQL_parameter  sql_pars[200] ;       /* ������ SQL-���������� */
               int  sql_pars_cnt ;
               int  map[1024] ;           /* ������� �������� ������� SQL->DBF */
              char  dummy[1024] ;         /* �������� �����x ����� */
          Dcl_decl *par ;
               int  type ;
               int  status ;
               int  size ;
              char *value ; 
              char *end ; 
              char  tmp[1024] ;
               int  n ;
               int  i ;
               int  j ;

   static   double  ret_value ;          /* ����� ��������� �������� */
   static Dcl_decl  dgt_return={_DGT_VAL, 0, 0, 0, "", &ret_value, NULL, 1, 1} ;

/*---------------------------------------------------- ������������� */

           memset(dummy, 0, sizeof(dummy)) ;

                       ret_value=0 ;
                               n=0 ;

/*------------------------------------------- ����������� ���������� */

  if(pars_cnt     <   1  ||
     pars[0]->addr==NULL   )  {          dcl_errno=E_CALL ; 
                                         ret_value=E_CALL ;
                                 return(&dgt_return) ;       }

      type=t_base(pars[0]->type) ;                                  /* ���� ������ �������� ����� �������� ��� - */
   if(type==_DGT_VAL) {                                             /*  - ������� ��� ������������ ����������    */
                         hdc=(int)Kernel->iDgt_get(pars[0]->addr, pars[0]->type) ;
                     connect=  NULL ;

                         pars++ ;                                   /*  �������� ������ �������� */
                         pars_cnt-- ;

                      }
   else               {                                             /* ���������� ��-��������� */
                         connect=dcl_sql_connect ;
                      }      

   if(connect==NULL) {
                                dcl_errno=E_HDC ;
                                ret_value=E_HDC ;
                        return(&dgt_return) ;
                     }

              connect->error_text[0]=0 ;                            /* ����� ������ */

/*------------------------------- ���������� ���� � ����� ���������� */

  if(pars_cnt     <   1  ||
     pars[0]->addr==NULL   )  {          dcl_errno=E_CALL ;
                                         ret_value=E_CALL ;
                                 return(&dgt_return) ;       }

      type=t_base(pars[0]->type) ;
   if(type!=_DCLT_CHR_AREA &&                                       /* �������� ���� �������� */
      type!=_DCLT_CHR_PTR    ) {         dcl_errno=E_CALL ;
                                         ret_value=E_CALL ;
                                 return(&dgt_return) ;       }

    if(pars[0]->size>=sizeof(path))  pars[0]->size=sizeof(path)-1 ; /* �������� �������� */

	  memset(path, 0, sizeof(path)) ;
	  memcpy(path, pars[0]->addr, pars[0]->size) ;              /* ��������� ���� � ������� ����� */

                         pars++ ;                                   /* �������� �������� */
                         pars_cnt-- ;

/*------------------------------ ���������� ���� � ����� DBF-������� */

  if(pars_cnt     <   1  ||
     pars[0]->addr==NULL   )  {          dcl_errno=E_CALL ;
                                         ret_value=E_CALL ;
                                 return(&dgt_return) ;       }

      type=t_base(pars[0]->type) ;
   if(type!=_DCLT_CHR_AREA &&                                       /* �������� ���� �������� */
      type!=_DCLT_CHR_PTR    ) {         dcl_errno=E_CALL ;
                                         ret_value=E_CALL ;
                                 return(&dgt_return) ;       }

    if(pars[0]->size>=sizeof(tmpl))  pars[0]->size=sizeof(tmpl)-1 ; /* �������� �������� */

	  memset(tmpl, 0, sizeof(tmpl)) ;
	  memcpy(tmpl, pars[0]->addr, pars[0]->size) ;              /* ��������� ���� � ������� ����� */

                         pars++ ;                                   /* �������� �������� */
                         pars_cnt-- ;

/*----------------------------------------- ���������� SQL-��������� */

  if(pars_cnt     <   1  ||
     pars[0]->addr==NULL   )  {          dcl_errno=E_CALL ;
                                         ret_value=E_CALL ;
                                 return(&dgt_return) ;       }

      type=t_base(pars[0]->type) ;
   if(type!=_DCLT_CHR_AREA &&                                       /* �������� ���� �������� */
      type!=_DCLT_CHR_PTR    ) {         dcl_errno=E_CALL ;
                                         ret_value=E_CALL ;
                                 return(&dgt_return) ;       }

                  sql=(char *)calloc(1, pars[0]->size+8) ;          /* �������� ������ �� pars[0]->size */
          memcpy( sql, pars[0]->addr, pars[0]->size) ;              /* ��������� SQL-�������� */

/*-------------------------------- ���������� ���������� ����������� */

   for(sql_pars_cnt=0, i=1 ; i<pars_cnt ; i++) {

        type= t_base(pars[i]->type) ;                               /* ��������� ������� ��� */
/*- - - - - - - - - - - - - - - - - - - - - ������ ������������ ���� */
     if(type==_XTP_OBJ) {
                        }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - ������� ������ */
     else               {

              ret_value=iDcl_sql_parform(pars[i], &sql_pars[sql_pars_cnt])  ;
           if(ret_value<0) {
                                     dcl_errno=ret_value ;
                             return(&dgt_return) ;
                           }

                                       sql_pars_cnt++ ;
                        }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                                               }
/*-------------------------------------------- ������� DBF-��������� */

        status=Table.Open(tmpl, NULL) ;                             /* �������� ����� DBF-������� */
     if(status) {
                  strncpy(connect->error_text, Table.error_text, sizeof(connect->error_text)-1) ;  
                            dcl_errno=E_DBF_FILE ;
                            ret_value=E_DBF_FILE ;
                    return(&dgt_return) ;
                }

        status=Table.Clone(path) ;                                  /* ������ ��������� � ���� ���������� */
     if(status) {
                  strncpy(connect->error_text, Table.error_text, sizeof(connect->error_text)-1) ;  
                            dcl_errno=E_DBF_FILE ;
                            ret_value=E_DBF_FILE ;
                    return(&dgt_return) ;
                }

               Table.iFree() ;

/*----------------------------------------------- ���������� ������� */

#define  NAME   (char *)cursor->columns[j].name
#define  VALUE  (char *)cursor->columns[j].value

   do {                                                             /* BLOCK.1 */
/*- - - - - - - - - - - - - - - - - - - - - - - - - �������� ������� */
       cursor=connect->LockCursor("Dcl_sql_fetch2csv") ;            /* ����������� ������ */
    if(cursor==NULL) {  ret_value=E_CURSOR_MAX ;
                               break ;           }

       status=connect->SelectOpen(cursor, sql,                      /* ��������� ������� */
                                          sql_pars, sql_pars_cnt) ;
    if(status) {  ret_value=E_SQL ;
                        break ;      }

        status=Table.Open(path, NULL) ;                             /* �������� ����� ���������� */
     if(status) {
                            dcl_errno=E_DBF_FILE ;
                            ret_value=E_DBF_FILE ;
                    return(&dgt_return) ;
                }
/*- - - - - - - - - - - - - - - - - - - - - - - - ���������� ������� */
    for(n=0 ; ; n++) {                                              /* LOOP - ��������� ������� */ 

           status=connect->SelectFetch(cursor) ;                    /* ��������� ��������� ������ */
        if(status==_SQL_NO_DATA)   break ;                          /* ���� ��� ������ ���������... */
        if(status) {                                                /* ���� ������ ������� */
                     ret_value=E_SQL ;
                      break ; 
                   }
/*- - - - - - - - - - - - - -  �������� ����� ������� � DBF-�������� */
     if(n==0) {

      for(j=0 ; j<cursor->columns_cnt ; j++) {

        for(i=0 ; i<Table.fields_cnt ; i++)
          if(!strcmp(NAME, Table.fields[i].name )) {  map[j]=i ;  break ;  }

          if(i>=Table.fields_cnt) {                                 /* ���� ���� �� ������� � DBF*/
                  sprintf(connect->error_text, "Field %s miseed in DBF file", cursor->columns[j].name) ;  
                            ret_value=E_DBF_FILE ;
                                        break ;
                                  }
                                             }
              }
/*- - - - - - - - - - - - - - - - - - - - ���������� ������ �� ����� */
      for(i=0 ; i<Table.fields_cnt    ; i++)  Table.fields[    i ].value=dummy ;

      for(j=0 ; j<cursor->columns_cnt ; j++)  Table.fields[map[j]].value=VALUE ;

/*- - - - - - - - - - - - - - - - - - - - -  �������� �������� ����� */
      for(i=0 ; i<Table.fields_cnt ; i++) {

         if(Table.fields[i].value[0]==0)  continue ;

         if(Table.fields[i].format[0]=='C') {
                                               continue ;
                                            }
         else
         if(Table.fields[i].format[0]=='N') {

                 strtod(Table.fields[i].value, &end) ;

             if(*end!=0) {
                           sprintf(connect->error_text,
                                    "Illegal number format for DBF-field %s : %s", 
                                                             Table.fields[i].name,
                                                             Table.fields[i].value ) ;
                               ret_value=E_DBF_FILE ;
                                  break ;
                         }
                                            }
         else
         if(Table.fields[i].format[0]=='D') {

                     value=Table.fields[i].value ;

                if((value[0] <'0' || value[0]>'3'                  ) ||
                   (value[1] <'0' || value[1]>'9'                  ) ||
                   (value[2]!='.' && value[2]!='/' && value[2]!='-') ||
                   (value[3] <'0' || value[3]>'1'                  ) ||
                   (value[4] <'0' || value[4]>'9'                  ) ||
                   (value[5]!='.' && value[5]!='/' && value[5]!='-') ||
                   (value[6] <'0' || value[6]>'9'                  ) ||
                   (value[7] <'0' || value[7]>'9'                  ) ||
                   (value[8] <'0' || value[8]>'9'                  ) ||
                   (value[9] <'0' || value[9]>'9'                  )   ) {
                           sprintf(connect->error_text,
                                    "Illegal date format for DBF-field %s : %s", 
                                                             Table.fields[i].name,
                                                             Table.fields[i].value ) ;
                               ret_value=E_DBF_FILE ;
                                  break ;
                                                                         }

                                            }
         else                               {
                           sprintf(connect->error_text,
                                    "Unknown format %s for DBF-field %s : %s", 
                                                             Table.fields[i].format,
                                                             Table.fields[i].name,
                                                             Table.fields[i].value ) ;
                               ret_value=E_DBF_FILE ;
                                  break ;
                                            }

                                          }

                                 if(ret_value)  break ;
/*- - - - - - - - - - - - - - - - -  ���������� ������ � DBF-������� */
      for(i=0 ; i<Table.fields_cnt ; i++) {

         if(Table.fields[i].value [0]== 0 )  continue ;

         if(Table.fields[i].format[0]=='C') {

              if(dcl_sql_cp_convert)
                   Kernel->zCodePageConvert(       Table.fields[i].value,
                                            strlen(Table.fields[i].value),
                                                dcl_sql_cp_data, dcl_sql_cp_out) ;
                                            }
         else
         if(Table.fields[i].format[0]=='N')   continue ;
         else
         if(Table.fields[i].format[0]=='D') {

                            tmp[0]=Table.fields[i].value[6] ;
                            tmp[1]=Table.fields[i].value[7] ;
                            tmp[2]=Table.fields[i].value[8] ;
                            tmp[3]=Table.fields[i].value[9] ;
                            tmp[4]=Table.fields[i].value[3] ;
                            tmp[5]=Table.fields[i].value[4] ;
                            tmp[6]=Table.fields[i].value[0] ;
                            tmp[7]=Table.fields[i].value[1] ;
                            tmp[8]= 0  ;

                strcpy(Table.fields[i].value, tmp) ;
                                            }

                                          }
/*- - - - - - - - - - - - - - - - - - - - - - - ������ ������ � ���� */
            status=Table.Insert() ;
         if(status) {
                       strncpy(connect->error_text, Table.error_text, sizeof(connect->error_text)-1) ;  
                             ret_value=E_DBF_FILE ;
                                   break ;
                    }
/*- - - - - - - - - - - - - - - - - - - - - - - - ���������� ������� */
                     }                                              /* ENDLOOP - ��������� ������� */ 
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
      } while(0) ;                                                  /* BLOCK.1 */

#undef  NAME
#undef  VALUE

    if(ret_value==E_SQL)                                            /* ���� SQL-������ - ��������� �� ��������� ���������� */
             strncpy(connect->error_text, 
                      cursor->error_text, sizeof(connect->error_text)-1) ;  

/*----------------------------------- ���������� �������� ���������� */

   for(i=0 ; i<sql_pars_cnt ; i++) {

        par=(Dcl_decl *)sql_pars[i].ref ;
     if(par==NULL)  continue ;

        type= t_base(par->type) ;                                   /* ��������� ������� ��� */
/*- - - - - - - - - - - - - - - - - - - - - ������ ������������ ���� */
     if(type==_XTP_OBJ) {
                        }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - ������� ������ */
     else               {
   
                            size=strlen(sql_pars[i].buff) ;
        if(par->buff<size)  size=par->buff ;

                        memset(par->addr, 0, par->buff) ;
                        memcpy(par->addr, sql_pars[i].buff, size) ;
                               par->size=size ;
         Kernel->iSize_correct(par) ;                               /* �������� �������� ��������� �������� ���������� �� ������ */

                        }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                                   }
/*-------------------------------------------- ������������ �������� */

   if(cursor!=NULL)  connect->UnlockCursor(cursor) ;

                                        free(sql) ;

   for(i=0 ; i<sql_pars_cnt ; i++) {
                                        free(sql_pars[i].name) ;
                                        free(sql_pars[i].buff) ;
                                   } 

                Table.iFree() ;

/*-------------------------------------------------------------------*/

                       dcl_errno=ret_value ;

     if(ret_value==0)  ret_value=n ;

   return(&dgt_return) ;
}


/*********************************************************************/
/*                                                                   */
/*       ���������� � ������� �� SELECT-�������                      */
/*                                                                   */
/*        int  sql_open([hdc], sql[, value1, ..., valueN]) ;         */
/*                                                                   */
/*            int  hdc    - ���������� ����������                    */
/*           char *sql    - SQL-��������                             */
/*              ?  value? - ������������� ����������                 */
/*                                                                   */
/*  ���������� ���������� ������� (��� �������� ����������)          */
/*   ��� ��� ������:                                                 */
/*     �_CALL            -  ������������ ��������� ������            */
/*     �_PAR_TYPE        -  ���������������� ��� ����������          */
/*     �_CONNECT_HEADER  -  ������������ ���������� ����������       */
/*     �_CURSOR_MAX      -  ��� ��������� ��������                   */
/*     E_SQL_ERROR       -  ������ ���������� SQL-���������          */

  Dcl_decl *Dcl_sql_open(Lang_DCL *Kernel, Dcl_decl **pars, int  pars_cnt)

{
        SQL_cursor *cursor ;
               int  cursor_idx ;
          SQL_link *connect ;
               int  hdc ;
              char *sql ;                /* SQL-��������� */
     SQL_parameter  sql_pars[200] ;      /* ������ SQL-���������� */
               int  sql_pars_cnt ;
          Dcl_decl *par ;
               int  type ;
               int  status ;  
               int  size ;
               int  i ;

   static   double  ret_value ;          /* ����� ��������� �������� */
   static Dcl_decl  dgt_return={_DGT_VAL, 0, 0, 0, "", &ret_value, NULL, 1, 1} ;

/*------------------------------------------- ����������� ���������� */

  if(pars_cnt     <   1  ||
     pars[0]->addr==NULL   )  {          dcl_errno=E_CALL ; 
                                         ret_value=E_CALL ;
                                 return(&dgt_return) ;       }

      type=t_base(pars[0]->type) ;                                  /* ���� ������ �������� ����� �������� ��� - */
   if(type==_DGT_VAL) {                                             /*  - ������� ��� ������������ ����������    */
                         hdc=(int)Kernel->iDgt_get(pars[0]->addr, pars[0]->type) ;
                     connect=  NULL ;

                         pars++ ;                                   /*  �������� ������ �������� */
                         pars_cnt-- ;

                      }
   else               {                                             /* ���������� ��-��������� */
                         connect=dcl_sql_connect ;
                      }      

   if(connect==NULL) {
                                dcl_errno=E_HDC ;
                                ret_value=E_HDC ;
                        return(&dgt_return) ;
                     }

              connect->error_text[0]=0 ;                            /* ����� ������ */

/*----------------------------------------- ���������� SQL-��������� */

  if(pars_cnt     <   1  ||
     pars[0]->addr==NULL   )  {          dcl_errno=E_CALL ;
                                         ret_value=E_CALL ;
                                 return(&dgt_return) ;       }

      type=t_base(pars[0]->type) ;
   if(type!=_DCLT_CHR_AREA &&                                       /* �������� ���� �������� */
      type!=_DCLT_CHR_PTR    ) {         dcl_errno=E_CALL ;
                                         ret_value=E_CALL ;
                                 return(&dgt_return) ;       }

                  sql=(char *)calloc(1, pars[0]->size+8) ;          /* �������� ������ �� pars[0]->size */
          memcpy( sql, pars[0]->addr, pars[0]->size) ;              /* ��������� SQL-�������� */

/*-------------------------------- ���������� ���������� ����������� */

   for(sql_pars_cnt=0, i=1 ; i<pars_cnt ; i++) {

        type= t_base(pars[i]->type) ;                               /* ��������� ������� ��� */
/*- - - - - - - - - - - - - - - - - - - - - ������ ������������ ���� */
     if(type==_XTP_OBJ) {
                        }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - ������� ������ */
     else               {

              ret_value=iDcl_sql_parform(pars[i], &sql_pars[sql_pars_cnt])  ;
           if(ret_value<0) {
                                     dcl_errno=ret_value ;
                             return(&dgt_return) ;
                           }

                                       sql_pars_cnt++ ;
                        }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                                               } 
/*----------------------------------- �������� ������� �� ���������� */

       cursor=connect->LockCursor("Dcl_sql_execute") ;              /* ����������� ������ */
    if(cursor==NULL) {          dcl_errno=E_CURSOR_MAX ;
                                ret_value=E_CURSOR_MAX ;
                        return(&dgt_return) ;             }

       for(i=0 ; i<_CURSORS_MAX ; i++)                              /* ���� ��������� ���� ��� ������� */
         if(dcl_sql_cursors[i].connect==NULL)  break ;

         if(i>=_CURSORS_MAX) {                                      /* ���� ��� ������... */
                                 connect->UnlockCursor(cursor) ; 
                                      dcl_errno=E_CURSOR_MAX ;   
                                      ret_value=E_CURSOR_MAX ;   
                               return(&dgt_return) ;   
                             }

                             cursor_idx=i ;
             dcl_sql_cursors[cursor_idx].connect=connect ;          /* ��������� ������ �� ���������� */
             dcl_sql_cursors[cursor_idx].cursor =cursor ;

/*--------------------------------------------- ���������� �������� */

       status=connect->SelectOpen(cursor, sql,                      /* ��������� SELECT-�������� */
                                          sql_pars, sql_pars_cnt) ;
    if(status)  ret_value=     E_SQL ;
    else        ret_value=cursor_idx ;

    if(status) {                                                    /* ��� ������ ����������� ������ � ���������� */
                  dcl_sql_cursors[cursor_idx].connect=NULL ;
                  dcl_sql_cursors[cursor_idx].cursor =NULL ;

                        connect->UnlockCursor(cursor) ;
               } 
/*----------------------------------- ���������� �������� ���������� */

   for(i=0 ; i<sql_pars_cnt ; i++) {

        par=(Dcl_decl *)sql_pars[i].ref ;
     if(par==NULL)  continue ;

        type= t_base(par->type) ;                                   /* ��������� ������� ��� */
/*- - - - - - - - - - - - - - - - - - - - - ������ ������������ ���� */
     if(type==_XTP_OBJ) {
                        }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - ������� ������ */
     else               {
   
                            size=strlen(sql_pars[i].buff) ;
        if(par->buff<size)  size=par->buff ;

                        memset(par->addr, 0, par->buff) ;
                        memcpy(par->addr, sql_pars[i].buff, size) ;
                               par->size=size ;
         Kernel->iSize_correct(par) ;                               /* �������� �������� ��������� �������� ���������� �� ������ */
                        }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                                   }
/*-------------------------------------------- ������������ �������� */

                                        free(sql) ;

   for(i=0 ; i<sql_pars_cnt ; i++) {
                                        free(sql_pars[i].name) ;
                                        free(sql_pars[i].buff) ;
                                   } 
/*-------------------------------------------------------------------*/

    if(ret_value==E_SQL)                                            /* ���� ������ - ��������� �� ��������� ���������� */
             strncpy(connect->error_text, 
                      cursor->error_text, sizeof(connect->error_text)-1) ;  

           dcl_errno=ret_value ;

   return(&dgt_return) ;
}


/*********************************************************************/
/*                                                                   */
/*           �������� ��������� ��� ���� ��������                    */
/*                                                                   */
/*        int  sql_close([hcursor]) ;                                */
/*                                                                   */
/*            int  hcursor - ���������� �������                      */
/*                                                                   */
/*  ���������� 0 ��� �������� ���������� ��� ��� ������:             */
/*     �_CURSOR_HEADER  -  ������������ ���������� ����������        */

  Dcl_decl *Dcl_sql_close(Lang_DCL *Kernel, Dcl_decl **pars, int  pars_cnt)

{
               int  idx ;
               int  type ;
               int  i ;

   static   double  ret_value ;          /* ����� ��������� �������� */
   static Dcl_decl  dgt_return={_DGT_VAL, 0, 0, 0, "", &ret_value, NULL, 1, 1} ;

/*---------------------------------- ����������� ����������� ������� */

   if(pars_cnt>=1) {                                                /* ���� ����� ���������� ������� */
                      type=t_base(pars[0]->type) ;

      if(pars[0]->addr==  NULL   ||                                 /*  �������� ���� �������� */
                  type!=_DGT_VAL   ) {        dcl_errno=E_CALL ; 
                                              ret_value=E_CALL ;
                                       return(&dgt_return) ;      }

               idx=(int)Kernel->iDgt_get(pars[0]->addr, pars[0]->type) ;

      if(idx<0 || idx>=_CURSORS_MAX) {                              /*  �������� �������� ����������� ������� */
                                               dcl_errno=E_CALL ;
                                               ret_value=E_CALL ;
                                        return(&dgt_return) ;      
                                     }

      if(dcl_sql_cursors[idx].connect==NULL) {                      /*  �������� ������������ ����������� ������� */
                                               dcl_errno=E_HCURSOR ;
                                               ret_value=E_HCURSOR ;
                                       return(&dgt_return) ;      
                                             }
                   }                     
   else            {
                                   idx= -1 ;
                   }  
/*------------------------------------------------ ��� ���� �������� */

    if(idx==-1) {

       for(i=0 ; i<_CURSORS_MAX ; i++)
         if(dcl_sql_cursors[i].connect!=NULL) {

             dcl_sql_cursors[i].connect->UnlockCursor(dcl_sql_cursors[i].cursor) ; 

                  dcl_sql_cursors[i].connect=NULL ;
                  dcl_sql_cursors[i].cursor =NULL ;

                                              }
                } 
/*------------------------------------------- ��� ���������� ������� */

    else        {

             dcl_sql_cursors[idx].connect->UnlockCursor(dcl_sql_cursors[idx].cursor) ; 

             dcl_sql_cursors[idx].connect=NULL ;
             dcl_sql_cursors[idx].cursor =NULL ;

                } 
/*-------------------------------------------------------------------*/

           dcl_errno=ret_value ;

   return(&dgt_return) ;
}


/*********************************************************************/
/*                                                                   */
/*                ������ ������ ��������� SQL-������                 */
/*                                                                   */
/*          int  sql_error([hdc], text) ;                            */
/*                                                                   */
/*              int  hdc  - ���������� ����������                    */
/*             char *text - ����� ��� ������ ������                  */
/*                                                                   */
/*    ���������� 0 ��� �������� ���������� ��� ��� ������:           */
/*     �_CALL            -  ������������ ��������� ������            */
/*     �_CONNECT_HEADER  -  ������������ ���������� ����������       */

  Dcl_decl *Dcl_sql_error(Lang_DCL *Kernel, Dcl_decl **pars, int  pars_cnt)

{
          SQL_link *connect ;
               int  hdc ;
               int  type ;
              char *error ;

   static   double  ret_value ;          /* ����� ��������� �������� */
   static Dcl_decl  dgt_return={_DGT_VAL, 0, 0, 0, "", &ret_value, NULL, 1, 1} ;

/*------------------------------------------- ����������� ���������� */

  if(pars_cnt     <   1  ||
     pars[0]->addr==NULL   )  {          dcl_errno=E_CALL ;
                                         ret_value=E_CALL ;
                                 return(&dgt_return) ;       }

      type=t_base(pars[0]->type) ;                                  /* ���� ������ �������� ����� �������� ��� - */
   if(type==_DGT_VAL) {                                             /*  - ������� ��� ������������ ����������    */
                         hdc=(int)Kernel->iDgt_get(pars[0]->addr, pars[0]->type) ;
                     connect=  NULL ;

                         pars++ ;                                   /*  �������� ������ �������� */
                         pars_cnt-- ;

                      }
   else               {                                             /* ���������� ��-��������� */
                         connect=dcl_sql_connect ;
                      }      

/*------------------------------------- ����������� ��������� ������ */

   if(connect==NULL)  error= dcl_sql_error ;                        /* ���� ��������� ��� ���������� - ���������� ��������� ����� */
   else               error=connect->error_text ;

/*---------------------------------------------------- ������ ������ */

  if(pars_cnt     <   1  ||
     pars[0]->addr==NULL   )  {          dcl_errno=E_CALL ;
                                         ret_value=E_CALL ;
                                 return(&dgt_return) ;       }

      type=t_base(pars[0]->type) ;
   if(type!=_DCLT_CHR_AREA &&                                       /* �������� ���� �������� */
      type!=_DCLT_CHR_PTR    ) {         dcl_errno=E_CALL ;
                                         ret_value=E_CALL ;
                                 return(&dgt_return) ;       }


      memset(        pars[0]->addr, 0, pars[0]->buff) ;             /* ������ ������ */
     strncpy((char *)pars[0]->addr, error, pars[0]->buff-1) ;

                     pars[0]->size=strlen((char *)pars[0]->addr) ;

           Kernel->iSize_correct(pars[0]) ;                         /*  �������� �������� ���������     */
								    /*    �������� ���������� �� ������ */

/*-------------------------------------------------------------------*/

           dcl_errno=ret_value ;

   return(&dgt_return) ;
}


/*********************************************************************/
/*                                                                   */
/*       ������������ SQL-��������� �� DCL-���������                 */

  double  iDcl_sql_parform(Dcl_decl *par, SQL_parameter *sql)

{
   int  type ;

/*----------------------------------------------- ������������ ����� */

                     sql->name=(char *)calloc(1, 128) ;             /* ��������� �������������� ��� ��������� */
             sprintf(sql->name, ":%s", par->name) ;                 /*    ����������� ������� ���������       */ 
                     sql->blob     =0 ;
                     sql->blob_size=0 ;

                 type=t_base(par->type) ;                           /* ��������� ������� ��� */

/*--------------------------------------------------- NULL-��������� */

       if(     type==_NULL_PTR ||                                   /* ��� NULL-���������� */
          par->addr== NULL       ) {

                  sql->buff=(char *)calloc(1, par->buff+8) ;
                  sql->size= par->buff+1 ;
                  sql->ref = NULL ;

                                   }  
/*---------------------------------------------- �������� ���������� */

       else
       if(type==_DCLT_CHR_AREA ||                                   /* ��� �������� ���������� */
          type==_DCLT_CHR_PTR    ) {

                     sql->buff=(char *)calloc(1, par->buff+8) ;
              memcpy(sql->buff, par->addr, par->size) ;
                     sql->size= par->buff+1 ;
                     sql->ref = par ;

         if(  type==_DCLT_CHR_PTR ||                                /* BLOB-��������� ������� ������������ ������� � ��������� */
            ( par->prototype!=NULL &&
             *par->prototype=='D'    ) ) {
                             sql->blob     = 1 ;
                             sql->blob_size=par->size ;  
                                         }

                                   }
/*-------------------------------------------- ���������������� ���� */

       else                        {                                /* ��� ��������������� ����� */
                                           return(E_CALL) ; 
                                   }
/*-------------------------------------------------------------------*/

   return(0.) ;
}


/*********************************************************************/
/*                                                                   */
/*   ��������� ������ ���������� � ������� �� ����������� �������    */

    int  iDcl_sql_getcursor(       int   idx, 
                              SQL_link **connect, 
                            SQL_cursor **cursor  )

{
      if(idx<0 || idx>=_CURSORS_MAX) {                              /*  �������� �������� ����������� ������� */
                                           *connect=NULL ;
                                            *cursor=NULL ;
                                              return(-1) ;
                                     }

   
            *connect=dcl_sql_cursors[idx].connect ;
             *cursor=dcl_sql_cursors[idx].cursor ;  

     if(*connect==NULL ||
         *cursor==NULL   ) {  dcl_errno=E_HCURSOR ;
                                   return(-1) ;      }

   return(0) ;
}

/*********************************************************************/
/*                                                                   */
/*              ���������� SELECT-�������                            */

   double  Dcl_sql_fetch(Lang_DCL *Kernel, Dcl_decl *source, 
                                                int  cursor_idx, 
                                                int  fetch_cnt  )

{
          SQL_link *connect ;
        SQL_cursor *cursor ;
               int  status ;
               int  size ;
               int  i ;
               int  j ;

          Dcl_decl *rec_data ;
  Dcl_complex_type  rec_template={ "sql", 0, 0, 0} ;

#define  NAME   (char *)cursor->columns[j].name
#define  VALUE  (char *)cursor->columns[j].value

/*----------------------------------------------- ���������� ������� */

      if(cursor_idx<       0     ||                                 /* �������� �������� ����������� ������� */
         cursor_idx>=_CURSORS_MAX  ) {  dcl_errno=E_HCURSOR ;
                                               return(-1) ;    }

         connect=dcl_sql_cursors[cursor_idx].connect ;
          cursor=dcl_sql_cursors[cursor_idx].cursor ;  

      if(connect==NULL ||
          cursor==NULL   ) {  dcl_errno=E_HCURSOR ;
                                   return(-1) ;      }

              connect->error_text[0]=0 ;                            /* ����� ������ */

/*------------------------------------ ������������ ��������� ������ */

         rec_data=(Dcl_decl *)                                      /* ��������� �������� ����� */
                   calloc(cursor->columns_cnt, sizeof(*rec_data)) ;

    for(j=0 ; j<cursor->columns_cnt ; j++) {                        /* ��������� �������� ����� */

                  rec_data[j].type=_CHR_AREA ;
          strncpy(rec_data[j].name, NAME, sizeof(rec_data[j].name)-1) ;
                                           }
/*------------------------------------------------ ���������� ������ */

    for(i=0 ; fetch_cnt<=0 || i<fetch_cnt ; i++) {                  /* LOOP - ��������� ������� */ 

           status=connect->SelectFetch(cursor) ;                    /* ��������� ��������� ������ */
        if(status==_SQL_NO_DATA)   break ;                          /* ���� ��� ������ ���������... */
        if(status) {                                                /* ���� ������ ������� */
                     dcl_errno=E_SQL ;
                      break ; 
                   }

      for(size=0, j=0 ; j<cursor->columns_cnt ; j++) {                      /* ����������� ������ ����� */
                    rec_data[j].addr     =(void *)size ;
                    rec_data[j].prototype=        VALUE ;
                    rec_data[j].size     = strlen(VALUE) ;
                    rec_data[j].buff     = strlen(VALUE)+1 ;
                                size    +=rec_data[j].buff ;
                                                     }

                    rec_template.list    =rec_data ;                /* ������� ������ ������ */
                    rec_template.list_cnt=cursor->columns_cnt ;
                    rec_template.size    =  size ;

             status=Kernel->iXobject_add(source, &rec_template) ;   /* ��������� ������ */
          if(status) break ;
          
                                                 }                  /* ENDLOOP - ��������� ������� */ 
/*-------------------------------------------- ������������ �������� */

                          free(rec_data) ;

/*-------------------------------------------------------------------*/

#undef  NAME
#undef  VALUE

   return(0) ;
}
