#include <SPI.h>
#include <WiFi101.h>
#include <ArduinoJson.h>
///////please enter your sensitive data in the Secret tab/arduino_secrets.h
char ssid[] = "";        // your network SSID (name)
char pass[] = "";    // your network password (use for WPA, or use as key for WEP)

int status = WL_IDLE_STATUS;
// if you don't want to use DNS (and reduce your sketch size)
// use the numeric IP instead of the name for the server:
//IPAddress server(74,125,232,128);  // numeric IP for Google (no DNS)
char server[] = "";    // name address for Google (using DNS)

// get json buffer
String jsonData; // stores the json data from the server
boolean startToRead; // for header removal
StaticJsonDocument<500> jsonBuffer; // json buffer

// Initialize the Ethernet client library
// with the IP address and port of the server
// that you want to connect to (port 80 is default for HTTP):
WiFiClient client;

void setup() {
  // for read and analyse json
  jsonData = "";
  startToRead = false;

  //Initialize serial and wait for port to open:
  // setup WiFi connection
  WiFi.setPins(8, 7, 4, 2);
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  // check for the presence of the shield:
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    // don't continue:
    while (true);
  }

  // attempt to connect to WiFi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(10000);

//    // send one http request
//    httpRequest();
  }

  Serial.println("Connected to wifi");
  printWiFiStatus();
}

void loop() {
  // if there are incoming bytes available
  // from the server, read them and print them:    
  // send one http request

  httpPostRequest();
  delay(1000);

  Serial.println(readResponseFromServer(jsonData, startToRead));

  // if the server's disconnected, stop the client:
  if (!client.connected()) {
    Serial.println();
    Serial.println("disconnecting from server.");
    client.stop();

    // do nothing forevermore:
    // while (true);
  }
}


void printWiFiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}

void httpGetRequest() {
  // Http request
  Serial.println("\nStarting connection to server...");
  // if you get a connection, report back via serial:
  if (client.connect(server, 80)) {
    Serial.println("connected to server");
    // Make a HTTP request:
    client.println("GET / HTTP/1.1");
    client.println("Host: zdlin.pythonanywhere.com");
    client.println("Connection: close");
    client.println();
  }
}

String readResponseFromServer (String jsonData, boolean startToRead) {
  // After http request you may want to read the json data which send by the server
  while (client.available()) { // read reponse from server
    char c = client.read();
    if (c == '{') { // start of the json
      startToRead = true;
    }
    if (startToRead) {
      jsonData = String(jsonData + String(c));
    }
  }
  // analyse the json we got
  deserializeJson(jsonBuffer, jsonData);
  const char* state = jsonBuffer["0"];
  
  // reset json global variables
  startToRead = false;
  jsonData = "";
  return state; // use this value to control light's behaviour
}

void httpPostRequest() {
  // Http request
  Serial.println("\nStarting connection to server...");
  // if you get a connection, report back via serial:
  if (client.connect(server, 80)) {
    char postData[] = "{\"id\": 0, \"behaviour\": \"abnormal\"}";
    Serial.println("connected to server");
    // Make a HTTP request:
    client.println("POST /update_behaviour HTTP/1.1");
    client.println("Host: ");
    client.println("Connection: close");
    client.println("Content-Type: application/json");
    client.print("Content-Length: ");
    client.println(strlen(postData));
    client.println();
    client.println(postData);
    Serial.println(postData);
  }
}
