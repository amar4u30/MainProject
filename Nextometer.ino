#include <SoftwareSerial.h>
// Configure software serial port
SoftwareSerial gsm(6, 7); // RX TX

String url="api.thingspeak.com/update?api_key=PSNDUDXYZN7SXM1S&field1="; // URL to which data will be sent


short int analogIncurr = A1; // current sensor
short int analogInvolt = A3; // voltage sensor
String textMessage;
 
 unsigned short int i=0,a=0;
short int ctr=40;


float volt,curr,power;



void setup()
{ 
  gsm.begin(9600);
  Serial.begin(9600);
  gsm.flush();
  Serial.flush();
  pinMode(8, OUTPUT); // Relay PIN
  digitalWrite(8, HIGH); // Relay set to off mode
  Serial.println("Config SIM800A..."); // AT command to set gsm to SMS mode
  delay(2000);
  Serial.println("Done!...");
  gsm.flush();
  Serial.flush();
  
  gsm.println("AT+CGATT?"); // attach or detach from GPRS service 
  delay(100);
  toSerial();

  gsm.println("AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\""); // bearer settings
  delay(2000);
  toSerial();

  gsm.println("AT+SAPBR=3,1,\"APN\",\"internet\""); // bearer settings
  delay(2000);
  toSerial();

  gsm.println("AT+SAPBR=1,1");  // bearer settings
  delay(2000);
  toSerial();
   gsm.print("AT+CMGF=1\r"); // Sets the GSM Module in Text Mode
  delay(1000);
  
  gsm.print("AT+CNMI=2,2,0,0,0\r"); // AT Command to recieve a live SMS 
  delay(1000);
}




void toSerial()  // To print in Serial monitor
{
  while(gsm.available()!=0)
  {
    Serial.print(gsm.readString());
  }
}




 float voltage()
{  
  unsigned short int bitvolt;
  float Vac,Vdc,Va,Vas; 
  bitvolt=analogRead(analogInvolt);
  Va=(bitvolt*5.08)/1023;
  Vdc=(Va*37500)/7500;
  Vac=Vdc;
  Vas=Vac*12.83; 
  if(Vas<200.0)
  {
    Vas=0.0;
  }
 return Vas;
}




float current()
{
  float a[40];
  float sum=0,avg=0,avgc;
  int mVperAmp = 100; // use 100 for 20A Module and 66 for 30A Module
  short int RawValue= 0;
  short int ACSoffset = 2500; 
  double Voltage = 0;
  double amps = 0;
  i=0;
  sum=0;
  avg=0;
  while(i<40)
  { 
     RawValue = analogRead(analogIncurr);
     Voltage = (RawValue / 1023.0) * 5000; // Gets you mV
     amps = ((Voltage - ACSoffset) / mVperAmp);
    if(amps>0)
    {
       a[i]=amps;
       i++;
    }
    delay(10);
  }
  
  for(i=0;i<40;i++)
  {
    sum=sum+a[i];  
  }
  avg=(sum/40.0);
  delay(250); 
  return avg;
 
}

void urlsend(float val)
{
   gsm.println("AT+HTTPINIT"); // AT to initialize connection
   delay(500); 
   toSerial();
   gsm.println("AT+HTTPPARA=URL," + url + val); // AT command to send data
   delay(2000);
   toSerial(); 
   gsm.println("AT+HTTPACTION=0"); // AT command to select action
   delay(500);
   toSerial();
   gsm.println("");
   gsm.println("AT+HTTPTERM"); // AT command to Terminate connection
   toSerial();
   delay(300);
   gsm.println("");
   delay(2000);

}



void loop()
{
  
  if(gsm.available()>0) // Checking for any SMS
 {
    textMessage = gsm.readString();
    Serial.print(textMessage);    
    
  } 
  if(textMessage.indexOf("RELAY_OFF")>=0) 
  {
    
    digitalWrite(8, HIGH);
    Serial.println("Relay set to OFF");  
    delay(2000);

  } 
  if(textMessage.indexOf("RELAY_ON")>=0)
  {
  
    digitalWrite(8, LOW);
    delay(2000);
    Serial.println("Relay set to ON");
  }
  if(ctr==0)
  {
       urlsend(power);
       ctr=40;
  }
  ctr--;
  volt=voltage();
  curr=current();
  power=(volt*curr);
  Serial.print("Power = ");
  Serial.println(power,3);
  textMessage = ""; 
    
   
} 
   


 







