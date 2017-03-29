/*********************************************************************/
/*                                                                   */
/*               СИСТЕМА ГРАФИЧЕСКОГО ИНТЕРФЕЙСА                     */
/*                                                                   */
/*********************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include <windows.h>

#include "UserDlg.h"

#pragma warning(disable : 4996)


/*********************************************************************/
/*                                                                   */
/*                Инициализация системы диалога                      */

 int  UD_init(void)

{
  static  WNDCLASS  UD_show_panel_wnd ;
  static  WNDCLASS  UD_color_panel_wnd ;
  static  WNDCLASS  UD_color_bank_wnd ;
  static  WNDCLASS  UD_diagram_2D_wnd ;
  static  WNDCLASS  UD_area_2D_wnd ;
              char  message[1024] ;

/*---------------------------- Регистрация класса окна UD_show_panel */

	UD_show_panel_wnd.lpszClassName="UD_show_panel" ;
	UD_show_panel_wnd.hInstance    = GetModuleHandle(NULL) ;
	UD_show_panel_wnd.lpfnWndProc  = UD_show_panel_prc ;
	UD_show_panel_wnd.hCursor      = LoadCursor(NULL, IDC_ARROW) ;
	UD_show_panel_wnd.hIcon        =  NULL ;
	UD_show_panel_wnd.lpszMenuName =  NULL ;
	UD_show_panel_wnd.hbrBackground=  NULL ;
	UD_show_panel_wnd.style        =    0 ;
	UD_show_panel_wnd.hIcon        =  NULL ;

    if(!RegisterClass(&UD_show_panel_wnd)) {
         sprintf(message, "UD_show_panel register error %d",
                                               GetLastError()) ;
        UD_error(message) ;
	                                return(-1) ;
				           }
/*--------------------------- Регистрация класса окна UD_color_panel */

	UD_color_panel_wnd.lpszClassName="UD_color_panel" ;
	UD_color_panel_wnd.hInstance    = GetModuleHandle(NULL) ;
	UD_color_panel_wnd.lpfnWndProc  = UD_color_panel_prc ;
	UD_color_panel_wnd.hCursor      = LoadCursor(NULL, IDC_ARROW) ;
	UD_color_panel_wnd.hIcon        =  NULL ;
	UD_color_panel_wnd.lpszMenuName =  NULL ;
	UD_color_panel_wnd.hbrBackground=  NULL ;
	UD_color_panel_wnd.style        = CS_DBLCLKS  ;
	UD_color_panel_wnd.hIcon        =  NULL ;

    if(!RegisterClass(&UD_color_panel_wnd)) {
         sprintf(message, "UD_color_panel register error %d",
                                               GetLastError()) ;
        UD_error(message) ;
	                                return(-1) ;
				           }
/*---------------------------- Регистрация класса окна UD_color_bank */

	UD_color_bank_wnd.lpszClassName="UD_color_bank" ;
	UD_color_bank_wnd.hInstance    = GetModuleHandle(NULL) ;
	UD_color_bank_wnd.lpfnWndProc  = UD_color_bank_prc ;
	UD_color_bank_wnd.hCursor      = LoadCursor(NULL, IDC_ARROW) ;
	UD_color_bank_wnd.hIcon        =  NULL ;
	UD_color_bank_wnd.lpszMenuName =  NULL ;
	UD_color_bank_wnd.hbrBackground=  NULL ;
	UD_color_bank_wnd.style        = CS_DBLCLKS  ;
	UD_color_bank_wnd.hIcon        =  NULL ;

    if(!RegisterClass(&UD_color_bank_wnd)) {
         sprintf(message, "UD_color_bank register error %d",
                                               GetLastError()) ;
        UD_error(message) ;
	                                return(-1) ;
				           }
/*---------------------------- Регистрация класса окна UD_diagram_2D */

	UD_diagram_2D_wnd.lpszClassName="UD_diagram_2D" ;
	UD_diagram_2D_wnd.hInstance    = GetModuleHandle(NULL) ;
	UD_diagram_2D_wnd.lpfnWndProc  = UD_diagram_2D_prc ;
	UD_diagram_2D_wnd.hCursor      = LoadCursor(NULL, IDC_ARROW) ;
	UD_diagram_2D_wnd.hIcon        =  NULL ;
	UD_diagram_2D_wnd.lpszMenuName =  NULL ;
	UD_diagram_2D_wnd.hbrBackground=  NULL ;
	UD_diagram_2D_wnd.style        =    0 ;
	UD_diagram_2D_wnd.hIcon        =  NULL ;

    if(!RegisterClass(&UD_diagram_2D_wnd)) {
         sprintf(message, "UD_diagram_2D register error %d",
                                               GetLastError()) ;
        UD_error(message) ;
	                                return(-1) ;
				           }
/*------------------------------- Регистрация класса окна UD_area_2D */

	UD_area_2D_wnd.lpszClassName="UD_area_2D" ;
	UD_area_2D_wnd.hInstance    = GetModuleHandle(NULL) ;
	UD_area_2D_wnd.lpfnWndProc  = UD_area_2D_prc ;
	UD_area_2D_wnd.hCursor      = LoadCursor(NULL, IDC_ARROW) ;
	UD_area_2D_wnd.hIcon        =  NULL ;
	UD_area_2D_wnd.lpszMenuName =  NULL ;
	UD_area_2D_wnd.hbrBackground=  NULL ;
	UD_area_2D_wnd.style        =    0 ;
	UD_area_2D_wnd.hIcon        =  NULL ;

    if(!RegisterClass(&UD_area_2D_wnd)) {
         sprintf(message, "UD_area_2D register error %d",
                                               GetLastError()) ;
        UD_error(message) ;
	                                return(-1) ;
				           }
/*-------------------------------------------------------------------*/

	return 0;
}


/*********************************************************************/
/*								     */
/*	          Обработка сообщений об ошибках                     */    

   int  UD_error(char *message)

{
    MessageBox(NULL, message, "User Dialog system", MB_ICONERROR) ;

  return(0) ;
}


/*********************************************************************/
/*                                                                   */
/*                Преобразование адреса данных в строку              */
/*                Преобразование строки в адрес данных               */

#define  _UD_PTR_PREFIX   "UD_Pointer:"

  union {
          unsigned  long  code ;
                    void *ptr ;  } UD_ptr_cvt ;


   char *UD_ptr_incode(void *ptr)

{
  static char  ptr_text[128] ;
         char  tmp[32] ;


              UD_ptr_cvt.ptr=ptr ;
        ultoa(UD_ptr_cvt.code, tmp, 16) ;

       strcpy(ptr_text, _UD_PTR_PREFIX) ;
       strcat(ptr_text,     tmp       ) ;

   return(ptr_text) ;
}


   void *UD_ptr_decode(char *text)

{
  char *end ;


        end=strstr(text, _UD_PTR_PREFIX) ;
     if(end==NULL)  return(NULL) ;

        end=text+strlen(_UD_PTR_PREFIX) ;

          UD_ptr_cvt.code=strtoul(end, &end, 16) ;

   return(UD_ptr_cvt.ptr) ;
}


/*********************************************************************/
/*                                                                   */
/*                Формирование канонического прямоугольника          */

   void  UDi_form_rect(RECT *Rect, int x1, int y1, int x2, int y2)

{
        if(x2>x1) {   Rect->left  =x1 ;
                      Rect->right =x2 ;  }
        else      {   Rect->left  =x2 ;
                      Rect->right =x1 ;  }
        if(x2==x1)    Rect->right++ ;

        if(y2>y1) {   Rect->top   =y1 ;
                      Rect->bottom=y2 ;  }
        else      {   Rect->top   =y2 ;
                      Rect->bottom=y1 ;  }
        if(y2==y1)    Rect->bottom++ ;
}


/*********************************************************************/
/*                                                                   */
/*                Рассчет данных по цветовой таблице                 */

   COLORREF UDi_assign_color(int  type, double  value, struct UD_color_range *table)

{
   COLORREF  color ;
     double  coef ;
        int  red ;
        int  green ;
        int  blue ;
        int  i_max ;
        int  i ;

/*--------------------------------------- Определение границ таблицы */

        if(table==NULL)  return(0) ;

                                  i_max=-1 ;
    for(i=0 ; table[i].use ; i++) i_max= i ;

        if(i_max==-1)  return(0) ;

/*-------------------------------------- Обработка граничных случаев */

        if(value<=table[   0 ].value)  return(table[   0 ].color) ;
        if(value>=table[i_max].value)  return(table[i_max].color) ;

/*---------------------------------------------------- Рассчет цвета */

    for(i=0 ; i<i_max ; i++) 
      if(value<table[i].value)  break ;
/*- - - - - - - - - - - - - - - - - - - - - - Плавный переход цветов */
      if(type & _UD_SMART_COLOR) {

          coef=(         value-table[i-1].value)/
               (table[i].value-table[i-1].value) ;

           red= GetRValue(table[i-1].color)  +
               (GetRValue(table[i  ].color)-
                GetRValue(table[i-1].color) )*coef ;               
         green= GetGValue(table[i-1].color)  +
               (GetGValue(table[i  ].color)-
                GetGValue(table[i-1].color) )*coef ;               
          blue= GetBValue(table[i-1].color)  +
               (GetBValue(table[i  ].color)-
                GetBValue(table[i-1].color) )*coef ;               

         color= RGB(red, green, blue) ;

                                 }
/*- - - - - - - - - - - - - - - - - - - - - -  Резкий переход цветов */
      else                      {
                                        color=table[i-1].color ;
                                }
/*-------------------------------------------------------------------*/

   return(color) ;
}

/*********************************************************************/
/*                                                                   */
/*                Рассчет индекса по цветовой таблице                */

   int UDi_assign_index(double  value, struct UD_color_range *table)

{
        int  i_max ;
        int  i ;

/*--------------------------------------- Определение границ таблицы */

        if(table==NULL)  return(0) ;

                                  i_max=-1 ;
    for(i=0 ; table[i].use ; i++) i_max= i ;

        if(i_max==-1)  return(0) ;

/*-------------------------------------- Обработка граничных случаев */

      if(value<=table[   0 ].value)  return(   0 ) ;
      if(value>=table[i_max].value)  return(i_max) ;

    for(i=i_max-1 ; i>=0 ; i--) 
      if(value>table[i].value)  break ;

/*-------------------------------------------------------------------*/

   return(i) ;
}


/*********************************************************************/
/*                                                                   */
/*                    Инверсия по границе зоны                       */

   void  UDi_zone_invert(HDC hDC, UD_metrics *metrics, RECT *Zone)
{
  RECT  Rect ;


         if(Zone->left  <0 ||
            Zone->right <0 ||
            Zone->top   <0 || 
            Zone->bottom<0   )  return ;

        UDi_form_rect(&Rect, Zone->left , Zone->top   ,
                             Zone->right, Zone->top    ) ;
           InvertRect(hDC, &Rect) ;
        UDi_form_rect(&Rect, Zone->right, Zone->top   ,
                             Zone->right, Zone->bottom ) ;
           InvertRect(hDC, &Rect) ;
        UDi_form_rect(&Rect, Zone->right, Zone->bottom,
                             Zone->left , Zone->bottom ) ;
           InvertRect(hDC, &Rect) ;
        UDi_form_rect(&Rect, Zone->left , Zone->bottom,
                             Zone->left , Zone->top    ) ;
           InvertRect(hDC, &Rect) ;
}


/*********************************************************************/
/*                                                                   */
/*                    Инверсия "креста" сканера                      */

   void  UDi_cross_invert(HDC hDC, UD_metrics *metrics, int  x, int  y)
{
  RECT  Rect ;



   if(x>=metrics->draw_x_base                     &&
      x< metrics->draw_x_base+metrics->draw_x_size  ) {

        UDi_form_rect(&Rect, x, metrics->draw_y_base,
                             x, metrics->draw_y_base+
                                metrics->draw_y_size-1) ;
           InvertRect(hDC, &Rect) ;
                                                      } 

   if(y>=metrics->draw_y_base                     ||
      y< metrics->draw_y_base+metrics->draw_y_size  ) {

        UDi_form_rect(&Rect, metrics->draw_x_base,   y,
                             metrics->draw_x_base+
                             metrics->draw_x_size-1, y) ;
           InvertRect(hDC, &Rect) ;
                                                      } 
}


/*********************************************************************/
/*                                                                   */
/*         Определение оптимального разбиения диапазона              */
/*                                                                   */
/*  Параметры:                                                       */
/*        range  -  Диапазон                                         */
/*                                                                   */
/*   Return:  шаг сетки диапазона                                    */

   double  UDi_range_step(double  range)

{
     int  power ;
  double  base ;
  double  mant ;
  double  opt1 ;
  double  opt2 ;
  double  range_step ;
     int  i ;

#define  _OPT_CNT   6.
#define _STEP_CNT  16
  double  step[_STEP_CNT]={ 0.01,  0.02, 0.025, 0.05, 
                            0.1,   0.2,  0.25,  0.5, 
                            1.,    2.,   2.5,   5., 
                           10.,   20.,  25.,  50.} ;
     int  cnt[_STEP_CNT] ;


	   power=log10(range) ;

					   base = 1. ;
    if(power<0)  for(i=power ; i<0 ; i++)  base/=10. ;
    else         for(i=0 ; i<power ; i++)  base*=10. ;


	  mant=range/base ;

    for(i=0 ; i<_STEP_CNT ; i++) {
				      cnt[i]=mant/step[i] ;
				   if(cnt[i]<_OPT_CNT)  break ;
				 }

	 opt1=_OPT_CNT/(cnt[i]+0.) ;
	 opt2=cnt[i-1]/_OPT_CNT ;

      if(opt1<opt2)  range_step=step[i  ]*base ;
      else           range_step=step[i-1]*base ;

  return(range_step) ;

#undef  _OPT_CNT    
#undef _STEP_CNT   

}


/*********************************************************************/
/*                                                                   */
/*         Определение потребной длины поля оцифровки                */
/*                                                                   */
/*  Параметры:                                                       */
/*		 value1  -  Диапазон чисел                           */
/*		 value2  -                                           */
/*	      accuracy   -  Градационный параметр                    */
/*                zone   -  Описание поля вывода                     */

  void  UDi_value_zone(  double  value1,
		         double  value2,
		         double  step,
                  UD_value_zone *zone     )

{
     char *digits ;
     char  format_type ;
      int  right_pos ;
      int  left1_pos ;
      int  left2_pos ;
      int  left_pos ;
      int  sign ;
      int  point ;
      int  i ;

/*---------------------------------------------------- Инициализация */

        memset(zone, 0, sizeof(*zone)) ;

                format_type=0 ;

/*------------------------------------- Правая граница значащих цифр */

               digits=fcvt(step, 500, &point, &sign) ;
        
            for(i=0 ; digits[i]!='0' && 
                      digits[i]!='9' && 
                      digits[i]!= 0     ; i++) ;
           
        if(point>0)  right_pos=point-i+1 ;
        else         right_pos=point-i  ;

/*-------------------------------------- Левая граница значащих цифр */

               digits=fcvt(value1, 500, &point, &sign) ;

            if(digits[0]=='0')  left1_pos=-99  ;
       else if( point    > 0 )  left1_pos=point  ;
       else                     left1_pos=point-1 ;

               digits=fcvt(value2, 500, &point, &sign) ;

            if(digits[0]=='0')  left2_pos=-99  ;
       else if( point    > 0 )  left2_pos=point  ;
       else                     left2_pos=point-1 ;

        if(left1_pos>left2_pos)  left_pos=left1_pos ;
        else                     left_pos=left2_pos ;       

/*----------------------------------- Определяем число значащих цифр */

                        zone->digits=left_pos-right_pos+1 ;             
                        zone->digits++  ;             

/*---------------------------------- Окончательная разводка форматов */

        if(right_pos> 3) {  format_type='E' ;
                            zone->power=right_pos
                                       +zone->digits-2-1 ;  }

        if( left_pos<-3) {  format_type='E' ;
                            zone->power=left_pos ;  }

        if(format_type=='E') {

           if(zone->digits==2) zone->digits=3 ;

                   zone->point=zone->digits-2 ;
                   zone->len  =zone->digits+2+zone->power/10+1 ;
                             }   
        else                 {

              if(right_pos<0)  zone->point=-right_pos ;
              else             zone->point=  0 ;   
                               zone->len  =zone->digits ;
                             }      
/*-------------------------------------------------------------------*/

#undef    _VALUES_MAX 
}


/*********************************************************************/
/*                                                                   */
/*                    Перевод числа в текст                          */
/*                                                                   */
/*  Параметры:                                                       */
/*		  value  -  число                                    */
/*                 zone  -  описание поля вывода                     */
/*	      fill_flag  -  флаг дополнения до полной зоны           */

     void  UDi_value_cvt(double  value, UD_value_zone *zone, 
                                                  int  fill_flag)

{
   char  format[128] ;

/*------------------------------------------- Приведение к E-формату */

     if(zone->power) {

        sprintf(format, "%s%d.%dlfe%+d", "%", 
                           zone->digits, zone->point, zone->power) ;
        sprintf(zone->buff, format, value/pow(10., zone->power)) ;
                     }
/*------------------------------------------- Приведение к F-формату */
        
     else            {

        sprintf(format, "%s%d.%dlf", "%", zone->len, zone->point) ;
        sprintf(zone->buff, format, value) ;
                     }   
/*--------------------------------------------- Дополнение пробелами */

/*-------------------------------------------------------------------*/
}


