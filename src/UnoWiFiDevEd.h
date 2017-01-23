#if defined(__AVR_ATmega328P__)
#ifndef UNOWIFI_DEVED_H_
#define UNOWIFI_DEVED_H_

#include <Arduino.h>

#include <Stream.h>
#include "lib/crc16.h"
#include "lib/rest.h"
#include "lib/mqtt.h"
#include "lib/espduino.h"
#include "lib/ringbuf.h"
#include "lib/FP.h"
#include "lib/SC16IS750.h"
#include "lib/CiaoData.h"
#include <string.h>

class ArduinoWifiClass : public WifiData
{

	public:
		void begin();
		boolean connected();
		void connect(char* , char*);

		void powerON();
		void powerOFF();

};

class CiaoClass : public WifiData
{
	public:
		void begin();

		CiaoData read( char*, char*, String data = "", char* method = "GET");      // “rest”, ”hostname”, data, ”method”
		CiaoData write( char*, char*, String, char* method = "GET");      // “rest”, ”hostname”, data, ”method”
};



extern CiaoClass Ciao;
extern ArduinoWifiClass Wifi;

#endif

#endif /* WIFI_H_ */
