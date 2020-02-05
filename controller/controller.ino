#include <Servo.h>

Servo myServo;

/* Parameters */
int servo_forward_limit = 10;
int servo_midpoint = 90;
int servo_reverse_limit = 170;

int pause_time_at_target = 1000;

int servo_movement_delay_ms = 20; // time between moving by single degree
unsigned int debounce_delay_ms = 50;

/* I/O Ports */
int startPin = 1;
int resetPin = 2;
int modePin = 3;
int servoPin = 7;

int inputPorts[3] = {startPin, resetPin, modePin};
int START_BUTTON = 0;
int RESET_BUTTON = 1;
int MODE_TOGGLE = 2;

/* State */
int is_running = 0;
int current_mode = 0; // forward 0, backwards 1

int inputValues[3] = {LOW, LOW, LOW};
int lastInputStates[3] = {LOW, LOW, LOW};
int lastDebounceTimes[3] = {0, 0, 0};

int servo_pos_deg = 0; // store current position


// Move servo arm to specified position
void move_to(int target) {
  is_running = true;
  
  while (servo_pos_deg != target) {
    if (servo_pos_deg < target) {
      myServo.write(++servo_pos_deg);
    } else {
      myServo.write(--servo_pos_deg);
    }
    delay(servo_movement_delay_ms);
  }

  is_running = false;
}


void setup() {
  myServo.attach(servoPin);
  move_to(servo_midpoint);
}


void loop() {

  /****** INPUT READING ******/
  for (int i = 0; i < 3; i++) {
    int reading = digitalRead(inputPorts[i]);
    
    if (reading != lastInputStates[i]) {
      //reset the debouncing timer
      lastDebounceTimes[i] = millis();
    }
    
    if ((millis() - lastDebounceTimes[i]) > debounce_delay_ms) {
      // whatever the reading is at, it's been there for longer than the debounce
      // delay, so take it as the actual current state
      if (reading != inputValues[i]) inputValues[i] = reading;
    }
    
    lastInputStates[i] = reading;
  }
  current_mode = inputValues[MODE_TOGGLE];

  /****** ACTION DECISION & EXECUTION ******/
  if (!is_running && inputValues[RESET_BUTTON]) move_to(servo_midpoint);
  else if (!is_running && inputValues[START_BUTTON]) {
    if (current_mode == 0){
      move_to(servo_forward_limit);
      delay(pause_time_at_target);
      move_to(servo_midpoint);
    } else {
      move_to(servo_reverse_limit);
      delay(pause_time_at_target);
      move_to(servo_midpoint);
    }
  }
}
