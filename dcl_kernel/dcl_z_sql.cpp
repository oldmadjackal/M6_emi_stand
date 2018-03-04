/*********************************************************************/
/*                                                                   */
/*                   DATA CONVERTION LANGUAGE                        */
/*                                                                   */
/*          ������� ��� ������ � SQL-���������                       */
/*                                                                   */
/*********************************************************************/

#include  <errno.h>
#include  <stddef.h>
#include  <stdlib.h>
#include  <string.h>
#include  <stdio.h>
#include  <ctype.h>
#include  <math.h>
#include  <memory.h>
#include  <sys/types.h>
#include  <sys/stat.h>

#ifdef UNIX
#include  <unistd.h>
#define   stricmp  strcasecmp
#else
#endif


#include "sql_common.h"
#include "sql_direct.h"

#define   DCL_INSIDE
#include "dcl.h"

#pragma warning(disable : 4996)

/*---------------------------------------------------- ������� ����� */

     extern         double  dcl_errno ;              /* ��������� ��������� ������ -> DCL_SLIB.CPP */
     extern class SQL_link *dcl_sql_connect ;        /* ��������� SQL-�������� ��-��������� */ 
     extern           char  dcl_sql_output_ignore ;  /* ���� ������������� �������� ���������� SQL */

    Dcl_decl *Dcl_sql_open   (Lang_DCL *, Dcl_decl **, int) ;      /* �������� ������� */
    Dcl_decl *Dcl_sql_close  (Lang_DCL *, Dcl_decl **, int) ;      /* �������� ������� */
      double  Dcl_sql_fetch  (Lang_DCL *, Dcl_decl *, int, int) ;  /* ���������� ������� */
    Dcl_decl *Dcl_sql_execute(Lang_DCL *, Dcl_decl **, int) ;      /* ���������� ��������� */


/*********************************************************************/
/*                                                                   */
/*              ���������� ����������� SELECT-�������                */

   int  Lang_DCL::zSqlSelectOnce(Dcl_decl  *source, 
                                 Dcl_decl **pars, 
                                      int   pars_cnt)

{
         int  fetch_cnt ;
    Dcl_decl *result ;
      double  fetch_errno ;

#define  RESULT   (*((double *)result->addr)) 
                                                        
/*------------------------------ ����������� ����� ����������� ����� */

     fetch_cnt=(int)this->iDgt_get(pars[0]->addr, pars[0]->type) ;

            fetch_errno=0 ;

/*---------------------------------------------------------- ������� */

          result=Dcl_sql_open(this, pars+1, pars_cnt-1) ;           /* �������� ������ */
       if(RESULT<0)  return(-1) ;                                   /* ���� ������... */

                Dcl_sql_fetch(this, source, RESULT, fetch_cnt) ;    /* ��������� ������� */
          fetch_errno=dcl_errno ;

                Dcl_sql_close(this, &result, 1) ;                   /* ����������� ������ */ 

       if(  dcl_errno!=0. || 
          fetch_errno!=0.   ) {
            if(fetch_errno!=0.)  dcl_errno=fetch_errno ;
                                  return(-1) ;
                              }
/*-------------------------------------------------------------------*/

#undef  RESULT

  return(0) ;
}


/*********************************************************************/
/*                                                                   */
/*        ���������� DML-��������� (INSERT, UPDATE, DELETE)          */ 
/*                     �� ���� ��������� �������                     */

   int  Lang_DCL::zSqlDmlExecute(Dcl_decl  *source, 
                                 Dcl_decl **pars, 
                                      int   pars_cnt)

{
  Dcl_complex_record *record ;
            Dcl_decl *result ;
            Dcl_decl  sql_var ;
            Dcl_decl  row_id ;
            Dcl_decl *sql_pars[200] ;      /* ������ SQL-���������� */
                 int  sql_pars_cnt ;
                char *sql ;                /* SQL-���������, ��������� */
                char *sql_ ;               /* SQL-���������, ������� */
                char  row_id_buff[32] ;
                char  options[512] ;
              double  exec_errno ;
                 int  marked_only ;
                 int  type ;
                 int  i ;
                 int  j ;
                 int  k ;

#define  RESULT   (*((double *)result->addr)) 

/*------------------------------------------------------- ���������� */

          marked_only =source->work_nmb & _DCL_MARKED_ONLY ;

                  sql =(char *)calloc(1, pars[0]->size+8) ;         /* �������� ������ �� pars[0]->size */
                  sql_=(char *)calloc(1, pars[0]->size+8) ;
          memcpy( sql, pars[0]->addr, pars[0]->size) ;              /* ��������� SQL-�������� */

                    memset(options, 0, sizeof(options)) ;
    if(pars_cnt>1)  memcpy(options, pars[1]->addr, pars[1]->size) ; /* ���� ����� �������� ����� - ��������� ��� */

                memset(&sql_var, 0, sizeof(sql_var)) ;
                memset(&row_id,  0, sizeof(row_id)) ;

                    exec_errno=0. ;

/*------------------------------------------------------- ���������� */

     for(record=(Dcl_complex_record *)source->addr,                 /* LOOP - ���������� ������ */
                        i=0 ; i<source->buff ; i++, 
         record=(Dcl_complex_record *)record->next_record) {

       if(marked_only && !record->find_mark)  continue ;            /* ��������� ������ MARKED_ONLY */
/*- - - - - - - - - - - - - - - - -  ���������� ������ SQL-��������� */
                      strcpy(sql_, sql) ;

                sql_var.addr=sql_ ;
                sql_var.size=strlen(sql_)  ;
                sql_var.buff=sql_var.size+1 ;
                sql_var.type=_DCLT_CHR_AREA ;
/*- - - - - - - - - - - - - - - - - ���������� �������������� ������ */
                      sprintf(row_id_buff, "%d", i) ;

         strcpy(row_id.name, "ROWID") ;
                row_id.addr=row_id_buff ;
                row_id.size=strlen(row_id_buff)  ;
                row_id.buff=row_id.size+1 ;
                row_id.type=_DCLT_CHR_AREA ;
/*- - - - - - - - - - - - - - - - - - -  ���������� ����� ���������� */
                          sql_pars_cnt =  0 ;                       /* � ������ ����� ���������� ����� SQL-�������� */
                 sql_pars[sql_pars_cnt]=&sql_var ;
                          sql_pars_cnt++ ;

                 sql_pars[sql_pars_cnt]=&row_id ;                   /* ����� ����� ������������� ������ */
                          sql_pars_cnt++ ;


          for(k=0 ; k<record->elems_cnt ; k++) {                    /* LOOP - ���������� �������� */

#define  E           record->elems[k]
#define  A  ((char *)record->elems[k].addr)

                 type=t_base(E.type) ;                              /* ��������� ������� ��� */

            if(strstr(options, "TRIM")!=NULL)                       /* ������������� �������� */
             if(type==_CHR_AREA ||
                type==_CHR_PTR    ) {

               for(j=E.size-1 ; j>=0 ; j--) 
                 if(A[j]==' ')  E.size-- ;
                 else            break ;

               for(j=0 ; j<E.size ; j++) 
                 if(A[j]!=' ')  break ;

                if(j>0) {         E.size-=j ;
                          memmove(A, A+j, E.size) ;
                        }
                                    }

#undef   A
#undef   E

                 sql_pars[sql_pars_cnt]=&record->elems[k] ;
                          sql_pars_cnt++ ;
                                               }                    /* ENDLOOP - ���������� �������� */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - ���������� */
                 dcl_sql_output_ignore=1 ;                          /* �������� ���������� ���� �� ������ */
          result=Dcl_sql_execute(this, sql_pars, sql_pars_cnt) ;
                 dcl_sql_output_ignore=0 ;
       if(RESULT<0) {  exec_errno=dcl_errno ;  break ;  }           /* ���� ������... */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                                                           }        /* ENDLOOP - ���������� ������ */
/*------------------------------------------------------- ���������� */

       if( dcl_errno!=0. || 
          exec_errno!=0.   ) {
            if(exec_errno!=0.)  dcl_errno=exec_errno ;
                                  return(-1) ;
                             }
/*-------------------------------------------------------------------*/

#undef  RESULT

  return(0) ;
}


/*********************************************************************/
/*                                                                   */
/*           ��������� �������� ���������� �������                   */

   int  Lang_DCL::zSqlDescribe(Dcl_decl  *source, 
                               Dcl_decl **pars, 
                                    int   pars_cnt)

{
   SQL_column  *list ;
         char   schema[128] ;
         char   table[128] ;
          int   columns_cnt ;
          int   i ;

            Dcl_decl  column_data[5]={
                         {_CHR_AREA, 0, 0, 0, "name",      (void *)0,    NULL,  0,  128},
                         {_CHR_AREA, 0, 0, 0, "type",      (void *)128,  NULL,  0,   32},
                         {_DCL_LONG, 0, 0, 0, "size",      (void *)160,  NULL,  1,    1},
                         {_DCL_LONG, 0, 0, 0, "precision", (void *)168,  NULL,  1,    1},
                         {_CHR_AREA, 0, 0, 0, "remark",    (void *)176,  NULL,  0,  512}
                                     } ;
    Dcl_complex_type  column_template={ "COLUMN", 1024, column_data, 5} ;

#define  RESULT   (*((double *)result->addr)) 
                               
/*-------------------------------------------- ���������� ���������� */

	  memset(table,  0, sizeof(table)) ;
	  memset(schema, 0, sizeof(schema)) ;

   if(pars[0]->size>=sizeof(table))                                 /* �������� �������� ���������� */
                  pars[0]->size=sizeof(table)-1 ;
   if(pars[1]->size>=sizeof(schema))
                  pars[1]->size=sizeof(schema)-1 ;

	  memcpy(table,  pars[0]->addr, pars[0]->size) ;            /* ���������� �������� ���������� */ 
	  memcpy(schema, pars[1]->addr, pars[1]->size) ;

/*---------------------------------------------------------- ������� */

                                                list=NULL ;

      columns_cnt=dcl_sql_connect->EnumColumns(&list, NULL, schema, table) ;
   if(columns_cnt<0)  return(-1) ;

/*------------------------------------- ���������� ������� X-������� */

     for(i=0 ; i<columns_cnt ; i++) {

                column_data[0].prototype=         list[i].name ;
                column_data[0].size     =  strlen(list[i].name) ;
                column_data[1].prototype=         list[i].type ;
                column_data[1].size     =  strlen(list[i].type) ;
                column_data[2].prototype=(char *)&list[i].size ;
                column_data[3].prototype=(char *)&list[i].precision ;
                column_data[4].prototype=         list[i].remark ;
                column_data[4].size     =  strlen(list[i].remark) ;

                   iXobject_add(source, &column_template) ;
              if(mError_code)  break ;

                                   }
/*-------------------------------------------- ������������ �������� */

                 dcl_sql_connect->EnumFree(list) ;

/*-------------------------------------------------------------------*/

#undef  RESULT

  return(i) ;
}


/*********************************************************************/
/*                                                                   */
/*              ���������� DBF-�����                                 */

   int  Lang_DCL::zDbfRead(Dcl_decl  *source, 
                           Dcl_decl **pars, 
                                int   pars_cnt)

{
    SQL_direct_DBF  Table ;
              char   path[FILENAME_MAX] ;
              char   fields[4096] ;
              char   mask[32] ;
               int   status ;
               int   f_cnt ;
               int   size ;
               int   row ;
               int   n ;
               int   i ;

          Dcl_decl *rec_data ;
  Dcl_complex_type  rec_template={ "dbf", 0, 0, 0} ;

/*-------------------------------------------- ���������� ���������� */

	  memset(path,   0, sizeof(path)) ;
	  memset(fields, 0, sizeof(fields)) ;

   if(pars[0]->size>=sizeof(path))                                  /* �������� �������� ���������� */
                  pars[0]->size=sizeof(path)-1 ;
   if(pars[1]->size>=sizeof(fields)-2)
                  pars[1]->size=sizeof(fields)-3 ;

	  memcpy(path,     pars[0]->addr, pars[0]->size) ;          /* ���������� �������� ���������� */ 

   if(pars[1]->size>0) {
	         fields[0]=',' ;                                    /* ������ ����� ��������� ������������ ���� */
	  memcpy(fields+1, pars[1]->addr, pars[1]->size) ;
	  strcat(fields, ",") ;
                       }
/*----------------------------------------------- �������� DBF-����� */

        status=Table.Open(path, NULL) ;                             /* �������� ����� DBF-������� */
     if(status) {
                          this->mError_code=_DCLE_CALL_INSIDE ;
                  strncpy(this->mError_details, Table.error_text, sizeof(this->mError_details)-1) ;
                                          return(-1) ;
                }
/*---------------------------------------- �������� ���������� ����� */

   if(strlen(fields)==0) {
      for(i=0 ; i<Table.fields_cnt ; i++)  Table.fields[i].mark=i ;
                            f_cnt=Table.fields_cnt ;
                         }
   else                  {

      for(f_cnt=0, i=0 ; i<Table.fields_cnt ; i++) {

          sprintf(mask, ",%s,", Table.fields[i].name) ;

                                          Table.fields[i].mark=-1 ;
        if(strstr(fields, mask)!=NULL) {
                                          Table.fields[i].mark= f_cnt ;
                                                                f_cnt++ ;
                                       }
                                                   }
                         }
/*---------------------------------------- ���������� ������� ������ */

         rec_data=(Dcl_decl *)calloc(f_cnt, sizeof(*rec_data)) ;    /* ��������� ������ �������� */

      for(i=0 ; i<Table.fields_cnt ; i++)                           /* ��������� ����� � ������ ����� */
        if(Table.fields[i].mark>=0) {
                              n=Table.fields[i].mark ;
               strncpy(rec_data[n].name, Table.fields[i].name, sizeof(rec_data->name)-1) ; 
                       rec_data[n].buff=Table.fields[i].len+1 ;
                       rec_data[n].size=Table.fields[i].len ;
                                    }
/*--------------------------------------------- ���������� DBF-����� */

              row=0 ;

     do {
              row++ ;
           if(row==1)  status=Table.GetFirst() ;
           else        status=Table.GetNext() ;

           if(status>0) {
                           status=0 ;
                             break ;
                        }
           if(status<0) {
                          this->mError_code=_DCLE_CALL_INSIDE ;
                  strncpy(this->mError_details, Table.error_text, sizeof(this->mError_details)-1) ;
                                   break ;
                        }

      for(size=0, i=0 ; i<Table.fields_cnt ; i++)                   /* ��������� �������� ����� */
        if(Table.fields[i].mark>=0) {
                              n           =Table.fields[i].mark ;
                     rec_data[n].addr     =(void *)size ;
                     rec_data[n].prototype=Table.fields[i].value ;
                                 size    +=rec_data[n].buff ;
                                    }

                    rec_template.list    =rec_data ;                /* ������� ������ ������ */
                    rec_template.list_cnt=f_cnt ;
                    rec_template.size    =  size ;

              status=iXobject_add(source, &rec_template) ;
           if(status) {
                           this->mError_code=_DCLE_TYPEDEF_ELEM ;
                                   break ;
                      }

        } while(1) ;

/*----------------------------------------------- �������� DBF-����� */

              Table.iFree() ;

                     free(rec_data) ;

/*-------------------------------------------------------------------*/

  return(status) ;
}


