#define BLYNK_PRINT Serial
#define BLYNK_TEMPLATE_ID "TMPL3-txE0Pzz"
#define BLYNK_TEMPLATE_NAME "Self Watering System"
#define BLYNK_AUTH_TOKEN "JMV_wAp6RMPtzsylOQ_EseRs0dnlZiGs"

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>


char auth[] = "JMV_wAp6RMPtzsylOQ_EseRs0dnlZiGs";
char ssid[] = "TP-Link_ED29";
char pass[] = "71931821";

#define LED_PIN     D8  // D8 pin - Wemos D1 Mini
// Sensor pins
#define WATER_PWR D5    // D5 pin - Wemos D1 Mini
#define WATER_PIN A0    // A0 pin
#define MOISTURE_PWR D2  // D2 pin
#define MOISTURE_PIN D1  // D1 pin
#define WATER_PUMP D6   // D6 pin

/* Define how long the pump will run (3 seconds) */
#define PUMP_TIME 3000

/* Change these values based on your calibration values */
#define LVL_HIGH 520  // Define max value water level
#define LVL_LOW 100   // Define min value of water level
int sw;

BLYNK_WRITE(V4){
    int pinValue = param.asInt();
    if (pinValue == 1){
      sw=HIGH;
      int WATER_LEVEL = readWaterLevel();
      float percentLevel = map(WATER_LEVEL, 0, 520, 0, 100);
      Serial.print("Water Level: ");
      Serial.print(percentLevel);
      Serial.println(" %");
      if (WATER_LEVEL >= LVL_LOW) {
        Serial.println("Water level is perfect");
        int MOISTURE_LEVEL = readMoisture();
        if (MOISTURE_LEVEL == LOW) {
          Serial.println("Moisture is perfect");
          Blynk.virtualWrite(V1, "Moisture is perfect");
          Blynk.virtualWrite(V3, "The water pump is on standby!");
        } else {
          Serial.println("Low moisture! Time to water!");
          Blynk.virtualWrite(V1, "Low moisture! Time to water!");
          Blynk.virtualWrite(V3, "Water pump started!");
          digitalWrite(LED_PIN,HIGH);
          digitalWrite(WATER_PUMP, HIGH);
          Serial.print("Water pump started!");
          delay(PUMP_TIME);
          digitalWrite(WATER_PUMP, LOW);
          digitalWrite(LED_PIN,LOW);
          Serial.println("The water pump is on standby!");
        }
      } else {
        Serial.println("Water level is low! Time to add water!");
      }
      Blynk.virtualWrite(V2, percentLevel);
      Serial.println();
    } else if (pinValue == 0) {
      sw=LOW;
      Serial.println("System is in Manual mode!!");
     // do something when button is released;
    }
    Serial.print("V4 button value is: "); // printing value to serial monitor
    Serial.println(pinValue);
}


BLYNK_WRITE(V5){
        int pinValue = param.asInt(); // assigning incoming value from pin V1 to a variable
        
        if (pinValue == 1 && sw == LOW){
            Blynk.virtualWrite(V3, "Water pump started!");
            digitalWrite(LED_PIN,HIGH);
            digitalWrite(WATER_PUMP, HIGH);
            Serial.println("Water pump started!");
        }
         else if (pinValue == 0) {
           
            digitalWrite(WATER_PUMP, LOW);
            digitalWrite(LED_PIN,LOW);
            Serial.println("The water pump is on standby!");
            Blynk.virtualWrite(V3, "Water pump is off!");
            Serial.println("Water pump is off!");
        }
      Serial.print("V5 button value is: "); // printing value to serial monitor
        Serial.println(pinValue);
}
/* Define the time range for sensor measurements */
const int measurementInterval = 10000;
/* Time variable to keep track of the time range */
unsigned long previousMillis = 0;

void setup() {
  Serial.begin(115200);
  sw = HIGH;
  /*WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    //delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Wi-Fi connected: ");
  Serial.println(WiFi.localIP());
  */
  Blynk.begin(auth,ssid,pass);
  pinMode(WATER_PWR, OUTPUT);
  pinMode(WATER_PIN, INPUT);
  pinMode(MOISTURE_PWR, OUTPUT);
  pinMode(MOISTURE_PIN, INPUT);
  pinMode(WATER_PUMP, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  /* Initially keep the sensors and motor OFF */
  digitalWrite(LED_PIN,HIGH);
  delay(3000);
  digitalWrite(LED_PIN,LOW);
  digitalWrite(WATER_PWR, LOW);
  digitalWrite(MOISTURE_PWR, LOW);
  digitalWrite(WATER_PUMP, LOW);
}

void loop() {
  // Run the Blynk app.
  Blynk.run();
  // Run the LED function
  /* Get current time. If the defined time range 
  has not passed, terminate the loop */
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis < measurementInterval) {
    return;
  }
  previousMillis = currentMillis;
  if(sw==HIGH)
    without_wifi();
}

void without_wifi(){
  int WATER_LEVEL = readWaterLevel();
  float percentLevel = map(WATER_LEVEL, 0, 520, 0, 100);
  Serial.print("Water Level: ");
  Serial.print(percentLevel);
  Serial.println(" %");
  if (WATER_LEVEL >= LVL_LOW) {
    Serial.println("Water level is perfect");
    int MOISTURE_LEVEL = readMoisture();
    if (MOISTURE_LEVEL == LOW) {
      Serial.println("Moisture is perfect");
      //Blynk.virtualWrite(V1, "Moisture is perfect");
      //Blynk.virtualWrite(V3, "The water pump is on standby!");
    } else {
      Serial.println("Low moisture! Time to water!");
      //Blynk.virtualWrite(V1, "Low moisture! Time to water!");
      //Blynk.virtualWrite(V3, "Water pump started!");
      digitalWrite(LED_PIN,HIGH);
      digitalWrite(WATER_PUMP, HIGH);
      Serial.print("Water pump started!");
      delay(PUMP_TIME);
      digitalWrite(WATER_PUMP, LOW);
      digitalWrite(LED_PIN,LOW);
      Serial.print("The water pump is on standby!");
    }
  } else {
    Serial.println("Water level is low! Time to add water!");
  }
  //Blynk.virtualWrite(V2, percentLevel);
  Serial.println();
}
/* This function returns the analog water level measurement */
int readWaterLevel(){
  digitalWrite(WATER_PWR, HIGH);            // Turn the sensor ON
  delay(10);                                // Allow power to settle
  int sensorValue = analogRead(WATER_PIN);  // Read the analog value from sensor
  digitalWrite(WATER_PWR, LOW);             // Turn the sensor OFF
  return sensorValue;                       // Return analog water value
}

/* This function returns the digital soil moisture measurement */
int readMoisture(){
  digitalWrite(MOISTURE_PWR, HIGH);             // Turn the sensor ON
  delay(10);                                    // Allow power to settle
  int sensorValue = digitalRead(MOISTURE_PIN);  // Read the digital value from sensor
  digitalWrite(MOISTURE_PWR, LOW);              // Turn the sensor OFF
  return sensorValue;                           // Return digital moisture value
}