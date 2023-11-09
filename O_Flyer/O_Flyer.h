
/********************************************************************/
/*								    */
/*		������ ���������� �������� "�����"  		    */
/*								    */
/********************************************************************/

#ifdef O_FLYER_EXPORTS
#define O_FLYER_API __declspec(dllexport)
#else
#define O_FLYER_API __declspec(dllimport)
#endif

/*---------------------------------------------- ��������� ��������� */

/*------------------------ �������� ������ ��������� ������� "�����" */

  class O_FLYER_API RSS_Transit_Flyer : public RSS_Transit {

    public:
             virtual   int  vExecute(void) ;             /* ���������� �������� */
                                             
    public:
                            RSS_Transit_Flyer() ;        /* ����������� */
                           ~RSS_Transit_Flyer() ;        /* ���������� */

                                                           } ;

/*------------------------------------ �������� ��������� ���������� */

 struct RSS_Object_FlyerPFrame {
                                      int  wait_flag ;
                                      int  battle_flag ;
                                     char  used[128] ;
                                   double  t ;
                                      int  dt_flag ;
                                   double  x, w_x ;
                                   double  y, w_y ;
                                   double  z, w_z ;
                                   double  a, w_a, d_a, t_a ;
                                   double  e, w_e, d_e, t_e ;
                                   double  r, w_r, d_r, t_r ;
                                   double  g, w_g, d_g, t_g ;
                                   double  v, w_v, d_v, t_v ;
                               } ;

#define  _PFRAMES_MAX   100
 class RSS_Object_FlyerProgram {

    public:
                                  char  name[128] ;
                RSS_Object_FlyerPFrame  frames[_PFRAMES_MAX] ;
                                   int  frames_cnt ;

    public:
                            RSS_Object_FlyerProgram() ;        /* ����������� */
                           ~RSS_Object_FlyerProgram() ;        /* ���������� */
                               } ;

/*-------------------------------------- �������� ������� ���������� */

 struct RSS_Object_FlyerEvent {
                                 char  name[64] ;
                                 char  program[64] ;
                              } ;
/*---------------------------------- �������� ������ ������� "�����" */

 struct RSS_Object_FlyerTrace {
                                  double  x_base ;
                                  double  y_base ;
                                  double  z_base ;

                                  double  a_azim ;
                                  double  a_elev ;
                                  double  a_roll ;

                                  double  x_velocity ;
                                  double  y_velocity ;
                                  double  z_velocity ;
                              } ;

  class O_FLYER_API RSS_Object_Flyer : public RSS_Object {

    public:
                      char  model_path[FILENAME_MAX] ;        /* ���� ������ */

                    double  v_abs ;                           /* ���������� �������� */
                    double  v_abs_stack ;
                    double  g_ctrl ;                          /* ���������� ����������� ���������� */
                    double  g_ctrl_stack ;

#define   _EVENTS_MAX  10
     RSS_Object_FlyerEvent  events[_EVENTS_MAX] ;             /* ������ ������� */

#define   _PROGRAMS_MAX  10
   RSS_Object_FlyerProgram *programs[_PROGRAMS_MAX] ;         /* ������ �������� ���������� */
   RSS_Object_FlyerProgram *program ;                         /* ������� ��������� ���������� */
                       int  p_frame ;
                    double  p_start ;                         /* ����� ������� ��������� */

    RSS_Object_FlyerPFrame  p_controls ;                      /* ����������� ����������� ��������� */
                    double  x_prv, y_prv, z_prv,              /* �������� ������������ ���������� */
                            a_prv, e_prv, r_prv,
                            g_prv, v_prv ;
                    double  t_prv ;

                       int  trace_on ;                        /* ������� ���������� ����������� */
                    double  trace_time ;                      /* ����������� ����� ����������� */

                      char  battle_cb[8000] ;                 /* ����� ��� ������ �������� ����� */

                    double  r_ctrl ;                          /* ��������� ��������� �������� */
            class Matrix2d *m_ctrl ;
                    double  a_ctrl ;

                       int  mTrace_flag ;                     /* ���� ����������� ���������� */
     RSS_Object_FlyerTrace *mTrace ;                          /* ���������� */
                       int  mTrace_cnt ;  
                       int  mTrace_max ;
                  COLORREF  mTrace_color ;
                       int  mTrace_dlist ;

                      char  stream_path[FILENAME_MAX] ;        /* ���� ���������� */

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
         virtual        int  vEvent         (char *, double,    /* ��������� ������� */
                                                     char *, int) ;
         virtual        int  vSpecial       (char *, void *) ;  /* ����������� �������� */
                        int  iExecuteProgram(double, double) ;  /* ��������� ������������ ���������� */
                        int  iSaveTracePoint(char *) ;          /* ���������� ����� ���������� */
                       void  iShowTrace_    (void) ;            /* ����������� ���������� � ��������� ��������� */
                       void  iShowTrace     (void) ;            /* ����������� ���������� */

	                     RSS_Object_Flyer() ;               /* ����������� */
	                    ~RSS_Object_Flyer() ;               /* ���������� */
                                                        } ;

/*---------------------- �������� ������ ���������� �������� "�����" */

  class O_FLYER_API RSS_Module_Flyer : public RSS_Kernel {

    public:

     static
      struct RSS_Module_Flyer_instr *mInstrList ;           /* ������ ������ */

                  double  g_step ;                          /* ��� ��������� ����������� ���������� */
                  double  a_step ;                          /* ��� ��������� ����� */

    public:
     virtual  RSS_Object *vCreateObject (RSS_Model_data *) ; /* �������� ������� */ 
     virtual         int  vGetParameter (char *, char *) ;   /* �������� �������� */
     virtual         int  vExecuteCmd   (const char *) ;     /* ��������� ������� */
     virtual        void  vReadSave     (std::string *) ;    /* ������ ������ �� ������ */
     virtual        void  vWriteSave    (std::string *) ;    /* �������� ������ � ������ */

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
                     int  cProgram      (char *) ;                     /* ���������� PROGRAM */
                     int  cEvent        (char *) ;                     /* ���������� EVENT */
                     int  cUnit         (char *) ;                     /* ���������� UNIT */
                     int  cPath         (char *) ;                     /* ���������� PATH */
                     int  cTrace        (char *) ;                     /* ���������� TRACE */
                     int  cStream       (char *) ;                     /* ���������� STREAM */

        RSS_Object_Flyer *FindObject    (char *, int) ;                /* ����� ������� ���� BODY �� ����� */
                     int  iReadProgram  (RSS_Object_Flyer *, char *) ; /* ���������� ����� �������� ��������� */
          class RSS_Unit *AddUnit       (RSS_Object_Flyer *,           /* ���������� ���������� � ������� */
                                             char *, char *, char * ) ;

    public:
	                  RSS_Module_Flyer() ;              /* ����������� */
	                 ~RSS_Module_Flyer() ;              /* ���������� */
                                                       } ;

/*-------------------------------------------- ���������� ���������� */

 struct RSS_Module_Flyer_instr {

           char                   *name_full ;          /* ������ ��� ������� */
           char                   *name_shrt ;          /* �������� ��� ������� */
           char                   *help_row ;           /* HELP - ������ */
           char                   *help_full ;          /* HELP - ������ */
            int (RSS_Module_Flyer::*process)(char *) ;  /* ��������� ���������� ������� */
                               }  ;

/*--------------------------------------------- ���������� ��������� */

/* ����  O_Flyer.cpp */

/* ����  O_Flyer_dialog.cpp */
  INT_PTR CALLBACK  Object_Flyer_Help_dialog  (HWND, UINT, WPARAM, LPARAM) ;
  INT_PTR CALLBACK  Object_Flyer_Create_dialog(HWND, UINT, WPARAM, LPARAM) ;
  INT_PTR CALLBACK  Object_Flyer_Units_dialog (HWND, UINT, WPARAM, LPARAM) ;
