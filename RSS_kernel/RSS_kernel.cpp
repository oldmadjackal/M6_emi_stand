
#define _CRT_RAND_S  // ��� ����������� ������� rand_s
#include <stdlib.h>
#include <stdio.h>
#include <io.h>
#include <malloc.h>
#include <time.h>
#include <errno.h>
#include <math.h>
#include <sys\types.h> 
#include <sys\timeb.h>
#include <sys\stat.h>

#include <windows.h>

#include "..\RSS_feature\RSS_feature.h"
#include "..\RSS_object\RSS_object.h"
#include "RSS_Kernel.h"

#pragma warning(disable : 4267)
#pragma warning(disable : 4996)


BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
		case DLL_PROCESS_DETACH:
			break;
    }
    return TRUE;
}


/********************************************************************/
/*								    */
/*		       ����������� ����������			    */

 RSS_KERNEL_API    RSS_Kernel  *RSS_Kernel::kernel               =NULL ;
 RSS_KERNEL_API          HWND   RSS_Kernel::kernel_wnd           =NULL ;
 RSS_KERNEL_API          HWND   RSS_Kernel::active_wnd           =NULL ;
 RSS_KERNEL_API     HINSTANCE   RSS_Kernel::kernel_inst          =NULL ;

 RSS_KERNEL_API           int   RSS_Kernel::battle               =  0 ;
 RSS_KERNEL_API           int   RSS_Kernel::stop                 =  0 ;
 RSS_KERNEL_API           int   RSS_Kernel::next                 =  0 ;

 RSS_KERNEL_API    RSS_Object **RSS_Kernel::kernel_objects       =NULL ; 
 RSS_KERNEL_API           int   RSS_Kernel::kernel_objects_cnt   =  0 ; 

 RSS_KERNEL_API    RSS_Kernel **RSS_Kernel::feature_modules      =NULL ; 
 RSS_KERNEL_API           int   RSS_Kernel::feature_modules_cnt  =  0 ; 

 RSS_KERNEL_API    RSS_Kernel **RSS_Kernel::calculate_modules    =NULL ; 
 RSS_KERNEL_API           int   RSS_Kernel::calculate_modules_cnt=  0 ; 

 RSS_KERNEL_API      RSS_File  *RSS_Kernel::files                =NULL ; 
 RSS_KERNEL_API           int   RSS_Kernel::files_cnt            =  0 ; 

 RSS_KERNEL_API    RSS_Result **RSS_Kernel::results              =NULL ; 
 RSS_KERNEL_API           int   RSS_Kernel::results_cnt          =  0 ;

 RSS_KERNEL_API    RSS_Kernel  *RSS_Kernel::priority_entry       =NULL ;
 RSS_KERNEL_API    RSS_Kernel **RSS_Kernel::events_entry         =NULL ;
 RSS_KERNEL_API           int   RSS_Kernel::events_entry_cnt     =  0 ;

 RSS_KERNEL_API  RSS_CB_Entry   RSS_Kernel::callbacks[_CALLB_MAX] ; 

 RSS_KERNEL_API   RSS_Display   RSS_Kernel::display ;

 RSS_KERNEL_API           int   RSS_Kernel::srand_fixed ;

 RSS_KERNEL_API        double   RSS_Kernel::calc_time_step ;
 RSS_KERNEL_API        double   RSS_Kernel::show_time_step ;

 RSS_KERNEL_API           int   RSS_Kernel::debug_flag           =  0 ;
 RSS_KERNEL_API          char   RSS_Kernel::debug_list[1024] ;

/********************************************************************/
/*								    */
/*		    ���������� ������ - ����			    */

  void  RSS_Kernel::SetKernel(HWND wnd, HINSTANCE inst)

{
      kernel     =this ;
      kernel_wnd =wnd ;
      kernel_inst=inst ;
}


/********************************************************************/
/*								    */
/*		     ������� ������� �������� �����		    */

  RSS_Kernel_CallBack  RSS_Kernel::SetCallB(char *name, 
                                             RSS_Kernel_CallBack  proc)

{
   int  i ;


    for(i=0 ; i<_CALLB_MAX ; i++)
      if(!strcmp(callbacks[i].name, name)   ||
		 callbacks[i].name[0]==  0  ||
		 callbacks[i].proc   ==NULL   )  {

			 strcpy(callbacks[i].name, name) ;
				callbacks[i].proc=proc ;
				    break ;
					         }
   return(NULL) ;
}


/********************************************************************/
/*								    */
/*		     ������ ������� �������� �����		    */

  RSS_Kernel_CallBack  RSS_Kernel::GetCallB(char *name)

{
   int  i ;


    for(i=0 ; i<_CALLB_MAX ; i++)
      if(!strcmp(name, callbacks[i].name))  return(callbacks[i].proc) ;

   return(NULL) ;
}


/********************************************************************/
/*								    */
/*		    ����������� �������				    */

  void *RSS_Kernel::Resource(const char *name, const char *type)

{
    HRSRC  hRes ;    
  HGLOBAL  hData ;
     void *ptr ;
      int  i ;

/*----------------------------------------- �������� ������� ������� */

    for(i=0 ; i<resources_cnt ; i++)
     if(!strcmp(name, resources[i].name))  return(resources[i].ptr) ;

/*------------------------------------------------- �������� ������� */

	hRes=FindResource(DLL_module, name, type) ;
     if(hRes==NULL)  return(NULL) ;

        hData=LoadResource(DLL_module, hRes) ;
     if(hData==NULL)  return(NULL) ;

        ptr=LockResource(hData) ;
     if(ptr==NULL)  return(NULL) ;

/*---------------------------------------------- ����������� ������� */

     resources=(RSS_Resource *)
                  realloc(resources, sizeof(*resources)*
					 (resources_cnt+1)) ;
  if(resources==NULL) {
			resources_cnt=0 ;
			     return(NULL) ; ;
		      }

     strcpy(resources[resources_cnt].name, name) ;
	    resources[resources_cnt].ptr=ptr ;
                      resources_cnt++ ;

/*-------------------------------------------------------------------*/

    return(ptr) ;
}


/********************************************************************/
/*								    */
/*		      �������� ������������ �������		    */
/*								    */
/*	path - ���� � ������� ������������ �������� �������	    */

typedef       char *(*KERNEL_API)(void);
typedef RSS_Kernel *(*MODULE_PTR)(void);

   int  RSS_Kernel::Load(const char *path)

{
            char  dll_mask[512] ;   /* ���� � ������� DLL */
        intptr_t  dll_group ;	    /* ���������� ��������� ������ DLL */
     _finddata_t  dll_file ;	    /* �������� ���������� ����� */
            char  dll_path[512] ;   /* ������ ���� � ����� DLL */
	 HMODULE  module ;	    /* ���������� DLL */
      KERNEL_API  identify ;	    /* ����� ��������� ������������ DLL */
      MODULE_PTR  link ;	    /* ����� ��������� ����� � ������� */
	    char  message[512] ;    
             int  status ;
  

/*------------------------------------------------------- ���������� */

                      strcpy(dll_mask, path) ;
                      strcat(dll_mask, "*.dll") ;

/*--------------------------------------------------- �������� � DLL */

        dll_group=_findfirst(dll_mask, &dll_file);
     if(dll_group<0) {
	      sprintf(message, "No DLL on path: %s", dll_mask) ;
	    iErrorMsg(message) ;
	  	        return(-1) ;
		     }

  while(dll_group) {						    /* CIRCLE.1 */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - �������� DLL */
	          sprintf(dll_path, "%s%s", path, dll_file.name) ;
       module=LoadLibrary(dll_path);
    if(module==0) {						    /* IF.1 - ���� ������ �������� DLL... */
		       sprintf(message, "DLL load failure: %s", dll_path) ;
		     iErrorMsg(message) ;
		  }						    /* ELSE.1 */
/*- - - - - - - - - - - - - - - - - - - - - - - -  ������������� DLL */
    else	  {						    /* ELSE.1 - ���� DLL ���������... */

	        identify=(KERNEL_API)GetProcAddress(module,         /* �������� ��������� ������������� DLL */
                                                    "Identify");

      if(       identify==NULL     ||				    /* IF.2 - ���� ������������� �� ������... */
	 strcmp(identify(), keyword) ) {
					  FreeLibrary(module) ;
				       }			    /* ELSE.2 */
/*- - - - - - - - - - - - - - - - - - - - - - - - -  ����������� DLL */
      else			       {			    /* ELSE.2 - ���� ��� ������ DLL... */

	   link=(MODULE_PTR)GetProcAddress(module, "GetEntry");     /* �������� ��������� ����� � ������� */
	if(link==NULL) {					    /* ���� ��������... */
		           sprintf(message, "DLL registration error %d: %s",
					   GetLastError(), dll_path) ;
		         iErrorMsg(message) ;		
		       }
	else	       {					    /* ������������ ������... */
	      modules=(RSS_Module_Entry *)
                         realloc(modules, (modules_cnt+1)*sizeof(*modules)) ;
		modules[modules_cnt].entry =link() ; 
		modules[modules_cnt].module=module ; 
		
		modules[modules_cnt].entry->DLL_module=module ;	    /* �� ����������� ������          */
								    /*   ��������� ���������� ��� DLL */
		        modules_cnt++ ;
		       }
				       }			    /* END.2 */
		  }						    /* END.1 */
/*- - - - - - - - - - - - - - - - - - - - - - -  ����� ��������� DLL */
		status=_findnext(dll_group, &dll_file) ;
	     if(status!=0)  break ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
	           }						    /* CONTINUE.1 */

	   if(dll_group)  _findclose(dll_group) ;

/*-------------------------------------------------------------------*/

  return(0) ;
}


/********************************************************************/
/*								    */
/*		      �������� ������������ �������		    */

   char *RSS_Kernel::FileCache(char *path, char *error)

{
      RSS_File  cache ;
  struct _stat  attr ;
          FILE *file ;
           int  i ; 

/*------------------------------------------ ��������� ������� ����� */

     for(i=0 ; i<this->files_cnt ; i++)
       if(!stricmp(this->files[i].path, path))  return(this->files[i].data) ;

/*--------------------------------------------------- �������� ����� */

     if(_stat(path, &attr)) {
               sprintf(error, "File open error %d : %s", errno, path) ;
                                return(NULL) ;
                            } 

        strcpy(cache.path, path) ;
               cache.data=(char *)calloc(1, attr.st_size) ;

        file=fopen(path, "rb") ;
     if(file==NULL) {
                        sprintf(error, "File open error %d : %s", errno, path) ;
                           return(NULL) ;
                    }

           fread(cache.data, 1, attr.st_size, file) ;
          fclose(file) ;

/*---------------------------------------- ���������� ����� � ������ */

      this->files=(RSS_File *)
                    realloc(this->files, 
                              sizeof(this->files[0])*(this->files_cnt+1)) ;

      this->files[this->files_cnt]=cache ;

/*-------------------------------------------------------------------*/

  return(cache.data) ;
}


/********************************************************************/
/*								    */
/*		    ������������ ������������ �������		    */

   int  RSS_Kernel::Free(void)

{
  int  i ;

    
    if(modules_cnt) {
                	for(i=0 ; i<modules_cnt ; i++)
               	              FreeLibrary(modules[i].module) ;

	                             free(modules) ;
	    	                          modules    =NULL ;
		                          modules_cnt=  0 ;
                    }

  return(0) ;
}


/********************************************************************/
/*								    */
/*		       ����������� ������			    */

     RSS_Kernel::RSS_Kernel(void)

{
          keyword     =NULL ;
   identification     =NULL ;
         category     =NULL ;
             lego_type=NULL ;
             lego_set =NULL ;
         

       	  modules     =NULL ; 
	  modules_cnt =  0 ;
	resources     =NULL ;
	resources_cnt =  0 ;

	 priority     =  0 ;
}


/********************************************************************/
/*								    */
/*		        ���������� ������			    */

    RSS_Kernel::~RSS_Kernel(void)

{
}


/********************************************************************/
/*								    */
/*	                 ��������� �������                          */	

  int  RSS_Kernel::vKernelEvents(void)

{
   return(0) ;
}


/********************************************************************/
/*								    */
/*		������������ ��������� �� ������		    */	

   void  RSS_Kernel::iErrorMsg(const char *message)

{  
     MessageBox(GetActiveWindow(), message, "RSS Kernel", MB_ICONERROR) ;
       iLogFile(message) ;
}


/********************************************************************/
/*								    */
/*		������ ��������� � ���-����			    */	

   void  RSS_Kernel::iLogFile(const char *message)

{
         FILE *file ;         

       file=fopen("kernel.log", "at") ;
    if(file==NULL)  return ;

           fwrite(this->identification, 1, strlen(this->identification), file) ;
           fwrite(     "\t",            1, strlen(     "\t"           ), file) ;
           fwrite(      message,        1, strlen(      message       ), file) ;
           fwrite(     "\n",            1, strlen(     "\n"           ), file) ;
           fclose(file) ;
}


/********************************************************************/
/*								    */
/*		        ��������� ��������                          */

    void  RSS_Kernel::vStart(void)

{
}


/********************************************************************/
/*								    */
/*		        ������������� ������                        */

    void  RSS_Kernel::vInit(void)

{
}


/********************************************************************/
/*								    */
/*		        ������� ������ �� ������		    */

    void  RSS_Kernel::vReadSave(std::string *text)

{
}


/********************************************************************/
/*								    */
/*		        �������� ������ � ������		    */

    void  RSS_Kernel::vWriteSave(std::string *text)

{
     *text="" ;
}


/********************************************************************/
/*								    */
/*		        ������� ������                  	    */

    RSS_Object *RSS_Kernel::vCreateObject(RSS_Model_data *model)

{
   return(NULL) ;
}


/********************************************************************/
/*								    */
/*		        ������� ��������                	    */

    RSS_Feature *RSS_Kernel::vCreateFeature(RSS_Object *object, RSS_Feature *feature)

{
   return(NULL) ;
}


/********************************************************************/
/*								    */
/*		        ��������� �������       		    */

     int  RSS_Kernel::vExecuteCmd(const char *command)

{
   return(1) ;
}

     int  RSS_Kernel::vExecuteCmd(const char *command, RSS_IFace *iface)

{
   return(vExecuteCmd(command)) ;
}


/********************************************************************/
/*								    */
/*		        ��������� ���������     		    */

    int  RSS_Kernel::vCalculate(            char  *expression_type,
                                            char  *expression, 
                            struct RSS_Parameter  *const_list,
                            struct RSS_Parameter  *var_list,
                                          double  *result,
                                            void **context, 
                                            char  *error)

{

   return(0) ;
}


/********************************************************************/
/*								    */
/*		        �������� ��������       		    */
/*		        ������ ��������         		    */

     int  RSS_Kernel::vGetParameter(char *name, char *value)

{
/*-------------------------------------------------- �������� ������ */

    if(!stricmp(name, "$$MODULE_NAME")) {

         sprintf(value, "%-20.20s -  Unknown module", identification) ;
                                        }
/*-------------------------------------------------------------------*/

   return(0) ;
}


    void  RSS_Kernel::vSetParameter(char *name, char *value)

{
}


/********************************************************************/
/*								    */
/*                   ��������� ������� ����������                   */

    void  RSS_Kernel::vProcess(void)

{
}


/********************************************************************/
/*								    */
/*                    ���������� ��������� ������                   */

    void  RSS_Kernel::vShow(char *layer)

{
}


/********************************************************************/
/*								    */
/*             ��������� �������� � ��������� ������                */

    void  RSS_Kernel::vChangeContext(void)

{
}


/********************************************************************/
/*								    */
/*           ������� ������ ��������� ������ �� ������              */

    int  RSS_Kernel::vReadData(RSS_Context ***context,
                               std::string   *text    )
{
   return(0) ;
}


/********************************************************************/
/*								    */
/*            �������� ������ ��������� ������ � ������             */

   void  RSS_Kernel::vWriteData(RSS_Context *context,
                                std::string *text    )
{
   *text="" ;
}


/********************************************************************/
/*								    */
/*            ���������� ������� ������ ����������� ������          */

    void  RSS_Kernel::vReleaseData(RSS_Context *context)

{
     if(context==NULL)  return ;

        free(context->data) ;
        free(context) ;
}


/********************************************************************/
/*								    */
/*                ���� ��������� � ������ ����������                */

     RSS_Context *RSS_Kernel::vAddData(RSS_Context ***contexts)

{
  int  j ;

#define  C   (*contexts)

/*---------------------------------- ������������� ������ ���������� */

       if(C==NULL)                                                  /* ���� ������ ���������� ���� -  */
          C=(RSS_Context **)calloc(1, sizeof(*C)) ;                 /*   - �������������� ���         */
       if(C==NULL)  return(NULL) ;                                  /* ���� �� ������� ����������... */
                         
/*------------------------------------------- ����� ��������� ������ */

      for(j=0 ; C[j]!=NULL ; j++)                                   /* ���� �������� ����� ������ */
        if(!strcmp(C[j]->name, this->identification))  break ;

       if(C[j]!=NULL)  return(C[j]) ;                               /* ���� �������� ������ -> ��� ��������� */

/*---------------------------------------- �������� ��������� ������ */

          C=(RSS_Context **)realloc(C, (j+2)*sizeof(*C)) ;          /* ����������� ������ ���������� */
       if(C==NULL)  return(NULL) ;                                  /* ���� �� ������� ����������... */

          C[j+1]=  NULL ;                                           /* ����������� ������ ���������� */
          C[j  ]=(RSS_Context *)calloc(1, sizeof(**C)) ;            /* ��������� �������� */
       if(C[j  ]==NULL)  return(NULL) ;                             /* ���� �� ������� ����������... */

/*-------------------------------- ������� ��������������� ��������� */

         strcpy(C[j]->name, this->identification) ;                 /* ������� �������������� */
                C[j]->module=this ;                                 /* ������� ������-������ */

/*-------------------------------------------------------------------*/

   return(C[j]) ;
}


/********************************************************************/
/*								    */
/*                          ��������� �����                         */

     double  RSS_Kernel::vGetTime(void)

{
   struct _timeb  time_ms ;


        _ftime(&time_ms) ;

  return(time_ms.time+time_ms.millitm/1000.) ;
}


/********************************************************************/
/*								    */
/*           ��������� �������� � �������� ��������������           */

     double  RSS_Kernel::gLinearValue(double  v_min, double  v_max)

{
  unsigned int  g ;
        double  v ;


            rand_s(&g) ;

                  v=g/(double)UINT_MAX ;
                  v=v_min+(v_max-v_min)*v ;
 
  return(v) ;
}


/********************************************************************/
/*								    */
/*           ��������� �������� � ���������� ��������������         */
/*								    */
/*	m - �������������� ��������                                 */
/*	s - ����������� ���������� (s*s - ���������)                */

     double  RSS_Kernel::gGaussianValue(double  m, double  s)

{
  unsigned int  g ;
        double  r ;
        double  f ;
        double  v ;


            rand_s(&g) ;
                  r=g/(double)UINT_MAX ;
       if(r==0.)  r=0.5 ;
            rand_s(&g) ;
                  f=g/(double)UINT_MAX ;
       if(f==0.)  f=0.5 ;

                  v=cos(2*_PI*f)*sqrt(-2.*log(r)) ;
                  v=m+v*s ;

  return(v) ;
}


/********************************************************************/
/********************************************************************/
/**							           **/
/**                        ���������� �������                      **/
/**								   **/
/********************************************************************/
/********************************************************************/


 RSS_KERNEL_API          int   RSS_Kernel::memchk_regime  =  0 ;
 RSS_KERNEL_API   RSS_Memory  *RSS_Kernel::memchk_list    =NULL ;
 RSS_KERNEL_API          int   RSS_Kernel::memchk_list_max=  0 ; 
 RSS_KERNEL_API          int   RSS_Kernel::memchk_list_idx=  0 ; 
 RSS_KERNEL_API          int   RSS_Kernel::memchk_rep_num =  0 ; 


/********************************************************************/
/*								    */
/*                         ������ ������� CALLOC                    */

   void *RSS_Kernel::gMemCalloc(size_t  n, size_t  size, 
                                  char *entry, int  idx1, int  idx2 )
{
  void *ptr ;

          ptr=calloc(n, size) ;
         iMemAddList(ptr, size*n, entry, idx1, idx2) ;

   return(ptr) ;
}


/********************************************************************/
/*								    */
/*                         ������ ������� REALLOC                   */

   void *RSS_Kernel::gMemRealloc(void *ptr, size_t  size, 
                                 char *entry, int  idx1, int  idx2 )
{
        iMemDelList(ptr) ;
        ptr=realloc(ptr, size) ;
        iMemAddList(ptr, size, entry, idx1, idx2) ;

   return(ptr) ;
}


/********************************************************************/
/*								    */
/*                         ������ ������� FREE                      */

   void  RSS_Kernel::gMemFree(void *ptr)
{
               free(ptr) ;
        iMemDelList(ptr) ;
}


/********************************************************************/
/*								    */
/*                         ������ ������� HEAPCHK                   */

   int  RSS_Kernel::gMemCheck(void)
{
  int  status ;


       status=_heapchk() ;
    if(status==_HEAPBADBEGIN ||
       status==_HEAPBADNODE  ||
       status==_HEAPBADPTR     ) {
                                    return(-1) ;
                                 } 

    if(status==_HEAPOK &&
        errno== ENOSYS   ) {
                                    return( 1) ;
                           } 

                                    return( 0) ;
}


/********************************************************************/
/*								    */
/*               ���./����. ����������� ������������ ������         */

   void  RSS_Kernel::gMemOnOff(int  regime) 
{
        memchk_regime=regime ;
}


/*********************************************************************/
/*                                                                   */
/*                ������ ������������� ������ � ����                 */

   void  RSS_Kernel::gMemList(char *path) 
{
       FILE *file ;
       char  file_name[1024] ;
       char *star ;
  struct tm *hhmmss ;           
       char  text[1024] ;
        int  i ;

/*------------------------------------------------- ������� �������� */

     if(!memchk_regime)  return ;

     if(memchk_list==NULL)  return ;

/*-------------------------------- ������������ ����� ����� �� ����� */

              strcpy(file_name, path) ;
         star=strchr(file_name, '*') ;

      if(star!=NULL) {
                                              memchk_rep_num++ ;
                        sprintf(text, "%06d", memchk_rep_num) ;

                        memmove(star+5, star, strlen(star)+1) ;
                         memcpy(star, text, 6) ;
                     }
/*-------------------------------------------- �������� ����� ������ */

        file=fopen(file_name, "wt") ;
     if(file==NULL) {
                       iErrorMsg("Memory list file open error") ;
                              return ;
                    }
/*------------------------------------------- ������ �������� ������ */

        sprintf(text, "\n*** ACTIVE ENTRIES ***\n\n") ;
         fwrite(text, 1, strlen(text), file) ;

        for(i=0 ; i<memchk_list_max ; i++) {

          if(memchk_list[i].used!=1)  continue ;

               hhmmss=localtime(&memchk_list[i].time) ;

           sprintf(text, "%02d:%02d:%02d %02d.%02d.%02d %p Size %-7d Idx=%03d/%03d Entry=%s\n",
                          hhmmss->tm_hour, hhmmss->tm_min, hhmmss->tm_sec,
                          hhmmss->tm_mday, hhmmss->tm_mon+1, hhmmss->tm_year-100,
                          memchk_list[i].ptr,
                          memchk_list[i].size,
                          memchk_list[i].idx1,
                          memchk_list[i].idx2,
                          memchk_list[i].entry ) ;
            fwrite(text, 1, strlen(text), file) ;
                                           }

/*-------------------------------------------- �������� ����� ������ */

        fclose(file) ;

/*-------------------------------------------------------------------*/   
}


/*********************************************************************/
/*                                                                   */
/*                  ���������������� ����                            */
  
  void  RSS_Kernel::iMemAddList(void *ptr, int size, 
                                char *entry, int idx1, int idx2) 

{
  int  i ;

/*------------------------------------------------- ������� �������� */

     if(!memchk_regime)  return ;

     if(ptr==NULL)  return ;

/*------------------------------------------- ����� ���������� ����� */

    for(i=0 ; i<memchk_list_max ; i++)
      if(memchk_list[i].used==0)  break ;

/*----------------------------------------- ���������� ������ ������ */

      if(i==memchk_list_max) {

           memchk_list_max+=  100 ;
           memchk_list     =(RSS_Memory *)
                             realloc(memchk_list,
                                     memchk_list_max*sizeof(memchk_list[0])) ;
        if(memchk_list==NULL) {
                                     return ;
                              }  

             memset(&memchk_list[i], 0,
                    (memchk_list_max-i)*sizeof(memchk_list[0])) ;
                             }
/*------------------------------------------------ ����������� ����� */

        strncpy(memchk_list[i].entry, entry, sizeof(memchk_list[i].entry)-1) ;
                memchk_list[i].idx1=idx1 ;
                memchk_list[i].idx2=idx2 ;
                memchk_list[i].ptr =ptr ;
                memchk_list[i].size=size ;
                memchk_list[i].used= 1 ;
                memchk_list[i].time=time(NULL)  ;

/*-------------------------------------------------------------------*/
}


/*********************************************************************/
/*                                                                   */
/*                       ���������� ����                             */

  void  RSS_Kernel::iMemDelList(void *ptr)
{
   char  text[1024] ;
    int  i ;

/*------------------------------------------------- ������� �������� */

     if(!memchk_regime)  return ;

     if(ptr==NULL)  return ;

     if(memchk_list    ==NULL ||
        memchk_list_max==  0    )  return ;

/*------------------------------------------------------ ����� ����� */

    for(i=0 ; i<memchk_list_max ; i++)
      if(memchk_list[i].used== 1 &&
         memchk_list[i].ptr ==ptr  )  break ;

/*---------------------------------------------- ���� ���� �� ������ */

      if(i==memchk_list_max) {
/*- - - - - - - - - - - - - - - - - ����� ����� ������������� ������ */
        for(i=0 ; i<memchk_list_max ; i++)
          if(memchk_list[i].used==-1 &&
             memchk_list[i].ptr ==ptr  )  break ;


          if(i==memchk_list_max) {
                 sprintf(text, "Memory %p unregistered", ptr) ;                                
               iErrorMsg(text) ;
                                     return ;
                                 }

                 sprintf(text, "Memory %p already freed: <%s>.%d.%d",
                                             ptr, memchk_list[i].entry,
                                                  memchk_list[i].idx1,
                                                  memchk_list[i].idx2 ) ;
               iErrorMsg(text) ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                             }
/*----------------------------------------------- ������������ ����� */

    if(memchk_regime==_MEMCHECK_HISTORY)  memchk_list[i].used=-1 ;
    else                                  memchk_list[i].used= 0 ;    

/*-------------------------------------------------------------------*/
}


/********************************************************************/
/********************************************************************/
/**							           **/
/**                ��������� ������������ �����                    **/
/**								   **/
/********************************************************************/
/********************************************************************/

/********************************************************************/
/*								    */
/*		       ����������� ������			    */

     RSS_IFace::RSS_IFace(void)

{
   this->std_iface=(char *)calloc(1, 64000) ;
}


/********************************************************************/
/*								    */
/*		        ���������� ������			    */

    RSS_IFace::~RSS_IFace(void)

{
   if(this->std_iface!=NULL)  free(this->std_iface) ;
}


/********************************************************************/
/*								    */
/*                          �������� ������                         */

   void  RSS_IFace::vClear(void)
{
   if(this->std_iface!=NULL)  this->std_iface[0]=0 ;
}


/********************************************************************/
/*								    */
/*                          ������������ ������                     */

   void  RSS_IFace::vSignal(char *signal, void *data)
{
   if(this->std_iface!=NULL)  
         sprintf(this->std_iface, "%s:%s", signal, (char *)data) ;
}


/********************************************************************/
/*								    */
/*                          T������ �������                         */

   void  RSS_IFace::vPass(RSS_IFace *iface)
{
   if( this->std_iface!=NULL &&
      iface->std_iface!=NULL   )  
         strcpy(this->std_iface, iface->std_iface) ;
}


/********************************************************************/
/*								    */
/*                        ������ �������                            */

    int  RSS_IFace::vDecode(char *signal, void *data)
{
   char *end ;


    if(signal!=NULL)  *signal=0 ;

    if(this->std_iface!=NULL) {

          end=strchr(this->std_iface, ':') ;
       if(end!=NULL) {
                               *end=0 ;
         if(signal!=NULL)  strcpy(signal, this->std_iface) ;
         if(data  !=NULL)  strcpy((char *)data,   end+1) ;
                               *end=':'  ;
                     }
       else          {

         if(signal!=NULL)  strcpy(signal, this->std_iface) ;
                     }
                               }

  if( signal==NULL ||  
     *signal==  0    )  return(0) ;
                        return(1) ;
}


/********************************************************************/
/*								    */
/*                         �������� �������                         */

    int  RSS_IFace::vCheck(char *signal)
{

    if(this->std_iface==NULL)  return(0) ;

    if(signal==NULL) {

            if(this->std_iface[0]!=0)  return(1) ;

                      }
     else             {

            if(strlen(this->std_iface)<strlen(signal))  return(0) ;
              
            if(memicmp(this->std_iface, signal, 
                                       strlen(signal)))  return(0) ;

            if(this->std_iface[strlen(signal)]!=':' &&
               this->std_iface[strlen(signal)]!= 0    )  return(0) ;

                                                         return(1) ;
                      }

   return(0) ;
}


