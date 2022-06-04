/*********************************************************************/
/*                                                                   */
/*                    Утилита внешнего моделирования                 */
/*                                                                   */
/*********************************************************************/

#ifdef  __MAIN__
#define  _EXTERNAL
#else
#define  _EXTERNAL  extern
#endif

/*---------------------------------------------- Параметры генерации */

#define   _APPLICATION     "ExternalModel"
#define   _PROGRAM_TITLE   "ExternalModel"

#undef    _VERSION
#define   _VERSION   "16.01.2022"

/*--------------------------------------------------- Система команд */

  _EXTERNAL   int  __exit_flag ;             /* Флаг завершения работы */

/*---------------------------------- Коды пользовательских сообщений */

#define    _USER_SECTION_ENABLE    1
#define    _USER_RELOAD            2

/*-------------------------------------------------- Диалоговые окна */

  _EXTERNAL            UINT  TaskBar_Msg ;        /* Сообщение активизации иконы в TaskBar */
  _EXTERNAL  NOTIFYICONDATA  TbIcon ;             /* Описание TaskBar-иконки */

  _EXTERNAL       HINSTANCE  hInst ;
  _EXTERNAL        WNDCLASS  FrameWindow ;
  _EXTERNAL            HWND  hFrameWindow ;

  _EXTERNAL            HWND  __dialog ;

  _EXTERNAL             int  __window_closed ;    /* Флаг скрытия окна в трей */

  _EXTERNAL             int  __console_process ;  /* Флаг работы в консольном режиме */

#define                    _NO_ICON              0
#define               _WARNING_ICON              1
#define                 _ERROR_ICON              2

  _EXTERNAL  char  __title[FILENAME_MAX] ;        /* Заголовок окна */

/*------------------------------------------------- Общие переменные */

  _EXTERNAL            char  __cwd[FILENAME_MAX] ;              /* Рабочий раздел */

  _EXTERNAL            char  __log_path[FILENAME_MAX] ;         /* Путь файла лога */

/*-------------------------------------------------- Описание секций */

  typedef struct {
                    HWND  hWnd ;           /* Дескриптор окна */
                    char  title[128] ;     /* Заголовок */           
                 } Section ;

  _EXTERNAL      Section  __sections[10] ;       /* Список секций */
  _EXTERNAL          int  __sections_cnt ;

  _EXTERNAL          int  __sec_work ;           /* Индекс рабочий секции */

  _EXTERNAL       time_t  __sec_change_time ;    /* Время последнего переключения секций */

/*---------------------------------------- Общий управляющий процесс */

  _EXTERNAL  HANDLE  hFilesIface_Tread ;                   /* Процесс для файлового интерфейса */
  _EXTERNAL   DWORD  hFilesIface_PID ;

  _EXTERNAL  HANDLE  hTcpIface_Tread ;                     /* Процесс для сетевого интерфейса */
  _EXTERNAL   DWORD  hTcpIface_PID ;

  _EXTERNAL    HWND  hConsoleDialog ;

  _EXTERNAL    char  __control_folder[FILENAME_MAX] ;      /* Папка управляющих файлов */
  _EXTERNAL    char  __control_object[FILENAME_MAX] ;      /* Отслеживаемый объект - по умолчанию или * - все */
  _EXTERNAL    char  __targets_path[FILENAME_MAX] ;        /* Файл списка целей */

  _EXTERNAL    char  __port[FILENAME_MAX] ;                /* Контактный порт сетевого интерфейса */
  
  typedef struct {                                   /* Параметры объекта */
                      char  name[128] ;               /* Название */
                      char  type[128] ;               /* Тип */
                      char  target[128] ;             /* Цель */
                       int  start ;                   /* Признак начала моделирования */
                    double  t1 ;                      /* Период моделирования */
                    double  t2 ;
                    double  x, y, z ;                 /* Координаты объекта */
                    double  a_azim, a_elev, a_roll ;  /* Углы ориентации объекта */
                    double  v_x, v_y, v_z ;           /* Проекции скорости */

                 } Object ;

#define        _TARGETS_MAX   1000

  _EXTERNAL  Object *__targets[_TARGETS_MAX] ;        /* Список целей */
  _EXTERNAL     int  __targets_cnt ;
  _EXTERNAL  double  __targets_time ;

/*------------------------------------------------ Класс TCP-сервера */

#ifdef  __MAIN__

  typedef  struct {                        /* Описание клиента */ 
                      int   use_flag ;
                      int   close_flag ;     /* Постановка на закрытие */
                   time_t   close_time ;     /* Время закрытия */
                     char   descr[64] ;      /* Строка текстового описания */

               Tcp_client  *TCP_client ;     /* Ссылка на TCP-клиента */
                     char   ip[64] ;         /* IP-адрес клиента */
                      int   socket ;         /* Номер сокета */

                   HANDLE   thread ;         /* Идентификаторы потока обработки */
                    DWORD   thread_id ;
                      int   thread_exit ;    /* Флаг зевершения потока обработки */
                   HANDLE   hEvent ;         /* Ссылка на событие */
                  Tcp_job  *job ;            /* Данные обмена */

                   Object   object ;         /* Данные моделируемого объекта */
                      int   processed ;      /* Флаг завершения моделирования */
                      int   error ;          /* Флаг ошибки в процессе моделирования */
                     char   result[8192] ;   /* Результат расчета */

                  } SRV_client ;

  _EXTERNAL   SRV_client **__clients ;
  _EXTERNAL          int   __clients_cnt ;

  _EXTERNAL unsigned int   __process_idx ;

  class  Server_TCP : public Tcp {

      public :

       virtual int  vExternControl     (void) ;                        /* Обработчик внешнего управления */
       virtual int  vExternal          (SOCKET) ;                      /* Отработка внешних событий */ 
       virtual int  vCloseConnect      (void *) ;                      /* Закрытие канала связи */
       virtual int  vProcessData       (Tcp_job *, void *) ;           /* Обработка полученных данных */
       virtual int  vMessagesLog       (Tcp_client *,                  /* Ведение лога обмена */
                                            std::string *, char *) ;
                                                                    
      public :
                    Server_TCP() ;          /* Конструктор */
                   ~Server_TCP() ;          /* Деструктор */
                                      } ;

  _EXTERNAL     int  __IP_port ;             /* Контактный порт */

#endif

/*------------------------------------------ Параметры моделирования */

                                             /* Модель MISSILE */ 
  _EXTERNAL  double  __missile_v ;            /* Скорость полета ракеты */
  _EXTERNAL  double  __missile_g ;            /* Располагаемая перегрузка маневра */
  _EXTERNAL  double  __missile_hit ;          /* Радиус поражения БЧ */

                                             /* Модель ZRK */ 
  _EXTERNAL  double  __zrk_v ;                /* Скорость движения шасси */
  _EXTERNAL  double  __zrk_h_min ;            /* Минимальная высота полета воздушных целей */
  _EXTERNAL  double  __zrk_r_min ;            /* Минимальная дальность пуска */
  _EXTERNAL  double  __zrk_r_max ;            /* Максимальная дальность пуска */
  _EXTERNAL  double  __zrk_firing_interval ;  /* Интервал между пусками */
  _EXTERNAL    char  __zrk_missile[128] ;     /* "Шаблон" ЗУР */

/*---------------------------------------------- Хранилище контекста */

  typedef struct {                                   /* Контекст объекта */
                      char  name[128] ;               /* Название */
                    double  t1 ;                      /* Период моделирования */
                    double  x, y, z ;                 /* Координаты объекта */
                    double  x_t, y_t, z_t ;           /* Координаты цели */
                       int  missed ;                  /* Признак промаха */
                 } Context ;

#define        _CONTEXTS_MAX   100

  _EXTERNAL  Context *__contexts[_CONTEXTS_MAX] ;        /* Список контекстов объектов */
  _EXTERNAL      int  __contexts_cnt ;

/*-------------------------------------------------------- Прототипы */

/* ExternalModel.cpp */
             int  EM_system        (void) ;                         /* Обработка системных сообщений */
            void  EM_message       (char *) ;                       /* Система выдачи сообщений */
            void  EM_message       (char *, int) ;
             int  EM_log           (char *) ;                       /* Ведение лога */
             int  EM_read_config   (char *) ;                       /* Считывание файла конфигурации */
            void  EM_Change_Section(char *, int) ;                  /* Выбор секции */
            void  EM_text_trim     (char *) ;                       /* Отсечка начальных и конечных пробельных символов */
             int  EM_create_path   (char *) ;                       /* Формирование пути к разделу */
             int  EM_text_subst    (char *, char *, char *, int) ;  /* Подстановка полей данных */
             int  EM_json_subst    (char *, char *, char *) ;       /* Подстановка поля данных по ключу */
             int  EM_read_targets  (char *, int) ;                  /* Считывание файла целей */
             int  EM_read_request  (char *, Object *, int) ;        /* Считывание файла запроса */
             int  EM_read_request2 (char *, Object *, int) ;        /* Считывание файла запроса, сокращенная форма */
             int  EM_write_response(Object *, char *, char *) ;     /* Записать файл ответа */
             int  EM_process_model (Object *, char *, char *) ;     /* Расчет модели */

    DWORD WINAPI  FilesIface_Tread   (LPVOID) ;
    DWORD WINAPI  TcpIface_Tread     (LPVOID) ;
    DWORD WINAPI  Model_Process_Tread(LPVOID) ;

/* EM_console.cpp */
INT_PTR CALLBACK  EM_console_dialog(HWND, UINT, WPARAM, LPARAM) ;

/* EM_missile.cpp */
             int  EM_model_missile(Object *, char *, char *) ;      /* Расчет модели УР */

/* EM_ZRK.cpp */
             int  EM_model_ZRK(Object *, char *, char *) ;          /* Расчет модели ЗРК */
