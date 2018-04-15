
#ifndef  RSS_KERNEL_H 

#define  RSS_KERNEL_H 

#include "..\RSS_Model\RSS_Model.h"
#include "..\RSS_Feature\RSS_Feature.h"
#include "..\RSS_Object\RSS_Object.h"

#include <string>

#ifdef RSS_KERNEL_EXPORTS
#define RSS_KERNEL_API __declspec(dllexport)
#else
#define RSS_KERNEL_API __declspec(dllimport)
#endif

/*----------------------------- �������� "�����" ������������ ������ */

    struct RSS_Module_Entry {
		       class RSS_Kernel *entry ;     /* ���� */
				HMODULE  module ;    /* ���������� DLL */
			    } ;

/*----------------------------------------- �������� "�����" ������� */

    struct RSS_Resource {
		              char  name[256] ;      /* �������� */
		              void *ptr ;	     /* ��������� */
			} ;

/*-------------------------------------- �������� ������ ����������� */

    struct RSS_Parameter {
		              char  name[256] ;      /* �������� */
		            double  value ;	     /* �������� */
		            double *ptr ;	     /* ��������� */
			 } ;
/*---------------------------------------- �������� ��������� ������ */

   struct RSS_Context {
                         char  name[64] ;    /* �������� ��������� */
                         void *data ;        /* ������ ������ */
             class RSS_Kernel *module ;      /* ������ �� ������ */

                      }  ;
/*---------------------------------------- �������� ���������� ����� */

    struct RSS_Name {
		              char  name[256] ;      /* ��� */
		              char  module[256] ;    /* ��������� ������ */
		    } ;
/*------------------------------ �������� ��������������� ���������� */

   struct RSS_Redirect {
                         char *master ;     /* ������� ������ */
                         char *command ;    /* ������� */
             class RSS_Kernel *module ;     /* ������ �� ������ */

                       }  ;
/*---------------------------------------- �������� ����� ���������� */

   struct RSS_Result {
                         char  id[256] ;    /* ������������� */
                         void *result ;     /* ��������� */
                     }  ;
/*---------------------------------- ��������� � ������� ����������� */

    struct RSS_Display {
                          int  x ;  
                          int  y ;  

		          int  (* GetList)        (int) ;     /* �������������� ����������� ������ */
		         void  (* ReleaseList)    (int) ;     /* ������������ ����������� ������ */
		          int  (* SetFirstContext)(char *) ;  /* ���������� ������ ����������� �������� */
		          int  (* SetNextContext) (char *) ;  /* ���������� ��������� ���������� �������� */
		         void  (* ShowContext)    (char *) ;  /* ���������� �������� */
		       double  (* GetContextPar)  (char *) ;  /* ������ �������� ��������� */
                       } ;
/*---------------------------------- �������� "�����" �������� ����� */

typedef  int (CALLBACK *RSS_Kernel_CallBack)(int, void *)  ;

    struct RSS_CB_Entry {
		              char  name[64] ;       /* �������� */
	       RSS_Kernel_CallBack  proc ;	     /* ��������� */
			} ;

#define  _CALLB_MAX  10

/*----------------------------------------------- ���������� ������� */

 typedef  struct {
                    char  entry[128] ;  /* �������� ����� */
                     int  idx1 ;        /* 1-�� ������ */
                     int  idx2 ;        /* 2-�� ������ */

                    void *ptr  ;        /* ��������� �� ���� ������ */
                    long  size ;        /* ������ ����a ������ */
                     int  used ;        /* ���� ������������� */
                  time_t  time ;        /* ����� �������� */

                 }  RSS_Memory ;

/*----------------------------------------------- ����������� ������ */

 typedef  struct {
                    char  path[FILENAME_MAX] ;
                    char *data ;
                 }  RSS_File ;

/*------------------------------------- ��������� ������������ ����� */

  class RSS_KERNEL_API RSS_IFace {

    public:
                                     char *std_iface ;

    public:
             virtual  void  vClear (void) ;              /* �������� ������ */
             virtual  void  vSignal(char *, void *) ;    /* ������� ������ */
             virtual  void  vPass  (RSS_IFace *) ;       /* ������� ������� */
             virtual   int  vDecode(char *, void *) ;    /* ������ ������� */
             virtual   int  vCheck (char * ) ;           /* �������� ������� */
                                             
    public:
                            RSS_IFace() ;                /* ����������� */
                           ~RSS_IFace() ;                /* ���������� */

                                 } ; 

/*------------------------------ �������� ������ ������������ ������ */

  class RSS_KERNEL_API RSS_Kernel {

   public : 
    static   RSS_Kernel  *kernel ;	            /* ������-���� */
    static         HWND   kernel_wnd ;	            /* ���� ���� */
    static         HWND   active_wnd ;	            /* �������� o��� */
    static    HINSTANCE   kernel_inst ;	            /* ������������� ������ ���� */

    static          int   battle ;                  /* ���� ������ ������������� ��� */
    static          int   stop ;                    /* ���� ��������� ���������� */
    static          int   next ;                    /* ���� ���������� ���������� ���� */
#define                    _RSS_KERNEL_NEXT_STEP  1
#define                    _RSS_KERNEL_WAIT_STEP  2

    static   RSS_Kernel  *priority_entry ;	    /* ������ ��� ������������� ������ */
    static   RSS_Kernel **events_entry ;            /* ������ �������, ������� ��������� ��������� ������� */
    static          int   events_entry_cnt ;

    static   RSS_Object **kernel_objects ;          /* ������ �������� */
    static          int   kernel_objects_cnt ;

    static     RSS_Name  *kernel_names ;            /* ������ ��������� ���� */
    static          int   kernel_names_cnt ;

    static   RSS_Kernel **feature_modules ;         /* ������ �������-������� */
    static          int   feature_modules_cnt ;

    static   RSS_Kernel **calculate_modules ;       /* ������ ������������ */
    static          int   calculate_modules_cnt ;

    static     RSS_File  *files ;                   /* ������ ������������ ������ */
    static          int   files_cnt ;

    static   RSS_Result **results ;                 /* ���� ����������� ������ ������� */
    static          int   results_cnt ;

    static RSS_CB_Entry   callbacks[_CALLB_MAX] ;   /* ������� �������� ����� */

    static  RSS_Display   display ;                 /* ��������� � ������� ����������� */

    static          int   srand_fixed ;             /* ���� ������������� �������������� ���������� ��������� ����� */

    static       double   calc_time_step ;          /* ����� ��������� ������������� �� ������� */ 
    static       double   show_time_step ;          /* ������� ����������� ����� �� ������� */ 

    static          int   debug_flag ;
    static         char   debug_list[1024] ;        /* ������ ������������ ������� */ 
   
   public : 
		HMODULE   DLL_module ;		    /* ���������� DLL, ���������� ������ */
       RSS_Module_Entry  *modules ;	            /* ������ ����������� ������� */
		    int   modules_cnt ;
	   RSS_Resource  *resources ;	            /* ������ ������������ �������� */
		    int   resources_cnt ;
           RSS_Redirect  *command_redirect ;        /* ������ ��������������� ������ */
                    int   command_redirect_cnt ;

   public :
                   char  *keyword ;	            /* �������� ���� - ������������� ������� */
                   char  *identification ;          /* ������������� ������ */
                   char  *category ;                /* ��������� ������������ ������ */
                   char  *lego_type ;               /* ��� Lego-�������� */
                   char  *lego_set ;                /* ����� Lego-��������� */
                    int   priority ;                /* ��������� ��������� ���������� */
                    int   events_processing ;       /* ���� ����������� ��������� ������� */
                    
	  	    int   attempt ;		    /* ������ "������" */

   public:
		   void	  SetKernel(const HWND,         /* ���������� ���� */
				    const HINSTANCE) ; 
    RSS_Kernel_CallBack   SetCallB (char *,             /* ������� ������� �������� ����� */
				    RSS_Kernel_CallBack) ;
    RSS_Kernel_CallBack   GetCallB (char *) ;           /* ������ ������� �������� ����� */
                   void  *Resource (const char *,       /* ����������� ������� */
				    const char * ) ; 
		    int	  Load     (const char *) ;     /* �������� ������������ ������� */
                   char  *FileCache(char *, char *) ;   /* �������� ����� � ������ */
		    int	  Free     (void) ;	        /* ������������ ������������ ������� */
		    
   public:
	    virtual int   vKernelEvents (void) ;           /* ��������� ������� */

   public:
     virtual        void  vStart        (void) ;               /* ��������� �������� */
     virtual        void  vInit         (void) ;               /* ������������� ������ */
     virtual        void  vReadSave     (std::string *) ;      /* ������� ������ �� ������ */
     virtual        void  vWriteSave    (std::string *) ;      /* �������� ������ � ������ */
     virtual  RSS_Object *vCreateObject (RSS_Model_data *) ;   /* ������� ������ */
     virtual RSS_Feature *vCreateFeature(RSS_Object *,         /* ������� �������� */
                                         RSS_Feature * ) ;
     virtual         int  vExecuteCmd   (const char *) ;       /* ��������� ������� */
     virtual         int  vExecuteCmd   (const char *,         /* ��������� ������� � ������� ���������� �� ������ */
                                          RSS_IFace *) ;
     virtual         int  vCalculate    (char *, char *,       /* ��������� ��������� */
                                         struct RSS_Parameter *,
                                         struct RSS_Parameter *,
                                         double *, 
                                         void **, char *) ;
     virtual         int  vGetParameter (char *, char *) ;     /* �������� �������� */
     virtual        void  vSetParameter (char *, char *) ;     /* ���������� �������� */
     virtual        void  vProcess      (void) ;               /* ��������� ������� ���������� */
     virtual        void  vShow         (char *) ;             /* ���������� ��������� ������ */
     virtual        void  vChangeContext(void)  ;              /* ��������� �������� � ��������� ������ */

     virtual RSS_Context *vAddData      (RSS_Context ***) ;    /* ���� ��������� ������ � ������ ���������� */
     virtual         int  vReadData     (RSS_Context ***,      /* ������� ������ ��������� ������ �� ������ */
                                         std::string * ) ;
     virtual        void  vWriteData    (RSS_Context *,        /* �������� ������ ��������� ������ � ������ */
                                         std::string * ) ;
     virtual        void  vReleaseData  (RSS_Context *) ;      /* ���������� ������� ������ ��������� ������ */

     virtual      double  vGetTime      (void) ;               /* ��������� ����� */

    public:

                  double  gGaussianValue(double, double) ;     /* ���������� ������������� */

    public:

     static          int   RSS_Kernel::memchk_regime ;
#define                     _MEMCHECK_OFF      0
#define                     _MEMCHECK_ACTUAL   1
#define                     _MEMCHECK_HISTORY  2
     static   RSS_Memory  *RSS_Kernel::memchk_list ;
     static          int   RSS_Kernel::memchk_list_max ; 
     static          int   RSS_Kernel::memchk_list_idx ; 
     static          int   RSS_Kernel::memchk_rep_num ; 

		    void  gMemOnOff     (int) ;
		    void  gMemList      (char *) ;
		    void *gMemCalloc    (size_t, size_t, char *, int, int) ;  
		    void *gMemRealloc   (void *, size_t, char *, int, int) ;  
		    void  gMemFree      (void *) ;
		     int  gMemCheck     (void) ;
                    void  iMemAddList   (void *, int, char *, int, int) ;
                    void  iMemDelList   (void *) ;

    public:
		    void  iErrorMsg     (const char *) ;   /* ������������ ��������� �� ������� */
		    void  iLogFile      (const char *) ;   /* ������ ��������� � ��� ���� */

    public:
			  RSS_Kernel    () ;               /* ����������� */
			 ~RSS_Kernel    () ;               /* ���������� */
		                  } ;

/*-------------------------------------------------------- ��������� */

#define  _USER_EXECUTE_COMMAND    101    /* ��������� ������� */
#define  _USER_COMMAND_PREFIX     102    /* ��������� �������� ��������� ������ */
#define  _USER_DEFAULT_OBJECT     103    /* ��������� ������� �� ��������� */

#define  _USER_DIRECT_COMMAND     201    /* ������� ������ ������� ���������� */

#define  _USER_SHOW               301    /* ����������� */
#define  _USER_CHANGE_CONTEXT     302    /* ��������� � ��������� ������ ��������� */

#define  _USER_THREAD_ADD         401    /* ���������������� ������� ����� */
#define  _USER_THREAD_DELETE      402    /* ��������� ������� ����� */

#define  _USER_ERROR_MESSAGE      901    /* ��������� �� ������ */
#define  _USER_SHOW_COMMAND       902    /* �������� ������� */
#define  _USER_SHOW_INFO          903    /* �������� ���������� �� ������� */
#define  _USER_INFO_MESSAGE       904    /* ��������� �� �� ������ */
#define  _USER_CHECK_MESSAGE      905    /* ��������� ����������� ������� �����: �����������, ����������� � ������ */
#define  _USER_THREAD_MESSAGE     906    /* ��������� �� ���������� ������ */

/*-------------------------------------------------------------------*/

#endif        // RSS_KERNEL_H 
