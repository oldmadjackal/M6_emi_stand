/*   Bolotov P.D.  version  29.04.1994   */
/*     0 warnings at warning level 1     */

/*********************************************************************/
/*                                                                   */
/*                   DATA CONVERTION LANGUAGE                        */
/*                                                                   */
/*                 ���������� �������� � ������                      */
/*                                                                   */
/*********************************************************************/

#include  <errno.h>
#include  <stddef.h>
#include  <stdlib.h>
#include  <string.h>
#include  <malloc.h>
#include  <ctype.h>
#include  <math.h>


#define   DCL_INSIDE

#include "dcl.h"


/*----------------------------------- ���������� ������������ ������� */
/*                                                                    */
/*                                         �������� ����������        */
/*  dcl_all_vars -> �=======�<-vars[0] ->-----------�  ->-----------� */
/*  (=dcl_vars)     �-------�<-vars[1] � � Internal �  � �   Work   � */
/*                  �-------�   ...    � �          �  � �          � */
/*                  �-------�<-vars[N] � �          �  � �          � */
/*                  �=======� ---------- L-----------  � L----------- */
/*                  �-------� --------------------------              */
/*                  �=======�<-NULL                                   */
/*                  L=======-                                         */
/*                                                                    */

/*------------------------------------------------------ ����������� */

//#define DGT_SZ     sizeof(double)
#define CHR_SZ     sizeof(char)

/*-------------------------------------------------------------------*/


/************************************************************************/
/*                                                                      */
/*              ������������� ������� ���������� �������                */
/*                                                                      */
/*    vars  -   ������� ������� ������� ����������                      */
/*                                                                      */
/*    Return: ����� ����� ������� ������� ����������                    */

   Dcl_decl **Lang_DCL::iMem_init(Dcl_decl **vars)

{
  int  all_cnt ;  /* ����� ����� ������� ���������� */
  int  ext_cnt ;  /* ������� ������� ������� ���������� */
  int  i ;


			    iMem_free() ;                           /* ����� ������� ���������� ������� */

                         nInt_cnt=0 ;
                        nWork_cnt=0 ;

         nInt_page=(Dcl_decl *)                                     /* ��������� �������� ���������� ���������� */
                      calloc(_TITLE_PAGE, sizeof(*nInt_page)) ;
      if(nInt_page==NULL) {  mError_code=_DCLE_MEMORY ;             /* �����.�������� ������ */
		                      return(NULL) ;      }
         nWork_page=(Dcl_decl *)                                    /* ��������� �������� ���������� ���������� */
                      calloc(_TITLE_PAGE, sizeof(*nWork_page)) ;
      if(nWork_page==NULL) {  mError_code=_DCLE_MEMORY ;            /* �����.�������� ������ */
		                      return(NULL) ;      }
         nWork_mem =(void **)                                       /* ��������� �������� ������ ���������� ���������� */
                      calloc(_TITLE_PAGE, sizeof(*nWork_mem)) ;
      if(nWork_mem ==NULL) {  mError_code=_DCLE_MEMORY ;            /* �����.�������� ������ */
		                      return(NULL) ;      }

      if(vars!=NULL)                                                /* ���� ���� ������� ���������� - */
	    for(ext_cnt=0 ; vars[ext_cnt]!=NULL ; ext_cnt++) ;      /*   ������������ ����� �������   */
      else      ext_cnt=0 ;                                         /*     ������� ����������         */

	       all_cnt=ext_cnt+2+1 ;                                /* ��������� �������� �����. � ������� ���������� */

	   nAll_vars=(Dcl_decl **)                                  /* ��������� ����� ������� ������� */
                      calloc(all_cnt, sizeof(*nAll_vars)) ;
	if(nAll_vars==NULL) {  mError_code=_DCLE_MEMORY ;           /* �����.�������� ������ */
		                      return(NULL) ;      }

    for(i=0 ; i<ext_cnt ; i++)  nAll_vars[i]=vars[i] ;              /* �������� ������� ������� ������� */
								    /*   ���������� � ����� �������     */
    for(    ; i<all_cnt ; i++)  nAll_vars[i]=NULL ;                 /* ����. ������� ������� ����������   */
								    /*   �����.����������, � ����� ������ */
								    /*    ����� ����� ��������� �������   */  
          nAll_vars[ext_cnt]=nInt_page ;                            /* �������� �������� ���������� ���������� ���������� */

   return(nAll_vars) ;
}


/*********************************************************************/
/*                                                                   */
/*                  �������� ������� ���������� �������              */

       void Lang_DCL::iMem_free(void)

{
  int  i ;


   if(nAll_vars==NULL)  return ;                                    /* �������� '�������' ���������� */

    for(i=0 ; i<nInt_cnt ; i++)                                     /* ����������� ������� ���������� - ����� ���������� */
     if((nInt_page[i].type & _DCLT_DGT_PTR)!=_DCLT_DGT_PTR &&
        (nInt_page[i].type & _DCLT_CHR_PTR)!=_DCLT_CHR_PTR   )
       if(nInt_page[i].addr!=NULL)  free(nInt_page[i].addr) ;

                                    free(nInt_page) ;

    for(i=0 ; i<nWork_cnt ; i++)                                    /* ����������� ���������� ���������� */
     if(nWork_mem[i]!=NULL)  free(nWork_mem[i]) ;

                             free(nWork_mem) ;
                             free(nWork_page) ;

                             free(nAll_vars) ;                      /* ���.������ ����� ������� ������� */
	                          nAll_vars=NULL ;                  /* '�����' ���������� */
}


/************************************************************************/
/*                                                                      */
/*                ������ ������ ���������� ���������                    */
/*                                                                      */
/*   type  -  ��� ������: _WORK_VAR - ������� ����������                */
/*                    _INTERNAL_VAR - �����.����������                  */
/*                  _CLEAR_WORK_VAR - �������� ������� ����������       */
/*                                                                      */
/*    ������� ���������� (_WORK_VAR) ����������� �� ��������� ��������  */
/*  �� ��������� ���������, ������� ����������� ��� ����������          */
/*  ����������(_INTERNAL_VAR).                                          */

    Dcl_decl *Lang_DCL::iNext_title(int  type)

{
  Dcl_decl *page  ;   /* ������� �������� ���������� */
  Dcl_decl *title ;   /* ���������� ��������� */
       int  i ;

/*---------------------------------------------------- ������������� */

                          title=NULL ;

/*---------------------------------- ������������ ������� ���������� */

  if(type==_CLEAR_WORK_VAR) {

       for(i=1 ; i<nWork_cnt ; i++)                                 /* ��������! ������� ���������� ����������� � ������� 1 */
         if(nWork_mem[i]!=NULL)  free(nWork_mem[i]) ;

                                      nWork_cnt=0 ;                 /* ����� �������� ������� ����� */
  
				 return(NULL) ;
			    }
/*---------------------- ���������� ���������� ���������� ���������� */

  if(type==_INTERNAL_VAR) {

      if(nInt_cnt==_TITLE_PAGE) { mError_code=_DCLE_MEMORY_VT ;     /* �������� ������������ */
					    return(NULL) ;      }

             page=      nInt_page ;                                 /* ���.����� ������� �������� */
            title=&page[nInt_cnt] ;                                 /* �������� ��������� */
                        nInt_cnt++ ;                                /* �����.������� ���������� �����.���������� */

                 memset(title, 0, sizeof(*title)) ;                 /* ������� ��������� */

			  }
/*------------------------- ���������� ���������� ������� ���������� */

  if(type==_WORK_VAR) {

     if(nWork_cnt==_TITLE_PAGE) { mError_code=_DCLE_MEMORY_VT ;     /* �������� ������������ */
					    return(NULL) ;      }

		 page=nWork_page ;                                  /* ���.����� ������� �������� */

        if(nWork_cnt==0)  nWork_cnt=1 ;                             /* '�������' ��������� �� ������� */

	                 title=&page[nWork_cnt] ;                   /* �������� ��������� */

                  memset(title, 0, sizeof(*title)) ;                /* ������� ��������� */
	                 title->name[0] =  0 ;                      /* ���. '�������' ��� */
	                 title->work_nmb=nWork_cnt  ;               /* ���.���� ������� ������ */
                               
                               nWork_mem[nWork_cnt]=NULL ;          /* ����� ��������� ����������� ������ */
                                         nWork_cnt++ ;              /* �����.������� ������� ���������� */
		      }
/*-------------------------------------------------------------------*/

   return(title) ;
}


/************************************************************************/
/*                                                                      */
/*         ���������� ���������� � ������������ ������                  */
/*                                                                      */
/*   type  -  ��� ������: _WORK_VAR - ������� ����������                */
/*                    _INTERNAL_VAR - �����.������                      */
/*                  _CLEAR_WORK_VAR - �������� ����� ������� ���������� */
/*    var  -  �������� ����������                                       */
/*   save  -  ������ ������������ ������                                */
/*                                                                      */
/*   ��� ��������� ������ ��� ������ ������� ����������(_WORK_VAR)      */
/*  � ��� ������, ���� � �������� ������� ��������� �������� addr,      */
/*  ������������ ������� ������ �� ������� ����� �� �����               */

    void *Lang_DCL::iAlloc_var(int  type, Dcl_decl *var, int save)

{
   int  var_size ;  /* ��������� ��� ������ ����� */
   int  var_type ;  /* ������� ��� ���������� */
   int  cell ;      /* ������ "�������" ������ � ������ */
  char *addr ;

/*---------------------------------- ������������ ������� ���������� */

  if(type==_CLEAR_WORK_VAR) {
				 return(NULL) ;
			    }
/*-------------------------------- ����������� ������� ������ ������ */

                              var_type=t_base(var->type) ;          /* ��������� ������� ��� ���������� */

    if(var_type==_DGT_VAL  ||                                       /* ���������� ������ "�������" ������ */
       var_type==_DGT_AREA   )    cell=iDgt_size(var->type) ;       /*   ����������                       */
    else                          cell=  1 ;

                              var_size =cell*var->buff ;
                                  save*=cell ;

/*----------------------------------- ���������� ���������� �������� */

  if(type==_INTERNAL_VAR) {

          var->addr=calloc(var_size+1, 1) ;
       if(var->addr==NULL) {  mError_code=_DCLE_MEMORY_VA ;         /* ���� ������ ��� - ���.������ */
                                      return(NULL) ;         }
		          }
/*------------------------------------ ���������� ������� ���������� */

  if(type==_WORK_VAR) {
/*- - - - - - - - - - - - - - - - - - - - - - - - - ��������� ������ */
               addr=(char *)var->addr ;                             /* ��������� ������� ������ */

          var->addr=calloc(var_size+1, 1) ;
       if(var->addr==NULL) {  mError_code=_DCLE_MEMORY_VA ;         /* ���� ������ ��� - ���.������ */
                                      return(NULL) ;         }
   
           nWork_mem[var->work_nmb]=var->addr ;                     /* ������ ��������� ����������� ������ */
/*- - - - - - - - - - - - - - - - - -  �������������� ������� ������ */
     if(addr!=NULL && save>0)  memmove(var->addr, addr, save) ;     /* �������� ������ � ����� */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
		      }
/*-------------------------------------------------------------------*/

   return(var->addr) ;
}


/************************************************************************/
/*                                                                      */
/*         ������ ������ ��������� ������� ������ �� �� ������          */
/*                                                                      */
/*    n  -  ����� ������� ������                                        */

   Dcl_decl *Lang_DCL::iGet_work(int  n)

{
  Dcl_decl *page  ;   /* ������ �������� ���������� */
  Dcl_decl *title ;   /* ������ ��������� */

/*------------------------------- 1-���������� ���������� ���������� */

		 page=nWork_page ;                                  /* ���.����� ������� �������� */
		title=    &page[n] ;                                /* �����.����� ��������� */

/*---------------------------------- �������� ������������ ��������� */

     if(title->work_nmb!=n)  mError_code=_DCLE_WORK_FAULT ;

/*-------------------------------------------------------------------*/

   return(title) ;
}


/*********************************************************************/
/*                                                                   */
/*             ���������� ���������� ���������� �������              */

typedef  struct {
		  Dcl_decl **all_vars ;              /* ����� ������� ������� ���������� */
		  Dcl_decl  *int_page ;              /* �������� ���������� ���������� ���������� */
		       int   int_cnt ;
		  Dcl_decl  *work_page ;             /* �������� ���������� ������� ���������� */
                      void **work_mem ;
		       int   work_cnt ;
		} Dcl_mstack ;

    int  Lang_DCL::iMem_stack(int  oper_code)

{
         Dcl_mstack *addr ;

  static Dcl_mstack *stack[_STACK_MAX] ;
  static        int  stack_cnt ;

/*--------------------------------------------- ������ ������ � ���� */

  if(oper_code==_DCL_PUSH) {
/*- - - - - - - - - - - - - - - - - - - - - - - - - ���������� ����� */
     if(stack_cnt==_STACK_MAX) {  mError_code=_DCLE_STACK_OVR ;     /* ���� ���� ��������... */
					      return(-1) ;       }

	   addr=(Dcl_mstack *)calloc(sizeof(*addr), 1) ;            /* �������� ������ ��� ���� */
	if(addr==NULL) {  mError_code=_DCLE_STACK_MEM ;             /* ���� ������ ���... */
				  return(-1) ;          }

	        stack[stack_cnt]=addr ;                             /* ������� ����� ������ � ���� */
		      stack_cnt++    ;
/*- - - - - - - - - - - - - - - - - - - - -  ��������� ������ � ���� */
                   addr->all_vars =nAll_vars ;
                   addr->int_page =nInt_page ;
    	           addr->int_cnt  =nInt_cnt   ;
                   addr->work_page=nWork_page ;
                   addr->work_mem =nWork_mem ;
    	           addr->work_cnt =nWork_cnt   ;
/*- - - - - - - - - - - - - - - - - - - - - - - ������������� ������ */
		         nAll_vars=NULL ;
		         nInt_page=NULL ;
		        nWork_page=NULL ;
                        nWork_mem =NULL ;
		         nInt_cnt =  0 ;
		        nWork_cnt =  0 ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
		       }
/*--------------------------------------- ���������� ������ �� ����� */

  if(oper_code==_DCL_POP) {
/*- - - - - - - - - - - - - - - - - - - - - - - - - ���������� ����� */
         if(stack_cnt<=0) {  mError_code=_DCLE_STACK_UND ;          /* ���� ���� ����... */
	                                  return(-1) ;       }

                      stack_cnt-- ;
	   addr=stack[stack_cnt] ;                                  /* ��������� ����� ������ �� ���� */
/*- - - - - - - - - - - - - - - - - - �������������� ������ �� ����� */
	      nAll_vars=addr->all_vars ;
	      nInt_page=addr->int_page ;
	      nInt_cnt =addr->int_cnt  ;
             nWork_page=addr->work_page ; 
             nWork_mem =addr->work_mem ;
             nWork_cnt =addr->work_cnt ;
/*- - - - - - - - - - - - - - - - - - - - - - -  ������������ ������ */
			     free(addr) ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
		      }
/*-------------------------------------------------------------------*/

  return(0) ;
}
