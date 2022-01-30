/*
 * Auteur : G.Menez
 */

// SPIFFS
#include <SPIFFS.h>
// OTA
#include <ArduinoOTA.h>
#include "ota.h"
// Capteurs
#include "OneWire.h"
#include "DallasTemperature.h"
// Wifi (TLS) https://github.com/espressif/arduino-esp32/tree/master/libraries/WiFiClientSecure
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include "classic_setup.h"
// MQTT https://pubsubclient.knolleary.net/
#include <PubSubClient.h>

/*===== ESP GPIO configuration ==============*/
/* ---- LED         ----*/
const int LEDpin = 19; // LED will use GPIO pin 19
/* ---- Light       ----*/
const int LightPin = A5; // Read analog input on ADC1_CHANNEL_5 (GPIO 33)
/* ---- Temperature ----*/
OneWire oneWire(23); // Pour utiliser une entite oneWire sur le port 23
DallasTemperature TempSensor(&oneWire) ; // Cette entite est utilisee par le capteur de temperature


String whoami; // Identification de CET ESP au sein de la flotte

//StaticJsonBuffer<200> jsonBuffer;
/*===== MQTT broker/server and TOPICS ========*/

const char* mqtt_server = "home.haysberg.io"; 
//  String MQTT_SERVER = "test.mosquitto.org";

int MQTT_PORT =  8883; // for TLS cf https://test.mosquitto.org/
const char* test_root_ca= \
"-----BEGIN CERTIFICATE-----\n" \
"MIIFazCCA1OgAwIBAgIRAIIQz7DSQONZRGPgu2OCiwAwDQYJKoZIhvcNAQELBQAw\n" \
"TzELMAkGA1UEBhMCVVMxKTAnBgNVBAoTIEludGVybmV0IFNlY3VyaXR5IFJlc2Vh\n" \
"cmNoIEdyb3VwMRUwEwYDVQQDEwxJU1JHIFJvb3QgWDEwHhcNMTUwNjA0MTEwNDM4\n" \
"WhcNMzUwNjA0MTEwNDM4WjBPMQswCQYDVQQGEwJVUzEpMCcGA1UEChMgSW50ZXJu\n" \
"ZXQgU2VjdXJpdHkgUmVzZWFyY2ggR3JvdXAxFTATBgNVBAMTDElTUkcgUm9vdCBY\n" \
"MTCCAiIwDQYJKoZIhvcNAQEBBQADggIPADCCAgoCggIBAK3oJHP0FDfzm54rVygc\n" \
"h77ct984kIxuPOZXoHj3dcKi/vVqbvYATyjb3miGbESTtrFj/RQSa78f0uoxmyF+\n" \
"0TM8ukj13Xnfs7j/EvEhmkvBioZxaUpmZmyPfjxwv60pIgbz5MDmgK7iS4+3mX6U\n" \
"A5/TR5d8mUgjU+g4rk8Kb4Mu0UlXjIB0ttov0DiNewNwIRt18jA8+o+u3dpjq+sW\n" \
"T8KOEUt+zwvo/7V3LvSye0rgTBIlDHCNAymg4VMk7BPZ7hm/ELNKjD+Jo2FR3qyH\n" \
"B5T0Y3HsLuJvW5iB4YlcNHlsdu87kGJ55tukmi8mxdAQ4Q7e2RCOFvu396j3x+UC\n" \
"B5iPNgiV5+I3lg02dZ77DnKxHZu8A/lJBdiB3QW0KtZB6awBdpUKD9jf1b0SHzUv\n" \
"KBds0pjBqAlkd25HN7rOrFleaJ1/ctaJxQZBKT5ZPt0m9STJEadao0xAH0ahmbWn\n" \
"OlFuhjuefXKnEgV4We0+UXgVCwOPjdAvBbI+e0ocS3MFEvzG6uBQE3xDk3SzynTn\n" \
"jh8BCNAw1FtxNrQHusEwMFxIt4I7mKZ9YIqioymCzLq9gwQbooMDQaHWBfEbwrbw\n" \
"qHyGO0aoSCqI3Haadr8faqU9GY/rOPNk3sgrDQoo//fb4hVC1CLQJ13hef4Y53CI\n" \
"rU7m2Ys6xt0nUW7/vGT1M0NPAgMBAAGjQjBAMA4GA1UdDwEB/wQEAwIBBjAPBgNV\n" \
"HRMBAf8EBTADAQH/MB0GA1UdDgQWBBR5tFnme7bl5AFzgAiIyBpY9umbbjANBgkq\n" \
"hkiG9w0BAQsFAAOCAgEAVR9YqbyyqFDQDLHYGmkgJykIrGF1XIpu+ILlaS/V9lZL\n" \
"ubhzEFnTIZd+50xx+7LSYK05qAvqFyFWhfFQDlnrzuBZ6brJFe+GnY+EgPbk6ZGQ\n" \
"3BebYhtF8GaV0nxvwuo77x/Py9auJ/GpsMiu/X1+mvoiBOv/2X/qkSsisRcOj/KK\n" \
"NFtY2PwByVS5uCbMiogziUwthDyC3+6WVwW6LLv3xLfHTjuCvjHIInNzktHCgKQ5\n" \
"ORAzI4JMPJ+GslWYHb4phowim57iaztXOoJwTdwJx4nLCgdNbOhdjsnvzqvHu7Ur\n" \
"TkXWStAmzOVyyghqpZXjFaH3pO3JLF+l+/+sKAIuvtd7u+Nxe5AW0wdeRlN8NwdC\n" \
"jNPElpzVmbUq4JUagEiuTDkHzsxHpFKVK7q4+63SM1N95R1NbdWhscdCb+ZAJzVc\n" \
"oyi3B43njTOQ5yOf+1CceWxG1bQVs5ZufpsMljq4Ui0/1lvh+wjChP4kqKOJ2qxq\n" \
"4RgqsahDYVvTH9w7jXbyLeiNdd8XM2w9U/t7y0Ff/9yi0GE44Za4rF2LN9d11TPA\n" \
"mRGunUHBcnWEvgJBQl9nJEiU0Zsnvgc/ubhPgXRR4Xq37Z0j4r7g1SgEEzwxA57d\n" \
"emyPxgcYxn/eR44/KJ4EBs+lVDR3veyJm+kXQ99b21/+jh5Xos1AnX5iItreGCc=\n" \
"-----END CERTIFICATE-----\n";

//==== MQTT TOPICS ==============
#define TOPIC_TEMP "sensors/temp"
#define TOPIC_JSON "json"
#define TOPIC_LED "sensors/led"



#define USERNAME "esp32-pierre";

const char* fingerprint = "93:3C:6D:DE:E9:5C:9C:41:A4:0F:9F:50:49:3D:82:BE:03:AD:87:BF";
float lat = 43.584549;
float longi = 7.1164553;
//==== ESP is MQTT Client =======
WiFiClientSecure wifiClient;     // Avec TLS !!!
PubSubClient client(wifiClient); // MQTT client


/*============== MQTT CALLBACK ===================*/

void mqtt_pubcallback(char* topic, byte* message, unsigned int length) {
  /* 
   *  Callback if a message is published on this topic.
   */
  
  // Byte list to String ... plus facile a traiter ensuite !
  // Mais sans doute pas optimal en performance => heap ?
  String messageTemp ;
  for(int i = 0 ; i < length ; i++) {
    messageTemp += (char) message[i];
  }
  
  Serial.print("Message : ");
  Serial.println(messageTemp);
  Serial.print("arrived on topic : ");
  Serial.println(topic) ;
 
  // Analyse du message et Action 
  if(String (topic) == TOPIC_LED) {
     // Par exemple : Changes the LED output state according to the message   
    Serial.print("Action : Changing output to ");
    if(messageTemp == "on") {
      Serial.println("on");
      set_pin(LEDpin,HIGH);
     
    } else if (messageTemp == "off") {
      Serial.println("off");
      set_pin(LEDpin,LOW);
    }
  }
}

/*============= CONNECT and SUBSCRIBE =====================*/

void mqtt_connect() {
  /*
     Subscribe to a MQTT topic
  */
 
  while (!client.connected()) { // Loop until we're reconnected
    Serial.print("Attempting MQTT connection...");

    // Attempt to connect => https://pubsubclient.knolleary.net/api
if (client.connect("esp32-pierre", /* Client Id when connecting to the server */
                        "pierre",    /* No credential */ 
                        "hardstuckplatine")) {
      Serial.println("connected");
    }
    else {
      Serial.print("failed, rc=");
      Serial.print(client.state());

      Serial.println(" try again in 5 seconds");
      delay(5000); // Wait 5 seconds before retrying
    }
  }
}

void mqtt_subscribe(char *topic) {
  mqtt_connect();
  client.subscribe(topic);
}

void setup_mqtt_server() {
  wifiClient.setCACert(test_root_ca);
  wifiClient.connect(mqtt_server, 8883);
  wifiClient.verify(fingerprint,mqtt_server);
  // set server of our client
  client.setServer(mqtt_server, 8883);
  // set callback when publishes arrive for the subscribed topic
  client.setCallback(mqtt_pubcallback);
}
/*============= ACCESSEURS ====================*/

float get_temperature() {
  float temperature;
  TempSensor.requestTemperaturesByIndex(0);
  delay (750);
  temperature = TempSensor.getTempCByIndex(0);
  return temperature;
}

float get_light(){
  return analogRead(LightPin);
}

void set_pin(int pin, int val){
 digitalWrite(pin, val) ;
}

int get_pin(int pin){
  return digitalRead(pin);
}


/*=============== SETUP =====================*/
void setup () {
  Serial.begin(9600);
  while (!Serial); // wait for a serial connection. Needed for native USB port only

  // Connexion Wifi
  connect_wifi();
  print_network_status();

  /* Choix d'une identification pour cet ESP ---*/
  // whoami = "esp1"; 
  whoami =  String(WiFi.macAddress());

  // Initialize the LED
  setup_led(LEDpin, OUTPUT, LOW);

  // Init temperature sensor
  TempSensor.begin();

  // Initialize SPIFFS
  SPIFFS.begin(true);

  setup_mqtt_server();
  mqtt_connect();   
}

/*================= LOOP ======================*/

void loop () {
  static uint32_t tick = 0;
  char data[256];
  String payload; // Payload : "JSON ready" 
  int32_t period = 5 * 1000l; // Publication period

  /*--- subscribe to TOPIC_LED  */
  mqtt_subscribe((char *)(TOPIC_LED));

  if ( millis() - tick < period)
  {
    goto END;
  }

  Serial.println("End of stand by period");
  tick = millis();

  /* Publish Temperature periodically */
  payload = "{\"who\": \"";
  payload += whoami;   
  payload += "\", \"lat\" : ";
  payload += lat;
  payload += ", \"lng\" : ";
  payload += longi;
  payload += ", \"value\": " ;
  payload += get_temperature(); 
  payload += "}";
  payload.toCharArray(data, (payload.length() + 1)); // Convert String payload to a char array

  Serial.println(data);
  client.publish(TOPIC_JSON, data);  // publish it 

  
END :  
  // Process MQTT ... obligatoire une fois par loop()
  client.loop(); 
}
