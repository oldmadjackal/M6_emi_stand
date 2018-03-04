/*********************************************************************/
/*                                                                   */
/*                   DATA CONVERTION LANGUAGE                        */
/*                                                                   */
/*                  УТИЛИТА РАБОТЫ СО СТРОКАМИ                       */
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
#include  <stdio.h>
#include  <string.h>
#include  <ctype.h>
#include  <math.h>
#include  <memory.h>
#include  <sys/types.h>
#include  <sys/stat.h>

#ifdef UNIX
#include  <unistd.h>
#include  <dirent.h>
#define   stricmp   strcasecmp
#define  _snprintf  snprintf
#define   O_BINARY    0
#else
#include  <io.h>
#include  <direct.h>
#include  <windows.h>
#endif

#define   DCL_INSIDE
#include "dcl.h"


#pragma warning(disable : 4996)

/*---------------------------------------------------- Внешние связи */

          extern double  dcl_errno ;       /* Системный указатель ошибок -> DCL_SLIB.CPP */

/*********************************************************************/
/*                                                                   */
/*                    Разбор текста по шаблону                       */
/*                                                                   */
/*   Опции:                                                          */
/*      FIELD:      - указывает два символа - начала и конца         */
/*                     описателя поля (по-умолчанию - {} )           */
/*      ERROR_RAISE - указывает на эскалацию ошибок на уровень       */
/*                     интерпритатора                                */
/*                                                                   */
/*   Описатели полей шаблона:                                        */
/*     {Name}    - по позиционным границам                           */
/*     {Name:20} - по длине                                          */
/*                                                                   */
/*   Return: >=0 - число заполненных полей                           */
/*            -1 - ошибка обработки                                  */
/*            -2 - текст не соответствует шаблону                    */

   int  Lang_DCL::zParseByTemplate(    char *text,     int  text_size,
                                       char *format,   int  format_size,
                                       char *options,  int  options_size,
                                   Dcl_decl *fields,   int  fields_max   )

{
   char  field_b ;       /* Ограничители описателя поля */
   char  field_e ;
    int  error_raise ;
    int  fields_cnt ;
    int  field_flag ;
    int  field_size ;
    int  is_digit ;      /* Признак цифрового поля */
    int  is_VALID ;      /* Признак корректного символа */
    int  error_code ;
   char *entry ;
   char *end ;
   char  tmp[1024] ;
    int  i ;
    int  k ;

/*---------------------------------------------------- Инициализация */

                  field_b='{' ;
                  field_e='}' ;

              error_raise= 0 ;

              error_code = 0 ;
                        
/*----------------------------------------------------- Разбор опций */

          memset(tmp, 0, sizeof(tmp)) ;
         strncpy(tmp, options, sizeof(tmp)-1) ;

         entry=strstr(tmp, "FIELD:") ;                              /* FIELD: */
      if(entry!=NULL) {  field_b=entry[strlen("FIELD:")+0] ;
                         field_e=entry[strlen("FIELD:")+1] ;  }

         entry=strstr(tmp, "ERROR_RAISE") ;                         /* ERROR_RAISE */
      if(entry!=NULL)  error_raise=1 ;

/*---------------------------------------------- Сканирование текста */

#define  F   fields[fields_cnt-1]

                     is_digit=0 ;
                  field_flag =0 ;
                  field_size =0 ;
                 fields_cnt  =0 ;

     for(i=0, k=0 ; i<text_size && k<format_size ; i++, k++) {      /* LOOP - Синхронно сканируем данные и шаблон */
/*- - - - - - - - - - - - - - - - - - - - -  Проверка состава данных */
                                is_VALID=1 ;
       if(is_digit)                                                 /* Проверка на цифру */
         if(!isdigit(text[i]))  is_VALID=0 ;
/*- - - - - - - - - - - - - - - - - - - - -  Занесение данных в поле */
       if(field_flag) {

         if(field_size) {                                           /* Если поле фиксированной длины... */

             if(!is_VALID)  break ;                                 /* Если символ не соотв.дополнительным условиям... */

                              F.size++ ;
                            field_size-- ;

                  if(field_size==0)  field_flag=0 ;                 /* Если нужное число данных отобрано - "закрываем" поле */ 
                  else                  k-- ;                       /*  иначе - "держим" "сканер" шаблона */

                        }
         else           {                                           /* Если поле неопределенной длины... */

           if(k==format_size-1) {                                   /* Если это "хвостовое" поле... */
                                    F.size=text_size-i ;
                                         i=text_size-1 ;
                                }
           else                 {
                                   error_code=_DCLE_CALL_INSIDE ; 
                                strncpy(mError_details, "Unsupported use of free-size field", sizeof(mError_details)-1) ;
                                       break ; 
                                }
                        }

                      }
/*- - - - - - - - - - - - - - - - - - - - - - - - - Обнаружение поля */
       else
       if(format[k]==field_b) {                                     /* IF - Если в шаблоне найдена спецификация поля */

                                      is_digit=0 ;

             entry=(char *)memchr(format+k+1, field_e,              /* Ищем конец спецификации поля */ 
                                                format_size-i) ; 
          if(entry==NULL) {    error_code=_DCLE_CALL_INSIDE ;       /* Если не нашли... */
                             strncpy(mError_details, "Unclosed field's specification", sizeof(mError_details)-1) ;
                                       break ;  }
         
                   memset(tmp, 0, sizeof(tmp)) ;                    /* Извлекаем спецификацию поля */
                   memcpy(tmp, format+k+1, entry-format-k-1) ;
                                 k+=strlen(tmp)+1 ;                 /* Сдвигаем "сканер" шаблона */ 

          if(*tmp==0) {    error_code=_DCLE_CALL_INSIDE ;           /* Если спецификация пустая */
                         strncpy(mError_details, "Empty field's specification", sizeof(mError_details)-1) ;
                                    break ;  }

             entry=strchr(tmp, ':') ;                               /* Ищем разделитель имя/описание */
          if(entry==NULL) {                                         /* Если нет... */
                              field_size=0 ; 
                          }
          else            {                                         /* Если да -  извлекаем описание */
                                     *entry=0 ;                     /* Отсекаем имя */  
                       field_size=strtoul(entry+1, &end, 10) ;      /* Извлекаем длину поля */

                if(*end!=0) {    error_code=_DCLE_CALL_INSIDE ;
                              strncpy(mError_details, "Bad field's specification", sizeof(mError_details)-1) ;
                                     break ;   }
                          }

          if(*tmp==0) {    error_code=_DCLE_CALL_INSIDE ;           /* Если пустое имя */
                         strncpy(mError_details, "Empty field's name", sizeof(mError_details)-1) ;
                                    break ;  }

          if(fields_cnt==fields_max) {                              /* Проверка переполнения списка полей */
                                        error_code=_DCLE_CALL_INSIDE ;
                     strncpy(mError_details, "Too many fields in template", sizeof(mError_details)-1) ;
                                             break ;
                                     }

                 memset(&fields[fields_cnt], 0, sizeof(*fields)) ;  /* Регистрируем поле */
                strncpy( fields[fields_cnt].name, tmp, sizeof(fields->name)-1) ;
                         fields[fields_cnt].addr=text+i ;
                         fields[fields_cnt].size= 0 ;
                                fields_cnt++ ;

                               field_flag=1 ;                       /* Пошли данные поля */
                                  i-- ;                             /* Отшагиваем сканер данных */
                                  k-- ;                             /* Отшагиваем сканер шаблона */
                              }                                     /* END IF */
/*- - - - - - - - - - - - - - - - - - - Проверка фиксированной части */
       else
       if(text[i]!=format[k])  break ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                                                             }      /* END LOOP */

#undef  F

/*------------------------------------------------ Анализ результата */

   if(error_code) {                                                 /* Обработка ошибок */
       if(error_raise)  this->mError_code=error_code ;
                                  return(-1) ;
                  }

          if(i<text_size || k<format_size)  return(-2) ;             /* Если текст не соответствует шаблону */

/*-------------------------------------------------------------------*/

  return(fields_cnt) ;
}


/*********************************************************************/
/*                                                                   */
/*                  Формирование текста по шаблону                   */
/*                                                                   */
/*   Опции:                                                          */
/*      FIELD:      - указывает два символа - начала и конца         */
/*                     описателя поля (по-умолчанию - {} )           */
/*      ERROR_RAISE - указывает на эскалацию ошибок на уровень       */
/*                     интерпритатора                                */
/*                                                                   */
/*   Описатели полей шаблона:                                        */
/*     {Name}    - по позиционным границам                           */

   int  Lang_DCL::zFormByTemplate(    char *text,     int  text_buff,
                                      char *format,   int  format_size,
                                      char *options,  int  options_size,
                                  Dcl_decl *fields,   int  fields_cnt   )

{
   char  field_b ;       /* Ограничители описателя поля */
   char  field_e ;
    int  error_raise ;
    int  error_code ;
    int  field_size ;
   char *entry ;
   char *end ;
   char  tmp[1024] ;
    int  size ;
    int  i ;
    int  j ;
    int  k ;

/*---------------------------------------------------- Инициализация */

                  field_b='{' ;
                  field_e='}' ;

              error_raise= 0 ;

              error_code = 0 ;
                        
/*----------------------------------------------------- Разбор опций */

          memset(tmp, 0, sizeof(tmp)) ;
         strncpy(tmp, options, sizeof(tmp)-1) ;

         entry=strstr(tmp, "FIELD:") ;                              /* FIELD: */
      if(entry!=NULL) {  field_b=entry[strlen("FIELD:")+0] ;
                         field_e=entry[strlen("FIELD:")+1] ;  }

         entry=strstr(tmp, "ERROR_RAISE") ;                         /* ERROR_RAISE */
      if(entry!=NULL)  error_raise=1 ;

/*---------------------------------------------- Сканирование текста */

     for(i=0, k=0 ; i<text_buff && k<format_size ; i++, k++) {      /* LOOP - Идем по шаблону */
/*- - - - - - - - - - - - - - - - - - - - - - - - - Обнаружение поля */
       if(format[k]==field_b) {                                     /* IF - Если в шаблоне найдена спецификация поля */

             entry=(char *)memchr(format+k+1, field_e,              /* Ищем конец спецификации поля */ 
                                                format_size-i) ; 
          if(entry==NULL) {    error_code=_DCLE_CALL_INSIDE ;       /* Если не нашли... */
                             strncpy(mError_details, "Unclosed field's specification", sizeof(mError_details)-1) ;
                                       break ;  }
         
                   memset(tmp, 0, sizeof(tmp)) ;                    /* Извлекаем спецификацию поля */
                   memcpy(tmp, format+k+1, entry-format-k-1) ;
                                 k+=strlen(tmp)+1 ;                 /* Сдвигаем "сканер" шаблона */ 

          if(*tmp==0) {    error_code=_DCLE_CALL_INSIDE ;           /* Если спецификация пустая */
                         strncpy(mError_details, "Empty field's specification", sizeof(mError_details)-1) ;
                                    break ;  }

             entry=strchr(tmp, ':') ;                               /* Ищем разделитель имя/описание */
          if(entry==NULL) {                                         /* Если нет... */
                              field_size=0 ; 
                          }
          else            {                                         /* Если да -  извлекаем описание */
                                     *entry=0 ;                     /* Отсекаем имя */  
                       field_size=strtoul(entry+1, &end, 10) ;      /* Извлекаем длину поля */

                if(*end!=0) {    error_code=_DCLE_CALL_INSIDE ;
                              strncpy(mError_details, "Bad field's specification", sizeof(mError_details)-1) ;
                                     break ;   }
                          }

          if(*tmp==0) {    error_code=_DCLE_CALL_INSIDE ;           /* Если пустое имя */
                         strncpy(mError_details, "Empty field's name", sizeof(mError_details)-1) ;
                                    break ;  }

              for(j=0 ; j<fields_cnt ; j++)                         /* Ищем поле в списке */
                if(!strcmp(fields[j].name, tmp))  break ;

                if(j>=fields_cnt) {                                 /* Если поле не найдено... */
                                        error_code=_DCLE_CALL_INSIDE ;
                     _snprintf(mError_details, sizeof(mError_details)-1, "Unknown field: %s", tmp) ;
                                             break ;
                                  }

                                      size=fields[j].size ;         /* Определяем размер вставки */ 
                if(i+size>text_buff)  size=text_buff-i ;

                if(size!=0)  memmove(text+i, fields[j].addr, size); /* Вставляем блок данных из поля */

                                  i+=size-1 ;                       /* Прошагиваем вставленный блок */
                              }                                     /* END IF */
/*- - - - - - - - - - - - - - - - - - -  Пропись фиксированной части */
       else                   {
                                 text[i]=format[k] ;
                              }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                                                             }      /* END LOOP */

/*------------------------------------------------ Анализ результата */

   if(error_code) {                                                 /* Обработка ошибок */
       if(error_raise)  this->mError_code=error_code ;
                                  return(-1) ;
                  }
/*-------------------------------------------------------------------*/

  return(i) ;
}


/*********************************************************************/
/*                                                                   */
/*                  Преобразование кодовой страницы                  */

   int  Lang_DCL::zCodePageConvert(char *text, int  size, char *CP_in, char *CP_out)

{
  static  char *buff ;
  static   int  buff_size ;

/*------------------------------------------------------- Подготовка */

    if(size>buff_size) {

          buff=(char *)realloc(buff, size*2+8) ;
       if(buff==NULL) {
                          this->mError_code=_DCLE_CALL_INSIDE ;
                  strncpy(this->mError_details, "Memory over for temporary buffer", sizeof(this->mError_details)-1) ;
                                          return(-1) ;
                      }
                            buff_size=size ;
                       }
/*------------------------------------- Преобразование CP866->CP1251 */

    if(!stricmp(CP_in,  "CP866" ) &&
       !stricmp(CP_out, "CP1251")   ) {

#ifdef  UNIX
			   zDosToWin(text, size) ;
#else
                    OemToCharBuff(text, text, size) ;
#endif
                                            return(size) ;
                                      }  
/*------------------------------------- Преобразование CP1251->CP866 */

    if(!stricmp(CP_in,  "CP1251") &&
       !stricmp(CP_out, "CP866" )   ) {

#ifdef  UNIX
			   zWinToDos(text, size) ;
#else
                    CharToOemBuff(text, text, size) ;
#endif
                                            return(size) ;
                                      }  
/*-------------------------------------- Преобразование UTF8->CP1251 */

    if(!stricmp(CP_in,  "UTF-8" ) &&
       !stricmp(CP_out, "CP1251")   ) {

#ifdef  UNIX
                    this->mError_code=_DCLE_CALL_INSIDE ;
            strncpy(this->mError_details, "Unsupported conversion", sizeof(this->mError_details)-1) ;
                                          return(-1) ;
#else
                    size=zUTF8ToWin(text, size, buff) ;
#endif
                                            return(size) ;
                                      } 
/*-------------------------------------- Преобразование CP1251->UTF8 */

    if(!stricmp(CP_in,  "CP1251") &&
       !stricmp(CP_out, "UTF-8" )   ) {

#ifdef  UNIX
                    this->mError_code=_DCLE_CALL_INSIDE ;
            strncpy(this->mError_details, "Unsupported conversion", sizeof(this->mError_details)-1) ;
                                          return(-1) ;
#else
                    size=zWinToUTF8(text, size, buff) ;
#endif
                                            return(size) ;
                                      }  
/*--------------------------------------- Преобразование UTF8->CP866 */

    if(!stricmp(CP_in,  "UTF-8" ) &&
       !stricmp(CP_out, "CP866" )   ) {

#ifdef  UNIX
                    this->mError_code=_DCLE_CALL_INSIDE ;
            strncpy(this->mError_details, "Unsupported conversion", sizeof(this->mError_details)-1) ;
                                          return(-1) ;
#else
                  size=zUTF8ToWin(text, size, buff) ;
                    CharToOemBuff(text, text, size) ;
#endif
                                            return(size) ;
                                      }  
/*--------------------------------------- Преобразование CP866->UTF8 */

    if(!stricmp(CP_in,  "CP866" ) &&
       !stricmp(CP_out, "UTF-8" )   ) {

#ifdef  UNIX
                    this->mError_code=_DCLE_CALL_INSIDE ;
            strncpy(this->mError_details, "Unsupported conversion", sizeof(this->mError_details)-1) ;
                                          return(-1) ;
#else
                    OemToCharBuff(text, text, size) ;
                  size=zWinToUTF8(text, size, buff) ;
#endif
                                            return(size) ;
                                      }  
/*----------------------------------------------------------- Прочее */

                    this->mError_code=_DCLE_CALL_INSIDE ;
            strncpy(this->mError_details, "Unsupported conversion", sizeof(this->mError_details)-1) ;
                                          return(-1) ;

/*-------------------------------------------------------------------*/

  return(0) ;
}


/********************************************************************/
/*                                                                  */
/*                 Перекодировка DOS->WINDOWS                       */
/*                 Перекодировка WINDOWS->DOS                       */


   void  Lang_DCL::zDosToWin(char *text, long  size)
{

   static int  dos2win_table[256]={

		 0x000, 0x001, 0x002, 0x003, 0x004, 0x005, 0x006, 0x007,
		 0x008, 0x009, 0x00a, 0x00b, 0x00c, 0x00d, 0x00e, 0x00f,
		                                                     
		 0x010, 0x011, 0x012, 0x013, 0x014, 0x015, 0x016, 0x017,
		 0x018, 0x019, 0x01a, 0x01b, 0x01c, 0x01d, 0x01e, 0x01f,
		                                                     
		 0x020, 0x021, 0x022, 0x023, 0x024, 0x025, 0x026, 0x027,
		 0x028, 0x029, 0x02a, 0x02b, 0x02c, 0x02d, 0x02e, 0x02f,
		                                                     
		 0x030, 0x031, 0x032, 0x033, 0x034, 0x035, 0x036, 0x037,
		 0x038, 0x039, 0x03a, 0x03b, 0x03c, 0x03d, 0x03e, 0x03f,
		                                                     
		 0x040, 0x041, 0x042, 0x043, 0x044, 0x045, 0x046, 0x047,
		 0x048, 0x049, 0x04a, 0x04b, 0x04c, 0x04d, 0x04e, 0x04f,
		                                                     
		 0x050, 0x051, 0x052, 0x053, 0x054, 0x055, 0x056, 0x057,
		 0x058, 0x059, 0x05a, 0x05b, 0x05c, 0x05d, 0x05e, 0x05f,
		                                                     
		 0x060, 0x061, 0x062, 0x063, 0x064, 0x065, 0x066, 0x067,
		 0x068, 0x069, 0x06a, 0x06b, 0x06c, 0x06d, 0x06e, 0x06f,
		                                                     
		 0x070, 0x071, 0x072, 0x073, 0x074, 0x075, 0x076, 0x077,
		 0x078, 0x079, 0x07a, 0x07b, 0x07c, 0x07d, 0x07e, 0x07f,
		                                                     
		 0x0c0, 0x0c1, 0x0c2, 0x0c3, 0x0c4, 0x0c5, 0x0c6, 0x0c7,  // АБВГДЕЖЗ
		 0x0c8, 0x0c9, 0x0ca, 0x0cb, 0x0cc, 0x0cd, 0x0ce, 0x0cf,  // ИЙКЛМНОП
		                                                     
		 0x0d0, 0x0d1, 0x0d2, 0x0d3, 0x0d4, 0x0d5, 0x0d6, 0x0d7,  // РСТУФХЦЧ
		 0x0d8, 0x0d9, 0x0da, 0x0db, 0x0dc, 0x0dd, 0x0de, 0x0df,  // ШЩЪЫЬЭЮЯ
		                                                     
		 0x0e0, 0x0e1, 0x0e2, 0x0e3, 0x0e4, 0x0e5, 0x0e6, 0x0e7,  // абвгдежз
		 0x0e8, 0x0e9, 0x0ea, 0x0eb, 0x0ec, 0x0ed, 0x0ee, 0x0ef,  // ийклмноп
		                                                     
		 0x0b0, 0x0b1, 0x0b2, 0x0b3, 0x0b4, 0x0b5, 0x0b6, 0x0b7,  //
		 0x0b8, 0x0b9, 0x0ba, 0x0bb, 0x0bc, 0x0bd, 0x0be, 0x0bf,  //
		                                                     
		 0x0c0, 0x0c1, 0x0c2, 0x0c3, 0x0c4, 0x0c5, 0x0c6, 0x0c7,  //
		 0x0c8, 0x0c9, 0x0ca, 0x0cb, 0x0cc, 0x0cd, 0x0ce, 0x0cf,  //
		                                                     
		 0x0d0, 0x0d1, 0x0d2, 0x0d3, 0x0d4, 0x0d5, 0x0d6, 0x0d7,  //
		 0x0d8, 0x0d9, 0x0da, 0x0db, 0x0dc, 0x0dd, 0x0de, 0x0df,  //
		                                                     
		 0x0f0, 0x0f1, 0x0f2, 0x0f3, 0x0f4, 0x0f5, 0x0f6, 0x0f7,  //рстуфхцч
		 0x0f8, 0x0f9, 0x0fa, 0x0fb, 0x0fc, 0x0fd, 0x0fe, 0x0ff,  //шщъыьэюя
		                                                     
		 0x0f0, 0x0f1, 0x0f2, 0x0f3, 0x0f4, 0x0f5, 0x0f6, 0x0f7,  //
		 0x0f8, 0x0f9, 0x0fa, 0x0fb, 0x0fc, 0x0fd, 0x0fe, 0x0ff   //
                                  } ;

  unsigned char *text_ ;
           long  i ;
            int  n ;


   for(text_=(unsigned char *)text, i=0 ; i<size ; i++) {
                                         n = text_[i] & 0x0ff ;
                                   text_[i]=dos2win_table[n] ;
                                                        }
}


   void  Lang_DCL::zWinToDos(char *text, long  size)
{

   static int  win2dos_table[256]={
		 0x000, 0x001, 0x002, 0x003, 0x004, 0x005, 0x006, 0x007, 
		 0x008, 0x009, 0x00a, 0x00b, 0x00c, 0x00d, 0x00e, 0x00f,

		 0x010, 0x011, 0x012, 0x013, 0x014, 0x015, 0x016, 0x017, 
		 0x018, 0x019, 0x01a, 0x01b, 0x01c, 0x01d, 0x01e, 0x01f, 

		 0x020, 0x021, 0x022, 0x023, 0x024, 0x025, 0x026, 0x027, 
		 0x028, 0x029, 0x02a, 0x02b, 0x02c, 0x02d, 0x02e, 0x02f, 

		 0x030, 0x031, 0x032, 0x033, 0x034, 0x035, 0x036, 0x037, 
		 0x038, 0x039, 0x03a, 0x03b, 0x03c, 0x03d, 0x03e, 0x03f, 

		 0x040, 0x041, 0x042, 0x043, 0x044, 0x045, 0x046, 0x047, 
		 0x048, 0x049, 0x04a, 0x04b, 0x04c, 0x04d, 0x04e, 0x04f, 

		 0x050, 0x051, 0x052, 0x053, 0x054, 0x055, 0x056, 0x057, 
		 0x058, 0x059, 0x05a, 0x05b, 0x05c, 0x05d, 0x05e, 0x05f, 

		 0x060, 0x061, 0x062, 0x063, 0x064, 0x065, 0x066, 0x067, 
		 0x068, 0x069, 0x06a, 0x06b, 0x06c, 0x06d, 0x06e, 0x06f, 

		 0x070, 0x071, 0x072, 0x073, 0x074, 0x075, 0x076, 0x077, 
		 0x078, 0x079, 0x07a, 0x07b, 0x07c, 0x07d, 0x07e, 0x07f, 

		 0x080, 0x081, 0x082, 0x083, 0x084, 0x085, 0x086, 0x087, 
		 0x088, 0x089, 0x08a, 0x08b, 0x08c, 0x08d, 0x08e, 0x08f, 

		 0x090, 0x091, 0x092, 0x093, 0x094, 0x095, 0x096, 0x097, 
		 0x098, 0x099, 0x09a, 0x09b, 0x09c, 0x09d, 0x09e, 0x09f, 

		 0x0a0, 0x0a1, 0x0a2, 0x0a3, 0x0a4, 0x0a5, 0x0a6, 0x0a7, 
		 0x0a8, 0x0a9, 0x0aa, 0x0ab, 0x0ac, 0x0ad, 0x0ae, 0x0af, 

		 0x0b0, 0x0b1, 0x0b2, 0x0b3, 0x0b4, 0x0b5, 0x0b6, 0x0b7, 
		 0x0b8, 0x0b9, 0x0ba, 0x0bb, 0x0bc, 0x0bd, 0x0be, 0x0bf, 

		 0x080, 0x081, 0x082, 0x083, 0x084, 0x085, 0x086, 0x087, 
		 0x088, 0x089, 0x08a, 0x08b, 0x08c, 0x08d, 0x08e, 0x08f, 

		 0x090, 0x091, 0x092, 0x093, 0x094, 0x095, 0x096, 0x097, 
		 0x098, 0x099, 0x09a, 0x09b, 0x09c, 0x09d, 0x09e, 0x09f, 

		 0x0a0, 0x0a1, 0x0a2, 0x0a3, 0x0a4, 0x0a5, 0x0a6, 0x0a7, 
		 0x0a8, 0x0a9, 0x0aa, 0x0ab, 0x0ac, 0x0ad, 0x0ae, 0x0af, 

		 0x0e0, 0x0e1, 0x0e2, 0x0e3, 0x0e4, 0x0e5, 0x0e6, 0x0e7, 
		 0x0e8, 0x0e9, 0x0ea, 0x0eb, 0x0ec, 0x0ed, 0x0ee, 0x0ef
                                  } ;

  unsigned char *text_ ;
           long  i ;
            int  n ;


   for(text_=(unsigned char *)text, i=0 ; i<size ; i++) {
                                         n = text_[i] & 0x0ff ;
                                   text_[i]=win2dos_table[n] ;
                                                        }
}

/********************************************************************/
/*                                                                  */
/*                 Перекодировка WINDOWS->UTF8                      */
/*                 Перекодировка UTF8->WINDOWS                      */

  long  Lang_DCL::zWinToUTF8(char *data, long size, char *work)

{
   long  cnt ;


#ifdef  UNIX
                cnt=0 ;
#else    
        cnt=MultiByteToWideChar(CP_ACP,  0,         data, size, (LPWSTR)work, size) ;
        cnt=WideCharToMultiByte(CP_UTF8, 0, (LPWSTR)work,  cnt,         data, size*2, NULL, NULL) ;
#endif

    return(cnt) ;
}

  long  Lang_DCL::zUTF8ToWin(char *data, long  size, char *work)

{
   int  cnt ;

     
#ifdef  UNIX
                cnt=0 ;
#else    
      cnt=MultiByteToWideChar(CP_UTF8, 0,         data, size, (LPWSTR)work, size) ;
      cnt=WideCharToMultiByte(CP_ACP,  0, (LPWSTR)work,  cnt,         data, size, NULL, NULL) ;
#endif

    return(cnt) ;
}
