#include <Servo.h>

Servo myServo;

/* Parameters */
int servo_forward_limit = 10;
int servo_midpoint = 95;
int servo_reverse_limit = 175;

int pause_time_at_target = 1000;

int servo_movement_delay_ms = 20; // time between moving by single degree
unsigned int debounce_delay_ms = 50;

/* I/O Ports */
int startPin = 8;
int resetPin = 9;
int modePin = 10;
int servoPin = 7;
int modeOneLedPin = 11;
int modeZeroLedPin = 12;

int ledPin=4;


int inputPorts[3] = {startPin, resetPin, modePin};
int START_BUTTON = 0;
int RESET_BUTTON = 1;
int MODE_TOGGLE = 2;

/* State */
int is_running = 0;
int current_mode = 0; // forward 0, backwards 1
unsigned int last_mode_switch = 0;

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
  pinMode(startPin, INPUT);
  pinMode(resetPin, INPUT);
  pinMode(modePin, INPUT);
  pinMode(servoPin, OUTPUT);
  pinMode(modeOneLedPin, OUTPUT);
  pinMode(modeZeroLedPin, OUTPUT);

  // Start with mode = 0
  digitalWrite(modeZeroLedPin, HIGH);
  digitalWrite(modeOneLedPin, LOW);
}


void loop() {

  int mode_signal = digitalRead(modePin);

  // If mode toggle is HIGH, switch modes
  if (mode_signal == HIGH) {
    current_mode = (current_mode == 0) ? 1 : 0;
    if (current_mode == 1) {
      digitalWrite(modeOneLedPin, HIGH);
      digitalWrite(modeZeroLedPin, LOW);
    } else {
      digitalWrite(modeZeroLedPin, HIGH);
      digitalWrite(modeOneLedPin, LOW);
    }
    delay(500);
  }

  int start_signal = digitalRead(startPin);
  int reset_signal = digitalRead(resetPin);
  
  Serial.println(start_signal);
  Serial.println(reset_signal);
  Serial.println();
  
  /****** ACTION DECISION & EXECUTION ******/
  if (!is_running && reset_signal == HIGH) move_to(servo_midpoint);
  else if (!is_running && start_signal == HIGH) {
    if (current_mode == 0){
      move_to(servo_forward_limit);
      delay(pause_time_at_target);
      move_to(servo_midpoint);
    } else if (current_mode == 1) {
      move_to(servo_reverse_limit);
      delay(pause_time_at_target);
      move_to(servo_midpoint);
    }
  }
}
