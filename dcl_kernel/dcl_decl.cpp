/*   Bolotov P.D.  version  29.04.1994   */
/*     0 warnings at warning level 1     */

/*********************************************************************/
/*                                                                   */
/*                   DATA CONVERTION LANGUAGE                        */
/*                                                                   */
/*                    РАБОТА С ОПИСАНИЯМИ ОБЪЕКТОВ                   */
/*                                                                   */
/*********************************************************************/

#if defined(AIX) || defined(LINUX)
#ifndef UNIX
#define  UNIX
#endif
#endif


#include  <errno.h>
#include  <stddef.h>
#include  <stdlib.h>
#include  <string.h>
#include  <ctype.h>
#include  <math.h>

#ifdef UNIX
#define   stricmp  strcasecmp
#endif


#define DCL_INSIDE

#include "dcl.h"


#pragma warning(disable : 4267)
#pragma warning(disable : 4996)

/*----------------------------------- Описания типов и модификаторов */

typedef struct {
		   char *mnemo ;             /* Мнемоника описания */
		    int  mnemo_size ;        /* Размер мнемоники описания */
		    int  base ;              /* Код базового типа(и по умолчанию): */
#define                   _NO_DEF       -1   /*   Умолчание отсутствует            */
		    int  mode ;              /* Код модификатора базового типа: */
#define                       _SHORT     0   /*   Короткий                      */
#define                        _LONG     1   /*   Длинный                       */
#define                      _SIGNED     0   /*   Знаковый                      */
#define                    _UNSIGNED     2   /*   Беззнаковый                   */
		    int  pref_type ;         /* Тип префикса:                */
#define                       _SZ_PRFX   1   /*   Префикс размера            */
#define                       _US_PRFX   2   /*   Префикс знака              */
#define                       _PR_PRFX   4   /*   Префикс параметра          */
#define                       _EX_PRFX   8   /*   Префикс внешней переменной */
#define                       _PB_PRFX  16   /*   Префикс глобальной переменной */
		    int  pref_mask ;         /* Маска допустимости префиксов: */
//#define                     _SZ_PRFX  1    /*   Префикс размера             */
//#define                     _US_PRFX  2    /*   Префикс знака               */
//#define                     _PR_PRFX  4    /*   Префикс параметра           */
//#define                     _EX_PRFX  8    /*   Префикс внешней переменной */
	       } Dcl_type ;

			 /* Базовые типы:                 */
#define     _CHAR    1   /*  Символьная строка или символ */
#define  _INTEGER    2   /*  Целое число                  */
#define    _FLOAT    3   /*  Число с плавающей точкой     */
#define  _TYPEDEF    4   /*  Комплексный тип              */

#define  _DECL_CNT  11   /* Число возможных модификаторов */
#define  _DECL_BASE  1   /* Начало списка базовых модификаторов */
 static Dcl_type  dcl_type[]={    /* Описание модификаторов */
  { "<dummy>",   7, _TYPEDEF,       0,         0, _PB_PRFX | _PR_PRFX | _EX_PRFX   },

  { "char",      4,    _CHAR,       0,         0,  _PB_PRFX | _PR_PRFX | _EX_PRFX  },
  { "int",       3, _INTEGER,       0,         0,  _PB_PRFX | _PR_PRFX | _EX_PRFX |
						   _SZ_PRFX | _US_PRFX             },
  { "short",     5, _INTEGER,    _SHORT, _SZ_PRFX, _PB_PRFX | _PR_PRFX | _EX_PRFX |
						   _US_PRFX                        },
  { "long",      4, _INTEGER,     _LONG, _SZ_PRFX, _PB_PRFX | _PR_PRFX | _EX_PRFX |
						   _US_PRFX                        },
  { "float",     5,   _FLOAT,       0,         0,  _PB_PRFX | _PR_PRFX | _EX_PRFX |
						   _SZ_PRFX                        },
  { "double",    6,   _FLOAT,     _LONG, _SZ_PRFX, _PB_PRFX | _PR_PRFX | _EX_PRFX  },
  { "unsigned",  8,  _NO_DEF, _UNSIGNED, _US_PRFX, _PB_PRFX | _PR_PRFX | _EX_PRFX  },

  { "public",    6,  _NO_DEF,       0,   _PB_PRFX,       0                         },
  { "parameter", 9,  _NO_DEF,       0,   _PR_PRFX,       0                         },
  { "extern",    6,  _NO_DEF,       0,   _EX_PRFX,       0                         }
                             } ;

 static              int  dcl_x_redef ;
 static         Dcl_decl *dcl_x_elem ;

 static              int  dcl_typedef ;    /* Флаг нахождения в блоке typedef */

#define  CURR_X_TYPE  nX_types[nX_cnt]

/*--------------------------------------------- Системные переменные */


/*----------------------------------------------- Внешние переменные */


/************************************************************************/
/*                                                                      */
/*                   Обнаружение описаний переменных                    */
/*                                                                      */
/*    text  -  Анализируемый текст                                      */
/*                                                                      */
/*   Return:    1 - При наличии описаний                                */
/*              0 - При отсутствии описаний                             */

    int  Lang_DCL::iDecl_detect(char *text)


{
  int  i ;


     if(dcl_typedef)  return(1) ;

     if(!memcmp(text, "typedef", strlen("typedef")))  return(1) ;

     if(text[0]=='<')  return(1) ;

    for(i=0 ; i<_DECL_CNT ; i++)
     if(!memcmp(text, dcl_type[i].mnemo,
		      dcl_type[i].mnemo_size))  return(1) ;

   return(0) ;
}


/*********************************************************************/
/*                                                                   */
/*               Обработка описаний переменных                       */
/*                                                                   */
/*    text      -  Обрабатываемый текст                              */
/*    ext_proc  -  Описание переменной для внешней обработки         */
/*                                                                   */
/*  Структура описания простого типа переменной:                     */
/*       [parameter] [us_prefix] [sz_prefix] [type]                  */
/*                                                                   */
/*    где  parameter - префикс параметра ;                           */
/*         us_prefix - префикс знака: unsigned ;                     */
/*         sz_prefix - префикс размера: short, long, double ;        */
/*              type - базовый тип: char, int, float, block.         */
/*    В описании должен присутствовать хотя-бы один из               */
/*    модификаторов type или sz_prefix. Если присутствует только     */
/*    модификатор sz_prefix, то базовый тип устанавливается в        */
/*    соответствии со следующими умолчаниями:  short -> int          */
/*                                              long -> int          */
/*                                            double -> float        */
/*                                                                   */
/*      Если переменная имеет модификатор PARAMETER, но таблица      */
/*    транзитных параметров уже исчерпана - она считается            */
/*    внутренней переменной процесса.                                */
/*                                                                   */
/*      Если EXT_PROC!=NULL, то регистрация переменной не            */
/*    а ее данные выдаются в структуру EXT_PROC.                     */

    int  Lang_DCL::iDecl_maker(char *text, Dcl_decl *ext_proc)

{
   Dcl_decl *var ;         /* Указатель заголовка переменной */
       char *name ;        /* Имя переменной */
        int  buff_new ;    /* Размер нового буфера */
        int  size ;        /* Размер 'ячейки' обьекта */
        int  base_type ;   /* Базовый тип */
        int  mode ;        /* Модификатор базового типа */
        int  type ;        /* Тип переменной */
        int  pref_type ;   /* Тип префикса */
        int  par_flag ;    /* Флаг переменной-параметра */
        int  ext_flag ;    /* Флаг внешней переменной */
        int  pub_flag ;    /* Флаг глобальной переменной */
        int  ptr_flag ;    /* Флаг описания указателя */
        int  area_flag ;   /* Флаг описания массива */
        int  offset   ;    /* Флаг описания массива */
       char *addr ;
       char *end ;
       char *tmp ;
        int  i ;

/*--------------------------------- Входной контроль и инициализация */

/*- - - - - - - - - - - - -  Регистрация системных комплексных типов */
   if(nX_types==NULL) {

              nX_types=(Dcl_complex_type *)
                         realloc(nX_types,
                                 (nX_cnt+1)*sizeof(*nX_types)) ;

           strcpy(nX_types[nX_cnt].name, "<unknown>") ;
                  nX_types[nX_cnt].list    =NULL ;
                  nX_types[nX_cnt].list_cnt=  0 ;
                  nX_types[nX_cnt].size    =  0 ;
                           nX_cnt++ ;
                      }
/*------------------------------------------- Закрытие TYPEDEF-блока */

     if(dcl_typedef && text[0]=='}') {                              /* Закрытие TYPEDEF-блока */
                                         dcl_typedef=0 ;

        if(dcl_x_redef<0) {  nX_cnt++ ;                             /* Если первичное описание типа */
                              return(0) ;   }
 
           if(        nX_types[dcl_x_redef].list_cnt!=              /* Проверка переопределения типа */
                      nX_types[   nX_cnt  ].list_cnt   ||
               memcmp(nX_types[dcl_x_redef].list, 
                      nX_types[   nX_cnt  ].list,
                         sizeof(nX_types->list[0])*
                                nX_types[nX_cnt].list_cnt)  ) {
                                      mError_code=_DCLE_TYPEDEF_RDF ;
                                                return(0) ;           
                                                              }  

                                           return(0) ;           
                                     } 
/*-------------------------------- Подготовка элемента TYPEDEF-блока */

     if(dcl_typedef) {                                              /* Выделяем очередную запись             */
          CURR_X_TYPE.list_cnt++ ;                                  /*  в списке элементов комплексного типа */
          CURR_X_TYPE.list=(Dcl_decl *)
                            realloc(CURR_X_TYPE.list, CURR_X_TYPE.list_cnt*sizeof(Dcl_decl)) ;
                dcl_x_elem=&CURR_X_TYPE.list[CURR_X_TYPE.list_cnt-1] ;
                     }
/*------------------------------------------- Открытие TYPEDEF-блока */

     if(!memcmp(text, "typedef", strlen("typedef"))) {

           if(dcl_typedef) {                                        /* Проверяем на вложенность... */
                              mError_code=_DCLE_TYPEDEF_IN ;
                                    return(0) ;           
                           }
/*- - - - - - - - - - - - - - - - - - - - - - - - -  Разбор описания */
                 name=text+strlen("typedef") ;
                  end=strchr(name, '{') ;
               if(end==NULL) {
                                mError_code=_DCLE_TYPEDEF_STR ;
                                      return(0) ;           
                             }

                 *end=0 ;   
/*- - - - - - - - - - - - - - - - - - Проверка синтаксиса имени типа */
         if(name[        0     ]!='<' ||
            name[strlen(name)-1]!='>'   ) {
                                mError_code=_DCLE_TYPEDEF_NAME ;
                                      return(0) ;           
                                          }
/*- - - - - - - - - - - - - - - - - - - - - - Проверка типа по имени */
                                                     dcl_x_redef=-1 ;

          for(i=0 ; i<nX_cnt ; i++)
            if(!stricmp(nX_types[i].name, name))  dcl_x_redef= i ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - Регистрация типа */
              nX_types=(Dcl_complex_type *)
                         realloc(nX_types,
                                 (nX_cnt+1)*sizeof(*nX_types)) ;

          strncpy(nX_types[nX_cnt].name, name, sizeof(nX_types->name)-1) ;
                  nX_types[nX_cnt].list    =NULL ;
                  nX_types[nX_cnt].list_cnt=  0 ;
                  nX_types[nX_cnt].size    =  0 ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                                    dcl_typedef=1 ;
                                       return(0) ;           

                                                     }
/*---------------------------- Обнаружение и обработка описания типа */

		    base_type=_NO_DEF ;                             /* Сброс базового типа */
		    pref_type=   0 ;                                /* Сброс типа префикса */
			 mode=_SHORT ;                              /* Сброс модификатора типа */

  while(1) {                                                        /* CIRCLE.1 - Сканируем исходный */
								    /*                   текст ...   */
/*- - - - - - - - - - - - - - - - -  Идентификация комплексного типа */
     if(*text=='<') {

        for(i=0 ; i<nX_cnt ; i++)                                   /* Идентификация комплексного типа */
          if(!memcmp(text, nX_types[i].name,
                       strlen(nX_types[i].name)))  break ;

          if(i>=nX_cnt) {                                           /* При такой тип неизвестен... */
                           mError_code=_DCLE_NO_TYPE ;
                                   return(0) ;           
                        }

             dcl_type[0].mode      =   i ;                          /* Заполняем слот комплексного типа */ 
             dcl_type[0].mnemo_size=strlen(nX_types[i].name) ;
                         mode      =   i ;
                           i       =   0 ;
                    }   
/*- - - - - - - - - - - -  Идентификация очередного слова исх.текста */
     else           {
      
	for(i=_DECL_BASE ; i<_DECL_CNT ; i++)                       /* Сверяем текущее слово    */
	  if(!memcmp(text, dcl_type[i].mnemo,                       /*   со списком описателей  */
			   dcl_type[i].mnemo_size))  break ;

		if(i==_DECL_CNT)  break ;                           /* Если это 'постороннее' слово... */

                    }   

                        text+=dcl_type[i].mnemo_size ;              /* Переходим к следующему слову */
/*- - - - - - - - - - - - - - - - - - -  Анализ выявленного описания */
    if((pref_type|dcl_type[i].pref_mask)                            /* Контроль 'уместности' префикса */
	   !=     dcl_type[i].pref_mask ) {
                                       mError_code=_DCLE_BAD_PREF ;
					            return(0) ;           
                                          }

    if(dcl_type[i].pref_type) {                                     /* IF.1 - Если это префикс ... */
	if(pref_type & dcl_type[i].pref_type) {                     /* Контроль повторного использования */
                                      mError_code=_DCLE_DBL_MOD ;   /*      однородного префикса         */
						  return(0) ;     
                                              }

	      pref_type|=dcl_type[i].pref_type ;                    /* Корректируем тип префикса */
		   mode|=dcl_type[i].mode ;                         /* Корр.модификатор типа */

	if(dcl_type[i].base!=_NO_DEF)                               /* Если есть умолчание базового типа - */
			 base_type=dcl_type[i].base ;               /*   - устанавливаем его               */

			      }                                     /* ELSE.1 */
    else                      {                                     /* ELSE.1 - Если это базовый тип ... */
				 base_type=dcl_type[i].base ;       /* Уст.базовый тип */
				       break ;                      /* Прекр.обработку описания типа */
			      }                                     /* END.1 */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
	   }                                                        /* CONTINUE.1 */
/*--------------------- Анализ обнаружения и обработки описания типа */
/*- - - - - - - - - - - - - - - - -  Выявление переменных-параметров */
		       par_flag=0 ;                                 /* Сброс флага параметра */
		       ext_flag=0 ;                                 /* Сброс флага внешней переменной */
		       pub_flag=0 ;                                 /* Сброс флага глобальной переменной */

	  if((pref_type&_PR_PRFX)==_PR_PRFX)                        /* Если есть префикс параметра      */
	    if(nTransit_num!=nTransit_cnt)  par_flag=1 ;            /*  и транзитная таблица не         */
								    /*   исчерпана - уст.флаг параметра */
	  if((pref_type&_EX_PRFX)==_EX_PRFX)  ext_flag=1 ;          /* Если префикс внешней переменной ... */
	  if((pref_type&_PB_PRFX)==_PB_PRFX)  pub_flag=1 ;          /* Если префикс глобальной переменной ... */
/*- - - - - - - - - - - - - - - - - - - - - -  Общий анализ описания */
   if(!par_flag && !ext_flag) {                                     /* Если это не параметр и       */
								    /*    не внешняя переменная ... */
	  if(pref_type==   0  &&                                    /*  Если описания типа не обнаружено - */
	     base_type==_NO_DEF )  return(0) ;                      /*    прекращаем обработку             */

	  if(base_type==_NO_DEF) {  mError_code=_DCLE_NO_TYPE ;     /*  Если не задан базовый тип ... */
						return(0) ;      }
			      }
/*-------------------------------------- Обработка списка переменных */

  for( addr=text ; addr!=NULL ; text=addr+1) {                      /* CIRCLE.2 - Скан.список переменных ... */

		 addr=iFind_close(text, ',') ;                      /* Ищем разделитель параметров */
	      if(addr!=NULL)  *addr=0 ;                             /* Терминируем параметр */

		    ptr_flag=0 ;                                    /* Сброс флагов типа              */
		   area_flag=0 ;                                    /*   переменной: указатель/массив */
/*- - - - - - - - - - - - - - Выделяем и контролируем имя переменной */
	   if(*text=='*') {  ptr_flag=1 ;  text++ ;  }              /* Если индицирован указатель ... */

		     name=text ;                                    /* Уст.имя переменной */

	  for( ; *text!=0 && *text!='[' ; text++)                   /* Контроль состава символов имени */
	      if(!NAME_CHAR(*text)) {
                                  mError_code=_DCLE_SINTAX_NAME ;
					   break ;
                                    }
              if(mError_code)  break ;

	      if(*text=='[')  area_flag=1 ;                         /* Если опознан массив ... */

		*text=0 ;                                           /* Терминируем имя */
		 text++ ;
/*- - - - - - - - - - - - - - - - - - -  Обработка 'суммарного' типа */
	 if( area_flag && ptr_flag)  mError_code=_DCLE_PTR_AREA ;   /* Массив указателей запрещен ... */

         if( base_type==_TYPEDEF && area_flag)  area_flag=0 ;       /* Для объекта комплексного тип статический массив излишен */
                                                                    /*   так как он является динамическим массивом             */
	     if(mError_code)  return(0) ;                           /* Если есть ошибки ... */

		type=iType_define(base_type, mode,                  /* Определяем окончательный тип */
                                         ptr_flag, area_flag ) ;
	     if(mError_code)  return(0) ;                           /* Если есть ошибки ... */

	if(base_type==_INTEGER ||
	   base_type==_FLOAT     )  size=iDgt_size(type) ;          /* Опред.размер ячейки обьекта */
	else                        size= 1 ;
/*- - - - - - - - - - - - - - -  Определяем размер потребного буфера */
        if(area_flag) {                                             /* IF.1 - Если массив ... */

         if(text[0]==']') {                                         /* Если массив динамический... */
             if(dcl_typedef) {  mError_code=_DCLE_TYPEDEF_AREA ;    /* Запрещены динамические массивы */
                                               break ;           }  /*   внутри комплексного типа     */

                               buff_new=-1 ;
                          }
         else             {                                         /* Если массив статический... */

	           tmp=iFind_close(text, ']') ;                     /* Ищем окончание индекса */
                if(tmp==NULL ||                                     /* Если пустое или незакрытое */
		   tmp==text   ) {  mError_code=_DCLE_NOINDEX ;     /*    индексное выражение     */
						 break ;  }
		       *tmp=0 ;                                     /* Терминируем индексное выражение */

		    iCulculate(text) ;                              /* Вычисляем индексное выражение */
               if(mError_code)  break ;                             /* Если есть ошибки ... */

               if(t_base(nResult.type)!=_DGT_VAL) {                 /* Контроль типа индексного выражения */
			          mError_code=_DCLE_INDEX_TYPE ;
                                                     break ;
                                                  }

              buff_new=(int)iDgt_get(nResult.addr, nResult.type) ;  /* Извлекаем рез.вычисления индекса */
                          }

		      }                                             /* END.1 */
	 else if( ptr_flag)   buff_new=0 ;                          /* Если указатель ... */
	 else                 buff_new=1 ;                          /* Если простая переменная ... */

//                            buff_new*=size ;                      /* Приводим обьект к символьным размерам */
/*- - - - - - - - - - - - - -  Выдача описания для внешней обработки */
     if(ext_proc!=NULL) {

                        memset(ext_proc, 0, sizeof(*ext_proc)) ;
                       strncpy(ext_proc->name, name, sizeof(dcl_x_elem->name)-1) ;
 		               ext_proc->type=type ;
 		               ext_proc->size=size*buff_new ;
 		               ext_proc->buff=buff_new ;

                                    break ;
                        }
/*- - - - - - - - - - - - - - - - - - -  Обработка TYPEDEF-элементов */
     if(dcl_typedef) {
                        memset(dcl_x_elem, 0, sizeof(*dcl_x_elem)) ;

                       strncpy(dcl_x_elem->name, name, sizeof(dcl_x_elem->name)-1) ;
 		               dcl_x_elem->type=type ;
 		               dcl_x_elem->size=buff_new ;
 		               dcl_x_elem->buff=buff_new ;
#pragma warning(disable : 4312)
 		               dcl_x_elem->addr=(void *)CURR_X_TYPE.size  ;
#pragma warning(default : 4312)

                          offset=((size*buff_new-1)/4+1)*4 ;        /* Выравниваем размер под 32-разряда */
                        
                               CURR_X_TYPE.size+=offset ;

                                continue ;
                     }
/*- - - - - - - - - - - - - - - - - - -  Контроль наличия переменной */
        var=(Dcl_decl *)iTest_var(name, _DCL_INTERNAL) ;            /* Ищем переменную с заданным именем   */
     if(var!=NULL) {                                                /*   среди уже существующих внутренних */
                               mError_code=_DCLE_NAME_INUSE ;  
                                      break ;                  
                   }

     if(ext_flag) {                                                 /* Если это описание внешней переменной ... */
	  if(iTest_var(name, _DCL_EXTERNAL)!=NULL)  continue ;      /* Если существует такая внешняя    */
                                                                    /*  переменная - идем на след.имя,  */
                  }                                                 /*   иначе создаем внутр.переменную */
/*- - - - - - - - - - - - - - - - -  Обработка переменных-параметров */
     if(par_flag) {                                                 /* Если описание параметра ... */
                    strcpy(nTransit[nTransit_num].name, name) ;     /*  Заносим имя параметра в транзитную таблицу */
                           nTransit[nTransit_num].work_nmb= 0 ;     /*  Сброс признака рабочей ячейки */
                                    nTransit_num++ ;                /*  Модиф.номер параметра */

                    if(nTransit_num!=nTransit_cnt)  par_flag=0 ;    /*   Контролируем завершение транзитной таблицы */
				    continue ;                      /*   Идем на след.имя */
                  }
/*- - - - - - - - - - - - - - - - - - - - - - Регистрация переменной */
                 var=iNext_title(_INTERNAL_VAR) ;                   /* Размещаем новый заголовок */
	    if(mError_code)  break ;                                /*   Если есть ошибки ... */

                        strncpy(var->name, name, sizeof(var->name)-1) ;
		                var->type     =type ;
		                var->prototype=NULL ;
                                var->addr     =NULL ;

     if(base_type==_TYPEDEF) {  var->size     =  0 ;                /* Задание размера буфера */
		                var->buff     =  0 ;         }
     else
     if(buff_new < 0       ) {  var->prototype= "D" ;
                                var->size     =  0 ;
                                var->buff     =_DYN_CHUNK ;  }
     else                    { 

        if(base_type==_INTEGER ||
           base_type==_FLOAT     )  var->size =buff_new ;
        else                        var->size =  0 ;
                                
                                    var->buff  =buff_new ;    }

      if(pub_flag || ext_flag)  var->local_idx= 0 ;                 /* Задание зоны видимости перерменной */
      else                      var->local_idx=nLocal ;
/*- - - - - - - - - - - - - - - - - - - - Обработка текстовых блоков */
     if(var->type==_DCLT_CHR_PTR) {                                 /* Только для символьных указателей */ 

       for(i=0 ; i<nT_blocks_cnt ; i++)                             /* Ищем имя переменной среди текстовых блоков */
         if(!strcmp(nT_blocks[i].name, var->name))  break ;

         if(i<nT_blocks_cnt) {                                      /* Если нужны текстовой блок найден -     */
                                 var->addr= nT_blocks[i].data ;     /*  - присваиваем его значение переменной */
                                 var->size=strlen((char *)var->addr) ;
                                 var->buff= var->size+1 ;
                                             continue ;
                             }
                                  }
/*- - - - - - - - - - - - - - - - - - - Размещение буфера переменной */
     if(var->buff>0) {
                                iAlloc_var(_INTERNAL_VAR, var, 0) ;

				   if(mError_code)  break ;         /* Если есть ошибки ... */
                     }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
					     }                      /* CONTINUE.2 */
/*-------------------------------------------------------------------*/

    return(0) ;
}


/*********************************************************************/
/*                                                                   */
/*    Определение типа переменной по базовому типу (base_type)       */
/*       и модификатору (mode).                                      */
/*                                                                   */
/*     Результат выдается в переменную type.                         */

  int  Lang_DCL::iType_define(int  base_type, int  mode, 
                              int  ptr_flag,  int  area_flag)

{
  int  type ;

       if(base_type==_TYPEDEF) {                                    /* Обьект комплекного типа -  */
 	                            type=_DCLT_XTP_OBJ | mode<<8 ; 
                               } 
  else if(base_type==_CHAR   ) {
				 if(ptr_flag)  type=_CHR_PTR  ;
				 else          type=_CHR_AREA ;
			       }
  else if(base_type==_INTEGER) {                                    /* Определяем уточняющий */
		  if(mode==_UNSIGNED        )  type=_DCLT_USHORT ;  /*  тип для числовых     */
	     else if(mode==            _LONG)  type=_DCLT_LONG ;    /*   обьектов INTEGER    */
	     else if(mode==_UNSIGNED + _LONG)  type=_DCLT_ULONG ;
	     else                              type=_DCLT_SHORT ;
			       }
  else if(base_type==_FLOAT  ) {                                    /* Определяем уточняющий */
			      if(mode==_LONG)  type=_DCLT_DOUBLE ;  /*  тип для числовых     */
			      else             type=_DCLT_FLOAT ;   /*   обьектов FLOAT      */
			       }

       if(base_type==_FLOAT  ||                                     /* Определяем базовый тип */
	  base_type==_INTEGER  ) {                                  /*   для числовых обьектов */
		    if(area_flag)  type|=_DGT_AREA ;
	       else if( ptr_flag)  type|=_DGT_PTR ;
	       else                type|=_DGT_VAL ;
				 }

  return(type) ;
}


/************************************************************************/
/*                                                                      */
/*                 Проверка наличия переменной                          */
/*                                                                      */
/*   name  -  Имя переменной                                            */
/*   type  -  Область действия: _DCL_INTERNAL - внутренняя              */
/*                              _DCL_EXTERNAL - внешняя                 */

   void *Lang_DCL::iTest_var(char *name, int  type)

{
   Dcl_decl *vars ;       /* Текущий массив описаний переменных и процедур */
   Dcl_decl *targ ;       /* Указатель на описание переменной */
        int  i ;

/*-------------------------------------------- Инициализация */

		       targ=NULL ;

/*----------------------------- Работа с транзитной таблицей */
 
 if(type==_DCL_INTERNAL) {
  
    for(i=0 ; i<nTransit_cnt ; i++)
      if(!strcmp(nTransit[i].name, name)) {
                                             targ=&nTransit[i] ;
                                                  break ;
                                          }
                         }
/*------------------------- Работа с внутренними переменными */

 if(type==_DCL_INTERNAL) {

      for(vars=nInt_page ; vars->name[0]!=0 ; vars++)
        if(!strcmp(vars->name, name)) {  targ=vars ; 
                                            break ;  }
			 }
/*---------------------------- Работа с внешними переменными */

 if(type==_DCL_EXTERNAL) {

   for(i=0 ; targ==NULL ; i++) {

	if(nVars[i]==   NULL  ||
	   nVars[i]==nInt_page  )  break ;

    for(vars=nVars[i] ; vars->name[0]!=0 ; vars++)
     if(        vars->func_flag==0 &&
                vars->local_idx==0 &&
	!strcmp(vars->name, name)    ) {  targ=vars ;
					     break ;  }

			       }
			 }
/*----------------------------- Работа с внешней библиотекой */

   if( targ   ==NULL &&                                     /* Если обьект не найден и задана             */
      mExt_lib!=NULL   ) {                                  /*  процедура связи с внешней библиотекой ... */

		 targ=mExt_lib(_DCL_LOAD_FNC, name) ;       /*  Ищем обьект во внешней библиотеке */
                         }
/*-----------------------------------------------------------*/

  return(targ) ;
}
