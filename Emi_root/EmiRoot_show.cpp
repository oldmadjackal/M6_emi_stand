/*********************************************************************/
/*                                                                   */
/*               СИСТЕМА ГРАФИЧЕСКОГО ИНТЕРФЕЙСА                     */
/*                                                                   */
/*********************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <search.h>
#include <windows.h>

#include "..\Matrix\Matrix.h"

//#include "..\RSS_Feature\RSS_Feature.h"
//#include "..\RSS_Object\RSS_Object.h"
#include "..\RSS_Kernel\RSS_Kernel.h"

#include "EmiRoot.h"
#include "EmiRoot_show.h"

#pragma warning(disable : 4996)


/*--------------------------------------------- Системные переменные */

#define  SEND_ERROR(text)      SendMessage(RSS_Kernel::kernel_wnd, WM_USER,  \
                                         (WPARAM)_USER_ERROR_MESSAGE,        \
                                         (LPARAM) text)

/*--------------------------------------------- Контексты Рендеринга */

#define  _OPENGL_WND_MAX    10

   static  WndContext  EmiRoot_contexts[_OPENGL_WND_MAX] ;

   static         int  EmiRoot_context_idx ;
   static        HWND  EmiRoot_context_hWnd ;
   static         HDC  EmiRoot_context_hDC ;
   static       HGLRC  EmiRoot_context_hOpenGL ;


/*------------------------------------------------ Дисплейные списки */

#define                           _DRAW_LIST_MAX   10000
  static  char  EmiRoot_draw_list[_DRAW_LIST_MAX] ;       /* FAT дисплейных списков */
  static   int  EmiRoot_draw_list_max ;


/*********************************************************************/
/*                                                                   */
/*                Инициализация системы отображения                  */

 int  EmiRoot_show_init(void)

{
  static  WNDCLASS  EmiRoot_show_wnd ;
              char  message[1024] ;

/*-------------------------------------------- Инициализация Open GL */

/*---------------------------- Регистрация класса окна UD_Show_panel */

	EmiRoot_show_wnd.lpszClassName="EmiRoot_show_class" ;
	EmiRoot_show_wnd.hInstance    = GetModuleHandle(NULL) ;
	EmiRoot_show_wnd.lpfnWndProc  = EmiRoot_show_prc ;
	EmiRoot_show_wnd.hCursor      = LoadCursor(NULL, IDC_ARROW) ;
	EmiRoot_show_wnd.hIcon        =  NULL ;
	EmiRoot_show_wnd.lpszMenuName =  NULL ;
	EmiRoot_show_wnd.hbrBackground=  NULL ;
	EmiRoot_show_wnd.style        =    0 ;
	EmiRoot_show_wnd.hIcon        =  NULL ;

    if(!RegisterClass(&EmiRoot_show_wnd)) {
              sprintf(message, "EmiRoot_Show register error %d", GetLastError()) ;
        EmiRoot_error(message, MB_ICONERROR) ;
	                return(-1) ;
                                          }
/*-------------------------------------------------------------------*/

	return 0;
}


/*********************************************************************/
/*                                                                   */
/*                 Окно системы отображения                          */

 LRESULT CALLBACK  EmiRoot_show_prc(  HWND  hWnd,     UINT  Msg,
 			            WPARAM  wParam, LPARAM  lParam)
{
                    int  wnd_num ;
                    HDC  hDC ;
  PIXELFORMATDESCRIPTOR  Pfd ;
                    int  PixelFormat ;
                   RECT  wnd_rect ;
                 double  xy_aspect ;
                 double  range ;
                 double  range_min ;
                GLfloat  light_pos[4] ;
               Matrix2d  Sum_Matrix ;
               Matrix2d  Oper_Matrix ;  
               Matrix2d  Point_Matrix ;  
                    int  layer ;
                    int  status ;
                   char  text[1024] ;
                    int  i ;

#define  CONTEXT          EmiRoot_contexts[wnd_num]
#define  OPENGL_CONTEXT   EmiRoot_contexts[wnd_num].hOpenGL

/*--------------------------------------- Определение контекста окна */

   for(wnd_num=0 ; wnd_num<_OPENGL_WND_MAX ; wnd_num++)             /* Ищем по таблице контекстов */
     if(EmiRoot_contexts[wnd_num].hWnd==hWnd)  break ;

     if(wnd_num==_OPENGL_WND_MAX) {                                 /* Если нет - занимаем свободный слот */

      for(wnd_num=0 ; wnd_num<_OPENGL_WND_MAX ; wnd_num++)
        if(EmiRoot_contexts[wnd_num].hWnd==NULL)  break ;

        if(wnd_num==_OPENGL_WND_MAX) {
               MessageBox(NULL, "EmiRoot_Show: Too many windows openned", 
                                "Fatal Error", MB_ICONSTOP) ;
                                        return(0) ;
                                     }

        if(CONTEXT.hWnd==NULL) {
                            memset(&CONTEXT, 0, sizeof(CONTEXT)) ;
                                    CONTEXT.Look_z   =   5. ;
                                    CONTEXT.Look_azim=-180. ;
                                    CONTEXT.Zoom     =   1. ;
                               }

                                    CONTEXT.hWnd=hWnd ;
                               }
/*---------------------------------------------- Определение статуса */

/*--------------------------------------------------- Общая разводка */

  switch(Msg) {

/*---------------------------------------------------- Создание окна */

    case WM_CREATE: {
/*- - - - - - - - - - - - - - - - - - - - - Регистрация "имени" окна */
         if(wnd_num==0) {
                    strcpy(EmiRoot_contexts[wnd_num].name, "MAIN") ;
                        }
/*- - - - - - - - - - - - - - - - - -  Описание графического формата */
           memset(&Pfd, 0, sizeof(Pfd)) ;
 
                   Pfd.nSize     =sizeof(Pfd) ;
                   Pfd.nVersion  = 1 ;
                   Pfd.dwFlags   =PFD_DOUBLEBUFFER   |
                                  PFD_SUPPORT_OPENGL |
                                  PFD_DRAW_TO_WINDOW  ;
                   Pfd.iPixelType=PFD_TYPE_RGBA ;
                   Pfd.cColorBits= 24 ;
                   Pfd.cDepthBits= 32 ;
                   Pfd.iLayerType= 24 ;
/*- - - - - - - - - - - - - - - - - - Установка графического формата */
                   hDC=            GetDC(hWnd) ;
           PixelFormat=ChoosePixelFormat(hDC, &Pfd) ; 
        if(PixelFormat==0) {
                  sprintf(text, "EmiRoot_Show: Open-GL"
                                " format choose error: %d", GetLastError()) ;
               MessageBox(NULL, text, "Fatal Error", MB_ICONSTOP) ;
                                   break ;
                           }

           status=SetPixelFormat(hDC, PixelFormat, &Pfd) ;
        if(status==0) {
                  sprintf(text, "EmiRoot_Show: Open-GL"
                                " format set error: %d", GetLastError()) ;
               MessageBox(NULL, text, "Fatal Error", MB_ICONSTOP) ;
                                   break ;
                      }
/*- - - - - - - - - - - - - - - - - - - - Создание Open-GL контекста */
           OPENGL_CONTEXT=wglCreateContext(hDC) ;
        if(OPENGL_CONTEXT==NULL) {
                  sprintf(text, "EmiRoot_Show: Open-GL"
                                " context create error: %d", GetLastError()) ;
               MessageBox(NULL, text, "Fatal Error", MB_ICONSTOP) ;
                                   break ;
                                 }

                               ReleaseDC(hWnd, hDC) ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                         break ;
                    }

/*---------------------------------------------------- Удаление окна */

    case WM_DESTROY: {
                           wglMakeCurrent(NULL, NULL) ;
                         wglDeleteContext(OPENGL_CONTEXT) ;

                          EmiRoot_contexts[wnd_num].hWnd=NULL ;
  
                                            break ;
                     }
/*----------------------------------------------- Основные сообщения */

    case WM_COMMAND:  {
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - Закрыть окно */
//   if(elm==IDK_QUIT) {
//                            return(FALSE) ;
//                     }            
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                                break ;
                      }
/*-------------------------------------------------------- Отрисовка */

    case WM_PAINT:   {
/*- - - - - - - - - - - - - - - - - - - - Задание текущего контекста */
//              InvalidateRect(hWnd, NULL, false) ;
           hDC=          GetDC(hWnd) ;
                wglMakeCurrent(hDC, OPENGL_CONTEXT) ;
/*- - - - - - - - - - - - - - - - - - - - - Задание окна отображения */
         GetClientRect(hWnd, &wnd_rect) ;
             xy_aspect=((double)wnd_rect.bottom)/
                       ((double)wnd_rect.right) ;

            glViewport(0, 0, wnd_rect.right, wnd_rect.bottom) ;
/*- - - - - - - - - - - - - - - - - - -  Конфигурирование рендеренга */
              glEnable(GL_DEPTH_TEST) ;
              glEnable(GL_POLYGON_OFFSET_LINE) ;
              glEnable(GL_POLYGON_OFFSET_FILL) ;

//            glEnable(GL_LIGHTING) ;
              glEnable(GL_COLOR_MATERIAL) ;
       glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE) ;
           glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA) ;

          glClearColor(0.7f, 0.7f, 0.7f, 0.0f) ;
               glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
/*- - - - - - - - - - - - - - - - - - - -  Описание источников света */
#pragma warning(disable : 4244)

                light_pos[0]=CONTEXT.Look_x ;
                light_pos[1]=CONTEXT.Look_y ;
                light_pos[2]=CONTEXT.Look_z ;
                light_pos[3]=   1. ;

#pragma warning(default : 4244)

              glEnable(GL_LIGHT0) ;
             glLightfv(GL_LIGHT0, GL_POSITION, light_pos) ;
/*- - - - - - - - - - - - - - - - - - - - - - - Задание точки зрения */
     if(CONTEXT.AtObject[0]!=0) {                                   /* Если специфицирована точка наблюдения */
                                   EmiRoot_lookat_point(&CONTEXT) ;
                                    range=CONTEXT.Range_strobe ;
                                }
     else                       {
                                    range=0. ;
                                }

          glMatrixMode(GL_PROJECTION) ;
        glLoadIdentity() ;
//      gluPerspective(30., xy_aspect, 5., 100.) ;
//           glFrustum(-2., 2., -2., 2, 5., 100.) ;

                             range_min=range-10.*CONTEXT.Zoom ;
           if(range_min<0.)  range_min=  0. ;
                             range_min=  0. ;
  
               glOrtho(-CONTEXT.Zoom*0.5, 
                        CONTEXT.Zoom*0.5, 
                       -CONTEXT.Zoom*0.5*xy_aspect,
                        CONTEXT.Zoom*0.5*xy_aspect,
                               range_min, range+100.*CONTEXT.Zoom) ;
  
          glMatrixMode(GL_MODELVIEW) ;
        glLoadIdentity() ;

               Point_Matrix.LoadZero   (3, 1) ;
               Point_Matrix.SetCell    (2, 0, 10.) ;

                 Sum_Matrix.Load3d_azim(CONTEXT.Look_azim) ;
                Oper_Matrix.Load3d_elev(CONTEXT.Look_elev) ;
                 Sum_Matrix.LoadMul    (&Sum_Matrix, &Oper_Matrix) ;
               Point_Matrix.LoadMul    (&Sum_Matrix, &Point_Matrix) ;

             gluLookAt(CONTEXT.Look_x,
                       CONTEXT.Look_y,
                       CONTEXT.Look_z,
                       CONTEXT.Look_x+Point_Matrix.GetCell(0, 0),
                       CONTEXT.Look_y+Point_Matrix.GetCell(1, 0),
                       CONTEXT.Look_z+Point_Matrix.GetCell(2, 0),
                             0.,        1.,        0.  ) ;
/*- - - - - - - - - - - - - - - - - - - - - - -  Отрисовка тел сцены */
       for(layer=1 ; layer<3 ; layer++) 
        for(i=1 ; i<=EmiRoot_draw_list_max ; i++)
          if(EmiRoot_draw_list[i]==layer) {

                      glPushMatrix() ;

                        glCallList(i) ;
                 status=glGetError() ;
              if(status) {
                              sprintf(text, "OpenGL error: %d", status) ;
                           MessageBox(NULL, text, "", 0) ;
                         }

                       glPopMatrix() ;
                                      }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - Вывод на экран */
                                 glFlush() ;
                             SwapBuffers(hDC) ;
/*- - - - - - - - - - - - - - - - -  Освобождение текущего контекста */
                          wglMakeCurrent(NULL, NULL) ;
                               ReleaseDC(hWnd, hDC) ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                          ValidateRect(hWnd,  NULL) ;

			     break ;
		     }
/*---------------------------------- Отработка инструментальной зоны */

/*------------------------------------------------- Прочие сообщения */

    default :        {
		return( DefWindowProc(hWnd, Msg, wParam, lParam) ) ;
			    break ;
		     }
/*-------------------------------------------------------------------*/
	      }
/*-------------------------------------------------------------------*/

#undef  CONTEXT      
#undef  OPENGL_CONTEXT

    return(0) ;
}


/*********************************************************************/
/*                                                                   */
/*                 Установить первый контекст                        */
/*                 Установить следующий контекст                     */

 int  EmiRoot_first_context(char *name)

{
   EmiRoot_context_idx=-1 ;

  return(EmiRoot_next_context(name)) ;
}

 int  EmiRoot_next_context(char *name)

{
#define  CONTEXT   EmiRoot_contexts[EmiRoot_context_idx]

/*----------------------------------- Закрытие предыдущих контекстов */

  if(EmiRoot_context_hDC){
                          ReleaseDC(EmiRoot_context_hWnd, EmiRoot_context_hDC) ;
//                   wglMakeCurrent(NULL, NULL) ;
//                 wglDeleteContext(EmiRoot_context_hOpenGL) ;

                           EmiRoot_context_hWnd    =NULL ;
                           EmiRoot_context_hDC     =NULL ;
                            EmiRoot_context_hOpenGL=NULL ;
                         }
/*----------------------------------------- Открытие нового конткста */

         EmiRoot_context_idx++ ;

  for( ; EmiRoot_context_idx<_OPENGL_WND_MAX ; EmiRoot_context_idx++) {

    if(CONTEXT.hWnd==NULL)  continue ;

           EmiRoot_context_hWnd   =                 CONTEXT.hWnd ;
           EmiRoot_context_hDC    =           GetDC(CONTEXT.hWnd) ;
//         EmiRoot_context_hOpenGL=wglCreateContext(EmiRoot_context_hDC) ;
           EmiRoot_context_hOpenGL=                 CONTEXT.hOpenGL ;

                wglMakeCurrent(EmiRoot_context_hDC, EmiRoot_context_hOpenGL) ;
      
                            return(0) ;
                                                                      }

/*-------------------------------------------------------------------*/

#undef  CONTEXT

  return(-1) ;
}


/*********************************************************************/
/*                                                                   */
/*                      Отобразить контекст                          */

 void  EmiRoot_show_context(char *name)

{
    SendMessage(EmiRoot_context_hWnd, WM_PAINT, 0, 0) ;
}


/*********************************************************************/
/*                                                                   */
/*                     Выдать параметр контекста                     */

  double  EmiRoot_get_context(char *par)

{
#define  CONTEXT   EmiRoot_contexts[EmiRoot_context_idx]


    if(!stricmp(par, "ZOOM" ))  return(CONTEXT.Zoom) ;
    if(!stricmp(par, "RANGE"))  return(CONTEXT.Zoom) ;
 
#undef  CONTEXT

                               return(0.) ;
}


/*********************************************************************/
/*                                                                   */
/*                   Считать параметры контекста                     */

  void  EmiRoot_read_context(std::string *buff)

{
   char  text[1024] ;
   char *name ;
   char *entry ;
   char *end ;

#define  CONTEXT   EmiRoot_contexts[EmiRoot_context_idx]

/*------------------------------------------------------- Подготовка */

            memset(text, 0, sizeof(text)) ;
           strncpy(text, buff->c_str(), sizeof(text)-1) ;

/*-------------------------------------- Идентификация окна по имени */

         name =strstr(text, " NAME=") ;
      if(name==NULL)  return ;
         name+=strlen(" NAME=") ;

         end=strchr(name, ' ') ;
      if(end!=NULL)  *end=0 ;

     for(EmiRoot_context_idx=0 ; 
         EmiRoot_context_idx<_OPENGL_WND_MAX ;
         EmiRoot_context_idx++                 ) {

                 if(         CONTEXT.hWnd==NULL )  continue ;
                 if(!stricmp(CONTEXT.name, name))  break ;
                                                 }
/*---------------------------------------- Создание нового контекста */

    if(EmiRoot_context_idx==_OPENGL_WND_MAX) {
                                                 return ;
                                             }
/*------------------------------------------ Пропись параметров окна */

    entry=strstr(end+1, "LOOK_X=") ;  CONTEXT.Look_x   =atof(entry+strlen("LOOK_X=")) ;
    entry=strstr(end+1, "LOOK_Y=") ;  CONTEXT.Look_y   =atof(entry+strlen("LOOK_Y=")) ;
    entry=strstr(end+1, "LOOK_Z=") ;  CONTEXT.Look_z   =atof(entry+strlen("LOOK_Z=")) ;
    entry=strstr(end+1, "LOOK_A=") ;  CONTEXT.Look_azim=atof(entry+strlen("LOOK_A=")) ;
    entry=strstr(end+1, "LOOK_E=") ;  CONTEXT.Look_elev=atof(entry+strlen("LOOK_E=")) ;
    entry=strstr(end+1, "LOOK_R=") ;  CONTEXT.Look_roll=atof(entry+strlen("LOOK_R=")) ;
    entry=strstr(end+1, "ZOOM="  ) ;  CONTEXT.Zoom     =atof(entry+strlen("ZOOM=")) ;

    entry=strstr(end+1, "LOOKAT=") ;
         strncpy(CONTEXT.AtObject, entry+strlen("LOOKAT="), sizeof(CONTEXT.AtObject)) ;

/*-------------------------------------------------------------------*/

#undef  CONTEXT
}


/*********************************************************************/
/*                                                                   */
/*                   Записать параметры контекста                    */

  void  EmiRoot_write_context(std::string *text)

{
  char  field[1024] ;

#define  CONTEXT   EmiRoot_contexts[EmiRoot_context_idx]

                                                           *text = "" ;
    sprintf(field, " NAME=%s",       CONTEXT.name     ) ;  *text+=field ;
    sprintf(field, " LOOK_X=%.10lf", CONTEXT.Look_x   ) ;  *text+=field ;
    sprintf(field, " LOOK_Y=%.10lf", CONTEXT.Look_y   ) ;  *text+=field ;
    sprintf(field, " LOOK_Z=%.10lf", CONTEXT.Look_z   ) ;  *text+=field ;
    sprintf(field, " LOOK_A=%.10lf", CONTEXT.Look_azim) ;  *text+=field ;
    sprintf(field, " LOOK_E=%.10lf", CONTEXT.Look_elev) ;  *text+=field ;
    sprintf(field, " LOOK_R=%.10lf", CONTEXT.Look_roll) ;  *text+=field ;
    sprintf(field, " ZOOM=%.10lf",   CONTEXT.Zoom     ) ;  *text+=field ;
    sprintf(field, " LOOKAT=%s",     CONTEXT.AtObject ) ;  *text+=field ;

#undef  CONTEXT
}


/*********************************************************************/
/*                                                                   */
/*            Резервирование номера дисплейного списка               */
/*                                                                   */
/*     Параметры:  mode=0  -  основная группа нормальной             */
/*                              очередности отрисовки                */
/*                 mode=2  -  основная группа последней              */
/*                              очередности отрисовки                */
/*                 mode=1  -  вложенная группа                       */

 int  EmiRoot_show_getlist(int mode_flag)

{
  int  i ;


    for(i=1 ; i<_DRAW_LIST_MAX ; i++)
      if(!EmiRoot_draw_list[i]) {

            if(mode_flag==2)  EmiRoot_draw_list[i]= 2 ;      
       else if(mode_flag==1)  EmiRoot_draw_list[i]=-1 ;      
       else                   EmiRoot_draw_list[i]= 1 ;

       if(i>EmiRoot_draw_list_max)  EmiRoot_draw_list_max=i ;
                                    return(i) ;
                                }

        MessageBox(NULL, "Display lists numeration overflow", 
                         "Fatal Error", MB_ICONSTOP) ;

  return(0) ;
}


/*********************************************************************/
/*                                                                   */
/*               Освобождение номера дисплейного списка              */

   void  EmiRoot_show_releaselist(int  idx)

{
                              EmiRoot_draw_list[idx]=0;   

    if(idx==EmiRoot_draw_list_max)  
      while(                  EmiRoot_draw_list_max > 0 &&
            EmiRoot_draw_list[EmiRoot_draw_list_max]==0   )
                              EmiRoot_draw_list_max-- ;
}


/*********************************************************************/
/*                                                                   */
/*              Работа с точкой зрения камеры                        */

   int  EmiRoot_look(  char *name, char *oper, 
                     double *x, double *y, double *z,
                     double *a, double *e, double *r ) 
{
   int  wnd_num ;

/*--------------------------------------- Определение контекста окна */

   for(wnd_num=0 ; wnd_num<_OPENGL_WND_MAX ; wnd_num++)             /* Ищем по таблице контекстов */
     if(!stricmp(EmiRoot_contexts[wnd_num].name, name))  break ;

     if(wnd_num==_OPENGL_WND_MAX)  return(-1) ;

/*------------------------------------------------------- Извлечение */

   if(!stricmp(oper, "GET")) {

            if(x!=NULL)  *x=EmiRoot_contexts[wnd_num].Look_x ;
            if(y!=NULL)  *y=EmiRoot_contexts[wnd_num].Look_y ;
            if(z!=NULL)  *z=EmiRoot_contexts[wnd_num].Look_z ;
            if(a!=NULL)  *a=EmiRoot_contexts[wnd_num].Look_azim ;
            if(e!=NULL)  *e=EmiRoot_contexts[wnd_num].Look_elev ;
            if(r!=NULL)  *r=EmiRoot_contexts[wnd_num].Look_roll ;

                             }
/*-------------------------------------------------------- Занесение */

   if(!stricmp(oper, "SET")) {

            if(x!=NULL)  EmiRoot_contexts[wnd_num].Look_x   =*x ;
            if(y!=NULL)  EmiRoot_contexts[wnd_num].Look_y   =*y ;
            if(z!=NULL)  EmiRoot_contexts[wnd_num].Look_z   =*z ;
            if(a!=NULL)  EmiRoot_contexts[wnd_num].Look_azim=*a ;
            if(e!=NULL)  EmiRoot_contexts[wnd_num].Look_elev=*e ;
            if(r!=NULL)  EmiRoot_contexts[wnd_num].Look_roll=*r ;

                             }
/*-------------------------------------------------------------------*/

  return(0) ;
}


/*********************************************************************/
/*                                                                   */
/*                 Работа с полем зрения камеры                      */

   int  EmiRoot_zoom(char *name, char *oper, double *zoom) 
{
   int  wnd_num ;

/*--------------------------------------- Определение контекста окна */

   for(wnd_num=0 ; wnd_num<_OPENGL_WND_MAX ; wnd_num++)             /* Ищем по таблице контекстов */
     if(!stricmp(EmiRoot_contexts[wnd_num].name, name))  break ;

     if(wnd_num==_OPENGL_WND_MAX)  return(-1) ;

/*------------------------------------------------------- Извлечение */

   if(!stricmp(oper, "GET")) {

            if(zoom!=NULL)  *zoom=EmiRoot_contexts[wnd_num].Zoom ;

                             }
/*-------------------------------------------------------- Занесение */

   if(!stricmp(oper, "SET")) {

            if(zoom!=NULL)  EmiRoot_contexts[wnd_num].Zoom=*zoom ;

                             }
/*-------------------------------------------------------------------*/

  return(0) ;
}


/*********************************************************************/
/*                                                                   */
/*                 Работа с точкой наблюдения камеры                 */

   int  EmiRoot_lookat(char *name, char *oper, char *look_at) 
{
   int  wnd_num ;

/*--------------------------------------- Определение контекста окна */

   for(wnd_num=0 ; wnd_num<_OPENGL_WND_MAX ; wnd_num++)             /* Ищем по таблице контекстов */
     if(!stricmp(EmiRoot_contexts[wnd_num].name, name))  break ;

     if(wnd_num==_OPENGL_WND_MAX)  return(-1) ;

/*------------------------------------------------------- Извлечение */

   if(!stricmp(oper, "GET")) {

     if(look_at!=NULL)  strcpy(look_at, EmiRoot_contexts[wnd_num].AtObject) ;

                             }
/*-------------------------------------------------------- Занесение */

   if(!stricmp(oper, "SET")) {

     if(look_at!=NULL) {

         memset(EmiRoot_contexts[wnd_num].AtObject, 0, 
                              sizeof(EmiRoot_contexts[wnd_num].AtObject)) ;
        strncpy(EmiRoot_contexts[wnd_num].AtObject, look_at,
                              sizeof(EmiRoot_contexts[wnd_num].AtObject)-1) ;
                       } 
                             }
/*-------------------------------------------------------------------*/

  return(0) ;
}


/*********************************************************************/
/*                                                                   */
/*                         Перерисовать окно                         */

   int  EmiRoot_redraw(char *name) 
{
   int  wnd_num ;

/*--------------------------------------- Определение контекста окна */

   for(wnd_num=0 ; wnd_num<_OPENGL_WND_MAX ; wnd_num++)             /* Ищем по таблице контекстов */
     if(!stricmp(EmiRoot_contexts[wnd_num].name, name))  break ;

     if(wnd_num==_OPENGL_WND_MAX)  return(-1) ;

/*------------------------------------------------------ Перерисовка */

      SendMessage(EmiRoot_contexts[wnd_num].hWnd, WM_PAINT, 0, 0) ;

/*-------------------------------------------------------------------*/

  return(0) ;
}


/*********************************************************************/
/*                                                                   */
/*                   Обработка точки наблюдения                      */

   int  EmiRoot_lookat_point(WndContext *context) 
{
  RSS_Point  target ;
     double  x ;
     double  y ;
     double  z ;
     double  dy ;
     double  ds ;
       char  name[256] ;
       char *end ;
        int  i ;

#define  _RAD_TO_GRD  57.296  

/*------------------------------------------- Обработка на константу */

                x=strtod(context->AtObject, &end) ;
                y=strtod(              end, &end) ;
                z=strtod(              end, &end) ;

/*---------------------------------------------- Обработка на объект */

   if(*end!=0) {

#define   OBJECTS       RSS_Kernel::kernel->kernel_objects 
#define   OBJECTS_CNT   RSS_Kernel::kernel->kernel_objects_cnt 

                    memset(name, 0, sizeof(name)) ;
                   strncpy(name, context->AtObject, sizeof(name)-1) ;

       for(i=0 ; i<OBJECTS_CNT ; i++)
         if(!stricmp(OBJECTS[i]->Name, name))  break ;

    if(i==OBJECTS_CNT) {                                            /* Если имя не задано... */
             SEND_ERROR("Объект наблюдения НЕ существует") ;
                  context->AtObject[0]=0 ;
                             return(0) ;
                       }

            OBJECTS[i]->vGetPosition(&target) ;

                        x=target.x ;
                        y=target.y ;
                        z=target.z ;

/*-------------------------------------------------------------------*/

#undef   OBJECTS
#undef   OBJECTS_CNT

               } 
/*---------------------------------------- Пересчет углов наблюдения */

                dy=y-context->Look_y ;
                ds=sqrt((x-context->Look_x)*(x-context->Look_x)+
                        (z-context->Look_z)*(z-context->Look_z) ) ;

     context->Look_elev=-_RAD_TO_GRD*atan (dy/ds) ;
     context->Look_azim=-_RAD_TO_GRD*atan2(x-context->Look_x, 
                                           z-context->Look_z) ;

/*------------------------------------------------- Расчет дальности */

     context->Range_strobe=sqrt((x-context->Look_x)*(x-context->Look_x)+
                                (y-context->Look_y)*(y-context->Look_y)+
                                (z-context->Look_z)*(z-context->Look_z) ) ;

/*-------------------------------------------------------------------*/

#undef  _RAD_TO_GRD

  return(0) ;
}
