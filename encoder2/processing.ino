void get_data_bt(String text){                                                      // получение параметров от мобильного приложения
          if(text.length() > 1){
              StaticJsonDocument<200> doc;
               deserializeJson(doc, text);
              String key = doc["key"];
              if(key == "control"){                                                  // Выполняем условие в зависимости от ключа 
                   String temp = doc["start"];
                   if(temp == "START"){     auto_measuring = true;  }                // Запуск автозамера
                   else if(temp == "STOP"){ auto_measuring = false; }                // Остановка атозамера
                   else if(temp == "RESET"){                                         // Сброс счетчика
                            encoder0Pos = 0;
                      encoder0Rotations = 0;
                    }
               
              }else if(key == "save"){                                               // Сохранение расчетного коэффициента
                  String coeff = doc["coeff"];
                  settings.counter_coeff = coeff.toFloat();
                  strncpy(settings.coeff, coeff.c_str(), MAX_STRING_LENGTH);
                  save_config();        
              }
       }         
}

void mesuring_data_send(){
    if(encoder0PosARH != encoder0Pos or stat_led or bat_arch != bat or coeficient_arh != settings.counter_coeff or auto_measuring_arch != auto_measuring or auto_start_arh != auto_start){
            
            encoder0PosARH     = encoder0Pos;
            bat_arch           = bat;
            coeficient_arh     = settings.counter_coeff;
            auto_measuring_arch = auto_measuring;
            auto_start_arh     = auto_start;
            
         if(auto_measuring){ stat_auto = "1"; } else { stat_auto = "0"; }
         if(auto_start){ stat_auto2= "start"; } else { stat_auto2 = "finish"; }
         if(stat_connect){ change_charact(JSON_DATA()); }
       }
  }

void led_stat(){                                                           // Индикатор подключения 
    if(!stat_led){
     if (millis() - timeModeSwitch > 1000) {
          digitalWrite(LED, !digitalRead(LED));
          timeModeSwitch = millis();
         }      
       }else{
          digitalWrite(LED, HIGH);
      }
  }  

  void battery_mon(){                                                       // Уровень заряда аккумулятора
    
      if (millis() - timeModeSwitch2 > 10000 or timeModeSwitch2 == 0) {
          int anbat = map(analogRead(BAT), 0, 4095, 0, 420);
                bat = anbat*0.01;
           int bata = bat*100;
            battery = map(bata, 311, 419, 0, 100);
            timeModeSwitch2 = millis();
       } 
  }

void st_sensor_p(){                                                        // Контроль автоматического замера
  if(auto_measuring){
     buttonState = digitalRead(st_pin);
     if(buttonPushCounter==0){
        encoder0Pos = 0;
        encoder0Rotations =0;
     }
     
    if(buttonState != lastButtonState) { 
         if (buttonState == HIGH) {
            if(buttonPushCounter == 0){
               timeSensor = millis();
               }
             if(buttonPushCounter == 0 or millis() - timeSensor > 1500  ){
                buttonPushCounter++; 
              }
          }         
       }  
       
     if(buttonPushCounter == 1 ){
                auto_start = true;
              }else{
                auto_start = false;              
              }
          lastButtonState = buttonState;
       }else {
          buttonPushCounter = 0;
       }
}

void change_charact(String data){                                  // Записываем данные в характиристику
        if (millis() - timeSend > 1 or timeSend == 0) {
            pCharacteristic->setValue(data.c_str());
            pCharacteristic->notify();
            timeSend = millis();
       } 
}
