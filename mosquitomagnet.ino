// Pin Definitions
const int FAN_PIN = 4;
const int IGNITER_PIN = 7;
const int SOLENOID_PIN = 8;
const int LED_PIN = 13;
const int THERMISTOR_PIN = A1;


// Thresholds for thermistor voltage
const float MIN_VOLTAGE = 2.0;
const float MAX_VOLTAGE = 3.0;


// Functions for measuring elapsed seconds
unsigned long timerStart;
void timerReset() {
  timerStart = millis() / 1000;
}

bool timer(int s) { // True if 's' seconds since last reset
  unsigned long timeNow = millis() / 1000;
  return timeNow > timerStart + s;
}


// Blinking the LED
int ledDelay = 500;
unsigned long ledTimer = millis();
bool led = false;
void blink() {
  if (millis() > ledTimer + ledDelay) {
    ledTimer = millis();
    led = !led;
    digitalWrite(LED_PIN, led);
  }
}


// States
enum {
  IGNITER_WAIT,
  SOLENOID_WAIT,
  FAN_WAIT,
  EVERYTHING_STARTED,
  SOLENOID_SHUTDOWN,
  IGNITER_SHUTDOWN,
  FAN_SHUTDOWN,
  EVERYTHING_STOPPED
};

unsigned char state = IGNITER_WAIT;


void setup() {
  pinMode(FAN_PIN, OUTPUT);
  pinMode(IGNITER_PIN, OUTPUT);
  pinMode(SOLENOID_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  timerReset();
}


void loop() {
  switch (state) {
    case IGNITER_WAIT:
      if (timer(15)) {
        digitalWrite(IGNITER_PIN, HIGH);
        state = SOLENOID_WAIT;
      }
      break;
    case SOLENOID_WAIT:
      if (timer(17)) { 
        digitalWrite(SOLENOID_PIN, HIGH);
        state = FAN_WAIT;
      }
      break;
    case FAN_WAIT:
      if (timer(27)) {
        digitalWrite(FAN_PIN, HIGH);
        ledDelay = 1000;
        state = EVERYTHING_STARTED;
      }
      break;
    case EVERYTHING_STARTED:
      if (timer(600)) { // Lets 10 minutes pass befor checking thermistor
          float voltage = analogRead(THERMISTOR_PIN) * 5.0 / 1023.0;
          if (voltage < MIN_VOLTAGE || voltage > MAX_VOLTAGE) {
              state = SOLENOID_SHUTDOWN;
              ledDelay = 300;
          }
      }
      break;
    case SOLENOID_SHUTDOWN:
      timerReset(); 
      digitalWrite(SOLENOID_PIN, LOW);
      state = IGNITER_SHUTDOWN;
      break;
    case IGNITER_SHUTDOWN:
      if (timer(10)) {
        digitalWrite(IGNITER_PIN, LOW);
        state = FAN_SHUTDOWN;
      }
      break;
    case FAN_SHUTDOWN:
      if (timer(130)) {
        digitalWrite(FAN_PIN, LOW);
        state = EVERYTHING_STOPPED;
        ledDelay = 2000;
      }
      break;
    case EVERYTHING_STOPPED:
      break;
  }
  blink();  
}