#include <Servo.h>
#include "Adafruit_VL53L1X.h"

// tof 
#define IRQ_PIN 8
#define XSHUT_PIN 7

Adafruit_VL53L1X vl53 = Adafruit_VL53L1X(XSHUT_PIN, IRQ_PIN);

// stepper
#define DIR_PIN 12
#define PUL_PIN 13

// servo

Servo myservo;

bool running = true;

float de2 = 1000;
float de1 = 250;

int n = 1;

int dists[] = {760,770,780,790,800,810,820,830,840,850,860,870,880,890,900,910,920,930,940,950,960,970,980,990,1000,1010,1020,1030};
int steps[] = {4,  5,  6,  7,  8,  12,  18,  20,  25,  25,  25,  25,  26,  29,  28,  28,  20,  20,  22,  25,  25,  25,  30,  30,  30,  40,  40,  40};

void setup() {
  
  pinMode(DIR_PIN, OUTPUT);
  pinMode(PUL_PIN, OUTPUT);

  tof_init();

  delay(100);

   if(running) {
      if (vl53.dataReady()) {
        int16_t distance = vl53.distance();
        Serial.print(distance);
        
        if (distance == -1) {
          Serial.print(F("Couldn't get distance: "));
          Serial.println(vl53.vl_status);   
        }

        int dist = round10(distance);
        int l = sizeof(dists<distance) / sizeof(dists[0]);
        int step_count = steps[l];

        for (int i = 0; i <= step_count; i++) {
          delay(5+i);
          step();
          delay(40);
        }

        delay(5000);
        myservo.attach(4);
        trigger();
        delay(250);
        untrigger();
      }
   }
}


void tof_init() {
  Serial.begin(115200);

  while (!Serial) delay(10);
  Wire.begin();
  if (! vl53.begin(0x29, &Wire)) {
    Serial.print(F("Error on init of VL sensor: "));
    Serial.println(vl53.vl_status);
    while (1)       delay(10);
  }
  if (! vl53.startRanging()) {
    Serial.print(F("Couldn't start ranging: "));
    Serial.println(vl53.vl_status);
    while (1)       delay(10);
  }
  Serial.println("Ranging started");
  vl53.setTimingBudget(50);
}

void unstep() {
  digitalWrite(DIR_PIN,LOW); 
  delayMicroseconds(5);

  for (int i = 0; i <= n; i++) {
    digitalWrite(PUL_PIN,HIGH);
    delayMicroseconds(de1);
    digitalWrite(PUL_PIN,LOW);
    delayMicroseconds(de2);
  }
}

void step() {
  for (int i = 0; i <= n; i++) {
    digitalWrite(DIR_PIN,HIGH);
    delayMicroseconds(5);
    digitalWrite(PUL_PIN,HIGH);
    delayMicroseconds(de1);
    digitalWrite(PUL_PIN,LOW);
    delayMicroseconds(de2);
  }
}

int round10(int num){
  int len = log10(num);
  float div = pow(10, len);
  int rounded = ceil(num / div) * div;
  return rounded;
}

void trigger() {
  myservo.write(0);
}

void untrigger() {
  myservo.write(180);
}

void loop() {

}
