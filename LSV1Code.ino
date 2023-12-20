#include <WiFiClient.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

// WiFi settings
const char* ssid = "Haras_Home";
const char* password = "Tamaer2541";

// Philips Hue settings
const char* hueBridgeIP = "192.168.86.83";
const char* hueAPIKey = "yEJeNtnrBVwN2EI0WXIMFsI8J4Dwjq2SAg8ioy2U";
const int hueLightID = 18;  // The ID of the light you want to control. Change as needed.

// Button settings
const int buttonPin = 5;  // D1 on NodeMCU
int buttonState;
int lastButtonState = HIGH;
unsigned long lastDebounceTime = 0;
const long debounceDelay = 50;

void setup() {
  pinMode(buttonPin, INPUT_PULLUP);
  
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
}

void loop() {
  //if not connected to wifi, attempts to reconnect
  if (WiFi.status() != WL_CONNECTED) {
    reconnectWiFi();
  }
  
  int buttonState = digitalRead(buttonPin);

  if (buttonState == LOW && lastButtonState == HIGH) { // The button is pressed and was not pressed in the last loop
  Serial.println("Button Pressed!");

    if (WiFi.status() == WL_CONNECTED) {
      bool isLightOn = getLightState();
      
      HTTPClient http;
      WiFiClient client;
      String url = "http://" + String(hueBridgeIP) + "/api/" + String(hueAPIKey) + "/lights/" + hueLightID + "/state";
      
      http.begin(client, url);
      http.addHeader("Content-Type", "text/plain");
  
      // Here you can decide if you want to turn the light ON or OFF or toggle.
      int httpCode;
      if (isLightOn){
        httpCode = http.PUT("{\"on\":false}");
      }
      else{
        httpCode = http.PUT("{\"on\":true}");
      }
      
      Serial.println(httpCode);
      http.end();
    }
  
    delay(1000);  // Debounce delay. Adjust if necessary.
  }

  lastButtonState = buttonState;  // Store the current state of the button for the next loop
}

bool getLightState() {
  HTTPClient http;
  WiFiClient client;
  
  String url = "http://" + String(hueBridgeIP) + "/api/" + String(hueAPIKey) + "/lights/" + hueLightID;
  http.begin(client, url);
  
  int httpCode = http.GET();
  if(httpCode == HTTP_CODE_OK) {
    String payload = http.getString();
    return payload.indexOf("\"on\":true") != -1;  // If "on":true is in the response, the light is on.
  }
  http.end();
  return false;
}

void reconnectWiFi() {
  // Disconnect if already trying to connect
  WiFi.disconnect();

  // Begin connecting
  WiFi.begin(ssid, password);

  // Wait for connection
  int retryCount = 0;
  while (WiFi.status() != WL_CONNECTED && retryCount < 10) {  // retry for up to 10 times
    delay(1000);
    Serial.println("Attempting to reconnect to WiFi...");
    retryCount++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("Connected to WiFi");
  } else {
    Serial.println("Failed to reconnect after multiple attempts.");
  }
}




  //Debugging
//  int httpCode = http.GET();
//  if (httpCode > 0) {
//    String payload = http.getString();
//    Serial.print("Response Code: ");
//    Serial.println(httpCode);
//    Serial.print("Payload: ");
//    Serial.println(payload);
//    // rest of the code
//  } else {
//    Serial.print("Failed with HTTP Code: ");
//    Serial.println(httpCode);
//  }
////////
 
