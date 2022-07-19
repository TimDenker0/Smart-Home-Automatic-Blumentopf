
//----- TemperatureSensor -----
  // Example testing sketch for various DHT humidity/temperature sensors written by ladyada
  // REQUIRES the following Arduino libraries:
  // - DHT Sensor Library: https://github.com/adafruit/DHT-sensor-library
  // - Adafruit Unified Sensor Lib: https://github.com/adafruit/Adafruit_Sensor

  #include "DHT.h"

  #define DHTPIN 4     // Digital pin connected to the DHT sensor
  // Feather HUZZAH ESP8266 note: use pins 3, 4, 5, 12, 13 or 14 --
  // Pin 15 can work but DHT must be disconnected during program upload.

  // Uncomment whatever type you're using!
  //#define DHTTYPE DHT11   // DHT 11
  #define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
  //#define DHTTYPE DHT21   // DHT 21 (AM2301)

  // Connect pin 1 (on the left) of the sensor to +5V
  // NOTE: If using a board with 3.3V logic like an Arduino Due connect pin 1
  // to 3.3V instead of 5V!
  // Connect pin 2 of the sensor to whatever your DHTPIN is
  // Connect pin 4 (on the right) of the sensor to GROUND
  // Connect a 10K resistor from pin 2 (data) to pin 1 (power) of the sensor
  DHT dht(DHTPIN, DHTTYPE);


//----- BodenfeuchtigkeitsSensor -----
  #define BodenfeuchtigkeitsSensora_Pinnus 32
  #define BodenfeuchtigkeitsSensora2_Pinnus 2  //33


//----- WasserDaWasserWegSensor -----
  #define WaterNoWater1_PIN 35  
  #define WaterNoWater2_PIN 34 

//----- Turbine -----
  int Turbine1_PIN = 11;
  int turbnieValue = 0;
  int GPIO_Turbine1_pin=21;
  int Turbine_Ch=1;
  int Turbine_Freq=1000;
  int Turbine_Res=8;

  int Turbine2_PIN = 8;
  int GPIO_Turbine2_pin=5;


//----- Pumpe -----
  
  int Pumpe_PIN = 10;
  int pumpeValue = 0;
  int GPIO_Pumpe_pin=19;
  int Pumpe_Ch=2;          
  int Pumpe_Freq=1000;
  int Pumpe_Res=8;


//----- LED -----

  int LED_PIN = 14;
  int ledValue = 0;
  int GPIO_LED_pin=22;
  int LED_Ch=3;         
  int LED_Freq=1000;
  int LED_Res=8;

//----- WLAN -----

  #include <WiFi.h>
  #include <WiFiMulti.h>

  WiFiMulti wifiMulti;


//----- Time -----
  #include "time.h"
  const char* ntpServer = "pool.ntp.org";
  const long  gmtOffset_sec = 3600;
  const int   daylightOffset_sec = 3600;


//----- NodeRed -----
  #include <PubSubClient.h>
  const char* mqtt_server = "mqtt.iot.informatik.uni-oldenburg.de";
  const int mqtt_port = 2883;
  const char* mqtt_user = "sutk";
  const char* mqtt_pw = "SoftSkills";
  
  const char* mqtt_humidityRoom_publish = "humidityRoom";
  const char* mqtt_temperature_publish = "temperature";
  const char* mqtt_mudHumidity1_publish = "mudHumidity1";
  const char* mqtt_mudHumidity2_publish = "mudHumidity2";
  const char* mqtt_waterTankLevel3_publish = "waterTankLevel3";
  const char* mqtt_waterTankLevel6_publish = "waterTankLevel6";

  const char* mqtt_ALARM_Wasserstand_6_publish = "ALARM_Wasserstand_1/6";
  const char* mqtt_ALARM_Wasserstand_3_publish = "ALARM_Wasserstand_1/3";
  
  
  const char* mqtt_topic_subscribe = "requestDataSheet";

  WiFiClient espClient;
  PubSubClient client(espClient);


  boolean sendToday = false;          //einmalige Initialisierung fuer taegliche Statusupdates


//----- Values -----

  float humiditaVallus;
  float temperaturaVallus;
  int mud1Vallus;
  int mud2Vallus;
  int waterNoWater1Value;
  int waterNoWater2Value;

  int dynDelay = 600000;         //alle 10min wird aktualisiert
  boolean pumpeAn = false;       //verhindert, dass pumpe und luefter gleichzeitig laufen mit vorrangierger Priorität auf Pumpe


 
void setup() {
  
Serial.begin(9600);

//----- TemperatureSensor -----
  dht.begin();

//----- Turbine -----
  pinMode(Turbine1_PIN,OUTPUT);
  pinMode(Turbine2_PIN,OUTPUT);
  ledcAttachPin(GPIO_Turbine1_pin, Turbine_Ch);
  ledcAttachPin(GPIO_Turbine2_pin, Turbine_Ch);
  ledcSetup(Turbine_Ch, Turbine_Freq, Turbine_Res);


//----- Pumpe -----
  pinMode(Pumpe_PIN,OUTPUT);
  ledcAttachPin(GPIO_Pumpe_pin, Pumpe_Ch);
  ledcSetup(Pumpe_Ch, Pumpe_Freq, Pumpe_Res);

//----- LED -----
  pinMode(LED_PIN,OUTPUT);
  ledcAttachPin(GPIO_LED_pin, LED_Ch);
  ledcSetup(LED_Ch, LED_Freq, LED_Res);


//----- WLAN -----
    wifiMulti.addAP("666", "Passwort1");
    wifiMulti.addAP("Pixel_1308", "Passwort2");
    wifiMulti.addAP("ssid_from_AP_3", "your_password_for_AP_3");

    Serial.println("Connecting Wifi...");
    if(wifiMulti.run() == WL_CONNECTED) {
      Serial.println("WiFi connected");
    }


//----- Time -----
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);


//----- NodeRed -----
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);              
  
}


void sendNodeRed(float humiditaVallus,float temperaturaVallus,int mud1Vallus,int mud2Vallus,int waterNoWater1Value,int waterNoWater2Value){

  String humidityRoom_Str = String(humiditaVallus);
  char humidityRoom_buff[humidityRoom_Str.length() +1];
  humidityRoom_Str.toCharArray(humidityRoom_buff, humidityRoom_Str.length() +1);
  client.publish("humidityRoom", humidityRoom_buff);

  String temperature_Str = String(temperaturaVallus);
  char temperature_buff[temperature_Str.length() +1];
  temperature_Str.toCharArray(temperature_buff, temperature_Str.length() +1);
  client.publish("temperature", temperature_buff);

  String mudHumidity1_Str = String(mud1Vallus);
  char mudHumidity1_buff[mudHumidity1_Str.length() +1];
  mudHumidity1_Str.toCharArray(mudHumidity1_buff, mudHumidity1_Str.length() +1);
  client.publish("mudHumidity1", mudHumidity1_buff);

  String mudHumidity2_Str = String(mud2Vallus);
  char mudHumidity2_buff[mudHumidity1_Str.length() +1];
  mudHumidity2_Str.toCharArray(mudHumidity2_buff, mudHumidity2_Str.length() +1);
  client.publish("mudHumidity2", mudHumidity2_buff);

  String waterNoWater1_Str = String(waterNoWater1Value);
  char waterNoWater1_buff[waterNoWater1_Str.length() +1];
  waterNoWater1_Str.toCharArray(waterNoWater1_buff, waterNoWater1_Str.length() +1);
  client.publish("waterTankLevel3", waterNoWater1_buff);

  String waterNoWater2_Str = String(waterNoWater2Value);
  char waterNoWater2_buff[waterNoWater2_Str.length() +1];
  waterNoWater2_Str.toCharArray(waterNoWater2_buff, waterNoWater2_Str.length() +1);
  client.publish("waterTankLevel6", waterNoWater2_buff);  
  
}






void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");

    // Create a random client ID: Client ID MUSS inviduell sein, da der MQTT Broker nicht mehrere Clients mit derselben ID bedienen kann
    String clientId = "Client-";
    clientId += String(random(0xffff), HEX);

    // Attempt to connect
    if (client.connect(clientId.c_str(), mqtt_user, mqtt_pw)) {
      Serial.println("connected");
      // Once connected, publish an announcement...

      
      client.subscribe("requestDataSheet");         

      
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void callback(char* topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;
  
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();

  // Feel free to add more if statements to control more GPIOs with MQTT

  // If a message is received on the topic esp32/output, you check if the message is either "on" or "off". 
  // Changes the output state according to the message
  if (String(topic) == "requestDataSheet") {
    //Serial.print("Changing output to ");
    if(messageTemp == "!dataSheet"){
      sendNodeRed(humiditaVallus, temperaturaVallus, mud1Vallus, mud2Vallus, waterNoWater1Value, waterNoWater2Value);
    }
  }
}


void sendNodeRedWarning(int waterNoWaterValue1, int waterNoWaterValue2){
  if (waterNoWaterValue2 ==0){
    client.publish("ALARM_Wasserstand_1/6", "Wassertank bei Füllstand von weniger als 1/6");
  }
  else if(waterNoWaterValue1 == 0){
    client.publish("ALARM_Wasserstand_1/3", "Wassertank bei Füllstand von weniger als 1/6");
  }
}




void loop() {

  delay(dynDelay);

//----- TemperatureSensor -----
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  humiditaVallus = dht.readHumidity();
  // Read temperature as Celsius (the default)
  temperaturaVallus = dht.readTemperature();

  if (isnan(humiditaVallus) || isnan(temperaturaVallus)) {
    //Serial.println(F("Failed to read from DHT sensor!"));
  }

  // Compute heat index in Celsius (isFahreheit = false)
  //float hic = dht.computeHeatIndex(t, h, false);

  //Serial.print(F("Humidity: "));
  //Serial.print(humiditaVallus);
  //Serial.print(F("%  Temperature: "));
  //Serial.print(temperaturaVallus);
  //Serial.print(F("°C "));
  //Serial.print('\n');
//----- END TemperatureSensor -----  


//----- BodenfeuchtigkeitsSensor -----
  mud1Vallus = analogRead(BodenfeuchtigkeitsSensora_Pinnus);
  mud2Vallus = analogRead(BodenfeuchtigkeitsSensora2_Pinnus);
  //Serial.println(mud1Vallus);
  //Serial.println(mud2Vallus);
//----- END BodenfeuchtigkeitsSensor -----


//----- WasserDaWasserWegSensor -----
  waterNoWater1Value = analogRead(WaterNoWater1_PIN);
  waterNoWater2Value = analogRead(WaterNoWater2_PIN);
  //Serial.println(waterNoWater1Value);
  //Serial.println(waterNoWater2Value);
//----- END WasserDaWasserWegSensor -----

//----- Turbine -----
      
   turbinenSteuerung(Turbine_Ch, temperaturaVallus);

//----- END Turbine -----


//----- Pumpe -----

   pumpenSteuerung(Pumpe_Ch, mud1Vallus, mud2Vallus);

//----- END Pumpe -----


//----- WLAN -----
  if(wifiMulti.run() != WL_CONNECTED) {
        Serial.println("WiFi not connected!");
        
        wifiMulti.addAP("666", "62729030318813756005");
        wifiMulti.addAP("Pixel_1308", "12345678");
        wifiMulti.addAP("ssid_from_AP_3", "your_password_for_AP_3");

        Serial.println("Connecting Wifi...");
        if(wifiMulti.run() == WL_CONNECTED) {
          Serial.println("WiFi connected");
        }
    }
//----- END WLAN -----


//----- LED -----

  ledSteuerung(LED_Ch);

//----- END LED -----


//----- NodeRed -----
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
  }
  
  int stunde = timeinfo.tm_hour;
  
  if(stunde == 12 && sendToday == false){
    sendNodeRed(humiditaVallus, temperaturaVallus, mud1Vallus, mud2Vallus, waterNoWater1Value, waterNoWater2Value);
    sendToday = true;
  }
  else if(stunde == 1){
    sendToday = false;
  }
  
  sendNodeRedWarning(waterNoWater1Value, waterNoWater2Value);       //wird immer abgefragt
  
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  
//----- END NodeRed -----

}












void ledSteuerung(int LED_ChUebergabe){
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
  }
  int ledValue = 0;
  int stunde = timeinfo.tm_hour;
  //Serial.println(stunde);
  
  if(stunde > 7 && stunde < 21){
    ledValue = 255;
    //Serial.println("LED: An");
  }
  else{
    //Serial.println("LED: Aus");
  }

  ledcWrite(LED_ChUebergabe, ledValue);
}


void pumpenSteuerung(int Pumpe_ChUebergabe, int mud1Vallus, int mud2Vallus){
  int pumpeValue = 0;
  dynDelay = 600000;
  pumpeAn = false;
  
  int mudVallusAvg = (mud1Vallus + mud2Vallus)/2;

  if(abs((mudVallusAvg - mud1Vallus)) > 500 || abs((mudVallusAvg - mud2Vallus)) > 500){
     //Serial.println("Du hast verkackt! Diskrepanz von Sensoren ueber 500 Pkt.");
  }
  
  if(abs((mudVallusAvg - mud1Vallus)) < 500 && abs((mudVallusAvg - mud2Vallus)) < 500){
    if(mudVallusAvg > 1200){
      pumpeValue = 255;
      dynDelay = 2000;
      pumpeAn = true;
    }
  }
  else if(mud1Vallus > 1800 || mud2Vallus > 1800){
    pumpeValue = 255;
    dynDelay = 2000;
    pumpeAn = true;
  }
  
  ledcWrite(Pumpe_ChUebergabe, pumpeValue);
}

void turbinenSteuerung(int Turbine_ChUebergabe, float temperaturaVallusUebergabe){
  int turbinenValue = 0;
  dynDelay = 600000;
    
  if(temperaturaVallusUebergabe > 29.0 && !pumpeAn){                //ca. bei 26*C im Gehaeuse (evtl. Sperre durch Pumpe)
    turbinenValue = 255;                 //100%
    dynDelay = 10000;
  } 
  else if(temperaturaVallusUebergabe > 25.0 && !pumpeAn){           //ca. bei 22*C im Gehaeuse (evtl. Sperre durch Pumpe)
    turbinenValue = 204;                 // 80%
    dynDelay = 10000;
  }

  ledcWrite(Turbine_ChUebergabe, turbinenValue);
}
