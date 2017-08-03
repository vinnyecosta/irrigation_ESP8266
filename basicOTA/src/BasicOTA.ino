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
//EEPROM
#include <EEPROM.h>
#define MEM_ALOC_SIZE 1024
// #include <String.h>
#define ADD         1
#define REMOVE      2
#define REMOVE_ALL  3
#define EDIT        4
Ticker tickerSetHigh;
Ticker tickerSetLow;
char tick;
u8 en_timeon = 0;
u32 reg_timeoff;

String inputString = "";         // a string to hold incoming data
boolean stringComplete = false;  // whether the string is complete
u8 i;
// Structure to hold the values received from the APP

MDNSResponder mdns;
ESP8266WebServer server(80);
WiFiClient client;

// GLOBAL VARIABLES DECLARATION
typedef struct data_t{
  u8 weekday;
  u8 add_rem_edt;
  u8 sethour;
  u8 setminute;
  u8 sethouron;
  u8 setminuteon;
} dt_t;
dt_t dt;
dt_t *dt_p;
#define ESPGPIO_D2 gpio4_pin
//enum days{  sun, mon, tue, wed, thu, fri, sat  };

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

/*
* Called at every 1 second
* Input   : state
* Output  : None
* Desc    : Check if there is any valid entry into memory at every 1 second
*           if it does, turn the output on, off otherwise
*/
void setPin(int state) {
  u16 hm;
  if(tick == 0)
     tick = 1;
  else
     tick = 0;
     ttime = ttime==86399 ? 0 : ttime + 1;
     hm = calc_timer();
     if(search_timeeeprom(hm)==1)
        digitalWrite(ESPGPIO_D2, 1);
     else if (reg_timeoff==ttime)
     {
       digitalWrite(ESPGPIO_D2, 0);
       en_timeon = 0;
     }
    //  Serial.printf("timeroff -> %d\n", reg_timeoff);
    //  Serial.printf("ttime -> %d\n", ttime);

  //digitalWrite(13, m&1);
}

void setup() {
  dt_p = &dt;
  EEPROM.begin(MEM_ALOC_SIZE);
  //START OTA
  Serial.begin(115200);

  Serial.println("Booting");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  IPAddress subnet(255, 255, 255, 0);
  WiFi.config(IPAddress(192, 168, 0, 125),
              IPAddress(192, 168, 0, 1), subnet);
 // WiFi.config(IPAddress(192, 168, 43, 125),
            // IPAddress(192, 168, 43, 1), subnet);
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
  pinMode(ESPGPIO_D2, OUTPUT);
  digitalWrite(ESPGPIO_D2, LOW);
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
    // Serial.print("hostheader->");
    // Serial.println(server.hostHeader());
    // Serial.print("uri->");
    // Serial.println(server.uri());
    // Serial.print("argname(0)->");
    // Serial.println(server.argName(0));
    // Serial.print("argname(1)->");
    // Serial.println(server.argName(1));
    // Serial.print("argname(2)->");
    // Serial.println(server.argName(2));
    // Serial.print("argname(3)->");
    // Serial.println(server.argName(3));
    // Serial.print("argname(4)->");
    // Serial.println(server.argName(4));
    // Serial.print("argname(5)->");
    // Serial.println(server.argName(5));
    // Serial.print("argname(6)->");
    // Serial.println(server.argName(6));
    // Serial.print("args->");
    // Serial.println(server.args());
    // Serial.print("header(0)->");
    // Serial.println(server.header(0));
    // Serial.print("header(1)->");
    // Serial.println(server.header(1));
    // digitalWrite(ESPGPIO_D2, HIGH);
    // if (opcode == 1) {
      // for (i = 1; i < server.args(); i++)
      // {
    u16 entry_found;
    dt_p->weekday     = server.argName(1).toInt();
    dt_p->add_rem_edt = server.argName(2).toInt();
    dt_p->sethour     = server.argName(3).toInt();
    dt_p->setminute   = server.argName(4).toInt();
    dt_p->sethouron   = server.argName(5).toInt();
    dt_p->setminuteon = server.argName(6).toInt();
    if (dt_p->weekday == 2)
      removeall_data2eeprom();
    //   entry_found = search_entryeeprom();
    // else
    //   removeall_data2eeprom();
    entry_found = search_entryeeprom();
    switch (dt_p->add_rem_edt) {
      case ADD:
      // Check if there is any input with the same hash CODE
      // save to eeprom memory
        if(entry_found != 0xFFFF)
        {
          Serial.println("Entrou no 1");
          save_data2eeprom(entry_found);
        }
        else if(entry_found==0xFFFF)
        {
          Serial.println("Entrou no 2");
          edit_data2eeprom();
        }
      break;
      case REMOVE:
      // remove from eeprom memory
        if(entry_found == 0xFFFF)
          remove_data2eeprom();
      break;
      case REMOVE_ALL:
      // remove from eeprom memory
          removeall_data2eeprom();
      break;
      // case EDIT:
      // // edit and save to eeprom memory
      //   if(entry_found==0xFFFF)
      //     edit_data2eeprom();
      // break;
    }
    // Serial.print("weekday->");
    // Serial.println(dt_p->weekday);
    // Serial.print("add_rem_edt->");
    // Serial.println(dt_p->add_rem_edt);
    // Serial.print("sethour->");
    // Serial.println(dt_p->sethour);
    // Serial.print("setminute->");
    // Serial.println(dt_p->setminute);
    // Serial.print("sethouron->");
    // Serial.println(dt_p->sethouron);
    // Serial.print("setminuteon->");
    // Serial.println(dt_p->setminuteon);
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
    digitalWrite(ESPGPIO_D2, HIGH);
    delay(1000);
  });
  server.on("/socket2Off", []() {
    server.send(200, "text/html", webPage);
    digitalWrite(ESPGPIO_D2, LOW);
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
  tickerSetLow.attach_ms(1000, setPin, 0);

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
  // digitalWrite(gpio13_pin, HIGH);
  // digitalWrite(gpio12_pin, HIGH);
  // digitalWrite(ESPGPIO_D2, HIGH);

  //START OTA
  // ArduinoOTA.handle();
  serialEvent();

  server.handleClient();
  // if (!client)
  // {
  //   return;
  // }
  // Serial.println("Somebody has connected :)");
  // String request = client.readString();
  // Serial.println(request);
}
