//----------------------------------------------------
void write_string_eeprom(char *str, int a){
  for(int i = 0; i < strlen(str); i++){
    char x = str[i];
    EEPROM.write(a,x);
    a++;
    yield();
  }
}

//----------------------------------------------------
String read_string_eeprom(int start, int finish){
  String temp = "";
  for(start; start < finish; start++){
    char c;
    c = EEPROM.read(start);
    if(c != NULL){
      temp += c;
    }
  }
  return temp;
}

//----------------------------------------------------
void clear_eeprom(int start, int finish){
  for(start; start < finish; start++){
    EEPROM.write(start,'\0');
    yield();
  }
}

//=======================================================
void Read_EEPROM(){
  byte a;

  EEPROM.get( addr_Suhu_limit,Suhu_limit );
  EEPROM.get( addr_Arus_control_limit,Arus_control_limit );
  EEPROM.get( addr_Arus_motor_limit,Arus_motor_limit );
  EEPROM.get( addr_control_limit_min,control_limit_min );
  EEPROM.get( addr_control_limit_max,control_limit_max );
  EEPROM.get( addr_motor_limit_min,motor_limit_min );
  EEPROM.get( addr_motor_limit_max,motor_limit_max );

}

//=====================================================================
void Restore_default(){
  Suhu_limit=35;
  Arus_control_limit=1;
  Arus_motor_limit=3;
  control_limit_min=10;
  motor_limit_min=5;
  control_limit_max=13;
  motor_limit_max=7;

  EEPROM.put( addr_Suhu_limit,Suhu_limit );
  EEPROM.put( addr_Arus_control_limit,Arus_control_limit );
  EEPROM.put( addr_Arus_motor_limit,Arus_motor_limit );
  EEPROM.put( addr_control_limit_min,control_limit_min );
  EEPROM.put( addr_control_limit_max,control_limit_max );
  EEPROM.put( addr_motor_limit_min,motor_limit_min );
  EEPROM.put( addr_motor_limit_max,motor_limit_max );
}
//----------------------------------------------------
void EEPROM_Setup(){
 
 /*
  //Just for write
  clear_eeprom(0,100);
  write_string_eeprom("10.1.37.177",addrDIP); //10.1.37.139 punya reza
  write_string_eeprom("10.1.36.254",addrGIP);
  write_string_eeprom("255.255.252.0",addrNIP);
  write_string_eeprom("10.1.37.97",addrSIP);
  write_string_eeprom("6017",addrPORT);
 */
  
  ip = String2IpAddress(read_string_eeprom(addrDIP,addrDIP+15));
  gateway = String2IpAddress(read_string_eeprom(addrGIP,addrGIP+15));
  subnet = String2IpAddress(read_string_eeprom(addrNIP,addrNIP+15));
  server = String2IpAddress(read_string_eeprom(addrSIP,addrSIP+15));
  port = read_string_eeprom(addrPORT,addrPORT+5).toInt();
  /*
  Serial.print("Device IP : ");
  Serial.println(device_ip);
  Serial.print("Gateway   : ");
  Serial.println(gateway_ip);
  Serial.print("Subnet    : ");
  Serial.println(subnet_ip);
  
  Serial.print("Server IP   : ");
  Serial.println(server_ip);
  Serial.print("Server port : ");
  Serial.println(s_port);
  */
}

//----------------------------------------------------
IPAddress String2IpAddress(String Temp_IP){
  String temp;
  byte a,b=0;
  byte IP_temp[4];
  
  for(a=0;a<=Temp_IP.length();a++){
    if(Temp_IP[a]=='.'){
      IP_temp[b]=temp.toInt();
      temp="";
      b++;
    }else{
      temp +=Temp_IP[a];
    }
  }
  IP_temp[b]=temp.toInt();
  return IP_temp;
}

//----------------------------------------------------
String IpAddress2String(const IPAddress& ipAddress)
{
  return String(ipAddress[0]) + String(".") +\
  String(ipAddress[1]) + String(".") +\
  String(ipAddress[2]) + String(".") +\
  String(ipAddress[3])  ;
}
