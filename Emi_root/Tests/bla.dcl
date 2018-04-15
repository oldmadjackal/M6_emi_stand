  typedef <data>  {
                     double  x ;
                     double  z ;
                        int  step ;
                        int  x_direct ;
                  } ;

      <data>  data ;

  double  x_min, x_max, z_min, z_max ;
  double  x_step ;
  double  done ;

//------------------------------- Конфигурация

    x_min =(-10000.) ;
    x_max =  10000. ;
    z_min =      0. ;
    z_max =  20000. ;
    x_step=   5000. ;

//------------------------------- Подготовка

  if($Time<5)  return ;

     data.StateRead() ;

//------------------------------- Выход в квадрат

   if(data.step==0)
   {
        done=ToPoint(x_min, z_min, $dTime) ;
     if(done) {
                 Log("Zone 0 reached") ;
                     data.step++ ;
                     data.x       =x_min ;
                     data.z       =z_max ;
                     data.x_direct= 1 ;
              }
   }
//------------------------------- Барражирование в квадрате

  else 
  {
       done=ToPoint(data.x, data.z, $dTime) ;
    if(done) {
                 Log("Turning point reached: " @ atos(data.x) @ ":"  @ atos(data.z)) ;

        if(data.step==1)
        {
           data.step=2 ;

            if(data.x==x_min)  data.x_direct=  1 ;
            if(data.x==x_max)  data.x_direct=(-1) ;
                               data.x       =data.x+data.x_direct*x_step ;
        }
        else
        {
           data.step=1 ;

            if(data.z==z_min)  data.z=z_max ;
            else               data.z=z_min ; 
        }

             }
  }
//------------------------------- Завершение
 
     data.StateSave() ;

return ;
