String JSON_DATA(){
          char jsonBuffer[1024] = {0};
          int rotta = encoder0Pos/5000;
          
          DynamicJsonDocument chan1(1024);
                              chan1["rotate"]       = String(rotta);
                              chan1["count"]        = String(encoder0Pos);
                              chan1["rotation_plus"]= String(encoder0Rotations);
                              chan1["calcul"]       = String(encoder0Pos*settings.counter_coeff);
                              chan1["bat_v"]        = String(bat);
                              chan1["bat_ps"]       = String(battery);
                              chan1["coef"]         = String(settings.coeff);
                              chan1["auto"]         = stat_auto;
                              chan1["stat"]         = stat_auto2;
                              
           serializeJson(chan1, jsonBuffer, sizeof(jsonBuffer));
           
  return jsonBuffer;
}
