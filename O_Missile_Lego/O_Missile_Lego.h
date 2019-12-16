
/********************************************************************/
/*								    */
/*		������ ���������� �������� "������-����"            */
/*								    */
/********************************************************************/

#ifdef O_MISSILE_LEGO_EXPORTS
#define O_MISSILE_LEGO_API __declspec(dllexport)
#else
#define O_MISSILE_LEGO_API __declspec(dllimport)
#endif


/*---------------------------------------------- ��������� ��������� */

/*------------------ �������� ������ ��������� ������� "������-����" */

  class O_MISSILE_LEGO_API RSS_Transit_MissileLego : public RSS_Transit {

    public:
             virtual   int  vExecute(void) ;             /* ���������� �������� */
                                             
    public:
                            RSS_Transit_MissileLego() ;   /* ����������� */
                           ~RSS_Transit_MissileLego() ;   /* ���������� */

                                                               } ;

/*---------------------------- �������� ������ ������� "������-����" */

 struct RSS_Object_MissileLegoTrace {
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

  class O_MISSILE_LEGO_API RSS_Object_MissileLego : public RSS_Object {

    public:
                      char   model_path[FILENAME_MAX] ;        /* ���� ������ */

          RSS_Unit_WarHead  *unit_warhead ;                    /* ���������� */
           RSS_Unit_Homing  *unit_homing ;
           RSS_Unit_Engine  *unit_engine ;
          RSS_Unit_Control  *unit_control ;
            RSS_Unit_Model  *unit_model ;

                RSS_Object **mSpawn ;                          /* ������ ������ ��� �������� ����� */
                       int   mSpawn_cnt ;

                      HWND  hDropsViewWnd ;                    /* ���� ������������� ����� �������/������������ */

   private:

    RSS_Object_MissileLegoTrace *mTrace ;                      /* ���������� */
                            int  mTrace_cnt ;  
                            int  mTrace_max ;
                       COLORREF  mTrace_color ;
                            int  mTrace_dlist ;

    public:
         virtual       void  vFree          (void) ;            /* ���������� ������� */
         virtual RSS_Object *vCopy          (char *) ;          /* ���������� ������ */
         virtual       void  vWriteSave     (std::string *) ;   /* �������� ������ � ������ */
         virtual        int  vCalculateStart(double) ;          /* ���������� ������� ��������� ��������� */
         virtual        int  vCalculate     (double, double,    /* ������ ��������� ��������� */
                                                     char *, int) ;
         virtual        int  vCalculateShow (double, double) ;  /* ����������� ���������� ������� ��������� ��������� */
         virtual        int  vSpecial       (char *, void *) ;  /* ����������� �������� */
                        int  iSaveTracePoint(char *) ;          /* ���������� ����� ���������� */
                       void  iShowTrace_    (char *) ;          /* ����������� ���������� � ��������� ��������� */
                       void  iShowTrace     (char *) ;          /* ����������� ���������� */
                       void  iClearSpawn    (void) ;            /* ������������ �������� ������������� ����� */

	                     RSS_Object_MissileLego() ;          /* ����������� */
	                    ~RSS_Object_MissileLego() ;          /* ���������� */
                                                        } ;

/*---------------- �������� ������ ���������� �������� "������-����" */

  class O_MISSILE_LEGO_API RSS_Module_MissileLego : public RSS_Kernel {

    public:

     static
      struct RSS_Module_MissileLego_instr *mInstrList ;      /* ������ ������ */

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
                     int  cLego         (char *) ;                     /* ���������� LEGO */ 
                     int  cTrace        (char *) ;                     /* ���������� TRACE */
                     int  cSpawn        (char *) ;                     /* ���������� SPAWN */
                     int  cStat         (char *) ;                     /* ���������� STAT */

              RSS_Object *FindObject    (char *, int) ;                /* ����� ������� �� ����� */
          class RSS_Unit *AddUnit       (RSS_Object_MissileLego *,     /* ���������� ���������� � ������� */
                                             char *, char *, char * ) ;

    public:
	                  RSS_Module_MissileLego() ;              /* ����������� */
	                 ~RSS_Module_MissileLego() ;              /* ���������� */
                                                       } ;

/*-------------------------------------------- ���������� ���������� */

 struct RSS_Module_MissileLego_instr {

        char                          *name_full ;          /* ������ ��� ������� */
        char                          *name_shrt ;          /* �������� ��� ������� */
        char                          *help_row ;           /* HELP - ������ */
        char                          *help_full ;          /* HELP - ������ */
         int (RSS_Module_MissileLego::*process)(char *) ;   /* ��������� ���������� ������� */
                                 }  ;

/*--------------------------------------------- ���������� ��������� */

/* ����  O_Missile_Lego.cpp */

/* ����  O_Missile_Lego_dialog.cpp */
  INT_PTR CALLBACK  Object_MissileLego_Help_dialog  (HWND, UINT, WPARAM, LPARAM) ;
  INT_PTR CALLBACK  Object_MissileLego_Create_dialog(HWND, UINT, WPARAM, LPARAM) ;
  INT_PTR CALLBACK  Object_MissileLego_Lego_dialog  (HWND, UINT, WPARAM, LPARAM) ;
