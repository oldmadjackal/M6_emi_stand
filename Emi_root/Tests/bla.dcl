  typedef <destination>  {
                           double  x ;
                           double  z ;
                              int  step ;
                              int  x_direct ;
                         } ;

  <destination>  destination ;
      <unknown>  targets ;
      <unknown>  orders ;

  double  x_min, x_max, z_min, z_max ;
  double  x_step ;
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

//------------------------------- Выход в квадрат

   if(destination.step==0)
   {
        done=ToPoint(x_min, z_min, $dTime, "G", 5.) ;
     if(done) {
                 Log("Zone 0 reached") ;
                     destination.step++ ;
                     destination.x       =x_min ;
                     destination.z       =z_max ;
                     destination.x_direct= 1 ;
              }
   }
//------------------------------- Барражирование в квадрате

  else 
  {
//- - - - - - - - - - - - - - - - - -  Движение по маршруту
       done=ToPoint(destination.x, destination.z, $dTime, "G", 5.) ;
    if(done) {
                 Log("Turning point reached: " @ atos(destination.x) @ ":"  @ atos(destination.z)) ;

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

//     for(i=0 ; i<orders.count ; i++)  Log("  Order " @ atos(i) @ " > " @ atos(orders[i].x) @ ":"  @ atos(orders[i].z)) ;

//       if(orders.count)  Message("Target!") ;
    }
  }
//------------------------------- Завершение
 
     destination.StateSave("Destination") ;

return ;
