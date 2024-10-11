void check_clean(){                             // Функция очистки памяти при первой прошивке
       if(settings.statteeprom != 15){ 
          for (int i = 0; i < 4096; ++i) {
              EEPROM.write(i, 0); 
            }      
              EEPROM.commit();
             settings.statteeprom = 15;
        }
   }
   

void read_config(){                             // Функция чтения конфигурации из EEPROM
        EEPROM.get(0, settings);
   }

   
void save_config(){                             // Функция записи конфигурации в EEPROM
        EEPROM.begin(sizeof(settings));
        EEPROM.put(0, settings);                // Сохранение конфигурации в EEPROM 
        EEPROM.commit();
   }
