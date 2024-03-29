
/********************************************************************/
/*                                                                  */
/*           ������ ���������� �������� "������� ������"            */
/*                                                                  */
/********************************************************************/

#ifdef O_EXTERNAL_EXPORTS
#define O_EXTERNAL_API __declspec(dllexport)
#else
#define O_EXTERNAL_API __declspec(dllimport)
#endif


/*---------------------------------------------- ��������� ��������� */

/*--------------- �������� ������ ��������� ������� "������� ������" */

  class O_EXTERNAL_API RSS_Transit_External : public RSS_Transit {

    public:
             virtual   int  vExecute(void) ;             /* ���������� �������� */
                                             
    public:
                            RSS_Transit_External() ;     /* ����������� */
                           ~RSS_Transit_External() ;     /* ���������� */

                                                                 } ;

/*------------------------- �������� ������ ������� "������� ������" */

 struct RSS_Object_ExternalTrace {
                                     double  x_base ;
                                     double  y_base ;
                                     double  z_base ;

                                     double  a_azim ;
                                     double  a_elev ;
                                     double  a_roll ;
  
                                     double  x_velocity ;
                                     double  y_velocity ;
                                     double  z_velocity ;

                                   COLORREF  color ;
                                 } ;

struct RSS_Object_ExternalLink {
                                     char  link[FILENAME_MAX] ;
                                   double  time_mark ;
                               } ;

  class O_EXTERNAL_API RSS_Object_External : public RSS_Object {

    public : 

#define                       _TARGETS_MAX     512000
#define                 _TARGETS_LINKS_MAX       1000

         static                            char *targets ;                           /* �������� ����� */
         static                          double  targets_time ;                      /* ����� ������� ��� �������� ����� */
         static                             int  targets_init ;                      /* ����� ������������� �������� ����� */

         static  struct RSS_Object_ExternalLink *targets_links[_TARGETS_LINKS_MAX] ; /* ������ ������� �������� �������� ����� */
         static                             int  targets_links_cnt ;

    public:
                        char  model_path[FILENAME_MAX] ;         /* ���� ������ */

                      double  v_abs ;                            /* ���������� �������� */
                      double  g_ctrl ;                           /* ���������� ����������� ���������� */

                         int  g_over ;                           /* ������ � ������������ ����������� */

                         int  start_flag ;                       /* ������� ������� ���� ������������� */
                        char  object_type[FILENAME_MAX] ;        /* ��� ������� */
                        char  iface_type[FILENAME_MAX] ;         /* ��� ����������: FILE, TCP-SERVER */
                        char  iface_file_folder[FILENAME_MAX] ;  /* ����� ��� ���������� �������� ������ */
                        char  iface_targets[FILENAME_MAX] ;      /* ��� ����� ����� ��� URL ���������� ��� �������� ������ ����� */
                        char  iface_file_control[FILENAME_MAX] ; /* ��� ��� ��������/��������� ������ ������, ���������� - in � out */
                        char  iface_tcp_connect[FILENAME_MAX] ;  /* URL ���������� */

   private:

    RSS_Object_ExternalTrace *mTrace ;                           /* ���������� */
                         int  mTrace_cnt ;  
                         int  mTrace_max ;
                    COLORREF  mTrace_color ;
                         int  mTrace_dlist ;

    public:

         virtual       void  vFree          (void) ;            /* ���������� ������� */
         virtual RSS_Object *vCopy          (char *) ;          /* ���������� ������ */
         virtual       void  vPush          (void) ;            /* ��������� ��������� ������� */
         virtual       void  vPop           (void) ;            /* ������������ ��������� ������� */
         virtual       void  vWriteSave     (std::string *) ;   /* �������� ������ � ������ */
         virtual        int  vCalculateStart(double) ;          /* ���������� ������� ��������� ��������� */
         virtual        int  vCalculate     (double, double,    /* ������ ��������� ��������� */
                                                     char *, int) ;
         virtual        int  vCalculateExt1 (double, double,
                                                     char *, int) ;
         virtual        int  vCalculateExt2 (double, double,
                                                     char *, int) ;
         virtual        int  vCalculateShow (double, double) ;  /* ����������� ���������� ������� ��������� ��������� */
         virtual        int  vEvent         (char *, double,    /* ��������� ������� */
                                                     char *, int) ;
         virtual        int  vSpecial       (char *, void *) ;  /* ����������� �������� */
                        int  iSaveTracePoint(char *) ;          /* ���������� ����� ���������� */
                       void  iShowTrace_    (void) ;            /* ����������� ���������� � ��������� ��������� */
                       void  iShowTrace     (void) ;            /* ����������� ���������� */

	                     RSS_Object_External() ;             /* ����������� */
	                    ~RSS_Object_External() ;             /* ���������� */
                                                        } ;

/*------------- �������� ������ ���������� �������� "������� ������" */

  class O_EXTERNAL_API RSS_Module_External : public RSS_Kernel {

    public:

     static
      struct RSS_Module_External_instr *mInstrList ;        /* ������ ������ */

                  double  g_step ;                          /* ��� ��������� ����������� ���������� */
                  double  a_step ;                          /* ��� ��������� ����� */

    public:
     virtual  RSS_Object *vCreateObject (RSS_Model_data *) ;           /* �������� ������� */ 
     virtual         int  vGetParameter (char *, char *) ;             /* �������� �������� */
     virtual         int  vExecuteCmd   (const char *) ;               /* ��������� ������� */
     virtual        void  vReadSave     (std::string *) ;              /* ������ ������ �� ������ */
     virtual        void  vWriteSave    (std::string *) ;              /* �������� ������ � ������ */

    public:
                     int  cHelp         (char *) ;                     /* ���������� HELP */ 
                     int  cCreate       (char *) ;                     /* ���������� CREATE */ 
                     int  cInfo         (char *) ;                     /* ���������� INFO */ 
                     int  cCopy         (char *) ;                     /* ���������� COPY */ 
                     int  cOwner        (char *) ;                     /* ���������� OWNER */ 
                     int  cTarget       (char *) ;                     /* ���������� TARGET */ 
                     int  cBase         (char *) ;                     /* ���������� BASE */ 
                     int  cAngle        (char *) ;                     /* ���������� ANGLE */ 
                     int  cVelocity     (char *) ;                     /* ���������� VELOCITY */
                     int  cControl      (char *) ;                     /* ���������� CONTROL */
                     int  cTrace        (char *) ;                     /* ���������� TRACE */
                     int  cIType        (char *) ;                     /* ���������� ITYPE */
                     int  cIFile        (char *) ;                     /* ���������� IFILE */
                     int  cITcp         (char *) ;                     /* ���������� ITCP */

              RSS_Object *FindObject    (char *, int) ;                /* ����� ������� ���� EXTERNAL �� ����� */

    public:
	                  RSS_Module_External() ;              /* ����������� */
	                 ~RSS_Module_External() ;              /* ���������� */
                                                       } ;

/*-------------------------------------------- ���������� ���������� */

 struct RSS_Module_External_instr {

           char                       *name_full ;          /* ������ ��� ������� */
           char                       *name_shrt ;          /* �������� ��� ������� */
           char                       *help_row ;           /* HELP - ������ */
           char                       *help_full ;          /* HELP - ������ */
            int (RSS_Module_External::*process)(char *) ;   /* ��������� ���������� ������� */
                                 }  ;

/*--------------------------------------------- ���������� ��������� */

/* ����  O_External.cpp */

/* ����  O_External_dialog.cpp */
  INT_PTR CALLBACK  Object_External_Help_dialog  (HWND, UINT, WPARAM, LPARAM) ;
  INT_PTR CALLBACK  Object_External_Create_dialog(HWND, UINT, WPARAM, LPARAM) ;
  INT_PTR CALLBACK  Object_External_Iface_dialog (HWND, UINT, WPARAM, LPARAM) ;
