/*
File: MqttPub.ino

This example subscribes a MQTT channel and reads published data, then prints it
into the serial monitor. Please configure the MQTT server address via Uno WiFi
Web Panel. Topics are automatically created (or subscribed) via api calls into
the sketch. You can use a maximum of 3 topics.

Note: works only with Arduino Uno WiFi Developer Edition.
*/

#include <Wire.h>
#include <UnoWiFiDevEd.h>

#define CONNECTOR "mqtt"
#define TOPIC "arduino/data"

void setup() {
	Ciao.begin();
	Serial.begin(9600);
}


void loop(){

	delay(5000);

	CiaoData data = Ciao.read(CONNECTOR, TOPIC);        //topic to manage servo
	if (!data.isEmpty()){
		char* value = data.get(2);
		Serial.println(value);
	}

}
