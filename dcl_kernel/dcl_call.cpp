/*   Bolotov P.D.  version  29.04.1994   */
/*     0 warnings at warning level 1     */

/*********************************************************************/
/*                                                                   */
/*                   DATA CONVERTION LANGUAGE                        */
/*                                                                   */
/*                ���������� ������ � �����������                    */
/*                                                                   */
/*********************************************************************/

#if defined(AIX) || defined(LINUX)
#ifndef UNIX
#define  UNIX
#endif
#endif


#include  <stddef.h>
#include  <stdlib.h>
#include  <string.h>
#include  <malloc.h>
#include  <ctype.h>
#include  <math.h>
#include  <fcntl.h>
#include  <stdio.h>
#include  <errno.h>
#include  <sys/types.h>
#include  <sys/stat.h>

#ifdef UNIX
#include  <unistd.h>
#define   stricmp  strcasecmp
#else
#include  <io.h>
#endif

#define DCL_INSIDE

#include "dcl.h"


#pragma warning(disable : 4996)

/*********************************************************************/
/*                                                                   */
/*            ������������� ������� ��������� DCL-���������          */

    void  Lang_DCL::iCall_init(void)

{
/*------------------------------ ������������ ������� main-��������� */

         memset(&nCall_main, 0, sizeof(nCall_main)) ;

         strcpy(nCall_main.name,    "main") ;
                nCall_main.path     =mProgram_name ;
                nCall_main.mem_flag =!mFile_flag ;
                nCall_main.file_addr=mByte_cnt ;
                nCall_main.mem_addr =mFile_mem ;

         strcpy(nCall_main.decl.name, nCall_main.name) ;
                nCall_main.decl.func_flag=   1 ;
                nCall_main.decl.work_nmb = DCL_WORK_NUM(0, _DCL_MAIN_P) ;
                nCall_main.decl.addr     =_DCL_LOAD ;

/*--------------------------------------------- ����.������ �������� */

       nCall_list    =(Dcl_call *)
                       calloc(_CALL_MAX, sizeof(Dcl_call)) ;

       nCall_list[0]=nCall_main ;

       nCall_list_cnt=  1 ;

/*-------------------------------------- ����.����� ������� �������� */

       nCall_stack    =(Dcl_call **)
                        calloc(_STACK_MAX, sizeof(*nCall_stack)) ;
       nCall_stack_ptr=  0 ;

       nCall_stack[0]=&nCall_list[0] ;

/*-------------------------------------------------------------------*/

}


/*********************************************************************/
/*                                                                   */
/*       ������������ �������� ������� ��������� DCL-���������       */

    void  Lang_DCL::iCall_free(void)

{
   if(nCall_list !=NULL)  free(nCall_list) ;
                               nCall_list=NULL ;

   if(nCall_stack!=NULL)  free(nCall_stack) ;
                               nCall_stack=NULL ;

       nCall_stack_ptr= 0 ;
}


/*********************************************************************/
/*                                                                   */
/*                    ����� DCL-���������                            */
/*                                                                   */
/*   ����� � �������� � ������� ��������� DCL-�������� ����������    */
/*    ����� ������ .work_num .                                       */

  Dcl_decl *Lang_DCL::iCall_find(char *name)

{
 Dcl_call *call_addr ;  /* �������� ��������� */
      int  ctlg ;
      int  n ;
      int  i ;

/*---------------------------------------------------- ������������� */

                    mError_code=  0 ;

                      call_addr=NULL ;

/*---------------------- ����� � ������� "������-���������" �������� */

#define  CURRENT_MODULE   nCall_stack[nCall_stack_ptr]

    for(i=0 ; i<CURRENT_MODULE->proc_cnt ; i++)
      if(!strcmp(CURRENT_MODULE->proc_list[i].name, name))
       if(CURRENT_MODULE->proc_list[i].v_flag==0) {                 /* ���� ��������� ��������� - ������ �� ����� */  
                      return(&CURRENT_MODULE->proc_list[i].decl) ;
                                                  }
       else                                       {                 /* ���� ����������� ��������� - ���� ������� �� ����� */
             for(n=0 ; n<=nCall_stack_ptr ; n++)
               if(!strcmp(nCall_stack[n]->proc_list[i].name, name) &&
                          nCall_stack[n]->proc_list[i].v_flag==1     )
                                  return(&nCall_stack[n]->proc_list[i].decl) ;
                                                  }

#undef   CURRENT_MODULE

/*--------------------------- ����� ����� ����� ������������ ������� */

    for(i=0 ; i<nCall_list_cnt ; i++)                               /* ���� ��������� �� ���-����� ������� */
       if(!strcmp(nCall_list[i].decl.name, name))                   /*  ���� ��������� ������� � ������ -  */
			    return(&nCall_list[i].decl) ;           /*  - ������ �� �������� �� �����      */

/*------------------------------------------ ����� � ������� ������� */

   if(call_addr==NULL &&                                            /* ���� ��������� �� ������� */
      mWorkDir !=NULL   ) {
                            call_addr=iCall_file(name, mWorkDir) ;  /* ���� ��������� ������� � ����            */
       if(call_addr==NULL)  call_addr=iCall_lib (name, mWorkDir) ;  /*  ���������� �����, � ����� � ����������� */
                          } 
/*------------------------------------ ����� � ������������ �������� */

   if(call_addr==NULL &&                                            /* ���� ��������� �� �������      */
      mLibDirs !=NULL   ) {                                         /*  � ������ ������������ ������� */

     for(ctlg=0 ; mLibDirs[ctlg]!=NULL &&                           /* ���� ��������� � ������ ������������ �������          */
		  call_addr     ==NULL   ; ctlg++) {                /* ������� � ���� ���������� �����, � ����� � ����������� */
			   call_addr=iCall_file(name, mLibDirs[ctlg]) ;
      if(call_addr==NULL)  call_addr=iCall_lib (name, mLibDirs[ctlg]) ;
                                                   }
                          }
/*-------------------------------- ����������� ��������� � ���-����� */

    if(call_addr!=NULL) {                                           /* ���� ��������� ������� ... */

      if(nCall_list_cnt>=_CALL_MAX) {  mError_code=_DCLE_MEMORY ;   /* ���� ������������ ������ ��������... */
                                         return(NULL) ;           }

#define  NEW_CALL   nCall_list[nCall_list_cnt]

	NEW_CALL               = *call_addr ;                       /* ������� ����� ������ �������� */

	 strcpy(NEW_CALL.decl.name,     NEW_CALL.name) ;            /* ��������� �������� ��������� */
	        NEW_CALL.decl.func_flag=   1 ;
	        NEW_CALL.decl.prototype= NULL ;
	        NEW_CALL.decl.work_nmb = DCL_WORK_NUM(nCall_list_cnt, _DCL_MAIN_P) ;
	        NEW_CALL.decl.addr     =_DCL_LOAD ;

#undef   NEW_CALL
                               nCall_list_cnt++;
			}
/*-------------------------------------------------------------------*/

   if(call_addr==NULL)  return(NULL) ;
   else                 return(&nCall_list[nCall_list_cnt-1].decl) ;
}


/*********************************************************************/
/*                                                                   */
/*                       ����� DCL-�����                             */

  Dcl_call *Lang_DCL::iCall_file(char *name, char *folder)

{
 static Dcl_call  call_addr ;            /* �������� ��������� */
	    char  path[FILENAME_MAX] ;   /* ��� DCL-����� */

/*------------------------------------------------------ ����� ����� */

          sprintf(path, "%s//%s.dcl", folder, name) ;

	if(access(path, 0x04))  return(NULL) ;                      /* ���� ���� ���������� */

/*-------------------------------------------- ����������� ��������� */

     strncpy(call_addr.name, name, sizeof(call_addr.name)-1) ;      /* ��� ��������� */
     strncpy(call_addr.path, path, sizeof(call_addr.path)-1) ;      /* ���� ����� */
	     call_addr.file_addr= 0L ;                              /* ��������� ����� */

/*-------------------------------------------------------------------*/

  return(&call_addr) ;
}


/*********************************************************************/
/*                                                                   */
/*                  ����� ��������� � DCL-����������                 */

  Dcl_call *Lang_DCL::iCall_lib(char *name, char *folder)

{
  return(NULL) ;
}


/*********************************************************************/
/*                                                                   */
/*         ����������� ������ ��������� � ����������� ��������       */
/*           � ��������� ������                                      */

   void  Lang_DCL::iCall_proc_list(Dcl_call *main)

{
    char  work[_BUFF_SIZE] ;  /* ������� ����� */
     int  work_cnt ;          /* ������� �������� ������ ����� */
     int  proc_cnt ;          /* ������� �������� */
     int  virtual_flag ;
     int  tblock_flag ;       /* ���� ����������� �� ���������� ����� */
     int  module_idx ;
    char *tmp ;
     int  i ;
     int  j ;

/*------------------------------------------------- ������� �������� */

    if(DCL_WORK2P(main->decl.work_nmb)!=_DCL_MAIN_P)  return ;      /* ���� ��� �� �������� ��������� ������... */

    if(main->proc_list!=NULL)  return ;                             /* ���� ������ �������� ��� ��������... */

          module_idx=DCL_WORK2M(main->decl.work_nmb) ;

/*-------------------------------- ���������� ������ ������ �������� */

                 mByte_cnt =0 ;                                     /* ����. �������� ������ */
                mError_code=0 ;

            main->proc_cnt= 0 ;
                  proc_cnt =0 ;

                tblock_flag=0 ;

  do {                                                              /* LOOP - ������������ �� ������� */
/*- - - - - - - - - - - - - - - - - - -  ���������� ��������� ������ */
	 work_cnt=iSysin(work, mByte_cnt, _BUFF_SIZE) ;             /* ��������� ��������� ������ */
      if(work_cnt==0)  break ;                                      /* ���� ��� ������... */

	 tmp=(char *)memchr(work, '\n', work_cnt) ;                 /* ���� ����� ������ <��> */

      if(tmp==NULL)                                                 /* ���� ����� ������ �� ������... */
	if(work_cnt==_BUFF_SIZE) tmp=work+_BUFF_SIZE-1 ;            /*  ������� ������ ������ ��������� ��������� ������ */
	else                     tmp=work+work_cnt ;                /*     ��� ������� ������                            */

		    *tmp=0 ;                                        /* �����.���������� ������ */
	      memset(tmp, 0, _BUFF_SIZE+work-tmp-2) ;               /* ��������� ����� ������ ������ */

	      mByte_cnt+=tmp-work+1 ;                               /* �����.������� ���� �� ����. ������ */

	       tmp=strchr(work, '\r') ;                             /* ��������� ���������    */
	    if(tmp!=NULL)  *tmp='\0' ;                              /*   ���������� <RET><NL> */
/*- - - - - - - - - - - - - - - - - - - ����������� ���������� ����� */
#define  TEXT  nT_blocks[nT_blocks_cnt]

   if(tblock_flag) {

     if(memcmp(work, "text_end", strlen("text_end"))==0) {          /* ���� ����� ����� ���������� �����... */

               TEXT.data=(char *)realloc(TEXT.data, strlen(TEXT.data)+8) ;

                                nT_blocks_cnt++ ;
                                   tblock_flag=0 ;
                                       continue ;
                                                         }
     else                                                {          /* ���� ���������� ���������� �����... */

              if(strlen(TEXT.data)+strlen(work)+2>=_TBLOCK_SIZE) {
                                          mError_code=_DCLE_TBLOCK_SIZE ;
                                                 break ;
                                                                 }

                              strcat(work, "\r\n") ;
                              strcat(TEXT.data, work) ;
                                      continue ;
                                                         }
                   }
   else            {

     if(memcmp(work, "text_start", strlen("text_start"))==0) {

            tmp=strstr(work, "text_start")+strlen("text_start") ;   /* �������� �� ��� ���������� ����� */
         while(*tmp==' ')  tmp++ ;

                nT_blocks=(Dcl_tblock *)                            /* ������������� ������ ��������� ������ */ 
                            realloc(nT_blocks, 
                                    (nT_blocks_cnt+1)*sizeof(*nT_blocks)) ;

                 strncpy(TEXT.name, tmp, sizeof(TEXT.name)-1) ;     /* ������������ ���� */
                         TEXT.data=(char *)calloc(1, _TBLOCK_SIZE) ;

                                          tblock_flag=1 ;
                                              continue ;
                                                             }
                   }

#undef   TEXT
/*- - - - - - - - - - - - - - - - - - -  ������ �������� � ��������� */
     for(i=0, j=0 ; work[i] ; i++, j++) 
       if(work[i]== ' ' ||                                          /* ��� ����������� �������       */
	  work[i]=='\t'   )      j-- ;                              /*  ��� ��������� - ��������� �� */
       else                 work[j]=work[i] ;

                            work[j]= 0 ;                            /* ����������� ������ */
/*- - - - - - - - - - - - - - - - - - - - - -  ��������� ����������� */
	 if(work[0]=='/' &&
	    work[1]=='/'   )  continue ;
/*- - - - - - - - - - - - - - - - - - -  ����������� ����� ��������� */
     if(memcmp(work,        "procedure", 
              strlen(       "procedure"))==0 ||
        memcmp(work, "virtualprocedure", 
              strlen("virtualprocedure"))==0   ) {

          virtual_flag=(memcmp(work, "virtual", 
                              strlen("virtual"))==0)?1:0 ;

            tmp=strstr(work, "procedure")+strlen("procedure") ;     /* �������� �� ��� ��������� */

        if(stricmp(tmp, "main"     )==0 ||
           stricmp(tmp,  main->name)==0   )  continue ;             /* ���� "��������" ��������� �� ������������ */

           main->proc_list=(Dcl_call *)                             /* ������������� ������ �������� */ 
                            realloc(main->proc_list, 
                                     (proc_cnt+1)*sizeof(main->proc_list[0])) ;

#define  CALL  main->proc_list[proc_cnt]
#define  DECL  main->proc_list[proc_cnt].decl

          strncpy(CALL.name, tmp, sizeof(CALL.name)-1) ;            /* ������������ ���� */
                  CALL.path     =main->path ;
                  CALL.mem_flag =main->mem_flag ;
                  CALL.file_addr=mByte_cnt ;
                  CALL.mem_addr =main->mem_addr ;
                  CALL.v_flag   = virtual_flag ;

           strcpy(DECL.name,     CALL.name) ;                       /* ��������� �������� ��������� */
                  DECL.func_flag=   1 ;
 	          DECL.prototype= NULL ;
                  DECL.work_nmb = DCL_WORK_NUM(module_idx, proc_cnt) ;
                  DECL.addr     =_DCL_LOAD ;

#undef   CALL
#undef   DECL

                                 proc_cnt++ ;                       /* �����.������� �������� */
                  main->proc_cnt=proc_cnt ;
                                                 }
/*- - - - - - - - - - - - - - - - - - - - - - - - -  ���� �� ������� */
     } while(1) ;                                                   /* ENDLOOP - ������������ �� ������� */

/*------------------------------------------------ ������ ���������� */

       if(tblock_flag) {                                            /* ���� ������� ���������� ��������� ���� */ 
                           mError_code=_DCLE_TBLOCK_CLOSE ;
                                 return ;
                       }

      if(main->proc_list==NULL)                                     /* ���� ������ �� �������� ��������� � ����������� */
                 main->proc_list=(Dcl_call *)0x0001 ;               /*  ����������� - ������ �������� ��� ����������   */
                                                                    /*  ���������� ���������� ������ �����������       */
/*-------------------------------------------------------------------*/

  return ;
}


/*********************************************************************/
/*                                                                   */
/*                    ���������� DCL-���������                       */

   Dcl_decl *Lang_DCL::iCall_exec(int decl_num)

{
   Dcl_call *entry ;
        int  m_idx ;
        int  p_idx ;
        int  par_base ;    /* ������� ����� ���������� */
        int  pars_cnt ;    /* ������� ���������� */
        int  i ;

//#define  CALL   nCall_list[decl_num]

/*---------------------------------- ����������� ������ �� ��������� */

            m_idx=DCL_WORK2M(decl_num) ;
            p_idx=DCL_WORK2P(decl_num) ;

     if(p_idx==_DCL_MAIN_P)  entry=&nCall_list[m_idx] ;
     else                    entry=&nCall_list[m_idx].proc_list[p_idx] ;

/*---------------------------------------- ����������� ����� ������� */

     if(nCall_stack_ptr>=_STACK_MAX) {  mError_code=_DCLE_STACK_OVR ;
                                           return(NULL) ;             }

                   nCall_stack_ptr++ ;
       nCall_stack[nCall_stack_ptr]=entry ;

/*-------------------------------------- �������� ���������� ������� */

  if(nPars_cnt) {                                                   /* ���� ������� ��������� */
        nTransit=(Dcl_decl *)calloc(nPars_cnt, sizeof(Dcl_decl)) ;  /* �������� ������ ��� ���������� ������� */
     if(nTransit==NULL)  return(NULL) ;

	    par_base=nPars_cnt_all-nPars_cnt ;                      /* O��.������� ����� ���������� */

	for(i=0 ; i<nPars_cnt ; i++)                                /* ��������� ���������� ������� ���������� */
	     nTransit[i]=*nPars_table[par_base+i] ;
		}
/*------------------------------------------------- ���������� ����� */

		    pars_cnt=nPars_cnt ;                            /* ����.����� �����.���������� */

	   Stack(_DCL_PUSH) ;                                       /* ����.������� ��������� ��������  */
                                                                    /*   � ������. ������ ������.������ */
   if(mError_code) return(NULL) ;                                   /* ���� ���� ������... */ 

	     nTransit_cnt=pars_cnt ;                                /* ���.����� ������������ ���������� */

               nSeg_start=entry->file_addr ;
                   nLocal++ ;

   if(entry->mem_flag)                                              /* ��������� DCL-��������� */
           Interpritator(_DCL_MEMORY, entry->mem_addr, nVars) ;
   else    Interpritator(_DCL_FILE,   entry->path,     nVars) ;

                   Stack(_DCL_POP) ;                                /* �����.�������� ��������� �������� */

/*------------------------------------------------ ���������� ������ */

    if(nTransit!=NULL)  free(nTransit) ;                            /* �����.������ ���������� ������� */

/*-------------------------------------------- ������� ����� ������� */

                   nCall_stack_ptr-- ;
                            nLocal-- ;

/*-------------------------------------------------------------------*/

 return(&nReturn) ;
}


/*********************************************************************/
/*                                                                   */
/*       ���������� �������� ���������� ������������ ��������        */

   void  Lang_DCL::iCall_final(void)

{
   Dcl_decl *vars ;       /* ������� ������ �������� ���������� � �������� */
        int  i ;

   void (*final_proc)(void) ;

/*---------------------------- ���������� $Final$-�������� ��������� */

   for(i=0 ;  ; i++) {                                              /* LOOP - ���������� ���������� */

                        if(nVars[i]==NULL) break ;

     for(vars=nVars[i] ; vars->name[0]!=0 ; vars++) {               /* � ���������� ���� � �������� ���������  */ 
                                                                    /*      � ������ $Final$                   */
          if(      ! vars->func_flag       )  continue ;
          if(stricmp(vars->name, "$Final$"))  continue ;

              final_proc=(void (*)(void))vars->addr ;
              final_proc() ;
                                                    }


                    }                                              /* END LOOP - ���������� ���������� */
/*-------------------------------------------------------------------*/

   return ;
}

