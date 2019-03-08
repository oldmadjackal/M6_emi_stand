/********************************************************************/
/*                                                                  */
/*        ������ ���������� ��������� � ������� ���������           */
/*                                                                  */
/********************************************************************/

#ifdef F_TERRAIN_EXPORTS
#define F_TERRAIN_API __declspec(dllexport)
#else
#define F_TERRAIN_API __declspec(dllimport)
#endif

/*---------------------------------------------- ��������� ��������� */

/*---------------- �������� ������ ���������� �������� "�����������" */

  class F_TERRAIN_API RSS_Module_Terrain : public RSS_Kernel {

    public:

     static
      struct RSS_Module_Terrain_instr *mInstrList ;          /* ������ ������ */
				     
    public:
     virtual         int  vExecuteCmd   (const char *) ;    /* ��������� ������� */

    public:
                     int  cHelp         (char *) ;          /* ���������� Help */
                     int  cSlave        (char *) ;          /* ���������� Slave */
                     int  cFlat         (char *) ;          /* ���������� Flat */
                     int  cLink         (char *) ;          /* ���������� Link */
                     int  cScan         (char *) ;          /* ���������� Scan */
                     int  cUp           (char *) ;          /* ���������� Up */
              RSS_Object *FindObject    (char *, int) ;     /* ����� ������� ��������� �� ����� */

    public:
     virtual        void  vStart        (void) ;            /* ��������� �������� */
     virtual RSS_Feature *vCreateFeature(RSS_Object *,      /* ������� �������� */
                                         RSS_Feature *) ;
     virtual        void  vWriteSave    (std::string *) ;   /* �������� ������ � ������ */
     virtual        void  vReadSave     (std::string *) ;   /* ������� ������ �� ������ */

	                  RSS_Module_Terrain() ;              /* ����������� */
	                 ~RSS_Module_Terrain() ;              /* ���������� */
                                                            } ;

/*-------------------------------------------- ���������� ���������� */

 struct RSS_Module_Terrain_instr {

           char                         *name_full ;         /* ������ ��� ������� */
           char                         *name_shrt ;         /* �������� ��� ������� */
           char                         *help_row ;          /* HELP - ������ */
           char                         *help_full ;         /* HELP - ������ */
            int (RSS_Module_Terrain::*process)(char *) ;  /* ��������� ���������� ������� */
                                    }  ;

/*---------------------------------------------- ���������� �������� */

 typedef struct {                                            /* �������� ����������� �������������� */
                             double  x_min ;
                             double  y_min ;
                             double  z_min ;
                             double  x_max ;
                             double  y_max ;
                             double  z_max ;
                }  RSS_Feature_Terrain_Dim ;

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
                }  RSS_Feature_Terrain_Vertex ;

#define  _VERTEX_PER_FACET_MAX   32

 typedef struct {                                            /* �������� ����� */

                       int  vertexes[_VERTEX_PER_FACET_MAX] ; /* ������ ����� */
                       int  vertexes_cnt ;
                    double  a ;                               /* ������������ ��������� ���������: Ax+By+Cz+D=0 */
                    double  b ;
                    double  c ;
                    double  d ;
                       int  abcd_formed ;

     RSS_Feature_Terrain_Dim  overall ;                    /* ������� ����� */

                }  RSS_Feature_Terrain_Facet ;

 typedef struct {                                            /* �������� ���� */
                               char  name[128] ;              /* �������� */
               struct RSS_Parameter *extrn_pars ;             /* ������ �� ��������� */

         RSS_Feature_Terrain_Vertex *Vertexes ;               /* ������ ������ */
                                int  Vertexes_cnt ; 
          RSS_Feature_Terrain_Facet *Facets ;                 /* ������ ������ */
                                int  Facets_cnt ;

            RSS_Feature_Terrain_Dim  overall ;                /* ������� ���� */

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
                }  RSS_Feature_Terrain_Body ;

/*----------------------------- �������� ������ �������� "���������" */

  class F_TERRAIN_API RSS_Feature_Terrain : public RSS_Feature {

    public:       
                RSS_Feature_Terrain  *terrain_last ;                /* ��������� ��������� ������� ��������� */
                                int   terrain_body ;                /* ... ������ ���� */
                                int   terrain_facet ;               /* ... ������ ����� */
                                int   slave ;                       /* ������� ���������� �� ������� */

                               char   link_object[128] ;            /* ��������� ������-������ */

    public:       
           RSS_Feature_Terrain_Body  *Bodies ;                      /* ������ ��� */
                                int   Bodies_cnt ;

               struct RSS_Parameter  *Pars_work ;

            RSS_Feature_Terrain_Dim   overall ;                     /* ������� ������� */

                          RSS_Point   track_s ;                     /* ����������� ��������� ������� */
                          RSS_Point   track_s_prv ;                 /* ����������� ��������� ���������� */
                                int   track_flag ;                  /* ���� ����������� �������� */

    public:
            virtual void  vReadSave     (char *, std::string *,          /* ������� ������ �� ������ */
                                                        char * ) ;
            virtual void  vGetInfo      (std::string *) ;                /* ������ ���������� � �������� */
            virtual  int  vResetCheck   (void *) ;                       /* ����� ��������� �������� ������������������ �������� */  
            virtual  int  vPreCheck     (void *) ;                       /* ���������� � �������� ������������������ �������� */  
            virtual  int  vCheck        (void *,                         /* �������� ������������������ �������� */  
                                         RSS_Objects_List *) ;
            virtual void  vBodyDelete   (char *) ;                       /* ������� ���� */
            virtual void  vBodyBasePoint(char *,                         /* ������� ������� ����� ���� */
                                         double, double, double) ;
            virtual void  vBodyAngles   (char *,                         /* ������� ���������� ���� */
                                         double, double, double) ;
            virtual void  vBodyMatrix   (char *, double[4][4]) ;         /* ������� ������� ��������� ���� */
            virtual void  vBodyShifts   (char *,                         /* ������� �������� ��������� �  */
                                          double, double, double,        /*    ���������� ����            */
                                          double, double, double ) ;
            virtual void  vBodyPars     (char *,                         /* ������� ������ ���������� */
                                             struct RSS_Parameter *) ;

                     int  RecalcPoints  (void) ;                         /* ���������� ����� ��� ������� */
              RSS_Kernel *iGetCalculator(void) ;                         /* ����������� ������� ����������� */
                     int  iOverallTest  (RSS_Feature_Terrain *) ;        /* �������� ��������� ����� � ������� ������� */
                     int  iFacetTest    (RSS_Feature_Terrain *,          /* �������� ��������� ����� ������ �������� ������� */
                                                           int, int) ;
                     int  iPlaceObject  (RSS_Feature_Terrain *,          /* ������ ��������� ������� �� �������� � �������� ��������� */
                                                           int, int) ;
                     int  iPlaneByPoints(RSS_Feature_Terrain_Facet *,    /* ����������� ������������� ��������� ��������� �� 3 ������ */
                                         RSS_Feature_Terrain_Vertex *,
                                         RSS_Feature_Terrain_Vertex *,
                                         RSS_Feature_Terrain_Vertex * ) ;

	                  RSS_Feature_Terrain() ;                          /* ����������� */
	                 ~RSS_Feature_Terrain() ;                          /* ���������� */
                                                              } ;

/*--------------------------------------------- ���������� ��������� */

/* ����  F_Terrain.cpp */

/* ����  F_Terrain_dialog.cpp */
  INT_PTR CALLBACK  Feature_Terrain_Help_dialog(HWND, UINT, WPARAM, LPARAM) ;
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                