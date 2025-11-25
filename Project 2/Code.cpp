#include <Servo.h>
Servo myServo;

const int trigPin = 8;
const int echoPin = 7;
const int greenLED = 2;
const int yellowLED = 3;
const int redLED = 4;
long duration;
int distance;

void setup()
{
    pinMode(trigPin, OUTPUT);
    pinMode(echoPin, INPUT);
    3 pinMode(greenLED, OUTPUT);
    pinMode(yellowLED, OUTPUT);
    pinMode(redLED, OUTPUT);
    myServo.attach(9);
    Serial.begin(9600);
}

void loop()
{
    // Send ultrasonic pulse
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    // Measure echo time
    duration = pulseIn(echoPin, HIGH);
    // Calculate distance in cm
    distance = duration * 0.034 / 2;
    Serial.print("Distance: ");
    Serial.print(distance);
    Serial.println(" cm");
    // Control LEDs and Servo based on distance
    if (distance <= 10)
    { // Close range
        digitalWrite(greenLED, HIGH);
        digitalWrite(yellowLED, LOW);
        digitalWrite(redLED, LOW);
        myServo.write(90);
    }
    else if (distance <= 20)
    { // Medium range
        digitalWrite(greenLED, LOW);
        4 digitalWrite(yellowLED, HIGH);
        digitalWrite(redLED, LOW);
        myServo.write(45);
    }
    else
    { // Far range
        digitalWrite(greenLED, LOW);
        digitalWrite(yellowLED, LOW);
        digitalWrite(redLED, HIGH);
        myServo.write(0);
    }
    delay(200); // Short delay for stability
}