#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>
#include "DHT.h"
#include <ESP32Servo.h>
#define WIFI_SSID "xxx"
#define WIFI_PASSWORD "xxx"
#define BOT_TOKEN
"xxx"
#define CHAT_ID "xxx"
    WiFiClientSecure secured_client;
UniversalTelegramBot bot(BOT_TOKEN, secured_client);
unsigned long lastBotCheck = 0;
#define TRIG_PIN 5
#define ECHO_PIN 13
#define DHTPIN 4
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);
#define MQ2_PIN 15
#define BUZZER_PIN 12
#define RAIN_PIN 17
#define SOIL_PIN 27
#define RELAY_PIN 14
#define LDR_TL 34
#define LDR_TR 35
#define LDR_BL 32
#define LDR_BR 33
#define SERVO_X 18
#define SERVO_Y 19
Servo servoX;
Servo servoY;
int posX = 90;
int posY = 90;
const int minAngle = 45;
const int maxAngle = 135;
WiFiServer server(80);
bool motorState = false; // false = OFF, true = ON
float getDistance()
{
    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);
    long duration = pulseIn(ECHO_PIN, HIGH);
    return (duration * 0.0343) / 2;
}
int ldrTL, ldrTR, ldrBL, ldrBR;
void updateServos()
{
    ldrTL = analogRead(LDR_TL);
    ldrTR = analogRead(LDR_TR);
    ldrBL = analogRead(LDR_BL);
    ldrBR = analogRead(LDR_BR);
    int topAvg = (ldrTL + ldrTR) / 2;
    int bottomAvg = (ldrBL + ldrBR) / 2;
    int leftAvg = (ldrTL + ldrBL) / 2;
    int rightAvg = (ldrTR + ldrBR) / 2;
    int verticalDiff = topAvg - bottomAvg;
    int horizontalDiff = leftAvg - rightAvg;
    if (abs(verticalDiff) > 50)
    {
        if (verticalDiff > 0 && posY < maxAngle)
            posY++;
        else if (verticalDiff < 0 && posY > minAngle)
            posY--;
    }
    if (abs(horizontalDiff) > 50)
    {
        if (horizontalDiff > 0 && posX > minAngle)
            posX--;
        else if (horizontalDiff < 0 && posX < maxAngle)
            posX++;
    }
    servoX.write(posX);
    servoY.write(posY);
    delay(40);
}
String readSmoke()
{
    int smokeVal = digitalRead(MQ2_PIN);
    if (smokeVal == LOW)
    {
        digitalWrite(BUZZER_PIN, HIGH);
        return " Smoke / Gas Detected!";
    }
    else
    {
        digitalWrite(BUZZER_PIN, LOW);
        return " Air Clear";
    }
}
String readRain()
{
    return digitalRead(RAIN_PIN) == LOW ? " It's Raining" : "No Rain";
}
String readSoil(){
    return digitalRead(SOIL_PIN) == HIGH ? "Soil is Dry" : "Soil is
               Moist ";
} String motorStatus()
{
    return motorState ? "Motor ON" : "Motor OFF";
}
void motorOn()
{
    digitalWrite(RELAY_PIN, LOW);
    motorState = true;
}
void motorOff()
{
    digitalWrite(RELAY_PIN, HIGH);
    motorState = false;
}
void handleNewMessages(int numNewMessages)
{
    for (int i = 0; i < numNewMessages; i++)
    {
        String chat_id = String(bot.messages[i].chat_id);
        String text = bot.messages[i].text;
        text.trim();
        text.toLowerCase();
        if (chat_id != CHAT_ID)
        {
            bot.sendMessage(chat_id, "Unauthorized user", "");
            continue;
        }
        if (text == "/start")
        {
            String welcome = " *ESP32 Smart Irrigation Bot*\n\n";
            welcome += "Available Commands:\n";
            welcome += "/status - Show all sensor & solar data\n";
            welcome += "/motor_on - Turn ON motor\n";
            welcome += "/motor_off - Turn OFF motor\n";
            bot.sendMessage(CHAT_ID, welcome, "Markdown");
        }
        else if (text == "/status")
        {
            float temp = dht.readTemperature();
            float hum = dht.readHumidity();
            float dist = getDistance();
            String msg = " *System Status:*\n";
            msg += " Temp: " + String(temp, 1) + "°C\n";
            msg += " Humidity: " + String(hum, 1) + "%\n";
            msg += " Distance: " + String(dist, 1) + " cm\n";
            msg += readRain() + "\n";
            msg += readSoil() + "\n";
            msg += readSmoke() + "\n";
            msg += "Motor: " + motorStatus() + "\n";
            msg += "\n *Solar Tracker Data:*\n";
            msg += "LDR TL: " + String(ldrTL) + "\n";
            msg += "LDR TR: " + String(ldrTR) + "\n";
            msg += "LDR BL: " + String(ldrBL) + "\n";
            msg += "LDR BR: " + String(ldrBR) + "\n";
            msg += "Servo X: " + String(posX) + "°\n";
            msg += "Servo Y: " + String(posY) + "°";
            bot.sendMessage(CHAT_ID, msg, "Markdown");
        }
        else if (text == "/motor_on")
        {
            motorOn();
            bot.sendMessage(CHAT_ID, " Motor turned ON ✅", "");
        }
        else if (text == "/motor_off")
        {
            motorOff();
            bot.sendMessage(CHAT_ID, "⚪ Motor turned OFF ✅", "");
        }
        else
        {
bot.sendMessage(CHAT_ID, " Unknown command. Type /start for
help.", "");
        }
    }
}
void setup()
{
    Serial.begin(115200);
    pinMode(TRIG_PIN, OUTPUT);
    pinMode(ECHO_PIN, INPUT);
    pinMode(MQ2_PIN, INPUT);
    pinMode(RAIN_PIN, INPUT);
    pinMode(SOIL_PIN, INPUT);
    pinMode(BUZZER_PIN, OUTPUT);
    pinMode(RELAY_PIN, OUTPUT);
    digitalWrite(RELAY_PIN, HIGH);
    servoX.attach(SERVO_X);
    servoY.attach(SERVO_Y);
    servoX.write(posX);
    servoY.write(posY);
    dht.begin();
    Serial.println("Connecting to WiFi...");
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nWiFi connected!");
    Serial.print(" Local dashboard: http://");
    Serial.println(WiFi.localIP());
    secured_client.setInsecure();
    server.begin();
}
void loop()
{
    updateServos();
    if (millis() - lastBotCheck > 2000)
    {
        int numNewMessages = bot.getUpdates(bot.last_message_received +
                                            1);
        while (numNewMessages)
        {
            handleNewMessages(numNewMessages);
            numNewMessages = bot.getUpdates(bot.last_message_received + 1);
        }
        lastBotCheck = millis();
    }
    WiFiClient client = server.available();
    if (!client)
        return;
    String request = client.readStringUntil('\r');
    client.flush();
    float distance = getDistance();
    float humidity = dht.readHumidity();
    float temperature = dht.readTemperature();
    String smokeStatus = readSmoke();
    String rainStatus = readRain();
    String soilStatus = readSoil();
    if (request.indexOf("GET /motor?state=on") >= 0)
        motorOn();
    if (request.indexOf("GET /motor?state=off") >= 0)
        motorOff();
    if (digitalRead(SOIL_PIN) == HIGH && digitalRead(RAIN_PIN) ==
                                             HIGH)
        motorOn();
    else
        motorOff();
    if (request.indexOf("GET /data") >= 0)
    {
        String json = "{";
        json += "\"distance\":" + String(distance, 2) + ",";
        json += "\"temperature\":" + String(temperature, 1) + ",";
        json += "\"humidity\":" + String(humidity, 1) + ",";
        json += "\"smoke\":\"" + smokeStatus + "\",";
        json += "\"rain\":\"" + rainStatus + "\",";
        json += "\"soil\":\"" + soilStatus + "\",";
        json += "\"motor\":\"" + motorStatus() + "\",";
        json += "\"ldrTL\":" + String(ldrTL) + ",";
        json += "\"ldrTR\":" + String(ldrTR) + ",";
        json += "\"ldrBL\":" + String(ldrBL) + ",";
        json += "\"ldrBR\":" + String(ldrBR) + ",";
        json += "\"servoX\":" + String(posX) + ",";
        json += "\"servoY\":" + String(posY);
        json += "}";
        client.println("HTTP/1.1 200 OK");
        client.println("Content-Type: application/json");
        client.println("Connection: close");
        client.println();
        client.println(json);
        return;
    }
    String html = "<!DOCTYPE html><html><head><meta charset='UTF-
        8'><title>ESP32 IoT Dashboard</title>"; html += "<style>body{background:#0d1117;color:#00ffcc;font-
        family : Arial;
    text - align : center;
padding:
    20px;
}
button
{
padding:
    10px 20px;
margin:
    5px;
    border - radius : 8px;
}
< / style > ";
    html += "<script>function
    updateData()
{fetch('/data').then(r=>r.json()).then(d=>{
        ";
            html +=
            "document.getElementById('distance').innerHTML=d.distance+' cm';";
        html +=
            "document.getElementById('temp').innerHTML=d.temperature+' °C';";
        html +=
            "document.getElementById('hum').innerHTML=d.humidity+' %';";
        html += "document.getElementById('smoke').innerHTML=d.smoke;";
        html += "document.getElementById('rain').innerHTML=d.rain;";
        html += "document.getElementById('soil').innerHTML=d.soil;";
        html += "document.getElementById('motor').innerHTML=d.motor;";
        html += "document.getElementById('ldrTL').innerHTML=d.ldrTL;";
        html += "document.getElementById('ldrTR').innerHTML=d.ldrTR;";
        html += "document.getElementById('ldrBL').innerHTML=d.ldrBL;";
        html += "document.getElementById('ldrBR').innerHTML=d.ldrBR;";
        html +=
            "document.getElementById('servoX').innerHTML=d.servoX+'°';";
        html +=
            "document.getElementById('servoY').innerHTML=d.servoY+'°';";
        html += "});}";
        html += "setInterval(updateData,2000);window.onload=updateData;";
        html += "function motorControl(s){fetch('/motor?state='+s);}";
        html += "</script></head><body><h1> ESP32 Smart IoT
                Dashboard < / h1 >
                ";
                html += "<p><b>Distance:</b> <span id='distance'>--</span></p>";
        html += "<p><b>Temperature:</b> <span id='temp'>--</span></p>";
        html += "<p><b>Humidity:</b> <span id='hum'>--</span></p>";
        html += "<p><b>Smoke:</b> <span id='smoke'>--</span></p>";
        html += "<p><b>Rain:</b> <span id='rain'>--</span></p>";
        html += "<p><b>Soil:</b> <span id='soil'>--</span></p>";
        html += "<p><b>Motor:</b> <span id='motor'>--</span></p>";
        html += "<button onclick=\"motorControl('on')\">Motor ON</button>";
        html += "<button onclick=\"motorControl('off')\">Motor
                OFF < / button >
                ";
                html += "<hr><h2> Solar Tracker Data</h2>";
        html += "<p><b>LDR TL:</b> <span id='ldrTL'>--</span> |
                < b > TR : </ b><span id = 'ldrTR'></ span> < / p > ";
                           html += "<p><b>LDR BL:</b> <span id='ldrBL'>--</span> |
                                   < b > BR : </ b><span id = 'ldrBR'> --</ span> < / p > ";
                                              html += "<p><b>Servo X:</b> <span id='servoX'>--</span> |
                                                      < b > Servo Y : </ b><span id = 'servoY'> --</ span> < / p > ";
                                                                      html += "</body></html>";
        client.println("HTTP/1.1 200 OK");
        client.println("Content-Type: text/html");
        client.println("Connection: close");
        client.println();
        client.println(html);
}