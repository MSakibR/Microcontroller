#include <Servo.h>
Servo barrierServo;

int touchPin = 2;
int buzzerPin = 12;
bool doorLocked = true;

void setup()
{
    barrierServo.attach(9);
    barrierServo.write(0);
    pinMode(buzzerPin, OUTPUT);
    pinMode(touchPin, INPUT);
}

void loop()
{
    int touchState = digitalRead(touchPin);
    if (touchState == HIGH)
    {
        if (doorLocked)
        {
            // Unlock the door
            barrierServo.write(90);
            tone(buzzerPin, 1000, 400);
            delay(400);
            noTone(buzzerPin);
            digitalWrite(buzzerPin, HIGH);
            doorLocked = false;
        }
        else
        {
            // Lock the door
            barrierServo.write(45);
            tone(buzzerPin, 1000, 200);
            delay(200);
            noTone(buzzerPin);
            digitalWrite(buzzerPin, LOW);
            doorLocked = true;
        }
        delay(1000); //
    }
}