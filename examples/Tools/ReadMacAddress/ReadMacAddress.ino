/*
This sketch Read the MAC Address from the ESP8266EX and print the code in the console.
*/

#include <Wire.h>
#include <UnoWiFiDevEd.h>

#define BAUD 9600
WifiData EspSerial;
String mac = "";         // a string to hold incoming data
String ssid="";
boolean flag=false;

char command1[]={0xc0,0x0,0xa,0x4,0x0,0x0,0x0,0x0,0x0,0x50,0x0,0xf0,0x3f,0xc0};
char command2[]={0xc0,0x0,0xa,0x4,0x0,0x0,0x0,0x0,0x0,0x54,0x0,0xf0,0x3f,0xc0};


void setup()
{
  Serial.begin(BAUD);
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);
  // UART to Serial Bridge Initialization
  EspSerial.begin(BAUD);               //baudrate setting
  mac.reserve(2000);
  while(!Serial);
}

//Parse the reply
String cleanString(char inChar){
  String string = String(inChar, HEX);
  if(string.length()==1)
    string="0"+string;
  if(string.length()==3)
    string=String(string.charAt(1)) + String(string.charAt(2));
  if(string.length()==4)
    string=String(string.charAt(2)) + String(string.charAt(3));

  return string;
}

void loop()
{
  read_mac();
}

void read_mac(){
  int i;
  int c=0;

  //Send the command1 to the ESP char by char
  for(i=0; i<sizeof(command1); i++)
      EspSerial.write(command1[i]);

  //Read the reply but the response is not useful
  while (EspSerial.available()) {
    EspSerial.read();
    c++;
  }

  //Send the command2 to the ESP char by char
  for(i=0; i<sizeof(command2); i++)
      EspSerial.write(command2[i]);

  //Read the reply
  while (EspSerial.available()) {
    char inChar = (char)EspSerial.read();
    c++;

    //Read and stor the char 9, 18 and 19.

    if(c==9){
      mac=cleanString(inChar) + mac;
      ssid=mac;
    }
    if(c==18){
      String tmp = cleanString(inChar);
      mac=tmp + ":" +  mac;
      ssid=tmp + ssid;
    }
    if(c==19){
      String tmp = cleanString(inChar);
      mac=tmp + ":" + mac;
      ssid=tmp + ssid;
    }
  }

  if(mac.length()==0)
    Serial.println("Please, push the  ESP B/L button on the board and plug the USB cable before run the sketch");
  else{
    Serial.println("\nMAC:");
    String mac_complete="5c:cf:7f:" + mac;
    Serial.println(mac_complete);

    Serial.println("\nSSID:");
    Serial.println("Arduino-Uno-WiFi-" + ssid);

    Serial.println("You need to replug your board to work with the Wi-Fi");
  }

  while(1);

};
