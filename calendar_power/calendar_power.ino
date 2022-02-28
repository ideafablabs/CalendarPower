// calendar_power = control a power relay from a vcalendar 2.0 feed
// 2/26/2022 - sfranzyshen@ for Idea Fab Labs, chico ca

#define USING_AXTLS
#include <ESP8266WiFi.h>

#include "RTClib.h"
RTC_Millis rtc;

// force use of AxTLS
#include <WiFiClientSecureAxTLS.h>
using namespace axTLS;

#include "secret.h"
#ifndef STASSID
#define STASSID   "enter ssid"          // "MyWiFi"
#define STAPSK    "enter password"      // "pennygetyourownwifi"
#endif

#ifndef HOSTNAME
#define HOSTNAME  "enter host name"     // "calendar.google.com"
#define HOSTPRINT "enter finger print"  // "8e 4a b0 91 50 92 3e 08 a2 cb 65 fd c1 df 57 4f f5 ed 90 bd"
#define HOSTURL   "enter host url"      // "/calendar/ical/username%40gmail.com/public/basic.ics"
#endif

const char* ssid = STASSID;
const char* password = STAPSK;

const char* host = HOSTNAME;
const int httpsPort = 443;

// SHA1 fingerprint of the certificate
const char* fingerprint = HOSTPRINT;

bool  ACTIVE = 0;
int   LASTHOUR = 0;
int   OUTPIN = LED_BUILTIN;

void setup() {
  Serial.begin(115200);
  pinMode(OUTPIN, OUTPUT);
  digitalWrite(OUTPIN, HIGH);
  // sets the RTC to the date & time this sketch was compiled
  rtc.begin(DateTime(F(__DATE__), F(__TIME__)));
  Serial.println();
  Serial.print("connecting to ");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  check_calendar();
}

void check_calendar() {
  int MM;
  String MMM, NOW, URL = HOSTURL;
  char format1[] = "YYYYMMDDThh0000Z";
  char format2[] = "YYYYMMDDThhmmssZ";
  DateTime now;
  
  // Use WiFiClientSecure class to create TLS connection
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored  "-Wdeprecated-declarations"
  WiFiClientSecure client;
#pragma GCC diagnostic pop
  Serial.print("connecting to ");
  Serial.println(host);
  if (!client.connect(host, httpsPort)) {
    Serial.println("connection failed");
    return;
  }
  if (client.verify(fingerprint, host)) {
    Serial.println("certificate matches");
  } else {
    Serial.println("certificate doesn't match");
    //return;
  }
  Serial.println("requesting URL");
  client.print(String("GET ") + URL + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "User-Agent: VCalendarCotrol\r\n" +
               "Connection: close\r\n\r\n");
               
  while (client.connected()) {
    String header = client.readStringUntil('\n');
    if(header == "\r") {
      Serial.println("end of headers received");
      break;
    } else {
      header.toUpperCase();
      if (header.startsWith("DATE: ")) {
        MMM = header.substring(14, 17);
        
        if(MMM == "JAN") {
          MM = 1;
        } 
        else if(MMM == "FEB") {
          MM = 2;
        }
        else if(MMM == "MAR") {
          MM = 3;
        }
        else if(MMM == "APR") {
          MM = 4;
        }
        else if(MMM == "MAY") {
          MM = 5;
        }
        else if(MMM == "JUN") {
          MM = 6;
        }
        else if(MMM == "JUL") {
          MM = 7;
        }
        else if(MMM == "AUG") {
          MM = 8;
        }
        else if(MMM == "SEP") {
          MM = 9;
        }
        else if(MMM == "OCT") {
          MM = 10;
        }
        else if(MMM == "NOV") {
          MM = 11;
        }
        else if(MMM == "DEC") {
          MM = 12;
        }
        
        // sets the RTC to the date & time retrieved from the header
        rtc.adjust(DateTime(header.substring(17, 22).toInt(), MM, header.substring(11, 13).toInt(), header.substring(23, 25).toInt(), header.substring(26, 28).toInt(), header.substring(29, 31).toInt()));
        now = rtc.now();
        NOW = now.toString(format1);
        Serial.print("Setting RTC: ");
        Serial.println(now.toString(format2));
        LASTHOUR = now.hour();
      }
    }
  }
  while (client.connected()) {
    String calendar = client.readStringUntil('\n');
    if(calendar == "\r") {
      Serial.println("end of calendar received");
      if(ACTIVE) {
        Serial.println("Turning Off ...");
        digitalWrite(OUTPIN, HIGH);
        ACTIVE = 0;
      } else {
        Serial.println("Already Off ...");
      }
      break;
    } else {
      calendar.toUpperCase();
      if(calendar.startsWith("DTSTAMP:")) {
        if(calendar.substring(8,24) == NOW) {
          if(!ACTIVE) {
            Serial.println("Turning On ...");
            digitalWrite(OUTPIN, LOW);
            ACTIVE = 1;
          } else {
            Serial.println("Already On ...");
          }
          break;
        }
      }
    }
  }
  Serial.println("closing connection");
}

void loop() {
  DateTime now = rtc.now();
  char buf[] = "YYYYMMDDThhmmssZ";
  Serial.println(now.toString(buf));
  if(LASTHOUR != now.hour()) {
      check_calendar();
  }
  delay(60000);
}
