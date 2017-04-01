/********************************************************************/
/*								    */
/*		МОДУЛЬ СТАНДАРТНОГО ВЫЧИСЛИТЕЛЯ        	            */
/*								    */
/********************************************************************/

#ifdef C_STD_EXPORTS
#define C_STD_API __declspec(dllexport)
#else
#define C_STD_API __declspec(dllimport)
#endif


/*---------------------------------------------- Параметры генерации */

/*----------------------------------------------- Описание контекста */

 typedef struct {  
                   struct L_module  module_decl ; 

                               int  compiled ;
 
                }  RSS_Calc_Std_context ;

/*------------------------- Описание класса стандартного вычислителя */

  class C_STD_API RSS_Calc_Std : public RSS_Kernel {

    public:
     virtual         int  vCalculate  (char *, char *,         /* Вычислить выражение */                                       
                                       struct RSS_Parameter *,
                                       struct RSS_Parameter *,
                                       double *,
                                       void **, char *) ;

    public:
     virtual        void  vStart      (void) ;                 /* Стартовая разводка */

                    void  iDebug      (char *, char *, int) ;  /* Отладочная печать в файл */ 
                    void  iDebug      (char *, char *) ;

	                  RSS_Calc_Std() ;                     /* Конструктор */
	                 ~RSS_Calc_Std() ;                     /* Деструктор */
                                                   } ;

/*---------------------------------------------------- Прототипы п/п */

  void  RSS_Calc_Std_text_norm(char *) ; /* Нормализация строки выражения */
  char *RSS_Calc_Std_get_text (void) ;   /* Процедура построчной выдачи текста компилятору */
  char *RSS_Calc_Std_error    (int) ;    /* Выдача текста ошибки по коду */
  char *RSS_Calc_Std_warning  (int) ;    /* Выдача текста предупреждения по коду */
  void  RSS_Calc_Std_debug    (int) ;    /* Процедура отладки вычислителя */
