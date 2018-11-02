#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

#define PORT_BASE 0
#define PORT_LOWER 1
#define PORT_UPPER 2
#define SERVO_BASE_MIN  150 // this is the 'minimum' pulse length count (out of 4096)
#define SERVO_BASE_MAX  572
#define SERVO_LOWER_MIN  200
#define SERVO_LOWER_MAX  572
#define SERVO_UPPER_MIN  150
#define SERVO_UPPER_MAX  572

int currentBaseDegree;
int currentLowerDegree;
int currentUpperDegree;
boolean isEmbarked = true;
int zoomInCount = 0;
int zoomOutCount = 0;


Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

void setup() {
  Serial.begin(9600);
  pwm.begin();
  pwm.setPWMFreq(60);  // Analog servos run at ~60 Hz updates
  setBaseServoToInitialPosition();
  setLowerServoToInitialPosition();
  setHigherServoToInitialPosition();
  yield();
}

void setServoToDegree(int port, int degree, uint16_t SERVOMIN, uint16_t SERVOMAX) {
  uint16_t pulselength = map(degree, 0, 180, SERVOMIN, SERVOMAX);
  pwm.setPWM(port, 0, pulselength);
  delay(50);
}

void setBaseServoToInitialPosition() {
  setServoToDegree(PORT_BASE, 180, SERVO_BASE_MIN, SERVO_BASE_MAX);
  currentBaseDegree = 180;
}

void setLowerServoToInitialPosition() {
  setServoToDegree(PORT_LOWER, 0, SERVO_LOWER_MIN, SERVO_LOWER_MAX);
  currentLowerDegree = 0;
}

void setHigherServoToInitialPosition() {
  setServoToDegree(PORT_UPPER, 180, SERVO_UPPER_MIN, SERVO_UPPER_MAX);
  currentUpperDegree = 180;
}

void disembarkLapka() {
  for (int i = 0; i < 45; i++) {
    currentLowerDegree = currentLowerDegree + 1;
    setServoToDegree(PORT_LOWER, currentLowerDegree, SERVO_LOWER_MIN, SERVO_LOWER_MAX);
    currentUpperDegree = currentUpperDegree - 1;
    setServoToDegree(PORT_UPPER, currentUpperDegree, SERVO_UPPER_MIN, SERVO_UPPER_MAX);
  }
  for (int i = 0; i < 35; i++) {
    currentLowerDegree = currentLowerDegree + 1;
    setServoToDegree(PORT_LOWER, currentLowerDegree, SERVO_LOWER_MIN, SERVO_LOWER_MAX);
    currentUpperDegree = currentUpperDegree - 2;
    setServoToDegree(PORT_UPPER, currentUpperDegree, SERVO_UPPER_MIN, SERVO_UPPER_MAX);
  }
  isEmbarked = false;
}

void embarkLapka() {
  while (zoomOutCount != 0) {
    zoomIn();
  }
  while (zoomInCount != 0) {
    zoomOut();
  }
  for (int i = 0; i < 45; i++) {
    currentLowerDegree = currentLowerDegree - 1;
    setServoToDegree(PORT_LOWER, currentLowerDegree, SERVO_LOWER_MIN, SERVO_LOWER_MAX);
    currentUpperDegree = currentUpperDegree + 1;
    setServoToDegree(PORT_UPPER, currentUpperDegree, SERVO_UPPER_MIN, SERVO_UPPER_MAX);
  }
  for (int i = 0; i < 35; i++) {
    currentLowerDegree = currentLowerDegree - 1;
    setServoToDegree(PORT_LOWER, currentLowerDegree, SERVO_LOWER_MIN, SERVO_LOWER_MAX);
    currentUpperDegree = currentUpperDegree + 2;
    setServoToDegree(PORT_UPPER, currentUpperDegree, SERVO_UPPER_MIN, SERVO_UPPER_MAX);
  }
  isEmbarked = true;
}

void turnRight() {

  for (int i = 0; i < 45; i++) {
    currentBaseDegree = currentBaseDegree - 1;
    setServoToDegree(PORT_BASE, currentBaseDegree, SERVO_BASE_MIN, SERVO_BASE_MAX);
  }
}

void turnLeft() {
  for (int i = 0; i < 45; i++) {
    currentBaseDegree = currentBaseDegree + 1;
    setServoToDegree(PORT_BASE, currentBaseDegree, SERVO_BASE_MIN, SERVO_BASE_MAX);
  }
}

void zoomIn() {
  if (zoomInCount < 2) {
    for (int i = 0; i < 15; i++) {
      currentLowerDegree = currentLowerDegree + 1;
      setServoToDegree(PORT_LOWER, currentLowerDegree, SERVO_LOWER_MIN, SERVO_LOWER_MAX);
      currentUpperDegree = currentUpperDegree - 1;
      setServoToDegree(PORT_UPPER, currentUpperDegree, SERVO_UPPER_MIN, SERVO_UPPER_MAX);
    }
    if (zoomOutCount == 0) {
      zoomInCount++;
    } else {
      zoomOutCount--;
    }
  } else {
    Serial.write("Cannot zoom more");
  }
}

void zoomOut() {
  if (zoomOutCount < 2) {
    for (int i = 0; i < 15; i++) {
      currentLowerDegree = currentLowerDegree - 1;
      setServoToDegree(PORT_LOWER, currentLowerDegree, SERVO_LOWER_MIN, SERVO_LOWER_MAX);
      currentUpperDegree = currentUpperDegree + 1;
      setServoToDegree(PORT_UPPER, currentUpperDegree, SERVO_UPPER_MIN, SERVO_UPPER_MAX);
    }
    if (zoomInCount == 0) {
      zoomOutCount++;
    } else {
      zoomInCount--;
    }
  } else {
    Serial.write("Cannot zoom more");
  }
}

void processIncomingCommand(int incomingCommand) {
  if (incomingCommand == 49) { //disembark
    if (currentBaseDegree == 180) {
      if (isEmbarked) {
        Serial.write("Disembarking");
        disembarkLapka();
        Serial.write("Finished disembarking");
      } else {
        Serial.write("Already disembarked, ignoring");
      }
    } else {
      Serial.write("Cannot disembark in this position");
    }
  }
  if (incomingCommand == 50) { //embark
    if (currentBaseDegree == 180) {
      if (isEmbarked) {
        Serial.write("Already embarked, ignoring");
      } else {
        Serial.write("Embarking");
        embarkLapka();
        Serial.write("Finished embarking");
      }
    } else {
      Serial.write("Cannot embark in this position");
    }
  }
  if (incomingCommand == 51) { //right
    if (currentBaseDegree == 0) {
      Serial.write("Cannot go righter");
    } else {
      Serial.write("Turning right");
      turnRight();
      Serial.write("Finished turning right");
    }
  }
  if (incomingCommand == 52) { //left
    if (currentBaseDegree == 180) {
      Serial.write("Cannot go lefter");
    } else {
      Serial.write("Turning left");
      turnLeft();
      Serial.write("Finished turning left");
    }
  }
  if (incomingCommand == 53) { //zoom in
    zoomIn();
  }
  if (incomingCommand == 54) { //zoom out
    zoomOut();
  }
}

void loop() {
  if (Serial.available() > 0) {
    int incomingCommand = Serial.read();
    Serial.print("I received: ");
    Serial.write(incomingCommand);
    processIncomingCommand(incomingCommand);
  }
}
