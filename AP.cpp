#include <WiFi.h>
#include <WebSocketsServer.h>  // Asegúrate de haber instalado esta librería

const char* apSSID = "nombreOlimex";  
const char* apPassword = "12345678";  

WiFiServer server(80);  
WebSocketsServer webSocket = WebSocketsServer(81); // WebSocket en el puerto 81

#define RELAY1 32  
#define RELAY2 33  

// **DECLARACIÓN DE LA FUNCIÓN ANTES DE setup()**
void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length);

void setup() {
  Serial.begin(9600);
  pinMode(RELAY1, OUTPUT);
  pinMode(RELAY2, OUTPUT);
  digitalWrite(RELAY1, LOW);
  digitalWrite(RELAY2, LOW);

  WiFi.softAP(apSSID, apPassword);

  Serial.println("WiFi AP started!");
  Serial.print("Connect to: ");
  Serial.println(apSSID);
  Serial.print("Then open: http://");
  Serial.println(WiFi.softAPIP());

  server.begin();
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);  
}

void loop() {
  webSocket.loop();

  WiFiClient client = server.available();
  if (client) {
    Serial.println("New client connected");

    String request = "";
    while (client.available()) {
      char c = client.read();
      request += c;
    }

    Serial.println("Request: " + request);

    client.println("HTTP/1.1 200 OK");
    client.println("Content-type:text/html");
    client.println("Connection: close");
    client.println();
    
    client.println("<!DOCTYPE html><html><head>");
    client.println("<meta name='viewport' content='width=device-width, initial-scale=1'>");
    client.println("<script>");
    client.println("var socket = new WebSocket('ws://' + location.hostname + ':81/');");
    
    client.println("function sendRelayCommand(cmd) { socket.send(cmd); }");

    client.println("socket.onmessage = function(event) {");
    client.println("  var state = JSON.parse(event.data);");
    client.println("  document.getElementById('relay1').textContent = state.relay1 ? 'ON' : 'OFF';");
    client.println("  document.getElementById('relay2').textContent = state.relay2 ? 'ON' : 'OFF';");
    client.println("};");

    client.println("</script>");
    client.println("<style>");
    client.println("body { font-family: Arial, sans-serif; text-align: center; }");
    client.println(".container { margin-top: 50px; }");
    client.println(".relay-block { margin-bottom: 20px; }");
    client.println("button { font-size: 30px; padding: 10px; width: 150px; margin: 5px; display: block; }");
    client.println(".on { background-color: green; color: white; }");
    client.println(".off { background-color: red; color: white; }");
    client.println("</style>");
    client.println("</head><body>");
    
    client.println("<h1>ESP32 Relay Control</h1>");
    client.println("<div class='container'>");

    client.println("<div class='relay-block'>");
    client.println("<h2>Relay 1</h2>");
    client.println("<button id='relay1' class='on' onclick=\"sendRelayCommand('relay1_on')\">ON</button>");
    client.println("<button class='off' onclick=\"sendRelayCommand('relay1_off')\">OFF</button>");
    client.println("</div>");

    client.println("<div class='relay-block'>");
    client.println("<h2>Relay 2</h2>");
    client.println("<button id='relay2' class='on' onclick=\"sendRelayCommand('relay2_on')\">ON</button>");
    client.println("<button class='off' onclick=\"sendRelayCommand('relay2_off')\">OFF</button>");
    client.println("</div>");

    client.println("</div>");
    client.println("</body></html>");
    
    delay(10);
    client.stop();
    Serial.println("Client disconnected");
  }
}

// **DEFINICIÓN de webSocketEvent()**
void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
  String message = String((char *)payload);

  if (message == "relay1_on") {
    digitalWrite(RELAY1, HIGH);
  } else if (message == "relay1_off") {
    digitalWrite(RELAY1, LOW);
  } else if (message == "relay2_on") {
    digitalWrite(RELAY2, HIGH);
  } else if (message == "relay2_off") {
    digitalWrite(RELAY2, LOW);
  }

  // **Enviar estado actualizado de los relés**
  String relayState = "{\"relay1\": " + String(digitalRead(RELAY1)) + ", \"relay2\": " + String(digitalRead(RELAY2)) + "}";
  webSocket.broadcastTXT(relayState);
}
