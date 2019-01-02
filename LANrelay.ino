//ALL CREDITS TO http://academicfox.com/aurduino-ethernet-module-enc28j60-or-web-control-relay-arduino-webserver/

/* 
 -----------------------
Link to ethernet module "ENC28J60 Module"
 VCC -   3.3V
 GND -    GND
 SCK - Pin 13
 SO  - Pin 12
 SI  - Pin 11
 CS  - Pin 10 
------------------

-----------------------
Link to relay module 
 Relay 1 - Pin 2
 Relay 2  - Pin 3
 Relay 3  - Pin 4
 Relay 4  - Pin 5 
------------------
 */

// REMEMBER TO CHANGE IP ADDRESS AND NETWORK CONFIGURATION
// default  ip used is 192.168.1.78, gateway 192.168.1.1
//

#include <EtherCard.h>  // https://yadi.sk/d/R57sVoglbhTRN
#define VoltAC  A0 //220 volt presence dector (pin to ground Low = 220 V OK)
#define VoltDC  A1 //battery voltage measurement pin input
#define R1R2     4 //voltage divider ratio, calculated as R2/(R1+R2)


// ip address  
static byte myip[] = {192,168,1,78};
const static uint8_t gw[] = {192,168,1,1};
const static uint8_t dns[] = {192,168,1,1};

// MAC Address 
static byte mymac[] = { 
  0x5A,0x5A,0x5A,0x5A,0x5A,0x5A };

byte Ethernet::buffer[800];
BufferFiller bfill;

// Pin per collegare i relay
int LedPins[] = {
  2,3,4,5};


boolean PinStatus[] = {
  1,2,3,4};
boolean VoltACval;
int sensorDC;
float VoltDCval;
char VoltDCval_str[10];

//-------------

const char http_OK[] PROGMEM =
"HTTP/1.0 200 OK\r\n"
"Content-Type: text/html\r\n"
"Pragma: no-cache\r\n\r\n";

const char http_Found[] PROGMEM =
"HTTP/1.0 302 Found\r\n"
"Location: /\r\n\r\n";

const char http_Unauthorized[] PROGMEM =
"HTTP/1.0 401 Unauthorized\r\n"
"Content-Type: text/html\r\n\r\n"
"<h1>401 Unauthorized</h1>";

//------------

// Робимо функцію для оформлення нашої Web сторінки. 
void homePage()
{
  VoltACval=!digitalRead(VoltAC);
  sensorDC=analogRead(VoltDC);
  VoltDCval=(5.0/1023) * R1R2 * sensorDC;
  dtostrf(VoltDCval, 6, 2, VoltDCval_str);
  
  bfill.emit_p( PSTR("$F"
    "<title>IR5UCM tlc</title>" 
    "IR5UCM Cima del Monte - Isola d'Elba<br />"
    "<br />"
    "DSTAR VHF --> Relay 1: <a href=\"?ArduinoPIN1=$F\">$F</a><br />"
    "DMR UHF -----> Relay 2: <a href=\"?ArduinoPIN2=$F\">$F</a><br />"  
    "MTK tetto ------> Relay 3: <a href=\"?ArduinoPIN3=$F\">$F</a><br />"
    "MTK modem ---> Relay 4: <a href=\"?ArduinoPIN4=$F\">$F</a><br />"
    "<br />"
    "220 V AC: $F<br />"
    "Batteria: $S V<br />"
    ),   

  http_OK,
  PinStatus[1]?PSTR("off"):PSTR("on"),
  PinStatus[1]?PSTR("<font color=\"green\"><b>ON</b></font>"):PSTR("<font color=\"red\">OFF</font>"),
  PinStatus[2]?PSTR("off"):PSTR("on"),
  PinStatus[2]?PSTR("<font color=\"green\"><b>ON</b></font>"):PSTR("<font color=\"red\">OFF</font>"),
  PinStatus[3]?PSTR("off"):PSTR("on"),
  PinStatus[3]?PSTR("<font color=\"green\"><b>ON</b></font>"):PSTR("<font color=\"red\">OFF</font>"),
  PinStatus[4]?PSTR("off"):PSTR("on"),
  PinStatus[4]?PSTR("<font color=\"green\"><b>ON</b></font>"):PSTR("<font color=\"red\">OFF</font>"),

  VoltACval?PSTR("<font color=\"green\"><b>PRESENTE</b></font>"):PSTR("<font color=\"red\">ASSENTE</font>"),
  VoltDCval_str
  );
}

//------------------------



void setup()
{
  Serial.begin(9600);

  if (ether.begin(sizeof Ethernet::buffer, mymac,10) == 0);
  ether.staticSetup(myip, gw, dns);
  ether.printIp("My IP: ", ether.myip); // Виводимо в Serial монітор IP адресу, яку нам присвоїв Router. 
  pinMode(VoltAC,INPUT_PULLUP); 
  pinMode(VoltDC,INPUT); 
  
  for(int i = 0; i <= 4; i++)
  {
    pinMode(LedPins[i],OUTPUT); 
    digitalWrite (LedPins[i],HIGH);
    PinStatus[i]=true;
  }  
}

// --------------------------------------

void loop()
{

  delay(1); 

  word len = ether.packetReceive();   // check for ethernet packet 
  word pos = ether.packetLoop(len);   // check for tcp packet 

  if (pos) {
    bfill = ether.tcpOffset();
    char *data = (char *) Ethernet::buffer + pos;
    if (strncmp("GET /", data, 5) != 0) {
      bfill.emit_p(http_Unauthorized);
    }
   
   
    else {

      data += 5;
      if (data[0] == ' ') {       
        homePage(); // Return home page 
        for (int i = 0; i <= 3; i++)digitalWrite(LedPins[i],PinStatus[i+1]);
      }

      // "16" = "?ArduinoPIN1=on ".
      else if (strncmp("?ArduinoPIN1=on ", data, 16) == 0) {
        PinStatus[1] = true;        
        bfill.emit_p(http_Found);
      }
      else if (strncmp("?ArduinoPIN2=on ", data, 16) == 0) {
        PinStatus[2] = true;        
        bfill.emit_p(http_Found);
      }
      else if (strncmp("?ArduinoPIN3=on ", data, 16) == 0) {
        PinStatus[3] = true;        
        bfill.emit_p(http_Found);
      }
      else if (strncmp("?ArduinoPIN4=on ", data, 16) == 0) {
        PinStatus[4] = true;        
        bfill.emit_p(http_Found);
      
      }


      //------------------------------------------------------  


      else if (strncmp("?ArduinoPIN1=off ", data, 17) == 0) {
        PinStatus[1] = false;        
        bfill.emit_p(http_Found);
      }
      else if (strncmp("?ArduinoPIN2=off ", data, 17) == 0) {
        PinStatus[2] = false;        
        bfill.emit_p(http_Found);
      }
      else if (strncmp("?ArduinoPIN3=off ", data, 17) == 0) {
        PinStatus[3] = false;        
        bfill.emit_p(http_Found);
      }
      else if (strncmp("?ArduinoPIN4=off ", data, 17) == 0) {
        PinStatus[4] = false;        
        bfill.emit_p(http_Found);
      }
      


      //---------------------------


      else {
        // Page not found
        bfill.emit_p(http_Unauthorized);
      }
    }
    ether.httpServerReply(bfill.position());    // send http response
  }
}




