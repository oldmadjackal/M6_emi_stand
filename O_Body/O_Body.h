
/********************************************************************/
/*								    */
/*		������ ���������� �������� "����"  		    */
/*								    */
/********************************************************************/

#ifdef O_BODY_EXPORTS
#define O_BODY_API __declspec(dllexport)
#else
#define O_BODY_API __declspec(dllimport)
#endif


/*---------------------------------------------- ��������� ��������� */

/*----------------------------------- �������� ������ ������� "����" */

  class O_BODY_API RSS_Object_Body : public RSS_Object {

    public:
                      char  model_path[FILENAME_MAX] ;        /* ���� ������ */

    public:
               virtual void  vFree          (void) ;          /* ���������� ������� */
               virtual void  vWriteSave     (std::string *) ; /* �������� ������ � ������ */

	                     RSS_Object_Body() ;              /* ����������� */
	                    ~RSS_Object_Body() ;              /* ���������� */
                                                       } ;

/*----------------------- �������� ������ ���������� �������� "����" */

  class O_BODY_API RSS_Module_Body : public RSS_Kernel {

    public:

     static
      struct RSS_Module_Body_instr *mInstrList ;          /* ������ ������ */
				     
    public:
     virtual  RSS_Object *vCreateObject (RSS_Model_data *) ; /* �������� ������� */ 
     virtual         int  vGetParameter (char *, char *) ;   /* �������� �������� */
     virtual         int  vExecuteCmd   (const char *) ;     /* ��������� ������� */
     virtual        void  vReadSave     (std::string *) ;    /* ������ ������ �� ������ */
     virtual        void  vWriteSave    (std::string *) ;    /* �������� ������ � ������ */

    public:
                     int  cHelp         (char *) ;           /* ���������� HELP */ 
                     int  cCreate       (char *) ;           /* ���������� CREATE */ 
                     int  cInfo         (char *) ;           /* ���������� INFO */ 
                     int  cVisible      (char *) ;           /* ���������� VISIBLE */
                     int  cBase         (char *) ;           /* ���������� BASE */ 
                     int  cAngle        (char *) ;           /* ���������� ANGLE */ 

         RSS_Object_Body *FindObject    (char *) ;           /* ����� ������� ���� BODY �� ����� */

    public:
	                  RSS_Module_Body() ;               /* ����������� */
	                 ~RSS_Module_Body() ;               /* ���������� */
                                                       } ;

/*-------------------------------------------- ���������� ���������� */

 struct RSS_Module_Body_instr {

           char                   *name_full ;         /* ������ ��� ������� */
           char                   *name_shrt ;         /* �������� ��� ������� */
           char                   *help_row ;          /* HELP - ������ */
           char                   *help_full ;         /* HELP - ������ */
            int (RSS_Module_Body::*process)(char *) ;  /* ��������� ���������� ������� */
                              }  ;

/*--------------------------------------------- ���������� ��������� */

/* ����  O_Body.cpp */

/* ����  O_Body_dialog.cpp */
  INT_PTR CALLBACK  Object_Body_Help_dialog  (HWND, UINT, WPARAM, LPARAM) ;
  INT_PTR CALLBACK  Object_Body_Create_dialog(HWND, UINT, WPARAM, LPARAM) ;
