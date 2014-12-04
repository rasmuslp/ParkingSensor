#include <NewPing.h>

// Debug ?
#define DEBUG

// LEDs
#define RED_LED 2
#define GREEN_LED 3
#define LED_GND 4

// Ultrasonic sensor
#define TRIGGER_PIN 12
#define ECHO_PIN 11
#define MAX_DISTANCE 200  // Vehicle triggered

#define NEAR_DISTANCE 150 // Vehicle on approach
#define STOP_DISTANCE 10  // Vehicle should stop

#define SLOW_RATE 250 //Blinkrate when outside NEAR_DISTANCE

NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);

unsigned int cm;
boolean prevWasZero = false;

#define TRIGGER_DIFF_CM 5
#define TRIGGER_DIFF_COUNT 50
unsigned int staticCm = 0;
unsigned int staticCount = 0;

void setup() {
#ifdef DEBUG
  Serial.begin(115200);
#endif
  pinMode(RED_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(LED_GND, OUTPUT);
  digitalWrite(LED_GND, LOW);
}

void loop() {
  // Wait 50ms between pings (about 20 pings/sec). 29ms should be the shortest delay between pings.
  delay(50);
  
  // Measure distance
  cm = sonar.ping_cm();
  
#ifdef DEBUG
  Serial.print("Distance: ");
  Serial.print(cm);
  Serial.println(" cm");
#endif
  
  // Zero handling
  if (cm == 0) {
    // Input errored or outside of range
    
    if (prevWasZero) {
#ifdef DEBUG
      Serial.println("Nothing within trigger distance");
#endif
      led(RED_LED, LOW);
      delay(250);
    } else {
      prevWasZero = true;
    }
    
    return;
  }
  
  // Non-zero distance
  prevWasZero = false;
  
  // Car parked ?
  if (abs(cm-staticCm) < TRIGGER_DIFF_CM) {
    staticCount++;
    if (staticCount > TRIGGER_DIFF_COUNT) {
      // Car parked
#ifdef DEBUG
      Serial.println("Car parked");
#endif
      led(RED_LED, LOW);
      delay(250);
      return;
    }
  } else {
#ifdef DEBUG
      Serial.println("Movement");
#endif
    staticCm = cm;
    staticCount = 0;
  }

  if (cm < STOP_DISTANCE) {
    ledWithDelay(RED_LED, HIGH, 200);
  } else {
    led(RED_LED, LOW);  
    if (cm < NEAR_DISTANCE) {
      flashLed(GREEN_LED, map(cm, STOP_DISTANCE, NEAR_DISTANCE, 10, SLOW_RATE));    
    } else {
      flashLed(GREEN_LED, SLOW_RATE);
    }
  }
}

inline void flashLed(unsigned char pin, unsigned int halfDurationInMs) {
  flashLed(pin, halfDurationInMs, halfDurationInMs);
}

inline void flashLed(unsigned char pin, unsigned int durationOnInMs, unsigned int durationOffInMs) {
  ledWithDelay(pin, HIGH, durationOnInMs);
  ledWithDelay(pin, LOW, durationOffInMs);
}

inline void led(unsigned char pin, boolean state) {
  ledWithDelay(pin, state, 0);
}

inline void ledWithDelay(unsigned char pin, boolean state, unsigned int delayInMs) {
  digitalWrite(pin, state);
  delay(delayInMs);
}
