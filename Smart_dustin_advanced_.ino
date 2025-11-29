#include <Servo.h>
#include <Stepper.h>

// Sensor Pins
const int IR_SENSOR_PIN = 2;
const int MOISTURE_SENSOR_PIN = A0;
const int PROXIMITY_SENSOR_PIN = A1;

// Stepper Motor Pins
const int STEPS_PER_REV = 2048;
const int IN1 = 8;
const int IN2 = 9;
const int IN3 = 10;
const int IN4 = 11;

Stepper sortingStepper(STEPS_PER_REV, IN1, IN3, IN2, IN4);

// Servo Pin
const int SERVO_LID_PIN = 6;
Servo lidServo;

// Servo Positions
const int LID_OPEN_POS = 90;
const int LID_CLOSED_POS = 0;

// Thresholds
const int MOISTURE_THRESHOLD = 500;
const int PROXIMITY_THRESHOLD = 300;

void setup() {
  Serial.begin(9600);

  pinMode(IR_SENSOR_PIN, INPUT);

  lidServo.attach(SERVO_LID_PIN);
  lidServo.write(LID_CLOSED_POS);

  sortingStepper.setSpeed(10);

  Serial.println("Smart Dustbin Ready. Waiting for object...");
  delay(1000);
}

void loop() {

  // IR sensor detects object (LOW = detected)
  if (digitalRead(IR_SENSOR_PIN) == LOW) {
    Serial.println("Object detected...");

    delay(300); // debounce

    // Read sensors properly
    int moistureValue = analogRead(MOISTURE_SENSOR_PIN);
    int proximityValue = analogRead(PROXIMITY_SENSOR_PIN);

    Serial.print("Moisture value: ");
    Serial.println(moistureValue);

    Serial.print("Proximity value: ");
    Serial.println(proximityValue);

    delay(300);

    // ----------------------------
    // 1️⃣ METAL WASTE DETECTION
    // ----------------------------
    if (proximityValue < PROXIMITY_THRESHOLD) {
      Serial.println("Metal waste detected!");

      sortingStepper.step(700); // Move to metal bin
      delay(500);

      lidServo.write(LID_OPEN_POS);
      delay(1000);
      lidServo.write(LID_CLOSED_POS);

      sortingStepper.step(-700); // Return back
    }

    // ----------------------------
    // 2️⃣ WET WASTE (Moisture HIGH)
    // ----------------------------
    else if (moistureValue < MOISTURE_THRESHOLD) {
      Serial.println("Wet waste detected!");

      lidServo.write(LID_OPEN_POS);
      delay(1000);
      lidServo.write(LID_CLOSED_POS);
    }

    // ----------------------------
    // 3️⃣ DRY WASTE
    // ----------------------------
    else {
      Serial.println("Dry waste detected!");

      sortingStepper.step(-700); // Move to dry bin
      delay(500);

      lidServo.write(LID_OPEN_POS);
      delay(1000);
      lidServo.write(LID_CLOSED_POS);

      sortingStepper.step(700); // Return center
    }

    Serial.println("Sorting complete. Waiting for next object...\n");
    delay(1000);
  }

  delay(200);
}
