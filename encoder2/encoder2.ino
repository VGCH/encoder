/* Микро ПО модуля устройста измерения длины окружности
 * 
 * CYBEREX TECH
 * 2018/2024
 * 
 */
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <EEPROM.h>
#include <ArduinoJson.h>

BLEServer *pServer = NULL;
BLECharacteristic *pCharacteristic = NULL;

#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"


#define encoder0PinA 14                 // Вход А 
#define encoder0PinB 17                 // Вход B 2
#define encoder0PinZ 4                  // Вход Z
#define st_pin       18                 // Пин сенсора автозмера
#define LED_ST       12                 // Пин светодиода
#define LED          32                 // Пин светодиода
#define BAT          34                 // Пин сенсор АКБ

volatile long encoder0Pos       = 0;  // Переменная в обработчике прерываний по подсчету импульсов
volatile long encoder0PosARH    = 0;  // Переменная в обработчике прерываний по подсчету импульсов
volatile long encoder0Rotations = 0;  // Переменная по подсчету оборотов.
volatile int direction          = 0;  // Направление вращения: 1 = CW, -1 = CCW

bool stat_led;
bool stat_connect;
float bat      = 0.0;
float bat_arch = 0.0;
int battery          = 0;
long timeModeSwitch  = 0;
long timeModeSwitch2 = 0;
long timeSensor      = 0;
long timeSend        = 0;
String stat_auto, stat_auto2;
float coeficient_arh;
bool auto_measuring;
bool auto_measuring_arch;
bool auto_start;
bool auto_start_arh;
int buttonPushCounter = 0;   // счётчик 
int buttonState       = 0;   // текущее состояние 
int lastButtonState   = 0;   // предыдущее состояние 

//BLE калбеки 

void get_data_s(String str){
   get_data_bt(str);
}

class MyServerCallbacks : public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
     stat_led     = true;
     stat_connect = true;                          // Если устройство подключено
    }

    void onDisconnect(BLEServer* pServer) {
      stat_connect = false;                        // Если устройство отключено
      stat_led     = false;
    }
};

class MyCharacteristicCallbacks : public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
        std::string rxValue = pCharacteristic->getValue();
        if (!rxValue.empty()) {
            get_data_s(rxValue.c_str());          // Обработка полученных данных
        }
    }
};

// Функции обработки прерывания

void IRAM_ATTR doEncoderZ(){                                           // Обработчик прерывания для канала Z
  
    if(auto_measuring && auto_start or !auto_measuring){ //если запущен автозамер
    if(digitalRead(encoder0PinZ)){
         encoder0Rotations = encoder0Rotations +1;
      }
   }
}
 

void IRAM_ATTR doEncoderA() {                                           // Обработчик прерывания для канала A
  if(auto_measuring && auto_start or !auto_measuring){
       if (digitalRead(encoder0PinA) == digitalRead(encoder0PinB)) {    // Определяем направление вращения по состоянию канала B
              direction = -1;                                           // Вращение против часовой стрелки
            } else {
              direction = 1;                                            // Вращение по часовой стрелке
       }
  encoder0Pos += direction;
  }
}

#define MAX_STRING_LENGTH 30                                   // Максимальное количество символов для хранения в конфигурации

struct {                                                       // Структура для хранения конфигурации устройства
     int     statteeprom; 
     float   counter_coeff;
     char    coeff[MAX_STRING_LENGTH];
 
  } settings;


void setup() {
  EEPROM.begin(sizeof(settings)); 
  read_config();                                               // Чтение конфигурации из EEPROM
  check_clean();                                               // Очистка EEPROM при первом запуске 
  pinMode(encoder0PinA, INPUT);                                // Установка режима пина фазы A         
  pinMode(encoder0PinB, INPUT);                                // Установка режима пина фазы B
  pinMode(encoder0PinZ, INPUT);                                // Установка режима пина фазы Z
  pinMode(st_pin, INPUT); 
  pinMode(LED, OUTPUT);
  pinMode(LED_ST, OUTPUT);
  digitalWrite(LED, HIGH);
  ble_ini();                                                   // BLE Инициализация
  attachInterrupt(encoder0PinA, doEncoderA, CHANGE);           // Функция обработки внешнего прерывания фазы А
  attachInterrupt(encoder0PinZ, doEncoderZ, RISING);           // Функция обработки внешнего прерывания фазы Z
}


  
void loop (){
  digitalWrite(LED_ST, digitalRead(st_pin));                   // Индикация срабатывания датчика автозамера
  battery_mon();                                               // Мониторинг аккумулятора
  st_sensor_p();                                               // Обработка сингала сенсора автозамера
  mesuring_data_send();                                        // Обработка измерений
  led_stat();                                                  // Индикация BLE подключения (мигает - откл, горит - подкл.)
}
