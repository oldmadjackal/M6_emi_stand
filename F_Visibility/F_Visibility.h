/********************************************************************/
/*                                                                  */
/*               ������ ���������� ���������� ��������              */
/*                                                                  */
/********************************************************************/

#ifdef F_VISIBILITY_EXPORTS
#define F_VISIBILITY_API __declspec(dllexport)
#else
#define F_VISIBILITY_API __declspec(dllimport)
#endif

/*---------------------------------------------- ��������� ��������� */

/*---------------- �������� ������ ���������� �������� "�����������" */

  class F_VISIBILITY_API RSS_Module_Visibility : public RSS_Kernel {

    public:

     static
      struct RSS_Module_Visibility_instr *mInstrList ;          /* ������ ������ */
				     
    public:
     virtual         int  vExecuteCmd   (const char *) ;    /* ��������� ������� */

    public:
                     int  cHelp         (char *) ;          /* ���������� Help */
                     int  cRadius       (char *) ;          /* ���������� Radius */
                     int  cCategory     (char *) ;          /* ���������� Category */

    public:
     virtual        void  vStart        (void) ;            /* ��������� �������� */
     virtual RSS_Feature *vCreateFeature(RSS_Object *,      /* ������� �������� */
                                         RSS_Feature *) ;
     virtual        void  vWriteSave    (std::string *) ;   /* �������� ������ � ������ */
     virtual        void  vReadSave     (std::string *) ;   /* ������� ������ �� ������ */

	                  RSS_Module_Visibility() ;              /* ����������� */
	                 ~RSS_Module_Visibility() ;              /* ���������� */
                                                            } ;

/*-------------------------------------------- ���������� ���������� */

 struct RSS_Module_Visibility_instr {

           char                         *name_full ;         /* ������ ��� ������� */
           char                         *name_shrt ;         /* �������� ��� ������� */
           char                         *help_row ;          /* HELP - ������ */
           char                         *help_full ;         /* HELP - ������ */
            int (RSS_Module_Visibility::*process)(char *) ;  /* ��������� ���������� ������� */
                                    }  ;

/*---------------------------------------------- ���������� �������� */

 typedef struct {                                            /* �������� ����������� �������������� */
                             double  x_min ;
                             double  y_min ;
                             double  z_min ;
                             double  x_max ;
                             double  y_max ;
                             double  z_max ;
                }  RSS_Feature_Visibility_Dim ;

 typedef struct {                                            /* �������� ����� */
                    double  x ;                               /* X - �������� */
                      char *x_formula ;                       /* X - ���������� ��������� */
                      void *x_calculate ;                     /* X - �������� ����������� */

                    double  y ;                               /* Y - �������� */
                      char *y_formula ;                       /* Y - ���������� ��������� */
                      void *y_calculate ;                     /* Y - �������� ����������� */

                    double  z ;                               /* Z - �������� */
                      char *z_formula ;                       /* Z - ���������� ��������� */
                      void *z_calculate ;                     /* Z - �������� ����������� */

                    double  x_abs ;                           /* ���������� � ���������� �� */
                    double  y_abs ;
                    double  z_abs ;
                }  RSS_Feature_Visibility_Vertex ;

#define  _VERTEX_PER_FACET_MAX   32

 typedef struct {                                            /* �������� ����� */

                       int  vertexes[_VERTEX_PER_FACET_MAX] ; /* ������ ����� */
                       int  vertexes_cnt ;

     RSS_Feature_Visibility_Dim  overall ;                    /* ������� ����� */

                }  RSS_Feature_Visibility_Facet ;

 typedef struct {                                            /* �������� ���� */
                               char  name[128] ;              /* �������� */
               struct RSS_Parameter *extrn_pars ;             /* ������ �� ��������� */

      RSS_Feature_Visibility_Vertex *Vertexes ;               /* ������ ������ */
                                int  Vertexes_cnt ; 
       RSS_Feature_Visibility_Facet *Facets ;                 /* ������ ������ */
                                int  Facets_cnt ;

         RSS_Feature_Visibility_Dim  overall ;                /* ������� ���� */

                                int  recalc ;                 /* ���� ������������� ����������� ����� ���� */

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
                }  RSS_Feature_Visibility_Body ;

/*----------------------------- �������� ������ �������� "���������" */

  class F_VISIBILITY_API RSS_Feature_Visibility : public RSS_Feature {

    public:       
                      static    int  *hit_count ;                   /* ������� ��������� */
                                int   hit_done ;                    /* ������� ��������� */

    public:       
        RSS_Feature_Visibility_Body  *Bodies ;                      /* ������ ��� */
                                int   Bodies_cnt ;

               struct RSS_Parameter  *Pars_work ;

         RSS_Feature_Visibility_Dim   overall ;                     /* ������� ������� */

                          RSS_Point   track_s ;                     /* ����������� ��������� ������� */
                          RSS_Point   track_s_prv ;                 /* ����������� ��������� ���������� */
                                int   track_flag ;                  /* ���� ����������� �������� */

    public:
            virtual void  vReadSave     (char *, std::string *,     /* ������� ������ �� ������ */
                                                        char * ) ;
            virtual void  vGetInfo      (std::string *) ;           /* ������ ���������� � �������� */
            virtual  int  vResetCheck   (void *) ;                  /* ����� ��������� �������� ������������������ �������� */  
            virtual  int  vPreCheck     (void *) ;                  /* ���������� � �������� ������������������ �������� */  
            virtual  int  vCheck        (void *,                    /* �������� ������������������ �������� */  
                                         RSS_Objects_List *) ;
            virtual void  vBodyDelete   (char *) ;                  /* ������� ���� */
            virtual void  vBodyBasePoint(char *,                    /* ������� ������� ����� ���� */
                                         double, double, double) ;
            virtual void  vBodyAngles   (char *,                    /* ������� ���������� ���� */
                                         double, double, double) ;
            virtual void  vBodyMatrix   (char *, double[4][4]) ;    /* ������� ������� ��������� ���� */
            virtual void  vBodyShifts   (char *,                    /* ������� �������� ��������� �  */
                                          double, double, double,   /*    ���������� ����            */
                                          double, double, double ) ;
            virtual void  vBodyPars     (char *,                    /* ������� ������ ���������� */
                                             struct RSS_Parameter *) ;

                     int  RecalcPoints  (void) ;                    /* ���������� ����� ��� ������� */
              RSS_Kernel *iGetCalculator(void) ;                    /* ����������� ������� ����������� */
                     int  iOverallTest  (RSS_Feature_Visibility *) ;       /* �������� ����������� "���������" ����� */
                     int  iFacetsTest   (RSS_Feature_Visibility_Body *) ;  /* �������� ����������� ������ */
                     int  iToFlat       (RSS_Feature_Visibility_Vertex *,  /* ������ ������� ��������� ��������� x0z � ��������� ����� P0-P1-P2 */
                                         RSS_Feature_Visibility_Vertex *,
                                         RSS_Feature_Visibility_Vertex *,
                                                              Matrix2d * ) ;

	                  RSS_Feature_Visibility() ;                     /* ����������� */
	                 ~RSS_Feature_Visibility() ;                     /* ���������� */
                                                              } ;

/*--------------------------------------------- ���������� ��������� */

/* ����  F_Visibility.cpp */

/* ����  F_Visibility_dialog.cpp */
  INT_PTR CALLBACK  Feature_Visibility_Help_dialog(HWND, UINT, WPARAM, LPARAM) ;
