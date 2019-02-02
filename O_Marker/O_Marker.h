
/********************************************************************/
/*								    */
/*		������ ���������� �������� "������"  		    */
/*								    */
/********************************************************************/

#ifdef O_MARKER_EXPORTS
#define O_MARKER_API __declspec(dllexport)
#else
#define O_MARKER_API __declspec(dllimport)
#endif


/*---------------------------------------------- ��������� ��������� */

/*----------------------- �������� ������ ��������� ������� "������" */

  class O_MARKER_API RSS_Transit_Marker : public RSS_Transit {

    public:
             virtual   int  vExecute(void) ;             /* ���������� �������� */
                                             
    public:
                            RSS_Transit_Marker() ;       /* ����������� */
                           ~RSS_Transit_Marker() ;       /* ���������� */

                                                             } ;

/*--------------------------------- �������� ������ ������� "������" */

  class O_MARKER_API RSS_Object_Marker : public RSS_Object {

    public:
                      char  model[64] ;                 /* ������ ������� */
                  COLORREF  color ;                     /* ���� ������ */
                       int  dlist1_idx ;                /* ������� ���������� ������� */
                       int  dlist2_idx ;
                    double  size ;                      /* ����.������� ������� */   

    public:
               virtual void  vWriteSave      (std::string *) ;       /* �������� ������ � ������ */
               virtual  int  vListControlPars(RSS_ControlPar *) ;    /* �������� ������ ���������� ���������� */
               virtual  int  vSetControlPar  (RSS_ControlPar *) ;    /* ���������� �������� ��������� ���������� */
                        int  iFormMarker     (void) ;                /* ������� ����� ������� */
                        int  iFormMarker_    (void) ;                /* ������� ����� ������� � ��������� ��������� */
                        int  iPlaceMarker    (void) ;                /* ���������������� ������� */
                        int  iPlaceMarker_   (void) ;                /* ���������������� ������� � ��������� ��������� */

	                     RSS_Object_Marker() ;                   /* ����������� */
	                    ~RSS_Object_Marker() ;                   /* ���������� */
                                                           } ;

/*--------------------- �������� ������ ���������� �������� "������" */

  class O_MARKER_API RSS_Module_Marker : public RSS_Kernel {

    public:

     static
      struct RSS_Module_Marker_instr *mInstrList ;           /* ������ ������ */

                   RSS_Object_Marker *mContextObject ;       /* ������ '�����������' �������� */
                                char  mContextAction[64] ;   /* �������� '�����������' �������� */
                          
                              double  mSize ;                /* ����.������� ������� */   

    public:
     virtual  RSS_Object *vCreateObject (RSS_Model_data *) ; /* �������� ������� */ 
     virtual         int  vGetParameter  (char *, char *) ;  /* �������� �������� */
     virtual        void  vShow          (char *) ;          /* ���������� ��������� ������ */
     virtual         int  vExecuteCmd    (const char *) ;    /* ��������� ������� */
     virtual        void  vReadSave      (std::string *) ;   /* ������ ������ �� ������ */
     virtual        void  vWriteSave     (std::string *) ;   /* �������� ������ � ������ */

    public:
                     int  cHelp       (char *) ;             /* ���������� HELP */ 
                     int  cCreate     (char *) ;             /* ���������� CREATE */ 
                     int  cInfo       (char *) ;             /* ���������� INFO */ 
                     int  cBase       (char *) ;             /* ���������� BASE */ 
                     int  cAngle      (char *) ;             /* ���������� ANGLE */ 

       RSS_Object_Marker *FindObject  (char *) ;             /* ����� ������� ���� MARKER �� ����� */

    public:
	                  RSS_Module_Marker() ;              /* ����������� */
	                 ~RSS_Module_Marker() ;              /* ���������� */
                                                           } ;

/*-------------------------------------------- ���������� ���������� */

 struct RSS_Module_Marker_instr {

           char                     *name_full ;         /* ������ ��� ������� */
           char                     *name_shrt ;         /* �������� ��� ������� */
           char                     *help_row ;          /* HELP - ������ */
           char                     *help_full ;         /* HELP - ������ */
            int (RSS_Module_Marker::*process)(char *) ;  /* ��������� ���������� ������� */
                                }  ;

/*--------------------------------------------- ���������� ��������� */

/* ����  O_Marker.cpp */

/* ����  O_Marker_dialog.cpp */
  INT_PTR CALLBACK  Object_Marker_Help_dialog(HWND, UINT, WPARAM, LPARAM) ;
