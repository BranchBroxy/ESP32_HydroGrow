

// Load Wi-Fi library
#include <WiFi.h>
#include <WebServer.h>


WebServer main_server(80);  // Object of WebServer(HTTP port, 80 is defult)

// Replace with your network credentials
const char* ssid = "Steigerwald";
const char* password = "!SteigerwaldLan888!";

//for time and date
#include <NTPClient.h>

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

//for OTA
#include <WiFiClient.h>
#include <ESPmDNS.h>
#include <ArduinoOTA.h>


// Variable to store the HTTP request
String header;
// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0;
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;


int sliderValue = 0;


#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <WebSerial.h>

AsyncWebServer serial_server(5000);



#define MicroPin 4
#define GrowPin 0
#define BloomPin 2
#define MainPin 15
int dutyCycle;
const int PWMFreq = 5000; /* 5 KHz */
const int PWMChannel_micro = 0;
const int PWMChannel_grow = 1;
const int PWMChannel_bloom = 2;
const int PWMChannel_main = 3;
const int PWMResolution = 10;
const int MAX_DUTY_CYCLE = (int)(pow(2, PWMResolution) - 1);


#include <Wire.h>                  // Include Wire library (required for I2C devices)
#include <LiquidCrystal_I2C.h>     // Include LiquidCrystal_I2C library 

#define I2C_SDA 21
#define I2C_SCL 22
LiquidCrystal_I2C lcd(0x27, 20, 4);  // Configure LiquidCrystal_I2C library with 0x27 address, 16 columns and 2 rows


struct PinStruct {
  int pump_micro;
  int pump_grow;
  int pump_bloom;
  int pump_main;
};



void setup() {

  //Serial.begin(115200);
  //Serial.println("Try Connecting to ");
  //Serial.println(ssid);


  Wire.begin(I2C_SDA, I2C_SCL);
  lcd.init();                        // Initialize I2C LCD module
  lcd.backlight();                   // Turn backlight ON
  lcd.setCursor(0, 0);               // Go to column 0, row 0
  lcd.print("NurtriTron 0.1");
  lcd.setCursor(0, 1);               // Go to column 0, row 1
  lcd.print("Connecting to Wifi");
  WiFi.begin(ssid, password);

  // Connect to your wi-fi modem


  //Check wi-fi is connected to wi-fi network
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
  }
  //Serial.print(".");}
  //Serial.println("");
  //Serial.println("WiFi connected successfully");
  //Serial.print("Got IP: ");
  //Serial.println(WiFi.localIP());  //Show ESP32 IP on serial

  lcd.setCursor(0, 1);
  lcd.print("                   ");
  lcd.setCursor(0, 1);
  lcd.print(WiFi.localIP());

  WebSerial.begin(&serial_server);
  WebSerial.msgCallback(recvMsg);

  serial_server.begin();
  setup_main_server();

  //Serial.println("HTTP main_server started");
  delay(100);

  timeClient.begin();
  timeClient.setTimeOffset(3600); // set time offset to 1 hour (3600 seconds)
  //Serial.print("Current time: ");
  //Serial.println(timeClient.getFormattedTime());

  setup_ota();

  //MicroPin
  ledcSetup(PWMChannel_micro, PWMFreq, PWMResolution);
  ledcAttachPin(MicroPin, PWMChannel_micro);
  ledcWrite(PWMChannel_micro, 0);

  //GrowPin
  ledcSetup(PWMChannel_grow, PWMFreq, PWMResolution);
  ledcAttachPin(GrowPin, PWMChannel_grow);
  ledcWrite(PWMChannel_grow, 0);

  //BloomPin
  ledcSetup(PWMChannel_bloom, PWMFreq, PWMResolution);
  ledcAttachPin(BloomPin, PWMChannel_bloom);
  ledcWrite(PWMChannel_bloom, 0);

  //MainPin
  ledcSetup(PWMChannel_main, PWMFreq, PWMResolution);
  ledcAttachPin(MainPin, PWMChannel_main);
  ledcWrite(PWMChannel_main, 0);
}


int temp_value = 0;
String lcd_mode_text;

unsigned long previousMillis = 0;
const long sec_loop_interval = 1000;

unsigned long currentMillisforpump;
unsigned long previousMillisforpump;
const long sec_pump_interval = 5000;

bool reset_pump = false;
bool set_pumps_bool = false;
PinStruct pump_map;
void loop()
{
  unsigned long currentMillis = millis();
  String webserial_string;

  if (currentMillis - previousMillis >= sec_loop_interval)
  {
    previousMillis = currentMillis;
    //WebSerial.print("Current time: ");
    //WebSerial.println(timeClient.getFormattedTime());
    lcd.setCursor(0, 3);
    lcd.print(timeClient.getFormattedTime());
    lcd.setCursor(0, 2);
    if (sliderValue != temp_value)
    {
      if (sliderValue == 0)
      {
        lcd_mode_text = "No Mode - Default";

      }
      else if (sliderValue == 1)
      {
        lcd_mode_text = "Wurzel Mode";
      }
      else if (sliderValue == 2)
      {
        lcd_mode_text = "Blatt Mode";
      }
      else if (sliderValue == 3)
      {
        lcd_mode_text = "Growing Mode";
      }
      else {
        lcd_mode_text = "Error";
      }
      pump_map = set_pump_mode(sliderValue);
      webserial_string = "The Pump_Map is as follows:\nPumpe für Micro: " + String(pump_map.pump_micro);
      WebSerial.println(webserial_string);
      webserial_string = "The Pump_Map is as follows:\nPumpe für Grow: " + String(pump_map.pump_grow);
      WebSerial.println(webserial_string);
      webserial_string = "The Pump_Map is as follows:\nPumpe für Bloom: " + String(pump_map.pump_bloom);
      WebSerial.println(webserial_string);

      lcd.print("               ");
      lcd.setCursor(0, 2);
      lcd.print(lcd_mode_text);
      temp_value = sliderValue;
      set_pumps_bool = true;
    }
    //ledcWrite(PWMChannel, sliderValue * 10.24);
    //WebSerial.println(sliderValue);
    // WebSerial.println(sliderValue * 10.24);
  }

  if (set_pumps_bool == true || timeClient.getFormattedTime() == "12:46:00")
  {
    set_pumps(pump_map, ledcWrite, PWMChannel_micro);
    currentMillisforpump = millis();
    previousMillisforpump = 0;
    reset_pump = true;
    set_pumps_bool = false;
    WebSerial.println("Turn on Pump");
  }

  previousMillisforpump = millis();

  if (previousMillisforpump - currentMillisforpump >= sec_pump_interval && reset_pump == true)
  {
    //previousMillisforpump = currentMillisforpump;
    ledcWrite(PWMChannel_micro, 0);
    ledcWrite(PWMChannel_grow, 0);
    ledcWrite(PWMChannel_bloom, 0);
    ledcWrite(PWMChannel_main, 0);
    reset_pump = false;
    WebSerial.println("Turn off Pump");
  }
  if (timeClient.getFormattedTime() == "00:00:00")
  {

    ESP.restart();
  }
  main_server.handleClient();
  timeClient.update();
  ArduinoOTA.handle();
}
