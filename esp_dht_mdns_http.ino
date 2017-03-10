// https://github.com/mrdunk/esp8266_mdns
#include <mdns.h>

// Additional Board Manager URLs:
// http://arduino.esp8266.com/stable/package_esp8266com_index.json
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

// https://github.com/adafruit/Adafruit_Sensor
// https://github.com/adafruit/DHT-sensor-library
#include "DHT.h"

#define DHTTYPE DHT22
#define QUERY_HOSTNAME "example.local" // MUST HAVE .local AT THE END

#define RETRY_INTERVAL (60*1000000) // 1 minute
#define CYCLE_SLEEP_INTERVAL (15*60*1000000) // 15 minutes
#define WIFI_WAIT_SECONDS 15
#define MDNS_WAIT_SECONDS 15

#define HOST_PORT 8030 // Change as you like

// Replace with your network details
const char* ssid = "SSID HERE";
const char* password = "PASSWORD HERE";

char answerHost[80];
unsigned int last_packet_count = 0;

void answerCallback(const mdns::Answer* answer) {
  if (answer->rrtype == MDNS_TYPE_A) {
    if (strcmp(answer->name_buffer, QUERY_HOSTNAME)==0) {
      strcpy(answerHost, answer->rdata_buffer);
    }
  }
}

// mDNS library setup
byte buffer[512];
mdns::MDns my_mdns(NULL, NULL, answerCallback, buffer, 512);

// DHT Sensor library setup
const int DHTPin = 13;
DHT dht(DHTPin, DHTTYPE);

// Temporary variables
static char celsiusTemp[7];
static char fahrenheitTemp[7];
static char humidityPercent[7];

void setup() {
    int waitCounter = 0;

    // Initializing serial port for debugging purposes
    Serial.begin(115200);
    delay(10);

    // Start up DHT sensor
    dht.begin();

    // Connecting to WiFi network
    WiFi.begin(ssid, password);

    unsigned long started = millis();

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
        if ((millis() - started)/1000 > WIFI_WAIT_SECONDS) {
            Serial.println("Timed out waiting for wifi startup, sleeping...");
            ESP.deepSleep(RETRY_INTERVAL); // Go to sleep for a while, then reboot
        }
    }
    Serial.println("Waiting for the ESP IP...");
    delay(5000);
    Serial.println(WiFi.localIP());

    // Send mDNS query for server
    my_mdns.Clear();
    struct mdns::Query query_mqtt;
    strncpy(query_mqtt.qname_buffer, QUERY_HOSTNAME, MAX_MDNS_NAME_LEN);

    query_mqtt.qtype = MDNS_TYPE_A;
    query_mqtt.qclass = 1;    // "INternet"
    query_mqtt.unicast_response = 0;

    my_mdns.AddQuery(query_mqtt);
    my_mdns.Send();

    started = millis(); // reuse var from wifi loop

    answerHost[0] = '\0'; // make sure name is empty at start of loop

    while (strlen(answerHost)==0) {
        my_mdns.loop();
        if ((millis()-started)/1000 > MDNS_WAIT_SECONDS) {
            Serial.println("Timed out waiting for proper mDNS reply, sleeping...");
            ESP.deepSleep(RETRY_INTERVAL); // try again in a while after rebooting
        }
    }
}

void loop() {
    char http_buffer[80];
    HTTPClient http;
        // Sensor readings may also be up to 2 seconds 'old' (it's a very slow sensor)
        float hp = dht.readHumidity();
        // Read temperature as Celsius (the default)
        float tc = dht.readTemperature();
        // Read temperature as Fahrenheit (isFahrenheit = true)
        float tf = dht.readTemperature(true);
        // Check if any reads failed and exit early (to try again).

        if (isnan(hp) || isnan(tc) || isnan(tf)) {
            ESP.deepSleep(RETRY_INTERVAL); // Try again after a while
        } else {
            dtostrf(tc, -6, 2, celsiusTemp); // negative width does left-justify
            dtostrf(tf, -6, 2, fahrenheitTemp);
            dtostrf(hp, -6, 2, humidityPercent);
            sprintf(
                http_buffer,
                "http://%s:%d/esp_sample?tc=%s&tf=%s&hp=%s", // Change this to your needs
                answerHost,
                HOST_PORT,
                celsiusTemp,
                fahrenheitTemp,
                humidityPercent
            );

            http.begin(http_buffer);
            // Yeah, I know this technically should be a POST. So sue me. :p
            int resultCode = http.GET();
        }
    ESP.deepSleep(CYCLE_SLEEP_INTERVAL);
}
