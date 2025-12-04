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

float de2 = 10; // step delays
float de1 = 5;

int n = 3; // n iterations of stepper drive signal

// target distance and corresponding number of steps to take
int dists[] = {150,160,170,180,190,200,260,270,280,290,300,310,320,330,340,350,360,370,380,390, 400, 
              410, 420, 430, 440,450,460,470,480,490,500,510,520,530,540,550,560,570,580,600,610,620,
              630,640,650,660,670,680,690,700,710,720,730,740,750,760,770,780,790,800,810,820,830,840,
              850,860,870,880};
int steps[] = { 0,  0,  0,  0,  1,  2,  2,  3,  3,  3,  5,  5,  5,  6,  6,  6,  6,
        6,  6,  8,  8,  9,  9,  9,  10, 10, 10, 10, 11, 11, 12, 12, 13, 14,
       15, 16, 16, 17, 18, 20, 22, 22, 22, 23, 23, 24, 25, 26, 26, 26, 28,
       28, 28, 29, 29, 30, 32, 32, 35, 35, 36, 38, 39, 40};

void setup() {
  
  pinMode(DIR_PIN, OUTPUT);
  pinMode(PUL_PIN, OUTPUT);

  tof_init();

  delay(100);

   if(running) {
      if (vl53.dataReady()) {
        int16_t distance1 = vl53.distance();

        delay(50);
        int16_t distance2 = vl53.distance();

        delay(50);
        int16_t distance3 = vl53.distance();

        int16_t distance_arr[] ={distance1,distance2,distance3};
        
        int m = sizeof(distance_arr) / sizeof(distance_arr[0]);

        int16_t distance=findMedian(distance_arr,m);
        Serial.println(distance);
      
        if (distance == -1) {
          Serial.print(F("Couldn't get distance: "));
          Serial.println(vl53.vl_status);   
        }

        int dist = round10(distance);

        int len = sizeof(dists) / sizeof(dists[1]);
        delay(100);

        delay(100);
        int i=0;
        while (dists[i] <= dist){i++;
        }
// match new measurement to corresponding step count in steps[]    
        int step_count = steps[i];
        Serial.println(step_count);   


        for (int i = 0; i <= step_count; i++) { 
          delay(10+i);
          step();
          delay(3);
        }

        delay(2000+step_count*10); // delay, should make this a function of step_count to reduce unnecessary delay for small angle adjustments
        myservo.attach(4);
        trigger(); // trigger w/ servo
        delay(250);
        untrigger(); // untrigger w/ servo
        delay(450);
        for (int i = 0; i <= step_count+15; i++) { 
          delay(10+i);
          unstep();
          delay(3);
        }
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
  for (int i = 0; i <= n; i++) {
    digitalWrite(DIR_PIN,HIGH); 
    delayMicroseconds(5);
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
