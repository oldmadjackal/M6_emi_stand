
/********************************************************************/
/*                                                                  */
/*     ������ ���������� �������� "�������� �������� ��������"      */
/*                                                                  */
/********************************************************************/

#ifdef O_ZRK_EXPORTS
#define O_ZRK_API __declspec(dllexport)
#else
#define O_ZRK_API __declspec(dllimport)
#endif


/*---------------------------------------------- ��������� ��������� */

/*-------------------------- �������� ������ ��������� ������� "���" */

  class O_ZRK_API RSS_Transit_ZRK : public RSS_Transit {

    public:
             virtual   int  vExecute(void) ;        /* ���������� �������� */
                                             
    public:
                            RSS_Transit_ZRK() ;     /* ����������� */
                           ~RSS_Transit_ZRK() ;     /* ���������� */

                                                       } ;

/*------------------------------------ �������� ������ ������� "���" */

 struct RSS_Object_ZRKTrace {
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

 struct RSS_Object_ZRK_trace {
                                RSS_Object *target ;
                                      char  missile_name[128] ;  /* �������� ��������� ��������� ��� */
                                RSS_Object *missile ;            /* ������ �� ��������� ��������� ��� */
                                    double  last_check ;
                                       int  next ;               /* ������� ��� �������� ��������� � ������� ����� */
                             }  ;

  class O_ZRK_API RSS_Object_ZRK : public RSS_Object {

    public:
                        char  model_path[FILENAME_MAX] ;         /* ���� ������ */

                      double  r_firing_min ;                     /* ����������� ��������� ����� */
                      double  r_firing_max ;                     /* ������������ ��������� ����� */
                      double  h_firing_max ;                     /* ������������ ������ ���� */
                         int  ballistic_target ;                 /* ����� ������ �������������� ����� */
                        char  missile_type[FILENAME_MAX] ;       /* ��� ������� - ������� ������ */
                  RSS_Object *missile_tmpl ;                     /* ������ �� ������ - ������ ������ */
                         int  missiles_max ;                     /* ������������ ����� �����, ������� � ����� */
                         int  missiles_firing ;                  /* ����� ����� �� ���� ���� */
                      double  firing_pause ;                     /* �������� ����� �������, ������ */
                         int  v_launch ;                         /* ������������ ���� */ 

                         int  start_flag ;                       /* ������� ������� ���� ������������� */

                      double  v_abs ;                            /* ���������� �������� ����� */
                         int  missiles_cnt ;                     /* ����� �����, ������� � ����� */
                         int  missiles_idx ;                     /* ������� ���������� ����� */
                      double  last_firing ;                      /* ����� ���������� ����� ��� */
        RSS_Object_ZRK_trace *firing_channel ;                   /* ������� ������� �����  */
                         int  stage ;                            /* ������ ������� ����� */
#define                              _SEARCH_STAGE  0
#define                        _FIRING_FIRST_STAGE  1
#define                         _FIRING_NEXT_STAGE  2

#define                              _TRACES_MAX  20
        RSS_Object_ZRK_trace  traces[_TRACES_MAX] ;              /* ��������� �������������� ����� */

   private:

         RSS_Object_ZRKTrace *mTrace ;                           /* ���������� */
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
         virtual        int  vCalculateShow (double, double) ;  /* ����������� ���������� ������� ��������� ��������� */
         virtual        int  vEvent         (char *, double) ;  /* ��������� ������� */
         virtual        int  vSpecial       (char *, void *) ;  /* ����������� �������� */
                        int  iSaveTracePoint(char *) ;          /* ���������� ����� ���������� */
                       void  iShowTrace_    (void) ;            /* ����������� ���������� � ��������� ��������� */
                       void  iShowTrace     (void) ;            /* ����������� ���������� */

	                     RSS_Object_ZRK() ;                     /* ����������� */
	                    ~RSS_Object_ZRK() ;                     /* ���������� */
                                                        } ;

/*------------------------ �������� ������ ���������� �������� "���" */

  class O_ZRK_API RSS_Module_ZRK : public RSS_Kernel {

    public:

     static
      struct RSS_Module_ZRK_instr *mInstrList ;             /* ������ ������ */

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
                     int  cTrace        (char *) ;                     /* ���������� TRACE */
                     int  cFiring       (char *) ;                     /* ���������� FIRING */

              RSS_Object *FindObject    (char *, int) ;                /* ����� ������� ���� ZRK �� ����� */

    public:
	                  RSS_Module_ZRK() ;                     /* ����������� */
	                 ~RSS_Module_ZRK() ;                     /* ���������� */
                                                       } ;

/*-------------------------------------------- ���������� ���������� */

 struct RSS_Module_ZRK_instr {

           char                       *name_full ;          /* ������ ��� ������� */
           char                       *name_shrt ;          /* �������� ��� ������� */
           char                       *help_row ;           /* HELP - ������ */
           char                       *help_full ;          /* HELP - ������ */
            int (RSS_Module_ZRK::*process)(char *) ;        /* ��������� ���������� ������� */
                             }  ;

/*--------------------------------------------- ���������� ��������� */

/* ����  O_ZRK.cpp */

/* ����  O_ZRK_dialog.cpp */
  INT_PTR CALLBACK  Object_ZRK_Help_dialog  (HWND, UINT, WPARAM, LPARAM) ;
  INT_PTR CALLBACK  Object_ZRK_Create_dialog(HWND, UINT, WPARAM, LPARAM) ;
  INT_PTR CALLBACK  Object_ZRK_Firing_dialog(HWND, UINT, WPARAM, LPARAM) ;
