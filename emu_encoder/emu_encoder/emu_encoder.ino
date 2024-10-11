// Пины для имитации выходов энкодера
#define pinA 8
#define pinB 9
#define pinZ 10
#define pinR 11                                          // Канал для реверса направления

const int pulsesPerRevolution = 5000;                    // Количество импульсов на один оборот (например, 5000 импульсов)

int pulseCount = 0;                                      // Счётчик импульсов для канала Z

void setup() {

  pinMode(pinA, OUTPUT);
  pinMode(pinB, OUTPUT);
  pinMode(pinZ, OUTPUT);

  digitalWrite(pinA, LOW);
  digitalWrite(pinB, LOW);
  digitalWrite(pinZ, LOW);
}

void loop() {

   generateEncoderSignals();                               // Генерация импульсов на каналах A и B

  if (pulseCount >= pulsesPerRevolution) {
    pulseCount = 0;                                        // Сброс после полного оборота
    generateZSignal();                                     // Генерация сигнала Z
  }

  delay(55);                                               // Настроить для регулировки скорости
}

void generateEncoderSignals() {
  int delays = 55;
  if (digitalRead(pinR)) {                                    // Вращение по часовой стрелке (A опережает B)
    digitalWrite(pinA, HIGH);
    delay(delays);                                          // Половина периода сигнала
    digitalWrite(pinB, HIGH);
    delay(delays);
    digitalWrite(pinA, LOW);
    delay(delays);
    digitalWrite(pinB, LOW);
  } else {                                                  // Вращение против часовой стрелки (B опережает A)
    digitalWrite(pinB, HIGH);
    delay(delays);                                          // Половина периода сигнала
    digitalWrite(pinA, HIGH);
    delay(delays);
    digitalWrite(pinB, LOW);
    delay(delays);
    digitalWrite(pinA, LOW);
  }

  pulseCount++;                                             // Увеличение счётчика импульсов
}

void generateZSignal() {                                    // Имитация индекса (Z) — один импульс на оборот
  digitalWrite(pinZ, HIGH);
  delay(10);                                               
  digitalWrite(pinZ, LOW);
}
