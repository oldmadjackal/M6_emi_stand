/*----------------------------------------------- 'Внешняя' разводка */


#ifdef  _CCL_1_MAIN
#define  _EXTERNAL
#else
extern "C" {
#define  _EXTERNAL  extern
#endif

/*---------------------------------------------- Системные структуры */

   struct L_entry {                          /* Вход упр.кода */
		   char  name[_NAME_MAX+1] ;  /* Имя входа */
		    int  start ;              /* Номер начального кадра */
		    int  pars[50] ;           /* Список параметров: номеров внутр.переменных */
		    int  pars_cnt ;           /* Число параметров */
		  } ;

/*-------------------------------------- Основная система переменных */

#ifndef  _NAME_MAX
#define  _NAME_MAX  15
#endif

_EXTERNAL struct L_variable *lng_vars ;         /* Описание внешних переменных */
_EXTERNAL               int  lng_vars_cnt ;     /* Счетчик внешних переменных */
_EXTERNAL struct L_function *lng_func ;         /* Описание функций */
_EXTERNAL               int  lng_func_cnt ;     /* Счетчик функций */

/*--------------------------------- 'Расширенная' система переменных */

 struct L_table {
		     int  type ;      /* Тип списка */
		    void *objects ;   /* Список обьектов */
		} ;

_EXTERNAL struct L_table *lng_table ;  /* Таблица списков переменных */

/* Внимание! Таблица списков терминируется NULL-указателем,       */
/*             списки LANG стандарта - описанием с пустым именем, */
/*             а CCL и DCL стандартов - описанием с NULL именем.  */

/* Внимание! Вызов функций в стандарте CCL и DCL не поддерживается */

/*----------------------------------- Система динамической подгрузки */

_EXTERNAL void *(*lng_ext_lib)(int, char *) ; /* Функция внешней библиотеки */

/* Первым параметром пользовательской функции lng_ext_lib             */
/* является тип подгружаемого обьекта, а вторым - его символьное      */
/* имя. Если обьект найден - возвращается указатель на его описание,  */
/* а если нет - NULL.                                                 */

 _EXTERNAL     int   lng_ext_load_flag ;                      /* Флаг разрешения внешней подгрузки */

 _EXTERNAL  double (*lng_ext_exec)(char *, double *, int) ;   /* Функция внешней подгрузки */

/* Первым параметром пользовательской функции lng_ext_exec             */
/* является символьное имя требуемой процедуры, вторым - адрес таблицы */
/* параметров, третьим - число параметров. Возвращается нужное         */
/* числовое значение.                                                  */

 _EXTERNAL    void  *lng_ext_addr ;                           /* Адрес внешней подгрузки */

/*-------------------------------------------------- Система отладки */

 _EXTERNAL    int  lng_debug ;              /* Флаг отладки */

 _EXTERNAL  void (*lng_user_debug)(int) ;   /* Пользовательский вход отладки */

/* Первым параметром функции lng_user_debug передается номер кадра */
/* управляющего кода.                                              */

/*---------------------------------------------- Глобальное описание */

   struct L_module {                         /* Описание программного модуля */
		     void   *ctrl_buff ;         /* Буфер управляющего кода */
		      int    ctrl_size ;         /* Размер буфера управляющего кода */
		      int    ctrl_cnt ;          /* Размер управляющего кода, кадров */
		      int    ctrl_used ;         /* Исольз.размер буфера, байт */
		      int    ctrl_frame ;        /* Номер кадра управляющего кода */

		     void   *culc_buff ;         /* Буфер вычислительного кода */
		      int    culc_size ;         /* Размер буфера вычислительного кода */
		      int    culc_cnt ;          /* Размер вычислительного кода, кадров */
		      int    culc_used ;         /* Исольз.размер буфера, байт */
		      int    culc_frame ;        /* Номер кадра вычислительного кода */

		     char *(*get_text)(void) ;   /* Ссылка на п/п выдачи текста */
		      int  (*test_name)(int,     /* Ссылка на п/п контроля имен */
					char *) ;

	struct L_variable   *ext_vars ;          /* Описание внешних переменных */
		      int    ext_vars_cnt ;      /* Счетчик внешних переменных */
	struct L_function   *ext_func ;          /* Описание функций */
		      int    ext_func_cnt ;      /* Счетчик функций */
	   struct L_table   *ext_table ;         /* Таблица списков переменных */
		     void *(*ext_lib)(int,       /* Функция внешней библиотеки */
				      char *) ;
		   double  (*ext_exec)(char *,   /* Функция внешней подгрузки */
				       double *, int) ;

	struct L_variable   *int_vars ;          /* Массив внутренних переменных */
		      int    int_vars_max ;      /* Макс.число внутренних переменных */
		      int    int_vars_cnt ;      /* Счетчик внутренних переменных */
		   double   *work_vars ;         /* Массив рабочих ячеек */
		      int    work_vars_max ;     /* Макс.число рабочих ячеек */
		      int    work_vars_cnt ;     /* Счетчик рабочих ячеек */

	   struct L_entry   *entry_list ;
		      int    entry_max ;
		      int    entry_cnt ;
		   } ;
   
/*    Массивы int_vars и work_vars являются НЕПЕРЕМЕСТИМЫМИ!        */
/*    Максимальный размер массивов int_vars и work_vars не может    */
/*  превышать 255 ячеек для каждого и следовательно их максимальный */
/*  байтовый размер состовляет 6120 и 2040 соответственно.          */

/*------------------------------------- Описание ошибочных состояний */

 struct L_warning {
		     int  code ;  /* Код предупреждения */
		     int  row ;   /* Строка текста с предупреждением */
		  } ;

_EXTERNAL              int  lng_error ;      /* Индикатор ошибки */
_EXTERNAL             char *lng_bad ;        /* Указатель ошибочного места */
_EXTERNAL              int  lng_row  ;       /* Ошибочная строка текста */

#define       _LNG_WARN_MAX     100
_EXTERNAL struct L_warning  lng_warn[_LNG_WARN_MAX] ;  /* Массив индикаторов предупреждений */
_EXTERNAL              int  lng_warn_cnt ;             /* Счетчик предупреждений */


#define   _EL_GET_TEXT      999    /* Не задана процедура подачи текста */
#define   _EL_CTRL_BUFF       1    /* Переполнение буфера управл.кода */
#define   _EL_CULC_BUFF       2    /* Переполнение буфера счетного кода */
#define   _EL_CYR_CHAR        3    /* "Русский" символ в тексте */
#define   _EL_MARK_BAD        4    /* Некорректная или слишком длинная метка */
#define   _EL_MARK_DBL        5    /* Повторная метка */
#define   _EL_MARK_UNK        6    /* Неизвестная метка */
#define   _EL_END_MARK        7    /* Оператор END содержит метку */
#define   _EL_END_FILE        8    /* Отсутствует оператор END */
#define   _EL_NAME_LEN        9    /* Слишком длинное имя */
#define   _EL_NAME_EMPTY     10    /* Пустое имя */
#define   _EL_RET_OUT        11    /* В тексте отсутствует RETURN */
#define   _EL_RET_FMT        12    /* Некорректный формат оператора RETURN */
#define   _EL_RET_NAME       13    /* Парам. опер. RETURN - выражение или некорр.имя */
#define   _EL_RET_UNK        14    /* Имя RETURN-переменной неизвестно */
#define   _EL_IF_FMT         15    /* Некорректный формат оператора IF */
#define   _EL_ELSE_MARK      16    /* Оператор ELSE содержит метку */
#define   _EL_ELSE_UNK       17    /* ELSE без IF */
#define   _EL_BLOCK_UNK      18    /* Попытка закрыть неоткрытый блок */
#define   _EL_UNCLOSE        19    /* "Незакрытые" операторы IF или DO */
#define   _EL_DO_FMT         20    /* Некорректный формат оператора DO */
#define   _EL_EXT_CRC        21    /* BREAK или CONTINUE без цикла */
#define   _EL_TEXT_OPEN      24    /* Открытая текстовая константа */
#define   _EL_INV_CHAR       25    /* Некорректная Escape-последовательность */
#define   _EL_DECL_FMT       26    /* Некорректный формат описания */
#define   _EL_DECL_SIZE      27    /* Некорректный размер переменной */
#define   _EL_NAME_USED      28    /* Повторное использование имени */
#define   _EL_ENTRY_UNK      29    /* Неуказанный адрес ENTRY-входа */

#define   _EL_FUNC_BEFORE    31   /* Перед функцией непонятный символ */
#define   _EL_FUNC_FMT       32   /* Некоректный формат вызова функции */
#define   _EL_FUNC_SINTAX    34   /* Неизвестный или недопустимый параметр */
#define   _EL_FUNC_UNK       35   /* Неисвестное имя функции */
#define   _EL_OPEN           36   /* Лишние открывающие скобки */
#define   _EL_CLOSE          37   /* Лишние закрывающие скобки */
#define   _EL_EMPTY          38   /* Пустое выражение (подвыражение в скобках) */
#define   _EL_NAME_UNK       39   /* Неизвестное имя переменной */
#define   _EL_NAME_SINTAX    40   /* Нкорректное имя переменной */
#define   _EL_INDEX          41   /* Незакрытое индексное выражение */
#define   _EL_SINTAX1        51   /* Ошибка синтаксиса */
#define   _EL_SINTAX2        52   /* Ошибка синтаксиса */
#define   _EL_SINTAX3        53   /* Ошибка синтаксиса */
#define   _EL_SINTAX4        54   /* Ошибка синтаксиса */
#define   _EL_SINTAX5        55   /* Ошибка синтаксиса */

#define   _EL_STD_OUT        61   /* Вызов функций стандарта CCL или DCL */
#define   _EL_VARS_OUT       62   /* Число переменных, заданных через   */
				  /*   lng_vars не может быть более 255 */
#define   _EL_FUNC_OUT       63   /* Число функций, заданных через      */
				  /*   lng_func не может быть более 255 */

#define   _EL_LEN_MAX        91   /* Слишком длинная исходная строка */
#define   _EL_IFDO_MAX       93   /* Слишком глубокая вложенность операторов IF и DO */
#define   _EL_INT_VAR_MAX    94   /* Слишком много внутренних переменных */
#define   _EL_WRK_VAR_MAX    95   /* Слишком много рабочих ячеек в строке */
#define   _EL_PARS_MAX       96   /* Слишком много параметров */
#define   _EL_TEXT_MAX       97   /* Слишком много строчных констант */
#define   _EL_INDEX_MAX      98   /* Слишком много индексов */
#define   _EL_AREA_MAX       99   /* Переполнение буфера внутренних массивов */
#define   _EL_ENTRY_MAX     100   /* Переполнение списка входов */
#define   _EL_GOTO_OVR       22   /* Слишком много меток или GOTO-переходов */
#define   _EL_IFDO_OVR       23   /* Переполнение IFDO-стека */

#define   _EL_UNK_ENTRY     201   /* Неизвестный вход модуля */

#define   _WL_EXT_CHNG        1   /* Попытка изменения внешней переменной */
#define   _WL_FUNC_PARS      33   /* Несоответствии числа параметров функции */

/*--------------------------------------------- Системные переменные */

_EXTERNAL    char *(*L_get_text )(void) ;         /* Ссылка на п/п выдачи текста */
_EXTERNAL     int  (*L_test_name)(int, char *) ;  /* Ссылка на п/п контроля имени */
#define                  _L_POINTER_NAME   0

_EXTERNAL     int    lng_ctrl_abort ;             /* Флаг завершения исполнения всей программы */
_EXTERNAL     int    lng_culc_abort ;             /* Флаг сброса исполнения вычислительного фрагмента */

_EXTERNAL     int    lng_pars_cnt ;               /* Число переданных параметров */

_EXTERNAL    long    lng_ctrl_used ;              /* Размер управляющего кода */
_EXTERNAL    long    lng_culc_used ;              /* Размер вычислительного кода */

/*---------------------------------------------- Описание прототипов */

    int  L_compile      (struct L_module *) ;    /* Компиляция модуля */
 double  L_execute      (struct L_module *,      /* Исполнение модуля */
			  char *, double *, int) ;

   void  L_debug_set    (int) ;                  /* Вкл. режима отладки */
 double  L_set_pointer  (void *) ;               /* Кодирование адреса */
   void *L_get_pointer  (double) ;               /* Декодирование адреса */

    int  L_ctrl_buff    (char *, int) ;          /* Задание буфера управляющего кода */
    int  L_culc_buff    (char *, int) ;          /* Задание буфера счетного кода */
    int  L_area_buff    (char *, int) ;          /* Задание буфера внутренних массивов */
   void *L_int_vars     (int *) ;                /* Выдает адрес массива внутренних переменных */
    int  L_ctrl_maker   (void) ;                 /* Транслятор управляющего псевдокода */

    int  L_decl_maker   (char *) ;               /* Обработка описаний */
    int  L_ctrl_code    (int, double *, int) ;   /* Формирователь управляющего псевдокода */
   void  L_ied_close    (int) ;                  /* Закрытие IF, ELSE и DO операторов */
   char *L_equ_maker    (char *) ;               /* Обработка выражений присвоения */
    int  L_alloc_var    (char *) ;               /* Размещение внутренной переменной */

   char *L_culc_maker   (char *) ;               /* Транслятор вычислительного псевдокода */
   char *L_simple_string(char *) ;               /* Обработка простых строк */
   char *L_simple_unit  (char *) ;               /* Обработка одинарной константы или переменной */
    int  L_table_objects(int, char *) ;          /* Работа с обьектами внешней таблицы LNG_TABLE */
#define  _FIND_VAR   1
#define  _FIND_FUNC  2
    int  L_culc_code    (int, char *, char *) ;  /* Формирователь вычислительного псевдокода */
    int  L_escape_dcd   (char *, char *) ;       /* Обработка Escape-последовательностей */

    int  L_decl_entry   (char *, double *, int); /* 'Заявка' входа */
 double  L_ctrl_exec    (void) ;                 /* Исполнение управляющего псевдокода по 0-входу */
 double  L_culc_exec    (int) ;                  /* Исполняющий модуль вычислительного псевдокода */

#ifdef  _CCL_1_MAIN
#else
}
#endif
