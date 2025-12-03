#include <Servo.h>
#include "Adafruit_VL53L1X.h"

// tof pins
#define IRQ_PIN 8
#define XSHUT_PIN 7

Adafruit_VL53L1X vl53 = Adafruit_VL53L1X(XSHUT_PIN, IRQ_PIN);

// stepper pins
#define DIR_PIN 12
#define PUL_PIN 13

// servo
Servo myservo;

bool running = true;

float de2 = 1000; // step delays
float de1 = 250;

int n = 1; // n iterations of stepper drive signal

// target distance and corresponding number of steps to take
int dists[] = {150,160,170,180,190,200,260,270,280,290,300,310,320,330,340,350,360,370,380,390, 400, 410, 420, 430, 440,450,460,470,480,490,500,510,520,530,540,550,560,570,580,600,610,620,630,640,650,660,670,680,690,700,710,720,730,740,750,760,770,780,790,800,810,820,830,840,850,860,870,880};
int steps[] = {0, 0, 0, 0, 0, 0, 3, 3, 3, 3, 6, 6, 6, 6, 6, 6, 6, 6, 6,  9,  9,  9,  9,  9,  9, 12, 12, 12, 12, 12, 15, 15, 15, 15, 15, 18, 18, 18, 18, 18, 18, 21, 21, 21, 21, 21, 21, 24, 24, 24, 24,24, 24, 27, 27, 27, 27, 27, 30, 30, 30, 30, 30, 30};

void setup() {
  
  pinMode(DIR_PIN, OUTPUT);
  pinMode(PUL_PIN, OUTPUT);

  tof_init();

  delay(100);

   if(running) {
      if (vl53.dataReady()) {
        int16_t distance1 = vl53.distance();
        Serial.println(distance1);
        delay(50);
        int16_t distance2 = vl53.distance();
        Serial.println(distance2);
        delay(50);
        int16_t distance3 = vl53.distance();
        Serial.println(distance3);

        int16_t distance_arr[] ={distance1,distance2,distance3};
        
        int m = sizeof(distance_arr) / sizeof(distance_arr[0]);

        int16_t distance=findMedian(distance_arr,m);
        Serial.println(distance);
      
        if (distance == -1) {
          Serial.print(F("Couldn't get distance: "));
          Serial.println(vl53.vl_status);   
        }

        int dist = round10(distance);
        int l = sizeof(dists<distance) / sizeof(dists[0]); // match new measurement to corresponding step count in steps[]
        int step_count = steps[l];

        for (int i = 0; i <= step_count; i++) { // make step_count number of steps
          delay(5+i);
          step();
          delay(40);
        }

        delay(5000); // delay, should make this a function of step_count to reduce unnecessary delay for small angle adjustments
        myservo.attach(4);
        trigger(); // trigger w/ servo
        delay(250);
        untrigger(); // untrigger w/ servo
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
    digitalWrite(DIR_PIN,LOW);
    delayMicroseconds(5);
    digitalWrite(PUL_PIN,HIGH);
    delayMicroseconds(de1);
    digitalWrite(PUL_PIN,LOW);
    delayMicroseconds(de2);
  }
}

int round10(int num){ // round to the nearest power of 10
  int len = log10(num);
  float div = pow(10, len);
  int rounded = ceil(num / div) * div;
  return rounded;
}

double findMedian(int arr[], int m)
{

    // first sort the array
    int16_t sort(arr, arr + m);

    // Check if the number is even or odd
    if (m % 2 != 0) {
        // If number of element is odd, return the middle
        // element
        return arr[m / 2];
    }
    else {
        // If number of element is even, return the average
        // of the two middle elements
        return (arr[(m - 1) / 2] + arr[m / 2]) / 2.0;
    }
}

void trigger() {
  myservo.write(0);
}

void untrigger() {
  myservo.write(180);
}

void loop() {

}
