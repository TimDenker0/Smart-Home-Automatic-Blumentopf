
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


//----- WasserDaWasserWegSensor -----
  #define WaterNoWater1_PIN 35  
  #define WaterNoWater2_PIN 34 

//----- Turbine -----
  //int Turbine1_PIN = 11;
  int turbnieValue = 0;
  //int GPIO_Turbine1_pin=21;
  int Turbine_Ch=1;
  int Turbine_Freq=1000;
  int Turbine_Res=8;

  int Turbine2_PIN = 11;
  int GPIO_Turbine2_pin=21;


//----- Pumpe -----
  
  int Pumpe_PIN = 10;
  int pumpeValue = 0;
  int GPIO_Pumpe_pin=19;
  int Pumpe_Ch=1;           // int Pumpe_Ch=2;
  int Pumpe_Freq=1000;
  int Pumpe_Res=8;

 
void setup() {
  
Serial.begin(9600);

//----- TemperatureSensor -----
  dht.begin();

//----- Turbine -----
  //pinMode(Turbine1_PIN,OUTPUT);
  pinMode(Pumpe_PIN,OUTPUT);
  pinMode(Turbine2_PIN,OUTPUT);
  Serial.println("Send a value between 0 and 255");
  //ledcAttachPin(GPIO_Turbine1_pin, Turbine_Ch);
  ledcAttachPin(GPIO_Turbine2_pin, Turbine_Ch);
  ledcAttachPin(GPIO_Pumpe_pin, Turbine_Ch);
  ledcSetup(Turbine_Ch, Turbine_Freq, Turbine_Res);


//----- Pumpe -----

  
}

void loop() {

  delay(2000);

//----- TemperatureSensor -----
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float humiditaVallus = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float temperaturaVallus = dht.readTemperature();

  if (isnan(humiditaVallus) || isnan(temperaturaVallus)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  // Compute heat index in Celsius (isFahreheit = false)
  //float hic = dht.computeHeatIndex(t, h, false);

  Serial.print(F("Humidity: "));
  Serial.print(humiditaVallus);
  Serial.print(F("%  Temperature: "));
  Serial.print(temperaturaVallus);
  Serial.print(F("°C "));
  Serial.print('\n');
//----- END TemperatureSensor -----  


//----- BodenfeuchtigkeitsSensor -----
  int mudVallus = analogRead(BodenfeuchtigkeitsSensora_Pinnus);
  Serial.println(mudVallus);
//----- END BodenfeuchtigkeitsSensor -----


//----- WasserDaWasserWegSensor -----
  int waterNoWater1Value = analogRead(WaterNoWater1_PIN);
  int waterNoWater2Value = analogRead(WaterNoWater2_PIN);
  Serial.println(waterNoWater1Value);
  Serial.println(waterNoWater2Value);
//----- END WasserDaWasserWegSensor -----


//----- Turbine -----
  if (Serial.available() > 1) {
      turbnieValue =  Serial.parseInt();
      Serial.print("Set Speed to: ");
      Serial.println(turbnieValue);
      ledcWrite(Turbine_Ch, turbnieValue);
      Serial.println("doneTurbine!");
  }
//----- END Turbine -----


//----- Pumpe -----
//  if (Serial.available() > 1) {
//      pumpeValue =  Serial.parseInt();
//      Serial.println(pumpeValue);
//      ledcWrite(Pumpe_Ch, pumpeValue);
//      Serial.println("donePump!");
//  }
//----- END Pumpe -----
}
