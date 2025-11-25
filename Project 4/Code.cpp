#define TRIG_PIN 10
#define ECHO_PIN 11
#define EN_PIN 5
#define IN1_PIN 6
#define IN2_PIN 7

const unsigned long READ_INTERVAL = 500;
unsigned long lastRead = 0;
bool pumpOn = false;
const int FULL_DISTANCE = 5;   // cm - "tank full"
const int EMPTY_DISTANCE = 12; // cm - "tank low"

void setup()
{
    Serial.begin(9600);
    pinMode(TRIG_PIN, OUTPUT);
    pinMode(ECHO_PIN, INPUT);
    pinMode(EN_PIN, OUTPUT);
    pinMode(IN1_PIN, OUTPUT);
    pinMode(IN2_PIN, OUTPUT);
    digitalWrite(TRIG_PIN, LOW);
    digitalWrite(IN1_PIN, LOW);
    digitalWrite(IN2_PIN, LOW);
    analogWrite(EN_PIN, 0);
    delay(50);
    Serial.println("Water-level pump control starting...");
}

void loop()
{
    if (millis() - lastRead >= READ_INTERVAL)
    {
        lastRead = millis();
        long distance = readUltrasonicCm();
        Serial.print("Distance: ");
        if (distance >= 999)
        {
            Serial.print("no echo");
        }
        else
        {
            Serial.print(distance);
            Serial.print(" cm");
        }
        Serial.print(" | Pump: ");
        Serial.println(pumpOn ? "ON" : "OFF");
        // Control logic with hysteresis to avoid rapid toggling:
        if (!pumpOn && distance > EMPTY_DISTANCE)
        {
            startPump();
        }
        else if (pumpOn && distance <= FULL_DISTANCE)
        {
            stopPump();
        }
        4
    }
}

long readUltrasonicCm()
{
    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);
    long duration = pulseIn(ECHO_PIN, HIGH, 30000);
    if (duration == 0)
        return 999;
    long distanceCm = duration * 0.034 / 2.0;
    return distanceCm;
}

void startPump()
{
    Serial.println(">> START PUMP");
    digitalWrite(IN1_PIN, HIGH);
    digitalWrite(IN2_PIN, LOW);
    analogWrite(EN_PIN, 255);
    pumpOn = true;
}

void stopPump()
{
    Serial.println(">> STOP PUMP");
    analogWrite(EN_PIN, 0);
    digitalWrite(IN1_PIN, LOW);
    digitalWrite(IN2_PIN, LOW);
    pumpOn = false;
}