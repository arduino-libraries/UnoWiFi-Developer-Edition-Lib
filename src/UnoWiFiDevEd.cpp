/*
****************************************************************************
* Copyright (c) 2015 Arduino srl. All right reserved.
*
* File : ArduinoWiFi.cpp
* Date : 2016/03/24
* Revision : 0.0.1 $
* Author: adriano[at]arduino[dot]org
*
****************************************************************************
  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
*/


#include <UnoWiFiDevEd.h>
#include <stdarg.h>
#include <stdio.h>

#if defined(__AVR_ATmega328P__)
#define TOPIC_NUM  3		//number of topic

WifiData espSerial;
ESP esp(&espSerial);
REST rest(&esp);
MQTT mqtt(&esp);

boolean wifiConnected = false;
boolean attached = false;
String mqtt_topic ="";					//topic event
String mqtt_data="";						//data event
char* topic_array[TOPIC_NUM];	//topic subscribed
int topic_number=0;							//number of topics subscribed

void wifiCb(void* response)
{
	uint32_t status;
	RESPONSE res(response);

	if(res.getArgc() == 1) {
		res.popArgs((uint8_t*)&status, 4);
		if(status == STATION_GOT_IP){
			espSerial.println("DBG: Internet Connected");
			wifiConnected = true;
		}
		else {
			wifiConnected = false;
		}
	}
}

void mqttData(void* response)
{
  RESPONSE res(response);
	mqtt_topic = res.popString();
	mqtt_data = res.popString();
}

void ArduinoWifiClass::powerON(){}
void ArduinoWifiClass::powerOFF(){}

void ArduinoWifiClass::connect(char* ssid,char* pwd){
	esp.wifiConnect(ssid, pwd);
}
boolean ArduinoWifiClass::connected(){
	return wifiConnected;
}

void WifiBegin(short isCiao) {
	espSerial.begin(9600);
	if(espSerial.ping()!=1) {
		espSerial.println("DBG: esp not found");
		while(1);
	}
	else {
		//espSerial.println("device found");
	}
	//put GPIO control here !!!
	esp.enable();
	delay(1000);
	esp.reset();
	delay(1000);
	while(!esp.ready());
	if (isCiao == 0)
		esp.wifiCb.attach(&wifiCb);

	espSerial.println("\nDBG: UnoWiFi Start");
}
void ArduinoWifiClass::begin() {
	WifiBegin(0);
}

void check_topic(char* topic){
	if(!attached){
		mqtt.dataCb.attach(&mqttData);			//attach mqttdata callback
		attached = true;
	}
	if(topic_number < TOPIC_NUM){
		if(topic_number==0){
			mqtt.subscribe(topic);
			topic_number++;
		}
		else{
			int check=1;
			for(int idx=0; idx < topic_number; idx++)
				check = strcmp(topic,topic_array[idx]);
			if(check){
				mqtt.subscribe(topic);
				topic_number++;
			}
		}
	}

}

void CiaoClass::begin() {

	WifiBegin(1);
	if(!mqtt.begin("", "", "", 120, 1)) {
	 while(1);
	}
	rest.begin("google.com");
	rest.get("/");
	delay(3000);
}
CiaoData responseREAD(){
	CiaoData ciao_data;
	delay(400);
	char response[64] = "";
	char cstr[8] = "";

	int ret = rest.getResponse(response, 64);
	//espSerial.println(ret);

	snprintf(cstr,8,"%d",ret);

		ciao_data.msg_split[0]="id";
		ciao_data.msg_split[1]=cstr;
		ciao_data.msg_split[2]=response;

	return ciao_data;
}
CiaoData requestPOST(char* hostname, String data){
	rest.begin(hostname);
	//delay(1000);	fix
	//esp.process();
	rest.post((const char*) data.c_str(),0);
	return responseREAD();
}
CiaoData requestGET(char* hostname, String data){

	rest.begin(hostname);
	//delay(1000);   fix
	//esp.process();
	rest.get((const char*) data.c_str());
	return responseREAD();
}
CiaoData PassThroughRead(char* connector,char* hostname, String data, char* method){

	short mode = 0;
	if (!strcmp(connector, "rest")){
		mode = 0;
	}
	else if (!strcmp(connector, "mqtt")){
		mode = 1;
	}
	else {
		CiaoData ciao_data;
		ciao_data.msg_split[0]="";
		ciao_data.msg_split[1]="";
		ciao_data.msg_split[2]="Protocol Error";
		return ciao_data;
	}

	if (mode == 0){
		if (!strcmp(method, "GET")){
			return requestGET(hostname, data);
		}
		else if (!strcmp(method, "POST")){
			return requestPOST(hostname, data);
		}
		else{
			CiaoData ciao_data;
			ciao_data.msg_split[0]="";
			ciao_data.msg_split[1]="";
			ciao_data.msg_split[2]="Method Error";
			return ciao_data;
		}
	}
	else if (mode == 1){		//mqtt class

			CiaoData ciao_data;
			esp.process();
			check_topic(hostname);
			char topic[mqtt_topic.length()+1];
			mqtt_topic.toCharArray(topic,mqtt_topic.length()+1);
			if(mqtt_data != "" && !strcmp(hostname,topic)){
					char message[mqtt_data.length()+1];
					mqtt_data.toCharArray(message,mqtt_data.length()+1);
					ciao_data.msg_split[0]="mqtt";
					ciao_data.msg_split[1]= hostname;
					ciao_data.msg_split[2]= message;
					mqtt_topic="";
					mqtt_data="";
			 }
			else{
					ciao_data.msg_split[0]="mqtt";
					ciao_data.msg_split[1]=hostname;
					ciao_data.msg_split[2]="";
			}
			return ciao_data;

	}
}
CiaoData PassThroughWrite(char* connector,char* hostname, String data, char* method){

	short mode = 0;
	if (!strcmp(connector, "rest")){
		mode = 0;
	}
	else if (!strcmp(connector, "mqtt")){
		mode = 1;
	}
	else {
		CiaoData ciao_data;
		ciao_data.msg_split[0]="";
		ciao_data.msg_split[1]="";
		ciao_data.msg_split[2]="Protocol Error";
		return ciao_data;
	}

	if (mode == 0){
		if (!strcmp(method, "GET")){
			return requestGET(hostname, data);
		}
		else if (!strcmp(method, "POST")){
			return requestPOST(hostname, data);
		}
		else{
			CiaoData ciao_data;
			ciao_data.msg_split[0]="";
			ciao_data.msg_split[1]="";
			ciao_data.msg_split[2]="Method Error";
			return ciao_data;
		}
	}
	else if (mode == 1){		//mqtt class
	    mqtt.publish(hostname, (char*) data.c_str());
	    CiaoData ciao_data;
	    ciao_data.msg_split[0]="mqtt";
	    ciao_data.msg_split[1]="OK";
	    ciao_data.msg_split[2]="";
	    return ciao_data;
	}
}
CiaoData CiaoClass::write(char* connector,char* hostname, String data, char* method) {
	return PassThroughWrite(connector, hostname, data, method);
}
CiaoData CiaoClass::read(char* connector,char* hostname, String data, char* method) {
	return PassThroughRead(connector, hostname, data, method);
}

CiaoClass Ciao;
ArduinoWifiClass Wifi;

#endif
