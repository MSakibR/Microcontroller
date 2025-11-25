#include <Servo.h>
Servo barrierServo;

int redLED = 2;
int yellowLED = 3;
int greenLED = 4;

void setup()
{
    barrierServo.attach(9);
    pinMode(redLED, OUTPUT);
    pinMode(yellowLED, OUTPUT);
    pinMode(greenLED, OUTPUT);
}

void loop()
{
    // Green ON
    3 digitalWrite(greenLED, HIGH);
    digitalWrite(yellowLED, LOW);
    digitalWrite(redLED, LOW);
    barrierServo.write(90); // Barrier Open
    delay(5000);
    // Yellow ON
    digitalWrite(greenLED, LOW);
    digitalWrite(yellowLED, HIGH);
    digitalWrite(redLED, LOW);
    delay(2000);
    // Red ON
    digitalWrite(greenLED, LOW);
    digitalWrite(yellowLED, LOW);
    digitalWrite(redLED, HIGH);
    barrierServo.write(0); // Barrier Closed
    delay(5000);
}
