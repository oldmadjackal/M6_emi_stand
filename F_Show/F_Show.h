/********************************************************************/
/*								    */
/*		������ ���������� ������������ ��������	            */
/*								    */
/********************************************************************/

#ifdef F_SHOW_EXPORTS
#define F_SHOW_API __declspec(dllexport)
#else
#define F_SHOW_API __declspec(dllimport)
#endif


/*---------------------------------------------- ��������� ��������� */

/*----------------------- �������� ������ ���������� �������� "����" */

  class F_SHOW_API RSS_Module_Show : public RSS_Kernel {

    public:

     static
      struct RSS_Module_Show_instr *mInstrList ;          /* ������ ������ */
				     
    public:
     virtual         int  vGetParameter (char *, char *) ;  /* �������� �������� */
     virtual        void  vStart        (void) ;            /* ��������� �������� */
     virtual RSS_Feature *vCreateFeature(RSS_Object *,      /* ������� �������� */
                                         RSS_Feature *) ;
     virtual        void  vShow         (char *) ;          /* ���������� ��������� ������ */
     virtual         int  vExecuteCmd   (const char *) ;    /* ��������� ������� */
     virtual        void  vWriteSave    (std::string *) ;   /* �������� ������ � ������ */

    public:
                     int  cHelp         (char *) ;          /* ���������� Help */ 
                     int  cColor        (char *) ;          /* ���������� Color */
                     int  cVisible      (char *) ;          /* ���������� Visible */

                     int  iShowScene    (void) ;            /* ����������� ����� */

	                  RSS_Module_Show() ;               /* ����������� */
	                 ~RSS_Module_Show() ;               /* ���������� */
                                                       } ;

/*-------------------------------------------- ���������� ���������� */

 struct RSS_Module_Show_instr {

           char                   *name_full ;         /* ������ ��� ������� */
           char                   *name_shrt ;         /* �������� ��� ������� */
           char                   *help_row ;          /* HELP - ������ */
           char                   *help_full ;         /* HELP - ������ */
            int (RSS_Module_Show::*process)(char *) ;  /* ��������� ���������� ������� */
                              }  ;

/*---------------------------------------------- ���������� �������� */

 typedef struct {                                           /* �������� ����� */
                    double  x ;                              /* X - �������� */
                      char *x_formula ;                      /* X - ���������� ��������� */
                      void *x_calculate ;                    /* X - �������� ����������� */

                    double  y ;                              /* Y - �������� */
                      char *y_formula ;                      /* Y - ���������� ��������� */
                      void *y_calculate ;                    /* Y - �������� ����������� */

                    double  z ;                              /* Z - �������� */
                      char *z_formula ;                      /* Z - ���������� ��������� */
                      void *z_calculate ;                    /* Z - �������� ����������� */
                }  RSS_Feature_Show_Vertex ;

#define  _VERTEX_PER_FACET_MAX   32

 typedef struct {                                           /* �������� ����� */
                    int  vertexes[_VERTEX_PER_FACET_MAX] ;
                    int  vertexes_cnt ;
                }  RSS_Feature_Show_Facet ;

 typedef struct {                                            /* �������� ���� */
                               char  name[128] ;              /* �������� */
               struct RSS_Parameter *extrn_pars ;             /* ������ �� ��������� */

                                int  Visible ;                /* ��������� */
                           COLORREF  Color ;                  /* ���� */
                           
            RSS_Feature_Show_Vertex *Vertexes ;               /* ������ ������ */
                                int  Vertexes_cnt ; 
             RSS_Feature_Show_Facet *Facets ;                 /* ������ ������ */
                                int  Facets_cnt ;

                             double  x_base ;                 /* ���������� ������� ����� */
                             double  y_base ;
                             double  z_base  ;
                             double  x_base_s ;
                             double  y_base_s ;
                             double  z_base_s ;

                             double  a_azim ;                 /* ���� ���������� */
                             double  a_elev ;
                             double  a_roll  ;
                             double  a_azim_s ;
                             double  a_elev_s ;
                             double  a_roll_s ;

                             double  Matrix[4][4] ;           /* ������� ��������� ����� */
                                int  Matrix_flag ;            /* ���� �������������... */
      
                                int  list_idx ;               /* ������ ����������� ������ */
                }  RSS_Feature_Show_Body ;


/*----------------------------------- �������� ������ �������� "���" */

  class F_SHOW_API RSS_Feature_Show : public RSS_Feature {

    public:       
                                int  Visible ;                /* ��������� */
                           COLORREF  Color ;                  /* ���� */

              RSS_Feature_Show_Body *Bodies ;                 /* ������ ��� */
                                int  Bodies_cnt ;

               struct RSS_Parameter *Pars_work ;

    public:
            virtual void  vReadSave     (char *, std::string *,     /* ������� ������ �� ������ */
                                                        char * ) ;
            virtual void  vGetInfo      (std::string *) ;           /* ������ ���������� � �������� */
            virtual  int  vParameter    (char *, char *, char *) ;  /* ������ � ����������� */  
            virtual void  vBodyDelete   (char *) ;                  /* ������� ���� */
            virtual void  vBodyBasePoint(char *,                    /* ������� ������� ����� ���� */
                                         double, double, double) ;
            virtual void  vBodyAngles   (char *,                    /* ������� ���������� ���� */
                                         double, double, double) ;
            virtual void  vBodyMatrix   (char *, double[4][4]) ;    /* ������� ������� ��������� ����� */
            virtual void  vBodyShifts   (char *,                    /* ������� �������� ��������� �  */
                                          double, double, double,   /*    ���������� ����            */
                                          double, double, double ) ;
            virtual void  vBodyPars     (char *,                    /* ������� ������ ���������� */
                                             struct RSS_Parameter *) ;

                    void  Show          (void) ;                    /* ���������� ������ */

                    void  iCalcNormal   (RSS_Feature_Show_Vertex *, /* ��������� ������� � ��������� */
                                         RSS_Feature_Show_Vertex *,   
                                         RSS_Feature_Show_Vertex *,   
                                         RSS_Feature_Show_Vertex * ) ;
              RSS_Kernel *iGetCalculator(void) ;                    /* ����������� ������� ����������� */ 

	                  RSS_Feature_Show() ;                /* ����������� */
	                 ~RSS_Feature_Show() ;                /* ���������� */
                                                         } ;

/*--------------------------------------------- ���������� ��������� */

/* ����  F_Show.cpp */

/* ����  F_Show_dialog.cpp */
  INT_PTR CALLBACK  Feature_Show_Help_dialog(HWND, UINT, WPARAM, LPARAM) ;
