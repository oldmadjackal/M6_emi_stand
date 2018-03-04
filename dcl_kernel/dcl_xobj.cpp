/*   Bolotov P.D.  version  29.04.1994   */
/*     0 warnings at warning level 1     */

/*********************************************************************/
/*                                                                   */
/*                   DATA CONVERTION LANGUAGE                        */
/*                                                                   */
/*             РАБОТА с ОБЪЕКТАМИ КОМПЛЕКСНОГО ТИПА                  */
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
#include  <memory.h>
#include  <malloc.h>

#define   DCL_INSIDE
#include "dcl.h"
#include "dcl_md5.h"

#pragma warning(disable : 4996)

/*---------------------------------------------------- Внешние связи */

       extern    double  dcl_errno ;           /* Системный указатель ошибок -> DCL_SLIB.CPP */
       static       int  dcl_sql_inner ;       /* Флаг внутреннего выделения интерфейса по-умолчанию */
       static      char  dcl_sql_error[512] ;  /* Буфер ошибки для освобождаемых интерфейсов */


/*********************************************************************/
/*                                                                   */
/*              Проверка на наличие операций "комплексирования"      */
/*                                                                   */
/*    beg       -  Начало обраб. фрагмента                           */
/*    end       -  Окончание обраб. фрагмента                        */
/*    ptr       -  Указатель положения точки                         */

   int  Lang_DCL::iXobject_check(char *beg, char *end, char **ptr)

{
  int  check ;

           check=0 ;

  for(*ptr=end ; *ptr>=beg ; (*ptr)--)
         if(           **ptr=='.') {  check=1 ;  break ;  }
    else if(!NAME_CHAR(**ptr)    )    break ;


  if(check) { 
                                                        check=0 ; 
      for( ; beg<=end ; beg++)  if(!DIGITS_CHAR(*beg))  check=1 ;
            }

  return(check) ;
}


/*********************************************************************/
/*                                                                   */
/*              Очистка ресурсов объекта комплексного типа           */
/*                                                                   */
/*    object    -  Объект комплексного типа                          */

   int  Lang_DCL::iXobject_clear(Dcl_decl *object)

{
  Dcl_complex_record *record ;
                void *record_ ;
                 int  i ;

/*---------------------------------------- Формирование новой записи */

   if(object->addr!=NULL) {

      for(record=(Dcl_complex_record *)object->addr,
                               i=1 ; i<object->buff ; i++ ) {

              record_=(              void *)record ;
              record =(Dcl_complex_record *)record->next_record ;
         free(record_) ;
                                                            }
                          }
/*-------------------------------------------- Инициализация объекта */

               object->addr=NULL ;
               object->buff=  0 ;
               object->size=  0 ;

/*-------------------------------------------------------------------*/

  return(0) ;
}


/*********************************************************************/
/*                                                                   */
/*              Добавление записи для объекта комплексного типа      */
/*                                                                   */
/*    object    -  Объект комплексного типа                          */

   int  Lang_DCL::iXobject_add(Dcl_decl *object, Dcl_complex_type *master)

{
                 int  type_idx ;
                 int  size ;
                char *row ;
  Dcl_complex_record *record ;
            Dcl_decl *elems ;
                char *data ;
                 int  type ;
                 int  cell ;
                 int  i ;
                 int  j ;

/*------------------------------------------------------- Подготовка */

   if(master==NULL) {

        type_idx=t_mode(object->type)>>8 ;
//   if(type_idx==0)  return(0) ;

          master=&nX_types[type_idx] ;
                    }

            size=sizeof(Dcl_complex_record) 
                +master->list_cnt*sizeof(Dcl_decl)
                +master->size ;

/*---------------------------------------- Формирование новой записи */
                    
              row=(char *)calloc(1, size) ;

           record=(Dcl_complex_record *)row ;
            elems=(Dcl_decl *)(row+sizeof(Dcl_complex_record)) ;
             data=row+master->list_cnt*sizeof(Dcl_decl)+sizeof(Dcl_complex_record) ;

                record->size     =master->size ;
                record->elems    =elems ;
                record->elems_cnt=master->list_cnt ;

         for(j=0 ; j<master->list_cnt ; j++) {
                      elems[j]     =master->list[j] ;
                      elems[j].addr=data+(long)(elems[j].addr) ;

              type=t_base(elems[j].type) ;
           if(type==_DGT_VAL  ||                                    /* Приведение размеров    */
              type==_DGT_AREA ||                                    /*    числовых переменных */
              type==_DGT_PTR    )  cell=iDgt_size(elems[j].type) ;
           else                    cell=  1 ;

           if(elems[j].prototype!=NULL &&
              elems[j].size     !=  0    )
                 memmove(elems[j].addr, elems[j].prototype, elems[j].size*cell) ;
                                             }
/*---------------------------------------- Включение записи в объект */

    if(object->buff==0) {                                           /* Если это первая запись... */
                              object->addr=row ;
                        }
    else                {                                           /* Если это следующая запись... */

          for(record=(Dcl_complex_record *)object->addr,
                             i=1 ; i<object->buff ; i++ )
                   record=(Dcl_complex_record *)record->next_record ;

                            record->next_record=row ;
                        }

                              object->size =object->buff ;
                              object->buff++ ;

/*-------------------------------------------------------------------*/

  return(0) ;
}


/*********************************************************************/
/*                                                                   */
/*   Пропись значений текущей записи для объекта комплексного типа   */
/*                                                                   */
/*    object      -  Объект комплексного типа                        */
/*    values      -  Список значений, терминированный name[0]=0      */
/*    values_cnt  -  Число значений                                  */

   int  Lang_DCL::iXobject_set(Dcl_decl *object, Dcl_decl *values, int  values_cnt)

{
  Dcl_complex_record *record ;
            Dcl_decl *field ;
                char *name ;
                 int  append_flag ;
                 int  type ;
                 int  size ;
                 int  i ;
                 int  j ;

/*---------------------------------------- Выходим на текущую запись */
                 
    for(record=(Dcl_complex_record *)object->addr,                  /* Проходим на нужную запись */
                               i=0 ; i<object->size ; i++ )
        record=(Dcl_complex_record *)record->next_record ;

/*--------------------------------------------- Простановка значений */
/*- - - - - - - - - - - - - - - - - - - -  Установка ПУСТЫХ значений */
   if(values==NULL) {

       for(field=record->elems, j=0 ; 
               j<record->elems_cnt ; field++, j++)  field->size=0 ;

                    }
/*- - - - - - - - - - - - - - - - - - -  Установка НЕПУСТЫХ значений */
   else             {
                   
     for(i=0 ; i<values_cnt ; i++) {

                                   name=values[i].name ;
                            append_flag= 0 ;
        if(name[0]=='+') {  append_flag= 1 ;  name++ ; }

        if(!stricmp(name, "IGNORE"))  continue;                     /* Игнорируем поля, указанные с именем IGNORE */

       for(field=record->elems, j=0 ;                               /* Ищем поле */
               j<record->elems_cnt ; field++, j++) 
         if(!strcmp(field->name, name))  break ;
    
         if(j>=record->elems_cnt)  return(-1) ;                     /* Если такого поля нет... */

          type=t_base(field->type) ;                                /* Извлекаем тип элемента */ 
       if(type!=_CHR_AREA &&                                        /* Если числовая переменная... */
          type!=_CHR_PTR    ) {

                          size=values[i].size*iDgt_size(field->type) ;
                  memcpy(field->addr, values[i].addr, size) ;       /*  Занесение данных */

                              }
       else                   {                                     /* Если строчная переменная... */

                                   size=values[i].size ;

         if(append_flag) {                                          /* Если добавление значения к существующему... */
                if(field->size+size>field->buff)                    /*  Контроль переполнения буфера значения */
                               size=field->buff-field->size ;

                if(size>0) {                                        /*  Занесение данных */
                      memcpy((char *)field->addr+field->size, values[i].addr, size) ;
                             field->size+=size ;
                           }
                         }
         else            {                                          /* Если перезапись значения... */
                if(size>field->buff)  size=field->buff ;            /*  Контроль переполнения буфера значения */

                     memcpy(field->addr, values[i].addr, size) ;    /*  Занесение данных */
                            field->size=size ;
                         }
                              }

                                   }
                    }
/*-------------------------------------------------------------------*/

  return(0) ;
}


/*********************************************************************/
/*                                                                   */
/*              Индексирование объекта комплексного типа             */
/*                                                                   */
/*    object    -  Объект комплексного типа                          */
/*    idx       -  Индекс                                            */
/*    operand   -  Описатель "строки" объекта                        */

   int  Lang_DCL::iXobject_index(Dcl_decl *object, int *idx, Dcl_decl *operand)

{
  Dcl_complex_record *record ;
                 int  i ;

/*------------------------------------------------------- Подготовка */

/*--------------------------------------- Адресация последней записи */

      if(*idx<0)  *idx=object->buff-1 ;
      if(*idx<0)  *idx= 0 ;

/*---------------------------------------------- Доразмещение буфера */

   while(*idx>=object->buff) {
                                   iXobject_add(object, NULL) ;
                               if(mError_code)  return(0) ;
                             }
/*------------------------------------------ Формирование результата */

    for(record=(Dcl_complex_record *)object->addr,                  /* Проходим на нужную запись */
                               i=0 ; i<*idx ; i++ )
        record=(Dcl_complex_record *)record->next_record ;

     memset(operand, 0, sizeof(*operand)) ;

            operand->type=object->type | _DCLT_XTP_REC ;
            operand->addr=(void *)record ;
 
/*-------------------------------------------------------------------*/

  return(0) ;
}


/*********************************************************************/
/*                                                                   */
/*       Удаление записей записей объекта комплексного типа          */
/*                                                                   */
/*    object    -  Объект комплексного типа                          */
/*                                                                   */
/*  Удаляются записи, у которых EXEC_MARK=1                          */

   int  Lang_DCL::iXobject_delete(Dcl_decl *object)

{
  Dcl_complex_record  *record ;
  Dcl_complex_record  *record_next ;
                void **link ;
                 int   i ;

/*-------------------------------------------------- Перебор записей */

                        link=&object->addr ;

    for(record=(Dcl_complex_record *)object->addr,                  /* LOOP - Перебор записей объекта */
                             i=0 ; i<object->buff ; ) {
/*- - - - - - - - - - - - - - - - - - - - - -  Если запись удаляется */
      if(record->exec_mark) {

             record_next=(Dcl_complex_record *)record->next_record ;

                        free(record) ;

                             record=record_next ;
                              *link=record_next ;

                                object->buff-- ;

                            }
/*- - - - - - - - - - - - - - - - - - - - - - - Если запись остается */
      else                  {

               link=&(record->next_record) ;

             record=(Dcl_complex_record *)record->next_record ;

                                    i++ ;
                            }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                                                          }         /* ENDLOOP - Перебор записей объекта */

                *link=NULL ;

/*-------------------------------------------------------------------*/

  return(0) ;
}


/*********************************************************************/
/*                                                                   */
/*    Добавление поля в структуру записей объекта комплексного типа  */
/*                                                                   */
/*    object    -  Объект комплексного типа                          */
/*    field     -  Описание добавляемого типа                        */

   int  Lang_DCL::iXobject_extend(Dcl_decl *object, Dcl_decl *field)

{
                 int   offset ;
                 int   size ;
  Dcl_complex_record  *record ;
  Dcl_complex_record  *record_new ;
                void **link ;
                 int   i ;
                 int   j ;

/*------------------------------------------------------- Подготовка */

                 offset=((field->size-1)/4+1)*4 ;                   /* Выравниваем размер добавляемого поля под 32-разряда */

/*-------------------------------------------------- Перебор записей */

                        link=&object->addr ;

    for(record=(Dcl_complex_record *)object->addr,                  /* LOOP - Перебор записей объекта */
                             i=0 ; i<object->buff ; i++ ) {
/*- - - - - - - - - - - - - - - - - - - - - -  Переразмещение записи */
            size=sizeof(Dcl_complex_record)                         /* Получаем размер новой записи */
                +(record->elems_cnt+1)*sizeof(Dcl_decl)
                + record->size+offset ;

       record_new       =(Dcl_complex_record *)                     /* Переразмещаем запись */
                                 realloc(record, size) ;     
       record_new->elems=(Dcl_decl *)
                           ((char *)record_new+sizeof(Dcl_complex_record)) ;
/*- - - - - - - - - - - - - - - - - - - Корректировка "старых" полей */
             size=record_new->size ;                                /* Получаем размер буфера данных старой записи */

      for(j=0 ; j<record_new->elems_cnt ; j++)                      /* Корректируем указатели на данные "старых" полей */
         record_new->elems[j].addr= (char *)record_new+
                                   ((char *)record_new->elems[j].addr-(char *)record)+
                                    sizeof(Dcl_decl) ;

        memmove((char *)record_new->elems[0].addr,                  /* Сдвигаем данные на новое место */ 
                (char *)record_new->elems[0].addr-sizeof(Dcl_decl), size) ;
/*- - - - - - - - - - - - - - - - - - - - - - Добавление нового поля */
                           j      = record_new->elems_cnt ;
         record_new->elems[j]     =*field ;
         record_new->elems[j].size= 0 ;
         record_new->elems[j].addr=(char *)record_new+
                                     sizeof(Dcl_complex_record)+
                                    (record->elems_cnt+1)*sizeof(Dcl_decl)+size ;
         record_new->elems_cnt++ ;
         record_new->size     +=offset ;
/*- - - - - - - - - - - - - - - - - - Переадресация "верхней" ссылки */
                *link=  record_new ;
                 link=&(record_new->next_record) ;
/*- - - - - - - - - - - - - - - - - - - - Переход к следующей записи */
          record=(Dcl_complex_record *)record_new->next_record ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                                                          }         /* ENDLOOP - Перебор записей объекта */
/*-------------------------------------------------------------------*/

  return(0) ;
}


/*********************************************************************/
/*                                                                   */
/*   Сортировка записей объекта комплексного типа по заданному полю  */
/*                                                                   */
/*    object    -  Объект комплексного типа                          */
/*    field     -  Поле, по которому производится сортировка         */

  static      char *sort_field ;
  static  Lang_DCL *sort_kernel ;
 
   int  iXobjectSortProc(const void *elem1, const void *elem2 ) ;


   int  Lang_DCL::iXobject_sort(Dcl_decl *object, char *field)

{
  Dcl_complex_record **sort_list ;
  Dcl_complex_record  *record   ;
                 int   i ;

/*------------------------------------------------- Входной контроль */

   if(object->buff<2)  return(0) ;                                  /* Если менее 2 записей... */

/*-------------------------------------- Формирование списка записей */

         sort_list=(Dcl_complex_record **)                          /* Выделяем память под список */
                      calloc(object->buff, sizeof(*sort_list)) ;

    for(record=(Dcl_complex_record *)object->addr,                  /* LOOP - Перебор записей объекта */
                             i=0 ; i<object->buff ; i++ ) {
       sort_list[i]=  record ;
          record   =(Dcl_complex_record *)record->next_record ;
                                                          }         /* ENDLOOP - Перебор записей объекта */
/*----------------------------------------------- Сортировка записей */

                sort_field =field ;
                sort_kernel=this ;

     qsort(sort_list, object->buff, 
                        sizeof(*sort_list), iXobjectSortProc) ;

   if(mError_code)  return(-1) ;

/*------------------------------ Формирование списка записей объекта */

            object->addr=sort_list[0] ;                             /* Фиксируем адрес первой записи */
 
      for(i=0 ; i<object->buff-1 ; i++)                             /* Формируем цепочку ссылок */
            sort_list[i]->next_record=sort_list[i+1] ;

            sort_list[i]->next_record=NULL ;                        /* В последней записи ставим "терминатор" */

/*-------------------------------------------------------------------*/

  return(0) ;
}


   int  iXobjectSortProc(const void *elem1, const void *elem2 )
{
  Dcl_complex_record **rec_link ;
  Dcl_complex_record  *rec_1 ;
  Dcl_complex_record  *rec_2 ;
            Dcl_decl  *sort_1 ;
            Dcl_decl  *sort_2 ;
                 int   type ;
                 int   size_1 ;
                 int   size_2 ;
                 int   size  ;
              double   value_1 ;
              double   value_2 ;
                 int   status ;
                 int   i ;

         
        rec_link=(Dcl_complex_record **)elem1 ;
        rec_1   =  *rec_link ;
        rec_link=(Dcl_complex_record **)elem2 ;
        rec_2   =  *rec_link ;
       
   for(sort_1=NULL, i=0 ; i<rec_1->elems_cnt ; i++)
     if(!strcmp(rec_1->elems[i].name, sort_field)) {  sort_1=&rec_1->elems[i] ;  break ;  }

   for(sort_2=NULL, i=0 ; i<rec_2->elems_cnt ; i++)
     if(!strcmp(rec_2->elems[i].name, sort_field)) {  sort_2=&rec_2->elems[i] ;  break ;  }

          if(sort_1==NULL && sort_2==NULL)  return( 0) ;            /* Обработка отсутствия поля */ 
     else if(sort_1==NULL && sort_2!=NULL)  return( 1) ;
     else if(sort_1!=NULL && sort_2==NULL)  return(-1) ;

          if(sort_1->type!=sort_2->type) {                          /* Проверка однотипности поля сортировки */
                sort_kernel->mError_code=_DCLE_TYPEDEF_SORT ;
                                                return(0) ;
                                         }  
       type=t_base(sort_1->type) ;                                  /* Извлекаем базовый тип */

    if(type==_DGT_VAL) {
                          value_1=sort_kernel->iDgt_get(sort_1->addr, type) ;
                          value_2=sort_kernel->iDgt_get(sort_2->addr, type) ;
                       if(value_1>value_2)  status= 1 ;
                  else if(value_1<value_2)  status=-1 ;
                  else                      status= 0 ;
                                     return(status) ;
                       }

    if(type==_CHR_AREA ||
       type==_CHR_PTR    ) {

              size_1=sort_1->size ;
              size_2=sort_2->size ;
              size  =size_2>size_1?size_1:size_2 ;

            status=memcmp(sort_1->addr, sort_2->addr, size) ;
         if(status==  0   &&
            size_1!=size_2  )  status=size_2>size_1?-1:1 ;
  
                               return(status) ;
                           }

     sort_kernel->mError_code=_DCLE_TYPEDEF_SORT ;

   return(0) ;
}


/*********************************************************************/
/*                                                                   */
/*    Сравнение наборов записей объектов комплексного типа           */
/*                                                                   */
/*    Предварительно наборы должны быть упорядочены по возрастанию   */
/*    значения ключевого поля                                        */
/*                                                                   */
/*    result    -  Объект для размещения результата                  */
/*    object_1  -  Первый исходный объект                            */
/*    object_2  -  Второй исходный объект                            */
/*    mode      -  Режим сравнения: ANY, NOT_IN_1, NOT_IN_2,         */
/*                   BY_VALUE. NULL соответсвует ANY                 */
/*    key       -  Ключевое поле, если NULL - в качестве ключевого   */
/*                   используется первое по порядку поле в записи    */

   int  Lang_DCL::iXobject_diff(Dcl_decl *result, 
                                Dcl_decl *object_1,
                                Dcl_decl *object_2,
                                    char *diff_mode,
                                    char *key       )

{
  Dcl_complex_record *record_1 ;
  Dcl_complex_record *record_2 ;
                 int  id_pos_1 ;
                 int  id_pos_2 ;
                void *id_data_1 ;
                void *id_data_2 ;
                 int  id_size_1 ;
                 int  id_size_2 ;
                 int  n1 ;
                 int  n2 ;
                 int  n1_prv ;
                 int  n2_prv ;
                 int  status ;
                 int  mode ;
                 int  save_result ;
                 int  cnt ;
                 int  i ;

#define        _ANY_MODE   0
#define   _NOT_IN_1_MODE   1
#define   _NOT_IN_2_MODE   2
#define   _BY_VALUE_MODE   4

/*---------------------------------------------------- Инициализация */

                id_data_1=NULL ;
                id_data_2=NULL ;
                id_size_1=  0 ;
                id_size_2=  0 ;

/*------------------------------------------ Разбор режима сравнения */

         if(         diff_mode==NULL       )  mode=     _ANY_MODE ;
    else if(!stricmp(diff_mode, "ANY"     ))  mode=     _ANY_MODE ;
    else if(!stricmp(diff_mode, "NOT_IN_1"))  mode=_NOT_IN_1_MODE ;
    else if(!stricmp(diff_mode, "NOT_IN_2"))  mode=_NOT_IN_2_MODE ;
    else if(!stricmp(diff_mode, "BY_VALUE"))  mode=_BY_VALUE_MODE ;
    else                                    {    
                                     mError_code=_DCLE_CALL_INSIDE ;
                             strncpy(mError_details, "Unknown compare mode", sizeof(mError_details)-1) ;
                                              return(-1) ;  
                                            }
/*------------------------------- Определение позиции ключевого поля */

   if(key!=NULL) {
                           id_pos_1=0 ;
                           id_pos_2=0 ;
                 }
   else          {
                           id_pos_1=0 ;
                           id_pos_2=0 ;
                 }
/*---------------------------------------- Сравнение наборов записей */

           record_1=(Dcl_complex_record *)object_1->addr ;
           record_2=(Dcl_complex_record *)object_2->addr ;

   for(n1=0, n2=0, n1_prv=-1, n2_prv=-1 ; ; ) {

               save_result=0 ;
/*- - - - - - - - - - - - - - - - - - - - - - - - Завершение списков */
     if(n1>=object_1->buff ||
        n2>=object_2->buff   )  break ; 
/*- - - - - - - - - - - - - - - - - - - -  Извлечение ключевых полей */
     if(n1!=n1_prv) {

             if(n1!=0)  record_1=(Dcl_complex_record *)record_1->next_record ;

                       id_data_1=record_1->elems[id_pos_1].addr ;
                       id_size_1=record_1->elems[id_pos_1].size ;

                          n1_prv=n1 ;
                    }
     if(n2!=n2_prv) {

             if(n2!=0)  record_2=(Dcl_complex_record *)record_2->next_record ;

                       id_data_2=record_2->elems[id_pos_2].addr ;
                       id_size_2=record_2->elems[id_pos_2].size ;

                          n2_prv=n2 ;   
                    }     
/*- - - - - - - - - - - - - - - - - - - - - Сравнение ключевых полей */
        status=memcmp(id_data_1, id_data_2, 
                          id_size_1<id_size_2?id_size_1:id_size_2) ;
     if(status==0) status=id_size_1-id_size_2 ;

     if(status>0) { 
                      if(mode==     _ANY_MODE ||
                         mode==_NOT_IN_1_MODE   )  save_result=1 ;

                           n2++ ; 
                  }
     else
     if(status<0) {
                      if(mode==     _ANY_MODE ||
                         mode==_NOT_IN_2_MODE   )  save_result=1 ;

                           n1++ ; 
                  }
/*- - - - - - - - - - - - - - - - - - - - - - - Сравнение всех полей */
     else         {
                      if(mode==     _ANY_MODE ||
                         mode==_BY_VALUE_MODE   ) {

#define  R1  record_1->elems
#define  R2  record_2->elems

                                cnt=record_1->elems_cnt<record_2->elems_cnt
                                                       ?record_1->elems_cnt
                                                       :record_2->elems_cnt ;

                         for(i=0 ; i<cnt ; i++) {
                              status=memcmp(R1[i].addr, R2[i].addr, R1[i].size<R2[i].size?R1[i].size:R2[i].size) ;
                           if(status==0) status=R1[i].size-R2[i].size ;
                           if(status!=0) { 
                                            save_result=1 ;
                                                 break ;
                                         }
                                                } 

#undef   R1
#undef   R2
                                                  }
                           n1++ ; n2++ ;
                  }
/*- - - - - - - - - - - - - - - - - - - Регистрация отличных записей */
     if(save_result) {
                              iXobject_add(result, NULL) ;

       if(n1!=n1_prv)  status=iXobject_set(result, record_1->elems, record_1->elems_cnt) ;
       else            status=iXobject_set(result, record_2->elems, record_2->elems_cnt) ;

                    if(status) {
                                  mError_code=_DCLE_TYPEDEF_ELEM ;
                                      return(-1) ; 
                               }
                     }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                                              }
/*---------------------------------------------------- Запись хвоста */

    if(mode==     _ANY_MODE ||
       mode==_NOT_IN_2_MODE   ) {

       for( ; n1<object_1->buff ; n1++) {

                              iXobject_add(result, NULL) ;
                       status=iXobject_set(result, record_1->elems, record_1->elems_cnt) ;
                    if(status)  return(-1) ;

                        record_1=(Dcl_complex_record *)record_1->next_record ;
                                        }
                                }

    if(mode==     _ANY_MODE ||
       mode==_NOT_IN_1_MODE   ) {

       for( ; n2<object_2->buff ; n2++) {

                              iXobject_add(result, NULL) ;
                       status=iXobject_set(result, record_2->elems, record_2->elems_cnt) ;
                    if(status)  return(-1) ;

                        record_2=(Dcl_complex_record *)record_2->next_record ;
                                        }

                                }
/*-------------------------------------------------------------------*/

#undef        _ANY_MODE
#undef   _NOT_IN_1_MODE
#undef   _NOT_IN_2_MODE
#undef   _BY_VALUE_MODE


   return(0) ;
}


/*********************************************************************/
/*                                                                   */
/*        Операции над полями записи объекта комплексного типа       */
/*                                                                   */
/*    record    -  Запись объекта комплексного типа                  */
/*    oper      -  Название операции                                 */
/*    oper_size -  Длина названия операции                           */

   Dcl_decl *Lang_DCL::iXobject_field(Dcl_complex_record *record, 
                                                    char *oper, int  oper_size)

{
 static   double  dgt_value ;          /* Буфер числового значения */
 static Dcl_decl  dgt_return={_DGT_VAL, 0,0,0,"", &dgt_value, NULL, 1, 1} ;

#define   IS_KEYWORD(p)  (oper_size==strlen(p) && !memcmp(oper, p, strlen(p)))

/*------------------------------------------------------- Подготовка */

/*----------------------------------------- Разбор ссылок и операций */

    if(IS_KEYWORD("field$name" )) {
                                        return(&record->fld_name) ;
                                  } 
    else
    if(IS_KEYWORD("field$value")) {
                                        return(&record->fld_value) ;
                                  }
    else
    if(IS_KEYWORD("field$first")) {
                                        record->fld_idx=0 ;
                                  }
    else
    if(IS_KEYWORD("field$next" )) {
                                        record->fld_idx++ ;
                                  }
    else                          {
                                      mError_code=_DCLE_TYPEDEF_ELEM ;
                                          return(NULL) ;
                                  }

#undef   IS_KEYWORD

/*-------------------------------------------- Позиционирование поля */

#define  NAME   record->fld_name
#define  VALUE  record->fld_value
#define  ELEM   record->elems[record->fld_idx]

                   memset(&NAME,  0, sizeof(NAME )) ;
                   memset(&VALUE, 0, sizeof(VALUE)) ;
 
                        dgt_value=-1 ;

   if(record->fld_idx<record->elems_cnt) {

                             NAME.type=   _DCLT_CHR_AREA ;
                             NAME.addr=       ELEM.name ;
                             NAME.size=strlen(ELEM.name) ;
                             NAME.buff=sizeof(ELEM.name) ;
 
                             VALUE    =       ELEM ;

                                  dgt_value=0 ;

                                         }

#undef   NAME
#undef   VALUE
#undef   ELEM

/*-------------------------------------------------------------------*/

  return(&dgt_return) ;
}


/*********************************************************************/
/*                                                                   */
/*           Разборка вызова метода объекта комплексного типа        */
/*                                                                   */
/*    beg       -  Начало обраб. фрагмента                           */
/*    end       -  Окончание обраб. фрагмента                        */

   Dcl_decl *Lang_DCL::iXobject_method(char *beg, char *end)

{
  Dcl_decl *vars ;
       int  n ;
       int  i ;

  static  Dcl_decl  xobject_methods[]={
	 {_DGT_VAL,  _DCL_METHOD, 0, 0, "add_field",         NULL, "s",                  0, 0},
	 {_DGT_VAL,  _DCL_METHOD, 0, 0, "sort",              NULL, "s",                  0, 0},
	 {_DGT_VAL,  _DCL_METHOD, 0, 0, "find",              NULL, "sasasasasasasasasa", 0, 0},
	 {_DGT_VAL,  _DCL_METHOD, 0, 0, "get_files_by_mask", NULL, "s",                  0, 0},
	 {_DGT_VAL,  _DCL_METHOD, 0, 0, "get_files",         NULL, "s",                  0, 0},
	 {_DGT_VAL,  _DCL_METHOD, 0, 0, "trace_size",        NULL, "v",                  0, 0},
	 {_DGT_VAL,  _DCL_METHOD, 0, 0, "marked_only",       NULL, "v",                  0, 0},
         {_DGT_VAL,  _DCL_METHOD, 0, 0, "read_csv",          NULL, "ss",                 0, 0},
         {_DGT_VAL,  _DCL_METHOD, 0, 0, "write_csv",         NULL, "ss",                 0, 0},
         {_DGT_VAL,  _DCL_METHOD, 0, 0, "read_dbf",          NULL, "ss",                 0, 0},
         {_DGT_VAL,  _DCL_METHOD, 0, 0, "compare",           NULL, "aa",                 0, 0},
         {_DGT_VAL,  _DCL_METHOD, 0, 0, "sql_select_once",   NULL, "vs",                 0, 0},
         {_DGT_VAL,  _DCL_METHOD, 0, 0, "sql_dml_execute",   NULL, "s",                  0, 0},
         {_DGT_VAL,  _DCL_METHOD, 0, 0, "sql_describe",      NULL, "ss",                 0, 0},
	 {_DGT_VAL,  _DCL_METHOD, 0, 0, "MIDAS_API_new",     NULL, "ss",                 0, 0},
	 {_DGT_VAL,  _DCL_METHOD, 0, 0, "MIDAS_API_send",    NULL, "ssss",               0, 0},
	 {_CHR_AREA, _DCL_METHOD, 0, 0, "MIDAS_API_errors",  NULL, "s",                  0, 0},
	 {_DGT_VAL,  _DCL_METHOD, 0, 0, "QC_result_new",     NULL, "",                   0, 0},
	 {_DGT_VAL,  _DCL_METHOD, 0, 0, "QC_result_send",    NULL, "ssss",               0, 0},
	 {_CHR_AREA, _DCL_METHOD, 0, 0, "QC_errors",         NULL, "",                   0, 0},
	 {_DGT_VAL,  _DCL_METHOD, 0, 0, "EMAIL_new",         NULL, "",                   0, 0},
	 {_DGT_VAL,  _DCL_METHOD, 0, 0, "EMAIL_send",        NULL, "ss",                 0, 0},
	 {_DGT_VAL,  _DCL_METHOD, 0, 0, "EMAIL_errors",      NULL, "",                   0, 0},
         {_CHR_PTR,  _DCL_METHOD, 0, 0, "form_xml",          NULL, "ss",                 0, 0},
         {_DGT_VAL,  _DCL_METHOD, 0, 0, "file_xml",          NULL, "sssss",              0, 0},
         {_CHR_PTR,  _DCL_METHOD, 0, 0, "record_md5",        NULL, "ss",                 0, 0},
         {_CHR_PTR,  _DCL_METHOD, 0, 0, "all_record_md5",    NULL, "ss",                 0, 0},

	 {0, 0, 0, 0, "", NULL, NULL, 0, 0}
                                      } ;

/*------------------------------------------------ Встроенные методы */

   for(i=0 ; xobject_methods[i].name[0]!=0 ; i++)
     if( end-beg+1==strlen(xobject_methods[i].name) &&
        !memcmp(xobject_methods[i].name, beg, 
                 strlen(xobject_methods[i].name))       )  return(&xobject_methods[i]) ;

/*--------------------------------------------------- Внешние методы */

    for(n=0 ; nVars[n]!=NULL ; n++) {

       for(vars=nVars[n] ; vars->name[0]!=0 ; vars++)
         if(vars->func_flag==_DCL_METHOD) 
          if( end-beg+1==strlen(vars->name) &&
             !memcmp(vars->name, beg, strlen(vars->name)))  return(vars) ;
                                    }
/*-------------------------------------------------- Метод не найден */

        mError_code=_DCLE_TYPEDEF_METH ;

/*-------------------------------------------------------------------*/

  return(0) ;
}


/*********************************************************************/
/*                                                                   */
/*           Исполнение метода объекта комплексного типа             */

   Dcl_decl *Lang_DCL::iXobject_invoke(Dcl_decl  *object, 
                                       Dcl_decl  *method, 
                                       Dcl_decl **pars, 
                                            int   pars_cnt) 

{
 static   double  dgt_value ;          /* Буфер числового значения */
 static Dcl_decl  dgt_return={_DGT_VAL, 0,0,0,"", &dgt_value, NULL, 1, 1} ;
 static     char  chr_value[2048] ;   /* Буфер числового значения */
 static Dcl_decl  chr_return={_CHR_AREA, 0,0,0,"", chr_value, NULL, 1, 2048} ;

            Dcl_decl  decl ;
            Dcl_decl *ext_result ;
            Dcl_decl *values ;
  Dcl_complex_record *record ;
                 int  marked_only ;
                 int  cnt ;
                 int  work_nmb ;
                char  path[1024] ;    /* Путь к файлу */
                char *attr ;
                FILE *file ;
                char *words[1000] ;
                char  templ[2000] ;
                 int  auto_templ ;
                 int  field ;
                 int  type ;          /* Основной тип переменной */
                 int  status ;
                 int  size ;
                char *buff ;
                 int  buff_size ;
                char *entry ;
                char *end ;
                char  tmp[1024] ;
                 int  i ;
                 int  j ;
                 int  k ;

    Dcl_decl *(*dcl_method)(Lang_DCL *, Dcl_decl *, Dcl_decl **, int) ;

#define    _BUFF_MAX   32000
#define  _VALUES_MAX     100

/*------------------------------------------------------- Подготовка */

          marked_only =object->work_nmb & _DCL_MARKED_ONLY ;
             dgt_value=  0 ;

/*--------------------------------------------------- Внешние методы */

   if(method->addr!=NULL) {

          dcl_method=(Dcl_decl *(*)(Lang_DCL *,                     /* Уст.адрес процедуры обработки метода */
                                    Dcl_decl *, 
                                    Dcl_decl **, int))method->addr ;  

               ext_result=dcl_method(this, object, pars, pars_cnt) ;
        return(ext_result) ;

                          }
/*-------------------------------------------------- Метод ADD_FIELD */

   if(!strcmp(method->name, "add_field")) {

       if(pars_cnt     !=1   ||                                     /* Проверяем число параметров */
	  pars[0]->addr==NULL  ) {
                                    mError_code=_DCLE_PROTOTYPE ;
                                      return(&dgt_return) ; 
                                 }

                    memset(templ, 0, sizeof(templ)) ;               /* Копируем описание добавляемого поля в рабочий буфер */
        if(pars[0]->size>=sizeof(templ))
                    memcpy(templ, pars[0]->addr, sizeof(templ)-1) ;
        else        memcpy(templ, pars[0]->addr, pars[0]->size) ;

            for(i=0, j=0 ; templ[i] ; i++, j++)                     /* Ужатие пробелов и табуляций */
              if(templ[i]== ' ' ||
	         templ[i]=='\t'   )      j-- ;
              else                 templ[j]=templ[i] ;

                                   templ[j]=0 ;

               iDecl_maker(templ, &decl) ;                          /* Обрабатываем описание */
        if(mError_code) return(&dgt_return) ;                       /*  Если есть ошибки ... */

           iXobject_extend(object, &decl) ;                         /* Добавляем поле */

                                          } 
/*------------------------------------------------------- Метод SORT */

   if(!strcmp(method->name, "sort")) {

       if(pars_cnt     !=1   ||                                     /* Проверяем число параметров */
	  pars[0]->addr==NULL  ) {
                                    mError_code=_DCLE_PROTOTYPE ;
                                      return(&dgt_return) ; 
                                 }

                    memset(templ, 0, sizeof(templ)) ;               /* Копируем описание добавляемого поля в рабочий буфер */
        if(pars[0]->size>=sizeof(templ))
                    memcpy(templ, pars[0]->addr, sizeof(templ)-1) ;
        else        memcpy(templ, pars[0]->addr, pars[0]->size) ;

           iXobject_sort(object, templ) ;                           /* Сортируем записи */

                                     } 
/*------------------------------------------------- Метод TRACE_SIZE */

   if(!strcmp(method->name, "trace_size")) {

       if(pars_cnt     !=1   ||                                     /* Проверяем число параметров */
	  pars[0]->addr==NULL  ) {
                                    mError_code=_DCLE_PROTOTYPE ;
                                      return(&dgt_return) ; 
                                 }

          k=(int)iDgt_get(pars[0]->addr, pars[0]->type) ;           /* Извлекаем значение параметра */

       if(k) object->work_nmb|=_DCL_XTRACE ;                        /* Уст./снимаем признак  */
       else  object->work_nmb^=_DCL_XTRACE ;

                                           } 
/*------------------------------------------------ Метод MARKED_ONLY */

   if(!strcmp(method->name, "marked_only")) {

       if(pars_cnt     !=1   ||                                     /* Проверяем число параметров */
	  pars[0]->addr==NULL  ) {
                                    mError_code=_DCLE_PROTOTYPE ;
                                      return(&dgt_return) ; 
                                 }

          k=(int)iDgt_get(pars[0]->addr, pars[0]->type) ;           /* Извлекаем значение параметра */

       if(k) object->work_nmb|=_DCL_MARKED_ONLY ;                   /* Уст./снимаем признак MARKED_ONLY */
       else  object->work_nmb^=_DCL_MARKED_ONLY ;

                                            } 
/*------------------------------------------------------- Метод FIND */

   else
   if(!strcmp(method->name, "find")) {
                                         object->size=-1 ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - Входной контроль */
     if(pars_cnt%2) {                                               /* Число параметров должно быть четным */
                        mError_code=_DCLE_PROTOTYPE ;
                          return(&dgt_return) ; 
                    }
/*- - - - - - - - - - - - - - - - - - - - - - - Безусловная разметка */
     if(pars_cnt==0) {

       if(object->buff>0) {

          for(record=(Dcl_complex_record *)object->addr,             /* Проходим по записям */
                             i=0 ; i<object->buff ; i++, 
              record=(Dcl_complex_record *)record->next_record)
                                               record->find_mark=1 ;
                          }  
      
                              object->size= 0 ;
                                 dgt_value=object->buff ;
                          return(&dgt_return) ;
                     }
/*- - - - - - - - - - - - - - - - - - -  Проверка условий по записям */
                 cnt=0 ;

     for(record=(Dcl_complex_record *)object->addr,
                        i=0 ; i<object->buff ; i++, 
         record=(Dcl_complex_record *)record->next_record) {

          if(marked_only && !record->find_mark)  continue ;         /* Отработка режима MARKED_ONLY */ 

        for(j=0 ; j<pars_cnt ; j+=2) {

          for(k=0 ; k<record->elems_cnt ; k++)
            if(  strlen(record->elems[k].name)==pars[j]->size &&
                !memcmp(record->elems[k].name, pars[j]->addr, 
                                               pars[j]->size)   ) {
                          work_nmb=pars[j+1]->work_nmb ;
                                   pars[j+1]->work_nmb=0 ;

                    iProcessor(203, pars[j+1], &record->elems[k]) ;
                                    pars[j+1]->work_nmb=work_nmb ;

                 if(mError_code)  return(&dgt_return) ; 
                                     break ;
                                                                  }
                 if(k>=record->elems_cnt) {
                        mError_code=_DCLE_TYPEDEF_ELEM ;
                          return(&dgt_return) ; 
                                          }

                 if(!nIf_ind)  break ;
                                     }

                 if(nIf_ind) {
                                record->find_mark = 1 ;
                                              cnt++ ;
                         if(cnt==1)  object->size = i ;
                             }
                 else        {
                                record->find_mark=0 ;
                             }
                                                           }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                                         dgt_value=cnt ;
                                     }
/*--------------------------------------- Методы READ_CSV, WRITE_CSV */

   else
   if(!strcmp(method->name,  "read_csv") ||
      !strcmp(method->name, "write_csv")   ) {

                      dcl_errno=0 ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - Открытие файла */
      if(pars_cnt      <  1  ||                                     /* Если файл не задан */
         pars[0]->addr==NULL   ) {
                                            dcl_errno=ENOENT ;
                                            dgt_value=-1. ;
                                    return(&dgt_return) ;
                                 }

      if(pars[0]->size>=sizeof(path))                               /* Контроль размеров имени файла */
                          pars[0]->size=sizeof(path)-1 ;

      if(!strcmp(method->name,  "read_csv"))  attr="rb" ;           /* Определяем аттрибуты */
      else                                    attr="wb" ; 

             memset(path, 0, sizeof(path)) ;
             memcpy(path, pars[0]->addr, pars[0]->size) ;           /* Занесение имени в рабочий буфер */
         file=fopen(path, attr) ;
      if(file==NULL) {
                                            dcl_errno=errno ;
                                            dgt_value=-1. ;
                                    return(&dgt_return) ;
                     }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - Разбор формата */
      if(pars_cnt      <  2  ||                                     /* Если формат не задан */
         pars[1]->addr==NULL   )  auto_templ=1 ;
      else                        auto_templ=0 ;

      if(auto_templ==0) {

                    memset(templ, 0, sizeof(templ)) ;               /* Копируем шаблон в рабочий буфер */
        if(pars[1]->size>=sizeof(templ))
                    memcpy(templ, pars[1]->addr, sizeof(templ)-1) ;
        else        memcpy(templ, pars[1]->addr, pars[1]->size) ;

                    memset(words, 0, sizeof(words)) ;
                           words[0]=templ ;

        for(field=0, i=0, k=1 ; templ[i]!=0 ; i++)                  /* Разбираем формат на "слова" */
          if(field) {                                               /* В описании поля */
                       if(templ[i]=='%') {
                                                  field = 0 ;
                                                templ[i]= 0 ;
                                                words[k]=templ+i+1 ;
                                                      k++ ;
                                         }                     
                    }
          else      {                                               /* В текстовом фрагманте */
                       if(templ[i]=='%')                            
                        if(templ[i+1]=='%') { 
                                              strcpy(templ+i, templ+i+1) ;
                                                         i-- ;
                                            }
                        else                {
                                                  field = 1 ;
                                                templ[i]= 0 ;
                                                words[k]=templ+i+1 ;
                                                      k++ ;
                                            }
                    }

                        }
/*- - - - - - - - - - - - - - - - - - - - -  Чтение записей из файла */
    if(!strcmp(method->name,  "read_csv")) {

      if(auto_templ) {                                              /* Если шаблон считывания не задан */
                           mError_code=_DCLE_TYPEDEF_ELEM ;
                                 return(&dgt_return) ; 
                     }


         buff=(char *)calloc(1, _BUFF_MAX) ;
      if(buff==NULL) {
                        mError_code=_DCLE_MEMORY ;
                            return(&dgt_return) ; 
                     }

         values=(Dcl_decl *)calloc(_VALUES_MAX, sizeof(*values)) ;  /* Размещаем список парсинга */

            for(j=0 ; words[j]!=NULL ; j++)                         /* Заполняем имена полей */
              if(j%2==1)  strncpy(values[j/2].name, words[j], sizeof(values->name)-1) ; 

                             cnt=0 ;

                   memset(buff, 0, _BUFF_MAX) ;
       do {
                end=fgets(buff, _BUFF_MAX-1, file) ;
             if(end==NULL)  break ;
         
                end=strchr(buff, '\r') ;
             if(end!=NULL)  *end=0 ;
                end=strchr(buff, '\n') ;
             if(end!=NULL)  *end=0 ;

            for(entry=buff, j=0 ; words[j]!=NULL ; j++) {           /* LOOP - Перебираем слова шаблона */

              if(j  ==0) {                                          /* Проверяем префикс... */
                  if(*words[j]!=0)
                   if(memcmp(buff, words[j], strlen(words[j])))  break ;

                                       entry=buff+strlen(words[j]) ;
                            values[j/2].addr=       entry ;
                            values[j/2].size=strlen(entry) ;
                         }
              else
              if(words[j+1]==NULL) {                                /* Проверяем постфикс... */

                  if(*words[j]==0)  continue ;

                     entry=strstr(entry, words[j]) ;
                  if(entry==NULL)  break ;
                     
                            values[j/2-1].size=entry-(char *)values[j/2-1].addr ;
                                   }   
              else
              if(j%2==0) {                                          /* Нечетные слова - текстовые фрагменты - маркеры... */
                  
                     entry=strstr(entry, words[j]) ;
                  if(entry==NULL)  break ;

                            values[j/2-1].size=entry-(char *)values[j/2-1].addr ;
                            values[j/2  ].addr=entry+strlen(words[j]) ;
                            values[j/2  ].size=strlen((char *)values[j/2].addr) ;
                                        entry+=strlen(words[j]) ;
                         }
                                                        }           /* ENDLOOP - Перебираем слова шаблона */

             if(words[j]==NULL) {                                   /* Если строка проидена целиком */
                                   iXobject_add(object, NULL) ;
                            status=iXobject_set(object, values, j/2) ;
                         if(status) {
                                       mError_code=_DCLE_TYPEDEF_ELEM ;
                                             return(&dgt_return) ; 
                                    }
                                               cnt++ ;
                                }
          } while(1) ;

                             free(buff) ;

                                           }
/*- - - - - - - - - - - - - - - - - - - - - Занесение записей в файл */
    else                                   { 

                 cnt=0 ;

     for(record=(Dcl_complex_record *)object->addr,                 /* LOOP - Перебираем записи */
                        i=0 ; i<object->buff ; i++, 
         record=(Dcl_complex_record *)record->next_record) {

       if(marked_only && !record->find_mark)  continue ;            /* Отработка режима MARKED_ONLY */ 

                               cnt++ ;

       if(auto_templ) {

          for(k=0 ; k<record->elems_cnt ; k++) {                    /* LOOP - Перебираем элементы */

            if(k)  fwrite(";", 1, 1, file) ;                        /* Ставим разделитель */ 

               type=t_base(record->elems[k].type) ;                 /* Извлекаем тип элемента */ 
            if(type==_CHR_AREA ||                                   /* Если строчная переменная */
               type==_CHR_PTR    ) {
                if(record->elems[k].size>0)
                    fwrite(record->elems[k].addr, 1, record->elems[k].size, file) ;
                                   }
            else                   {                                /* Если числовая переменная */
                     iNumToStr(&record->elems[k], tmp) ;
                        fwrite(tmp, 1, strlen(tmp), file) ;
                                   }
                                               }                    /* ENDLOOP - Перебираем элементы */

                   fwrite("\n", 1, 1, file) ;                       /* Переводим строку */ 

                      }
       else           {

        for(j=0 ; words[j]!=NULL ; j++) {                           /* LOOP - Перебираем слова шаблона */

              if(j%2==0) {                                          /* Нечетные слова - текстовые фрагменты... */
                    fwrite(words[j], 1, strlen(words[j]), file) ;
                              continue ;
                         }

          for(k=0 ; k<record->elems_cnt ; k++)                      /* Ищем элемент по имени */
            if(!strcmp(record->elems[k].name, words[j]))  break ;

            if(k>=record->elems_cnt) {                              /* Если элемент не найден */
                                mError_code=_DCLE_TYPEDEF_ELEM ;
                                        return(&dgt_return) ; 
                                     }

               type=t_base(record->elems[k].type) ;                 /* Извлекаем тип элемента */ 
            if(type==_CHR_AREA ||                                   /* Если строчная переменная */
               type==_CHR_PTR    ) {
                if(record->elems[k].size>0)
                    fwrite(record->elems[k].addr, 1, record->elems[k].size, file) ;
                                   }
            else                   {                                /* Если числовая переменная */
                     iNumToStr(&record->elems[k], tmp) ;
                        fwrite(tmp, 1, strlen(tmp), file) ;
                                   }
                                        }                           /* ENDLOOP - Перебираем слова шаблона */
                      }
                                                           }        /* ENDLOOP - Перебираем записи */
                                           }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - Закрытие файла */
                fclose(file) ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                                         dgt_value=cnt ;

                                             }
/*--------------------------------------------------- Метод READ_DBF */

   else
   if(!strcmp(method->name,  "read_dbf")   ) {

       if(pars_cnt     < 2    ||                                    /* Проверяем число параметров */
	  pars[0]->addr==NULL ||
	  pars[1]->addr==NULL   ) {
                                    mError_code=_DCLE_PROTOTYPE ;
                                      return(&dgt_return) ; 
                                  }

           dgt_value=zDbfRead(object, pars, pars_cnt) ;

                                             }
/*--------------------------------------------------- Метод FORM_XML */
/*                                                    Метод FILE_XML */
/*  Спецификация тэгов:  <Tэг поля 1>=<Поле1>:<Преобразование>,...   */

   else
   if(!strcmp(method->name,  "form_xml") ||
      !strcmp(method->name,  "file_xml")   ) {

      char  header[2048] ;
      char  footer[2048] ;
       int  by_row ;
      char  conv[2048] ;
      char *tag ;
      char *cvt ;
      char *data;
       int  data_size ;
       int  m ;
       int  ii ;

      struct {
               char *fld ;
               char *tag ;
               char *cvt ;
             }  tags[200] ;
           int  tags_cnt ;

                      dcl_errno=  0 ;

                           buff=NULL ;
                           size=  0 ;
/*- - - - - - - - - - - - - - - - - - - - - -  Разбор кадрового тэга */
                    memset(header, 0, sizeof(header)) ;
                    memset(footer, 0, sizeof(footer)) ;

      if(pars_cnt      <  1  ||                                     /* Если кадровый тэг не задан */
         pars[0]->addr==NULL   )    ;
      else 
      if(pars[0]->size>0) {

         if(pars[0]->size<sizeof(header)-8) {
                                               header[0]='<' ;
                     if(pars[0]->size)  memcpy(header+1, pars[0]->addr, pars[0]->size) ;
                                        strcat(header, ">") ; 
                                            }
         else                               {
                                        strcpy(header, "<!Overflow!>") ;
                                            }

         if(pars[0]->size<sizeof(footer)-8) {
                                               footer[0]='<' ;
                                               footer[1]='/' ;
                     if(pars[0]->size)  memcpy(footer+2, pars[0]->addr, pars[0]->size) ;
                                        strcat(footer, ">\r\n") ; 
                                            }
         else                               {
                                        strcpy(footer, "</!Overflow!>\r\n") ;
                                            }

                          }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - Разбор формата */
                         by_row= 0 ;

      if(pars_cnt      <  2  ||                                     /* Если формат не задан */
         pars[1]->addr==NULL   )  auto_templ= 1 ;
      else                        auto_templ= 0 ;

                                  tags_cnt=0 ;

      if(auto_templ==0) do {

                    memset(templ, 0, sizeof(templ)) ;               /* Копируем шаблон в рабочий буфер */
        if(pars[1]->size>=sizeof(templ))
                    memcpy(templ, pars[1]->addr, sizeof(templ)-1) ;
        else        memcpy(templ, pars[1]->addr, pars[1]->size) ;

         if(!stricmp(templ, "TAGBYROW")) {
                                          auto_templ=1 ;
                                              by_row=1 ;
                                                break ;
                                        }  

        for(entry=templ, end=templ ; end!=NULL; entry=end+1) {      /* Разбираем формат на "тэги" */

                end=strchr(entry, ',') ;
             if(end!=NULL)  *end=0 ;

                 tags_cnt++ ;
            tags[tags_cnt-1].tag=entry ;
            tags[tags_cnt-1].fld=NULL ;
            tags[tags_cnt-1].cvt=NULL ;

               entry=strchr(entry, '=') ;
            if(entry==NULL)  continue ;

                          *entry= 0 ;
            tags[tags_cnt-1].fld=entry+1 ;

               entry=strchr(entry+1, ':') ;
            if(entry==NULL)  continue ;

                          *entry= 0 ;
            tags[tags_cnt-1].cvt=entry+1 ;
                                                             }
                           } while(0) ;
/*- - - - - - - - - - - - - - - - - - - - Формирование XML-структуры */
#define _TAG_SIZE  64000

                    buff_size=0 ;

     for(record=(Dcl_complex_record *)object->addr,                 /* LOOP - Перебираем записи */
                        i=0 ; i<object->buff ; i++, 
         record=(Dcl_complex_record *)record->next_record) {

       if(marked_only && !record->find_mark)  continue ;            /* Отработка режима MARKED_ONLY */ 

       if(header[0]!=0) {                                           /* Пишем заголовок кадра */
          if(buff_size-size<_TAG_SIZE) {                            /*  Доразмещение памяти по мере ее заполнения */
                   buff_size+= 2*_TAG_SIZE ;
                   buff      =(char *)realloc(buff, buff_size) ;
                                       }
                 memcpy(buff+size, header, strlen(header)+1) ;
                             size+=strlen(header) ;
                        }

      for(k=0 ; k<record->elems_cnt ; k++) {                        /* LOOP - Перебираем элементы */

#define  F  record->elems[k]

                                tag=  NULL ;
                                cvt=  NULL ;
        if(auto_templ) {                                            /* Определяем тэг и преобразование для поля */
                                tag=F.name ;
                                cvt=  NULL ;
                       }
        else           {

             for(m=0 ; m<tags_cnt ; m++)
               if(!stricmp(tags[m].fld, F.name)) {
                                tag=tags[m].tag ;
                                cvt=tags[m].cvt ;
                                   break ;
                                                 }
                       }

        if(tag==NULL)  continue ;                                   /* Если поле не включено в XML... */

        if(buff_size-size<_TAG_SIZE) {                              /* Доразмещение памяти по мере ее заполнения */
                     buff_size+= 2*_TAG_SIZE ;
                     buff      =(char *)realloc(buff, buff_size) ;
                                     }

              strcpy(buff+size, "<") ;  size++ ;                    /* Ставим заголовок тэга поля */ 
              strcpy(buff+size, tag) ;  size+=strlen(tag) ;
              strcpy(buff+size, ">") ;  size++ ;

           type=t_base(record->elems[k].type) ;                     /* Извлекаем тип элемента */ 
        if(type==_CHR_AREA ||                                       /* Если строчная переменная... */
           type==_CHR_PTR    ) {
                                  data     =(char *)record->elems[k].addr ;
                                  data_size=        record->elems[k].size ;
                               }
        else                   {                                    /* Если числовая переменная... */
                      iNumToStr(&record->elems[k], tmp) ;
                                  data     =       tmp ;
                                  data_size=strlen(tmp) ;
                               }

         for( ; data_size>0 ; data_size--)                          /* Проходим пробелы */
           if(data[data_size-1]!=' ')  break ;

         for( ; data_size>0 ; data++, data_size--)
           if(*data!=' ')  break ;

        if(cvt==NULL)  cvt="" ;

        if(strstr(cvt, "YMD>DMY")!=NULL) {                          /* Преобразование YYYY.MM.DD -> DD.MM.YYYY */
             if(data_size>=10) {
                      sprintf(conv, "%2.2s.%2.2s.%4.4s", data+8, data+5, data) ;
                       memcpy(data, conv, 10) ;
                               }
                                         }

        if(strstr(cvt, "NL_IGNORE")!=NULL) {                        /* Преобразование перевода строки в пробелы */
             for(ii=0 ; ii<data_size ; ii++) 
               if(data[ii]=='\r' ||
                  data[ii]=='\n'   )  data[ii]=' ' ;
                                           }

#define  KEY "<![CDATA["
        if(strstr(cvt, "CDATA")!=NULL) {                            /* Обработка спецификатора CDATA */
              memcpy(buff+size, KEY, strlen(KEY)) ;  size+=strlen(KEY) ;
                                       } 
#undef   KEY

        if(data_size>0) 
              memcpy(buff+size, data, data_size) ;  size+=data_size ;

#define  KEY "]]>"
        if(strstr(cvt, "CDATA")!=NULL) {                            /* Обработка спецификатора CDATA */
              memcpy(buff+size, KEY, strlen(KEY)) ;  size+=strlen(KEY) ;
                                       }
#undef   KEY

              strcpy(buff+size, "</") ;  size+=2 ;                  /* Ставим терминатор тэга поля */ 
              strcpy(buff+size,  tag) ;  size+=strlen(tag) ;
              strcpy(buff+size,  ">") ;  size++ ;

        if(by_row) {                                                /* Если построчный вывод */
              strcpy(buff+size,  "\n") ;  size++ ;
                   }

#undef  F
                                               }                    /* ENDLOOP - Перебираем элементы */

       if(footer[0]!=0) {                                           /* Пишем терминатор кадра */
          if(buff_size-size<_TAG_SIZE) {                            /*  Доразмещение памяти по мере ее заполнения */
                   buff_size+= 2*_TAG_SIZE ;
                   buff      =(char *)realloc(buff, buff_size) ;
                                       }
                     memcpy(buff+size, footer, strlen(footer)+1) ;
                                 size+=strlen(footer) ;
                        }
                                                           }        /* ENDLOOP - Перебираем записи */

#undef   _TAG_SIZE
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                        chr_return.addr= buff ;
                        chr_return.buff= size ;
                        chr_return.size= size ;
		return(&chr_return);
                                             }
/*-------------------------------------------------- Метод GET_FILES */

   if(!strcmp(method->name, "get_files")) {

       if(pars_cnt     !=1   ||                                     /* Проверяем число параметров */
	  pars[0]->addr==NULL  ) {
                                    mError_code=_DCLE_PROTOTYPE ;
                                      return(&dgt_return) ; 
                                 }

                    memset(templ, 0, sizeof(templ)) ;               /* Копируем путь к папке в рабочий буфер */
        if(pars[0]->size>=sizeof(templ))
                    memcpy(templ, pars[0]->addr, sizeof(templ)-1) ;
        else        memcpy(templ, pars[0]->addr, pars[0]->size) ;

           dgt_value=zGetFilesByFolder(object, templ) ;

                                          } 
/*------------------------------------------ Метод GET_FILES_BY_MASK */

   if(!strcmp(method->name, "get_files_by_mask")) {

       if(pars_cnt     !=1   ||                                     /* Проверяем число параметров */
	  pars[0]->addr==NULL  ) {
                                    mError_code=_DCLE_PROTOTYPE ;
                                      return(&dgt_return) ; 
                                 }

                    memset(templ, 0, sizeof(templ)) ;               /* Копируем маску файлов в рабочий буфер */
        if(pars[0]->size>=sizeof(templ))
                    memcpy(templ, pars[0]->addr, sizeof(templ)-1) ;
        else        memcpy(templ, pars[0]->addr, pars[0]->size) ;

           dgt_value=zGetFilesByMask(object, templ) ;

                                                  } 
/*---------------------------------------------------- Метод COMPARE */

   if(!strcmp(method->name, "compare")) {

       if(pars_cnt     <  2   ||                                    /* Проверяем число параметров */
	  pars[0]->addr==NULL ||
	  pars[1]->addr==NULL   ) {
                                    mError_code=_DCLE_PROTOTYPE ;
                                      return(&dgt_return) ; 
                                  }

                        memset(templ, 0, sizeof(templ)) ;           /* Копируем маску файлов в рабочий буфер */

       if(pars_cnt     >= 3   ||
	  pars[2]->addr==NULL   ) {
        if(pars[2]->size>=sizeof(templ))
			memcpy(templ, pars[2]->addr, sizeof(templ)-1) ;
        else            memcpy(templ, pars[2]->addr, pars[2]->size) ;
                                  }  
       else                       {
			strcpy(templ, "ANY") ;
                                  } 

              iXobject_clear(object) ;                              /* Очищаем результирующий объект */
              iXobject_diff (object, pars[0], pars[1],              /* Сравниваем наборы записей объектов */
                                                templ, NULL) ;
                                        }
/*---------------------------------------------------- Методы SQL... */
/*- - - - - - - - - - - - - - - - - - - - - -  Метод SQL_SELECT_ONCE */
   if(!strcmp(method->name, "sql_select_once")) {

       if(pars_cnt     < 2    ||                                    /* Проверяем число параметров */
	  pars[0]->addr==NULL ||
	  pars[1]->addr==NULL   ) {
                                    mError_code=_DCLE_PROTOTYPE ;
                                      return(&dgt_return) ; 
                                  }

           dgt_value=zSqlSelectOnce(object, pars, pars_cnt) ;

                                                } 
/*- - - - - - - - - - - - - - - - - - - - - -  Метод SQL_DML_EXECUTE */
   if(!strcmp(method->name, "sql_dml_execute")) {

       if(pars_cnt     < 1    ||                                    /* Проверяем число параметров */
	  pars[0]->addr==NULL   ) {
                                    mError_code=_DCLE_PROTOTYPE ;
                                      return(&dgt_return) ; 
                                  }

           dgt_value=zSqlDmlExecute(object, pars, pars_cnt) ;

                                                } 
/*- - - - - - - - - - - - - - - - - - - - - - - - Метод SQL_DESCRIBE */
   if(!strcmp(method->name, "sql_describe")) {

       if(pars_cnt     < 2    ||                                    /* Проверяем число параметров */
	  pars[0]->addr==NULL ||
	  pars[1]->addr==NULL   ) {
                                    mError_code=_DCLE_PROTOTYPE ;
                                      return(&dgt_return) ; 
                                  }

           dgt_value=zSqlDescribe(object, pars, pars_cnt) ;

                                             } 
/*---------------------------------------------- Методы MIDAS_API... */
/*- - - - - - - - - - - - - - - - -  - - - - - - Метод MIDAS_API_NEW */
   if(!strcmp(method->name, "MIDAS_API_new")) {

       if(pars_cnt     < 2    ||                                    /* Проверяем число параметров */
	  pars[0]->addr==NULL ||
	  pars[1]->addr==NULL   ) {
                                    mError_code=_DCLE_PROTOTYPE ;
                                      return(&dgt_return) ; 
                                  }

           dgt_value=zMidasApiNew(object, pars, pars_cnt) ;

                                              } 
/*- - - - - - - - - - - - - - - - -  - - - - -  Метод MIDAS_API_SEND */
   if(!strcmp(method->name, "MIDAS_API_send")) {

       if(pars_cnt     < 4    ||                                    /* Проверяем число параметров */
	  pars[0]->addr==NULL ||
	  pars[1]->addr==NULL ||
	  pars[2]->addr==NULL ||
	  pars[3]->addr==NULL   ) {
                                    mError_code=_DCLE_PROTOTYPE ;
                                      return(&dgt_return) ; 
                                  }

           dgt_value=zMidasApiSend(object, pars, pars_cnt) ;

                                               } 
/*- - - - - - - - - - - - - - - - -  - - - -  Метод MIDAS_API_ERRORS */
   if(!strcmp(method->name, "MIDAS_API_errors")) {

       if(pars_cnt     < 1    ||                                    /* Проверяем число параметров */
	  pars[0]->addr==NULL   ) {
                                    mError_code=_DCLE_PROTOTYPE ;
                                      return(NULL) ; 
                                  }

              zMidasApiErrors(object, pars, pars_cnt, 
                                       chr_value, sizeof(chr_value)) ;

                         chr_return.size=strlen((char *)chr_return.addr) ;
                 return(&chr_return) ; 
                                                 } 
/*----------------------------------------------------- Методы QC... */
/*- - - - - - - - - - - - - - - - - - - - - - -  Метод QC_RESULT_NEW */
   if(!strcmp(method->name, "QC_result_new")) {

       if(pars_cnt>0) {
                                    mError_code=_DCLE_PROTOTYPE ;
                                      return(&dgt_return) ; 
                      }

           dgt_value=zQC_ResultNew(object, pars, pars_cnt) ;

                                              } 
/*- - - - - - - - - - - - - - - - - - - - - - - Метод QC_RESULT_SEND */
   if(!strcmp(method->name, "QC_result_send")) {

       if(pars_cnt     < 4    ||                                    /* Проверяем число параметров */
	  pars[0]->addr==NULL ||
	  pars[1]->addr==NULL ||
	  pars[2]->addr==NULL ||
	  pars[3]->addr==NULL   ) {
                                    mError_code=_DCLE_PROTOTYPE ;
                                      return(&dgt_return) ; 
                                  }

           dgt_value=zQC_ResultSend(object, pars, pars_cnt) ;

                                               } 
/*- - - - - - - - - - - - - - - - - - - - - - - - -  Метод QC_ERRORS */
   if(!strcmp(method->name, "QC_errors")) {

       if(pars_cnt>0) {
                         mError_code=_DCLE_PROTOTYPE ;
                             return(NULL) ; 
                      }

              zQC_Errors(object, pars, pars_cnt, 
                                       chr_value, sizeof(chr_value)) ;

                         chr_return.size=strlen((char *)chr_return.addr) ;
                 return(&chr_return) ; 
                                          } 
/*-------------------------------------------------- Методы EMAIL... */
/*- - - - - - - - - - - - - - - - - - - - - - - - -  Метод EMAIL_NEW */
   if(!strcmp(method->name, "EMAIL_new")) {

       if(pars_cnt>0) {
                                    mError_code=_DCLE_PROTOTYPE ;
                                      return(&dgt_return) ; 
                      }

           dgt_value=zEMailNew(object, pars, pars_cnt) ;

                                          } 
/*- - - - - - - - - - - - - - - - - - - - - - - - - Метод EMAIL_SEND */
   if(!strcmp(method->name, "EMAIL_send")) {

       if(pars_cnt<2) {
                                    mError_code=_DCLE_PROTOTYPE ;
                                      return(&dgt_return) ; 
                      }

           dgt_value=zEMailSend(object, pars, pars_cnt) ;

                                           }
/*- - - - - - - - - - - - - - - - - - - - - - - - Метод EMAIL_ERRORS */
   if(!strcmp(method->name, "EMAIL_errors")) {

       if(pars_cnt>0) {
                         mError_code=_DCLE_PROTOTYPE ;
                             return(NULL) ; 
                      }

              zEMailErrors(object, pars, pars_cnt, 
                                        chr_value, sizeof(chr_value)) ;

                         chr_return.size=strlen((char *)chr_return.addr) ;
                 return(&chr_return) ; 
                                             } 
/*--------------------------------------------------- Методы MD5... */
/*- - - - - - - - - - - - - - - - - - - - - - - -  Метод RECORD_MD5 */
	if(!strcmp(method->name, "record_md5"))
	{
	/* Объявление локальных переменных */
		md5_processor proc;     /* служебная структура для вычисления дайджеста */
		int size_digest;
		unsigned char *digest;          /* итоговый дайджест */
		unsigned char *digest_format;   /* дайджест, форматированный в соответствии со значением переменной flag */
		unsigned int size_input;
		unsigned char *input;   /* буфер для хэшируемых данных */
		int flag;               /* способ форматирования дайджеста */
                int index;              /* индекс записи */

	/*Проверка входных параметров и инициализация локальных переменных */
                if ( pars_cnt == 1 )
                {
			flag = 0;
                        index = atoi((char *)(pars[0]->addr));  
                }
		else
                if ( pars_cnt == 2 )
                {
			flag = atoi((char *)(pars[0]->addr));
                        flag /= 10;                                
                        index = atoi((char *)(pars[1]->addr));  
                }
                else
                {
                        mError_code=_DCLE_PROTOTYPE ;
                        return(&dgt_return) ;
                }
                if ( flag!=16 && flag!=2 && flag!=0 )
                {
                    printf("\nInvalid input parametr value!\n");
                    return(&dgt_return) ;
                }
              
		memset( &proc, 0, sizeof( md5_processor ) ) ;
		md5_initialization( &proc ) ;
	
		size_digest = 17;
		digest = new unsigned char [ size_digest ];
		memset( digest, 0, sizeof(digest) ) ;

		digest_format = new unsigned char [_MD5_BUFF_SIZE];
	
		size_input = 512 ;
		input = new unsigned char [ size_input ] ;

	/* Вычисление хэша текущей записи */		
		for( record = (Dcl_complex_record *)object->addr, i = index ; i > 0 ; i-- )
                {
                    if( record->next_record == NULL ) 
                    {
                        printf("\nIndex is not correct!\n");
                        return(&dgt_return) ;
                    }
                    record = (Dcl_complex_record *)record->next_record;
                }

		for( k=0 ; k < record->elems_cnt ; k++ )			
		{                  			
//		    itoa( record->elems[k].type, (char *)input, 10);
                    sprintf((char *)input, "%u", record->elems[k].type) ;
                    md5_start( &proc, input, strlen((char *)input) );	// хеширование поля "type"

//                  itoa( record->elems[k].func_flag, (char *)input, 10);
                    sprintf((char *)input, "%u", record->elems[k].func_flag) ;
                    md5_start( &proc, input, strlen((char *)input) );	// хеширование поля "func_flag"

//                  itoa( record->elems[k].work_nmb, (char *)input, 10);
                    sprintf((char *)input, "%u", record->elems[k].work_nmb) ;
                    md5_start( &proc, input, strlen((char *)input) );	// хеширование поля "work_nmb"

//                  itoa( record->elems[k].local_idx, (char *)input, 10);
                    sprintf((char *)input, "%d", record->elems[k].local_idx) ;
                    md5_start( &proc, input, strlen((char *)input) );	// хеширование поля "local_idx"

                    memcpy( input, record->elems[k].name, strlen(record->elems[k].name) );
		    md5_start( &proc, input, strlen(record->elems[k].name) );	// хеширование поля "name"

                    memcpy( input, record->elems[k].addr, record->elems[k].size );
		    md5_start( &proc, input, record->elems[k].size );	// хеширование поля "addr"

                    memcpy( input, record->elems[k].prototype, strlen(record->elems[k].prototype) );
		    md5_start( &proc, input, strlen(record->elems[k].prototype) );	// хеширование поля "prototype"

//                  itoa( record->elems[k].size, (char *)input, 10);
                    sprintf((char *)input, "%d", record->elems[k].size) ;
                    md5_start( &proc, input, strlen((char *)input) );	// хеширование поля "size"

//                  itoa( record->elems[k].buff, (char *)input, 10);
                    sprintf((char *)input, "%d", record->elems[k].buff) ;
                    md5_start( &proc, input, strlen((char *)input) );	// хеширование поля "buff"
		}

		md5_finish( &proc, digest );
		digest[ size_digest - 1 ] = '\0';

		md5_format( digest, digest_format, flag );		
                
		memset( &proc, 0, sizeof( md5_processor ) );

		chr_return.addr = digest_format;
		chr_return.buff = ( strlen((char *)digest_format) + 1 );
		chr_return.size = ( strlen((char *)digest_format) + 1 );
	
		return(&chr_return);
	}
/*- - - - - - - - - - - - - - - - - - - - - - - - -  Метод ALL_RECORD_MD5 */
	if( !strcmp(method->name, "all_record_md5") )
	{
	/* Объявление локальных переменных */
		md5_processor proc;
		int size_digest;
		unsigned char *digest;
		unsigned char *digest_format;
		unsigned int size_input;
		unsigned char *input;
		int flag;

	/* Проверяем входные параметры */
		if( pars_cnt > 1 ) 
		{
			mError_code=_DCLE_PROTOTYPE ;
            return(&dgt_return) ; 
        }

	/* Инициализация локальных переменных */
		memset( &proc, 0, sizeof( md5_processor ) ) ;
		md5_initialization( &proc ) ;
	
		size_digest = 17;
		digest = new unsigned char [ size_digest ];
		memset( digest, 0, sizeof(digest) ) ;

		digest_format = new unsigned char [_MD5_BUFF_SIZE];
	
		size_input = 512 ;
		input = new unsigned char [ size_input ] ;
	
		if ( pars[0]->addr==NULL )
			flag = 0;
		else
			flag = atoi((char *)(pars[0]->addr));
                if ( flag!=16 && flag!=2 && flag!=0 )
                {
                    mError_code=_DCLE_PROTOTYPE ;
                    return(&dgt_return) ;
                }

	/* Подсчитываем количество записей */
		cnt=0 ;
		for( record = (Dcl_complex_record *)object->addr, i=0 ; i < object->buff ; i++, record = (Dcl_complex_record *)record->next_record ) 
		{
			if( marked_only )  continue ;            // Отработка режима MARKED_ONLY  
                        cnt++ ;
		}	

	/* Вычисление хэша всех записей */		
		for( record = (Dcl_complex_record *)object->addr, i=0 ; i < object->buff ; i++, record = (Dcl_complex_record *)record->next_record )
		{
			for( k=0 ; k < record->elems_cnt ; k++ )			
			{			
//				itoa( record->elems[k].type, (char *)input, 10);
                                sprintf((char *)input, "%u", record->elems[k].type) ;
                                md5_start( &proc, input, strlen((char *)input) );	// хеширование поля "type"

//                              itoa( record->elems[k].func_flag, (char *)input, 10);
                                sprintf((char *)input, "%u", record->elems[k].func_flag) ;
                                md5_start( &proc, input, strlen((char *)input) );	// хеширование поля "func_flag"

//                              itoa( record->elems[k].work_nmb, (char *)input, 10);
                                sprintf((char *)input, "%u", record->elems[k].work_nmb) ;
                                md5_start( &proc, input, strlen((char *)input) );	// хеширование поля "work_nmb"

//                              itoa( record->elems[k].local_idx, (char *)input, 10);
                                sprintf((char *)input, "%d", record->elems[k].local_idx) ;
                                md5_start( &proc, input, strlen((char *)input) );	// хеширование поля "local_idx"

                                memcpy( input, record->elems[k].name, strlen(record->elems[k].name) );
				md5_start( &proc, input, strlen(record->elems[k].name) );	// хеширование поля "name"

                                memcpy( input, record->elems[k].addr, record->elems[k].size );
				md5_start( &proc, input, record->elems[k].size );	// хеширование поля "addr"

                                memcpy( input, record->elems[k].prototype, strlen(record->elems[k].prototype) );
				md5_start( &proc, input, strlen(record->elems[k].prototype) );	// хеширование поля "prototype"

//                              itoa( record->elems[k].size, (char *)input, 10);
                                sprintf((char *)input, "%d", record->elems[k].size) ;
                                md5_start( &proc, input, strlen((char *)input) );	// хеширование поля "size"

//                              itoa( record->elems[k].buff, (char *)input, 10);
                                sprintf((char *)input, "%d", record->elems[k].buff) ;
                                md5_start( &proc, input, strlen((char *)input) );	// хеширование поля "buff"
			}
		}
		md5_finish( &proc, digest );
		digest[ size_digest - 1 ] = '\0';

		md5_format( digest, digest_format, flag );		

		memset( &proc, 0, sizeof( md5_processor ) );

		chr_return.addr = digest_format ;
		chr_return.buff = ( strlen((char *)digest_format) + 1 ) ;
		chr_return.size = ( strlen((char *)digest_format) + 1 ) ;
	
		return(&chr_return);
	}

#undef    _BUFF_MAX
#undef  _VALUES_MAX

/*-------------------------------------------------------------------*/

  return(&dgt_return) ;
}


