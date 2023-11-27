#include <Wire.h>
#include <DHT11.h>
#include <Stepper.h>

#define RAINDROPS_PIN A0
#define LIMIT_PIN_1 12
#define LIMIT_PIN_2 13

// #define DEBUG

const int stepsPerRevolution = 2048;
Stepper myStepper(stepsPerRevolution, 2,4,3,5);

// Sensors
DHT11 dht11(7); // DHT 라이브러리의 DHT 객체 생성, 핀 및 센서 종류 설정

volatile int current = 0;
volatile int humi_temp_rain[9] = {0,};
volatile char op = 0;

void receiveEvent(int howMany) {
  while (0 < Wire.available()) {
    op = Wire.read();
  }
}

bool a = false;

void requestEvent() {
  if (op == 1) {
    // 모터 켜기
    Serial.println("Motor On!");
    a = true;
    Wire.write(0);
  } else if (op == 2) {
    // for (int i = 0; i < 18; i++) {
    //   Wire.write(((char *)humi_temp_rain)[i]);
    // }
    int readCurrent = current;
    int readCurrentFor = 3;
    while (readCurrentFor-- > 0) {
      Wire.write((char *)(humi_temp_rain + readCurrent), 2);
      Wire.write((char *)(humi_temp_rain + readCurrent + 3), 2);
      Wire.write((char *)(humi_temp_rain + readCurrent + 6), 2);
      readCurrent++;
      readCurrent %= 3;
    }
    // 값 가져오기
    
    // Wire.write(humi_temp_rain, (sizeof float) * 9);
  } else {
    Wire.write(0);
  }
}

void setup() {
  Wire.begin(8);                /* join i2c bus with address 8 */
  Wire.onReceive(receiveEvent); /* register receive event */
  Wire.onRequest(requestEvent); /* register request event */
  Serial.begin(9600);           /* start serial for debug */
  pinMode(A0, INPUT);
  myStepper.setSpeed(15);
  pinMode(LIMIT_PIN_1, INPUT_PULLUP);
  // pinMode(LIMIT_PIN_2, INPUT_PULLUP);
}

void loop() {
  // if (temp != DHT11::ERROR_CHECKSUM && temp != DHT11::ERROR_TIMEOUT &&
  //     humi != DHT11::ERROR_CHECKSUM && humi != DHT11::ERROR_TIMEOUT)    
  //   Serial.print("Error: ");
  //   Serial.println(DHT11::getErrorString(temp));
  //   Serial.println(DHT11::getErrorString(humi));
  humi_temp_rain[current] = (int) dht11.readHumidity();
  humi_temp_rain[current + 3] = (int) dht11.readTemperature();
  humi_temp_rain[current + 6] = (int) analogRead(RAINDROPS_PIN);
#ifdef DEBUG
  Serial.print("Humi: ");
  Serial.println(humi_temp_rain[current]);
  Serial.print("Temp: ");
  Serial.println(humi_temp_rain[current + 3]);
  Serial.print("Rain: ");
  Serial.println(humi_temp_rain[current + 6]);
#endif
  delay(2000);
  current++;
  current %= 3;
  while (a) {
    a = false;
    myStepper.step(stepsPerRevolution);
  }
}
