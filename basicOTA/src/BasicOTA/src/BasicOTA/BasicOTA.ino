#include <Arduino.h>

//START OTA
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
//TICKER
#include <Ticker.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <String.h>

Ticker tickerSetHigh;
Ticker tickerSetLow;
char tick;


String inputString = "";         // a string to hold incoming data
boolean stringComplete = false;  // whether the string is complete

#define mon stp[0]
#define tue stp[1]
#define wed stp[2]
#define thu stp[3]
#define fri stp[4]
#define sat stp[5]
#define sun stp[6]

MDNSResponder mdns;
ESP8266WebServer server(80);
WiFiClient client;

// GLOBAL VARIABLES DECLARATION
typedef struct st_0 {
  u8 en;
  u16 h_on1;
  u8 t_on1;
  u16 h_on2;
  u8 t_on2;
  u16 h_on3;
  u8 t_on3;
  } st;
//enum days{  sun, mon, tue, wed, thu, fri, sat  };
  st *stp[7];
String webPage = "";
int gpio13_pin = 13;
int gpio4_pin = 4;
int gpio12_pin = 12;
u8 s = 0, m = 0, h = 0;
u32 ttime = 0;

//START OTA
const char* ssid = "ParadinhaSex";
const char* password = "MinasParaiBA";

// const char* ssid = "Marcia";
// const char* password = "15011985";

// const char* ssid = "Xperia_Z2_8db7";
// const char* password = "556677889900";

//ROTINA PARA CONTROLAR A HORA POR NUMEROS INTEIROS
int calc_timer()
{
    u32 var = 0;
    u8 s, m, h;
    h = ttime/3600;
    var = h>=1 ? ttime - (h*3600) : ttime;
    m = var/60;
    s = var%60;
}
//ROTINA PARA CONVERTER HORA EM NUMERO
u32 calc_rtimer(u8 h, u8 m, u8 s)
{
    return h*m*s;
}

// SET PIN EVERY SECOND
void setPin(int state) {
  if(tick == 0)
     tick = 1;
  else
     tick = 0;
     calc_timer();
     ttime = ttime==86399 ? 0 : ttime + 1;
  digitalWrite(12, tick);
  //digitalWrite(13, m&1);
}
// SERIAL INTERRUPT

void serialEvent()
{
  while (Serial.available())
  {
    // get the new byte:
    char inChar = (char)Serial.read();
    // add it to the inputString:
    inputString += inChar;
    if (inChar == 'a')
    {
      digitalWrite(13, 0);
    }
    else if (inChar == 'b')
    {
      digitalWrite(13, 1);
    }
    // if the incoming character is a newline, set a flag
    // so the main loop can do something about it:
    if (inChar == '\n')
    {
      stringComplete = true;
    }
  }
}

void setup() {
  //START OTA
  Serial.begin(115200);

  Serial.println("Booting");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  IPAddress subnet(255, 255, 255, 0);
  // WiFi.config(IPAddress(192, 168, 1, 125),
              // IPAddress(192, 168, 1, 1), subnet);
 WiFi.config(IPAddress(192, 168, 43, 125),
            IPAddress(192, 168, 43, 1), subnet);
while (WiFi.waitForConnectResult() != WL_CONNECTED)
  {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }

  // Port defaults to 8266
  // ArduinoOTA.setPort(8266);

  // Hostname defaults to esp8266-[ChipID]
  // ArduinoOTA.setHostname("myesp8266");

  // No authentication by default
  // ArduinoOTA.setPassword((const char *)"123");

  ArduinoOTA.onStart([]() {
    Serial.println("Start");
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("End");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\n", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });
  ArduinoOTA.begin();
  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  //END OTA

  //START APP CODE
  webPage += "<h1>ESP8266 Web Server</h1>";
  webPage += "<p>Socket #1 <a href=\"socket1On\"><button>ON</button></a>&nbsp;<a href=\"socket1Off\"><button>OFF</button></a></p>";
  webPage += "<p>Socket #2 <a href=\"socket2On\"><button>ON</button></a>&nbsp;<a href=\"socket2Off\"><button>OFF</button></a></p>";
  webPage += "<p>Socket #3 <a href=\"socket3On\"><button>ON</button></a>&nbsp;<a href=\"socket3Off\"><button>OFF</button></a></p>";
  webPage += "<p>Socket #4 <input type=\"submit\" name=\"b3\" id=\"b3id\" value=\"ON\" /><input type=\"submit\" name=\"b4\" id=\"b4id\" value=\"OFF\" /></p>";

  // preparing GPIOs
  pinMode(gpio13_pin, OUTPUT);
  digitalWrite(gpio13_pin, LOW);
  pinMode(gpio4_pin, OUTPUT);
  digitalWrite(gpio4_pin, LOW);
  pinMode(gpio12_pin, OUTPUT);
  digitalWrite(gpio12_pin, LOW);

  delay(1000);
  Serial.begin(115200);
  //WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (mdns.begin("esp8266", WiFi.localIP()))
  {
    Serial.println("MDNS responder started");
  }

  server.on("/", []() {
    server.send(200, "text/html", webPage);
    check_input();
    Serial.print("0->");
    Serial.println(server.hostHeader());
    Serial.print("1->");
    Serial.println(server.uri());
    Serial.print("2->");
    Serial.println(server.argName(0));
    Serial.print("3->");
    Serial.println(server.argName(1));
    Serial.print("4->");
    Serial.println(server.argName(2));
    Serial.print("5->");
    Serial.println(server.args());
    Serial.print("6->");
    Serial.println(server.header(0));
    Serial.print("7->");
    Serial.println(server.header(1));
    digitalWrite(gpio4_pin, HIGH);
  });
  server.on("/socket1On", []() {
    server.send(200, "text/html", webPage);
    digitalWrite(gpio13_pin, HIGH);
    delay(1000);
  });
  server.on("/socket1Off", []() {
    server.send(200, "text/html", webPage);
    digitalWrite(gpio13_pin, LOW);
    delay(1000);
  });
  server.on("/socket2On", []() {
    server.send(200, "text/html", webPage);
    digitalWrite(gpio4_pin, HIGH);
    delay(1000);
  });
  server.on("/socket2Off", []() {
    server.send(200, "text/html", webPage);
    digitalWrite(gpio4_pin, LOW);
    delay(1000);
  });
  server.on("/socket3On", []() {
    server.send(200, "text/html", webPage);
    digitalWrite(gpio12_pin, HIGH);
    delay(1000);
  });
  server.on("/socket3Off", []() {
    server.send(200, "text/html", webPage);
    digitalWrite(gpio12_pin, LOW);
    delay(1000);
  });
  server.begin();
  Serial.println("HTTP server started");
  digitalWrite(gpio13_pin, HIGH);

  // every 100 ms, call setPin(0)
  tickerSetLow.attach_ms(100, setPin, 0);

}
//ENDOTA
void check_input(void)
{
  if (server.argName(0) == "global_timer")
  {
    //TODO PROBLEMA AQUI
    ttime = (server.arg(1)).toInt()*(server.arg(2)).toInt();
    Serial.print("000->");
    Serial.println(ttime);
  }

}
void loop()
{
  //START OTA
  ArduinoOTA.handle();

  server.handleClient();
  if (!client)
  {
    return;
  }
  Serial.println("Somebody has connected :)");
  String request = client.readString();
  Serial.println(request);

}
