#include <SPI.h>
#include <Ethernet.h>
#include <EEPROM.h>
#include <avr/wdt.h>
#include "Timer.h"

#define Pin_Relay 7
#define Pin_Buzzer A0
#define Pin_Sensor1 2 //logic AND, logic LOW jika kena obstacle (Falling)
#define Pin_Sensor2 3 //logic OR, logic HIGH jika kena obstacle (Rising)

//--------------------------------------------------
int Timeout_Connect = 6;      //3 detik
int Timeout_Reset = 1000;      //5 menit
int valuedelay = 100;
//--------------------------------------------------

// Update these with values suitable for your network.

byte mac[] = {0x28, 0x88, 0x7D, 0xFF, 0xFF, 0xFC };

IPAddress ip,subnet,gateway,server;
IPAddress ip_dns(0, 0, 0, 0);

int port;
int addrDIP = 1;
int addrGIP = 18;
int addrNIP = 35;
int addrSIP = 52;
int addrPORT = 69;

Timer timer1;

bool useint1,useint0;
bool relay = false;
bool reply=false;
bool Connect_Stat=false;
byte Count_Reset=0;
byte Reconnect_count=0;
byte Sensor_0=0;  //0:ready ; 1:detect
byte Sensor_1=0;
String buff,task;

EthernetClient client;

//================================================================
void setup()
{
  Serial.begin(115200);
  Serial.println("Starting");

  io();
  delay(1000);
  EEPROM_Setup();
  delay(2000);
  Ethernet.begin(mac, ip, ip_dns, gateway, subnet);
  delay(300);

  // device identification
  Serial.println("Device information");
  //*
  Serial.print("MAC: ");
  Serial.print(mac[0],HEX);
  Serial.print(":");
  Serial.print(mac[1],HEX);
  Serial.print(":");
  Serial.print(mac[2],HEX);
  Serial.print(":");
  Serial.print(mac[3],HEX);
  Serial.print(":");
  Serial.print(mac[4],HEX);
  Serial.print(":");
  Serial.println(mac[5],HEX);
  //*/  
  
  ip = Ethernet.localIP();
  Serial.print("IP: ");
  Serial.println(IpAddress2String(ip));

  gateway = Ethernet.gatewayIP();
  Serial.print("Gateway: ");
  Serial.println(IpAddress2String(gateway));
  
  subnet = Ethernet.subnetMask();
  Serial.print("Subnet: ");
  Serial.println(IpAddress2String(subnet));
    
  Serial.print("Connecting to: ");
  Serial.println(server);
  Serial.print("Port: ");
  Serial.println(port);
 
  con2server();
  
  // Allow the hardware to sort itself out
  digitalWrite(Pin_Buzzer,HIGH);
  delay(500);
  digitalWrite(Pin_Buzzer,LOW);
  delay(1000);
  timer1.every(400,Timer_isr);
  
  //attachInterrupt(digitalPinToInterrupt(Pin_Sensor1),INT0_Isr,FALLING);
  attachInterrupt(digitalPinToInterrupt(Pin_Sensor2),INT1_Isr,RISING);
  useint0 = false;
  useint1 = true;
  
  Count_Reset=Timeout_Reset;
  wdt_enable(WDTO_8S);
  wdt_reset();
}

//================================================================
void loop()
{
  int strLength;
  wdt_reset();
  if (client.available()>0){
    buff = client.readStringUntil('\n');
    delay(1);
    wdt_reset();
    //Serial.println(Serial_data);
    strLength = buff.indexOf("$");
    if(strLength != -1){
      buff = buff.substring(0,strLength);
      processBuffer();
      wdt_reset();
      buff ="";
    }
    client.flush();
  }
  timer1.update();   
  Send_Detect();
}
//================================================================

void Timer_isr(){
  if (!client.connected()) {
    if(Reconnect_count==0){
      con2server();
      Reconnect_count=Timeout_Connect;
    }else{
      Reconnect_count--;
    }
    if(Count_Reset==0){
      Serial.println("Restart");
      wdt_enable(WDTO_2S);
      while(1);
    }else{
      Count_Reset--;
    }
  }else{
    Reconnect_count=0;
    Count_Reset=Timeout_Reset;
    if(Connect_Stat==false){
      Serial.println("Conected");
      Connect_Stat=true;  
      String temp = IpAddress2String(ip);
      temp += "#IR SENSOR ETHERNET#$";
      client.print(temp);  
    }
  }
}

//---------------------------------------------------------------
void INT0_Isr(){
  if(Sensor_0==0){
    Sensor_0=1;
    Serial.println("YES");
  }
}

//---------------------------------------------------------------
void INT1_Isr(){
  if(Sensor_1==0){
    Sensor_1=1;
    Serial.println("YES");
  }
}

//---------------------------------------------------------------
void Send_Detect(){
  if(Sensor_0==1 && relay && useint0){
    Serial.println("Masuk gan");
    if (client.connected()) {
      Respon_Client("YES");
    }
    Sensor_0=0;
    digitalWrite(Pin_Buzzer,HIGH);
    delay(100);
    digitalWrite(Pin_Buzzer,LOW);
    delay(valuedelay-100);
  }
  else if(Sensor_0==0 && relay && useint0)
  {
    Respon_Client("NO");
    delay(valuedelay);
  }
  if(Sensor_1==1 && relay && useint1){
    if (client.connected()) {
      Respon_Client("YES");
    }
    Sensor_1=0;
    digitalWrite(Pin_Buzzer,HIGH);
    delay(100);
    digitalWrite(Pin_Buzzer,LOW);
    delay(valuedelay-100);
  }
  else if(Sensor_1 == 0 && relay && useint1)
  {
    Respon_Client("NO");
    delay(valuedelay);
  }
}
