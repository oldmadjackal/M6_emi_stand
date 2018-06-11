  typedef <destination>  {
                           double  x ;
                           double  z ;
                           double  azim ;
                              int  step ;
                              int  x_direct ;
                              int  elev ;
                           double  time ;
                         } ;

  <destination>  destination ;
      <unknown>  targets ;
      <unknown>  orders ;
      <unknown>  xyz ;

  double  x_min, x_max, z_min, z_max ;
  double  x_step ;
  double  distance ;
  double  done ;
     int  i ;

//------------------------------- Конфигурация

    x_min =(-10000.) ;
    x_max =  10000. ;
    z_min =      0. ;
    z_max =  20000. ;
    x_step=   5000. ;

//------------------------------- Подготовка

  if($Time<5)  return ;

     destination.StateRead("Destination") ;

//------------------------------- Контроль высоты

     if($ThisY<300. && destination.elev<0.)  destination.elev=0. ;

     if(destination.elev>$ThisElev) $ThisElev=$ThisElev+2 ;
     if(destination.elev<$ThisElev) $ThisElev=$ThisElev-2 ;

//------------------------------- Уход от цели

  if(destination.step==7)
  {
         done=Turn("A", destination.azim, $dTime, "G", 7.) ;
      if(done) {
                   Battle("exec la/q tank1;exec zoom 800") ;
               }
                      return ;
  }
//------------------------------- Выход в квадрат

  else 
  if(destination.step==0)
  {
        done=ToPoint(x_min, z_min, $dTime, "G", 7.) ;
     if(done) {
                 Log("Zone 0 reached") ;
                     destination.step++ ;
                     destination.x       =x_min ;
                     destination.z       =z_max ;
                     destination.x_direct= 1 ;
                 Battle("exec zoom 200") ;
              }

     destination.StateSave("Destination") ;
                return ;
  }
//------------------------------- Барражирование в квадрате

  else 
  if(destination.step<=2)
  {
//- - - - - - - - - - - - - - - - - -  Движение по маршруту
       done=ToPoint(destination.x, destination.z, $dTime, "G", 7.) ;
    if(done) {
                 Log("Достигнута маршрутная точка: " @ atos(destination.x) @ ":"  @ atos(destination.z)) ;

        if(destination.step==1)
        {
           destination.step=2 ;

            if(destination.x==x_min)  destination.x_direct=  1 ;
            if(destination.x==x_max)  destination.x_direct=(-1) ;
                                      destination.x       =destination.x+destination.x_direct*x_step ;
        }
        else
        {
           destination.step=1 ;

            if(destination.z==z_min)  destination.z=z_max ;
            else                      destination.z=z_min ; 
        }
             }
//- - - - - - - - - - - - - - - - - -  Идентификация цели
       targets.GetTargets ("radar") ;

    if(targets.count>0)
    {
        orders.DetectOrder(targets, 100) ;

     for(i=0 ; i<orders.count ; i++)  Log("  Target " @ atos(i) @ " > " @ atos(orders[i].x) @ " : "  @ atos(orders[i].z) @ " : "  @ atos(orders[i].azim)) ;

       if(orders.count) 
       {
                 Log("Обнаружена групповая цель") ;

          xyz.GetGlobalXYZ(orders[0].x, 0., orders[0].z) ;

           destination.step= 3 ;
           destination.time=$Time ;
           destination.x   =xyz.x ;
           destination.z   =xyz.z ;
       }
    }

       destination.StateSave("Destination") ;
                     return ;
  }
//------------------------------- Сбор информации о цели

  else 
  if(destination.step==3)
  {

       done=ToPoint(destination.x, destination.z, $dTime, "G", 7.) ;

     if($Time>destination.time+5.)
     {
                 Log("Выход на боевой курс...") ;

       targets.GetTargets ("radar") ;
        orders.DetectOrder(targets, 100) ;
           xyz.GetGlobalXYZ(orders[0].x, 0., orders[0].z) ;

           destination.step=  4 ;
           destination.x   = xyz.x ;
           destination.z   = xyz.z ;
           destination.azim=orders[0].azim+$ThisAzim ;
           destination.elev= (-30) ;
     }

       destination.StateSave("Destination") ;
                     return ;
  }
//------------------------------- Выход вдоль оси цели

  else 
  if(destination.step==4)
  {

       done=ToLine(destination.x, destination.z, destination.azim, $dTime, "G", 7.) ;
    if(done)
    {
         Message("1") ;
         Log("Боевой курс") ;
           destination.step= 5 ;
    }

       destination.StateSave("Destination") ;
                     return ;
  }
//------------------------------- Выход точку сброса

  else 
  if(destination.step==5 || destination.step==6)
  {

       targets.GetTargets ("radar") ;
        orders.DetectOrder(targets, 100) ;

    if(orders) {
                                   xyz.GetGlobalXYZ(orders[0].x, 0., orders[0].z) ;
                     destination.x=xyz.x ;
                     destination.z=xyz.z ;
               }

       done=ToPoint(destination.x, destination.z, $dTime, "G", 7.) ;

       distance=Distance(destination.x, destination.z) ;
    if(distance<2200)
    {
      if(destination.step==5)
      {
             Log("Сброс 1!") ;

          Battle("start kas1") ;

           destination.step= 6 ;
           destination.time=$Time ;
      }
      else 
      {
        if($Time>destination.time+0.5)
        {
             Log("Сброс 2!") ;

          Battle("start kas2") ;

         Log("Уход от цели!") ;

           destination.step= 7 ;
           destination.azim=$ThisAzim+120. ;
           destination.elev= (-30) ;
        }
      }
    }
    else if(distance<4000)
    {
      if($ThisY>400.)  destination.elev= 0 ;
      else             destination.elev=30 ;       
    }
  }
//------------------------------- Завершение
 
     destination.StateSave("Destination") ;

return ;
