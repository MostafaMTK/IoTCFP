#define BLYNK_TEMPLATE_ID "TMPLxx_xxx_xx"
#define BLYNK_TEMPLATE_NAME "IotFinalProject"
#define BLYNK_AUTH_TOKEN "xxxxxxx_xxxxxxx_xxxxxx_xxxxx"

#include<WiFiNINA.h>
#include<utility/wifi_drv.h>
#include <BlynkSimpleWiFiNINA.h>
#include <DHT.h>

char ssid[] = "Giza-Creativa";
char pass[] = "xxxxxxxxxxxxx";

int ledG=25;
int ledR=26;
int ledB=27;
int btn = 2;
int DHTPIN = 3;
int DHTTYPE = DHT11 ;
// V0 -> isOperative -- Dashboard -> Arduino
// V1 -> Red Control -- Dashboard -> Arduino
// V2 -> Red Status -- Arduino -> Dashboard
// V3 -> Green Control -- Dashboard -> Arduino
// V4 -> Green Status -- Arduino -> Dashboard
// V5 -> Blue Control -- Dashboard -> Arduino
// V6 -> Blue Status -- Arduino -> Dashboard
// V7 -> Button -- Arduino -> Dashboard
// V8 -> Over Heat -- Arduino -> Dashboard
// V9 -> High Humidity -- Arduino -> Dashboard
// V10 -> Temperature -- Arduino -> Dashboard
// V11 -> Humidity -- Arduino -> Dashboard
// V12 -> Command -- Dashboard -> Arduino
// V13 -> Color -- Dashboard -> Arduino
// V14 -> LED Button -- Dashboard -> Arduino
// V15 -> LED Brightness -- Dashboard -> Arduino

DHT dht(DHTPIN, DHTTYPE);

BlynkTimer timer;
int isOperative;
int red;
int green;
int blue;
int brightness=255;
float virtualt ;
float virtualh ;
bool phone =false;
void sendSensorData(){
  if(isOperative)
  {
    float h = dht.readHumidity();
    float t = dht.readTemperature();
    if(isnan(t)||isnan(h))
    {
      Serial.println("Failed to read DHT sensor!");
      t=0;
      h=0;
    }
    t+=virtualt;
    h+=virtualh;

    Blynk.virtualWrite(V2,phone ?red*brightness/100 :red);
    Blynk.virtualWrite(V4,phone ?green*brightness/100 :green);
    Blynk.virtualWrite(V6,phone ?blue*brightness/100 : blue);
    Blynk.virtualWrite(V7,!digitalRead(btn));
    Blynk.virtualWrite(V8,t>70);
    Blynk.virtualWrite(V9,h>70);
    Blynk.virtualWrite(V10,t);
    Blynk.virtualWrite(V11,h);
  }
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  timer.setInterval(5000L,sendSensorData);
  Serial.println("connected!");
  dht.begin();
  
  WiFiDrv::pinMode(ledR,OUTPUT);
  WiFiDrv::pinMode(ledG,OUTPUT);
  WiFiDrv::pinMode(ledB,OUTPUT);
  pinMode(btn,INPUT_PULLUP);
}

void loop() {
  // put your main code here, to run repeatedly:

  Blynk.run();
  timer.run();
}

BLYNK_WRITE(V0) // isOperative
{
  isOperative = param.asInt();
}
BLYNK_WRITE(V1) //Red Control
{
  red = param.asInt();
  if(isOperative) 
  WiFiDrv::analogWrite(ledR,red);
  phone = false;
}
BLYNK_WRITE(V3) //Green Control
{
  green = param.asInt();
  if(isOperative) WiFiDrv::analogWrite(ledG,green);
  phone = false;

}
BLYNK_WRITE(V5) //Blue Control
{
  blue = param.asInt();
  if(isOperative) WiFiDrv::analogWrite(ledB,blue);
  phone = false;
}
BLYNK_WRITE(V12) //Command
{
  if(isOperative)
  {
  String com = param.asString();
  if(com[0]=='t')
  {
    
    virtualt=com.substring(1).toFloat(); 
  }
  else if(com[0]=='h')
  {
   virtualh=com.substring(1).toFloat();
  }
  }
}
BLYNK_WRITE(V13) //LED Color
{
  if(isOperative)
  {
    red  = param[0].asInt();
    green  = param[1].asInt();
    blue = param[2].asInt();
      WiFiDrv::analogWrite(ledR,red*brightness/100);
      WiFiDrv::analogWrite(ledG,green*brightness/100);
      WiFiDrv::analogWrite(ledB,blue*brightness/100);
      phone = true;
  }

}
BLYNK_WRITE(V15) //LED Color
{
  if(isOperative)
  {
    brightness=param.asInt();
      WiFiDrv::analogWrite(ledR,red*brightness/100);
      WiFiDrv::analogWrite(ledG,green*brightness/100);
      WiFiDrv::analogWrite(ledB,blue*brightness/100);
      phone = true;
  }

}
BLYNK_WRITE(V14) //LED Button
{
  if(isOperative)
  {
    int ledButton = param.asInt();
    if(ledButton)
    {
      WiFiDrv::analogWrite(ledR,red);
      WiFiDrv::analogWrite(ledG,green);
      WiFiDrv::analogWrite(ledB,blue);
    }
    else {
      WiFiDrv::analogWrite(ledR,0);
      WiFiDrv::analogWrite(ledG,0);
      WiFiDrv::analogWrite(ledB,0);
    }
  }
}
