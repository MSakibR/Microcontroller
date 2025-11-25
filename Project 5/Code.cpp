const int pirPin = 2;
const int buzzerPin = 8;
const int greenLed = 12;
const int redLed = 13;

int pirState = LOW;
int val = 0;

void setup()
{
    pinMode(pirPin, INPUT);
    pinMode(buzzerPin, OUTPUT);
    pinMode(greenLed, OUTPUT);
    3 pinMode(redLed, OUTPUT);
    Serial.begin(9600);
}

void loop()
{
    val = digitalRead(pirPin);
    if (val == HIGH)
    { // Motion detected
        digitalWrite(buzzerPin, HIGH);
        digitalWrite(redLed, HIGH);
        digitalWrite(greenLed, LOW);
        if (pirState == LOW)
        {
            Serial.println("Motion detected! Possible theft!");
            pirState = HIGH;
        }
    }
    else
    { // No motion
        digitalWrite(buzzerPin, LOW);
        digitalWrite(redLed, LOW);
        digitalWrite(greenLed, HIGH);
        if (pirState == HIGH)
        {
            Serial.println("Area clear. No motion.");
            pirState = LOW;
        }
    }
}