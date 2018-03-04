/*   Bolotov P.D.  version  29.04.1994   */
/*     0 warnings at warning level 1     */

/*********************************************************************/
/*                                                                   */
/*                   DATA CONVERTION LANGUAGE                        */
/*                                                                   */
/*                    ������� ������ ����������                      */
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
#include  <ctype.h>
#include  <math.h>
#include  <memory.h>
#include  <time.h>
#include  <sys/timeb.h>

#ifdef UNIX
#include  <unistd.h>
#else
#include <process.h>
#endif


#define DCL_INSIDE

#include "dcl.h"


#pragma warning(disable : 4996)

/*--------------------------------------------- ����������� �������� */

  union Dcl_v_conversion {
				  double  v_double ;
				   float  v_float ;
				    long  v_long ;
			   unsigned long  v_ulong ;
				   short  v_short ;
			  unsigned short  v_ushort ;
			 } ;

/*********************************************************************/
/*                                                                   */
/*           ����� ��������� ������� �� ����������� � ������         */
/*                � �������������� ���������� ��������               */
/*                                                                   */
/*   text  -  ������������� ������                                   */
/*   end   -  �������� ������ ���                                    */
/*             _ANY_OPER        -  ����� ��������                    */
/*             _ANY_OPER_OR_END - ����� �������� ��� ������� ������  */
/*             _ANY_CLOSE       - ����� ����������� ������           */

  char *Lang_DCL::iFind_close(char *text, int  end)

{
  int  string_flag ;   /* ���� ���������� ������ ���������� ������ */
  int  n1 ;            /* ������� ����������� ������� ������ */
  int  n2 ;            /* ������� ����������� ���������� ������ */


	   string_flag=0 ;
		    n1=0 ;
		    n2=0 ;

  for( ; *text ; text++) {

/*------------------------------------- ����������� ���������� ����� */

    if(*text=='"')  if(string_flag) {                               /* ������������� � ��������� */
		     if(*(text-1)!='\\' ||                          /*   �������� � ��������     */
			*(text-2)=='\\'   )  string_flag=0 ;        /*    ���������� ������      */
				    }
		    else                     string_flag=1 ;

    if(string_flag)  continue ;                                     /* ������ ����.������ */

/*--------------------------------------------------- �������� ����� */

	 if(*text=='?')  text+=1 ;                                  /* �����.������ ���������� */
    else if((end ==_ANY_OPER ||                                     /* ����� ������ �������� */
	     end ==_ANY_OPER_OR_END) &&
	      OPER_CHAR(*text)         ) {
					   if(!n1 && !n2)  break ;
					 }
    else if(*text==end) {                                           /* ����� ����-������� */
			  if(!n1 && !n2)  break ;
			  if(end == ')')   n1-- ;
			}
    else if(*text==')') {                                           /* �����.�������� ������ */
	    if(end==_ANY_CLOSE)  break ;
	    else                  n1-- ;
			}
    else if(*text=='(')   n1++ ;                                    /* �����.�������� ������ */
    else if(*text==']')   n2-- ;                                    /* �����.�������� ������ */
    else if(*text=='[')   n2++ ;                                    /* �����.�������� ������ */

/*-------------------------------------------------------------------*/

			 }

       if(end==_ANY_OPER       ) {  if(*text== 0 )  text=NULL ;  }  /* �������� �����������     */
  else if(end==_ANY_OPER_OR_END) {                  text-- ;     }  /*   ����, ��� ���� �       */
  else if(end==_ANY_CLOSE      ) {  if(*text!=')')  text=NULL ;  }  /*    ����������� ��������� */
  else                           {
				     if(*text!=end)  text=NULL ;
				     if(  n1 || n2)  text=NULL ; //??????
				 }


 return(text) ;
}


/*********************************************************************/
/*                                                                   */
/*          ����� ���������� ������� �� ����������� � ������         */
/*                � �������������� ���������� ��������               */
/*                                                                   */
/*   text   -  ������������� ������                                  */
/*   start  -  ����� �����                                           */
/*   end    -  �������� ������ ���                                   */
/*              _ANY_OPER        -  ����� ��������                   */
/*              _ANY_OPER_OR_END - ����� �������� ��� ������� ������ */

  char *Lang_DCL::iFind_open(char *text, char *start, int  end)

{
 char *tmp ;           /* ������� ��������� */
  int  string_flag ;   /* ���� ���������� ������ ���������� ������ */
  int  n1 ;            /* ������� ����������� ������� ������ */
  int  n2 ;            /* ������� ����������� ���������� ������ */


	   string_flag=0 ;
		    n1=0 ;
		    n2=0 ;

  for(tmp=start ; tmp>=text ; tmp--) {

/*------------------------------------- ����������� ���������� ����� */

    if(*tmp=='"')  if(string_flag) {                                /* ������������� � ��������� */
		     if(*(tmp-1)!='\\' ||                           /*   �������� � ��������     */
			*(tmp-2)=='\\'   ) string_flag=0 ;          /*    ���������� ������      */
				   }
		   else                 string_flag=1 ;

    if(string_flag)  continue ;                                     /* ������ ����.������ */

/*--------------------------------------------------- �������� ����� */

	 if( tmp[-1]=='?')  tmp-=1 ;                                /* �����.������ ���������� */
    else if((end ==_ANY_OPER ||                                     /* ����� ������ �������� */
	     end ==_ANY_OPER_OR_END) &&
	      OPER_CHAR(*tmp)          ) {
					   if(!n1 && !n2)  break ;
					 }
    else if(*tmp==end) {                                            /* ����� ����-������� */
			  if(!n1 && !n2)  break ;
			  if(end == '(')   n1-- ;
		       }
    else if(*tmp==')')   n1++ ;                                     /* �����.�������� ������ */
    else if(*tmp=='(')   n1-- ;                                     /* �����.�������� ������ */
    else if(*tmp==']')   n2++ ;                                     /* �����.�������� ������ */
    else if(*tmp=='[')   n2-- ;                                     /* �����.�������� ������ */

/*-------------------------------------------------------------------*/

				     }

  if(end==_ANY_OPER_OR_END) {                   tmp++ ;     }
  else                         if(tmp <text  )  tmp=NULL ;

 return(tmp) ;
}


/*********************************************************************/
/*                                                                   */
/*          ����������� ����� ��������� ��������                     */
/*                                                                   */
/*   type  -  ��� ������                                             */

   int  Lang_DCL::iDgt_size(int  type)

{
    int  size ;

	   type=t_mode(type) ;                                      /* ��������� �����.���� */

                                size=  0 ;
	if(type==_DCLT_DOUBLE)  size=sizeof(double) ;               /* �������������� ������         */
   else if(type==_DCLT_FLOAT )  size=sizeof(float) ;                /*  � ��������� ������� - DOUBLE */
   else if(type==_DCLT_LONG  )  size=sizeof(long) ;
   else if(type==_DCLT_ULONG )  size=sizeof(long);
   else if(type==_DCLT_SHORT )  size=sizeof(short);
   else if(type==_DCLT_USHORT)  size=sizeof(short) ;

 return(size) ;
}


/*********************************************************************/
/*                                                                   */
/*                ���������� ��������� ��������                      */
/*                                                                   */
/*   addr  -  ����� ������                                           */
/*   type  -  ��� ������                                             */

   double  Lang_DCL::iDgt_get(void *addr, int  type)

{
  union Dcl_v_conversion  conv ;   /* ����� ������������-�������������� */
		  double  value ;  /* '��������' �������� */


	       if(addr==NULL)  return(0.) ;                         /* �������� ������ ������ */

	     memcpy(&conv, addr, sizeof(conv)) ;                    /* �������� ������ � ����� �������������� */

	   type=t_mode(type) ;                                      /* ��������� �����.���� */

                                value=  0. ;
	if(type==_DCLT_DOUBLE)  value=conv.v_double ;               /* �������������� ������         */
   else if(type==_DCLT_FLOAT )  value=conv.v_float ;                /*  � ��������� ������� - DOUBLE */
   else if(type==_DCLT_LONG  )  value=conv.v_long ;
   else if(type==_DCLT_ULONG )  value=conv.v_ulong ;
   else if(type==_DCLT_SHORT )  value=conv.v_short ;
   else if(type==_DCLT_USHORT)  value=conv.v_ushort ;

 return(value) ;
}


/*********************************************************************/
/*                                                                   */
/*                ��������� ��������� ��������                       */
/*                                                                   */
/*   value  -  ��������                                              */
/*   addr   -  ����� ������                                          */
/*   type   -  ��� ������                                            */

   double  Lang_DCL::iDgt_set(double  value, void *addr, int  type)

{
  union Dcl_v_conversion  conv ;   /* ����� ������������-�������������� */


	       if(addr==NULL)  return(0.) ;                         /* �������� ������ ������ */

	   type=t_mode(type) ;                                      /* ��������� �����.���� */

	if(type==_DCLT_DOUBLE)  conv.v_double=value ;               /* �������������� ������ �� ��������� ������� - DOUBLE */
   else if(type==_DCLT_FLOAT )  conv.v_float =(         float)value ; 
   else if(type==_DCLT_LONG  )  conv.v_long  =(          long)value ;
   else if(type==_DCLT_ULONG )  conv.v_ulong =(unsigned  long)value ;
   else if(type==_DCLT_SHORT )  conv.v_short =(         short)value ;
   else if(type==_DCLT_USHORT)  conv.v_ushort=(unsigned short)value ;

	     memcpy(addr, &conv, iDgt_size(type)) ;                 /* �������� ������ �� ����� ������. */

 return(value) ;
}


/*********************************************************************/
/*                                                                   */
/*         �������� ��������� �������� ��������                      */
/*                                                                   */
/*   base_var  -  ������� ���������� ���������                       */
/*                                                                   */
/*  ����� ���������� ���� ��, ������� ��������� �� ������� ������    */
/*  ��� ������ ��� � ������������ ������ ������� � ������������      */  
/*  � �������.                                                       */

   void  Lang_DCL::iSize_correct(Dcl_decl *base_var)

{
            Dcl_decl *vars ;       /* ������� ������ �������� ���������� � �������� */
  Dcl_complex_record *record ;
                 int  type ;       /* �������� ��� ���������� */
                 int  i ;
                 int  j ;
                 int  k ;

/*------------------------------------------------------- ���������� */

	type=t_base(base_var->type) ;
     if(type!=_CHR_AREA &&                                          /* �������������� ������ */
	type!=_CHR_PTR    )  return ;                               /*    �������� �������   */

/*------------------------------------ ������ �� �������� ���������� */

  for(i=0 ; nVars[i]!=NULL ; i++) {

     if(nVars[i]->addr==NULL)  continue ;                           /* �� ������� "���������" ������ �������� */

   for(vars=nVars[i] ; vars->name[0]!=0 ; vars++)
     if(vars->func_flag==0) {

          type=t_base(vars->type) ;
/*- - - - - - - - - - - - - - - - - - - - - - - - ����������� ������ */
       if(type==_XTP_OBJ) {

         if((vars->work_nmb & _DCL_XTRACE)!=_DCL_XTRACE) continue ; /* ������ ���� ������������ �� ������� ��������� */

          for(record=(Dcl_complex_record *)vars->addr,
                             j=0 ; j<vars->buff ; j++, 
              record=(Dcl_complex_record *)record->next_record)
         for(k=0 ; k<record->elems_cnt ; k++) {
              type=t_base(record->elems[k].type) ;
           if(type==_CHR_AREA ||
              type==_CHR_PTR    )  iSize_correct(base_var, &record->elems[k]) ;
                                              }

                                     continue ;
                          }
/*- - - - - - - - - - - - - - - - - - - - - - - - ������� ���������� */
       else
       if(type==_CHR_AREA ||
          type==_CHR_PTR    )  iSize_correct(base_var, vars) ;

                            }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                                  }
/*--------------------------------- ������ � ����������� ����������� */

   for(vars=nInt_page ; vars->name[0]!=0 ; vars++) {

          type=t_base(vars->type) ;
/*- - - - - - - - - - - - - - - - - - - - - - - - ����������� ������ */
       if(type==_XTP_OBJ) {

          for(record=(Dcl_complex_record *)vars->addr,
                             j=0 ; j<vars->buff ; j++, 
              record=(Dcl_complex_record *)record->next_record)
         for(k=0 ; k<record->elems_cnt ; k++) {
              type=t_base(record->elems[k].type) ;
           if(type==_CHR_AREA ||
              type==_CHR_PTR    )  iSize_correct(base_var, &record->elems[k]) ;
                                              }

                                     continue ;
                          }
/*- - - - - - - - - - - - - - - - - - - - - - - - ������� ���������� */
       else
       if(type==_CHR_AREA ||
          type==_CHR_PTR    )  iSize_correct(base_var, vars) ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                                                   }
/*------------------------------------- ������ � ���������� �������� */

  for(i=0 ; i<nTransit_cnt ; i++) {

           vars=&nTransit[i] ;
           type=t_base(vars->type) ;
/*- - - - - - - - - - - - - - - - - - - - - - - - ����������� ������ */
       if(type==_XTP_OBJ) {

          for(record=(Dcl_complex_record *)vars->addr,
                             j=0 ; j<vars->buff ; j++, 
              record=(Dcl_complex_record *)record->next_record)
         for(k=0 ; k<record->elems_cnt ; k++) {
              type=t_base(record->elems[k].type) ;
           if(type==_CHR_AREA ||
              type==_CHR_PTR    )  iSize_correct(base_var, &record->elems[k]) ;
                                              }

                                     continue ;
                          }
 /*- - - - - - - - - - - - - - - - - - - - - - - - ������� ���������� */
        else
	if(type==_CHR_AREA ||
	   type==_CHR_PTR    )  iSize_correct(base_var, vars) ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                                  }
/*-------------------------------------------------------------------*/
}


   void  Lang_DCL::iSize_correct(Dcl_decl *base, Dcl_decl *dest)

{
   char *addr_base ;  /* ������� ����� */
   char *addr_end ;   /* ����� ����� ������ ������� */
    int  size_base ;  /* ������� ������ */
    int  buff_base ;  /* ������� ����� */

/*------------------------------------------------------- ���������� */

	  addr_base=(char *)base->addr ;
	  size_base=        base->size ;
	  buff_base=        base->buff ;
	  addr_end =addr_base+buff_base ;

/*------------------------------------------- "���������" ���������� */

  if((char *)dest->addr+dest->buff==addr_end ||
          (  dest->addr>=addr_base &&
             dest->addr< addr_end    )         ) {

         if(dest->buff<0) {
                             mError_code=_DCLE_READONLY ;
                                return ;
                          }

	    dest->size=addr_base-(char *)dest->addr+size_base ;
	 if(dest->size<   0      )  dest->size=    0 ;
	 if(dest->size>dest->buff)  dest->size=dest->buff ;
						 }
/*-------------------------------- "�������" ������������ ���������� */

  else
  if(strcmp(base->name, dest->name)==0 &&
            base->prototype   !=NULL   &&
            base->prototype[0]=='D'    &&
            dest->prototype   !=NULL   &&
            dest->prototype[0]=='D'      ) {

         if(dest->buff<0) {
                             mError_code=_DCLE_READONLY ;
                                return ;
                          }
     
               dest->addr=base->addr ;
               dest->size=base->size ;
               dest->buff=base->buff ;
                                           }
/*-------------------------------------------------------------------*/
}


/*********************************************************************/
/*                                                                   */
/*   �������� ��������� �������� �������� ��� �������� INCLUDE       */
/*                                                                   */
/*    base_var  -  ������� ���������� ���������                      */
/*    begin     -  ���� ������ INCLUDE-�������                       */
/*    end       -  ����� ����� INCLUDE-�������                       */
/*    shift     -  ������������ ����� '������'                       */

   void  Lang_DCL::iIncl_correct(Dcl_decl *base_var, 
                                     char *begin, 
                                     char *end, 
                                      int  shift)

{
  Dcl_decl *vars ;       /* ������� ������ �������� ���������� � �������� */
      char *addr_base ;  /* ������� ����� */
      char *addr_end ;   /* ����� ����� ������ ������� */
       int  size_base ;  /* ������� ������ */
       int  buff_base ;  /* ������� ������ */
       int  i ;


     if(base_var->type!=_CHR_AREA &&                                /* �������������� ������ */
	base_var->type!=_CHR_PTR    )  return ;                     /*    �������� �������   */

     if(shift==0)  return ;                                         /* ���� ������ ������ �� ���� ... */

	  addr_base=(char *)base_var->addr ;
	  size_base=        base_var->size ;
	  buff_base=        base_var->buff ;
	  addr_end =addr_base+buff_base ;

  for(i=0 ; nVars[i]!=NULL ; i++) {                                 

     if(nVars[i]->addr==NULL)  continue ;                           /* �� ������� "���������" ������ �������� */

   for(vars=nVars[i] ; vars->name!=NULL ; vars++)                   /* ����� ���������� ���� ��, �������            */
     if( vars->func_flag==    0         &&                          /*   ��������� �� ������� ������ ��� ������ ��� */
	(vars->type     ==_CHR_AREA ||                              /*  (����� ������� �� ���������� ��������       */
	 vars->type     ==_CHR_PTR    )   )                         /*    ������� ������� �������� � �������        */
       if((char *)vars->addr+vars->buff==addr_end) {                /*       � �������� �������)                    */

/*--------------------------------------- ��� ������ ������ '������' */

		 if(vars->addr<=begin) {                            /* ���� ����� ������� �� INCLUDE-�������... */
					 vars->size+=shift ;        /*   ������������ ������ ������� */
			if(vars->size>                              /*   �������� ������������      */
			   vars->buff )  vars->size =vars->buff ;   /*     �������� �������� ������ */
				       }
	    else if(vars->addr<=end  ) {                            /* ���� ����� ������� ������ INCLUDE-�������... */
					 vars->size+=shift ;        /*   ������������ ������ ������� */
		       if(vars->size<0)  vars->size =  0  ;         /*   �������� �� '�������' ������� */
		       if(vars->size>                               /*   �������� ������������      */
			  vars->buff  )  vars->size =vars->buff ;   /*     �������� �������� ������ */
				       }                            /* ���� ����� ������� ����� INCLUDE-�������... */
	    else                       {
                           vars->addr =(char *)vars->addr+shift ;   /* �������.����� ������� � ������ ��� ������  */
                           vars->buff-=shift ;
				       }
/*-------------------------------------------------------------------*/
						   }
                                  }  
}


/*********************************************************************/
/*                                                                   */
/*               �������������� ������������� �������                */
/*                                                                   */
/*    base_var  -  ������� ���������� ���������                      */
/*    work_var  -  ������� ���������� ���������                      */
/*    size      -  ����� ������ ������������� �������                */

   void  Lang_DCL::iBuff_realloc(Dcl_decl *base_var, 
                                 Dcl_decl *work_var, 
                                   size_t  size)

{
#if 0
  Dcl_decl *vars ;       /* ������� ������ �������� ���������� � �������� */
      char *addr_base ;  /* ������� ����� */
      char *addr_end ;   /* ����� ����� ������ ������� */
       int  size_base ;  /* ������� ������ */
       int  buff_base ;  /* ������� ������ */
       int  i ;
#endif

     if(base_var->type!=_CHR_AREA &&                                /* �������������� ������ */
	base_var->type!=_CHR_PTR    )  return ;                     /*    �������� �������   */

                    size= size+_DYN_CHUNK ;
          base_var->addr=realloc(base_var->addr, size) ;
          base_var->buff= size ;

     if(work_var!=NULL) {
          work_var->addr=base_var->addr ;
          work_var->buff=base_var->buff ;
                        }

#if 0
	  addr_base=(char *)base_var->addr ;
	  size_base=        base_var->size ;
	  buff_base=        base_var->buff ;
	  addr_end =addr_base+buff_base ;

  for(i=0 ; nVars[i]!=NULL ; i++) {                                 

     if(nVars[i]->addr==NULL)  continue ;                           /* �� ������� "���������" ������ �������� */

   for(vars=nVars[i] ; vars->name!=NULL ; vars++)                   /* ����� ���������� ���� ��, �������            */
     if( vars->func_flag==    0         &&                          /*   ��������� �� ������� ������ ��� ������ ��� */
	(vars->type     ==_CHR_AREA ||                              /*  (����� ������� �� ���������� ��������       */
	 vars->type     ==_CHR_PTR    )   )                         /*    ������� ������� �������� � �������        */
       if((char *)vars->addr+vars->buff==addr_end) {                /*       � �������� �������)                    */

/*--------------------------------------- ��� ������ ������ '������' */

		 if(vars->addr<=begin) {                            /* ���� ����� ������� �� INCLUDE-�������... */
					 vars->size+=shift ;        /*   ������������ ������ ������� */
			if(vars->size>                              /*   �������� ������������      */
			   vars->buff )  vars->size =vars->buff ;   /*     �������� �������� ������ */
				       }
	    else if(vars->addr<=end  ) {                            /* ���� ����� ������� ������ INCLUDE-�������... */
					 vars->size+=shift ;        /*   ������������ ������ ������� */
		       if(vars->size<0)  vars->size =  0  ;         /*   �������� �� '�������' ������� */
		       if(vars->size>                               /*   �������� ������������      */
			  vars->buff  )  vars->size =vars->buff ;   /*     �������� �������� ������ */
				       }                            /* ���� ����� ������� ����� INCLUDE-�������... */
	    else                       {
                           vars->addr =(char *)vars->addr+shift ;   /* �������.����� ������� � ������ ��� ������  */
                           vars->buff-=shift ;
				       }
/*-------------------------------------------------------------------*/
						   }
                                  }  
#endif
}


/*********************************************************************/
/*                                                                   */
/*           ������� ���������� ������ � �����.                      */
/*                                                                   */
/*   text  -  �������������� ��������                                */
/*   end   -  ��������� ������������ ������ ���������                */

  double  Lang_DCL::iStrToNum(char *text, char **end)

{
   char *addr ;
 double  value ;


	    addr=strchr(text, '.') ;                                /* ���������� ������� �������������� */
								    /*      ���������� �����             */
	 if(addr   !=NULL  )  value=strtod(text, end) ;             /* �������������� ��� ��������� */
    else if(text[0]=='0' &&
	    text[1]=='x'   )  value=strtol(text+2, end, 16) ;       /* �������. ��� 16-������ ����� */
    else if(text[0]=='0' &&
	    text[1]=='o'   )  value=strtol(text+2, end,  8) ;       /* �������. ���  8-������ ����� */
    else                      value=strtol(text  , end, 10) ;       /* �������. ��� 10-������ ����� */

  return(value) ;
}


/*********************************************************************/
/*                                                                   */
/*              ������� ����� � ���������� ������                    */
/*                                                                   */
/*   value -  �������� ������                                        */
/*   text  -  �������� �����                                         */

  void   Lang_DCL::iNumToStr(Dcl_decl *value, char *text)

{
	     int  type_base ;
	     int  type_mode ;
	    char *tmp ;

	    long *a_long ;
  unsigned  long *a_longu ;
	   short *a_short ;
  unsigned short *a_shortu ;
	   float *a_float ;
	  double *a_double ;

/*------------------------------------------------- ������� �������� */

  if(value->addr==NULL) {
                            strcpy(text, "NULL") ;
                                   return ;
                        }
/*----------------------------------------------- ���������� ������� */

          type_base=t_base(value->type) ;
          type_mode=t_mode(value->type) ;

	   a_long  =(          long *)value->addr ;
	   a_longu =(unsigned  long *)value->addr ;
	   a_short =(         short *)value->addr ;
	   a_shortu=(unsigned short *)value->addr ;
	   a_float =(         float *)value->addr ;
	   a_double=(        double *)value->addr ;

/*----------------------------------- �������������� ��������� ����� */

  if(type_base==_DGT_VAL) {
/*- - - - - - - - - - - - - - - - - - - - - - ������� �������������� */
         if(type_mode==_DCLT_SHORT )  sprintf(text, "%hd",    *a_short) ;
    else if(type_mode==_DCLT_USHORT)  sprintf(text, "%hu",    *a_shortu) ;
    else if(type_mode==_DCLT_LONG  )  sprintf(text, "%ld",    *a_long) ;
    else if(type_mode==_DCLT_ULONG )  sprintf(text, "%lu",    *a_longu) ;
    else if(type_mode==_DCLT_FLOAT )  sprintf(text, "%.10f",  *a_float) ;
    else if(type_mode==_DCLT_DOUBLE)  sprintf(text, "%.10lf", *a_double) ;
/*- - - - - - - - - - - - - - - - - - - ������ ������� ������� ����� */
         if(type_mode==_DCLT_FLOAT  ||
            type_mode==_DCLT_DOUBLE   ) {

              for(tmp=text+strlen(text)-1 ; *tmp=='0' ; tmp--) *tmp=0 ;

                if(*tmp=='.')  *tmp=0 ;
                                        }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                          } 
/*-------------------------------------------------------------------*/

  return ;
}


/*********************************************************************/
/*                                                                   */
/*              ����� ���������� � ���������� ���                    */
/*                                                                   */
/*   text  -  �������� �����                                         */

   void  Lang_DCL::iLog(char *text_1, char *text_2)

{
#ifdef UNIX
      struct timeb  time_ms ;
#else
  struct __timeb32  time_ms ;
#endif
              char  prefix[128] ;
              FILE *file ;

/*------------------------------------------------- ������� �������� */

    if(mDebug_path[0]==0)  return ;

/*-------------------------------------- ����������� ��������� ����� */

#ifdef UNIX
      ftime(&time_ms) ; 
    sprintf( prefix, "%d.%hu> ", time_ms.time, time_ms.millitm) ;
#else
   _ftime32(&time_ms) ; 
    sprintf( prefix, "%d.%hu> ", time_ms.time, time_ms.millitm) ;
#endif

/*------------------------------------------------- ������ ���-����� */

      file=fopen(mDebug_path, "at") ;                               /* �������� ����� ���� */
   if(file==NULL)  return ;

                     fwrite(prefix, 1, strlen(prefix), file) ;
   if(text_1!=NULL)  fwrite(text_1, 1, strlen(text_1), file) ;
   if(text_2!=NULL)  fwrite(text_2, 1, strlen(text_2), file) ;
                     fwrite("\n",   1, strlen( "\n" ), file) ;

         fclose(file) ;	                                            /* �������� ����� ���� */

/*-------------------------------------------------------------------*/

  return ;
}
