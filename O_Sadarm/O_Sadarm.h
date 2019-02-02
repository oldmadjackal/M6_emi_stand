
/********************************************************************/
/*								    */
/*		������ ���������� �������� "SADARM"  		    */
/*								    */
/********************************************************************/

#ifdef O_SADARM_EXPORTS
#define O_SADARM_API __declspec(dllexport)
#else
#define O_SADARM_API __declspec(dllimport)
#endif


/*---------------------------------------------- ��������� ��������� */

/*----------------------- �������� ������ ��������� ������� "SADARM" */

  class O_SADARM_API RSS_Transit_Sadarm : public RSS_Transit {

    public:
             virtual   int  vExecute(void) ;             /* ���������� �������� */
                                             
    public:
                            RSS_Transit_Sadarm() ;        /* ����������� */
                           ~RSS_Transit_Sadarm() ;        /* ���������� */

                                                              } ;

/*----------------------------- �������� ������ �������� "���������" */

  class O_SADARM_API RSS_Feature_Hit_Sadarm : public RSS_Feature_Hit {

    public:
            virtual  int  vCheck   (void *, RSS_Objects_List *) ;     /* �������� ������������������ �������� */  
                                         

    private:
                     int  iBodyTest(RSS_Feature_Hit_Body *,           /* �������� ����������� ���� � ������� */
                                                  double *, double *, int) ;
                                    
    public:
	                  RSS_Feature_Hit_Sadarm() ;                /* ����������� */
	                 ~RSS_Feature_Hit_Sadarm() ;                /* ���������� */
                                                           } ;

/*--------------------------------- �������� ������ ������� "SADARM" */

  class O_SADARM_API RSS_Object_Sadarm : public RSS_Object {

    public:
                      char  model_path[FILENAME_MAX] ;        /* ���� ������ */

                  double  v_abs ;                             /* ���������� �������� */
                  double  scan_altitude ;                     /* ��������� ������ ������������ */
                  double  scan_tilt ;                         /* ���� ������� ����� ������������ � ��������� */
                  double  scan_rotation ;                     /* �������� ������������, �������� � ������ */
                  double  scan_view ;                         /* ���� ������, �������� */

                COLORREF  color ;                             /* ���� ������� */
                     int  view_dlist ;                        /* ������� ���������� ������� */
                     int  hit_flag ;                          /* ��������� �������� */
               RSS_Point  hit_point ;                         /* ����� ��������� */

    public:
         virtual       void  vFree          (void) ;            /* ���������� ������� */
         virtual RSS_Object *vCopy          (char *) ;          /* ���������� ������ */
         virtual       void  vWriteSave     (std::string *) ;   /* �������� ������ � ������ */
         virtual        int  vCalculateStart(double) ;          /* ���������� ������� ��������� ��������� */
         virtual        int  vCalculate     (double, double,    /* ������ ��������� ��������� */
                                                     char *, int) ;
         virtual        int  vCalculateShow (double, double) ;  /* ����������� ���������� ������� ��������� ��������� */
         virtual        int  vEvent         (char *, double) ;  /* ��������� ������� */
         virtual        int  vSpecial       (char *, void *) ;  /* ����������� �������� */

                        int  iShowView      (void) ;            /* ��������� ������� */
                        int  iShowView_     (void) ;            /* ��������� ������� � ��������� ��������� */

	                     RSS_Object_Sadarm() ;             /* ����������� */
	                    ~RSS_Object_Sadarm() ;             /* ���������� */
                                                        } ;

/*--------------------- �������� ������ ���������� �������� "SADARM" */

  class O_SADARM_API RSS_Module_Sadarm : public RSS_Kernel {

    public:

     static
      struct RSS_Module_Sadarm_instr *mInstrList ;          /* ������ ������ */

    public:
     virtual  RSS_Object *vCreateObject (RSS_Model_data *) ;           /* �������� ������� */ 
     virtual         int  vGetParameter (char *, char *) ;             /* �������� �������� */
     virtual        void  vShow         (char *) ;                     /* ���������� ��������� ������ */
     virtual         int  vExecuteCmd   (const char *) ;               /* ��������� ������� */
     virtual        void  vReadSave     (std::string *) ;              /* ������ ������ �� ������ */
     virtual        void  vWriteSave    (std::string *) ;              /* �������� ������ � ������ */

    public:
                     int  cHelp         (char *) ;                     /* ���������� HELP */
                     int  cCreate       (char *) ;                     /* ���������� CREATE */
                     int  cInfo         (char *) ;                     /* ���������� INFO */
                     int  cCopy         (char *) ;                     /* ���������� COPY */
                     int  cBase         (char *) ;                     /* ���������� BASE */ 
                     int  cVelocity     (char *) ;                     /* ���������� VELOCITY */
                     int  cScan         (char *) ;                     /* ���������� SCAN */
                     int  cTrace        (char *) ;                     /* ���������� TRACE */

              RSS_Object *FindObject    (char *, int) ;                /* ����� ������� ���� BODY �� ����� */

    public:
	                  RSS_Module_Sadarm() ;              /* ����������� */
	                 ~RSS_Module_Sadarm() ;              /* ���������� */
                                                       } ;

/*-------------------------------------------- ���������� ���������� */

 struct RSS_Module_Sadarm_instr {

           char                     *name_full ;          /* ������ ��� ������� */
           char                     *name_shrt ;          /* �������� ��� ������� */
           char                     *help_row ;           /* HELP - ������ */
           char                     *help_full ;          /* HELP - ������ */
            int (RSS_Module_Sadarm::*process)(char *) ;   /* ��������� ���������� ������� */
                                }  ;

/*--------------------------------------------- ���������� ��������� */

/* ����  O_Sadarm.cpp */

/* ����  O_Sadarm_dialog.cpp */
  INT_PTR CALLBACK  Object_Sadarm_Help_dialog  (HWND, UINT, WPARAM, LPARAM) ;
  INT_PTR CALLBACK  Object_Sadarm_Create_dialog(HWND, UINT, WPARAM, LPARAM) ;
