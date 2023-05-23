
/********************************************************************/
/*								    */
/*		������ ���������� �������� "���-����"  		    */
/*								    */
/********************************************************************/

#ifdef O_ROCKET_LEGO_EXPORTS
#define O_ROCKET_LEGO_API __declspec(dllexport)
#else
#define O_ROCKET_LEGO_API __declspec(dllimport)
#endif


/*---------------------------------------------- ��������� ��������� */

/*--------------------- �������� ������ ��������� ������� "���-����" */

  class O_ROCKET_LEGO_API RSS_Transit_RocketLego : public RSS_Transit {

    public:
             virtual   int  vExecute(void) ;             /* ���������� �������� */
                                             
    public:
                            RSS_Transit_RocketLego() ;   /* ����������� */
                           ~RSS_Transit_RocketLego() ;   /* ���������� */

                                                               } ;

/*------------------------------- �������� ������ ������� "���-����" */

 struct RSS_Object_RocketLegoTrace {
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

  class O_ROCKET_LEGO_API RSS_Object_RocketLego : public RSS_Object {

    public:
                      char   model_path[FILENAME_MAX] ;        /* ���� ������ */

          RSS_Unit_WarHead  *unit_warhead ;                    /* ���������� */
           RSS_Unit_Engine  *unit_engine ;
            RSS_Unit_Model  *unit_model ;

                RSS_Object **mSpawn ;                          /* ������ ������ ��� �������� ����� */
                       int   mSpawn_cnt ;

                      HWND  hDropsViewWnd ;                    /* ���� ������������� ����� �������/������������ */

    public:

     RSS_Object_RocketLegoTrace *mTrace ;                      /* ���������� */
                            int  mTrace_cnt ;  
                            int  mTrace_max ;
                       COLORREF  mTrace_colors[32] ;
                            int  mTrace_colors_cnt ;
                            int  mTrace_width ; 
                            int  mTrace_dlist ;

    public:
         virtual       void  vFree          (void) ;                  /* ���������� ������� */
         virtual RSS_Object *vCopy          (char *) ;                /* ���������� ������ */
         virtual       void  vWriteSave     (std::string *) ;         /* �������� ������ � ������ */
         virtual        int  vCalculateStart(double) ;                /* ���������� ������� ��������� ��������� */
         virtual        int  vCalculate     (double, double,          /* ������ ��������� ��������� */
                                                     char *, int) ;
         virtual        int  vCalculateShow (double, double) ;        /* ����������� ���������� ������� ��������� ��������� */
         virtual        int  vSpecial       (char *, void *) ;        /* ����������� �������� */
                        int  iSaveTracePoint(char *) ;                /* ���������� ����� ���������� */
                       void  iShowTrace_    (char *) ;                /* ����������� ���������� � ��������� ��������� */
                       void  iShowTrace     (char *) ;                /* ����������� ���������� */
                       void  iClearSpawn    (void) ;                  /* ������������ �������� ������������� ����� */

	                     RSS_Object_RocketLego() ;          /* ����������� */
	                    ~RSS_Object_RocketLego() ;          /* ���������� */
                                                        } ;

/*------------------- �������� ������ ���������� �������� "���-����" */

  class O_ROCKET_LEGO_API RSS_Module_RocketLego : public RSS_Kernel {

    public:

     static
      struct RSS_Module_RocketLego_instr *mInstrList ;      /* ������ ������ */

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
                     int  cLego         (char *) ;                     /* ���������� LEGO */ 
                     int  cTrace        (char *) ;                     /* ���������� TRACE */
                     int  cSpawn        (char *) ;                     /* ���������� SPAWN */
                     int  cStat         (char *) ;                     /* ���������� STAT */

              RSS_Object *FindObject    (char *, int) ;                /* ����� ������� �� ����� */
          class RSS_Unit *AddUnit       (RSS_Object_RocketLego *,      /* ���������� ���������� � ������� */
                                             char *, char *, char * ) ;

    public:
	                  RSS_Module_RocketLego() ;              /* ����������� */
	                 ~RSS_Module_RocketLego() ;              /* ���������� */
                                                       } ;

/*-------------------------------------------- ���������� ���������� */

 struct RSS_Module_RocketLego_instr {

        char                      *name_full ;             /* ������ ��� ������� */
        char                      *name_shrt ;             /* �������� ��� ������� */
        char                      *help_row ;              /* HELP - ������ */
        char                      *help_full ;             /* HELP - ������ */
         int (RSS_Module_RocketLego::*process)(char *) ;   /* ��������� ���������� ������� */
                                 }  ;

/*--------------------------------------------- ���������� ��������� */

/* ����  O_Rocket_Lego.cpp */

/* ����  O_Rocket_Lego_dialog.cpp */
  INT_PTR CALLBACK  Object_RocketLego_Help_dialog  (HWND, UINT, WPARAM, LPARAM) ;
  INT_PTR CALLBACK  Object_RocketLego_Create_dialog(HWND, UINT, WPARAM, LPARAM) ;
  INT_PTR CALLBACK  Object_RocketLego_Lego_dialog  (HWND, UINT, WPARAM, LPARAM) ;
  INT_PTR CALLBACK  Object_RocketLego_Drops_dialog (HWND, UINT, WPARAM, LPARAM) ;
