/*   Bolotov P.D.  version  29.04.1994   */
/*     0 warnings at warning level 1     */

/*********************************************************************/
/*                                                                   */
/*                   DATA CONVERTION LANGUAGE                        */
/*                                                                   */
/*                    ������ � ���������� ��������                   */
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

#ifdef UNIX
#define   stricmp  strcasecmp
#endif


#define DCL_INSIDE

#include "dcl.h"


#pragma warning(disable : 4996)

/*----------------------------------- �������� ����� � ������������� */

typedef struct {
		   char *mnemo ;             /* ��������� �������� */
		    int  mnemo_size ;        /* ������ ��������� �������� */
		    int  base ;              /* ��� �������� ����(� �� ���������): */
#define                   _NO_DEF       -1   /*   ��������� �����������            */
		    int  mode ;              /* ��� ������������ �������� ����: */
#define                       _SHORT     0   /*   ��������                      */
#define                        _LONG     1   /*   �������                       */
#define                      _SIGNED     0   /*   ��������                      */
#define                    _UNSIGNED     2   /*   �����������                   */
		    int  pref_type ;         /* ��� ��������:                */
#define                       _SZ_PRFX   1   /*   ������� �������            */
#define                       _US_PRFX   2   /*   ������� �����              */
#define                       _PR_PRFX   4   /*   ������� ���������          */
#define                       _EX_PRFX   8   /*   ������� ������� ���������� */
#define                       _PB_PRFX  16   /*   ������� ���������� ���������� */
		    int  pref_mask ;         /* ����� ������������ ���������: */
//#define                     _SZ_PRFX  1    /*   ������� �������             */
//#define                     _US_PRFX  2    /*   ������� �����               */
//#define                     _PR_PRFX  4    /*   ������� ���������           */
//#define                     _EX_PRFX  8    /*   ������� ������� ���������� */
	       } Dcl_type ;

			 /* ������� ����:                 */
#define     _CHAR    1   /*  ���������� ������ ��� ������ */
#define  _INTEGER    2   /*  ����� �����                  */
#define    _FLOAT    3   /*  ����� � ��������� ������     */
#define  _TYPEDEF    4   /*  ����������� ���              */

#define  _DECL_CNT  11   /* ����� ��������� ������������� */
#define  _DECL_BASE  1   /* ������ ������ ������� ������������� */
 static Dcl_type  dcl_type[]={    /* �������� ������������� */
  { "<dummy>",   7, _TYPEDEF,       0,         0, _PB_PRFX | _PR_PRFX | _EX_PRFX   },

  { "char",      4,    _CHAR,       0,         0,  _PB_PRFX | _PR_PRFX | _EX_PRFX  },
  { "int",       3, _INTEGER,       0,         0,  _PB_PRFX | _PR_PRFX | _EX_PRFX |
						   _SZ_PRFX | _US_PRFX             },
  { "short",     5, _INTEGER,    _SHORT, _SZ_PRFX, _PB_PRFX | _PR_PRFX | _EX_PRFX |
						   _US_PRFX                        },
  { "long",      4, _INTEGER,     _LONG, _SZ_PRFX, _PB_PRFX | _PR_PRFX | _EX_PRFX |
						   _US_PRFX                        },
  { "float",     5,   _FLOAT,       0,         0,  _PB_PRFX | _PR_PRFX | _EX_PRFX |
						   _SZ_PRFX                        },
  { "double",    6,   _FLOAT,     _LONG, _SZ_PRFX, _PB_PRFX | _PR_PRFX | _EX_PRFX  },
  { "unsigned",  8,  _NO_DEF, _UNSIGNED, _US_PRFX, _PB_PRFX | _PR_PRFX | _EX_PRFX  },

  { "public",    6,  _NO_DEF,       0,   _PB_PRFX,       0                         },
  { "parameter", 9,  _NO_DEF,       0,   _PR_PRFX,       0                         },
  { "extern",    6,  _NO_DEF,       0,   _EX_PRFX,       0                         }
                             } ;

 static              int  dcl_x_redef ;
 static         Dcl_decl *dcl_x_elem ;

 static              int  dcl_typedef ;    /* ���� ���������� � ����� typedef */

#define  CURR_X_TYPE  nX_types[nX_cnt]

/*--------------------------------------------- ��������� ���������� */


/*----------------------------------------------- ������� ���������� */


/************************************************************************/
/*                                                                      */
/*                   ����������� �������� ����������                    */
/*                                                                      */
/*    text  -  ������������� �����                                      */
/*                                                                      */
/*   Return:    1 - ��� ������� ��������                                */
/*              0 - ��� ���������� ��������                             */

    int  Lang_DCL::iDecl_detect(char *text)


{
  int  i ;


     if(dcl_typedef)  return(1) ;

     if(!memcmp(text, "typedef", strlen("typedef")))  return(1) ;

     if(text[0]=='<')  return(1) ;

    for(i=0 ; i<_DECL_CNT ; i++)
     if(!memcmp(text, dcl_type[i].mnemo,
		      dcl_type[i].mnemo_size))  return(1) ;

   return(0) ;
}


/*********************************************************************/
/*                                                                   */
/*               ��������� �������� ����������                       */
/*                                                                   */
/*    text      -  �������������� �����                              */
/*    ext_proc  -  �������� ���������� ��� ������� ���������         */
/*                                                                   */
/*  ��������� �������� �������� ���� ����������:                     */
/*       [parameter] [us_prefix] [sz_prefix] [type]                  */
/*                                                                   */
/*    ���  parameter - ������� ��������� ;                           */
/*         us_prefix - ������� �����: unsigned ;                     */
/*         sz_prefix - ������� �������: short, long, double ;        */
/*              type - ������� ���: char, int, float, block.         */
/*    � �������� ������ �������������� ����-�� ���� ��               */
/*    ������������� type ��� sz_prefix. ���� ������������ ������     */
/*    ����������� sz_prefix, �� ������� ��� ��������������� �        */
/*    ������������ �� ���������� �����������:  short -> int          */
/*                                              long -> int          */
/*                                            double -> float        */
/*                                                                   */
/*      ���� ���������� ����� ����������� PARAMETER, �� �������      */
/*    ���������� ���������� ��� ��������� - ��� ���������            */
/*    ���������� ���������� ��������.                                */
/*                                                                   */
/*      ���� EXT_PROC!=NULL, �� ����������� ���������� ��            */
/*    � �� ������ �������� � ��������� EXT_PROC.                     */

    int  Lang_DCL::iDecl_maker(char *text, Dcl_decl *ext_proc)

{
   Dcl_decl *var ;         /* ��������� ��������� ���������� */
       char *name ;        /* ��� ���������� */
        int  buff_new ;    /* ������ ������ ������ */
        int  size ;        /* ������ '������' ������� */
        int  base_type ;   /* ������� ��� */
        int  mode ;        /* ����������� �������� ���� */
        int  type ;        /* ��� ���������� */
        int  pref_type ;   /* ��� �������� */
        int  par_flag ;    /* ���� ����������-��������� */
        int  ext_flag ;    /* ���� ������� ���������� */
        int  pub_flag ;    /* ���� ���������� ���������� */
        int  ptr_flag ;    /* ���� �������� ��������� */
        int  area_flag ;   /* ���� �������� ������� */
        int  offset   ;    /* ���� �������� ������� */
       char *addr ;
       char *end ;
       char *tmp ;
        int  i ;

/*--------------------------------- ������� �������� � ������������� */

/*- - - - - - - - - - - - -  ����������� ��������� ����������� ����� */
   if(nX_types==NULL) {

              nX_types=(Dcl_complex_type *)
                         realloc(nX_types,
                                 (nX_cnt+1)*sizeof(*nX_types)) ;

           strcpy(nX_types[nX_cnt].name, "<unknown>") ;
                  nX_types[nX_cnt].list    =NULL ;
                  nX_types[nX_cnt].list_cnt=  0 ;
                  nX_types[nX_cnt].size    =  0 ;
                           nX_cnt++ ;
                      }
/*------------------------------------------- �������� TYPEDEF-����� */

     if(dcl_typedef && text[0]=='}') {                              /* �������� TYPEDEF-����� */
                                         dcl_typedef=0 ;

        if(dcl_x_redef<0) {  nX_cnt++ ;                             /* ���� ��������� �������� ���� */
                              return(0) ;   }
 
           if(        nX_types[dcl_x_redef].list_cnt!=              /* �������� ��������������� ���� */
                      nX_types[   nX_cnt  ].list_cnt   ||
               memcmp(nX_types[dcl_x_redef].list, 
                      nX_types[   nX_cnt  ].list,
                         sizeof(nX_types->list[0])*
                                nX_types[nX_cnt].list_cnt)  ) {
                                      mError_code=_DCLE_TYPEDEF_RDF ;
                                                return(0) ;           
                                                              }  

                                           return(0) ;           
                                     } 
/*-------------------------------- ���������� �������� TYPEDEF-����� */

     if(dcl_typedef) {                                              /* �������� ��������� ������             */
          CURR_X_TYPE.list_cnt++ ;                                  /*  � ������ ��������� ������������ ���� */
          CURR_X_TYPE.list=(Dcl_decl *)
                            realloc(CURR_X_TYPE.list, CURR_X_TYPE.list_cnt*sizeof(Dcl_decl)) ;
                dcl_x_elem=&CURR_X_TYPE.list[CURR_X_TYPE.list_cnt-1] ;
                     }
/*------------------------------------------- �������� TYPEDEF-����� */

     if(!memcmp(text, "typedef", strlen("typedef"))) {

           if(dcl_typedef) {                                        /* ��������� �� �����������... */
                              mError_code=_DCLE_TYPEDEF_IN ;
                                    return(0) ;           
                           }
/*- - - - - - - - - - - - - - - - - - - - - - - - -  ������ �������� */
                 name=text+strlen("typedef") ;
                  end=strchr(name, '{') ;
               if(end==NULL) {
                                mError_code=_DCLE_TYPEDEF_STR ;
                                      return(0) ;           
                             }

                 *end=0 ;   
/*- - - - - - - - - - - - - - - - - - �������� ���������� ����� ���� */
         if(name[        0     ]!='<' ||
            name[strlen(name)-1]!='>'   ) {
                                mError_code=_DCLE_TYPEDEF_NAME ;
                                      return(0) ;           
                                          }
/*- - - - - - - - - - - - - - - - - - - - - - �������� ���� �� ����� */
                                                     dcl_x_redef=-1 ;

          for(i=0 ; i<nX_cnt ; i++)
            if(!stricmp(nX_types[i].name, name))  dcl_x_redef= i ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - ����������� ���� */
              nX_types=(Dcl_complex_type *)
                         realloc(nX_types,
                                 (nX_cnt+1)*sizeof(*nX_types)) ;

          strncpy(nX_types[nX_cnt].name, name, sizeof(nX_types->name)-1) ;
                  nX_types[nX_cnt].list    =NULL ;
                  nX_types[nX_cnt].list_cnt=  0 ;
                  nX_types[nX_cnt].size    =  0 ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                                    dcl_typedef=1 ;
                                       return(0) ;           

                                                     }
/*---------------------------- ����������� � ��������� �������� ���� */

		    base_type=_NO_DEF ;                             /* ����� �������� ���� */
		    pref_type=   0 ;                                /* ����� ���� �������� */
			 mode=_SHORT ;                              /* ����� ������������ ���� */

  while(1) {                                                        /* CIRCLE.1 - ��������� �������� */
								    /*                   ����� ...   */
/*- - - - - - - - - - - - - - - - -  ������������� ������������ ���� */
     if(*text=='<') {

        for(i=0 ; i<nX_cnt ; i++)                                   /* ������������� ������������ ���� */
          if(!memcmp(text, nX_types[i].name,
                       strlen(nX_types[i].name)))  break ;

          if(i>=nX_cnt) {                                           /* ��� ����� ��� ����������... */
                           mError_code=_DCLE_NO_TYPE ;
                                   return(0) ;           
                        }

             dcl_type[0].mode      =   i ;                          /* ��������� ���� ������������ ���� */ 
             dcl_type[0].mnemo_size=strlen(nX_types[i].name) ;
                         mode      =   i ;
                           i       =   0 ;
                    }   
/*- - - - - - - - - - - -  ������������� ���������� ����� ���.������ */
     else           {
      
	for(i=_DECL_BASE ; i<_DECL_CNT ; i++)                       /* ������� ������� �����    */
	  if(!memcmp(text, dcl_type[i].mnemo,                       /*   �� ������� ����������  */
			   dcl_type[i].mnemo_size))  break ;

		if(i==_DECL_CNT)  break ;                           /* ���� ��� '�����������' �����... */

                    }   

                        text+=dcl_type[i].mnemo_size ;              /* ��������� � ���������� ����� */
/*- - - - - - - - - - - - - - - - - - -  ������ ����������� �������� */
    if((pref_type|dcl_type[i].pref_mask)                            /* �������� '����������' �������� */
	   !=     dcl_type[i].pref_mask ) {
                                       mError_code=_DCLE_BAD_PREF ;
					            return(0) ;           
                                          }

    if(dcl_type[i].pref_type) {                                     /* IF.1 - ���� ��� ������� ... */
	if(pref_type & dcl_type[i].pref_type) {                     /* �������� ���������� ������������� */
                                      mError_code=_DCLE_DBL_MOD ;   /*      ����������� ��������         */
						  return(0) ;     
                                              }

	      pref_type|=dcl_type[i].pref_type ;                    /* ������������ ��� �������� */
		   mode|=dcl_type[i].mode ;                         /* ����.����������� ���� */

	if(dcl_type[i].base!=_NO_DEF)                               /* ���� ���� ��������� �������� ���� - */
			 base_type=dcl_type[i].base ;               /*   - ������������� ���               */

			      }                                     /* ELSE.1 */
    else                      {                                     /* ELSE.1 - ���� ��� ������� ��� ... */
				 base_type=dcl_type[i].base ;       /* ���.������� ��� */
				       break ;                      /* �����.��������� �������� ���� */
			      }                                     /* END.1 */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
	   }                                                        /* CONTINUE.1 */
/*--------------------- ������ ����������� � ��������� �������� ���� */
/*- - - - - - - - - - - - - - - - -  ��������� ����������-���������� */
		       par_flag=0 ;                                 /* ����� ����� ��������� */
		       ext_flag=0 ;                                 /* ����� ����� ������� ���������� */
		       pub_flag=0 ;                                 /* ����� ����� ���������� ���������� */

	  if((pref_type&_PR_PRFX)==_PR_PRFX)                        /* ���� ���� ������� ���������      */
	    if(nTransit_num!=nTransit_cnt)  par_flag=1 ;            /*  � ���������� ������� ��         */
								    /*   ��������� - ���.���� ��������� */
	  if((pref_type&_EX_PRFX)==_EX_PRFX)  ext_flag=1 ;          /* ���� ������� ������� ���������� ... */
	  if((pref_type&_PB_PRFX)==_PB_PRFX)  pub_flag=1 ;          /* ���� ������� ���������� ���������� ... */
/*- - - - - - - - - - - - - - - - - - - - - -  ����� ������ �������� */
   if(!par_flag && !ext_flag) {                                     /* ���� ��� �� �������� �       */
								    /*    �� ������� ���������� ... */
	  if(pref_type==   0  &&                                    /*  ���� �������� ���� �� ���������� - */
	     base_type==_NO_DEF )  return(0) ;                      /*    ���������� ���������             */

	  if(base_type==_NO_DEF) {  mError_code=_DCLE_NO_TYPE ;     /*  ���� �� ����� ������� ��� ... */
						return(0) ;      }
			      }
/*-------------------------------------- ��������� ������ ���������� */

  for( addr=text ; addr!=NULL ; text=addr+1) {                      /* CIRCLE.2 - ����.������ ���������� ... */

		 addr=iFind_close(text, ',') ;                      /* ���� ����������� ���������� */
	      if(addr!=NULL)  *addr=0 ;                             /* ����������� �������� */

		    ptr_flag=0 ;                                    /* ����� ������ ����              */
		   area_flag=0 ;                                    /*   ����������: ���������/������ */
/*- - - - - - - - - - - - - - �������� � ������������ ��� ���������� */
	   if(*text=='*') {  ptr_flag=1 ;  text++ ;  }              /* ���� ����������� ��������� ... */

		     name=text ;                                    /* ���.��� ���������� */

	  for( ; *text!=0 && *text!='[' ; text++)                   /* �������� ������� �������� ����� */
	      if(!NAME_CHAR(*text)) {
                                  mError_code=_DCLE_SINTAX_NAME ;
					   break ;
                                    }
              if(mError_code)  break ;

	      if(*text=='[')  area_flag=1 ;                         /* ���� ������� ������ ... */

		*text=0 ;                                           /* ����������� ��� */
		 text++ ;
/*- - - - - - - - - - - - - - - - - - -  ��������� '����������' ���� */
	 if( area_flag && ptr_flag)  mError_code=_DCLE_PTR_AREA ;   /* ������ ���������� �������� ... */

         if( base_type==_TYPEDEF && area_flag)  area_flag=0 ;       /* ��� ������� ������������ ��� ����������� ������ ������� */
                                                                    /*   ��� ��� �� �������� ������������ ��������             */
	     if(mError_code)  return(0) ;                           /* ���� ���� ������ ... */

		type=iType_define(base_type, mode,                  /* ���������� ������������� ��� */
                                         ptr_flag, area_flag ) ;
	     if(mError_code)  return(0) ;                           /* ���� ���� ������ ... */

	if(base_type==_INTEGER ||
	   base_type==_FLOAT     )  size=iDgt_size(type) ;          /* �����.������ ������ ������� */
	else                        size= 1 ;
/*- - - - - - - - - - - - - - -  ���������� ������ ���������� ������ */
        if(area_flag) {                                             /* IF.1 - ���� ������ ... */

         if(text[0]==']') {                                         /* ���� ������ ������������... */
             if(dcl_typedef) {  mError_code=_DCLE_TYPEDEF_AREA ;    /* ��������� ������������ ������� */
                                               break ;           }  /*   ������ ������������ ����     */

                               buff_new=-1 ;
                          }
         else             {                                         /* ���� ������ �����������... */

	           tmp=iFind_close(text, ']') ;                     /* ���� ��������� ������� */
                if(tmp==NULL ||                                     /* ���� ������ ��� ���������� */
		   tmp==text   ) {  mError_code=_DCLE_NOINDEX ;     /*    ��������� ���������     */
						 break ;  }
		       *tmp=0 ;                                     /* ����������� ��������� ��������� */

		    iCulculate(text) ;                              /* ��������� ��������� ��������� */
               if(mError_code)  break ;                             /* ���� ���� ������ ... */

               if(t_base(nResult.type)!=_DGT_VAL) {                 /* �������� ���� ���������� ��������� */
			          mError_code=_DCLE_INDEX_TYPE ;
                                                     break ;
                                                  }

              buff_new=(int)iDgt_get(nResult.addr, nResult.type) ;  /* ��������� ���.���������� ������� */
                          }

		      }                                             /* END.1 */
	 else if( ptr_flag)   buff_new=0 ;                          /* ���� ��������� ... */
	 else                 buff_new=1 ;                          /* ���� ������� ���������� ... */

//                            buff_new*=size ;                      /* �������� ������ � ���������� �������� */
/*- - - - - - - - - - - - - -  ������ �������� ��� ������� ��������� */
     if(ext_proc!=NULL) {

                        memset(ext_proc, 0, sizeof(*ext_proc)) ;
                       strncpy(ext_proc->name, name, sizeof(dcl_x_elem->name)-1) ;
 		               ext_proc->type=type ;
 		               ext_proc->size=size*buff_new ;
 		               ext_proc->buff=buff_new ;

                                    break ;
                        }
/*- - - - - - - - - - - - - - - - - - -  ��������� TYPEDEF-��������� */
     if(dcl_typedef) {
                        memset(dcl_x_elem, 0, sizeof(*dcl_x_elem)) ;

                       strncpy(dcl_x_elem->name, name, sizeof(dcl_x_elem->name)-1) ;
 		               dcl_x_elem->type=type ;
 		               dcl_x_elem->size=buff_new ;
 		               dcl_x_elem->buff=buff_new ;
 		               dcl_x_elem->addr=(void *)CURR_X_TYPE.size  ;

                          offset=((size*buff_new-1)/4+1)*4 ;        /* ����������� ������ ��� 32-������� */
                        
                               CURR_X_TYPE.size+=offset ;

                                continue ;
                     }
/*- - - - - - - - - - - - - - - - - - -  �������� ������� ���������� */
        var=(Dcl_decl *)iTest_var(name, _DCL_INTERNAL) ;            /* ���� ���������� � �������� ������   */
     if(var!=NULL) {                                                /*   ����� ��� ������������ ���������� */
                               mError_code=_DCLE_NAME_INUSE ;  
                                      break ;                  
                   }

     if(ext_flag) {                                                 /* ���� ��� �������� ������� ���������� ... */
	  if(iTest_var(name, _DCL_EXTERNAL)!=NULL)  continue ;      /* ���� ���������� ����� �������    */
                                                                    /*  ���������� - ���� �� ����.���,  */
                  }                                                 /*   ����� ������� �����.���������� */
/*- - - - - - - - - - - - - - - - -  ��������� ����������-���������� */
     if(par_flag) {                                                 /* ���� �������� ��������� ... */
                    strcpy(nTransit[nTransit_num].name, name) ;     /*  ������� ��� ��������� � ���������� ������� */
                           nTransit[nTransit_num].work_nmb= 0 ;     /*  ����� �������� ������� ������ */
                                    nTransit_num++ ;                /*  �����.����� ��������� */

                    if(nTransit_num!=nTransit_cnt)  par_flag=0 ;    /*   ������������ ���������� ���������� ������� */
				    continue ;                      /*   ���� �� ����.��� */
                  }
/*- - - - - - - - - - - - - - - - - - - - - - ����������� ���������� */
                 var=iNext_title(_INTERNAL_VAR) ;                   /* ��������� ����� ��������� */
	    if(mError_code)  break ;                                /*   ���� ���� ������ ... */

                        strncpy(var->name, name, sizeof(var->name)-1) ;
		                var->type     =type ;
		                var->prototype=NULL ;
                                var->addr     =NULL ;

     if(base_type==_TYPEDEF) {  var->size     =  0 ;                /* ������� ������� ������ */
		                var->buff     =  0 ;         }
     else
     if(buff_new < 0       ) {  var->prototype= "D" ;
                                var->size     =  0 ;
                                var->buff     =_DYN_CHUNK ;  }
     else                    { 

        if(base_type==_INTEGER ||
           base_type==_FLOAT     )  var->size =buff_new ;
        else                        var->size =  0 ;
                                
                                    var->buff  =buff_new ;    }

      if(pub_flag || ext_flag)  var->local_idx= 0 ;                 /* ������� ���� ��������� ����������� */
      else                      var->local_idx=nLocal ;
/*- - - - - - - - - - - - - - - - - - - - ��������� ��������� ������ */
     if(var->type==_DCLT_CHR_PTR) {                                 /* ������ ��� ���������� ���������� */ 

       for(i=0 ; i<nT_blocks_cnt ; i++)                             /* ���� ��� ���������� ����� ��������� ������ */
         if(!strcmp(nT_blocks[i].name, var->name))  break ;

         if(i<nT_blocks_cnt) {                                      /* ���� ����� ��������� ���� ������ -     */
                                 var->addr= nT_blocks[i].data ;     /*  - ����������� ��� �������� ���������� */
                                 var->size=strlen((char *)var->addr) ;
                                 var->buff= var->size+1 ;
                                             continue ;
                             }
                                  }
/*- - - - - - - - - - - - - - - - - - - ���������� ������ ���������� */
     if(var->buff>0) {
                                iAlloc_var(_INTERNAL_VAR, var, 0) ;

				   if(mError_code)  break ;         /* ���� ���� ������ ... */
                     }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
					     }                      /* CONTINUE.2 */
/*-------------------------------------------------------------------*/

    return(0) ;
}


/*********************************************************************/
/*                                                                   */
/*    ����������� ���� ���������� �� �������� ���� (base_type)       */
/*       � ������������ (mode).                                      */
/*                                                                   */
/*     ��������� �������� � ���������� type.                         */

  int  Lang_DCL::iType_define(int  base_type, int  mode, 
                              int  ptr_flag,  int  area_flag)

{
  int  type ;

       if(base_type==_TYPEDEF) {                                    /* ������ ����������� ���� -  */
 	                            type=_DCLT_XTP_OBJ | mode<<8 ; 
                               } 
  else if(base_type==_CHAR   ) {
				 if(ptr_flag)  type=_CHR_PTR  ;
				 else          type=_CHR_AREA ;
			       }
  else if(base_type==_INTEGER) {                                    /* ���������� ���������� */
		  if(mode==_UNSIGNED        )  type=_DCLT_USHORT ;  /*  ��� ��� ��������     */
	     else if(mode==            _LONG)  type=_DCLT_LONG ;    /*   �������� INTEGER    */
	     else if(mode==_UNSIGNED + _LONG)  type=_DCLT_ULONG ;
	     else                              type=_DCLT_SHORT ;
			       }
  else if(base_type==_FLOAT  ) {                                    /* ���������� ���������� */
			      if(mode==_LONG)  type=_DCLT_DOUBLE ;  /*  ��� ��� ��������     */
			      else             type=_DCLT_FLOAT ;   /*   �������� FLOAT      */
			       }

       if(base_type==_FLOAT  ||                                     /* ���������� ������� ��� */
	  base_type==_INTEGER  ) {                                  /*   ��� �������� �������� */
		    if(area_flag)  type|=_DGT_AREA ;
	       else if( ptr_flag)  type|=_DGT_PTR ;
	       else                type|=_DGT_VAL ;
				 }

  return(type) ;
}


/************************************************************************/
/*                                                                      */
/*                 �������� ������� ����������                          */
/*                                                                      */
/*   name  -  ��� ����������                                            */
/*   type  -  ������� ��������: _DCL_INTERNAL - ����������              */
/*                              _DCL_EXTERNAL - �������                 */

   void *Lang_DCL::iTest_var(char *name, int  type)

{
   Dcl_decl *vars ;       /* ������� ������ �������� ���������� � �������� */
   Dcl_decl *targ ;       /* ��������� �� �������� ���������� */
        int  i ;

/*-------------------------------------------- ������������� */

		       targ=NULL ;

/*----------------------------- ������ � ���������� �������� */
 
 if(type==_DCL_INTERNAL) {
  
    for(i=0 ; i<nTransit_cnt ; i++)
      if(!strcmp(nTransit[i].name, name)) {
                                             targ=&nTransit[i] ;
                                                  break ;
                                          }
                         }
/*------------------------- ������ � ����������� ����������� */

 if(type==_DCL_INTERNAL) {

      for(vars=nInt_page ; vars->name[0]!=0 ; vars++)
        if(!strcmp(vars->name, name)) {  targ=vars ; 
                                            break ;  }
			 }
/*---------------------------- ������ � �������� ����������� */

 if(type==_DCL_EXTERNAL) {

   for(i=0 ; targ==NULL ; i++) {

	if(nVars[i]==   NULL  ||
	   nVars[i]==nInt_page  )  break ;

    for(vars=nVars[i] ; vars->name[0]!=0 ; vars++)
     if(        vars->func_flag==0 &&
                vars->local_idx==0 &&
	!strcmp(vars->name, name)    ) {  targ=vars ;
					     break ;  }

			       }
			 }
/*----------------------------- ������ � ������� ����������� */

   if( targ   ==NULL &&                                     /* ���� ������ �� ������ � ������             */
      mExt_lib!=NULL   ) {                                  /*  ��������� ����� � ������� ����������� ... */

		 targ=mExt_lib(_DCL_LOAD_FNC, name) ;       /*  ���� ������ �� ������� ���������� */
                         }
/*-----------------------------------------------------------*/

  return(targ) ;
}
