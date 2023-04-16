
/********************************************************************/
/*								    */
/*		������ ���������� �������� "�����"                  */
/*								    */
/********************************************************************/

#ifdef E_BLAST_EXPORTS
#define E_BLAST_API __declspec(dllexport)
#else
#define E_BLAST_API __declspec(dllimport)
#endif

/*---------------------------------------------- ��������� ��������� */


/*------------------------ �������� ������ ��������� ������� "�����" */

  class E_BLAST_API RSS_Transit_Blast : public RSS_Transit {

    public:
             virtual   int  vExecute(void) ;             /* ���������� �������� */
                                             
    public:
                            RSS_Transit_Blast() ;        /* ����������� */
                           ~RSS_Transit_Blast() ;        /* ���������� */

                                                            } ;

/*---------------------------------- �������� ������ ������� "�����" */

  class E_BLAST_API RSS_Effect_Blast : public RSS_Object {

    public:
                        int  blast_type ;                       /* ��� ������ */
#define                        _GROUND_BLAST  0                  /*  �������� ����� */
#define                           _AIR_BLAST  1                  /*  ��������� ����� */

                     double  hit_range ;                        /* ������ ��������� */

                     double  range_max ;                        /* ������������ ������ ������� */
                     double  range ;                            /* ������� ������ ������� */

                     double  t_0 ;                              /* ����� ������� */

                   COLORREF  color ;                            /* ���� ������ */
                        int  dlist1_idx ;                       /* ������� ���������� ������� */
                        int  dlist2_idx ;

                        int  blast_step ;                       /* ������ ������ */

    public:
               virtual void  vFree          (void) ;                 /* ���������� ������� */
               virtual void  vWriteSave     (std::string *) ;        /* �������� ������ � ������ */
               virtual  int  vCalculateStart(double) ;               /* ���������� ������� ��������� ��������� */
               virtual  int  vCalculate     (double, double,         /* ������ ��������� ��������� */
                                                     char *, int) ;
               virtual  int  vCalculateShow (double, double) ;       /* ����������� ���������� ������� ��������� ��������� */
               virtual  int  vEvent         (char *, double,         /* ��������� ������� */
                                                     char *, int) ;
               virtual  int  vSpecial       (char *, void *) ;       /* ����������� �������� */

                        int  iShowBlast     (void) ;                 /* ��������� ������� */
                        int  iShowBlast_    (void) ;                 /* ��������� ������� � ��������� ��������� */
                       void  iShowCone      (double, double, int, COLORREF) ;
                       void  iShowSphere    (double, int, COLORREF) ;

	                     RSS_Effect_Blast() ;                    /* ����������� */
	                    ~RSS_Effect_Blast() ;                    /* ���������� */
                                                        } ;

/*---------------------- �������� ������ ���������� �������� "�����" */

  class E_BLAST_API RSS_Module_Blast : public RSS_Kernel {

    public:

     static
      struct RSS_Module_Blast_instr *mInstrList ;          /* ������ ������ */

    public:
     virtual  RSS_Object *vCreateObject (RSS_Model_data *) ; /* �������� ������� */ 
     virtual         int  vGetParameter (char *, char *) ;   /* �������� �������� */
     virtual        void  vShow         (char *) ;           /* ���������� ��������� ������ */
     virtual         int  vExecuteCmd   (const char *) ;     /* ��������� ������� */
     virtual        void  vReadSave     (std::string *) ;    /* ������ ������ �� ������ */
     virtual        void  vWriteSave    (std::string *) ;    /* �������� ������ � ������ */

    public:
                     int  cHelp         (char *) ;           /* ���������� HELP */ 
                     int  cCreate       (char *) ;           /* ���������� CREATE */ 
                     int  cInfo         (char *) ;           /* ���������� INFO */ 
                     int  cRange        (char *) ;           /* ���������� RANGE */

              RSS_Object *FindObject    (char *, int) ;      /* ����� ������� ���� BLAST �� ����� */

    public:
	                  RSS_Module_Blast() ;               /* ����������� */
	                 ~RSS_Module_Blast() ;               /* ���������� */
                                                       } ;

/*-------------------------------------------- ���������� ���������� */

 struct RSS_Module_Blast_instr {

           char                    *name_full ;          /* ������ ��� ������� */
           char                    *name_shrt ;          /* �������� ��� ������� */
           char                    *help_row ;           /* HELP - ������ */
           char                    *help_full ;          /* HELP - ������ */
            int (RSS_Module_Blast::*process)(char *) ;   /* ��������� ���������� ������� */
                               }  ;

/*--------------------------------------------- ���������� ��������� */

/* ����  E_Blast.cpp */

/* ����  E_Blast_dialog.cpp */
  INT_PTR CALLBACK  Effect_Blast_Help_dialog(HWND, UINT, WPARAM, LPARAM) ;
