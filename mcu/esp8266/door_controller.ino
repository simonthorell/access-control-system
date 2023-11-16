/* This code is for the ESP8266-based microcontroller. 
It receives commands from a server application and controls the LEDs accordingly. 
The pins used for LED's are D1 and D2 which could instead be routed to a relay to control a door lock.
*/ 

#include <ESP8266WiFi.h>

// when sending TCP/IP command, do not forget to end string with \r. 
// EXAMPLE: result = wifiWrite("DOOR_UNLOCKED\r"); 

const char* ssid = "YOUR_SSID";
const char* password = "YOUR_PASSWORD";

IPAddress local_IP(192, 168, XX, XX); // REPLACE WITH YOUR DEVICE IP ADDRESS OF CHOICE
IPAddress gateway(192, 168, XX, X);   // REPLACE WITH YOUR GATEWAY ADDRESS
IPAddress subnet(255, 255, 255, 0);

const int greenLED = D1;
const int redLED = D2;

bool greenLedOn = false;
bool redLedOn = false;

const unsigned long COMMAND_DEBOUNCE_MS = 1000;
const unsigned long LED_DURATION = 3000;
const int SERVER_PORT = 23;

WiFiServer server(SERVER_PORT);

unsigned long lastCommandTime = 0; // Move this declaration outside of any functions

void setup() {
  pinMode(greenLED, OUTPUT);
  pinMode(redLED, OUTPUT);
  Serial.begin(115200);

  if (!configureWiFi()) {
    Serial.println("Failed to configure Wi-Fi.");
    // Handle error if needed
  }

  if (!startServer()) {
    Serial.println("Failed to start the server.");
    // Handle error if needed
  }

  initializeHardware();
}

void loop() {
  WiFiClient client = server.available();
  if (client) {
    // Handle client request and maintain the connection
    handleClientRequest(client);
  }
}

bool configureWiFi() {
  // Wi-Fi configuration code here
  WiFi.hostname("wemos_d1_mini");
  return WiFi.config(local_IP, gateway, subnet) && WiFi.begin(ssid, password) == WL_CONNECTED;
}

bool startServer() {
  // Server startup code here
  server.begin();
  return true; // Always return true since server.begin() doesn't return a value
}

void initializeHardware() {
  // LED and serial initialization code here
  digitalWrite(greenLED, LOW);
  digitalWrite(redLED, LOW);

  // Blink the red LED once while waiting for Wi-Fi connection
  digitalWrite(redLED, HIGH);
  digitalWrite(greenLED, HIGH);
  delay(1000);
  digitalWrite(redLED, LOW);
  digitalWrite(greenLED, LOW);
  
  // Check for Wi-Fi connection
  while (WiFi.status() != WL_CONNECTED) {
    // Blink the red LED while waiting
    digitalWrite(redLED, HIGH);
    delay(500);
    digitalWrite(redLED, LOW);
    delay(500);
  }
  
  // When Wi-Fi is connected, light up the green LED for 2 seconds
  digitalWrite(greenLED, HIGH);
  delay(3000);
  digitalWrite(greenLED, LOW);
}

void handleClientRequest(WiFiClient client) {
  while (client.connected()) {
    if (client.available()) {
      String request = client.readStringUntil('\r');
      Serial.println(request);

      unsigned long currentTime = millis();
      if (currentTime - lastCommandTime > COMMAND_DEBOUNCE_MS) {
        if (request.indexOf("DOOR_UNLOCKED") != -1) {
          greenLedOn = true;
          redLedOn = false;
        } else if (request.indexOf("DOOR_LOCKED") != -1) {
          redLedOn = true;
          greenLedOn = false;
        }
        lastCommandTime = currentTime;
      }

      client.println("Command received and executed.");
    }

    handleLEDs();
  }

  // Close the client connection when it's no longer connected
  client.stop();
}

void handleLEDs() {
  if (greenLedOn && millis() - lastCommandTime < LED_DURATION) {
    digitalWrite(greenLED, HIGH);
  } else {
    digitalWrite(greenLED, LOW);
  }

  if (redLedOn && millis() - lastCommandTime < LED_DURATION) {
    digitalWrite(redLED, HIGH);
  } else {
    digitalWrite(redLED, LOW);
  }

  if (millis() - lastCommandTime >= LED_DURATION) {
    greenLedOn = false;
    redLedOn = false;
  }
}
