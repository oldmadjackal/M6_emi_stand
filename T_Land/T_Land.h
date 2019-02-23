
/********************************************************************/
/*								    */
/*		������ ������ "�������� ���������"    		    */
/*								    */
/********************************************************************/

#ifdef T_LAND_EXPORTS
#define T_LAND_API __declspec(dllexport)
#else
#define T_LAND_API __declspec(dllimport)
#endif


/*---------------------------------------------- ��������� ��������� */

/*---------------------------------------------- ��������� ��������� */

#define  _EXIT_FRAME   -1
#define  _NEXT_FRAME    0
#define  _WAIT_FRAME    1

 typedef struct {                           /* �������� */
                     int  sync_type ;        /* ���� ���� ����� ������� */
#define                     _NO_SYNC  0
#define                      _T_SYNC  1
#define                     _DT_SYNC  2
                  double  t ;                /* ����� ������� */
                  double  dt ;
                    char  action[32] ;       /* �������� */
                    char  object[32] ;       /* ������ */
                    char  event[32] ;        /* ������� */
                     int  size ;             /* ������� ������� */
                  double  period ;           /* ������������� ���������� ������� */
                  double  t_par ;            /* ��������� �������� */

                    char  command[256] ;
              RSS_Object *templ ;
                     int  cnt ;
                  double  next_event ;
                } RSS_Module_Land_frame ;

#define  FRAME  RSS_Module_Land_frame

 typedef struct {                           /* �������� ���������� */
                   char  name[16] ;
                   char  value[256] ;
                } RSS_Module_Land_variable ;

#define  VAR  RSS_Module_Land_variable

 typedef struct {                           /* �������� ������� �������� */
                   RSS_Object *object ;
                          int  active ;
                       double  cut_time ;    /* ����� ������������ ������� */
                } RSS_Module_Land_object ;

#define  OBJ  RSS_Module_Land_object

 typedef struct {                           /* ���� ������� �� ����� */
                   char  object[64] ;
               COLORREF  color ;
                } RSS_Module_Land_color ;

#define  COLOR  RSS_Module_Land_color

 typedef struct {                           /* ����������� ������� �� ����� */
                   char  object[64] ;
              RSS_Point *points ;
                    int  points_max ;
                    int  points_cnt ;
                } RSS_Module_Land_trace ;

#define  TRACE  RSS_Module_Land_trace

/*----------- �������� ������ ��������� ������ "������ ������������" */

  class T_LAND_API RSS_Transit_Land : public RSS_Transit {

    public:
             virtual   int  vExecute(void) ;             /* ���������� �������� */

    public:
                            RSS_Transit_Land() ;  /* ����������� */
                           ~RSS_Transit_Land() ;  /* ���������� */

                                                               } ;

/*--------------------- �������� ������ ������ "������ ������������" */

#define  _OBJECTS_MAX   1000
#define   _SPAWNS_MAX    100

  class T_LAND_API RSS_Module_Land : public RSS_Kernel {

    public:

     static
          struct RSS_Module_Land_instr *mInstrList ;                /* ������ ������ */

                    static         OBJ  mObjects[_OBJECTS_MAX] ;
                    static         int  mObjects_cnt ;

                    static       FRAME *mSpawns[_SPAWNS_MAX] ;
                    static         int  mSpawns_cnt ;

                    static       FRAME *mScenario ;                 /* �������� ������ ������������ */
                    static         int  mScenario_cnt ;   

                    static         VAR *mVariables ;                /* ������ ���������� */
                    static         int  mVariables_cnt ;   
                    static        HWND  mVariables_Window ;

                    static         int  mHit_cnt ;                  /* ����� ��������� �������� */
    
                                  char  mContextAction[64] ;        /* �������� '�����������' �������� */

                    static   RSS_IFace  mExecIFace ;                /* ������������ ����� ���������� �������� */
                    static      double  mExecValue ;                /* �������� ��������� �������� */
                    static         int  mExecError ;                /* ������� ������ �������� */
                    static         int  mExecFail ;                 /* ������� ����������� ���������� ���������� ������ */

                    static        HWND  mMapWindow ;                /* ���� ���������� ����� */
                    static         int  mMapRegime ;                /* ����� ������ �����: */
#define                                    _MAP_KEEP_RANGE  1       /*   ��������� ������������ ����� */                                 
                    static      double  mMapXmin ;                  /* �������� ��������� ����� */
                    static      double  mMapXmax ;
                    static      double  mMapZmin ;
                    static      double  mMapZmax ;
                    static       COLOR  mMapColors[_OBJECTS_MAX] ;  /* ����� �������� �� ����� */
                    static         int  mMapColors_cnt ;
                    static       TRACE  mMapTraces[_OBJECTS_MAX] ;  /* ����� �������� �� ����� */
                    static         int  mMapTraces_cnt ;

    public:
                      RSS_Transit_Land *Context ;

    public:
     virtual         int  vExecuteCmd   (const char *) ;          /* ��������� ������� */
     virtual         int  vExecuteCmd   (const char *, RSS_IFace *) ;
     virtual        void  vReadSave     (std::string *) ;         /* ������ ������ �� ������ */
     virtual        void  vWriteSave    (std::string *) ;         /* �������� ������ � ������ */
     virtual         int  vGetParameter (char *, char *) ;        /* �������� �������� */
     virtual        void  vChangeContext(void)  ;                 /* ��������� �������� � ��������� ������ */

    public:
                     int  cHelp         (char *, RSS_IFace *) ;   /* ���������� HELP */
                     int  cClear        (char *, RSS_IFace *) ;   /* ���������� CLEAR */
                     int  cAdd          (char *, RSS_IFace *) ;   /* ���������� ADD */
                     int  cList         (char *, RSS_IFace *) ;   /* ���������� LIST */
                     int  cProgram      (char *, RSS_IFace *) ;   /* ���������� PROGRAM */
                     int  cRun          (char *, RSS_IFace *) ;   /* ���������� RUN */
                     int  cMap          (char *, RSS_IFace *) ;   /* ���������� MAP */

                     int  iFrameExecute (FRAME *, double, int) ;  /* ���������� ���������� ��������� ������ */

    public:
	                  RSS_Module_Land() ;               /* ����������� */
	                 ~RSS_Module_Land() ;               /* ���������� */
                                                             } ;

/*-------------------------------------------- ���������� ���������� */

 struct RSS_Module_Land_instr {

           char                   *name_full ;                      /* ������ ��� ������� */
           char                   *name_shrt ;                      /* �������� ��� ������� */
           char                   *help_row ;                       /* HELP - ������ */
           char                   *help_full ;                      /* HELP - ������ */
            int (RSS_Module_Land::*process)(char *, RSS_IFace *) ;  /* ��������� ���������� ������� */
                              }  ;
/*--------------------------------------------- ���������� ��������� */

/* ����  T_Land.cpp */

/* ����  T_Land_dialog.cpp */
  INT_PTR CALLBACK  Task_Land_Help_dialog(HWND, UINT, WPARAM, LPARAM) ;
  INT_PTR CALLBACK  Task_Land_View_dialog(HWND, UINT, WPARAM, LPARAM) ;
  INT_PTR CALLBACK  Task_Land_Vars_dialog(HWND, UINT, WPARAM, LPARAM) ;
  INT_PTR CALLBACK  Task_Land_Map_dialog (HWND, UINT, WPARAM, LPARAM) ;
  LRESULT CALLBACK  Task_Land_Map_prc    (HWND, UINT, WPARAM, LPARAM) ;
