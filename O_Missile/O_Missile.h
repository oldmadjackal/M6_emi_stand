
/********************************************************************/
/*								    */
/*		������ ���������� �������� "������"  		    */
/*								    */
/********************************************************************/

#ifdef O_MISSILE_EXPORTS
#define O_MISSILE_API __declspec(dllexport)
#else
#define O_MISSILE_API __declspec(dllimport)
#endif


/*---------------------------------------------- ��������� ��������� */

/*----------------------- �������� ������ ��������� ������� "������" */

  class O_MISSILE_API RSS_Transit_Missile : public RSS_Transit {

    public:
             virtual   int  vExecute(void) ;             /* ���������� �������� */
                                             
    public:
                            RSS_Transit_Missile() ;        /* ����������� */
                           ~RSS_Transit_Missile() ;        /* ���������� */

                                                               } ;

/*--------------------------------- �������� ������ ������� "������" */

 struct RSS_Object_MissileTrace {
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

  class O_MISSILE_API RSS_Object_Missile : public RSS_Object {

    public:
                      char  model_path[FILENAME_MAX] ;        /* ���� ������ */

                    double  v_abs ;                           /* ���������� �������� */
                    double  g_ctrl ;                          /* ���������� ����������� ���������� */

                       int  homing_type ;                     /* ��� ������������� */
#define                              _AHEAD_HOMING    0       /*   ��������� � ����������� ����� (�� ���������) */
#define                       _PROPORTIONAL_HOMING    1       /*   ��������� ������� ����������������� ��������� � �������� ������������� */
                    double  homing_koef ;                     /* ����������� ����������������� ��������� */

                    double  mark_hit ;                        /* ������ ������� ������ ��� ��������� */

                       int  xyz_trg_prv ;                     /* ���� ������� ������ �� ����������� ��������� ���� */ 
                    double  x_trg_prv ;                       /* ���������� ����������� �� ���� */
                    double  y_trg_prv ;
                    double  z_trg_prv ;

                       int  g_over ;                          /* ������ � ������������ ����������� */
                    double  t_lost ;                          /* ������ ������� ������ ���� */

   public:

     RSS_Object_MissileTrace *mTrace ;                        /* ���������� */
                         int  mTrace_cnt ;  
                         int  mTrace_max ;
                    COLORREF  mTrace_color ;
                    COLORREF  mTrace_color_over ;
                         int  mTrace_width ; 
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
         virtual        int  vEvent         (char *, double,    /* ��������� ������� */
                                                     char *, int) ;
         virtual        int  vSpecial       (char *, void *) ;  /* ����������� �������� */
                        int  iSaveTracePoint(char *) ;          /* ���������� ����� ���������� */
                       void  iShowTrace_    (void) ;            /* ����������� ���������� � ��������� ��������� */
                       void  iShowTrace     (void) ;            /* ����������� ���������� */

	                     RSS_Object_Missile() ;             /* ����������� */
	                    ~RSS_Object_Missile() ;             /* ���������� */
                                                        } ;

/*--------------------- �������� ������ ���������� �������� "������" */

  class O_MISSILE_API RSS_Module_Missile : public RSS_Kernel {

    public:

     static
      struct RSS_Module_Missile_instr *mInstrList ;         /* ������ ������ */

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
                     int  cHoming       (char *) ;                     /* ���������� HOMING */
                     int  cTrace        (char *) ;                     /* ���������� TRACE */
                     int  cMark         (char *) ;                     /* ���������� MARK */

              RSS_Object *FindObject    (char *, int) ;                /* ����� ������� ���� BODY �� ����� */

    public:
	                  RSS_Module_Missile() ;              /* ����������� */
	                 ~RSS_Module_Missile() ;              /* ���������� */
                                                       } ;

/*-------------------------------------------- ���������� ���������� */

 struct RSS_Module_Missile_instr {

           char                      *name_full ;          /* ������ ��� ������� */
           char                      *name_shrt ;          /* �������� ��� ������� */
           char                      *help_row ;           /* HELP - ������ */
           char                      *help_full ;          /* HELP - ������ */
            int (RSS_Module_Missile::*process)(char *) ;   /* ��������� ���������� ������� */
                                 }  ;

/*--------------------------------------------- ���������� ��������� */

/* ����  O_Missile.cpp */

/* ����  O_Missile_dialog.cpp */
  INT_PTR CALLBACK  Object_Missile_Help_dialog  (HWND, UINT, WPARAM, LPARAM) ;
  INT_PTR CALLBACK  Object_Missile_Create_dialog(HWND, UINT, WPARAM, LPARAM) ;
