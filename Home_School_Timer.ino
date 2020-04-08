/*****************  NEEDED TO MAKE NODEMCU WORK ***************************/
#define FASTLED_INTERRUPT_RETRY_COUNT 0
#define FASTLED_ALLOW_INTERRUPTS 0
#define FASTLED_ESP8266_RAW_PIN_ORDER


/******************  WIFI MANAGER LIBRARIES *************************************/
#include <FS.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager
#include <ArduinoJson.h>          //https://github.com/bblanchon/ArduinoJson //this needs to be first, or it all crashes and burns...
/******************  LIBRARY SECTION *************************************/

#include <FastLED.h>        //https://github.com/FastLED/FastLED
#include <SimpleTimer.h>    //https://github.com/thehookup/Simple-Timer-Library
#include <ESP8266WiFi.h>    //if you get an error here you need to install the ESP8266 board manager 
#include <ESP8266mDNS.h>    //if you get an error here you need to install the ESP8266 board manager 
#include <ArduinoOTA.h>     //ArduinoOTA is now included with the ArduinoIDE
#include <Espalexa.h>       //https://github.com/Aircoookie/Espalexa

/*****************  GENERAL VARIABLES  *************************************/
/*****************  GENERAL VARIABLES  *************************************/
/*****************  GENERAL VARIABLES  *************************************/
/*****************  GENERAL VARIABLES  *************************************/
/*****************  GENERAL VARIABLES  *************************************/

#define NUM_LEDS 10
#define CLIENT_ID "Home_School_Clock"

char alexa_id1[20];
char alexa_id2[20];
char alexa_id3[20];
char alexa_id4[20];
char alexa_id5[20];
char ledsRemainingS[6];
char mappedTimerS[20];
char subjectOneCompleteS[6];
char subjectTwoCompleteS[6];
char subjectThreeCompleteS[6];
char subjectFourCompleteS[6];
char subjectFiveCompleteS[6];
char timerModeS[6];
char currentSubjectS[6];
char savedBrightness[6];

String alexaString1;
String alexaString2;
String alexaString3;
String alexaString4;
String alexaString5;

int currentSubject = 0;
int subjectOneComplete = 1;
int subjectTwoComplete = 1;
int subjectThreeComplete = 1;
int subjectFourComplete = 1;
int subjectFiveComplete = 1;
int timerMode = 0;
int totalMinutes = 0;
int ledsRemaining = NUM_LEDS;
long mappedTimer = 0;

int timerNumber = 0;

bool shouldSaveConfig = false;
bool boot = true;

const int ledPin = 4; //marked as D2 on the board

/*****************  DECLARATIONS  ****************************************/
/*****************  DECLARATIONS  ****************************************/
/*****************  DECLARATIONS  ****************************************/
/*****************  DECLARATIONS  ****************************************/
/*****************  DECLARATIONS  ****************************************/
/*****************  DECLARATIONS  ****************************************/


WiFiClient espClient;
SimpleTimer timer;
Espalexa espalexa;
ESP8266WebServer server(80);

void colorLightChanged1(uint8_t brightness, uint32_t rgb);
void colorLightChanged2(uint8_t brightness, uint32_t rgb);
void colorLightChanged3(uint8_t brightness, uint32_t rgb);
void colorLightChanged4(uint8_t brightness, uint32_t rgb);
void colorLightChanged5(uint8_t brightness, uint32_t rgb);
boolean connectWifi();
boolean wifiConnected = false;

CRGB leds[NUM_LEDS];

/*****************  WIFI CONFIG STUFF  ****************************************/
/*****************  WIFI CONFIG STUFF  ****************************************/
/*****************  WIFI CONFIG STUFF  ****************************************/
/*****************  WIFI CONFIG STUFF  ****************************************/
/*****************  WIFI CONFIG STUFF  ****************************************/

//callback notifying us of the need to save config
void saveConfigCallback () {
  Serial.println("Should save config");
  shouldSaveConfig = true;
}

void configPortal()
{
  Serial.println("mounting FS...");
  if (SPIFFS.begin())
  {
    Serial.println("mounted file system");
    if (SPIFFS.exists("/savedVariables.json"))
    {
      //file exists, reading and loading
      Serial.println("reading saved variable file");
      File savedVariables = SPIFFS.open("/savedVariables.json", "r");
      if (savedVariables)
      {
        Serial.println("opened savedVariables file ");
        size_t size = savedVariables.size();
        // Allocate a buffer to store contents of the file.
        std::unique_ptr<char[]> buf(new char[size]);
        savedVariables.readBytes(buf.get(), size);
        DynamicJsonBuffer jsonBuffer;
        JsonObject& json = jsonBuffer.parseObject(buf.get());
        json.printTo(Serial);
        if (json.success())
        {
          Serial.println("\nparsed json");
              
          strcpy(ledsRemainingS, json["ledsRemainingS"]);
          ledsRemaining = atoi( ledsRemainingS );
          strcpy(mappedTimerS, json["mappedTimerS"]);
          mappedTimer = atoi( mappedTimerS );
          strcpy(subjectOneCompleteS, json["subjectOneCompleteS"]);
          subjectOneComplete = atoi( subjectOneCompleteS );
          strcpy(subjectTwoCompleteS, json["subjectTwoCompleteS"]);
          subjectTwoComplete = atoi( subjectTwoCompleteS );
          strcpy(subjectThreeCompleteS, json["subjectThreeCompleteS"]);
          subjectThreeComplete = atoi( subjectThreeCompleteS );
          strcpy(subjectFourCompleteS, json["subjectFourCompleteS"]);
          subjectFourComplete = atoi( subjectFourCompleteS );
          strcpy(subjectFiveCompleteS, json["subjectFiveCompleteS"]);
          subjectFiveComplete = atoi( subjectFiveCompleteS );
          strcpy(timerModeS, json["timerModeS"]);
          timerMode = atoi( timerModeS );
          strcpy(currentSubjectS, json["currentSubjectS"]);
          currentSubject = atoi( currentSubjectS );
        }
        else
        {
          Serial.println("failed to load json color1");
        }
      }
    }
    if (SPIFFS.exists("/config.json"))
    {
      //file exists, reading and loading
      Serial.println("reading config file");
      File configFile = SPIFFS.open("/config.json", "r");
      if (configFile)
      {
        Serial.println("opened config file");
        size_t size = configFile.size();
        // Allocate a buffer to store contents of the file.
        std::unique_ptr<char[]> buf(new char[size]);
        configFile.readBytes(buf.get(), size);
        DynamicJsonBuffer jsonBuffer;
        JsonObject& json = jsonBuffer.parseObject(buf.get());
        json.printTo(Serial);
        if (json.success())
        {
          Serial.println("\nparsed json");
          strcpy(alexa_id1, json["alexa_id1"]);
          alexaString1 = String((char *)alexa_id1);
          strcpy(alexa_id2, json["alexa_id2"]);
          alexaString2 = String((char *)alexa_id2);
          strcpy(alexa_id3, json["alexa_id3"]);
          alexaString3 = String((char *)alexa_id3);
          strcpy(alexa_id4, json["alexa_id4"]);
          alexaString4 = String((char *)alexa_id4);
          strcpy(alexa_id5, json["alexa_id5"]);
          alexaString5 = String((char *)alexa_id5);

          
        }
        else
        {
          Serial.println("failed to load json config");
        }
      }
    }
  }
  else
  {
    Serial.println("failed to mount FS");
  }
  //end read
  // The extra parameters to be configured (can be either global or just in the setup)
  // After connecting, parameter.getValue() will get you the configured value
  // id/name placeholder/prompt default length
  WiFiManagerParameter custom_alexa_id1("ID1", "SUBJECT 1", alexa_id1, 20);
  WiFiManagerParameter custom_alexa_id2("ID2", "SUBJECT 2", alexa_id2, 20);
  WiFiManagerParameter custom_alexa_id3("ID3", "SUBJECT 3", alexa_id3, 20);
  WiFiManagerParameter custom_alexa_id4("ID4", "SUBJECT 4", alexa_id4, 20);
  WiFiManagerParameter custom_alexa_id5("ID5", "SUBJECT 5", alexa_id3, 20);


  //WiFiManager
  //Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wifiManager;

  //set config save notify callback
  wifiManager.setSaveConfigCallback(saveConfigCallback);

  //add all your parameters here
  wifiManager.addParameter(&custom_alexa_id1);
  wifiManager.addParameter(&custom_alexa_id2);
  wifiManager.addParameter(&custom_alexa_id3);
  wifiManager.addParameter(&custom_alexa_id4);
  wifiManager.addParameter(&custom_alexa_id5);


  //reset settings - for testing
  if (digitalRead(D4) == LOW)
  {
    wifiManager.resetSettings();
  }
  if (digitalRead(D5) == LOW)
  {
    wifiManager.resetSettings();
    delay(2000);
    if (!wifiManager.autoConnect("School Timer Setup")) 
    {
    Serial.println("failed to connect and hit timeout");
    delay(3000);
    //reset and try again, or maybe put it to deep sleep
    ESP.reset();
    delay(5000);
    }
  }
  //if you get here you have connected to the WiFi
  Serial.println("connected...");

  //read updated parameters
  strcpy(alexa_id1, custom_alexa_id1.getValue());
  strcpy(alexa_id2, custom_alexa_id2.getValue());
  strcpy(alexa_id3, custom_alexa_id3.getValue());
  strcpy(alexa_id4, custom_alexa_id4.getValue());
  strcpy(alexa_id5, custom_alexa_id5.getValue());


  //save the custom parameters to FS
  if (shouldSaveConfig) {
    Serial.println("saving config");
    DynamicJsonBuffer jsonBuffer;
    JsonObject& json = jsonBuffer.createObject();
    json["alexa_id1"] = alexa_id1;
    json["alexa_id2"] = alexa_id2;
    json["alexa_id3"] = alexa_id3;
    json["alexa_id4"] = alexa_id4;
    json["alexa_id5"] = alexa_id5;


    File configFile = SPIFFS.open("/config.json", "w");
    if (!configFile) {
      Serial.println("failed to open config file for writing");
    }

    json.printTo(Serial);
    json.printTo(configFile);
    configFile.close();
    //end save
  }
}




/*****************  GLOBAL  FUNCTIONS  *******************************/
/*****************  GLOBAL  FUNCTIONS  *******************************/
/*****************  GLOBAL  FUNCTIONS  *******************************/
/*****************  GLOBAL  FUNCTIONS  *******************************/
/*****************  GLOBAL  FUNCTIONS  *******************************/

void resetAll()
{
  subjectOneComplete = 0;
  subjectTwoComplete = 0;
  subjectThreeComplete = 0;
  subjectFourComplete = 0;
  subjectFiveComplete = 0;
  timerMode = 0;
  timer.disable(timerNumber);
  delay(10);
  timer.deleteTimer(timerNumber);
  delay(10);
}

void saveVariables()
{
  itoa(ledsRemaining, ledsRemainingS, 10);
  itoa(mappedTimer, mappedTimerS, 10);
  itoa(subjectOneComplete, subjectOneCompleteS, 10);
  itoa(subjectTwoComplete, subjectTwoCompleteS, 10);
  itoa(subjectThreeComplete, subjectThreeCompleteS, 10);
  itoa(subjectFourComplete, subjectFourCompleteS, 10);
  itoa(subjectFiveComplete, subjectFiveCompleteS, 10);
  itoa(timerMode, timerModeS, 10);
  itoa(currentSubject, currentSubjectS, 10);

  Serial.println("saving variables");
  
  DynamicJsonBuffer jsonBuffer;
  JsonObject& json = jsonBuffer.createObject();
  json["ledsRemainingS"] = ledsRemainingS;
  json["mappedTimerS"] = mappedTimerS;
  json["subjectOneCompleteS"] = subjectOneCompleteS;
  json["subjectTwoCompleteS"] = subjectTwoCompleteS;
  json["subjectThreeCompleteS"] = subjectThreeCompleteS;
  json["subjectFourCompleteS"] = subjectFourCompleteS;
  json["subjectFiveCompleteS"] = subjectFiveCompleteS;
  json["timerModeS"] = timerModeS;
  json["currentSubjectS"] = currentSubjectS;
  File storedFile = SPIFFS.open("/savedVariables.json", "w");
  if (!storedFile) 
  {
    Serial.println("failed to open config file for writing");
  }
  json.printTo(Serial);
  json.printTo(storedFile);
  storedFile.close();
}

void noTimers()
{
  for (int i = 0; i < NUM_LEDS; i++)
  {
    leds[i] = CRGB(0, 0, 0);
  }
  if (subjectOneComplete == 1)
  {
    leds[0] = CRGB(0, 0, 255);
    leds[1] = CRGB(0, 0, 255);
  }
  if (subjectTwoComplete == 1)
  {
    leds[2] = CRGB(255, 255, 0);
    leds[3] = CRGB(255, 255, 0);
  }
  if (subjectThreeComplete == 1)
  {
    leds[4] = CRGB(255, 0, 0);
    leds[5] = CRGB(255, 0, 0);
  }
  if (subjectFourComplete == 1)
  {
    leds[6] = CRGB(255, 0, 255);
    leds[7] = CRGB(255, 0, 255);
  }
  if (subjectFiveComplete == 1)
  {
    leds[8] = CRGB(0, 255, 255);
    leds[9] = CRGB(0, 255, 255);
  }
}

void activeTimer()
{
  fadeToBlackBy( leds, NUM_LEDS, 10);
  int colorLED = map(ledsRemaining, 0, NUM_LEDS, 0, 96);
  for(int i = 0; i < ledsRemaining; i++) 
  {
    leds[i] = CHSV (colorLED,255,192);
  }
}

void subtractInterval()
{
  Serial.println("subtracting");
  if (ledsRemaining == 1)
  {
    if (currentSubject == 1)
    {
      subjectOneComplete = 1;
    }
    if (currentSubject == 2)
    {
      subjectTwoComplete = 1;
    }
    if (currentSubject == 3)
    {
      subjectThreeComplete = 1;
    }
    if (currentSubject == 4)
    {
      subjectFourComplete = 1;
    }
    if (currentSubject == 5)
    {
      subjectFiveComplete = 1;
    }
    timerMode = 0;
    mappedTimer = 0;
  }
  ledsRemaining--;
  saveVariables();
  Serial.print("LEDS Remaining: ");
  Serial.println(ledsRemaining);
}

/*****************  SETUP FUNCTION  ****************************************/
/*****************  SETUP FUNCTION  ****************************************/
/*****************  SETUP FUNCTION  ****************************************/
/*****************  SETUP FUNCTION  ****************************************/
/*****************  SETUP FUNCTION  ****************************************/


void setup() {
  Serial.begin(115200);
  configPortal();
  Serial.println(alexa_id1);
  Serial.println(alexa_id2);
  Serial.println(alexa_id3);
  Serial.println(alexa_id4);
  Serial.println(alexa_id5);
  FastLED.addLeds<WS2812B, ledPin, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(100);
  //FastLED.setDither( 0 );
  WiFi.setSleepMode(WIFI_NONE_SLEEP);
  WiFi.mode(WIFI_STA);
  wifiConnected = true;
  espalexa.addDevice(alexaString1, colorLightChanged1);
  espalexa.addDevice(alexaString2, colorLightChanged2);
  espalexa.addDevice(alexaString3, colorLightChanged3);
  espalexa.addDevice(alexaString4, colorLightChanged4);
  espalexa.addDevice(alexaString5, colorLightChanged5);
  espalexa.begin();
  ArduinoOTA.setHostname(CLIENT_ID);
  ArduinoOTA.begin();
  if(mappedTimer != 0)
  {
    timerNumber = timer.setTimer(mappedTimer, subtractInterval,  ledsRemaining);
  }
}



/*****************  MAIN LOOP  ****************************************/
/*****************  MAIN LOOP  ****************************************/
/*****************  MAIN LOOP  ****************************************/
/*****************  MAIN LOOP  ****************************************/
/*****************  MAIN LOOP  ****************************************/


void loop()
{
  espalexa.loop();
  if (timerMode == 0)
  {
    noTimers();
  }
  else
  {
    activeTimer();
  }
  ArduinoOTA.handle();
  timer.run();
  FastLED.show();
  delay(1);
}

/*****************  ALEXA CALLBACKS  ****************************************/
/*****************  ALEXA CALLBACKS  ****************************************/
/*****************  ALEXA CALLBACKS  ****************************************/
/*****************  ALEXA CALLBACKS  ****************************************/
/*****************  ALEXA CALLBACKS  ****************************************/

void colorLightChanged1(uint8_t brightnessCommand, uint32_t rgb)
{
  if (brightnessCommand != 252 )
  {
    timerMode = 1;
    Serial.println("Got Data Subject 1");
    Serial.println(brightnessCommand);
    int brightnessPercent = map(brightnessCommand, 0, 255, 0, 100);
    Serial.println(brightnessPercent);
    currentSubject = 1;
    timer.disable(timerNumber);
    delay(10);
    timer.deleteTimer(timerNumber);
    delay(10);
    ledsRemaining = NUM_LEDS;
    mappedTimer = (brightnessPercent * 60000) / NUM_LEDS;
    timerNumber = timer.setTimer(mappedTimer, subtractInterval, NUM_LEDS);
    Serial.println(mappedTimer);
  }
  else
  {
    resetAll();
  }
  saveVariables();
}

void colorLightChanged2(uint8_t brightnessCommand, uint32_t rgb)
{
  if (brightnessCommand != 252 )
  {
    timerMode = 1;
    Serial.println("Got Data Subject 2");
    Serial.println(brightnessCommand);
    int brightnessPercent = map(brightnessCommand, 0, 255, 0, 100);
    Serial.println(brightnessPercent);
    currentSubject = 2;
    timer.disable(timerNumber);
    delay(10);
    timer.deleteTimer(timerNumber);
    delay(10);
    ledsRemaining = NUM_LEDS;
    mappedTimer = (brightnessPercent * 60000) / NUM_LEDS;
    timerNumber = timer.setTimer(mappedTimer, subtractInterval, NUM_LEDS);
    Serial.println(mappedTimer);
  }
  else
  {
    resetAll();
  }
  saveVariables();
}

void colorLightChanged3(uint8_t brightnessCommand, uint32_t rgb)
{
  if (brightnessCommand != 252 )
  {
    timerMode = 1;
    Serial.println("Got Data Subject 3");
    Serial.println(brightnessCommand);
    int brightnessPercent = map(brightnessCommand, 0, 255, 0, 100);
    Serial.println(brightnessPercent);
    currentSubject = 3;
    timer.disable(timerNumber);
    delay(10);
    timer.deleteTimer(timerNumber);
    delay(10);
    ledsRemaining = NUM_LEDS;
    totalMinutes = brightnessPercent;
    mappedTimer = (brightnessPercent * 60000) / NUM_LEDS;
    timerNumber = timer.setTimer(mappedTimer, subtractInterval, NUM_LEDS);
    Serial.println(mappedTimer);
  }
  else
  {
    resetAll();
  }
  saveVariables();
}

void colorLightChanged4(uint8_t brightnessCommand, uint32_t rgb)
{
  if (brightnessCommand != 252)
  {
    timerMode = 1;
    Serial.println("Got Data Subject 4");
    Serial.println(brightnessCommand);
    int brightnessPercent = map(brightnessCommand, 0, 255, 0, 100);
    Serial.println(brightnessPercent);
    currentSubject = 4;
    timer.disable(timerNumber);
    delay(10);
    timer.deleteTimer(timerNumber);
    delay(10);
    ledsRemaining = NUM_LEDS;
    totalMinutes = brightnessCommand;
    mappedTimer = (brightnessPercent * 60000) / NUM_LEDS;
    timerNumber = timer.setTimer(mappedTimer, subtractInterval, NUM_LEDS);
    Serial.println(mappedTimer);
  }
  else
  {
    resetAll();
  }
  saveVariables();
}

void colorLightChanged5(uint8_t brightnessCommand, uint32_t rgb)
{
  if (brightnessCommand != 252 )
  {
    timerMode = 1;
    Serial.println("Got Data Subject 5");
    Serial.println(brightnessCommand);
    int brightnessPercent = map(brightnessCommand, 0, 255, 0, 100);
    Serial.println(brightnessPercent);
    currentSubject = 5;
    timer.disable(timerNumber);
    delay(10);
    timer.deleteTimer(timerNumber);
    delay(10);
    ledsRemaining = NUM_LEDS;
    totalMinutes = brightnessPercent;
    mappedTimer = (brightnessPercent * 60000) / NUM_LEDS;
    timerNumber = timer.setTimer(mappedTimer, subtractInterval, NUM_LEDS);
    Serial.println(mappedTimer);
  }
  else
  {
    resetAll();
  }
  saveVariables();
}
