/*   Bolotov P.D.  version  29.04.1994   */
/*     0 warnings at warning level 1     */

/*********************************************************************/
/*                                                                   */
/*                   DATA CONVERTION LANGUAGE                        */
/*                                                                   */
/*                     ВЫЧИСЛИТЕЛЬ ВЫРАЖЕНИЙ                         */
/*                                                                   */
/*********************************************************************/

#include  <errno.h>
#include  <stddef.h>
#include  <stdlib.h>
#include  <string.h>
#include  <ctype.h>
#include  <math.h>
#include  <malloc.h>


#define DCL_INSIDE

#include "dcl.h"

/*-------------------------------------------- Специлаьные параметры */

#define    _SYS_BASE      '0'
#define      _Q_SHIFT    0.49    /* Сдвиг квантования */

/*------------------------------------------------ Описание операций */

#define   _ADD_OPER  101
#define   _SUB_OPER  102
#define   _MUL_OPER  103
#define   _DEV_OPER  104
#define   _POW_OPER  105
#define   _LNK_OPER  106
#define   _INC_OPER  111
#define   _DEC_OPER  112
#define   _NEG_OPER  121

#define    _LT_OPER   201
#define    _LE_OPER   202
#define    _EQ_OPER   203
#define    _NE_OPER   204
#define    _GE_OPER   205
#define    _GT_OPER   206

#define    _OR_OPER   301
#define   _AND_OPER   302
#define   _INV_OPER   303

#define   _AOR_OPER   401

typedef struct {                  /* Описание операций процессора */
                 char *name ;    /* Имя операции */
                  int  len ;     /* Длина имени операции */
                  int  code ;    /* Код операции */
                  int  type ;    /* Тип операции */
               } Dcl_proc ;

#define  _BINARY_OT   1     /* Бинарная операция */
#define   _UNARY_OT   2     /* Унарная операция */
#define  _PREFIX_OT   4     /* Префиксная унарная операция */

/*  Внимание! Литерал операции должен состоять из 1 или 2   */
/* символов. Новый символ следует внести в макрос OPER_CHAR */
/* в файле DCL.H .                                          */

#define  _PROC_MAX  18
 static Dcl_proc dcl_proc[_PROC_MAX]={  /* Описание допустимых операций */
       "++",       2,  _INC_OPER,  _UNARY_OT,
       "--",       2,  _DEC_OPER,  _UNARY_OT,
       "!",        1,  _INV_OPER,  _UNARY_OT | _PREFIX_OT,
       "^",        1,  _POW_OPER, _BINARY_OT,
       "/",        1,  _DEV_OPER, _BINARY_OT,                          /* Внимание! Приоритет деления должен */
       "*",        1,  _MUL_OPER, _BINARY_OT,                          /*   быть больше приоритета умножения */
       "-",        1,  _SUB_OPER, _BINARY_OT | _UNARY_OT | _PREFIX_OT, /* Внимание! Приоритет вычитания должен */
       "+",        1,  _ADD_OPER, _BINARY_OT,                          /*      быть больше приоритета сложения */
       "@",        1,  _LNK_OPER, _BINARY_OT,
       "==",       2,   _EQ_OPER, _BINARY_OT,
       "!=",       2,   _NE_OPER, _BINARY_OT,
       "<=",       2,   _LE_OPER, _BINARY_OT,
       ">=",       2,   _GE_OPER, _BINARY_OT,
       "<",        1,   _LT_OPER, _BINARY_OT,
       ">",        1,   _GT_OPER, _BINARY_OT,
       "&&",       2,  _AND_OPER, _BINARY_OT,
       "||",       2,   _OR_OPER, _BINARY_OT,
       "|",        1,  _AOR_OPER, _BINARY_OT
                                     } ;

/*-------------------------------------------- Переменные процессора */

//       int   dcl_result_dst ;  /* Ячейка размещения результата: */
	  		         /*             0 - dcl_result    */
	                         /*             1 - 1-ый операнд  */
                                 /*             2 - 2-ый операнд  */

/*---------------------------------------- Система внешней подгрузки */

#define _EXT_LOAD  (void *)0x0000ffffL   /* 'Адрес' подгружаемого обьекта */

#ifdef  _EXT_LIBS

#define  _LOAD_MAX  20

    Dcl_decl *dcl_ext_list[_LOAD_MAX] ;        /* Список заголовков подгруженных обьектов */
    int       dcl_ext_cnt ;                    /* Счетчик подгруженных обьектов */

#endif

/*------------------------------------------------------ Обозначения */

//#define DGT_SZ     sizeof(double)
#define CHR_SZ     sizeof(char)
#define TTL_SZ     sizeof(nResult)

#define O_TYPE     operand->type
#define O_ADDR     operand->addr
#define O_SIZE     operand->size
#define O_BUFF     operand->buff
#define O_WORK     operand->work_nmb
#define O1_TYPE    operand1->type
#define O1_ADDR    operand1->addr
#define O1_SIZE    operand1->size
#define O1_BUFF    operand1->buff
#define O2_TYPE    operand2->type
#define O2_ADDR    operand2->addr
#define O2_SIZE    operand2->size
#define O2_BUFF    operand2->buff

#define R_TYPE     nResult.type
#define R_ADDR     nResult.addr
#define R_SIZE     nResult.size
#define R_BUFF     nResult.buff

#define P_TYPE       result->type
#define P_ADDR       result->addr
#define P_SIZE       result->size
#define P_BUFF       result->buff
#define P_RESULT     result

/*-------------------------------------------------------------------*/


/*********************************************************************/
/*                                                                   */
/*                      Процессор выражений                          */

  char *Lang_DCL::iCulculate(char *string)

{
	int  sub_flag ;     /* Флаг возможности наличия подвыражения в скобках */
	int  string_flag ;  /* Флаг нахождения внутри символьной строки */
       char *beg ;          /* Указатель на открывающую скобку */
       char *end ;          /* Указатель на закрывающую скобку */
	int  cnt ;          /* Число скобок */
	int  end_len ;      /* Длина концовки строки */
	int  len ;
       char *addr ;

/*---------------------------------------------------- Инициализация */

   if(string==NULL) {  nIf_ind=0 ;                                  /* Обработка особой ситуации */
			return(NULL) ;  }

		    iNext_title(_CLEAR_WORK_VAR) ;                  /* Сброс счетчика рабочих ячеек */
		     iAlloc_var(_CLEAR_WORK_VAR, NULL, 0) ;         /* Очистка рабочих буферов */

#ifdef  _EXT_LIBS
      for( ; dcl_ext_cnt ; dcl_ext_cnt--)                           /* Сброс ранее подгруженных обьектов */
	dcl_ext_list[dcl_ext_cnt-1]->addr=_EXT_LOAD ;
#endif

/*-------------------------------------------------- Контроль скобок */

       string_flag=0 ;
	  sub_flag=0 ;
	       cnt=0 ;

   for(addr=string ; *addr ; addr++) {                              /* CIRCLE.1 - Сканируем строку */
/*- - - - - - - - - - - - - - - - - - - Прохождение символьных строк */
    if(*addr=='"')  if(string_flag) {                               /* Идентификация и обработка */
		     if(*(addr-1)!='\\' ||
			*(addr-2)=='\\'   ) string_flag=0 ;         /*   открытия и закрытия     */
				    }                               /*    символьной строки      */
		    else                    string_flag=1 ;

    if(string_flag)  continue ;                                     /* Проход симв.строки */
/*- - - - - - - - - - - - - - - - - - - -  Прохождение кодовых групп */
      if(*addr=='?')  {  addr+=1 ;  continue ;  }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - Подсчет скобок */
	      if(*addr=='(') {  cnt++ ;  sub_flag=1 ;  }            /* Подсчитываем скобки */
	      if(*addr==')') {  cnt-- ;  sub_flag=1 ;  }

	       if(cnt<0)  break ;                                   /* Если закрывающих больше - ошибка */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
				     }                              /* CONTINUE.1 */

     if(cnt>0) {  mError_code=_DCLE_OPEN ;   return(NULL) ;  }      /* Обработка несоответствия скобок */
     if(cnt<0) {  mError_code=_DCLE_CLOSE ;  return(NULL) ;  }

/*--------------------------------- Обработка подвыражений в скобках */

	       len=strlen(string) ;

 if(sub_flag)                                                       /* IF.1 - Если возможны подвыражения ... */
  for(end=string ; ; end++) {                                       /* CIRCLE.2 - Цикл по числу скобок */
	      end=iFind_close(end, _ANY_CLOSE) ;                    /* Ищем любую внутреннюю закр.скобку */
	   if(end==NULL)  break ;                                   /* Если таковых больше нет - выход */

// ??????  if(end[-1]=='?') continue ;

	      beg=iFind_open(string, end-1, '(') ;                  /* Ищем парную скобку */

	 if(beg!=string)
	   if(NAME_CHAR(*(beg-1)))  continue ;                      /* Если перед скобкой находится    */
								    /*  алф-цифр.символ - это функция, */
								    /*   мы ее игнорируем              */
		 *end=0 ;                                           /* Терминируем подвыражение в скобках */

	       addr=iSimple_string(beg+1) ;                         /* Обрабат.выражение в скобках */
	   if(mError_code)  return(NULL) ;                          /* Если есть ошибки - _QUIT-выход */

	       end_len=strlen(end+1)+1 ;                            /* Определяем длинны "хвоста",  */
		   len=strlen(addr) ;                               /* обработанного выражения      */

		memmove(beg+len, end+1, end_len) ;                  /* "Поджимаем" исходное выражение */
		memmove(beg, addr, len) ;                           /* Переносим обраб.выраж. в исходное */

		  end=beg+len-1 ;                                   /* Коррекция указателя закр.скобки */
	   }                                                        /* CONTINUE.2 */
								    /* END.1 */
/*--------------------------------- Обработка "суммарного" выражения */

	      iSimple_string(string) ;                              /* Обрабатываем итоговое выражение */
	   if(mError_code)  return(NULL) ;                          /* Если есть ошибки - _QUIT-выход */

/*-------------------------------------------------------------------*/

  return(NULL) ;
}


/*********************************************************************/
/*                                                                   */
/*                Обработка простых строк                            */
/*  Return: Имя ячейки результата обработки или NULL - при ошибке    */

   char *Lang_DCL::iSimple_string(char *string)

{
      Dcl_decl  operand1 ;  /* Описание переменной/процедуры */
      Dcl_decl  operand2 ;  /* Описание переменной/процедуры */
      Dcl_decl *work ;      /* Описание рабочей ячейки */

	   int  oper_flag ;    /* Флаг наличия операций */
	   int  code ;         /* Код операции, предварительный */
	   int  oper_code ;    /* Код операции, оканчательный */
	  char *oper_name ;    /* Имя операции */
	   int  oper_len ;     /* Длина имени операции */
	   int  type ;         /* Тип операции */
	  char *first ;        /* Первый операнд */
	  char *second ;       /* Второй операнд */
	  char *end ;          /* Oкончание второго операнда */
	  char *oper_pos ;     /* Положение литерала операции */
	   int  dst_nmb ;      /* Номер рабочей ячейки результата */
	   int  str_len ;      /* 'Начальная' длина строки */

	  char *addr ;
//	  char  next ;
	   int  len ;
	   int  i ;
//	   int  j ;

   static char  work_name[]={'?', '0', 0} ;

#define  _WORK_SIZE   2

/*---------------------------------------------- Общая инициализация */

      if(*string==0) {  mError_code=_DCLE_EMPTY ;  return(NULL) ; } /* Обработка пустой строки */

	 addr=iFind_close(string, _ANY_OPER) ;                      /* Ищем любой символ операции */
      if(addr!=NULL)  oper_flag=1 ;                                 /* По результату поиска уст. */
      else            oper_flag=0 ;                                 /*   флаг наличия операций   */

          memset(&operand1, 0, sizeof(operand1)) ;
          memset(&operand2, 0, sizeof(operand2)) ;

/*----------------------------------------------- Обработка операций */

      if(oper_flag) {                                               /* IF.1 - При наличии операций */

	for(i=0 ; i<_PROC_MAX ; i++) {                              /* CIRCLE.1 - Сканируем по операциям */

			    code=dcl_proc[i].code ;                 /* Уст.код текущей операции */
		       oper_name=dcl_proc[i].name ;                 /* Уст.имя текущей операции */
			oper_len=dcl_proc[i].len ;                  /* Уст.длину имени текущей операции */
			    type=dcl_proc[i].type ;                 /* Уст.тип текущей операции */

	 for(oper_pos=string ; ; oper_pos++) {                      /* CIRCLE.2 - Работаем текущую операцию */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - Поиск операций */
		 oper_pos=iFind_close(oper_pos, oper_name[0]) ;     /* Ищем вход первого символа имени */
	      if(oper_pos==NULL)  break ;                           /* При отсутствии - след.операция */

	      if(oper_len==1) {
		   if((  oper_pos!=string &&
		       OPER_CHAR(oper_pos[-1])) ||
		       OPER_CHAR(oper_pos[ 1])    )  continue ;
			      }
	      else            {
		    if(oper_pos[1]!=oper_name[1]) continue ;
			      }

                       mError_position=oper_pos ;                   /* Уст.указатель ошибки на операцию */
			       str_len=strlen(string)+1 ;           /* Опр.длину строки */
/*- - - - - - - - - - - - - - - - - - - - Выделение первого операнда */
       if(oper_pos!=string) {                                       /* IF.2 - Если ожидается 1-й операнд ... */
	    first=iFind_open(string, oper_pos-1, _ANY_OPER_OR_END); /* Ищем начало операнда */
					         
	 if(first==NULL) {  mError_code=_DCLE_BAD_LEFT ;            /* Если начало на найдено - ошибка */
				   return(NULL) ;        }
//          first++ ;                                               /* Корректируем указатель на начало */

	 if(first==oper_pos)  first=NULL ;                          /* Если операнд отсутствует */
			    }                                       /* ELSE.2 */
       else                 {                                       /* ELSE.2 - Если 1-го операнда нет ... */
			      first=NULL ;
			    }                                       /* END.2 */
/*- - - - - - - - - - - - - - - - - - - - Выделение второго операнда */
	   second=oper_pos+oper_len ;                               /* Уст.начало 2-го операнда */
	      end=iFind_close(second, _ANY_OPER_OR_END) ;           /* Ищем окончание операнда */
	   if(end==NULL) {  mError_code=_DCLE_BAD_RIGHT ;           /* Если окончание на найдено - ошибка */
				   return(NULL) ;         }

	   if(end<second)  second=NULL ;  //??????????              /* Если операнд отсутствует ... */
/*- - - - - - - - - - - - - - - - - - - - - - Контроль типа операции */
	if(first==NULL && second==NULL) mError_code=_DCLE_EMPTY_BOTH; /* Если операндов нет вообще - ошибка */
   else if(first==NULL || second==NULL) {                           /* Если унарная операция ... */
	 if(type & _UNARY_OT) {                                     /*   Если унарная операция допустима ... */
	  if((type & _PREFIX_OT) &&                                 /*     Если операция постфиксная,  */
	      second==NULL         )  mError_code=_DCLE_PREFIX_OPER;  /*      а м/б только префиксная... */
			      }
	 else                         mError_code=_DCLE_BINARY_OPER ; /*   Если д/б бинарная операция... */
					}
   else                                 {                           /* Если бинарная операция ... */
	    if(!(type & _BINARY_OT))  mError_code=_DCLE_UNARY_OPER ;  /*   Если д/б унарная операция... */
					}

	 if(mError_code)  return(NULL) ;
/*- - - - - - - - - - - - - - - - - - Обработка унарной операции '-' */
    if( code==_SUB_OPER &&
       first==  NULL      )  oper_code=_NEG_OPER ;
    else                     oper_code=  code ;
/*- - - - - - - - - - - - - - - - - - - - - - -  Выполнение операции */
  if(first!=NULL)  {                                                /* 'Извлечение' операндов, */
		     iComplex_var(first, oper_pos-1, &operand1) ;   /*   причем, если операнд  */
  if(!mError_code)                                                  /*    только один, то он   */
   if(second!=NULL)  iComplex_var(second,       end, &operand2) ;   /*   извлекается в первый  */
		   }                                                /*      операнд            */
  else             {
                     iComplex_var(second,       end, &operand1) ;
                   }

	     if(mError_code)  return(NULL) ;                        /* Если есть ошибки ... */

	         iProcessor(oper_code, &operand1, &operand2) ;      /* Выполняе операцию */

	     if(mError_code)  return(NULL) ;                        /* Если есть ошибки ... */

	if(nResult_dst==1) {                                        /* Если результат записан */
				   dst_nmb=operand1.work_nmb ;      /*  в один из операндов - */
				      work=iGet_work(dst_nmb) ;     /*   копируем их в соотв. */
			       memcpy(work, &operand1, TTL_SZ) ;    /*     рабочую ячейку     */
			   }
   else if(nResult_dst==2) {
				   dst_nmb=operand2.work_nmb ;
				      work=iGet_work(dst_nmb) ;
			       memcpy(work, &operand2, TTL_SZ) ;
			   }
   else                    {                                        /* Если результат - в сист.ячейке ...  */
				  work=iNext_title(_WORK_VAR) ;     /*   Получаем адрес заголовка свободной */
				if(mError_code)  return(NULL) ;     /*   Обраб.отсутствия памяти */
			       dst_nmb=work->work_nmb ;             /*   Уст.номер рабочей ячейки и        */
		   memcpy(work, &nResult, sizeof(nResult)) ;        /*    заносим туда описание результата */
			work->work_nmb=dst_nmb ;                    /*   Восст.номер рабочей ячейки */
			   }

			if(mError_code)  return(NULL) ;

			work_name[1]=_SYS_BASE+dst_nmb ;            /* Уст.имя ячейки результата */

     if(first==NULL)  first=oper_pos ;                              /* Если первого операнда нет ... */
			len=str_len-(end-string) ;
		      memmove(first+_WORK_SIZE, end+1, len) ;       /* Поджимаем строку выражения */
		      memmove(first, work_name, _WORK_SIZE) ;       /* Вставляем имя результата */

		   oper_pos=first+_WORK_SIZE-1 ;                    /* Уст.начало необр.участка */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
					     }                      /* CONTINUE.2 */

				     }                              /* CONTINUE.1 */
		    }                                               /* END.1 */
/*--------------------------- Обработка 'суммарной' ячейки выражения */

		len=strlen(string) ;
	   iComplex_var(string, string+len-1, &operand1) ;          /* Анализируем 'суммарную' ячейку */

	 if(mError_code)  return(NULL) ;                            /* Если есть ошибки ... */

	    memcpy(&nResult, &operand1, sizeof(nResult)) ;          /* Переносим результат в ячейку рез. */

      if(nResult.work_nmb)   dst_nmb=nResult.work_nmb ;             /* Если ячейка - рабочая ... */
      else                 {                                        /* Если ячейка не рабочая ... */
				  work=iNext_title(_WORK_VAR) ;     /*   Получаем адрес заголовка свободной */
				if(mError_code)  return(NULL) ;     /*   Обраб.отсутствия памяти */
			       dst_nmb=nWork_cnt ;                  /*    рабочей ячейки и заносим туда     */
		   memcpy(work, &nResult, sizeof(nResult)) ;        /*      описание результата             */
			work->work_nmb=nWork_cnt ;                  /*   Восст.номер рабочей ячейки */
			   }

			work_name[1]=_SYS_BASE+dst_nmb ;            /* Уст.имя ячейки результата */

/*-------------------------------------------------------------------*/

  return(work_name) ;
}


/************************************************************************/
/*                                                                      */
/*           Обработка произвольной переменной или процедуры            */
/*                                                                      */
/*    beg       -  Начало обраб. фрагмента                              */
/*    end       -  Окончание обраб. фрагмента                           */
/*    operand   -  Описание операнда                                    */
/*                                                                      */
/*    123.45          - Числовая константа                              */
/*    "..."           - Символьная строка - константа                   */
/*    NULL            - NULL-указатель                                  */
/*    ?N? (3 символа) - Внутрисистемная переменная номер N              */
/*    Name            - Простая переменная                              */
/*                                                                      */
/*    Name[Index]     - Элемент массива или строки:                     */
/*                        Name-массив или указатель, Index-число        */
/*    Name[Index]     - Указатель на строку:                            */
/*                        Name-строка, Index-строка                     */
/*                                                                      */
/*    Name[Begin<Sb>...<Se>End] - Указатель на фрагмент строки :        */
/*           Begin - Численный индекс или строка с которой              */
/*                     начинается фрагмент                              */
/*                      (при отсутствии - с начала строки)              */
/*             End - Численный индекс или строка которой                */
/*                     завершается фрагмент                             */
/*                      (при отсутствии - до конца строки)              */
/*            <Sb> - Входной спецификатор включения: +/-                */
/*                    Устанавливает включается ли начальная строка      */
/*                     в состав фрагмента                               */
/*                      (при отсутствии считается как +)                */
/*            <Se> - Выходной спецификатор включения: +/-               */
/*                    Устанавливает включается ли концовка              */
/*                     в состав фрагмента                               */
/*                      (при отсутствии считается как +)                */
/*                                                                      */
/*    Операции над объектами комплексного типа:                         */
/*                                                                      */
/*    Name[Index].Elem  - Получить доступ к элементу записи Index       */
/*    Name.Elem         - Получить доступ к элементу текущей записи     */
/*                                                                      */
/*    ОСТОРОЖНО - ВОЗМОЖНА РЕКУРСИЯ !!!                                 */
/*     Некоторые действия выполняются повторно для корректной           */
/*      обработки рекурсии !                                            */
/*                                                                      */
/*   Возвращает адрес заголовка простой переменной                      */

   Dcl_decl *Lang_DCL::iComplex_var(char *beg, char *end, Dcl_decl *operand)

{
           Dcl_decl  *head ;       /* Описание переменной/процедуры */
           Dcl_decl  *par ;        /* Описание параметра */
           Dcl_decl **pars_table ; /* Начало текущей таблицы параметров */
           Dcl_decl  *var ;        /* Заголовок возвращаемой переменной */
           Dcl_decl  *x_object ;   /* Ссылка на X-объект, использующий метод */
 Dcl_complex_record  *x_record ;   /* Ссылка на запись X-объекта */
                int   chr_save ;   /* Ячейка сохранения символа */
                int   name_len ;   /* Длина имени */
                int   type ;       /* Базовый тип переменной или процедуры */
                int   mode ;       /* Уточненный тип переменной или процедуры */
                int   r_type ;     /* Базовый тип ресультата */
                int   fragm_flag ; /* Флаг фрагментарного индекса */
                int   inc1_flag ;  /* Входной спецификатор включения */
                int   inc2_flag ;  /* Выходной спецификатор включения */
               char  *index ;      /* Начало индексного выражения */
               char  *pars ;       /* Начало параметров процедуры */
                int   pars_cnt ;   /* Счетчик параметров */
                int   proto_flag ; /* Флаг контроля типов параметров */
                int   proto_type ; /* Предполагаемый тип параметра */
               char  *f_sep ;      /* Разделитель 'фрагменттарного' индекса */
           Dcl_decl   start ;      /* Описание остатка диапазона */
               char  *begin ;      /* Адрес входа диапазона */
               char  *addr ;
               char  *stop ;
             double   value ;
             double  *dbl  ;
                int   up_len ;     /* Длина чего-то перед обьектом */
                int   len ;
                int   cnt ;
                int   i ;
                int   j ;

   Dcl_decl *(*dcl_proc)(Lang_DCL *, Dcl_decl **, int) ;

/*---------------------------------------------------- Инициализация */
/*-------------------------------------------------------------------*/

	 mError_position= beg ;
		     var=&nTmp ;
		   f_sep= NULL ;
	      fragm_flag=   0 ;

	  *operand->name=   0 ;

/*------------------------------------ Символьная строка - константа */
/*-------------------------------------------------------------------*/

       if(*beg=='"') {                                              /* IF.1 - Если символьная строка ... */
			   head=iNext_title(_WORK_VAR) ;            /* Получаем адрес заголовка свободной */
								    /*     рабочей переменной             */
			if(mError_code)  return(NULL) ;             /* Обраб.отсутствия памяти */

				  len= end-beg-1 ;
			   head->type=_CHR_AREA ;                   /* Задаем тип и размер */
			   head->buff= len ;                        /*  константы          */

		      addr=(char *)iAlloc_var(_WORK_VAR, head, 0) ; /* Выделяем память под константу */
                   if(mError_code)  return(NULL) ;                  /* Обработка отсутствия памяти */

	  for(i=1, j=0 ; i<=len ; i++, j++) {                       /* CIRCLE.1 - Сканируем константу ... */
	      if(beg[i]=='\\') {                                    /* Если индиф.спец-символ ... */
			   i++  ;                                   /*   Смотрим следующий символ */
		    if(beg[i]=='n' )  addr[j]='\n' ;                /*   Обработка NL */
	       else if(beg[i]=='r' )  addr[j]='\r' ;                /*   Обработка RET */
	       else if(beg[i]=='t' )  addr[j]='\t' ;                /*   Обработка TAB */
	       else if(beg[i]=='\'')  addr[j]='\'' ;                /*   Обработка символа ' */
	       else if(beg[i]=='\"')  addr[j]='\"' ;                /*   Обработка символа " */
	       else if(beg[i]=='\\')  addr[j]='\\' ;                /*   Обработка символа \ */
	       else if(beg[i]=='x') {                               /*   Обработка 16-ричного кода символа, */
		       addr[j]=(char)strtol(beg+i+1, &stop, 16) ;   /*    код которого должен содержать     */
		   if(stop==beg+i+1 ||                              /*     1 или 2 цифры                    */
		      stop>=beg+i+4   )  mError_code=_DCLE_INVALID_CHR ;
			i=stop-beg-1 ;
				    }
	       else                  {                              /*   Обработка 8-ричного кода символа, */
		       addr[j]=(char)strtol(beg+i, &stop, 8) ;      /*    код которого должен содержать    */
		   if(stop==beg+i  ||                               /*     1, 2 или 3 цифры                */
		      stop>=beg+i+4  )  mError_code=_DCLE_INVALID_CHR ;
			i=stop-beg-1 ;
				     }
			       }
	      else                    addr[j]=beg[i] ;              /* Обработка 'простого' символа */

               if(mError_code)  return(NULL) ;                      /* При обнаружении ошибок прекр.обработку */
					    }                       /* CONTINUE.1 */

			  O_TYPE=_CHR_AREA ;                        /* Устан. тип, адрес  */
			  O_ADDR= addr ;                            /*   и длину операнда */
			  O_BUFF= len ;
			  O_SIZE=  j ;
			  O_WORK=  0 ;
		     }                                              /* END.1 */
/*--------------------------------------- Внутрисистемная переменная */
/*-------------------------------------------------------------------*/

  else if(*beg=='?') {                                              /* IF.2 - Если сист.переменная ... */
			       i=beg[1]-_SYS_BASE ;                 /* Извлекаем номер рабочей ячейки */
			    head=iGet_work(i) ;                     /* Получаем адрес ее заголовка */
			memcpy(operand, head, sizeof(*head)) ;      /* Копируем описание переменной на выход */
		     }                                              /* END.2 */
/*---------------------------------------- Объекты комплексного типа */
/*-------------------------------------------------------------------*/

  else if(iXobject_check(beg, end, &index)) {

                        *index= 0 ;                                 /* Терминируем имя объекта */

/*------------------------------------------------------ Метод CLEAR */

     if(        end-index==strlen("clear")  &&
        !memcmp(index+1, "clear", end-index)  ) {
/*- - - - - - - - - - - - - - - - - - - - - -  Идентификация объекта */
             head=iFind_var(beg, 0) ;                               /* Ищем переменную */
         if(mError_code)  return(NULL) ;

         if(head==NULL) {  mError_code=_DCLE_UNKNOWN ;              /* Если такой переменной нет...*/
                                return(NULL) ;          }
         
         if(t_base(head->type)!=_XTP_OBJ) {                         /* Если это не объект комплексного типа */
                                   mError_code=_DCLE_BAD_LEFT ;
                                             return(NULL) ;                    
                                          }
/*- - - - - - - - - - - - - - - - - - - - Собственно очистка объекта */
              iXobject_clear(head) ;

         if(mError_code)  return(NULL) ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                                                }
/*----------------------------------------------------- Метод DELETE */

     else
     if(        end-index==strlen("delete")  &&
        !memcmp(index+1, "delete", end-index)  ) {
/*- - - - - - - - - - - - - - - - - - - - - -  Идентификация объекта */
             head=iFind_var(beg, 0) ;                               /* Ищем переменную */
         if(mError_code)  return(NULL) ;

         if(head==NULL) {  mError_code=_DCLE_UNKNOWN ;              /* Если такой переменной нет...*/
                                return(NULL) ;          }
         
         if(t_base(head->type)!=_XTP_OBJ) {                         /* Если это не объект комплексного типа */
                                   mError_code=_DCLE_BAD_LEFT ;
                                             return(NULL) ;                    
                                          }
/*- - - - - - - - - - - - - - - - - - -  Собственно очистка ресурсов */
                        head=NULL ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                                                 }
/*-------------------------------------------------------- Метод ADD */

     else
     if(        end-index==strlen("add")  &&
        !memcmp(index+1, "add", end-index)  ) {
/*- - - - - - - - - - - - - - - - - - - - - -  Идентификация объекта */
             head=iFind_var(beg, 0) ;                               /* Ищем переменную */
         if(mError_code)  return(NULL) ;

         if(head==NULL) {  mError_code=_DCLE_UNKNOWN ;              /* Если такой переменной нет...*/
                                return(NULL) ;          }
         
         if(t_base(head->type)!=_XTP_OBJ) {                         /* Если это не объект комплексного типа */
                                   mError_code=_DCLE_BAD_LEFT ;
                                             return(NULL) ;                    
                                          }
/*- - - - - - - - - - - - - - - - - - - Собственно добавление записи */
              iXobject_add(head, NULL) ;
         if(mError_code)  return(NULL) ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                                              }
/*------------------------------------------------------ Метод COUNT */

     else
     if(        end-index==strlen("count")  &&
        !memcmp(index+1, "count", end-index)  ) {
/*- - - - - - - - - - - - - - - - - - - - - -  Идентификация объекта */
             head=iFind_var(beg, 0) ;                               /* Ищем переменную */
         if(mError_code)  return(NULL) ;

         if(head==NULL) {  mError_code=_DCLE_UNKNOWN ;              /* Если такой переменной нет...*/
                                return(NULL) ;          }
         
         if(t_base(head->type)!=_XTP_OBJ) {                         /* Если это не объект комплексного типа */
                                   mError_code=_DCLE_BAD_LEFT ;
                                             return(NULL) ;                    
                                          }
/*- - - - - - - - - - - - - - - - - - - - -  Выдача счетчика записей */
                       value=head->buff ;

                  head=iNext_title(_WORK_VAR) ;                     /* Получаем адрес заголовка свободной рабочей переменной */
          if(mError_code)  return(NULL) ;                           /* Обраб.отсутствия памяти */

                        head->type=_DGT_VAL ;                       /* Задаем тип и размер результата */
                        head->addr=&value ;
                        head->size=  1 ;
                        head->buff=  1 ;

                     iAlloc_var(_WORK_VAR, head, 1) ;               /* Выделяем память под константу */
          if(mError_code)  return(NULL) ;                           /* Обработка отсутствия памяти */

                 memcpy(operand, head, sizeof(*head)) ;             /* Выдаем данные на выход */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                                                }
/*----------------------------------------------- Адресация элемента */
     else                                        {
/*- - - - - - - - - - - - - - - - - - - - Получение ссылки на запись */
                head=iComplex_var(beg, index-1, operand) ;          /* Извлекаем указатель на запись */
            if(mError_code)  return(NULL) ;

            if((operand->type & _DCLT_XTP_REC)==0) {                /* Если неявный индекс на текущую рабочую запись */
                                         i=head->size ;
                   iXobject_index(head, &i, operand) ;
                                                   }

              x_record=(Dcl_complex_record *)operand->addr ;

/*- - - - - - - - - - - - - - - - - - - -  Обработка FIELD$-операций */
      if(                end-index>=6  &&                           /* Если операция над полем (field$-операция) */
         !memcmp(index+1, "field$", 6)   ) {

              var=iXobject_field(x_record, index+1, end-index) ;
            if(mError_code)  return(NULL) ;
                                           }
/*- - - - - - - - - - - - - - - - - - - - -  Адресация поля по имени */
      else                                 {                        /* Если ссылка на поле */

                   var=x_record->elems ;
                   cnt=x_record->elems_cnt ;

        for(i=0 ; i<cnt ; var++, i++)                               /* Ищем поле записи по имени */
          if(!memcmp(var->name, index+1, strlen(var->name)) &&
              strlen(var->name)==end-index                    )  break ; 

          if(i>=cnt) {  mError_code=_DCLE_TYPEDEF_ELEM ;            /* Если такого поля нет... */
                                    return(NULL) ;       }
                                           }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                     memcpy(operand, var, sizeof(*var)) ;
                                                 }
/*-------------------------------------------------------------------*/
                                            }
/*---------------------------- Элементы и фрагменты строк и массивов */
/*-------------------------------------------------------------------*/

  else if(*end==']') {                                              /* IF.3 - Если признак индекса ... */

	do {                                                        /* BLOCK.1 */

/*---------------------------------------- Начальное выделение имени */

      for(index=end-1, i=1 ; index>=beg ; index--)                  /* Ищем вход индекса */
             if(*index=='[') {     i-- ;
                               if(!i) break ;  }
        else if(*index==']')       i++ ;

        if(*index!='[') {  mError_code=_DCLE_NOINDEX ;  break ; }   /* Пустой или незакрытый индекс - ошибка */

	     *index= 0 ;                                            /* Терминируем имя */

           head=iComplex_var(beg, index-1, operand) ;               /* Разбираем имя массива */
//         head=iFind_var(beg, 0) ;                                 /* Ищем имя в списке */
	if(mError_code)      break ;
	if(head==NULL) {  mError_code=_DCLE_UNKNOWN ;  break ;  }   /* При отсутствии переменной в списках - */
								    /*    уст.ошибку и прекращаем обраб.     */
                            type=t_base(head->type) ;               /* Фиксируем тип переменной */
                            mode=t_mode(head->type) ;

        if(type==_DGT_VAL) {  mError_code=_DCLE_INDEXED_VAR ;       /* Если это простая переменная -     */
					break ;               }     /*    уст.ошибку и прекращаем обраб. */
/*------------------------------------------- Определяем тип индекса */

	     *end=0 ;                                               /* Терминируем индексное выражение */

	for(f_sep=index+1 ; ; f_sep++) {                            /* Проверяем индексное выражение */
			    f_sep=iFind_close(f_sep, '.') ;         /*  на наличие 'фрагментарного'  */
			 if(f_sep==NULL)  break ;                   /*   разделителя - '...'         */

			 if(f_sep[1]=='.'  &&
			    f_sep[2]=='.'    )  break ;
				       }
/*---------------------------------------------- Если простой индекс */

	if(f_sep==NULL) {                                           /* IF.4 - Если нет разделителя ... */
			     iSimple_string(index+1) ;              /* Обрабатываем индексное выражение */
			 if(mError_code)  break ;                   /* Если есть ошибки ... */

			  O_WORK= 0 ;                               /* Сброс признака рабочей ячейки */
			     dbl=(double *)nResult.addr ;           /* Извлекаем адрес возможного индекса */
			  r_type=t_base(R_TYPE) ;                   /* Извлекаем базовый тип индекса */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - Индекс - число */
	   if(r_type==_DGT_VAL) {                                   /* IF.5 - Если числовой индекс */

                value=iDgt_get(dbl, R_TYPE) ;                       /* 'Извлекаем' индекс */
             if(value>0)  i=(int)(value+_Q_SHIFT) ;
             else         i=(int)(value-_Q_SHIFT) ;
           
             if(type==_XTP_OBJ)  iXobject_index(head, &i, operand); /* Если объект комплексного типа */
             else
             if(type==_DGT_AREA ||                                  /* В зависимости от типа массива */
                type==_DGT_PTR    ) {                               /*  или указателя устанавливаем  */
			   O_TYPE=_DGT_VAL | mode ;                 /*   тип, адрес и длину операнда */
			   O_ADDR=(char *)head->addr+i*iDgt_size(mode) ;
			   O_SIZE= 1 ;
			   O_BUFF= 1 ;
                                    }
             else                   {
			   O_TYPE=_CHR_AREA ;
			   O_ADDR=(char *)head->addr+i ;
			   O_SIZE= 1 ;
			   O_BUFF= 1 ;
                                    }

             if(i<     0    ||                                      /* Обрабатываем уход за границы */
                i>=head->buff ) {  O_ADDR=NULL ;  break ;  }        /*     массива или строки       */

				}                                   /* END.5 */
/*- - - - - - - - - - - - - - - - - - - - - - - - -  Индекс - строка */
	 else if(r_type==_CHR_PTR ||                                /* IF.6 - Если индекс - строка */
		 r_type==_CHR_AREA  ) {

	       if(type!=_CHR_AREA &&                                /* Если строка применяется для   */
		  type!=_CHR_PTR    ) {                             /*   индексирования не строчного */
                                mError_code=_DCLE_INDEX_TYPE;       /*   объекта - ошибка            */
						break ;         
                                      }

		O_TYPE=_CHR_PTR ;                                   /* Уст.тип операнда */
		O_ADDR=iFind_index(head, &nResult) ;                /* Опред.адрес операнда */
             if(O_ADDR!=NULL) {
                      up_len=(char *)O_ADDR-(char *)head->addr ;    /* Опр.длину 'перед началом' */
                      O_SIZE=head->size-up_len ;                    /* Опред.размер операнда */
                      O_BUFF=head->buff-up_len ;                    /* Опред.размер разпологаемого буфера */
                              }
             else             {
                                   O_SIZE=0 ;
                                   O_BUFF=0 ;
                              }
				      }                             /* END.6 */
/*- - - - - - - - - - - - - - - - - - - - - - -  Некорректный индекс */
	 else                         {                             /* Если тип индексного выражения */
				   mError_code=_DCLE_INDEX_TYPE ;   /*   некорректен - уст. ошибку   */
						  break ;
				      }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
		       }                                            /* ELSE.4 */
/*--------------------------------------------- Если индекс-диапазон */
	else           {                                            /* ELSE.4 - При обнар.разделителя ... */
				     fragm_flag=1 ;                 /* Уст.флаг фрагментарного индекса */

	  if(type!=_CHR_AREA &&                                     /* Если 'фрагментарное' индексирование */
	     type!=_CHR_PTR    ) {  mError_code=_DCLE_INDEX_FRG ;   /*   для нестрочного объекта - ошибка  */
					    break ;               }

			      var=head ;

			   O_WORK= 0 ;                              /* Сброс признака рабочей ячейки */
			   O_TYPE=_CHR_PTR ;                        /* Уст.тип операнда */
/*- - - - - - - - - - - - - - - - - - - -  Обработка первого индекса */
	      if(f_sep[-1]=='-') { inc1_flag=0 ; f_sep[-1]=0 ;  }   /* Обраб.спецификаторы        */
	 else if(f_sep[-1]=='+') { inc1_flag=1 ; f_sep[-1]=0 ;  }   /*  включения индексных строк */
	 else                      inc1_flag=1 ;                    /*   в выделяемый фрагмент    */

				 f_sep[0]=0 ;                       /* Терминируем первый индекс */

	 if(index[1]!=0) {                                          /* IF.7 - Если первый индекс задан ... */
			     iSimple_string(index+1) ;              /* Обраб. первое индексное выражение */
			 if(mError_code)  break ;                   /* Если есть ошибки ... */

			   O_TYPE=_CHR_PTR ;                        /* Уст.тип операнда */

			      dbl=(double *)nResult.addr ;          /* Извлекаем адрес возможного индекса */
			   r_type=t_base(R_TYPE) ;                  /* Извлекаем базовый тип индекса */

	      if(r_type==_DGT_VAL   ) {                             /* Если числовой индекс ... */
		  i = (int)(iDgt_get(dbl, R_TYPE)+_Q_SHIFT) ;       /*   'Извлекаем' индекс */
	       if(i<     0    ||                                    /*   Обраб. уход за границы */
		  i>=head->buff ) {  O_ADDR=NULL ;  break ;  }      /*     буфера обьекта       */
	  else if(i>=head->size ) {   fragm_flag=0 ;                /*   Обраб.уход за пределы обьекта */
				      head->size=i ;  }

		 start.addr=(char *)head->addr+i ;                  /*   Уст.начальный адрес фрагмента */
		      begin=(char *)start.addr ;
		 start.size=head->size-i ;                          /*   Уст.длину фрагмента */
		 start.buff=head->buff-i ;                          /*   Уст.длину фрагмента */
				      }
	 else if(r_type==_CHR_PTR ||
		 r_type==_CHR_AREA  ) {                             /* Если индекс - строка ... */
		   begin=iFind_index(head, &nResult) ;              /*   Опред.начальный адрес фрагмента */
		if(begin==NULL) {  O_ADDR=NULL ;  break ;  }        /*   Если индекс не найден... */
		    start.addr=begin+nResult.size ;                 /*   Опред.адрес остатка */
                        up_len=nResult.size+begin-(char *)head->addr ;
		    start.size=head->size-up_len ;                  /*   Опред.длину остатка */
		    start.buff=head->buff-up_len ;                  /*   Опред.размер оставшегося места */
				      }
	 else                         {                             /* Если тип индексного выражения */
				     mError_code=_DCLE_INDEX_TYPE ; /*   некорректен - уст. ошибку   */
						  break ;
				      }
			 }                                          /* ELSE.7 */
	 else            {                                          /* ELSE.7 - Если первый индекс опущен */
			      start.addr=head->addr ;               /* Уст.начальный адрес фрагмента */
				   begin=(char *)start.addr ;
			      start.size=head->size ;               /* Уст.длину фрагмента */
			      start.buff=head->buff ;               /* Уст.длину распологаемого буфера */
			 }                                          /* END.7 */
/*- - - - - - - - - - - - - - - - - - - - Установка начала фрагмента */
	  if(inc1_flag) {  O_ADDR=begin ;                           /* Уст.начальный адрес фрагмента */
			   O_SIZE=start.size                        /*  и его предварительную длину  */
				 +(char *)start.addr-begin ;        /*   в зависимости от входного   */
			   O_BUFF=start.buff                        /*    спецификатора включения    */
				 +(char *)start.addr-begin ; }
	  else          {  O_ADDR=start.addr ;
			   O_SIZE=start.size ;
			   O_BUFF=start.buff ;                   }
/*- - - - - - - - - - - - - - - - - - - -  Обработка второго индекса */
	      if(*(f_sep+3)=='-') {  inc2_flag=0 ;  f_sep++ ;  }    /* Обраб.спецификаторы        */
	 else if(*(f_sep+3)=='+') {  inc2_flag=1 ;  f_sep++ ;  }    /*  включения индексных строк */
	 else                        inc2_flag=1 ;                  /*   в выделяемый фрагмент    */

	 if(f_sep[3]!=0) {                                          /* IF.8 - Если второй индекс задан ... */
			     iSimple_string(f_sep+3) ;              /* Обраб. второе индексное выражение */
			 if(mError_code)  break ;                   /* Если есть ошибки ... */

			  O_TYPE=_CHR_PTR ;                         /* Уст.тип операнда */

			     dbl=(double *)nResult.addr ;           /* Извлекаем адрес возможного индекса */
			  r_type=t_base(R_TYPE) ;                   /* Извлекаем базовый тип индекса */

	      if(r_type==_DGT_VAL   ) {                             /* Если числовой индекс ... */
		  i = (int)(iDgt_get(dbl, R_TYPE)+_Q_SHIFT) ;       /*   'Извлекаем' индекс */
	       if(i< head->size-start.size ||                       /*   Обраб. уход за границы */
		  i>=head->buff              ) {  O_ADDR=NULL ;     /*     буфера обьекта       */
						       break ;  }
	  else if(i>=head->size              ) {  i=head->size-1 ;  /*   Обраб.уход за пределы обьекта */
                                                 fragm_flag=0 ;   } 

		    O_SIZE=(char *)head->addr+i-(char *)O_ADDR+1 ;  /*   Уст.длину фрагмента */
				      }
	 else if(r_type==_CHR_PTR ||
		 r_type==_CHR_AREA  ) {                             /* Если индекс - строка ... */
		      addr=iFind_index(&start, &nResult) ;          /*   Опред.конечный адрес фрагмента */
		   if(addr==NULL) {  O_ADDR=NULL ;  break ;  }      /*   Если индекс не найден... */
              if(inc1_flag)  O_SIZE =addr-(char *)O_ADDR ;          /*   Опред.длину фрагмента */
              else           O_SIZE =addr-(char *)start.addr ;
	      if(inc2_flag)  O_SIZE+=nResult.size ;                 /*   Коррекция длины фрагмента        */
				      }                             /*    при уст.спецификаторе включения */
	 else                         {                             /* Если тип индексного выражения */
				     mError_code=_DCLE_INDEX_TYPE ; /*   некорректен - уст. ошибку   */
						  break ;
				      }
			 }                                          /* END.8 */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
		       }                                            /* END.4 */
/*-------------------------------------------------------------------*/

	   } while(0) ;                                             /* BLOCK.1 */
		     }                                              /* END.3 */
/*-------------------------------------------------------- Процедура */
/*-------------------------------------------------------------------*/

  else if(*end==')') {                                              /* IF.9 - Если признак процедуры ... */
	do {                                                        /* BLOCK.2 */

/*---------------------------------------- Начальное выделение имени */

             x_object=NULL ;

                pars=(char *)memchr(beg, '(', end-beg+1) ;          /* Ищем начало параметров */
	       *pars= 0 ;                                           /* Терминируем имя */

       if(iXobject_check(beg, pars-1, &index)) {                    /* Если метод X-объекта... */
                          *index= 0 ;

/*???*/
               if( beg[strlen(beg) - 1] == ']' )    /* если признах индекса в имени X-объекта */
               {
                   char *ind_xobj;  /* индекс записи */
                   char *pars_ind;  /* указатель на строку параметров */

                   ind_xobj = (char *)memchr( beg, '[', strlen(beg) );  /* выделяем индексное выражение */
                   beg[ strlen(beg) - strlen(ind_xobj) ] = 0;
                   pars_ind = (char *)memchr( (pars + 1), ')', strlen(pars + 1) );  /* записываем индекс в конец строки параметров метода */
                   if( *(pars+1) != ')' )                               /* если есть входные параметры, ставим запятую  */
                   {
                       (*pars_ind) = ','; 
                       pars_ind++;
                   }
                   (*pars_ind) = '"'; pars_ind++;
                   memcpy( (char *)pars_ind, (char *)(ind_xobj+1), strlen(ind_xobj+1) );
                   pars_ind += strlen(ind_xobj+1)-1;
                   (*pars_ind) = '"'; pars_ind++;
                   (*pars_ind) = ')'; 
                   end = pars_ind;
               }

                            head=iXobject_method(index+1, pars-1) ; /*   Идентифицируем метод */
                        x_object=      iFind_var(beg, 0) ;          /*   Идентифицируем X-объект */
              if(       x_object       == NULL   )  mError_code=_DCLE_UNKNOWN ;
         else if(t_base(x_object->type)!=_XTP_OBJ)  mError_code=_DCLE_BAD_LEFT ;
                                               }
       else                 head=iFind_var(beg, 1) ;                /* Если обычная процедура - ищем имя в списке процедур */

	if(mError_code)  break ;
	if(head==NULL ) {  mError_code=_DCLE_UNKNOWN ;  break ;  }  /* При отсутствии переменной в списках - */
								    /*    уст.ошибку и прекращаем обраб.     */
	 type=head->type ;                                          /* Фиксируем тип переменной */

/*--------------------------------------------- Обработка параметров */

		 proto_flag=  1 ;                                   /* Уст.флаг контроля типов параметров */
		 pars_table=&nPars_table[nPars_cnt_all] ;           /* Уст.адрес таблицы параметров */
		   pars_cnt=  0 ;                                   /* Сбрасываем счетчик параметров */
		       *end=  0 ;                                   /* Терминируем список параметров */
		       pars++ ;                                     /* Уст.указатель на первый параметр */

     if(pars[0]!=0) {                                               /* IF.10 - Если есть параметры ... */

	 do {                                                       /* CIRCLE.2 - Скан.список параметров ... */
/*- - - - - - - - - - - - - - - - - - - - - - - Вычисление параметра */
		 addr=iFind_close(pars, ',') ;                      /* Ищем разделитель параметров */
	      if(addr!=NULL)  *addr=0 ;                             /* Терминируем параметр */

		    iSimple_string(pars) ;                          /* Вычисляем параметр */
		if(mError_code)  break ;                            /* Если есть ошибки ... */
/*- - - - - - - - - - - - - - - Контроль типа параметра по прототипу */
        if(head->prototype!=NULL)
                 proto_type=head->prototype[pars_cnt] ;             /* Выделяем предполагаемый тип */
        else     proto_type=  0 ;

	if(proto_type==0) proto_flag=0 ;                            /* Если строка прототипов концилась -    */
								    /*  сбрас.флаг контроля типов параметров */
	      r_type=t_base(nResult.type) ;                         /* Извлекаем базовый тип результата */

	 if(proto_flag)                                             /* Если имеется прототиp -     */
	  if((proto_type==_F_DGT_VAL &&                             /*  контролируем тип параметра */
		  r_type!=_DGT_VAL     ) ||
	     (proto_type==_F_DGT_PTR &&
	      (   r_type!=_DGT_AREA &&
		  r_type!=_DGT_PTR    )) ||
	     (proto_type==_F_CHR_PTR &&
	      (   r_type!=_CHR_AREA &&
		  r_type!=_CHR_PTR    ))   ) {
				    mError_code=_DCLE_PROTOTYPE ;
                                                  return(NULL) ;
					     }
/*- - - - - - - - - - - - - - - - - -  Занесение параметра в таблицу */
		 par=iNext_title(_WORK_VAR) ;                       /* Получаем адрес заголовка свободной */
								    /*     рабочей переменной             */
              if(mError_code)  return(NULL) ;                       /* Обраб.отсутствия памяти */

		 memcpy(par, &nResult, sizeof(nResult));            /* Создаем копию результата */

	       pars_table[pars_cnt]=par ;                           /* Заносим адрес параметра в таблицу */
			  pars_cnt++ ;                              /* Модиф.счетчик параметров */
		     nPars_cnt_all++ ;                              /* Модиф.суммарный счетчик параметров */

			      pars=addr+1 ;                         /* Уст указатель на след.параметр */

		if(addr==NULL)  break ;                             /* Если параметры кончились ... */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
	    } while(1) ;                                            /* CONTINUE.2 */

		    }                                               /* END.10 */
/*-------------------------------------------------- Вызов процедуры */
/*- - - - - - - - - - - - - - - - - - - - - - - -  Подготовка вызова */
           nPars_cnt=pars_cnt ;                                     /* Уст.счетчик параметров */
            dcl_proc=(Dcl_decl *(*)(Lang_DCL *,                     /* Уст.адрес DCL-процедуры */
                                    Dcl_decl **, int))head->addr ;  
/*- - - - - - - - - - - - - - - - - - - - - - -  Вызов стандарта DCL */
     if(head->func_flag==_DCL_CALL  ||                            /* Вызываем процедуру или метод в станд.DCL ... */
        head->func_flag==_DCL_METHOD  ) {

            if(x_object!=  NULL   )  
                      head=iXobject_invoke(x_object, head,          /*   Вызов метода X-объекта */
                                                 pars_table, pars_cnt) ;
       else if(dcl_proc==_DCL_LOAD) 
                           head=iCall_exec(head->work_nmb) ;        /*   Вызов DCL-процедуры */
       else                head=  dcl_proc(this, pars_table,        /*   Собственно DCL-вызов */
                                                 pars_cnt   ) ;  

                 if(mError_code)  return(NULL) ;                    /*   Обраб.ошибок вызова */

		 if(head==NULL)  head=&nVoid ;                      /*   Обработка NULL-возврата */

                        var=iNext_title(_WORK_VAR) ;                /*   Получаем адрес заголовка свободной рабочей переменной */
                 if(mError_code)  return(NULL) ;                    /*   Обработка отсутствия памяти */

		      memcpy(var, head, sizeof(*var)) ;             /*   Переходим на новый заголовок */
                                  head=var ;

	                   iAlloc_var(_WORK_VAR, var, var->size) ;  /*   Выделяем память под RETURN-обьект */
                 if(mError_code)  return(NULL) ;                    /*   Обработка отсутствия памяти */
				        }
/*- - - - - - - - - - - - - - - - - - - - - - - -  Завершение вызова */
	     nPars_cnt_all-=nPars_cnt ;                             /* Корр. суммарн. счетчик параметров */

	    memcpy(operand, head, sizeof(*head)) ;                  /* Копируем возврат на выход */

/*-------------------------------------------------------------------*/

	   } while(0) ;                                             /* BLOCK.2 */
		     }                                              /* END.9 */
/*----------------------------------------- Обработка константы NULL */
/*-------------------------------------------------------------------*/

  else if((beg[0]=='N' || beg[0]=='n') &&
	  (beg[1]=='U' || beg[1]=='u') &&
	  (beg[2]=='L' || beg[2]=='l') &&
	  (beg[3]=='L' || beg[3]=='l')   ) {
					      O_TYPE=_NULL_PTR ;
					      O_ADDR= NULL ;
					      O_WORK=  0  ;
					   }
/*-------------- Обработка простой переменной или числовой константы */
/*-------------------------------------------------------------------*/

  else               {                                              /* IF.2 - Если простая перем. ... */
/*------------------------------ Подготовка анализируемого фрагмента */

			  name_len=end-beg+1 ;
			  chr_save=end[1] ;                         /* Терминируем имя переменной */
			    end[1]= 0 ;

/*------------------------------------- Обработка числовой константы */

			 value=iStrToNum(beg, &stop) ;              /* Пытаемся дешифровать число */

   if(stop==end+1) {                                                /* IF.11 - Если фрагмент - число ... */
			   head=iNext_title(_WORK_VAR) ;            /* Получаем адрес заголовка свободной */
								    /*     рабочей переменной             */
                        if(mError_code)  return(NULL) ;             /* Обраб.отсутствия памяти */

			   head->type=_DGT_VAL ;                    /* Задаем тип и размер */
			   head->addr=&value ;                      /*    константы        */
			   head->size=  1 ;
			   head->buff=  1 ;

			      iAlloc_var(_WORK_VAR, head, 1) ;      /* Выделяем память под константу */
                          if(mError_code)  return(NULL) ;           /* Обработка отсутствия памяти */

		      memcpy(operand, head, sizeof(*head)) ;        /* Выдаем данные на выход */
		   }                                                /* ELSE.11 */
/*------------------------------------- Обработка простой переменной */

   else            {                                                /* ELSE.11 - Если фрагмент - перем. ... */
                      head=iFind_var(beg, 0) ;                      /* Ищем переменную в списке */
              if(mError_code)  return(NULL) ;

             if(head==NULL)  mError_code=_DCLE_UNKNOWN ;            /* При отсутствии переменной в списках    */
             else           memcpy(operand, head, sizeof(*head)) ;  /*  уст. ошибку, а при наличии - передаем */

		         var=head ;
		   }                                                /* ELSE.11 */
								    /*   параметры на выход                   */
/*----------------------------------------- Восстановление фрагмента */

			if(!mError_code)  end[1]=chr_save ;         /* Восстан.состояния фрагмента */
		     }                                              /* END.2 */
/*-------------------------------------- Контроль ненайденного имени */
/*-------------------------------------------------------------------*/

     if(mError_code==_DCLE_UNKNOWN) {                               /* Если имя не найдено по   */
								    /*  нейзвестной причине ... */
				  len=strlen(mError_position) ;
	for(i=0 ; i<len; i++)                                       /*   Контр. состав символов */
          if(!NAME_CHAR(mError_position[i])) break ;   

	       if(i==len)  mError_code=_DCLE_UNKNOWN_NAME ;         /* Уст. тип ошибки */
	       else        mError_code=_DCLE_SINTAX_NAME ;
				    }
/*----------------------------------------- Обработка NULL-указателя */
/*-------------------------------------------------------------------*/

    if(O_ADDR==NULL) {  O_SIZE=0 ;
                        O_BUFF=0 ;  }

/*------------------------------------- Установка индикатора условия */
/*-------------------------------------------------------------------*/

  if(t_base(O_TYPE)==_DGT_VAL ) {
			    dbl =(double *)O_ADDR ;
		if(         dbl         ==NULL)  nIf_ind=0 ;
	   else if(iDgt_get(dbl, O_TYPE)== 0. )  nIf_ind=0 ;
	   else                                  nIf_ind=1 ;
				}
  else                          {
                               if(O_ADDR==NULL)  nIf_ind=0 ;
                               else              nIf_ind=1 ;
				}
/*-------------------------------------------------------------------*/
/*-------------------------------------------------------------------*/

	  nFragm_flag=fragm_flag ;

  return(var) ;
}


/************************************************************************/
/*                                                                      */
/*              Поиск переменной или процедуры по имени                 */
/*                                                                      */
/*   name       -  Имя переменной или процедуры                         */
/*   func_flag  -  Флаг работы с процедурой                             */

   Dcl_decl *Lang_DCL::iFind_var(char *name, int  func_flag)

{
   Dcl_decl *target  ;    /* Описание нужной переменной и процедуры */
   Dcl_decl *vars ;       /* Текущий массив описаний переменных и процедур */
        int  f_flag ;     /* Флаг функции */
        int  i ;


/*-------------------------------------------- Инициализация */

		   target=NULL ;

/*----------------------------- Работа с транзитной таблицей */

  for(i=0 ; i<nTransit_cnt ; i++)
    if(!strcmp(nTransit[i].name, name)) {
                                     target=&nTransit[i] ;
				                break ;
                                        }
/*------------------------- Работа с внутренними переменными */

   if(!func_flag) {                                         /* Если ищется переменная... */

     for(vars=nInt_page ; vars->name[0]!=0 ; vars++)
       if(!strcmp(vars->name, name)) {  target=vars ;  
                                             break ;  }
		  }
/*---------------------------- Работа с внешними переменными */

   for(i=0 ; target==NULL ; i++) {

	if(nVars[i]==NULL) break ;

    for(vars=nVars[i] ; vars->name[0]!=0 ; vars++) {

     if(vars->local_idx!=   0   &&                          /* Анализ зоны видимости */
        vars->local_idx!=nLocal   )  continue ; 

		    f_flag=vars->func_flag ;                /* Уст.флаг функции в 1        */
	if(f_flag)  f_flag=1 ;                              /*  независимо от типа функции */

     if( f_flag==func_flag &&         
	!strcmp(vars->name, name)) {  target=vars ;  break ;  }
						   }
				 }
/*----------------------------- Работа с внешней библиотекой */

#ifdef  _EXT_LIBS

   if(target     ==NULL &&                                  /* Если обьект не найден и задана             */
      dcl_ext_lib!=NULL   ) {                               /*  процедура связи с внешней библиотекой ... */

		    if(func_flag) obj_type=_LOAD_FNC ;      /*  Уст.тип обьекта */
		    else          obj_type=_LOAD_VAR ;

	      target=dcl_ext_lib(obj_type, name) ;          /*  Ищем обьект во внешней библиотеке */
	 if(dcl_ext_error) {  mError_code=dcl_ext_error ;
				    return(target) ;      }
			    }

#endif

/*--------------------------- Обработка вызова DCL-процедуры */

   if(target==NULL && func_flag) {                          /* Если обьект не найден */
							    /*   и это функция ...   */
		        target=iCall_find(name) ;           /*  Ищем среди DCL-процедур */
                                 }
/*------------------------- Обработка динамической подгрузки */

#ifdef  _EXT_LIBS

   if(target      !=    NULL  &&                            /* Если обьект существует,      */
      target->addr==_EXT_LOAD &&                            /*  описан как подгружаемый     */
      dcl_ext_load!=    NULL    ) {                         /*   задана процедура подгрузки */

       if(func_flag) obj_type=_LOAD_FNC ;                   /*  Уст.тип обьекта */
       else          obj_type=_LOAD_VAR ;

	status=dcl_ext_load(obj_type, target->name) ;       /*  Вызов процедуры подгрузки */

	      target->addr=dcl_ext_addr ;                   /*  Прием адреса подгруженного обьекта */

      if(dcl_ext_cnt==_LOAD_MAX) {                          /*  Контроль счетчика подгрузки */
			      mError_code=_DCLE_LOAD_LIST ;
				      return(target) ;
				 }

       dcl_ext_list[dcl_ext_cnt]=target ;                   /*  Занесение обьекта в список */
		    dcl_ext_cnt++ ;                         /*    подгруженных             */
				   }

#endif

/*-----------------------------------------------------------*/

  return(target) ;
}


/************************************************************************/
/*                                                                      */
/*         Поиск индексной строки в базовой строке                      */
/*                                                                      */
/*     base   -  Базовая строка                                         */
/*     index  -  Индексная строка                                       */
/*                                                                      */
/*    При локализации индекса возвращаем адрес места, а в противном     */
/*     случае - NULL.                                                   */

     char *Lang_DCL::iFind_index(Dcl_decl *base, Dcl_decl *index)

{
   int  find_flag ;
  char *b_str ;
  char *i_str ;
   int  len ;
   int  i ;
   int  j ;


    if(index->addr==   NULL ||                                      /* Если инд.строка неопределенная или */
       index->size> base->size )  return(NULL) ;                    /*  инд.строка длиннее базовой -      */
								    /*   - результат неопределен          */
    if(index->size==0)  return((char *)base->addr) ;                /* Если инд.строка - пустая, то     */
								    /*   результат - вся базовая строка */
	     find_flag=  0 ;                                        /* Сбрас.флаг обнаружения */
		 b_str=(char *)base->addr ;                         /* Уст.адреса базовой и */
		 i_str=(char *)index->addr ;                        /*   индексной строк    */
		   len=base->size-index->size+1 ;                   /* Уст длину поиска входа индекса */

   for(i=0 ; i<len ; b_str++, i=b_str-(char *)base->addr) {         /* CIRCLE.1 - Сканируем строку */

       b_str=(char *)memchr(b_str, i_str[0], len-i) ;               /* Ищем вход индексной строки */
    if(b_str==NULL)  break ;                                        /* При его отсутствии - конец поиска */

     for(j=0 ; j<index->size ; j++) if(b_str[j]!=i_str[j]) break ;  /* Проверяем 'истинность' входа */
								    /*      индексной строки        */
	if(j==index->size) {  find_flag=1 ;                         /* Если вход 'истинный' - уст. */
				 break ;   }                        /*  флаг обнар. и прекр.поиск  */
                                                          }         /* CONTINUE.1 */

           len=b_str-(char *)base->addr+index->size ;               /* Если инд.строка завершается за    */
	if(len>base->size)  find_flag=0 ;                           /*   пределами базовой строки - ...  */

       if(find_flag)  return(b_str) ;
       else           return(NULL) ;
}


/************************************************************************/
/*                                                                      */
/*                            Процессор                                 */
/*                                                                      */
/*    oper_code  -  Код операции                                        */
/*    operand1   -  Описание 1-го операнда                              */
/*    operand2   -  Описание 2-го операнда                              */
/*                                                                      */
/*   Допустимые операции :                                              */
/*                                                                      */
/*      Для всех типов операндов :                                      */
/*                            @  слияние в символьном режиме            */
/*                                                                      */
/*      Строка - строка (CHR_AREA | CHR_PTR | NULL_PTR):                */
/*                            -  разность адресов                       */
/*                                все операции сравнения                */
/*                            !  инверсия                               */
/*                                                                      */
/*      Строка - число  (CHR_AREA | CHR_PTR | NULL_PTR + DGT_VAL):      */
/*                            +  смещение указателя на строку вправо    */
/*                            -  смещение указателя на строку влево     */
/*                                 (только строка - число)              */
/*                            *  дублирование строки                    */
/*              Примечание: при работе с NULL результат всегда - NULL   */
/*                                                                      */
/*      Массив - массив (DGT_AREA | DGT_PTR | NULL_PTR):                */
/*                            -  разность адресов                       */
/*                                                                      */
/*      Массив - число  (DGT_AREA | DGT_PTR | NULL_PTR + DGT_VAL):      */
/*                            +  смещение указателя на строку вправо    */
/*                            -  смещение указателя на строку влево     */
/*                                 (только строка - число)              */
/*              Примечание: при работе с NULL результат всегда - NULL   */
/*                                                                      */
/*      Число - число  (DGT_VAL):                                       */
/*                                4 арифметических действия: +,-,*,/    */
/*                            ^  возведение в степень                   */
/*                           ++  инкрементация                          */
/*                           --  декрементация                          */
/*                                все операции сравнения                */
/*                            -  унарный минус                          */
/*                            !  инверсия                               */
/*                           &&  логическое И                           */
/*                           ||  логическое ИЛИ                         */
/*                            |  арифметическое ИЛИ                     */
/*                                                                      */

   int  Lang_DCL::iProcessor(int  oper_code, Dcl_decl *operand1, 
                                             Dcl_decl *operand2 )

{
    int      cmpl_type ;  /* Совокупный тип операндов */
    int      oper_type ;  /* Тип операции */
#define    _CC_TYPE     _CHR_AREA              /* Строка - строка             */
#define    _CD_TYPE    (_CHR_AREA|_DGT_VAL)    /* Строка - число              */
#define    _AA_TYPE     _DGT_AREA              /* Массив чисел - массив чисел */
#define    _AD_TYPE    (_DGT_AREA|_DGT_VAL)    /* Массив чисел - число        */
#define    _DD_TYPE     _DGT_VAL               /* Число - число               */

    int      status ;     /*  */
 double      value ;      /* Числовое значение */
    int      cnt ;        /* Счетчик */
//  int      size ;
    int      size1 ;      /* Размер 1-го операнда */
    int      size2 ;      /* Размер 2-го операнда */
    int      buff1 ;      /* Размер буфера 1-го операнда */
    int      buff2 ;      /* Размер буфера 2-го операнда */
    int      if_mask[6]={1, 3, 2, 5, 6, 4} ;  /*  Массив IF-масок: 0 бит -  < */
					      /*                   1 бит - == */
					      /*                   2 бит -  > */

        Dcl_decl *result ;   /* Адрес описания результата */
	  double  dgt1 ;     /* Числовые значения операндов */
	  double  dgt2 ;
   unsigned long  int1 ;
   unsigned long  int2 ;
   unsigned char *addr1 ;    /* Адреса данных операндов */
   unsigned char *addr2 ;
	     int  type1 ;    /* Базовые типы операндов */
	     int  type2 ;
	     int  mode1 ;    /* Уточняющие типы операндов */
	     int  mode2 ;

   unsigned char *addr ;
        Dcl_decl *tmp_v ;
    unsigned int  tmp_i ;
	     int  i ;

/*--------------------------------------- Обработка унарных операций */

      if(oper_code==_INV_OPER ||                                    /* При унарных операциях          */
	 oper_code==_INC_OPER ||                                    /*   второй операнд игнорирруется */
	 oper_code==_DEC_OPER ||
	 oper_code==_NEG_OPER   )  operand2=operand1 ;

/*---------------------------------------------------- Инициализация */

                addr1=(unsigned char *)O1_ADDR ;                    /* Извлекаем адреса данных */
                addr2=(unsigned char *)O2_ADDR ;
                type1= t_base(O1_TYPE) ;                            /* Извлекаем базовые типы данных */
                type2= t_base(O2_TYPE) ;
                mode1= t_mode(O1_TYPE) ;                            /* Извлекаем уточняющие типы данных */
                mode2= t_mode(O2_TYPE) ;

/*---------------------------------------- Определение типа операции */

	   cmpl_type=type1 | type2 ;                                /* Опред.совокупный тип операндов */
	   oper_type=cmpl_type & 0x000e ;                           /* Опред.тип операции */

	 if((cmpl_type & _NULL_PTR) &&                              /* Обр.особого случая:      */
	     oper_type ==_DD_TYPE      )  oper_type=0 ;             /*   число - NULL-указатель */

/*------------------------------------ Определение ячейки результата */

	 if(operand1->work_nmb) {  nResult_dst=  1 ;                /* Пытаемся разместить результат */
				        result=operand1 ;  }        /*   в рабочую ячейку, а при ее  */
    else if(operand2->work_nmb) {  nResult_dst=  2 ;                /*    отсутствии в системную     */
				        result=operand2 ;  }        /*      ячейку результата        */
    else                        {  nResult_dst=  0 ;
				        result=&nResult ;  }

/*-------------------------------------  Слияние в символьном режиме */

	  if(oper_code==_LNK_OPER) {
				     buff1=O1_BUFF ;                /* Извлекаем размеры буферов строк */
				     buff2=O2_BUFF ;
	    if(addr1!=NULL)          size1=O1_SIZE ;                /* Жестко устанавливаем   */
	    else                     size1= 0 ;                     /*   длину NULL-операндов */
	    if(addr2!=NULL)          size2=O2_SIZE ;
	    else                     size2= 0 ;

	  if(type1==_DGT_VAL  ||                                    /* Приводим размер численных          */
	     type1==_DGT_AREA ||                                    /*  операндов к строчным размерностям */
	     type1==_DGT_PTR    ) {  size1*=iDgt_size(mode1) ;
				     buff1*=iDgt_size(mode1) ;  }
	  if(type2==_DGT_VAL  ||
	     type2==_DGT_AREA ||
	     type2==_DGT_PTR    ) {  size2*=iDgt_size(mode2) ;
				     buff2*=iDgt_size(mode2) ;  }

				    P_TYPE=_CHR_AREA ;              /* Уст.тип результата */

	     if(addr1==NULL &&
		addr2==NULL   ) {                                   /* Если две NULL-строки ... */
					   P_ADDR=NULL ;            /* Задаем результат - NULL-строку */
				}
	     else               {                                   /* Если есть нормальная строка ... */
		       P_SIZE=size1+size2 ;                         /*   Опред.размер результата */
		       P_BUFF=P_SIZE ;                              /*   Опред.размер буфера результата */
			 addr=(unsigned char *)                     /*   Выделяем память */
                               iAlloc_var(_WORK_VAR, P_RESULT, 0) ;
		      if(mError_code)  return(0) ;                  /*   Обработка отсутствия памяти */

		   memcpy(        P_ADDR,       addr1, size1) ;     /*   Формируем результат */
		   memcpy((char *)P_ADDR+size1, addr2, size2) ;
				}
				   }
/*------------------------------------------- Операции строка-строка */

   else if(oper_type==_CC_TYPE) {

			       buff1=O1_BUFF ;                      /* Извлекаем размеры буферов строк */
			       buff2=O2_BUFF ;
	    if(addr1!=NULL)    size1=O1_SIZE ;                      /* Жестко устанавливаем   */
	    else               size1= 0 ;                           /*   длину NULL-операндов */
	    if(addr2!=NULL)    size2=O2_SIZE ;
	    else               size2= 0 ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - Разность адресов */
	  if(oper_code==_SUB_OPER) {
				       value=addr1-addr2 ;          /* Вычислеям разность адресов */
				      P_TYPE=_DGT_VAL ;             /* Уст.тип результата */
				      P_ADDR=&value ;               /* Уст.адрес 'источника' */
				      P_SIZE=  1 ;                  /* Уст.размер результата */
				      P_BUFF=  1 ;                  /* Уст.размер буфера результата */

			 addr=(unsigned char *)                     /* Выделяем память под результат */
                               iAlloc_var(_WORK_VAR, P_RESULT, 1) ;
		      if(mError_code)  return(0) ;                  /* Обработка отсутствия памяти */
				   }
/*- - - - - - - - - - - - - - - - - - - - - - - - -  Сравнение строк */
     else if(oper_code>=_LT_OPER &&
	     oper_code<=_GT_OPER   ) {

	   if(size1>size2)   status= 4 ;                            /* Определяем 'соотношение' строк   */
      else if(size1<size2)   status= 1 ;                            /*  исходя из : 1) их длины         */
      else                 {                                        /*              2) состава          */
	      if(size1)    {                                        /*              3) адресов          */
	                       status=memcmp(addr1, addr2, size1) ;
	                    if(status>0)  status=4 ;
                       else if(status<0)  status=1 ;
                       else               status=2 ; 
                           }                                        /*   В последнем случае различаются */
	 else if(addr1!=NULL &&                                     /*    только нулевой(NULL) и        */
		 addr2==NULL   )  status= 4 ;                       /*     нормальный адреса            */
	 else if(addr1==NULL &&
		 addr2!=NULL   )  status= 1 ;
	 else                     status= 2 ;
			   }

	    nIf_ind=status & if_mask[oper_code-_LT_OPER] ;          /* Опред.результат сравнения */
	   nDgt_val=nIf_ind ;

			 P_TYPE=_DGT_VAL ;                          /* Уст.тип, размер и      */
			 P_ADDR=&nDgt_val ;                         /*  'источник' результата */
			 P_SIZE= 1 ;
			 P_BUFF= 1 ;

		addr=(unsigned char *)                              /* Размещаем результат */
                      iAlloc_var(_WORK_VAR, P_RESULT, 1) ;
	     if(mError_code)  return(0) ;                           /* Обработка отсутствия памяти */
				     }
/*- - - - - - - - - - - - - - - - - - - - - - - - Инверсия указателя */
     else if(oper_code==_INV_OPER) {
				      P_TYPE=_CHR_PTR ;             /* Уст.тип результата */
                     if(addr1==NULL)  P_ADDR=(void *)2L ;           /* Уст. 'инверсный' адрес */
		     else             P_ADDR= NULL ;
				      P_SIZE=  0  ;                 /* Уст.размер результата */
				      P_BUFF=  0  ;                 /* Уст.размер буфера результата */
				   }
/*- - - - - - - - - - - - - - - - - - - - - - - Запрещенная операция */
     else              mError_code=_DCLE_BAD_OPERATION ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
				}
/*-------------------------------------------- Операции строка-число */

   else if(oper_type==_CD_TYPE) {
		if(type1==_DGT_VAL) {    tmp_v =operand1 ;          /* Приводим порядок операндов */
				       operand1=operand2 ;          /*    к порадку строка-число  */
				       operand2= tmp_v  ;
					 tmp_i =mode2 ;
					  mode2=mode1 ;
					  mode1= tmp_i ;    }

				 size1=O1_SIZE ;
				 buff1=O1_BUFF ;
				  dgt2=iDgt_get(addr2, mode2) ;     /* Извлекаем значение 2-го операнда */
/*- - - - - - - - - - - - - - - - - - - - - - Сдвиг указателя строки */
	  if(oper_code==_ADD_OPER ||
	     oper_code==_SUB_OPER   ) {
					 cnt=(int)(dgt2+_Q_SHIFT) ; /* Извлекаем число */
	       if(oper_code==_SUB_OPER)  cnt=-cnt ;                 /* Приводим вычитание к сложению */

				  P_TYPE=_CHR_PTR ;                 /* Уст.тип результата */

	     if(addr1==NULL)      P_ADDR=NULL ;                      /* Если NULL-строка ... */
	     else            {                                       /* Если нормальная строка ... */
				  P_ADDR=addr1+cnt ;                 /*   Опред.результирующий указатель */
		  if(size1>=cnt)  P_SIZE=size1-cnt ;                 /*   Опред.результирующий размер */
		  else            P_SIZE= 0 ;
		  if(buff1>=cnt)  P_BUFF=buff1-cnt ;                 /*   Опред. размер буфера */
		  else            P_BUFF= 0 ;

			     }
				      }
/*- - - - - - - - - - - - - - - - - - - - - - -  Дублирование строки */
     else if(oper_code==_MUL_OPER) {

				      cnt=(int)(dgt2+_Q_SHIFT) ;    /* Уст счетчик размножения */
				   P_TYPE=_CHR_AREA ;               /* Уст.тип результата */

	      if( cnt <= 0 ||                                       /* Обраб.ситуацию когда счетчик <=0 */
		 addr1==NULL )   P_ADDR=NULL ;                      /*  или дублируемая строка - NULL   */
	      else             {                                    /* Если операнды корректны ... */
		       P_SIZE=size1*cnt ;                           /*   Опред.размер результата */
		       P_BUFF=size1*cnt ;
		       P_ADDR=addr1 ;                               /*   Уст.источник первого 'повтора' */
			 addr=(unsigned char *)                     /*   Выделяем память */
                                iAlloc_var(_WORK_VAR, P_RESULT, 0) ;
		      if(mError_code)  return(0) ;                  /*   Обраб.отсутствия памяти */

		  for(i=0 ; i<cnt ; i++)                            /*   Формируем результат */
			  memcpy(addr+size1*i, addr1, size1) ;
			       }
				   }
/*- - - - - - - - - - - - - - - - - - - - - - - Запрещенная операция */
     else              mError_code=_DCLE_BAD_OPERATION ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
				}
/*------------------------------------------- Операции массив-массив */

   else if(oper_type==_AA_TYPE) {

/*- - - - - - - - - - - - - - - - - - - - - - - - - Разность адресов */
	  if(oper_code==_SUB_OPER) {
		       value=(addr1-addr2)/iDgt_size(mode1) ;       /* Вычислеям разность адресов    */
								    /*   в единицах первого операнда */
		      P_TYPE=_DGT_VAL ;                             /* Уст.тип результата */
		      P_ADDR=&value ;                               /* Уст.адрес 'источника' */
		      P_SIZE=  1 ;                                  /* Уст.размер результата */
		      P_BUFF=  1 ;

			addr=(unsigned char *)                      /* Выделяем память под результат */
                               iAlloc_var(_WORK_VAR, P_RESULT, 1) ;
		     if(mError_code)  return(0) ;                   /* Обработка отсутствия памяти */
				   }
/*- - - - - - - - - - - - - - - - - - - - - - - Запрещенная операция */
     else              mError_code=_DCLE_BAD_OPERATION ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
				}
/*-------------------------------------------- Операции массив-число */

   else if(oper_type==_AD_TYPE) {
		if(type1==_DGT_VAL) {    tmp_v =operand1 ;          /* Приводим порядок операндов */
				       operand1=operand2 ;          /*    к порадку массив-число  */
				       operand2= tmp_v  ;
					 tmp_i =mode2 ;
					  mode2=mode1 ;
					  mode1= tmp_i ;    }

				size1=O1_SIZE ;
				buff1=O1_BUFF ;
				 dgt2=iDgt_get(addr2, mode2) ;      /* Извлекаем значение 2-го операнда */
/*- - - - - - - - - - - - - - - - - - - - - - Сдвиг указателя строки */
	  if(oper_code==_ADD_OPER ||
	     oper_code==_SUB_OPER   ) {
					 cnt=(int)(dgt2+_Q_SHIFT) ; /* Извлекаем число */
	       if(oper_code==_SUB_OPER)  cnt=-cnt ;                 /* Приводим вычитание к сложению */

				P_TYPE=_DGT_PTR | mode1 ;           /* Уст.тип результата */

	     if(addr1==NULL)    P_ADDR=NULL ;                       /* Если NULL-строка ... */
	     else            {                                      /* Если нормальная строка ... */
				P_ADDR=addr1+cnt*iDgt_size(mode1) ; /*   Опред.результирующий указатель */				     
		if(size1>=cnt)  P_SIZE=size1-cnt ;                  /*   Опред.результирующий размер */
		else            P_SIZE= 0 ;
		if(buff1>=cnt)  P_BUFF=buff1-cnt ;                  /*   Опред. размер буфера */
		else            P_BUFF= 0 ;
			     }
				      }
/*- - - - - - - - - - - - - - - - - - - - - - - Запрещенная операция */
     else              mError_code=_DCLE_BAD_OPERATION ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
				}
/*--------------------------------------------- Операции число-число */

   else if(oper_type==_DD_TYPE) {
				    P_TYPE=_DGT_VAL ;               /* Уст.тип и размер результата */
				    P_SIZE=  1 ;
				    P_BUFF=  1 ;
				    P_ADDR=&nDgt_val ;              /* Уст.чисточник' результата */

	       if(addr1==NULL ||                                    /* Обработка наличия NULL-адресов */
		  addr2==NULL   ) {  P_ADDR=NULL ;  return(0) ;  }  /*    операндов                   */

				   dgt1=iDgt_get(addr1, mode1) ;    /* Извлекаем адрес операндов */
				   dgt2=iDgt_get(addr2, mode2) ;
				   int1=(int)(dgt1+_Q_SHIFT) ;      /* Извлекаем целочисленное значение */
				   int2=(int)(dgt2+_Q_SHIFT) ;
/*- - - - - - - - - - - - - - - - - - - - -  Арифметические операции */
	  if(oper_code==_NEG_OPER)   nDgt_val=-dgt1 ;
     else if(oper_code==_INC_OPER)   nDgt_val= dgt1+1 ;
     else if(oper_code==_DEC_OPER)   nDgt_val= dgt1-1 ;
     else if(oper_code==_ADD_OPER)   nDgt_val= dgt1+dgt2 ;
     else if(oper_code==_SUB_OPER)   nDgt_val= dgt1-dgt2 ;
     else if(oper_code==_MUL_OPER)   nDgt_val= dgt1*dgt2 ;
     else if(oper_code==_DEV_OPER) {
		       if(dgt2!=0.)  nDgt_val=dgt1/dgt2 ;
		       else          nDgt_val=  0. ;
				   }
     else if(oper_code==_POW_OPER) {
		       if(dgt1<=0.)  nDgt_val= 0. ;
		       else          nDgt_val=pow(dgt1,dgt2) ;
				   }
     else if(oper_code==_AOR_OPER)   nDgt_val=int1|int2 ;
/*- - - - - - - - - - - - - - - - - - - - - - - - -  Сравнение чисел */
     else if(oper_code>=_LT_OPER &&
	     oper_code<=_GT_OPER   ) {

	   if(dgt1>dgt2)   status = 4 ;                             /* Определяем 'соотношение' чисел */
      else if(dgt1<dgt2)   status = 1 ;
      else                 status = 2 ;

	    nIf_ind=status & if_mask[oper_code-_LT_OPER] ;          /* Опред.результат сравнения */
	   nDgt_val=nIf_ind ;
				     }
/*- - - - - - - - - - - - - - - - - - - - - - -  Логические операции */
     else if(oper_code>= _OR_OPER &&
	     oper_code<=_INV_OPER   ) {

	if(oper_code== _OR_OPER) if(int1 || int2)  nDgt_val=1. ;    /* Операция 'Логическое ИЛИ' */
				 else              nDgt_val=0. ;

	if(oper_code==_AND_OPER) if(int1 && int2)  nDgt_val=1. ;    /* Операция 'Логическое ИЛИ' */
				 else              nDgt_val=0. ;

	if(oper_code==_INV_OPER) if(int1        )  nDgt_val=0. ;    /* Операция 'Логическое НЕ' */
				 else              nDgt_val=1. ;
				      }
/*- - - - - - - - - - - - - - - - - - - - - - - Запрещенная операция */
     else              mError_code=_DCLE_BAD_OPERATION ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
   if(!mError_code) {
    if(oper_code==_INC_OPER || oper_code==_DEC_OPER)                /* Для операций ++ и -- результат       */
		    iDgt_set(nDgt_val, addr1, mode1) ;              /*  размещается в 'источнике', для      */
                                                                    /*   остальных в только рабочей области */
                  iAlloc_var(_WORK_VAR, P_RESULT, 1) ; 

		       if(mError_code)  return(0) ;                 /* Обработка отсутствия памяти */
		    }

			     nIf_ind=(int)(nDgt_val+_Q_SHIFT) ;     /* Уст.индикатор условия */

				}
/*---------------------------------------- Запрещенные типы операций */

   else                         {
				   mError_code=_DCLE_BAD_OPERATION ;
				}
/*-------------------------------------------------------------------*/

  return(0) ;
}


/*********************************************************************/
/*                                                                   */
/*               Стекование параметров процессора                    */

typedef struct {
		  Dcl_decl *pars_table[_PARS_MAX] ;  /* Таблица параметров */
		       int  pars_cnt ;               /* Счетчик параметров текущего вызова */
		       int  allpars_cnt ;            /* Суммарный счетчик параметров */
		  Dcl_decl *transit ;                /* Таблица транзитных параметров */
		       int  transit_cnt ;            /* Число транзитных параметров */
		       int  transit_num ;            /* Номер транзитного параметра */
		  Dcl_decl  tmp ;                    /* Псевдо-ячейка */
		  Dcl_decl  result ;                 /* Описание результата */
		       int  result_dst ;             /* Ячейка размещения результата: */
		       int  if_ind ;                 /* Индикатор условия */
		    double  dgt_val ;                /* Числовое значение */
		      char *chr_pnt ;                /* 'Нужный' фрагмент строки */
		       int  work_cnt ;               /* Счетчик рабочих переменных */
		       int  fragm_flag ;             /* Флаг переменной с фрагментарным индексом */
#ifdef  _EXT_LIBS
		  Dcl_decl *ext_list[_LOAD_MAX] ;    /* Список заголовков подгруженных обьектов */
		       int  ext_cnt ;                /* Счетчик подгруженных обьектов */
#endif
               } Dcl_cstack ;


   int  Lang_DCL::iCulc_stack(int  oper_code)

{
   Dcl_cstack *addr ;

  static Dcl_cstack *cstack[_STACK_MAX] ;
  static        int  cstack_cnt ;

/*--------------------------------------------- Запись данных в стек */

  if(oper_code==_DCL_PUSH) {
/*- - - - - - - - - - - - - - - - - - - - - - - - - Подготовка стека */
     if(cstack_cnt==_STACK_MAX) {  mError_code=_DCLE_STACK_OVR ;    /* Если стек заполнен... */
						return(-1) ;      }

	   addr=(Dcl_cstack *)calloc(1, sizeof(*addr)) ;            /* Выделяем память под стек */	                      
	if(addr==NULL) {  mError_code=_DCLE_STACK_MEM ;             /* Если памяти нет... */
				 return(-1) ;           }

	       cstack[cstack_cnt]=addr ;                            /* Заносим адрес данных в стек */
		      cstack_cnt++    ;
/*- - - - - - - - - - - - - - - - - - - - -  Занесение данных в стек */
   memcpy(addr->pars_table, nPars_table, sizeof(nPars_table)) ;
	  addr->pars_cnt   =nPars_cnt ;
	  addr->allpars_cnt=nPars_cnt_all ;
	  addr->transit    =nTransit ;
	  addr->transit_cnt=nTransit_cnt ;
	  addr->transit_num=nTransit_num ;
	  addr->tmp        =nTmp ;
	  addr->result     =nResult ;
	  addr->result_dst =nResult_dst ;
	  addr->if_ind     =nIf_ind ;
	  addr->dgt_val    =nDgt_val ;
          addr->chr_pnt    =nChr_pnt ;
          addr->work_cnt   =nWork_cnt ;
          addr->fragm_flag =nFragm_flag ;
#ifdef  _EXT_LIBS
   memcpy(addr->ext_list, dcl_ext_list, sizeof(dcl_ext_list)) ;
	  addr->ext_cnt    =dcl_ext_cnt ;
#endif
/*- - - - - - - - - - - - - - - - - - - - - - - Инициализация данных */
	       nPars_cnt =  0 ;
	   nPars_cnt_all =  0 ;
	     nTransit_num=  0 ;
	     nResult_dst =  0 ;
	         nIf_ind =  0 ;
	       nWork_cnt =  0 ;
	      nFragm_flag=  0 ;
#ifdef  _EXT_LIBS
	     dcl_ext_cnt =  0 ;
#endif
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
		           }
/*--------------------------------------- Извлечение данных из стека */

  if(oper_code==_DCL_POP ) {
/*- - - - - - - - - - - - - - - - - - - - - - - - - Подготовка стека */
	 if(cstack_cnt<=0) {  mError_code=_DCLE_STACK_UND ;         /* Если стек пуст... */
					    return(-1) ;        }

		       cstack_cnt-- ;
	   addr=cstack[cstack_cnt] ;                                /* Извлекаем адрес данных из стек */
/*- - - - - - - - - - - - - - - - - - Восстановление данных из стека */
   memcpy(nPars_table,  addr->pars_table, sizeof(nPars_table)) ;
	  nPars_cnt    =addr->pars_cnt    ;
	  nPars_cnt_all=addr->allpars_cnt ;
	  nTransit     =addr->transit     ;
	  nTransit_cnt =addr->transit_cnt ;
	  nTransit_num =addr->transit_num ;
	  nTmp         =addr->tmp         ;
	  nResult      =addr->result      ;
	  nResult_dst  =addr->result_dst  ;
	  nIf_ind      =addr->if_ind      ;
	  nDgt_val     =addr->dgt_val     ;
	  nChr_pnt     =addr->chr_pnt     ;
	  nWork_cnt    =addr->work_cnt     ;
	  nFragm_flag  =addr->fragm_flag  ;
#ifdef  _EXT_LIBS
   memcpy(dcl_ext_list, addr->ext_list, sizeof(dcl_ext_list)) ;
	  dcl_ext_cnt  =addr->ext_cnt     ;
#endif
/*- - - - - - - - - - - - - - - - - - - - - - -  Освобождение памяти */
                                 free(addr) ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
		           }
/*-------------------------------------------------------------------*/

  return(0) ;
}

