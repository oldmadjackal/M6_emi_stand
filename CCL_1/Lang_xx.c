/*   Bolotov P.D.  version  24.06.1993   */
/*     2 warnings at warning level 1     */

#include  <stddef.h>
#include  <stdlib.h>
#include  <string.h>
#include  <ctype.h>
#include  <math.h>
#include  <malloc.h>


#define  _CCL_1_MAIN
#include "lang_std.h"
#include "ccl_1.h"

#pragma warning(disable : 4996)


/*********************************************************************/
/*                                                                   */
/*             РЕКУРСИВНЫЙ МНОГОСЕГМЕНТНЫЙ ВАРИАНТ                   */
/*                                                                   */
/*********************************************************************/


/*********************************************************************/
/*                                                                   */
/*                      В Н И М А Н И Е !!!                          */
/*                                                                   */
/*    Внутрисистемные ограничения при генарации :                    */
/*       Длина исходной строки      - 128   _LEN_MAX                 */
/*       Меток                      -  20   _GOTO_MAX                */
/*       GOTO переходов             -  20   _GOTO_MAX                */
/*       IF-DO вложенность          -  20   _IFDO_MAX                */
/*       Внутренних переменных      - 100   _INT_VAR_MAX             */
/*       Рабочих переменных         - 100   _WRK_VAR_MAX             */
/*       Переменных в выражении     -  50   _LINE_VARS_MAX           */
/*       Параметров функций         -  20   _PARS_MAX                */
/*       Строчных констант в строке -  20   _TEXT_MAX                */
/*       Индексов в строке          -  20   _INDEX_MAX               */
/*                                                                   */
/*  Соответствующие определения идут ниже.                           */
/*                                                                   */
/*********************************************************************/

/*---------------------------------------------- Параметры генерации */

//#define  __INSIDE_DEBUGER__

#define  _LEN_MAX            128
#define  _GOTO_MAX            20
#define  _IFDO_MAX            20
#define  _INT_VAR_MAX        100       /* Не более 255 */
#define  _WRK_VAR_MAX        100       /* Не более 255 */
#define  _LINE_VARS_MAX       50
#define  _PARS_MAX            20
#define  _INDEX_MAX           20
#define  _TEXT_MAX            20
#define  _TEXT_BUFF    (SYS_WRK_MAX*sizeof(double)/2)

/*--------------------------------------- Обьекты связи и управления */

#define  _QUIT  -1

/*--------------------------------------- Операции управляющего кода */

#define  _ENTRY_MAX   10
 static    struct L_entry  entry_list[_ENTRY_MAX] ;   /* Список входов */
 static               int  entry_cnt ;
 static               int  entry_max=_ENTRY_MAX ;


 struct L_control {                     /* Операция упр.кока */
		      int  oper_code ;   /* Код операции */
		   double *operand ;     /* Адрес операнда */
		      int  addr ;        /* Адрес кода */
		      int  text_row ;    /* Номер текстовой строки */
		  } ;

#define     _NOP_OPER  0
#define     _SET_OPER  1
#define    _CULC_OPER  2
#define      _IF_OPER  3
#define  _RETURN_OPER  4
#define   _ALLOC_OPER  5

/*------------------------------------ Операции вычислительного кода */

 struct L_culc {
		      int  oper_code ;  /* Код операции */
		   double *addr1 ;      /* Адрес 1-го операнда */
		   double *addr2 ;      /* Адрес 2-го операнда */
	       } ;

 struct L_oper {                      /* Описание операций */
		     char *name ;       /* Имя операции */
		      int  len ;        /* Длина имени операции */
		      int  code ;       /* Код операции */
		      int  pref_flag ;  /* Флаг префиксной операции */
	       } ;


#define _CULC_FRAME   sizeof(struct L_culc)

				/* Тип первого операнда:        */
#define   _PNTR_OP1      0      /*   Полный адрес операнда      */
#define   _DATA_OP1      1      /*   Непосредственное данное    */
#define   _SHFT_OP1      2      /*   Данное=База+Содержимое     */
#define   _LONG_OP1      4      /*   Длинное данное за командой */

#define   _MOVE_OPER  0010      /* Код операции д/б кратным 8 !!! */
#define   _PUSH_OPER  0020
#define   _CALL_OPER  0030
#define  _CALLX_OPER  0040
#define  _CALLE_OPER  0050
#define  _POWER_OPER  0060
#define   _MULT_OPER  0070
#define    _DEV_OPER  0100
#define   _DEVC_OPER  0110
#define   _DEVQ_OPER  0120
#define    _SUB_OPER  0130
#define    _ADD_OPER  0140
#define     _EQ_OPER  0150
#define     _NE_OPER  0160
#define     _LE_OPER  0170
#define     _GE_OPER  0200
#define     _LT_OPER  0210
#define     _GT_OPER  0220
#define    _AND_OPER  0230
#define     _OR_OPER  0240
#define   _TSEG_OPER  0250     /* Установить сегмент текста */
#define   _IND1_OPER  0260     /* Установить индекс 1-го операнда */
#define   _IND2_OPER  0270     /* Установить индекс 2-го операнда */
#define   _STOP_OPER  0770

#define  _OPER_MAX  16
 static  struct L_oper lng_oper[_OPER_MAX]={ "^",  1, _POWER_OPER, 0, /* Описание допустимых операций */
					     "%%", 2,  _DEVC_OPER, 0,
					     "%",  1,  _DEVQ_OPER, 0,
					     "/",  1,   _DEV_OPER, 0, /* Внимание! Приоритет деления должен */
					     "*",  1,  _MULT_OPER, 0, /*   быть больше приоритета умножения */
					     "-",  1,   _SUB_OPER, 1, /* Внимание! Приоритет вычитания должен */
					     "+",  1,   _ADD_OPER, 0, /*      быть больше приоритета сложения */
					     "==", 2,    _EQ_OPER, 0,
					     "!=", 2,    _NE_OPER, 0,
					     "<=", 2,    _LE_OPER, 0,
					     ">=", 2,    _GE_OPER, 0,
					     "<",  1,    _LT_OPER, 0,
					     ">",  1,    _GT_OPER, 0,
					     "&&", 2,   _AND_OPER, 0,
					     "||", 2,    _OR_OPER, 0,
					     "=",  1,  _MOVE_OPER, 0  } ;

/*------------------------------------ Операнды вычислительного кода */
/*                                                                   */
/*      {TNI}  -   T - тип обьекта                                   */
/*                 N - 'адрес' обьекта                               */
/*                 I - (>0) номер индексирующей ячейки               */
/*                                                                   */
/*   Тип обьекта :                                                   */
/*         F - функция                -  vars_func                   */
/*         P - табличная функция      -  lng_table, vars_tbl         */
/*         R - ENTRY-вход             -  entry_list                  */
/*         E - внешняя переменная     -  lng_vars                    */
/*         T - табличная переменная   -  lng_table, vars_tbl         */
/*         I - внутренняя переменная  -  SYS_INT_VARS                */
/*         W - рабочая переменная     -  vars_work                   */
/*         C - константа              -  vars_cnst                   */
/*         Z - нуль                                                  */

 static char  func[6]={'{', 'F', '0', '0', '}', 0} ;    /* Спецификация функции */
 static char  rslt[6]={'{', 'W', '0', '0', '}', 0} ;    /* Спецификация рабочей ячейки */
 static char  cnst[6]={'{', 'C', '0', '0', '}', 0} ;    /* Спецификация числовой константы */
 static char  ltrl[6]={'{', 'L', '0', '0', '}', 0} ;    /* Спецификация символьной константы */
 static char  zero[6]={'{', 'Z', '0', '0', '}', 0} ;    /* Спецификация нуля */

 static char *index_list[_INDEX_MAX] ;     /* Индексный список */
 static  int  index_cnt  ;                 /* Счетчик индексов */
 static char  index_buff[_INDEX_MAX*6] ;   /* Буфер индексного списка */

/*------------------------- Переменные транслятора управляющего кода */

 struct L_marker {                           /* Описание меток и переходов */
		   char  name[_NAME_MAX+1] ;  /* Имя метки */
		    int  row ;                /* Строка текста или кода */
		 } ;

 struct L_if_do {                       /* Описание условных и цикловых операторов */
		    int  if_do_flag ;    /* Флаг условного или циклового оператора */
		    int  else_flag ;     /* Флаг ELSE-оператора */
		    int  block_flag ;    /* Флаг работы с блочным оператором */
		    int  addr ;          /* Рабочий адрес */
		} ;

 static   struct L_marker  mark[_GOTO_MAX] ;    /* Описание меток */
 static               int  mark_cnt ;           /* Счетчик меток */
 static   struct L_marker  dir[_GOTO_MAX] ;     /* Описание переходов */
 static               int  dir_cnt ;            /* Счетчик переходов */

 static    struct L_if_do  id_stk[_IFDO_MAX] ;  /* Стек IF-DO операторов */
 static               int  id_stk_cnt ;         /* Указатель стека IF-DO операторов */
 static               int  id_num ;             /* Указатель "крайнего блока" IF_DO стека */

 static              char  buff[_LEN_MAX] ;     /* Рабочий буфер */
 static               int  row ;                /* Номер строки исходного текста */
 static               int  block_flag ;         /* Флаг работы внутри IF-DO блока */
 static               int  any_flag ;           /* Флаг наличия любых операторов кроме IF-ELSE и DO */
 static               int  else_flag ;          /* Флаг ожидания ELSE-оператора */

/*---------------------- Переменные транслятора вычислительного кода */

 static            double  shft_base ;                /* База для операндов типа _SHIFT_OP2 */
 static              char *text_buff ;                /* Рабочий буфер строчных констант */
 static               int  text_addr[_TEXT_MAX] ;     /* Таблица 'адресов' строчных констант */
 static               int  text_cnt ;                 /* Счетчик строчных констант */
 static               int  text_size ;                /* Счетчик заполнения строчного буфера */
 static               int  text_flag ;                /* Флаг работы со строчными константами */
 static              char  extrn_name[80] ;           /* Имя внешнего обьекта */
#define    _TEXT_BASE  sizeof(struct L_culc)

 static               int  func_flag ;        /* Флаг обнаружения функции */
 static               int  func_type ;        /* Тип функции: F, P, R */
 static               int  func_num ;         /* Идентификационный номер функции */
 static               int  func_std ;         /* Стандарт функции */
 static               int  func_pars_cnt ;    /* Число параметров функции */

/*----------------------------------------------------- Буфера кодов */

 static  struct L_control *ctrl_code ;         /* Управляющий псевдокод */
 static               int  ctrl_code_size ;    /* Размер буфера управляющего псевдокода */
 static               int  ctrl_code_cnt ;     /* Счетчик кадров управляющего псевдокода */
 static               int  ctrl_code_max ;     /* Макс.число кадров управляющего псевдокода */
 static     struct L_culc *culc_code ;         /* Счетный псевдокод */
 static               int  culc_code_size ;    /* Размер буфера вычислительного псевдокода */
 static               int  culc_code_cnt ;     /* Счетчик кадров счетного псевдокода */
 static               int  culc_code_max ;     /* Макс.число кадров счетного псевдокода */

/*-------------------------------------- Области размещения обьектов */

 static struct L_variable  vars_int[_INT_VAR_MAX] ;   /* Описание внутренних переменных */
 static               int  vars_int_cnt ;             /* Счетчик внутренних переменных */
 static            double  vars_work[_WRK_VAR_MAX] ;  /* Массив рабочих ячеек */
 static               int  vars_work_cnt ;            /* Счетчик рабочих ячеек */
 static            double *vars_cnst ;                /* Массив констант */
 static               int  vars_cnst_cnt ;            /* Счетчик констант */
 static            double  vars_tmp ;                 /* Внутренний накопитель */
 static              void *vars_tbl[_LINE_VARS_MAX] ; /* Адреса табличных переменных/функций */
 static               int  vars_tbl_cnt ;             /* Счетчик адреса табличных переменных/функций */

 static            double  vars_stack[_PARS_MAX] ;    /* Рабочий стек параметров */
 static               int  vars_stack_cnt ;           /* Счетчик рабочего стека */

 static            double *vars_iarea ;               /* Буфер размещения внутренних массивов */
 static               int  vars_iarea_size ;          /* Емкость буфера внутренних массивов */
 static               int  vars_iarea_cnt ;           /* Счетчик заполнения буфера внутренних массивов */

/*------------------------------------- Глобальное описание процесса */

	 struct L_module *lng_system_work ;  /* Описание текущего программного модуля */
	 struct L_module *lng_system ;       /* Описание программного модуля */
 static  struct L_module  system_default ;   /* ... по умолчанию */

#define  SYS_CTRL_CODE  ((struct L_control *)lng_system_work->ctrl_buff)
#define  SYS_CTRL_SIZE   lng_system_work->ctrl_size
#define  SYS_CTRL_MAX   (lng_system_work->ctrl_size/sizeof(struct L_control))
#define  SYS_CTRL_CNT    lng_system_work->ctrl_cnt
#define  SYS_CTRL_USED   lng_system_work->ctrl_used

#define  SYS_CULC_CODE  ((struct L_culc *)lng_system_work->culc_buff)
#define  SYS_CULC_SIZE   lng_system_work->culc_size
#define  SYS_CULC_MAX   (lng_system_work->culc_size/sizeof(struct L_culc))
#define  SYS_CULC_CNT    lng_system_work->culc_cnt
#define  SYS_CULC_USED   lng_system_work->culc_used

#define  SYS_INT_VARS    lng_system_work->int_vars
#define  SYS_INT_MAX     lng_system_work->int_vars_max
#define  SYS_INT_CNT     lng_system_work->int_vars_cnt

#define  SYS_WRK_VARS    lng_system_work->work_vars
#define  SYS_WRK_MAX     lng_system_work->work_vars_max
#define  SYS_WRK_CNT     lng_system_work->work_vars_cnt

#define  SYS_ENTRY_LIST  lng_system_work->entry_list
#define  SYS_ENTRY_MAX   lng_system_work->entry_max
#define  SYS_ENTRY_CNT   lng_system_work->entry_cnt

#define  SYS_EXT_VARS    lng_system_work->ext_vars
#define  SYS_EXT_VCNT    lng_system_work->ext_vars_cnt
#define  SYS_EXT_FUNC    lng_system_work->ext_func
#define  SYS_EXT_FCNT    lng_system_work->ext_func_cnt
#define  SYS_EXT_TABLE   lng_system_work->ext_table

#define  SYS_GET_TEXT   (lng_system_work->get_text)
#define  SYS_TEST_NAME  (lng_system_work->test_name)
#define  SYS_EXT_LIB    (lng_system_work->ext_lib)
#define  SYS_EXT_EXEC   (lng_system_work->ext_exec)

/*-------------------------------------------------- Параметры языка */

#define   LNG_CHAR(c)    (isalnum(c) || c=='_' || c=='$' || c=='[' || c==']')
#define   LNG_NAME(c)    (isalnum(c) || c=='_' || c=='$')

/*-------------------------------------------- Переменные процессора */

 static               int  lng_start ;       /* Стартовый адрес */

 static            double  vars_if_ind ;     /* Индикатор операции IF */
 static            double  vars_go_ind ;     /* Индикатор операции GOTO */

/*---------------------------------------------------- Прототипы п/п */

   char *L_find_close(char *) ;              /* Поиск закрывающей скобки */
 double  L_call_entry(int, double *, int) ;  /* Вызов внутреннего входа */
   void  L_debug_ctrl(int) ;                 /* Отладка управляющего кода */


/************************************************************************/
/************************************************************************/
/*                                                                      */
/*                 УПРАВЛЮЩИЕ И ЗАДАЮЩИЕ ПРОЦЕДУРЫ                      */
/*                                                                      */
/************************************************************************/
/************************************************************************/

/************************************************************************/
/*                                                                      */
/*                            Компиляция модуля                         */

  int  L_compile(system)

    struct L_module *system ;  /* Описание процесса */

{
  int  status ;


       lng_system=system ;

	   status=L_ctrl_maker() ;

  return(status) ;
}


/************************************************************************/
/*                                                                      */
/*                    Исполнение модуля                                 */

  double  L_execute(system, entry, pars, pars_cnt)

    struct L_module *system ;    /* Описание процесса */
	       char *entry ;     /* Имя входа */
	     double *pars ;      /* Таблица параметров */
		int  pars_cnt ;  /* Число параметров */

{
  double  value ;
     int  status ;


       lng_system_work=system ;

    if(entry!=NULL) {
		      status=L_decl_entry(entry, pars, pars_cnt) ;
		   if(status) {  lng_error=_EL_UNK_ENTRY ;
					 return(0.) ;       }
		    }

	   value=L_ctrl_exec() ;

  return(value) ;
}


/************************************************************************/
/*                                                                      */
/*                Задание рабочих буферов                               */

     int  L_ctrl_buff(addr, size)

      char *addr ;  /* Адрес буфера */
       int  size ;  /* Емкость буфера, байт */

{
     ctrl_code     =(struct L_control *)addr ;
     ctrl_code_size=size ;
     ctrl_code_max =size/sizeof(ctrl_code[0]) ;

 return(0) ;
}


     int  L_culc_buff(addr, size)

      char *addr ;  /* Адрес буфера */
       int  size ;  /* Емкость буфера, байт */

{
     culc_code    =(struct L_culc *)addr ;
     culc_code_max=size/sizeof(culc_code[0]) ;

 return(0) ;
}


     int  L_area_buff(addr, size)

      char *addr ;  /* Адрес буфера */
       int  size ;  /* Емкость буфера, байт */

{
     vars_iarea     =(double *)addr ;
     vars_iarea_size=size/sizeof(vars_iarea[0]) ;

 return(0) ;
}

/************************************************************************/
/*                                                                      */
/*              Выдача адреса массива внутренних переменных             */

    void *L_int_vars(name_cnt)

      int *name_cnt ;

{
   *name_cnt=vars_int_cnt ;
  return(vars_int) ;
}


/************************************************************************/
/************************************************************************/
/*                                                                      */
/*                       ТРАНСЛЯЦИОННЫЙ БЛОК                            */
/*                                                                      */
/************************************************************************/
/************************************************************************/


/************************************************************************/
/*                                                                      */
/*                Транслятор управляющего псевдокода                    */

  int  L_ctrl_maker(void)

{
       char  text_src[4096] ;     /* Рабочий буфер транслируемого текста */
       char *text ;
	int  string_rgm ;         /* Режим работы со строчной константой */
	int  mark_flag ;          /* Флаг наличия в сроке метки */
	int  ret_flag ;           /* Флаг наличия в операторов RETURN в тексте */
	int  close_flag ;
       char *subrow_beg[50] ;     /* Массив указателей на начало подстрок */
       char *subrow_end[50] ;     /* Массив указателей на конец подстрок */
	int  subrow_cnt ;         /* Счетчик подстрок */
	int  subrow ;             /* Номер текущей подстроки */
       char  chr_save ;           /* Ячейка сохранения символа */
       char *addr ;
       char *name ;
static char  do_slct[4]={'=', ';', ';', ')'} ;  /* "Точки рассечки" оператора DO */
       char *do_addr[4] ;                       /* Адреса "рассечки" оператора DO */
//   double *var_addr ;
//      int  var_num ;
	int  base ;               /* Базовый адрес */
	int  shift ;              /* Параметр сдвига */
	int  size ;
	int  len ;
	int  n ;
	int  i ;
	int  j ;

/*------------------------- Подготовка глобального описания процесса */

	if(lng_system==NULL)  lng_system     =&system_default ;
			      lng_system_work= lng_system ;
/*- - - - - - - - - - - - - - - - Совмещение со 'старым' обрамлением */
	       system_default.ctrl_buff    = ctrl_code ;
	       system_default.ctrl_size    = ctrl_code_size ;
	       system_default.culc_buff    = culc_code ;
	       system_default.culc_size    = culc_code_size ;

	       system_default.int_vars     = vars_int ;
	       system_default.int_vars_max =_INT_VAR_MAX ;
	       system_default.work_vars    = vars_work ;
	       system_default.work_vars_max=_WRK_VAR_MAX ;

	       system_default.entry_list   = entry_list ;
	       system_default.entry_max    = entry_max ;

	       system_default.ext_vars     =lng_vars ;
	       system_default.ext_vars_cnt =lng_vars_cnt ;
	       system_default.ext_func     =lng_func ;
	       system_default.ext_func_cnt =lng_func_cnt ;
	       system_default.ext_table    =lng_table ;

	       system_default.get_text     = L_get_text ;
	       system_default.test_name    = L_test_name ;
	       system_default.ext_lib      = lng_ext_lib ;
	       system_default.ext_exec     = lng_ext_exec ;

/*------------------------------------------------- Входной контроль */

    if(SYS_GET_TEXT==NULL) {   lng_error=_EL_GET_TEXT ;
				     return(_QUIT) ;     }
    if(SYS_EXT_FCNT > 255) {   lng_error=_EL_FUNC_OUT ;
				     return(_QUIT) ;     }
    if(SYS_EXT_FCNT > 255) {   lng_error=_EL_FUNC_OUT ;
				     return(_QUIT) ;     }

/*---------------------------------------------- Общая инициализация */

		vars_cnst=         SYS_WRK_VARS ;                   /* Адресация буферов числовых */
		text_buff=(char *)(SYS_WRK_VARS+SYS_WRK_MAX/2) ;    /*    и строчных констант     */

		lng_error=0 ;
	     lng_warn_cnt=0 ;
		 mark_cnt=0 ;
		  dir_cnt=0 ;
	       id_stk_cnt=0 ;
	    SYS_ENTRY_CNT=0 ;
	     SYS_CTRL_CNT=0 ;
	     SYS_CULC_CNT=0 ;
	   vars_iarea_cnt=0 ;

	       close_flag=0 ;
	       block_flag=0 ;
		else_flag=0 ;
		 ret_flag=0 ;

    for(i=0 ; i<_INDEX_MAX ; i++) index_list[i]=&index_buff[i*6] ;

/*------------------------------------------ Построчное сканирование */

  for(row=1 ; ; row++) {                                            /* CIRCLE.1 - Сканируем текст по строкам */
		      if(lng_error)  return(_QUIT) ;                /* "Собираем" возможные ошибки */

			  text=SYS_GET_TEXT() ;                     /* Получаем следующую строку текста */

		       if(text==NULL)  break ;                      /* При обнаружении конца текста - */
								    /*    - завершаем работу          */
             memset(text_src, 0, sizeof(text_src)) ;                /* Копируем входную строку в рабочий буфер */
            strncpy(text_src, text, sizeof(text_src)-1) ;
               text=text_src ;

			 lng_row=row ;
		       mark_flag= 0 ;                               /* Сброс флага метки */

/*-------------------------------------- Обработка строчных констант */

	       text_flag=0 ;                                        /* Сброс флага текстовых констант */
	       text_cnt =0 ;                                        /* Сброс счетчика текстовых констант */
	       text_size=0 ;                                        /* Сброс буфера текстовых констант */
	      string_rgm=0 ;                                        /* Иниц.регима обработки */

//           len=strlen(text) ;

     for(i=0 ; text[i] ; i++) {                                     /* CIRCLE.2 - Сканируем строку ... */
      if(text[i]=='"') {                                            /* Если селектор константы ... */
/*- - - - - - - - - - - - - - - - - - - - - - -  'Упаковка' констант */
	 if(string_rgm==text[i]) {                                  /* IF.1 - Если константа 'закрывается'... */

	     if(text[i-1]=='\\' && text[i-2]!='\\')  continue ;     /* Контроль на Escape-символ */

			       text[i]= 0 ;                         /* Терминируем константу */

	  if(text_cnt>=_TEXT_MAX) {  lng_error=_EL_TEXT_MAX ;       /* Контроль переполнения таблицы */
					   return(_QUIT) ;      }   /*    строчных констант          */

	 for(n=1, j=0 ; addr[n] ; n++, j++) {                      /* Перевод в бинарное представление */
	  if(addr[n]=='\\')  n+=L_escape_dcd(addr+n+1, addr+j) ;
	  else                       addr[j]=addr[n] ;
	       if(lng_error)  return(_QUIT) ;
					    }
			addr[j]=  0 ;                               /* Терминируем константную строку */
			     j++     ;
			     j+=(j&1) ;                             /* Обеспеч.четность размера */

	  if(text_size+j>=_TEXT_BUFF) {  lng_error=_EL_TEXT_MAX ;   /* Контроль переполнения буфера */
					   return(_QUIT) ;        } /*    строчных констант         */

		      text_addr[text_cnt]=_TEXT_BASE+text_size ;    /* Уст.адрес строчных констант */

		    memmove(text_buff+text_size, addr, j) ;         /* Копируем строку в буфер */

			    ltrl[2]=text_cnt+1 ;                    /* Форм.специф.символьной константы */
		    memmove(addr+5, text+i+1, strlen(text+i+1)+1) ; /* Поджимаем хвост выражения */
		    memmove(addr  , ltrl    ,  5     ) ;            /* Вставл.специф.символьной константы */

				      shift =text-addr+i-1 ;        /* Опр. сдвиг */
					  i-=shift ;                /* Корр.рабочего счетчика */
//              			len-=shift ;                /* Корр.длины фрагмента */

				  string_rgm = 0  ;                 /* Сброс регима строчной константы */
				   text_size+=j ;                   /* Модиф.счетчик буфера строчных констант */
				    text_cnt++ ;                    /* Модиф.счетчик строчных констант */
				 }                                  /* ELSE.1 */
/*- - - - - - - - - - - - - - - - - - - - Обнаружение входа констант */
      else if(string_rgm==0)  {                                     /* ELSE.1 - Если конст. 'открывается'... */
				     text_flag=1 ;                  /* Уст.флага текстовых констант */
				    string_rgm=text[i] ;            /* Уст.соотв.режим */
					  addr=text+i ;             /* Фиксируем адрес начала константы */
			      }                                     /* END.1 */
		       }
/*- - - - - - - - - - - - - - - - - - - - - -  Обработка комментария */
      else             {
			  if( text[i  ]=='/' &&                     /* Если признак комментария          */
			      text[i+1]=='/' &&                     /*  за пределами срочной константы - */
			     string_rgm==  0    )  break ;          /*   прекращаем обработку констант   */
		       }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
			      }                                     /* CONTINUE.2 */

       if(string_rgm!=0) {  lng_error=_EL_TEXT_OPEN ;               /* Контроль завершения работы */
				  return(_QUIT) ;      }            /*  со строчными константами  */

/*------------------------------- Подавление комментариев и пробелов */

   for(i=0, j=0 ; text[j] ; j++) {                                  /* Сканируем строку */
		       if(text[j  ]=='/' &&                         /*   Отсечение комментария */
			  text[j+1]=='/'   )  break ;

		       if(text[j]==' ' ||                           /*   Подавление пробелов и табуляций */
			  text[j]=='\t'  )  continue ;

                       if(text[j]<  0 ||
                          text[j]>127   ) { lng_error=_EL_CYR_CHAR; /*   При обнаружении русского символа - */
                                                 return(_QUIT) ;   }/*      - выход с ошибкой               */

			      text[i]=text[j] ;                     /*   Поджатие строки */
				   i++ ;
				 }

			  text[i]=0 ;                               /* Терминируем обработанную строку */

	       if(i>=_LEN_MAX) {  lng_error=_EL_LEN_MAX ;           /* Если строка слишком длинна - */
				       return(_QUIT) ;     }        /*    - выход с ошибкой         */

/*-------------------------------------------------- Выделение меток */

	addr=strchr(text, ':') ;                                    /* Ищем разделитель метки */

     if(addr!=NULL) {                                               /* IF.1 - Если подозревается метка ... */
		       len=addr-text ;                              /* Опред.длину метки */

       for(i=0 ; i<len; i++)                                        /* Контролир.состав символов */
		if(!LNG_CHAR(text[i])) break ;                      /*   предполагаемой метки    */

      if(i==len)                                                    /* IF.2 - Если это действит. метка ... */
	  if(len>_NAME_MAX) {                                       /* IF.3 - Если метка слишком длинная ... */
				lng_error=_EL_MARK_BAD ;            /*  Устанавливаем ошибку */
				    return(_QUIT) ;                 /*  Прекращаем обработку */
			    }
	  else              {                                       /* ELSE.3 - Если метка корректная ... */
				  text[len]=0 ;                     /*  Терминируем метку */

		for(i=0 ; i<mark_cnt ; i++)                         /*  Проверяем совпадение метки */
		    if(!strcmp(mark[i].name, text))  break ;        /*     с предыдущими метками   */

		 if(i<mark_cnt) {  lng_error=_EL_MARK_DBL ;         /*  При обнаружении одинаковых меток - */
					 return(_QUIT) ;    }       /*   - уст.ошибку и прекращ. обработку */

	     if(mark_cnt==_GOTO_MAX-1) {  lng_error=_EL_GOTO_OVR ;  /* Если слишком много меток - */
					       return(_QUIT) ;   }  /*    - выход с ошибкой       */

		  strcpy(mark[mark_cnt].name, text) ;               /*  Заносим метку в список */
			 mark[mark_cnt].row=SYS_CTRL_CNT ;
			      mark_cnt++ ;
			      mark_flag=1 ;                         /*  Уст.флаг наличия метки в строке */

		  strcpy(text, text+len+1) ;                        /*  "Ужимаем" метку */
			    }                                       /* END.3 */
								    /* END.2 */
		    }                                               /* END.1 */
/*----------------------------------- Специальная обработка описаний */

     if(!memicmp(text, "CHAR",   4) ||
	!memicmp(text, "DOUBLE", 6) ||
	!memicmp(text, "ENTRY",  5) ||
	!memicmp(text, "EXTERN", 6)   ) {
					  L_decl_maker(text) ;
			    if(lng_error)  return(_QUIT) ;

					   continue ;
				        }
/*------------------------------------------ Обработка оператора END */

     if(!stricmp(text, "END"))                                      /* IF.4 - При обнаруж. оператора END ... */
		 if(mark_flag) {                                    /* Если строка содержит метку -       */
				  lng_error=_EL_END_MARK ;          /*  - уст.ошибку и прекращ. обработку */
				  return(_QUIT) ;
			       }
		 else          {                                    /* Если оператор корректен... */
				  break ;
			       }                                    /* END.4 */
/*----------------------------------- Разделение строки на подстроки */

	 subrow_cnt=0 ;

  do {
	 if(subrow_cnt) subrow_end[subrow_cnt-1]=text ;
			subrow_beg[subrow_cnt  ]=text ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
	 if(!memicmp(text, "ELSE", 4)) {
					if(text[4]=='{')  text+=5 ;
					else              text+=4 ;
		 	 	       }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    else if(!memicmp(text, "IF(", 3)) {
				   addr=L_find_close(text+3) ;      /* Поиск закрывающей скобки оператора */
				if(addr[1]=='{')  text=addr+2 ;
				else              text=addr+1 ;
				      }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    else if(!memicmp(text, "DO(", 3)) {
				   addr=L_find_close(text+3) ;      /* Поиск закрывающей скобки оператора */
				if(addr[1]=='{')  text=addr+2 ;
				else              text=addr+1 ;
				      }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    else if(!memicmp(text, "FOR(", 4)) {
				   addr=L_find_close(text+4) ;      /* Поиск закрывающей скобки оператора */
				if(addr[1]=='{')  text=addr+2 ;
				else              text=addr+1 ;
				       }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    else if(!memicmp(text, "RETURN", 6)) {

      if(text[6]=='(') {
			     addr=L_find_close(text+7) ;            /* Поиск закрывающей скобки оператора */
			     text=addr+1 ;
		       }
      else                  text+=6 ;
					 }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    else if(*text=='}') {
			     text++ ;
			}
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    else                     break ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

	    subrow_cnt++ ;

     } while(1) ;

	subrow_end[subrow_cnt]=text+strlen(text) ;
		   subrow_cnt++ ;

/*---------------------------------------------------- Цикл подстрок */

  for(subrow=0 ; subrow<subrow_cnt ; subrow++) {                    /* CIRCLE.2 - Сканируем текст по подстрокам */

			text= subrow_beg[subrow] ;
	    if(subrow) *text= chr_save ;
		    chr_save=*subrow_end[subrow] ;
	 *subrow_end[subrow]=  0 ;

	    if(*text==0)  break ;

		      close_flag= 0 ;
			any_flag= 0 ;

/*----------------------------------------- Обработка оператора ELSE */

    if(!memicmp(text, "ELSE", 4)) {                                 /* IF.5 - Если обнар.оператор ELSE ... */

      if(!else_flag) {  lng_error=_EL_ELSE_UNK ;                    /* При "неожиданном" появлении - */
			      return(_QUIT) ;    }                  /*   уст.ошибку и прекращ.обраб. */

      if(mark_flag) {   lng_error=_EL_ELSE_MARK ;                   /* При некорректном формате -    */
			      return(_QUIT) ;    }                  /*   уст.ошибку и прекращ.обраб. */

	 if(text[4]=='{')  block_flag=1 ;                           /* Опред. наличие IF(ELSE)-блока */
	 else              block_flag=0 ;
			    else_flag=0 ;                           /* Сбрасываем ELSE-флаг */

		L_ctrl_code(_IF_OPER, &vars_go_ind, 0) ;            /* Форм.собственно ELSE-оператор */
		L_ied_close(id_stk_cnt-1) ;                         /* Закрываем IF оператор */

	    id_stk[id_stk_cnt-1].else_flag = 1 ;                    /* Корректируем запись в IF_DO стеке */
	    id_stk[id_stk_cnt-1].block_flag=block_flag ;
	    id_stk[id_stk_cnt-1].addr      =SYS_CTRL_CNT-1 ;


				   text+=4+block_flag ;             /* "Проходим" IF-оператор */
				}                                   /* END.5 */
/*---------------------------------- Обработка "непоявившегося" ELSE */

   if(else_flag) {
      for( ; id_stk_cnt>0 ; id_stk_cnt--) {                         /* Закрыв. все "неблоковые" IF, ELSE и DO */
	  if( id_stk[id_stk_cnt-1].block_flag) break ;              /*  расположенные выше с одновременной    */
								    /*   коррекцией счетчика стека            */
	if(!id_stk[id_stk_cnt-1].if_do_flag &&
				   any_flag   )  SYS_CTRL_CNT-- ;

				 L_ied_close(id_stk_cnt-1) ;

	if(!id_stk[id_stk_cnt-1].if_do_flag &&
				   any_flag   )  SYS_CTRL_CNT++ ;
					  }

		      else_flag=0 ;
		 }
/*------------------------------------------- Обработка оператора IF */

    if(!memicmp(text, "IF(", 3)) {                                  /* IF.6 - Если обнар.оператор IF ... */

	   addr=L_find_close(text+3) ;                              /* Поиск закрывающей скобки оператора */

	if(addr==NULL) {   lng_error=_EL_IF_FMT ;                   /* Если таковой нет - ошибка */
			       return(_QUIT) ;    }
	else                     *addr =  0 ;                       /* Если есть - уст.разделитель */

	if(id_stk_cnt==_GOTO_MAX-1) {  lng_error=_EL_IFDO_OVR ;     /* Если IFDO стек переполнен - */
					       return(_QUIT) ;   }  /*     - выход с ошибкой       */

	 if(addr[1]=='{')  block_flag=1 ;                           /* Опред. наличие IF-блока */
	 else              block_flag=0 ;

	    id_stk[id_stk_cnt].if_do_flag= 0 ;                      /* Формируем запись в IF_DO стеке */
	    id_stk[id_stk_cnt].else_flag = 0 ;
	    id_stk[id_stk_cnt].block_flag=block_flag ;
	    id_stk[id_stk_cnt].addr      =SYS_CTRL_CNT ;
		   id_stk_cnt++ ;

	   L_ctrl_code(_CULC_OPER, &vars_if_ind, SYS_CULC_CNT) ;    /* Формир.опред. индикаторной переменной */
	   L_ctrl_code(  _IF_OPER, &vars_if_ind, 0) ;               /* Форм.собственно IF-оператор */

	    addr=L_culc_maker(text+3) ;                             /* Трансл. индикаторное выражение */
	 if(addr==NULL)  return(_QUIT) ;

				   text=addr+1+block_flag ;         /* "Проходим" IF-оператор */
				}                                   /* END.6 */
/*------------------------------------------ Обработка оператора FOR */

    if(!memicmp(text, "FOR(", 4)) {                                 /* Меняем 'заголовок' оператора */
				     text++ ;                       /*       с FOR на DO            */
				     text[0]='D' ;
				     text[1]='O' ;
				 }
/*------------------------------------------- Обработка оператора DO */

    if(!memicmp(text, "DO(", 3)) {                                  /* IF.7 - Если обнар.оператор IF ... */
/*- - - - - - - - - - - - - Поиск закрывающей скобки заголовка цикла */
			     text+=3 ;
	   addr=L_find_close(text) ;                                /* Поиск закрывающей скобки оператора */

	if(addr==NULL) {   lng_error=_EL_DO_FMT ;                   /* Если таковой нет - ошибка */
				return(_QUIT) ;    }
	else           {     do_addr[3]=addr ;                      /* Если есть - уст.разделитель */
				 *addr =  0 ;    }
/*- - - - - - - - - - - - - - -  Общий контроль и "рассечка" формата */
#ifdef _FORTRAN_DO
      for(i=2 ; i>=0 ; i--) {                                       /* Ищем точки "рассечки",          */
#else
       for(i=2 ; i>0 ; i--) {                                       /* Ищем точки "рассечки",          */
#endif
		 do_addr[i]=strrchr(text, do_slct[i]) ;             /*  при их отсутствии -            */
	      if(do_addr[i]==NULL) {   lng_error=_EL_DO_FMT ;       /*   уст.ошибку и прекращ.обраб.,  */
					    return(_QUIT) ;    }    /* а при обнаружении - терминируем */
	      else                           *do_addr[i]=0 ;        /*  строку в данной точке          */
			    }
/*- - - - - - - - - - - - - - -  Занесение соотв.записи в ID-DO стек */
	 if(id_stk_cnt==_GOTO_MAX-1) {  lng_error=_EL_IFDO_OVR ;    /* Если IFDO стек переполнен - */
					       return(_QUIT) ;   }  /*     - выход с ошибкой       */

	 if((do_addr[3])[1]=='{')  block_flag=1 ;                   /* Опред. наличие DO-блока */
	 else                      block_flag=0 ;

			       base=SYS_CTRL_CNT ;

	    id_stk[id_stk_cnt].if_do_flag= 1 ;                      /* Формируем запись в IF_DO стеке */
//          id_stk[id_stk_cnt].else_flag = 1 ;
	    id_stk[id_stk_cnt].block_flag=block_flag ;
	    id_stk[id_stk_cnt].addr      =base ;
		   id_stk_cnt++ ;
/*- - - - - - - - - - - - - - - - - - - - -  Обработка запуска цикла */
#ifdef _FORTRAN_DO
			  *do_addr[0]=do_slct[0] ;                  /* Восстан.иниц.часть оператора DO */
#endif

	   if(text[0]!=0) {                                         /* Если иниц.выражение не пустое... */
			addr=L_equ_maker(text) ;                    /*   Форм.иниц.параметра цикла */
		     if(addr==NULL)  return(_QUIT) ;
			  }
	   else           {                                         /* Если иниц.выражение пустое... */
			     L_ctrl_code(_NOP_OPER,0,0) ;            /*   Формир.пустой оператор */
			  }

		 L_ctrl_code(_IF_OPER, &vars_go_ind, base+3) ;      /* Форм.переход на тело цикла */
/*- - - - - - - - - - - - - - - - -  Обработка модификационной части */
#ifdef _FORTRAN_DO
			 name =text ;
		   *do_addr[0]=  0 ;                                /* Терминируем имя параметра цикла */
#endif

		   strcpy(buff, "") ;                               /* Сбрасываем рабочий буфер */
#ifdef _FORTRAN_DO
		   strcat(buff, name) ;                             /* Компонуем модификационное выражение */
		   strcat(buff, "=") ;
		   strcat(buff, name) ;
		   strcat(buff, "+") ;
#endif
		   strcat(buff, do_addr[2]+1) ;

	   if(buff[0]!=0) {                                         /* Если реиниц.выражение не пустое... */
			       addr=L_equ_maker(buff) ;             /*   Форм.модиф.параметра цикла */
			    if(addr==NULL)  return(_QUIT) ;
			  }
	   else           {                                         /* Если реиниц.выражение пустое... */
			     L_ctrl_code(_NOP_OPER,0,0) ;           /*   Формир.пустой оператор */
			  }
/*- - - - - - - - - - - - - - -  Обработка контрольно-выходной части */
		   strcpy(buff, "") ;                               /* Сбрасываем рабочий буфер */
#ifdef _FORTRAN_DO
		   strcat(buff, name) ;                             /* Компонуем модификационное выражение */
		   strcat(buff, "<=") ;
#endif
		   strcat(buff, do_addr[1]+1) ;

	     L_ctrl_code(_CULC_OPER, &vars_if_ind, SYS_CULC_CNT) ;  /* Формир.опред. индикаторной переменной */
	     L_ctrl_code(  _IF_OPER, &vars_if_ind, 0) ;             /* Форм.собственно IF-оператор */

		    addr=L_culc_maker(buff) ;                       /* Трансл. индикаторное выражение */
		 if(addr==NULL)  return(_QUIT) ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
		  text=do_addr[3]+1+block_flag ;
				}                                   /* END.7 */
/*---------------------------------------- Обработка оператора BREAK */

    if(!memicmp(text, "BREAK", 5)) {                                /* IF.5 - Если обнар.оператор RETURN ... */

      for(n=-1,i=0 ; i<id_stk_cnt ; i++)
		      if(id_stk[i].if_do_flag)  n=i ;

       if(n==-1) {   lng_error=_EL_EXT_CRC ;                        /* При отсутствии внешнего цикла - */
			  return(_QUIT) ;    }                      /*   уст.ошибку и прекращ.обраб.   */

	 L_ctrl_code(_SET_OPER, &vars_if_ind, 0) ;                  /* Установка индикатора условия */
	 L_ctrl_code( _IF_OPER, &vars_go_ind, id_stk[n].addr+4) ;

		    any_flag =1 ;
			text+=5 ;
				  }                                 /* END.5 */
/*------------------------------------- Обработка оператора CONTINUE */

    if(!memicmp(text, "CONTINUE", 8)) {                             /* IF.5 - Если обнар.оператор RETURN ... */

      for(n=-1,i=0 ; i<id_stk_cnt ; i++)
		      if(id_stk[i].if_do_flag)  n=i ;

       if(n==-1) {   lng_error=_EL_EXT_CRC ;                        /* При отсутствии внешнего цикла - */
			  return(_QUIT) ;    }                      /*   уст.ошибку и прекращ.обраб.   */

	 L_ctrl_code( _IF_OPER, &vars_go_ind, id_stk[n].addr+2) ;

		    any_flag =1 ;
			text+=8 ;
				    }                               /* END.5 */
/*--------------------------------------- Обработка оператора RETURN */

    if(!memicmp(text, "RETURN", 6)) {                               /* IF.6 - Если обнар.оператор RETURN ... */

      if(text[6]=='(') {
	  addr=L_find_close(text+7) ;                               /* Поиск закрывающей скобки оператора */
       if(addr==NULL) {   lng_error=_EL_RET_FMT ;                   /* При некорректном формате -    */
				return(_QUIT) ;    }                /*   уст.ошибку и прекращ.обраб. */
       else              *addr=0 ;

	   L_ctrl_code(_CULC_OPER, &vars_tmp, SYS_CULC_CNT) ;       /* Формир. RETURN-параметра */

	    addr=L_culc_maker(text+7) ;                             /* Трансл. выражение RETURN-параметра */
	 if(addr==NULL)  return(_QUIT) ;
		       }

	   L_ctrl_code(_RETURN_OPER, &vars_tmp, 0) ;                /* Создание исполнительного псевдокода */

		    ret_flag=1 ;                                    /* Уст.флаг наличия RETURN оператора */
		    any_flag=1 ;
				    }                               /* END.6 */
/*----------------------------------------- Обработка оператора GOTO */

    if(!memicmp(text, "GOTO", 4)) {                                 /* IF.7 - Если обнар.оператор GOTO ... */

			    text+=4 ;                               /* Устан.указатель на метку */
			     len =strlen(text) ;                    /* Опред.длину метки */

       if(text[len-1]==';') {       len-- ;                         /* Если оператор терминирован ';' - */
			       text[len]= 0 ;   }                   /*   - корректируем длину метки     */

      if(len>_NAME_MAX) {                                           /* IF.8 - Если метка слишком длинная ... */
				lng_error=_EL_MARK_BAD ;            /*  Устанавливаем ошибку */
				    return(_QUIT) ;                 /*  Прекращаем обработку */
			}
      else              {                                           /* ELSE.8 - Если метка корректная ... */

	      if(dir_cnt==_GOTO_MAX-1) {  lng_error=_EL_GOTO_OVR ;  /* Если слишком много переходов - */
					       return(_QUIT) ;   }  /*     - выход с ошибкой          */

		      strcpy(dir[dir_cnt].name, text) ;             /*  Заносим метку в список */
			     dir[dir_cnt].row=SYS_CTRL_CNT ;
				 dir_cnt++ ;

			  L_ctrl_code(_IF_OPER, &vars_go_ind, 0) ;  /* Создание исполнительного псевдокода */
			}                                           /* END.8 */

		    any_flag=1 ;
				 }                                  /* END.5 */
/*----------------------------------- Обработка оператора присвоения */

   if(!any_flag && text[0]!=0 && text[0]!='}') {

			addr=L_equ_maker(text) ;

		     if(addr==NULL)  return(_QUIT) ;

		     if(addr!=text) {    any_flag=  1 ;
					     text=addr ;  }

					       }
/*---------------------------- Обработка оператора '}' (конец блока) */

					 id_num=-1 ;
    if(*text=='}') {                                                /* IF.5 - Если обнар.конец блока ... */
      for(i=0 ; i<id_stk_cnt ; i++)
	       if(id_stk[i].block_flag)  id_num=i ;

      if(id_num<0) {  lng_error=_EL_BLOCK_UNK ;                     /* При "неожиданном" появлении - */
			     return(_QUIT) ;    }                   /*   уст.ошибку и прекращ.обраб. */

			    close_flag=1 ;
				  text++ ;                          /* "Проходим" оператор конца блока */
		   }                                                /* END.5 */
/*--------------------------- Специальная обработка IF-DO операторов */

   if(!close_flag)
    if(any_flag && id_stk_cnt)
	if(!id_stk[id_stk_cnt-1].block_flag)  close_flag=1 ;

/*------------------------------ Обработка закрытия IF-DO операторов */

    if(close_flag) {
/*- - - - - - - - - - - - - - - - Закрытие внутриблоковых операторов */
	if(id_num>=0)                                               /* Если закрывается блок - закрываем */
	    for(i=id_num ; i<id_stk_cnt ; i++)  L_ied_close(i) ;    /*  все "внутренние" IF-DO операторы */
	else     id_num=id_stk_cnt-1 ;

					 L_ied_close(id_num) ;      /* Закрываем оператор */

	  if(id_stk[id_num].if_do_flag ||                           /* Для операторов ELSE, DO... */
	     id_stk[id_num].else_flag    ) {
					      id_stk_cnt=id_num ;

	     for( ; id_stk_cnt>0 ; id_stk_cnt--) {                  /*   Закрываем все "неблоковые" ELSE и DO */
		 if( id_stk[id_stk_cnt-1].block_flag   )  break ;   /*    расположенные выше с одновременной  */
		 if(!id_stk[id_stk_cnt-1].if_do_flag &&             /*     коррекцией счетчика стека          */
		    !id_stk[id_stk_cnt-1].else_flag    )  break ;

				 L_ied_close(id_stk_cnt-1) ;
						 }

	   if(id_stk_cnt)
	     if(!id_stk[id_stk_cnt-1].if_do_flag &&
		!id_stk[id_stk_cnt-1].else_flag    )  else_flag=1 ;
	     else                                     else_flag=0 ;
					   }
	  else                             {
			      id_stk[id_num].block_flag= 0 ;
					     id_stk_cnt=id_num+1 ;
					      else_flag= 1 ;
					   }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
		   }
/*-------------------------------------------------------------------*/
					       }                    /* CONTINUE.2 */
		       }                                            /* CONTINUE.1 */

/*----------------------------- Разведение операторов GOTO по меткам */

  for(i=0 ; i<dir_cnt ; i++) {                                      /* CIRCLE.3 - Сканируем GOTO-список */
				name=dir[i].name ;

     for(j=0 ; j<mark_cnt ; j++)                                    /* Проверяем совпадение GOTO-метки */
       if(!strcmp(mark[j].name, name))  break ;                     /*      со списком меток           */

	 if(j==mark_cnt) {  lng_error=_EL_MARK_UNK ;                /* При отсутствии нужной метки -      */
				  return(_QUIT) ;    }              /*  - уст.ошибку и прекращ. обработку */

		  SYS_CTRL_CODE[dir[i].row].addr=mark[j].row ;      /* Уст.адрес перехода в упр.псевдокоде */
			     }                                      /* CONTINUE.3 */
/*------------------------------------------- Разведение ENTRY-меток */

  for(i=0 ; i<SYS_ENTRY_CNT ; i++) {                                /* CIRCLE.4 - Сканируем ENTRY-список */
				 name=SYS_ENTRY_LIST[i].name ;

     for(j=0 ; j<mark_cnt ; j++)                                    /* Проверяем совпадение GOTO-метки */
       if(!strcmp(mark[j].name, name))  break ;                     /*      со списком меток           */

			      lng_bad=name ;

	 if(j==mark_cnt) {  lng_error=_EL_ENTRY_UNK ;               /* При отсутствии нужной метки -      */
				  return(_QUIT) ;    }              /*  - уст.ошибку и прекращ. обработку */

			SYS_ENTRY_LIST[i].start=mark[j].row ;       /* Уст.адрес перехода в упр.псевдокоде */
				   }                                /* CONTINUE.4 */
/*-------------------------------------------- Заключительный анализ */

       if(!ret_flag) {   lng_error=_EL_RET_OUT ;                    /* При отсутстви RETURN в тексте - */
			       return(_QUIT) ;    }                 /*       - устанавливаем ошибку    */

       if(id_stk_cnt) {   lng_error=_EL_UNCLOSE ;                   /* При отсутстви RETURN в тексте - */
			       return(_QUIT) ;    }                 /*       - устанавливаем ошибку    */

/*---------------------------------------------- Размещение массивов */

	for(i=0 ; i<SYS_CTRL_CNT ; i++) {

	 if(SYS_CTRL_CODE[i].oper_code==_ALLOC_OPER) {
			    size =SYS_CTRL_CODE[i].addr ;
	   SYS_CTRL_CODE[i].addr =SYS_CTRL_CNT*sizeof(SYS_CTRL_CODE[0])
			       +vars_iarea_cnt*sizeof(double) ;
		  vars_iarea_cnt+=size ;
						     }
					}
/*--------------------------- Определяем размер упр. и вычисл. кодов */

     SYS_CTRL_USED=SYS_CTRL_CNT*sizeof(SYS_CTRL_CODE[0])
		  +vars_iarea_cnt*sizeof(double) ;

  if(SYS_CTRL_USED>SYS_CTRL_SIZE)  lng_error=_EL_AREA_MAX ;

     SYS_CULC_USED=SYS_CULC_CNT*sizeof(SYS_CULC_CODE[0]) ;

/*------------------------------- Совмещение со 'старым' обрамлением */

     lng_ctrl_used=SYS_CTRL_USED ;
     lng_culc_used=SYS_CULC_USED ;
      vars_int_cnt=SYS_INT_CNT ;
     vars_work_cnt=SYS_WRK_CNT ;
	 entry_cnt=SYS_ENTRY_CNT ;

/*-------------------------------------------------------------------*/

   return(0) ;
}


/************************************************************************/
/*                                                                      */
/*                       Обработка описаний                             */

    int  L_decl_maker(text)

      char *text ;

{
   int  var_num ;   /* Номер внутр.переменной */
  char *var  ;
  char *name ;
  char *index ;
  char *pars_list ;
  char *end  ;
   int  size ;
   int  i ;

/*-------------------------------------- Обработка числовых массивов */

   if(!memicmp(text, "DOUBLE", 6)) {
					  text+=6 ;
/*- - - - - - - - - - - - - - - - - - - - - - -  'Разборка' описания */
		      name=text ;                                   /* Адресуем имя массива */

		     index=strchr(name, '[') ;                      /* Поиск описателя массива */
		  if(index==NULL)  return(0)  ;                     /* Если это не массив */

		    *index=0 ;                                      /* Терминируем имя */
		     index++ ;

		       end=strchr(index, ']') ;                     /* Поиск конца описателя массива */
		    if(end==NULL) {  lng_error=_EL_INDEX ;
					    return(1) ;    }

		    if(end[1]!= 0 &&                                /* Контроль концовки описания */
		       end[1]!=';'  )  {  lng_error=_EL_DECL_FMT ;
						 return(1) ;       }

		      *end=0 ;                                      /* Терм.индексное выражение */
/*- - - - - - - - - - - - - - - - - - - - - -  Размещение переменной */
			 var=L_simple_unit(name) ;                  /* Ищем переменную среди существующих */
		      if(var!=NULL) {  lng_error=_EL_NAME_USED ;    /* Если имя уже использовано... */
					      return(1) ;         }

				lng_error=0 ;                       /* Сброс ошибки */

		       var_num=L_alloc_var(name) ;                  /* Размещаем переменную */
		    if(var_num==-1)  return(1) ;
/*- - - - - - - - - - - - - - - - - - - - - - - - Размещение массива */
			 size=atoi(index) ;                         /* Извлекаем размер массива */
		      if(size<=0) {   lng_error=_EL_DECL_SIZE ;
					     return(1) ;         }

     L_ctrl_code(_ALLOC_OPER, &(SYS_INT_VARS[var_num].value), size);/* Вставляем оператор размещения массива */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
				  }
/*------------------------------------------------- Обработка входов */

   if(!memicmp(text, "ENTRY", 5)) {

#define  ENTRY_NAME      SYS_ENTRY_LIST[SYS_ENTRY_CNT].name
#define  ENTRY_PARS_CNT  SYS_ENTRY_LIST[SYS_ENTRY_CNT].pars_cnt
#define  ENTRY_PARS      SYS_ENTRY_LIST[SYS_ENTRY_CNT].pars

					  text+=5 ;
/*- - - - - - - - - - - - - - - - - - - Контроль переполнения списка */
    if(SYS_ENTRY_CNT>=SYS_ENTRY_MAX) {   lng_error=_EL_ENTRY_MAX ;
					       return(1) ;         }
/*- - - - - - - - - - - - - - - - - - - - Выделение и контроль имени */
		 name=text ;
	    pars_list=strchr(name, '(') ;                           /* Ищем терминатор имени */
	 if(pars_list==NULL)  {  lng_error=_EL_DECL_FMT ;           /* Если его нет... */
					return(1) ;       }
	   *pars_list=0 ;                                           /* Терминируем имя */
	    pars_list++ ;                                           /* Проходим на список параметров */

	 if(strlen(name)>_NAME_MAX) {  lng_error=_EL_NAME_LEN ;     /* Если имя слишком длинное... */
					      return(-1) ;      }
	 if(strlen(name)==   0    ) {  lng_error=_EL_NAME_EMPTY ;   /* Если пустое имя... */
					      return(-1) ;      }

	       strcpy(ENTRY_NAME, name) ;                           /* Заносим имя входа */
/*- - - - - - - - - - - - - - - - -  Подготовка обработки параметров */
		   end=strchr(pars_list, ')') ;                     /* Поиск терминатора списка параметров */
		if(end==NULL) {  lng_error=_EL_DECL_FMT ;
					    return(1) ;        }

		if(end[1]!= 0 &&                                    /* Контроль концовки описания */
		   end[1]!=';'  )  {  lng_error=_EL_DECL_FMT ;
					     return(1) ;       }

		  *end=0 ;                                          /* Терм.список параметров */
/*- - - - - - - - - - - - - - - - - - - - - - - Обработка параметров */
     if(pars_list!=end) {                                           /* IF.1 - Если параметры есть... */

	 for(i=0 ; ; i++) {
			    end=strchr(pars_list, ',') ;            /* Ищем разделитель параметров */
			 if(end!=NULL)  *end=0 ;

	   for(var_num=0 ; var_num<SYS_INT_CNT ; var_num++)         /* Смотрим нет ли такой внутр.перем.    */
	    if(!strcmp(SYS_INT_VARS[var_num].name,                  /*  и если такая есть - уст.номер перем.*/
					 pars_list))  break ;

	      if(var_num==SYS_INT_CNT) {                            /* Если такой перем.нет... */
			   var_num=L_alloc_var(pars_list) ;         /*   Размещаем переменную */
			if(var_num==-1)  return(1) ;
				       }

			    ENTRY_PARS[i]=var_num ;

			 if(end==NULL)  break ;

				pars_list=end+1 ;
			  }
			       ENTRY_PARS_CNT=i+1 ;
			}                                           /* ELSE.1 */
/*- - - - - - - - - - - - - - -  Обработка пустого списка параметров */
     else               {                                           /* ELSE.1 */
			       ENTRY_PARS_CNT=0 ;
			}                                           /* END.1 */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
#undef  ENTRY_NAME
#undef  ENTRY_PARS_CNT
#undef  ENTRY_PARS
					SYS_ENTRY_CNT++ ;
				 }
/*---------------------------------------- Обработка внешних модулей */

   if(!memicmp(text, "EXTERN", 6)) {
	 			   }
/*-------------------------------------------------------------------*/

   return(0) ;
}


/************************************************************************/
/*                                                                      */
/*              Формирователь управляющего псевдокода                   */

   int  L_ctrl_code(code, oper, addr)

	 int  code ;   /* Код операции */
      double *oper ;   /* Адрес операнда */
	 int  addr ;   /* Рабочий адрес */

{
    if(SYS_CTRL_CNT>=SYS_CTRL_MAX-8) {
					lng_error=_EL_CTRL_BUFF ;
					       return(-1) ;
				     }

       SYS_CTRL_CODE[SYS_CTRL_CNT].oper_code=code ;
       SYS_CTRL_CODE[SYS_CTRL_CNT].operand  =oper ;
       SYS_CTRL_CODE[SYS_CTRL_CNT].addr     =addr ;
       SYS_CTRL_CODE[SYS_CTRL_CNT].text_row =row ;
		     SYS_CTRL_CNT++ ;

  return(0) ;
}


/************************************************************************/
/*                                                                      */
/*              Закрытие IF, ELSE и DO операторов                       */

 void  L_ied_close(pnt)

     int  pnt ;   /* Указатель строки стека */

{
   int  n ;

/*- - - - - - - - - - - - - - - - - - - - - -  Закрытие оператора DO */
	if(id_stk[pnt].if_do_flag) {

	  memmove(&SYS_CTRL_CODE[SYS_CTRL_CNT  ],
		  &SYS_CTRL_CODE[SYS_CTRL_CNT-1],
			      sizeof(SYS_CTRL_CODE[0])) ;

				 n=id_stk[pnt].addr ;

		      if(else_flag)  SYS_CTRL_CNT-- ;

		L_ctrl_code(_IF_OPER, &vars_go_ind, n+2) ;          /* Форм.собственно ELSE-оператор */

		      SYS_CTRL_CODE[n+4].addr=SYS_CTRL_CNT ;

		    if(else_flag)  SYS_CTRL_CNT++ ;
				   }
/*- - - - - - - - - - - - - - - - - - - - -  Закрытие оператора ELSE */
  else  if(id_stk[pnt].else_flag ) {
					  n =id_stk[pnt].addr ;
		       SYS_CTRL_CODE[n].addr=SYS_CTRL_CNT ;
				   }
/*- - - - - - - - - - - - - - - - - - - - - -  Закрытие оператора IF */
  else                             {
					    n =id_stk[pnt].addr ;
		       SYS_CTRL_CODE[n+1].addr=SYS_CTRL_CNT ;
				   }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
}


/************************************************************************/
/*                                                                      */
/*              Обработка выражений присвоения                          */

  char *L_equ_maker(text)

    char *text ;

{
    char *addr ;
    char *var ;
     int  len ;
     int  i ;


/*------------------------------------ Подготовка и входной контроль */

			    addr=strchr(text, ';') ;                /* Терминируем по ; */
	   if(addr!=NULL)  *addr=0 ;

	  if(*text==0)  return(text) ;                              /* Обработка пустого оператора */

/*---------------------- Автоматическое размещение ячейки результата */

	addr=strchr(text, '=') ;                                    /* Ищем разделитель присвоения */

  while(addr!=NULL && addr[1]!='=') {                               /* IF.1 - Если подозр. присвоение ... */

		       len=addr-text ;                              /* Опред.длину метки */

	  if(len==0) {   lng_error=_EL_NAME_EMPTY ;                 /* Если имя отсутствует -       */
				return(NULL) ;       }              /*  - уст.ошибку и прекр.обраб. */
/*- - - - - - - - - - - - - - - Выделение имени переменой результата */
       for(i=0 ; i<len; i++)                                        /* Контролир.состав символов  */
		if(!LNG_NAME(text[i]))  break ;                     /*  предполагаемой переменной */

	    if(text[i]=='[')  break ;                               /* Массивы автоматически не размещаем */

      if(i==len) {                                                  /* IF.2 - Если это действ.переменная ... */
	  if(len>_NAME_MAX) {   lng_error=_EL_NAME_LEN ;            /* Если имя слишком длинное -  */
				      return(NULL) ;      }         /*  - уст.ошибку и прекр.обраб. */
/*- - - - - - - - - - - - - - -  Поиск переменной среди существующих */
				 text[len]=  0 ;                    /* Терминируем имя */

			 var=L_simple_unit(text) ;                  /* Ищем переменную среди существующих */
		   lng_error=   0 ;                                 /* Сброс ошибки */

		if(var==NULL) {                                     /* Размещение новой переменной */
				    L_alloc_var(text) ;
				if(lng_error)  return(NULL) ;
			      }
/*- - - - - - - - - - - - - - - - - - - - - Восстановление выражения */
			      addr[0]='=' ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
		 }                                                  /* END.2 */
			break ;
				    }                               /* END.1 */
/*------------------------------ Создание исполнительного псевдокода */

		L_ctrl_code(_CULC_OPER, NULL, SYS_CULC_CNT) ;       /* Создание исполнительного псевдокода */
	  text=L_culc_maker(text) ;                                 /* Создание вычислительного кода */
       if(text==NULL)  return(NULL) ;

/*-------------------------------------------------------------------*/

  return(text) ;
}


/************************************************************************/
/*                                                                      */
/*              Размещение внутренной переменной                        */
/*                                                                      */
/*   Return:  адрес описания или NULL                                   */

  int  L_alloc_var(name)

     char  *name ;   /* Имя переменной */

{
  int  i ;

/*--------------------------------------------------- Контроль имени */

	  if(name[0]==0) {  lng_error=_EL_NAME_EMPTY ;              /* Если пустое имя... */
				    return(-1) ;         }

	      for(i=0 ; name[i]; i++)                               /* Контролир.состав символов  */
		if(!LNG_NAME(name[i]))  break ;                     /*  предполагаемой переменной */

	  if(name[i]!=0) {  lng_error=_EL_NAME_SINTAX ;             /* Если некорр.символ имени... */
				    return(-1) ;         }

	  if(i>_NAME_MAX) {  lng_error=_EL_NAME_LEN ;               /* Если имя слишком длинное... */
				    return(-1) ;        }

/*-------------------------------------------- Размещение переменной */

		if(SYS_INT_CNT==SYS_INT_MAX) {                      /*  Если слишком много переменных - */
				      lng_error=_EL_INT_VAR_MAX ;   /*     - выход с ошибкой            */
						 return(-1) ;
					     }

	       strcpy(SYS_INT_VARS[SYS_INT_CNT].name, name) ;       /*  Заносим имя во "внутренний" список */
				   SYS_INT_CNT++ ;

/*-------------------------------------------------------------------*/

  return(SYS_INT_CNT-1) ;
}


/************************************************************************/
/*                                                                      */
/*              Транслятор вычислительного псевдокода                   */

  char *L_culc_maker(text)

    char *text ;

{
static char  work[_LEN_MAX] ;
       char *addr ;
       char *beg ;
       char *end ;
	int  beg_cnt ;    /* Число открывающих скобок */
	int  end_cnt ;    /* Число закрывающих скобок */
	int  end_len ;
	int  str_len ;
	int  len ;

/*---------------------------------------------- Общая инициализация */

			strcpy(work, text) ;

	      text=text+strlen(work) ;                              /* Опр.след. за оператором символ */

	  addr=strchr(work, ';') ;                                  /* Ищем терминатор выражения */
       if(addr!=NULL)  *addr=0 ;                                    /* Отсекаем выражение */

		      vars_tbl_cnt= 0 ;                             /* Сброс счетчиков */
		       SYS_WRK_CNT= 0 ;
		     vars_cnst_cnt= 0 ;
			 index_cnt= 0 ;

/*-------------------------------------------------- Контроль скобок */

	   beg_cnt=0 ;
	   end_cnt=0 ;

   for(addr=work ; *addr ; addr++) {                                /* CIRCLE.1 - Сканируем строку */

      if(*addr=='{') {  addr+=4 ;  continue ;  }                    /* Проходим кодовые группы */

	      if(*addr=='(')  beg_cnt++ ;                           /* Подсчитываем скобки */
	      if(*addr==')')  end_cnt++ ;

	       if(beg_cnt<end_cnt)  break ;
				   }                                /* CONTINUE.1 */

     if(beg_cnt>end_cnt) {   lng_error=_EL_OPEN ;                   /* Обработ несоответствия скобок */
				  return(NULL) ;    }
     if(beg_cnt<end_cnt) {   lng_error=_EL_CLOSE ;
				  return(NULL) ;    }

/*--------------------------------- Обработка подвыражений в скобках */

	       len=strlen(work) ;
	       beg=&work[len-1] ;

  while(1) {                                                        /* CIRCLE.2 - Цикл по числу скобок */

   for(addr=beg ; addr>=work ; addr--)                              /* Ищем открывающие скобки справа налево */
	  if(*addr=='}')    addr-=4 ;                               /*   Проходим спец.символы */
     else if(*addr=='(') {
		if(LNG_CHAR(*(addr-1)))  continue ;                 /*   Если перед скобкой находится    */
					   break ;                  /*    алф-цифр.символ - это функция, */
			 }                                          /*     мы ее игнорируем              */

		 if(*addr!='(')  break ;

	      beg=addr ;
	      end=L_find_close(beg+1) ;

		 *end=0 ;                                           /* Терминируем подвыражение в скобках */

	       addr=L_simple_string(beg+1) ;                        /* Обрабат.подвыражение в скобках */

	   if(lng_error)  return(NULL) ;                            /* Если есть ошибки - _QUIT-выход */

		     end_len=strlen(end+1)+1 ;                      /* Определяем длинны "хвоста",  */
		     str_len=end-beg+1 ;                            /* исходного подвыр.в скобках и */
			 len=strlen(addr) ;                         /* обработанного выражения      */

	   if(len!=str_len)  memmove(beg+len, end+1, end_len) ;     /* "Поджимаем" исходное выражение */
			     memmove(beg, addr, len) ;              /* Переносим обраб.выраж. в исходное */
	   }                                                        /* CONTINUE.2 */
/*--------------------------------- Обработка "суммарного" выражения */

	       L_simple_string(work) ;
		   L_culc_code(_STOP_OPER, NULL, NULL) ;            /* Формируем оператор STOP */

      if(lng_error)  return(NULL) ;

      if(SYS_INT_CNT>SYS_WRK_MAX) {                                 /* Если слишком много переменных - */
				      lng_error=_EL_WRK_VAR_MAX ;   /*    - выход с ошибкой            */
						 return(NULL) ;
				  }

/*-------------------------------------------------------------------*/

  return(text) ;
}


/*********************************************************************/
/*                                                                   */
/*                Обработка простых строк                            */
/*                                                                   */
/*   Простая строка НЕ содержит подвыражений ни в скобках,           */
/*    ни в параметрах процедур, ни в индексах                        */

   char *L_simple_string(string)

    char *string ;   /* Обрабатываемая строка */

/* Return: Указатель на результат обработки или */
/*          NULL, если есть ошибка              */
{
 struct L_culc *exe ;               /* Стартовый адрес исполнит. кода */
   static char  work[2*_LEN_MAX] ;
	  char *pars[20] ;          /* Указатели на параметры */
	  char *name ;              /* Имя обьекта */
	  char *addr ;
	  char *beg ;
	  char *end ;
	  char *tmp ;
	  char  next ;
	   int  oper_code ;
	   int  len ;
	   int  n ;
	   int  i ;
	   int  j ;

/*---------------------------------------------- Общая инициализация */

	      strcpy(work, string) ;

/*--------------------------------------- Контроль наличия выражения */

   for(addr=work ; *addr ; addr++)
      if((*addr>='0' && *addr<='9') ||
	 (*addr>='A' && *addr<='z')   )  break ;

     if(*addr==0) {  lng_error=_EL_EMPTY ;
		       return(NULL) ;      }

/*-------------------------------------- Обработка строчных констант */

   if(text_flag) {
			n=text_size/_CULC_FRAME+1 ;                 /* Опред.число кадров под */
								    /*   текстовой сегмент    */
		    L_culc_code(_TSEG_OPER, (char *)n, NULL) ;      /* Специфицируем текстовой сегмент */

		       exe=&SYS_CULC_CODE[SYS_CULC_CNT] ;           /* Опр.адрес текстового буфера */
		memcpy(exe, text_buff, n*_CULC_FRAME) ;             /* Заполнеям текстовой сегмент */

			  SYS_CULC_CNT+=n ;                         /* Корр.счетчик кадров */
			     text_flag =0 ;
		 }
/*------------------------------------------------ Обработка функций */

    for(addr=work+1 ; ; addr++) {                                   /* CIRCLE.1 - Цикл анализа ... */
/*- - - - - - - - - - - - - - - - - - - - -  Выделение имени функции */
	       addr=strchr(addr, '(') ;                             /* Ищем откр.скобку */
	    if(addr==NULL)  break ;                                 /* Если нет - завершаем анализ */

	    if(!LNG_NAME(addr[-1]))  continue ;                     /* Если перед именем функции НЕ       */
								    /*   алф-цифр.символ - это НЕ функция */
	 for(name=addr-1 ; LNG_NAME(name[-1]) &&                    /* Выходим на начало имени */
				    name!=work-1 ; name--) ;

		 *addr=  0 ;                                        /* Терминируем имя */
	       lng_bad=name ;                                       /* Уст.указатель 'плохого' места */
/*- - - - - - - - - - - - - - - - - - - - - -  Идентификация функции */
	     func_flag=0 ;                                          /* Сбрас.флаг обнаружения функции */

	 for(i=0 ; i<SYS_ENTRY_CNT ; i++)                           /* Ищем функцию по списку ENTRY-входов */
	  if(!strcmp(name, SYS_ENTRY_LIST[i].name)) {
			  func_flag    =         1 ;
			  func_num     =         i ;
			  func_pars_cnt=SYS_ENTRY_LIST[i].pars_cnt ;
			  func_type    =        'R' ;
				      break ;
						    }

      if(!func_flag)
	 for(i=0 ; i<SYS_EXT_FCNT ; i++)                            /* Ищем функцию по внешнему списку */
	  if(!strcmp(name, SYS_EXT_FUNC[i].name)) {
			  func_flag    =         1 ;
			  func_num     =         i ;
			  func_pars_cnt=SYS_EXT_FUNC[i].pars_cnt ;
			  func_type    =        'F' ;
				      break ;
						  }

      if(!func_flag) {                                              /* Если функция не найдена -    */
	   func_num=L_table_objects(_FIND_FUNC, name) ;             /*   ищем ее по внешней таблице */
	if(func_num>=0) {
	  if(func_std!=_LANG_STD_FUNC) {  lng_error=_EL_STD_OUT ;   /*   Если несовпадение стандартов - */
					       return(NULL) ;     } /*    - уст.ошибку и выходим        */
			    func_flag= 1 ;
			    func_type='P' ;
			}
		     }

			    strcpy(extrn_name, name) ;              /* Уст.имя внешнего обьекта */

      if(!func_flag) {   lng_error=_EL_FUNC_UNK ;                   /* Если функция не найдена - */
			     return(NULL) ;        }                /*  - уст.ошибки и выходим   */
/*- - - - - - - - - - - - - - - - - - - - - -  Рассечение параметров */
		    *addr='(' ;                                     /* Восст.скобку */
		      beg=addr+1 ;                                  /* Уст.начало списка параметров */
		      end=strchr(beg, ')') ;                        /* Ищем конец списка параметров */
		   if(end==NULL) {  lng_error=_EL_FUNC_FMT ;        /* Если такового нет - ошибка */
					    return(NULL) ;  }
		     *end=0 ;                                       /* Терминируем список параметров */

      if(beg!=end) {                                                /* Если есть параметры ... */
	for(j=0 ; ; j++) {    pars[j]=strrchr(beg, ',') ;           /*   Определяем число параметров */
			   if(pars[j]==NULL) break ;                /*    и рассекаем их             */
			     *pars[j]=0 ;                 }

			      pars[j]=beg-1 ;
		   }
      else              j=-1 ;                                      /* Если параметров нет ... */

	   if(func_pars_cnt>=0)                                     /* Число параметров строго задано... */
	     if(j+1!=func_pars_cnt) {                               /*  Если число параметров не */
			 lng_warn[lng_warn_cnt].code=_WL_FUNC_PARS ;/*   совпадает с заданным -  */
			 lng_warn[lng_warn_cnt].row = row ;         /*    выдаем предупреждение  */

		  if(lng_warn_cnt<_LNG_WARN_MAX-1)  lng_warn_cnt++ ;
				    }
/*- - - - - - - - - - - - - - - Формируем вызов функции в псевдокоде */
	for( ; j>=0 ; j--) {                                        /* Кодируем занесение параметров */
			     tmp=L_simple_unit(pars[j]+1) ;         /*      функции в стек           */
	    if(tmp==NULL) {  lng_error=_EL_FUNC_SINTAX ;
					 return(NULL) ;  }
		  L_culc_code(_PUSH_OPER, tmp, NULL) ;
			   }

			func[1]=func_type ;                         /* Формируем код функции */
			func[2]=func_num+1 ;
			rslt[2]=SYS_WRK_CNT+1 ;                     /* Выделяем рабочую ячейку для результата */
				SYS_WRK_CNT++ ;

		L_culc_code(_CALL_OPER, func, rslt) ;               /* Формируем собств.вызов функции */

		  memmove(name+5, end+1, strlen(end+1)+1) ;         /* Вставл.ячейку результата в текст */
		  memmove(name, rslt, 5) ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
				}                                   /* CONTINUE.1 */
/*----------------------------------------------- Обработка констант */

   for(addr=work ; *addr ; addr++) {                                /* CIRCLE.4 - Сканируем строку */

      if(*addr=='{') {  addr+=4 ;  continue ;  }                    /* Проходим кодовые фрагменты */

	 if((*addr>='0' && *addr<='9') ||                           /* IF.2 - Если цифра, десятичная точка */
	     *addr=='.'                ||                           /*          или знак ...               */
	     *addr=='-'                ||
	     *addr=='+'                  ) {

      if(  addr    !=work         && 
           addr[-1]!='='          &&                                /* Если число не с начала буфера -  */          
	 (*addr=='-' || *addr=='+') ) {  end=addr+1 ;               /* оно не может начинаться со знака */
					  continue ;  }

	       vars_cnst[vars_cnst_cnt]=strtod(addr, &end) ;        /* Дешифруем число */

      do {                                                          /* BLOCK.1 */
                   j=0 ;

       if(addr[-1]!='[' && end[0]!=']') {                           /* IF.3 - Если НЕ индекс... */

	 if(*end!=0)                                                /* Если число не оканчивает строку */
	    for(j=0 ; j<_OPER_MAX ; j++)                            /*  Проверяем следует ли за числом */
	      if(lng_oper[j].len==2) {                              /*    знак операции                */
		    if((lng_oper[j].name)[0]==*(end+0) &&
		       (lng_oper[j].name)[1]==*(end+1)   ) break ;
				     }
	      else  if((lng_oper[j].name)[0]==*(end+0)   ) break ;

		if(j==_OPER_MAX)  break ;                           /* Если нет - это не число */

	 if(addr!=work) {                                           /* Если число не с начала буфера... */
	    for(j=0 ; j<_OPER_MAX ; j++)                            /*  Проверяем предшествует ли числу */
	      if(lng_oper[j].len==2) {                              /*    знак операции                 */
		    if((lng_oper[j].name)[0]==*(addr-2) &&
		       (lng_oper[j].name)[1]==*(addr-1)   ) break ;
				     }
	      else  if((lng_oper[j].name)[0]==*(addr-1)   ) break ;

		if(j==_OPER_MAX)  break ;                           /*   Если нет - это не число */
			}
					}                           /* END.3 */

			   cnst[2]=vars_cnst_cnt+1 ;
			   cnst[3]='0' ;

		  memmove(addr+5, end, strlen(end)+1) ;             /* Вставл.ячейку константы в текст */
		  memmove(addr, cnst, 5) ;
		      addr+=4 ;

		    vars_cnst_cnt++ ;                               /* Модиф.счетчик констант */

	 } while(0) ;                                               /* BLOCK.1 */
		      if(*end==0)  break ;                          /* Если конец строки - конец анализа */
					   }                        /* END.2 */

				   }                                /* CONTINUE.4 */
/*--------------------------------------------- Обработка переменных */

   for(addr=work ; *addr ; addr++) {                                /* CIRCLE.5 - Сканируем строку */

      if(*addr=='{') {  addr+=4 ;  continue ;  }                    /* Проходим кодовые фрагменты */

     if(LNG_CHAR(*addr)) {                                          /* IF.4 - Если буква */

       for(end=addr ; LNG_CHAR(*end) ; end++) {                     /* Ищем конец имени переменной */
			   if(*end=='[')  end+=1 ;
			   if(*end=='{')  end+=4 ;
					      }
		    lng_bad=addr ;                                  /* Уст.указатель ошибочного фрагмента */

			len=end-addr ;                              /* Опред.длину имени */
		       next=*end ;                                  /* Спасаем следующий за именем символ */
		       *end= 0 ;                                    /* Терминируем имя */

		     if(len>_NAME_MAX) {  lng_error=_EL_NAME_LEN ;  /* Если имя слишком длинное - это ошибка */
					   return(NULL) ;  }

			     tmp=L_simple_unit(addr) ;              /* Обрабатываем имя */
			  if(tmp==NULL)  return(NULL) ;             /* Если имя неизвестно - ошибка */

			 *end=next ;                                /* Восстан.символ после имени */

		  memmove(addr+5, end, strlen(end)+1) ;             /* Вставл.ячейку переменной текст */
		  memmove(addr, tmp, 5) ;
			  addr+=4 ;

//		      if(*end==0)  break ;                          /* Если конец строки - конец анализа */
			 }                                          /* END.4 */
				   }                                /* CONTINUE.5 */
/*--------------------------------------------- Трансляция выражения */
/*-------------------------------------------------------------------*/

		 exe=&SYS_CULC_CODE[SYS_CULC_CNT] ;

   for(i=0 ; i<_OPER_MAX ; i++)  do {                               /* CIRCLE.6 - По операциям */

		oper_code=lng_oper[i].code ;
		      len=lng_oper[i].len ;

    for(addr=work ; addr!=NULL ; addr+=len) {                       /* Ищем знак операции */
			    addr=strstr(addr, lng_oper[i].name) ;
                         if(addr     ==NULL)  break ;
			 if(addr[len]!= '}')  break ;
					    }
       if(addr==NULL)  break ;                                      /* Если его нет - перех. к след.операции */

       if(addr[len]!='{') {  lng_error=_EL_SINTAX1 ;                /* Если вслед за операцией нет операнда - */
				    return(NULL) ;   }              /*  фиксируем ошибку                      */

      if(oper_code!=_MOVE_OPER)
       if(addr[len+1]!='W') {     rslt[2]=SYS_WRK_CNT+1 ;           /* Если правый операнд -    */
			L_culc_code(_MOVE_OPER, addr+len, rslt) ;   /*  не рабочая ячейка, то   */
								    /*   заносим правый операнд */
				  addr[len+1]='W' ;                 /*  в рабочую ячейку с      */
				  addr[len+2]=SYS_WRK_CNT+1 ;       /*   соотв.заменой в тексте */
				  addr[len+3]='0' ;
					      SYS_WRK_CNT++ ;
						      exe++ ;  }

      if(addr==work) {                                              /* IF.5 - Если унитарная операция ... */
	 if(lng_oper[i].pref_flag) {  lng_error=_EL_SINTAX2 ;       /* Если операция не может быть    */
					     return(NULL) ;   }     /*   унитарной - фиксируем ошибку */
		    L_culc_code(oper_code, zero, addr+len) ;        /* Форм.код операции */
						 exe++ ;
		     }                                              /* ELSE.5 */
      else           {                                              /* ELSE.5 - Если парная операция */
	if(oper_code==_MOVE_OPER)                                   /* Форм.код операции */
			L_culc_code(oper_code, addr+len, addr-5) ;
	else            L_culc_code(oper_code, addr-5, addr+len) ;
					       exe++ ;
		     }                                              /* END.5 */

		       strcpy(addr-5, addr+len) ;                   /* "Удаляем" левый операнд */

				    } while(1) ;                    /* CONTINUE.6 */

/*---------------------------------------------- Контроль "остатков" */

	       len=strlen(work) ;

	 if(len!=5) {
			lng_error=_EL_SINTAX3 ;
			       return(0) ;
		    }
/*-------------------------------------------------- Анализ концовки */

	 if(work[1]!='W') {                                         /* Если остаточный операнд не W - */
				  rslt[2]=SYS_WRK_CNT+1 ;           /*  заносим его в                 */
			L_culc_code(_MOVE_OPER, work, rslt) ;       /*   рабочую ячейку, а на ее      */
								    /*   место пишем идентификатор    */
				  work[1]='W' ;                     /*   соотв. рабочей ячейки        */
				  work[2]=SYS_WRK_CNT+1 ;
				  work[3]='0' ;
					  SYS_WRK_CNT++ ;
			  }
/*-------------------------------------------------------------------*/
/*-------------------------------------------------------------------*/

  return(work) ;
}


/*********************************************************************/
/*                                                                   */
/*           Поиск закрывающей скобки                                */

  char *L_find_close(text)

     char *text ;

{
  int  n ;


  for(n=0 ; *text ; text++)
	 if(*text=='{')  text+=4 ;
    else if(*text==')') if(n==0)  break ;
			else       n-- ;
    else if(*text=='(')    n++ ;

      if(*text!=')')  text=NULL ;

  return(text) ;
}


/*********************************************************************/
/*                                                                   */
/*           Обработка одинарной константы или переменной            */

   char *L_simple_unit(name)

     char *name ;  /* Подлежащая обработке единица */

{
 static char  code[8]="{???}" ;
	char *index ;            /* Индексное выражение */
	char *index_code ;       /* Код индексного выражения */
	 int  index_num ;        /* 'Номер' индексного подвыражения */
	char *end ;
	 int  n  ;
	 int  i ;

/*--------------------------------------------- Обработка индексации */

		       index=strchr(name, '[') ;                    /* Ищем вход индекса */

     if(index!=NULL) {                                              /* IF.1 - Если есть индекс */

	if(index_cnt>=_INDEX_MAX) {  lng_error=_EL_INDEX_MAX ;      /* Контроль переполнения */
					    return(NULL) ;      }   /*    списка индексов    */

					  *index=0 ;                /* Терминируем имя массива */
					   index++ ;                /* Проходим скобку */

				  n=strlen(index) ;                 /* Контролируем закр.скобку */
		 if(index[n-1]!=']') {  lng_error=_EL_INDEX ;
					      return(NULL) ;   }

			     index[n-1]=0 ;                         /* Терминируем индекс */

			   index_code=L_simple_unit(index) ;        /* Обраб.индексного выражения */
			if(index_code==NULL)  return(NULL) ;

		      strcpy(index_list[index_cnt], index_code) ;
					index_cnt++ ;
			      index_num=index_cnt  ;
		     }                                              /* ELSE.1 */
     else            {                                              /* ELSE.1 - Если простой обьект */
			      index_num= 0 ;
		     }                                              /* END.1 */
/*--------------------------------- Обработка как числовой константы */

	      vars_cnst[vars_cnst_cnt]=strtod(name, &end) ;         /* Дешифруем число */

   if(*end==0) {                                                    /* Если это действительно константа */
		  code[1]='C' ;
		  code[2]=vars_cnst_cnt+1 ;
		  code[3]=index_num+'0'  ;
			  vars_cnst_cnt++ ;

			    return(code) ;
	       }
/*------------------------------- Обработка как символьной константы */

   if(name[0]=='{' &&
      name[1]=='L'   ) {
			  memcpy(code, name, 5) ;
			   code[3]=index_num+'0'  ;
				 return(code) ;
		       }

   if(name[0]=='{' &&
      name[1]=='C'   ) {
			  memcpy(code, name, 5) ;
			   code[3]=index_num+'0'  ;
				 return(code) ;
		       }
/*----------------------------------------- Обработка как переменной */

			   code[3]=index_num+'0'  ;

					 n=-1 ;

	    for(i=0 ; i<SYS_EXT_VCNT && n<0 ; i++)                  /* Смотрим нет ли такой внешн.перем.  */
		 if(!strcmp(SYS_EXT_VARS[i].name, name))  n=i ;     /*  и если такая есть уст.номер перем.*/

	  if(n>=0) {   code[1]='E' ;                                /* Если это внешняя переменная */
		       code[2]= n+1 ;
			return(code) ;   }

	    for(i=0 ; i<SYS_INT_CNT && n<0 ; i++)                   /* Смотрим нет ли такой внутр.перем.  */
		 if(!strcmp(SYS_INT_VARS[i].name, name))  n=i ;     /*  и если такая есть уст.номер перем.*/

	  if(n>=0) {   code[1]='I' ;                                /* Если это внутренняя переменная */
		       code[2]= n+1 ;
			return(code) ;   }

		     n=L_table_objects(_FIND_VAR, name) ;           /* Ищем переменную по внешней таблице */
	  if(n>=0) {  code[1]='T' ;                                 /* Если таковая имеется ... */
		      code[2]= n+1 ;
		       return(code) ;   }

/*---------------------------------- Обработка отсутствия переменной */

    lng_bad=  name ;
  lng_error=_EL_NAME_UNK ;

  return(NULL) ;
}


/************************************************************************/
/*                                                                      */
/*           Работа с обьектами внешней таблицы - LNG_TABLE             */
/*             и внешней библиотеки LNG_EXT_LIB                         */

    int  L_table_objects(oper_code, name)

      int  oper_code ;   /* Код операции:                     */
			 /*     _FIND_VAR  - найти переменную */
			 /*     _FIND_FUNC - найти функцию    */
     char *name ;        /* Имя обьекта */

{
 struct    L_variable *lang_vars ;   /* Переменные стандарта LANG */
 struct    L_function *lang_func ;   /* Функции стандарта LANG */
 struct    Ccl_object *ccl_obj ;     /* Обьект стандарта CCL */
 struct Dcl_vars_decl *dcl_obj ;     /* Обьект стандарта DCL */
		  int  func_flag ;   /* Флаг работы с функцией */
		  int  n_list ;      /* Номер списка в таблице */
		  int  i ;           /* Номер обьекта в списке */
		 void *addr ;        /* Адрес обьекта */

/*--------------------------------- Входной контроль и инициализация */

       if(SYS_EXT_TABLE==NULL)  return(-1) ;

	    addr=NULL ;

       if(oper_code==_FIND_FUNC)  func_flag=1 ;
       else                       func_flag=0 ;

/*---------------------------------------------------- Поиск обьекта */

  for(n_list=0 ; SYS_EXT_TABLE[n_list].objects!=NULL ; n_list++) {  /* CIRCLE.1 - Сканируем таблицу ... */
/*- - - - - - - - - - - - - - - - - - - - - - Список LANG-переменных */
	if(SYS_EXT_TABLE[n_list].type==_LANG_STD_VARS) {

	 if(func_flag)  continue ;

	     lang_vars=SYS_EXT_TABLE[n_list].objects ;

	   for(i=0 ; *lang_vars[i].name!=0 ; i++)
	    if(!strcmp(lang_vars[i].name, name)) {
			       addr=&(lang_vars[i].value) ;
				       break ;
						 }
						       }
/*- - - - - - - - - - - - - - - - - - - - - - -  Список LANG-функций */
   else if(SYS_EXT_TABLE[n_list].type==_LANG_STD_FUNC) {

	 if(!func_flag)  continue ;

	     lang_func=SYS_EXT_TABLE[n_list].objects ;

	   for(i=0 ; *lang_func[i].name!=0 ; i++)
	    if(!strcmp(lang_func[i].name, name)) {
			       addr=lang_func[i].addr ;
		      func_pars_cnt=lang_func[i].pars_cnt ;
				       break ;
						 }
						       }
/*- - - - - - - - - - - - - - - - - - - - - - -  Список CCL-обьектов */
   else if(SYS_EXT_TABLE[n_list].type==_CCL_STD) {

	     ccl_obj=SYS_EXT_TABLE[n_list].objects ;

	   for(i=0 ; ccl_obj[i].name!=NULL ; i++)
	    if(( ccl_obj[i].func_flag &&  func_flag) ||
	       (!ccl_obj[i].func_flag && !func_flag)   )
		     if(!strcmp(ccl_obj[i].name, name)) {
			     addr=ccl_obj[i].addr  ;
				      break ;
							}
						 }
/*- - - - - - - - - - - - - - - - - - - - - - -  Список DCL-обьектов */
   else if(SYS_EXT_TABLE[n_list].type==_DCL_STD) {

	     dcl_obj=SYS_EXT_TABLE[n_list].objects ;
#if 0
	   for(i=0 ; dcl_obj[i].name!=NULL ; i++)
	    if(( dcl_obj[i].func_flag &&  func_flag) ||
	       (!dcl_obj[i].func_flag && !func_flag)   )
		     if(!strcmp(dcl_obj[i].name, name)) {
			     addr=dcl_obj[i].addr ;
				      break ;
							}
#endif
						 }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
	       if(addr!=NULL)  break ;
								 }  /* CONTINUE.1 */
/*------------------------------------- Работа с внешней библиотекой */

   if(SYS_EXT_LIB!=NULL &&                                          /* Если задана внешняя библиотека */
	     addr==NULL   ) {                                       /*     и обьект не найден ...     */

     if(func_flag) {                                                /* Если ищем функцию -               */
		     lang_func=SYS_EXT_LIB(_LOAD_FNC, name) ;       /*  - запрашиваем внешнюю библиотеку */
		  if(lang_func!=NULL) {                             /*   по имени функции и если она     */
					addr=lang_func->addr ;      /*  найдена - извлекаем ее адрес и   */
			       func_pars_cnt=lang_func->pars_cnt ;  /*   число параметров                */
				      }
		   }
     else          {                                                /* Если ищем переменную -            */
		     lang_vars=SYS_EXT_LIB(_LOAD_VAR, name) ;       /*  - запрашиваем внешнюю библиотеку */
		  if(lang_vars!=NULL)  addr=&(lang_vars->value) ;   /*   по имени переменной и если она  */
		   }                                                /*  найдена - извлекаем ее адрес     */
			    }
/*---------------------------- Занесение обьекта в 'рабочую' таблицу */

   if(addr!=NULL) {
		      func_std=SYS_EXT_TABLE[n_list].type ;

		     vars_tbl[vars_tbl_cnt]=addr ;
			      vars_tbl_cnt++ ;
		       return(vars_tbl_cnt-1) ;
		  }
/*-------------------------------------------------------------------*/

    return(-1) ;
}


/************************************************************************/
/*                                                                      */
/*           Формирователь вычислительного псевдокода                   */

  int  L_culc_code(code, oper1, oper2)

	 int  code ;    /* Код операции */
	char *oper1 ;   /* Описание 1-го операнда */
	char *oper2 ;   /* Описание 2-го операнда */

{
 union {  double  dbl ;
	   short  bin[4] ;  }  cvt ;  /* Область преобразований */

    int  n1 ;
    int  n2 ;
    int  i ;
    int  j ;

/*----------------------------------------------- Начальный контроль */

      if(SYS_CULC_CNT>=SYS_CULC_MAX-8) {
					 lng_error=_EL_CULC_BUFF ;
						return(-1) ;
				       }
/*--------------------------------- Формирование текстового сегмента */

       if(code==_TSEG_OPER) {
				    i           =SYS_CULC_CNT ;
		      SYS_CULC_CODE[i].oper_code=code ;

				      cvt.dbl=(long)oper1 ;

			    SYS_CULC_CODE[i].oper_code|=_DATA_OP1 ;
		   memmove(&SYS_CULC_CODE[i].addr1, &cvt.bin[2], 4) ;

				 SYS_CULC_CNT++ ;
				     return(0) ;
			    }
/*--------------------------------------------- Обработка индексации */

    if(oper1!=NULL && oper1[3]!='0') {
      if(code==_IND2_OPER)
	L_culc_code(_IND2_OPER, index_list[oper1[3]-'0'-1], NULL) ;
       else
	L_culc_code(_IND1_OPER, index_list[oper1[3]-'0'-1], NULL) ;

				     }

    if(oper2!=NULL && oper2[3]!='0')
	L_culc_code(_IND2_OPER, index_list[oper2[3]-'0'-1], NULL) ;

/*-------------------------------------------------- Прочие операции */

		   if(oper1!=NULL)  n1=oper1[2]-1 ;
		   if(oper2!=NULL)  n2=oper2[2]-1 ;

				    i           =SYS_CULC_CNT ;
		      SYS_CULC_CODE[i].oper_code=code ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - Первый операнд */
       if(oper1 == NULL)   SYS_CULC_CODE[i].addr1=  NULL ;
  else if(oper1[1]=='F')   SYS_CULC_CODE[i].addr1=(void *)SYS_EXT_FUNC[n1].addr ;
  else if(oper1[1]=='W')   SYS_CULC_CODE[i].addr1=&(SYS_WRK_VARS[n1]) ;
  else if(oper1[1]=='I')   SYS_CULC_CODE[i].addr1=&(SYS_INT_VARS[n1].value) ;
  else if(oper1[1]=='E')   SYS_CULC_CODE[i].addr1=&(SYS_EXT_VARS[n1].value) ;
  else if(oper1[1]=='T')   SYS_CULC_CODE[i].addr1=  vars_tbl[n1] ;
  else if(oper1[1]=='P')   SYS_CULC_CODE[i].addr1=  vars_tbl[n1] ;
  else if(oper1[1]=='R')   SYS_CULC_CODE[i].addr1= (double *)n1 ;
  else if(oper1[1]=='Z') {
				      cvt.dbl=0 ;
				SYS_CULC_CODE[i].oper_code|=_DATA_OP1 ;
		       memmove(&SYS_CULC_CODE[i].addr1, &cvt.bin[2], 4) ;
			 }
  else if(oper1[1]=='C') {
			       cvt.dbl=vars_cnst[n1] ;
	 if(cvt.bin[0]==0 &&
	    cvt.bin[1]==0   ) {
				SYS_CULC_CODE[i].oper_code|=_DATA_OP1 ;
		       memmove(&SYS_CULC_CODE[i].addr1, &cvt.bin[2], 4) ;
			      }
	 else                 {
				SYS_CULC_CODE[i].oper_code|=_LONG_OP1 ;
		       memmove(&SYS_CULC_CODE[i+1], cvt.bin, 8) ;
				 SYS_CULC_CNT++ ;
			      }
			 }
  else if(oper1[1]=='L') {
			   SYS_CULC_CODE[i].oper_code|=_SHFT_OP1 ;
			      cvt.dbl=text_addr[n1] ;
		  memmove(&SYS_CULC_CODE[i].addr1, &cvt.bin[2], 4) ;
			 }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - Второй операнд */
       if(oper2 == NULL)   SYS_CULC_CODE[i].addr2=  NULL ;
  else if(oper2[1]=='W')   SYS_CULC_CODE[i].addr2=&(SYS_WRK_VARS[n2]) ;
  else if(oper2[1]=='I')   SYS_CULC_CODE[i].addr2=&(SYS_INT_VARS[n2].value) ;
  else if(oper2[1]=='E')   SYS_CULC_CODE[i].addr2=&(SYS_EXT_VARS[n2].value) ;
  else if(oper2[1]=='T')   SYS_CULC_CODE[i].addr2=  vars_tbl[n2] ;
  else if(oper2[1]=='P')   SYS_CULC_CODE[i].addr2=  vars_tbl[n2] ;
//else                  {  SYS_CULC_CODE[i].addr2=  NULL ;
//                         SYS_CULC_CODE[i].value=vars_cnst[n2] ;  }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
			      SYS_CULC_CNT++ ;

/*------------------------------ Обработка внешней подгрузки функции */

  if(code==_CALL_OPER) {
/*- - - - - - - - - - - - - - - - - -  Вызов внутреннего ENTRY-входа */
	if(oper1[1]=='R') {
		    SYS_CULC_CODE[i].oper_code=_CALLE_OPER ;
			  }
/*- - - - - - - - - - - - - - - - - - - -  Вызов 'удаленной' функции */
   else if(SYS_CULC_CODE[i].addr1==_EXT_EXEC) {                     /* Если процедура задана как      */
								    /*  подгружаемая - заносим ее имя */
	for(j=0 ; j<SYS_INT_CNT ; j++)                              /*   в свободную рабочую ячейку   */
	  if(!strcmp(SYS_INT_VARS[j].name, extrn_name))  break ;    /*    и уст.как первый операнд    */
								    /*     операции 'внешнего' вызова */
      if(j==SYS_INT_CNT) {
		    strcpy(SYS_INT_VARS[j].name, extrn_name) ;
			    SYS_INT_CNT++ ;
			 }

	 SYS_CULC_CODE[i].oper_code=_CALLX_OPER ;
	 SYS_CULC_CODE[i].addr1    =(double *)SYS_INT_VARS[j].name;
					      }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
		       }
/*-------------------------------------------------------------------*/

   return(0) ;
}


/************************************************************************/
/*                                                                      */
/*                Обработка Escape-последовательностей                  */
/*                                                                      */
/*  Return:  Число обработанных символов                                */
/*                                                                      */
/*  Ошибки:  _EL_INV_CHAR - некорректная Escape-последовательность      */

    int  L_escape_dcd(text, chr)

     char *text ;    /* Исходный текст операнда */
     char *chr ;     /* Адрес дешифрованного символа */

{
  char *end ;   /* Указатель на первый НЕ обработанный символ */
   int  len ;   /* Длина обработанного фрагмента */

/*------------------------------- Обработка спец-символьных форматов */

			      len=1 ;                               /* Иниц.длину обр.фрагмента  */

	    if(*text=='n' )  *chr='\n' ;                            /* Обработка NL */
       else if(*text=='r' )  *chr='\r' ;                            /* Обработка RET */
       else if(*text=='t' )  *chr='\t' ;                            /* Обработка TAB */
       else if(*text=='\'')  *chr='\'' ;                            /* Обработка символа ' */
       else if(*text=='\"')  *chr='\"' ;                            /* Обработка символа " */
       else if(*text=='\\')  *chr='\\' ;                            /* Обработка символа \ */

/*---------------------------------------- Обработка 16-ричного кода */

       else if(*text=='x') {                                        /* Если есть префикс 16-ного кода ... */
			     *chr=strtol(text, &end, 16) ;          /*  Дешифруем код */

			      len=end-text ;                        /*  Опред.длину кода (с префиксом) */

			   if(len<2 ||                              /*  Код должен содержать 1 или 2 */
			      len>3   )  lng_error=_EL_INV_CHAR ;   /*    символа, иначе - ошибка    */
			   }
/*----------------------------------------- Обработка 8-ричного кода */

       else                {                                        /* Последнее предположение - */
								    /*    -   8-ричный код ...   */
			     *chr=strtol(text, &end, 8) ;           /*  Дешифруем код */

			      len=end-text ;                        /*  Опред.длину кода */

			   if(len<1 ||                              /*  Код должен содержать 1, 2 или 3 */
			      len>3   )  lng_error=_EL_INV_CHAR ;   /*    символа, иначе - ошибка    */
			   }
/*-------------------------------------------------------------------*/

   return(len) ;
}


/************************************************************************/
/************************************************************************/
/*                                                                      */
/*                          ИСПОЛНЯЮЩИЙ БЛОК                            */
/*                                                                      */
/************************************************************************/
/************************************************************************/


/************************************************************************/
/*                                                                      */
/*                          'Заявка' входа                              */
/*                                                                      */
/*   Return: 0 - все нормально,  -1 - нейзвестный вход                  */

     int  L_decl_entry(name, pars, pars_cnt)

	char *name ;      /* Имя входа */
      double *pars ;      /* Таблица параметров */
	 int  pars_cnt ;  /* Число параметров */

{
    int  entry ;
    int  i ;

/*---------------------------------------------- Идентификация входа */

    for(entry=0 ; entry<SYS_ENTRY_CNT ; entry++)
      if(!strcmp(name, SYS_ENTRY_LIST[entry].name))  break ;

	  if(entry>=SYS_ENTRY_CNT)  return(-1) ;

/*-------------------------------------------- Подготовка исполнения */

	 lng_start=SYS_ENTRY_LIST[entry].start ;                    /* Уст.стартового адреса */

     for(i=0 ; i<                      pars_cnt &&                  /* Заносим параметры */
	       i<SYS_ENTRY_LIST[entry].pars_cnt    ; i++)
	 SYS_INT_VARS[SYS_ENTRY_LIST[entry].pars[i]].value=pars[i] ;

/*-------------------------------------------------------------------*/

  return(0) ;
}


/************************************************************************/
/*                                                                      */
/*                     Вызов внутреннего входа                          */

   double  L_call_entry(entry, pars, pars_cnt)

	 int  entry ;     /* Номер входа */
      double *pars ;      /* Таблица параметров */
	 int  pars_cnt ;  /* Число параметров */

{
  double  result ;
    char *safe_area ;   /* Область сохранения */
  double *ivars_area ;  /* Область сохранения, 'участок' внутренних переменных */
     int  i ;

#define  ENTRY_START     SYS_ENTRY_LIST[entry].start
#define  ENTRY_PARS_CNT  SYS_ENTRY_LIST[entry].pars_cnt
#define  ENTRY_PARS      SYS_ENTRY_LIST[entry].pars

/*------------------------------------ Сохранение состояния процесса */

      safe_area=calloc(SYS_WRK_CNT+ENTRY_PARS_CNT,                  /* Выделяем память */
				      sizeof(*SYS_WRK_VARS));

   if(safe_area==NULL)  {   lng_ctrl_abort=1 ;                      /* При неудаче - сквозной */
			    lng_culc_abort=1 ;                      /*   сброс процессов      */
				 return(0.) ;   }

      ivars_area=(double*)(safe_area+
			    SYS_WRK_CNT*sizeof(*SYS_WRK_VARS)) ;

	   memcpy(safe_area, SYS_WRK_VARS,                          /* Сохр.рабочие переменные */
		   SYS_WRK_CNT*sizeof(*SYS_WRK_VARS)) ;

      for(i=0 ; i<ENTRY_PARS_CNT ; i++)                             /* Сохраняем ячейки параметров */
	   ivars_area[i]=SYS_INT_VARS[ENTRY_PARS[i]].value ;

/*-------------------------------------------- Подготовка исполнения */

	 lng_start=ENTRY_START ;                                    /* Уст.стартового адреса */

     for(i=0 ; i<      pars_cnt &&                                  /* Заносим параметры */
	       i<ENTRY_PARS_CNT    ; i++)
		   SYS_INT_VARS[ENTRY_PARS[i]].value=pars[i] ;

/*------------------------------------------------------- Исполнение */

	       result=L_ctrl_exec() ;

/*-------------------------------- Восстановление состояния процесса */

	   memcpy(SYS_WRK_VARS, safe_area,                          /* Восст.рабочие переменные */
		   SYS_WRK_CNT*sizeof(*SYS_WRK_VARS)) ;

      for(i=0 ; i<ENTRY_PARS_CNT ; i++)                             /* Восст.ячейки параметров */
		   SYS_INT_VARS[ENTRY_PARS[i]].value=ivars_area[i] ;

	     free(safe_area) ;

/*-------------------------------------------------------------------*/

#undef  ENTRY_START
#undef  ENTRY_PARS_CNT
#undef  ENTRY_PARS

  return(result) ;
}


/************************************************************************/
/*                                                                      */
/*          Исполняющий модуль управляющего псевдокода                  */

#define   OPER_CODE  (SYS_CTRL_CODE[i].oper_code)
#define   SOURCE     (SYS_CTRL_CODE[i].operand)
#define   ADDR       (SYS_CTRL_CODE[i].addr)

  double  L_ctrl_exec(void)

{
  double  value ;
     int  i ;

/*---------------------------------------------------- Инициализация */

			    lng_ctrl_abort=0 ;                      /* Сброс флагов завершения */
			    lng_culc_abort=0 ;

#ifdef  __INSIDE_DEBUGER__
    if(lng_user_debug==NULL)  lng_user_debug=L_debug_ctrl ;
#endif

/*------------------------------------------------ Исполнение п/кода */

  for(i=lng_start ; ; ) {
			       lng_start=0 ;                        /* Сброс стартового адреса */

    if(lng_ctrl_abort)  break ;                                     /* Если задано 'аварийное'          */
								    /*  завершения исполнения программы */
    if(lng_debug)
     if(lng_user_debug!=NULL)  lng_user_debug(i) ;

	if(OPER_CODE==   _SET_OPER) {
				      *SOURCE=ADDR ;
					    i++ ;
				    }
   else if(OPER_CODE== _ALLOC_OPER) {
	      *SOURCE=L_set_pointer((char *)SYS_CTRL_CODE+ADDR) ;
					    i++ ;
				    }
   else if(OPER_CODE==  _CULC_OPER) {
					value=L_culc_exec(ADDR) ;
		    if(SOURCE!=NULL)  *SOURCE=   value ;
					   i++ ;
				    }
   else if(OPER_CODE==    _IF_OPER) {
				      if(*SOURCE<=0)  i=ADDR ;
				      else            i++ ;
				    }
   else if(OPER_CODE==_RETURN_OPER) {
				       return(*SOURCE) ;
				    }
   else                                     i++ ;
			}
/*-------------------------------------------------------------------*/

  return(0.) ;
}


/************************************************************************/
/*                                                                      */
/*          Исполняющий модуль вычислительного псевдокода               */

  double  L_culc_exec(start)

   int  start ;  /* Стартовый адрес */

{
 struct L_culc *exe ;          /* Стартовый адрес исполнит. кода */
	double  result ;
	double  oper1 ;
	double *addr1 ;
	double *addr2 ;
	double *oper2 ;
//	 float *tmp ;
	   int  oper ;
	   int  type ;         /* Тип первого операнда */

	   int  index1  ;      /* Индекс 1-го операнда */
	   int  index2  ;      /* Индекс 2-го операнда */
	   int  index1_flag ;  /* Флаг индексации 1-го операнда */
	   int  index2_flag ;  /* Флаг индексации 2-го операнда */


    union {          void *pntr ;            /* Ячейка преобразования */
	    unsigned long  dgt ;  }  cvt ;

    union {        double  dbl ;             /* Ячейка преобразования */
		    short  bin[4] ; }  dbl ;

  double  (*function)(double *) ;


		 exe    =&SYS_CULC_CODE[start] ;
	  vars_stack_cnt=  0 ;
	  lng_ctrl_abort=  0 ;                                      /* Сброс флага завершения */

	     index1_flag=0 ;
	     index2_flag=0 ;

/*================================================ Операционный цикл */

  for( ; exe->oper_code!=_STOP_OPER ; exe++) {

    if(lng_ctrl_abort | lng_culc_abort)  break ;                    /* Если задано 'аварийное'          */
								    /*  завершения исполнения программы */
								    /*   или вычислительного фрагмента  */
	      oper =exe->oper_code & 0770 ;                         /* Извлекаем код операции */
	      type =exe->oper_code & 0007 ;                         /* Извлекаем тип 1-го операнда */

	      oper2=exe->addr2 ;                                    /* Извлекаем адрес 2-го операнда */

/*---------------------------------- Обработка типа первого операнда */

	     memset(dbl.bin, 0, sizeof(dbl.bin)) ;

	 if(type==_DATA_OP1) {                                      /* Разводим первый операнд в */
		      memcpy(&dbl.bin[2], &exe->addr1, 4) ;         /*  зависимости от его типа  */
		       oper1=dbl.dbl ;
			     }
    else if(type==_SHFT_OP1) {
		      memcpy(&dbl.bin[2], &exe->addr1, 4) ;
		       oper1=dbl.dbl+shft_base ;
			     }
    else if(type==_LONG_OP1) {
					      exe++ ;
			       memcpy(&oper1, exe, 8) ;
			     }
    else                     {
/*- - - - - - - - - - - - - - - - - - - - - - - Обработка индексации */
	  if(index1_flag     &&
	      oper!=_IND2_OPER  ) {
					 index1_flag=0 ;

				     addr1 =L_get_pointer(*exe->addr1) ;
				     addr1+=   index1 ;
				  }
     else if(index2_flag     &&
	      oper==_IND2_OPER  ) {
				      addr1 =L_get_pointer(*exe->addr1) ;
				      addr1+=   index2 ;
				   }
     else                             addr1=exe->addr1 ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
		  if(oper!= _CALL_OPER &&
		     oper!=_CALLE_OPER &&
		     oper!=_CALLX_OPER   )  oper1=*addr1 ;
			     }

/*================================================ Разводка операций */

      switch(oper) {

/*------------------------------------------------------- Индексация */

       case _IND1_OPER: {
				  index1     =oper1 ;
				  index1_flag=1 ;
					break ; }

       case _IND2_OPER: {
				  index2     =oper1 ;
				  index2_flag=1 ;
					break ; }

/*-------------------------------------------- Специальные операторы */

       case _TSEG_OPER : {   cvt.pntr =exe ;
			    shft_base =cvt.dgt+0.5 ;
				  exe+=(unsigned long)oper1 ;
					break ; }

       case _MOVE_OPER : {
			if(index2_flag) {
					   addr2 =L_get_pointer(*oper2) ;
					   addr2+=   index2 ;
					     index2_flag=0 ;
					}
			else               addr2=oper2 ;

					  *addr2=oper1 ;
					break ; }

       case _PUSH_OPER : {  vars_stack[vars_stack_cnt]=oper1 ;
				     vars_stack_cnt++ ;
					break ;  }

       case _CALL_OPER : {
			     function=(void *)exe->addr1 ;
			 lng_pars_cnt= vars_stack_cnt ;
			       *oper2=function(vars_stack) ;
				         vars_stack_cnt=0 ;
					   break ;  }

       case _CALLX_OPER : {
			      lng_pars_cnt=vars_stack_cnt ;

	   if(SYS_EXT_EXEC!=NULL)
		  *oper2=SYS_EXT_EXEC((char *)exe->addr1, vars_stack,
						 vars_stack_cnt) ;
	   else   *oper2=0. ;
					 vars_stack_cnt=0 ;
					   break ;  }

       case _CALLE_OPER : {

	     *oper2=L_call_entry((int)exe->addr1,
				   vars_stack, vars_stack_cnt) ;
					 vars_stack_cnt=0 ;
					   break ;  }

/*----------------------------------------- Арифметические операторы */

       case _MULT_OPER : {  *oper2*=oper1 ;  break ;  }

       case  _DEV_OPER : {  if(*oper2==0.)  *oper2=0 ;
			    else            *oper2=oper1/(*oper2) ;
				   break ;  }

       case _DEVC_OPER : {  if(*oper2==0.)  *oper2=0 ;
			    else            *oper2=floor(oper1/(*oper2)) ;
				   break ;  }

       case _DEVQ_OPER : {  if(*oper2==0.)  *oper2=0 ;
			    else            *oper2=floor(oper1/(*oper2)+0.5) ;
				   break ;  }

       case  _ADD_OPER : {  *oper2+=oper1 ;  break ; }

       case  _SUB_OPER : {  *oper2 =oper1-(*oper2) ;  break ; }

       case _POWER_OPER : {    if(oper1<0.)   *oper2=0 ;
				else         {
				    *oper2=pow(oper1, *oper2) ;
				  if(!errno)  *oper2=0 ;
					     }
					break ;  }
/*--------------------------------------------- Логические операторы */

       case   _EQ_OPER : {  if(oper1==(*oper2)) *oper2=1. ;
			    else                *oper2=0. ;
				   break ; }

       case   _NE_OPER : {  if(oper1!=(*oper2)) *oper2=1. ;
			    else                *oper2=0. ;
				   break ; }

       case   _LE_OPER : {  if(oper1<=(*oper2)) *oper2=1. ;
			    else                *oper2=0. ;
				   break ; }

       case   _GE_OPER : {  if(oper1>=(*oper2)) *oper2=1. ;
			    else                *oper2=0. ;
				   break ; }

       case   _LT_OPER : {  if(oper1< (*oper2)) *oper2=1. ;
			    else                *oper2=0. ;
				   break ; }

       case   _GT_OPER : {  if(oper1> (*oper2)) *oper2=1. ;
			    else                *oper2=0. ;
				   break ; }

       case  _AND_OPER : {  if(oper1!=0. && (*oper2)!=0.) *oper2=1. ;
			    else                          *oper2=0. ;
				   break ; }

       case   _OR_OPER : {  if(oper1!=0. || (*oper2)!=0.) *oper2=1. ;
			    else                          *oper2=0. ;
				   break ; }

/*-------------------------------------------------------------------*/
		   }
					     }
/*===================================================================*/

	    result=(*oper2) ;

  return(result) ;
}


/************************************************************************/
/*                                                                      */
/*                Включение/выключение режима отладки                   */

    void  L_debug_set(flag)

       int  flag ;  /* Флаг отладки */

{
     lng_debug=flag ;
}

#ifdef  __INSIDE_DEBUGER__

/************************************************************************/
/*                                                                      */
/*                Отладка управляющего кода                             */

//#include <graph.h>
#include <conio.h>
#include <stdio.h>

#define  SETTEXTPOS      _settextposition
#define  PRINT            printf
#define  CLEARSCREEN()    system("cls")

#undef      _PAGE
#define     _PAGE   15
#define _LIST_MAX   10


    void  L_debug_ctrl(n)

      int  n ;   /* Номер кадра */

{
 struct L_variable *var ;            /* Описание переменных */
	static int  list[_LIST_MAX]={-1,-1,-1,-1,-1,-1,-1,-1,-1,-1} ;
	static int  row_stop ;       /* Строка остановки */
	static int  row_prv ;        /* Номер предыдущей строки */
	static int  pause_flag=1 ;   /* Флаг разрешения паузы */
	static int  stop_flag ;
	       int  oper ;
	       int  row ;
	    double *addr ;
	       int  key ;
	       int  max ;
	       int  i ;
	       int  j ;


 static char *oper_name[]={  "NOP ",   /* Название операций */
			     "SET",
			     "CULC",
			     "IF  ",
			     "RTRN",
			     "ALLC" } ;

 static char *frame_fmt="R:%3d  F:%3d  %s  Oper:%lp  Addr:%d   " ;
 static char *  var_fmt="%3d  %15s  Value:%12.6le  Addr:%lp  " ;

 static char *continue_msg="Посмотрел - жми кнопку. ESC - монитор.  " ;

#define  _HELP_MAX  14
  static char *help[]={
			    "Система команд отладчика:                ",
			    " R [N]  - делать до строки N             ",
			    " S      - список системных переменных    ",
			    " E      - список внешних переменных      ",
			    " I      - список внутрен.переменных      ",
			    " L      - особый список переменных       ",
			    "+{E|I}N - включить внутрен./внешнюю      ",
			    "     переменную номер N по соотв.списку  ",
			    "     в особый список переменных          ",
			    " P+     - делать с паузами               ",
			    " P-     - делать без пауз                ",
			    " X      - делать до конца                ",
			    " Y      - завершить исполнение кода      ",
			    " Z      - завершить программу            ",
			    " C      - очистить экран                 ",
			    "                                         "
		      } ;

/*---------------------------------------------------- Инициализация */

	   oper=SYS_CTRL_CODE[n].oper_code ;
	   addr=SYS_CTRL_CODE[n].operand ;
	    row=SYS_CTRL_CODE[n].text_row ;

	 if(n==0) {  stop_flag= 1 ;
		      row_stop=-1 ;
		       row_prv=-1 ;  }

/*----------------------------------------------- Обработка останова */

	      if(row_prv!=-1 &&
		 row_prv!=row  ) {
					 row_prv=-1 ;
		     if(row_stop==-1)  stop_flag= 1 ;
		     else              stop_flag= 0 ;
				 }

	      if(row_prv ==-1 &&
		 row_stop==row  ) {
					row_stop=-1 ;
				       stop_flag= 1 ;
				  }

		  if(!pause_flag)  stop_flag=0 ;
/*- - - - - - - - - - - - - - - - - - - - - - Контроль кода операции */
	  if(oper<_NOP_OPER  ||
	     oper>_ALLOC_OPER  ) {
		    SETTEXTPOS(3, 1) ;
		         PRINT("Накрытие --- Ошибка кода операции  ") ;
				     stop_flag=1 ;
				 }
/*- - - - - - - - - - - - - - - - - - - - - Контроль адреса операнда */
	 if(SYS_EXT_TABLE==NULL)
	  if(oper!=_NOP_OPER)
	      if((addr< &(SYS_EXT_VARS[0].value) ||
		  addr>=&(SYS_EXT_VARS[SYS_EXT_VCNT].value))    &&
		 (addr< &(SYS_INT_VARS[0].value) ||
		  addr>=&(SYS_INT_VARS[SYS_INT_MAX].value)) &&
		  addr!=&vars_if_ind                        &&
		  addr!=&vars_go_ind                        &&
		  addr!=&vars_tmp                             ) {
		    SETTEXTPOS(3, 1) ;
		         PRINT("Накрытие --- Ошибка адреса операнда  ") ;
			    stop_flag=1 ;
								}
/*-------------------------------- Вывод информации и запрос команды */

       SETTEXTPOS(2, 1) ;
	    PRINT(frame_fmt, row, n, oper_name[oper],
		     SYS_CTRL_CODE[n].operand, SYS_CTRL_CODE[n].addr) ;

        for(j=0 ; j<_LIST_MAX ; j++) {
	          if(list[j]==-1 )   continue ;
	     else if(list[j]< 300)  var=&SYS_EXT_VARS[list[j]] ;
	     else                   var=&SYS_INT_VARS[list[j]-10000] ;

		SETTEXTPOS(j+4, 1) ;
		     PRINT(var_fmt, j, var->name,
				       var->value,
				      &var->value ) ;
				     }


	       if(!stop_flag)  return ;

   do {
	     SETTEXTPOS(1, 1) ;
		  PRINT("Ctrl-debug(Help-?) >                  ") ;
	     SETTEXTPOS(1, 22) ;
		   gets(buff) ;

/*------------------------------------------------- Обработка команд */

		  strupr(buff) ;                                    /* Перевод в верхний регистр */

/*- - - - - - - - - - - - - - - - - - - - - - - - - -  Следующий шаг */
	if(buff[0]==0) {
			   return ;
		       }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - Help */
   else if(buff[0]=='?') {
	  for(i=0 ; i<_HELP_MAX ; i++) {  SETTEXTPOS(i+4, 1) ;
					       PRINT(help[i]) ;  }
			 }
/*- - - - - - - - - - - - - - - - - - - -  Включение/выключение пауз */
   else if(buff[0]=='P') {
			    if(buff[1]=='+')  pause_flag=1 ;
			    else              pause_flag=0 ;
			 }
/*- - - - - - - - - - - - - - - - - - - - - - - - -  Проход до конца */
   else if(buff[0]=='X') {
			      stop_flag=0 ;

			      return ;
			 }
/*- - - - - - - - - - - - - - - - - - - - - - -  Завершить программу */
   else if(buff[0]=='Y') {
			    lng_ctrl_abort=1 ;
			     return ;
			 }
/*- - - - - - - - - - - - - - - - - - - - - - -  Завершить программу */
   else if(buff[0]=='Z') {
			    _exit(77) ;
			 }
/*- - - - - - - - - - - - - - - - - - - - - - -  Завершить программу */
   else if(buff[0]=='C') {
			    CLEARSCREEN() ;
			 }
/*- - - - - - - - - - - - - - - - - - - - - - - - - Проход до строки */
   else if(buff[0]=='R') {
					  stop_flag= 0 ;
					   row_prv =row ;
			   if(buff[1]!=0)  row_stop=atoi(&buff[1]) ;

			      return ;
			 }
/*- - - - - - - - - - - - - - -  Вывод значений системных переменных */
   else if(buff[0]=='S') {

		SETTEXTPOS(4, 1) ;
		     PRINT(var_fmt, 0, "IF indicator",
					 vars_if_ind,
					&vars_if_ind ) ;

		SETTEXTPOS(5, 1) ;
		     PRINT(var_fmt, 0, "GO indicator",
					 vars_go_ind,
					&vars_go_ind ) ;

		SETTEXTPOS(6, 1) ;
		     PRINT(var_fmt, 0, "Work value",
					 vars_tmp,
					&vars_tmp ) ;
			 }
/*- - - - - - - - - - - - - - - - - - - -  Вывод значений переменных */
   else if(buff[0]=='E' ||
	   buff[0]=='L' ||
           buff[0]=='I'   ) {
		  if(buff[0]=='E')  max= SYS_EXT_VCNT ;
	     else if(buff[0]=='I')  max= SYS_INT_MAX ;
	     else                   max=_LIST_MAX ;

	    for(i=0 ; i<max ; i+=_PAGE) {
	      for(j=0 ; j<_PAGE && i+j<max ; j++) {

		  if(buff[0]=='E')  var=&SYS_EXT_VARS[i+j] ;
	     else if(buff[0]=='I')  var=&SYS_INT_VARS[i+j] ;
	     else if(list[j]==-1 )   continue ;
	     else if(list[j]< 300)  var=&SYS_EXT_VARS[list[j]] ;
	     else                   var=&SYS_INT_VARS[list[j]-10000] ;

		SETTEXTPOS(j+4, 1) ;
		     PRINT(var_fmt, i+j, var->name,
					 var->value,
					&var->value ) ;
						  }
	      if(i+j<max) {
		SETTEXTPOS(j+4, 10) ;
		     PRINT(continue_msg) ;
		     key=getch() ;
		  if(key==27) {  i=max ;  break ;  }
			  }
					}
			    }
/*- - - - - - - - - - - - - - - - -  Ввод переменных в особый список */
   else if(buff[0]=='+') {

	  for(i=0 ; i<_LIST_MAX ; i++) if(list[i]==-1) break ;

	    if(i==_LIST_MAX) {
		    SETTEXTPOS(3, 1) ;
		         PRINT("ERROR --- LIST OVERFLOW  ") ;
			     }

			  j=atoi(&buff[2]) ;

                  if(buff[1]=='E')  list[i]=j  ;
		  else              list[i]=j+10000 ;
			 }
/*- - - - - - - - - - - - - - - - Вывод переменных из особого списка */
   else if(buff[0]=='-') {
			        i =atoi(&buff[1]) ;
                           list[i]= -1  ;
			 }
/*- - - - - - - - - - - - - - - - - -  Обработка неизвестной команды */
   else                     {
			  SETTEXTPOS(3, 1) ;
			       PRINT("ERROR --- UNKNOWN COMMAND  ") ;
			    }
/*-------------------------------------------------------------------*/

      } while(1) ;

}
#endif  /*   __INSIDE_DEBUGER__  */


/************************************************************************/
/*                                                                      */
/*                   Кодирование/декодирование адреса                   */

     double  L_set_pointer(pntr)

       void *pntr ;

{
    union {          void *pntr ;            /* Ячейка преобразования */
	    unsigned long  dgt ;  }  cvt ;

	 cvt.pntr=pntr ;
  return(cvt.dgt) ;
}


       void *L_get_pointer(pntr)

	 double  pntr ;

{
    union {          void *pntr ;            /* Ячейка преобразования */
	    unsigned long  dgt ;  }  cvt ;

	 cvt.dgt=pntr ;
  return(cvt.pntr) ;
}

