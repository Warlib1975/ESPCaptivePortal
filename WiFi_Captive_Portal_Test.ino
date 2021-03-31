#define LOG_LOCAL_LEVEL ESP_LOG_VERBOSE

#include <WiFi.h>
#include <DNSServer.h>
#include <WebServer.h>
//#include "esp32-hal-log.h"
#include "esp_log.h"

const byte DNS_PORT = 53;
//IPAddress apIP(8,8,4,4); // The default android DNS
IPAddress apIP(192,168,20,1); // The default android DNS
DNSServer dnsServer;
WiFiServer server(80);

String responseHTML_1 = ""
  "<!DOCTYPE html><html><head><title>CaptivePortal</title></head><body>"
  //"<script type=\"text/javascript\">"
  //"document.querySelector(document).addEventListener('click', 'a[target=\"_blank\"]', function(ev) {"
  //"var url;ev.preventDefault();url = document.querySelector(this).attr('href');window.open(url, '_system');});"
  //"</script>"
  "<h1>Hello World!</h1><p>This is a captive portal example. All requests will "
  "be redirected here.<a href=\"http://{IP_address}\" target=\"_blank\">URL test<a></p></body></html>"; //target=\"_system\"

String responseHTML_2 = ""
  "<!DOCTYPE html><html><head><title>CaptivePortal</title></head><body>"
  "<h1>URL clicked</h1><p>This is a captive portal example.</p></body></html>";


const char* ssid     = "WiFi router name";     
const char* password = "xxx";  

void handleRoot() {
  String ip = "www.evan.ru/test"; //"91.236.11.32"; //apIP.toString();
  responseHTML_1.replace("{IP_address}", ip);
  //server.send(200, "text/html", responseHTML_1);
}

void handleTest() {
  //server.send(200, "text/html", responseHTML_2);
}

/*void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";

  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }

  server.send(404, "text/plain", message);
}*/

void setup() { 
  WiFi.mode(WIFI_AP); //WIFI_AP_STA
  WiFi.softAP("ESP32-DNSServer");
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));

  WiFi.begin(); //(ssid, password); 

  Serial.begin(9600);   
  Serial.setDebugOutput(true);
  
  esp_log_level_set("*", ESP_LOG_VERBOSE);
    log_v("test output - verbose");
  
  Serial.println();
  Serial.println();
  Serial.println("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.print("ESP Mac Address: ");
  Serial.println(WiFi.macAddress());
  Serial.print("Subnet Mask: ");
  Serial.println(WiFi.subnetMask());
  Serial.print("Gateway IP: ");
  Serial.println(WiFi.gatewayIP());
  Serial.print("DNS: ");
  Serial.println(WiFi.dnsIP());

  //server.onNotFound([]() {
  //  char * msg = "HELLO<br>Default landing page<br>";
  //  server.send(200, "text/html", msg );
  //});

  // if DNSServer is started with "*" for domain name, it will reply with
  // provided IP to all DNS request
  dnsServer.start(DNS_PORT, "*", apIP);

  //server.onNotFound(handleNotFound);
  //server.on("/", handleRoot);
  //server.on("/test", handleTest);
  
  server.begin();
  Serial.println("HTTP server started");
}


void loop() {
  dnsServer.processNextRequest();
  //server.handleClient(); //Handling of incoming requests
  
  WiFiClient client = server.available();   // listen for incoming clients
  bool start_flag = true;

  if (client) {
    String currentLine = "";
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.write(c);
        if (c == '\n') 
        {
          if (start_flag & currentLine.length() == 0) { ///(currentLine.endsWith("GET /")) {//
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();
            //String ip = WiFi.localIP().toString();
            String ip = "www.evan.ru/test"; //"91.236.11.32"; //apIP.toString();
            responseHTML_1.replace("{IP_address}", ip);
            client.print(responseHTML_1);
            start_flag = false;
            break;
          } else {
            currentLine = "";
          }
        } 
        else 
        if (c != '\r') {
          currentLine += c;
        }

        // Check to see if the client request was "GET /test":
        if (currentLine.endsWith("GET /test")) 
        {
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();
            client.print(responseHTML_2);
            break;
        }
      }
    }  
    client.stop();
    Serial.println("Client Disconnected.");
  }
}
