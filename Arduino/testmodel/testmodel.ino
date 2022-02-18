#include <ESP8266WiFi.h>  
#include <NTPClient.h>
#include <DHT.h> 
#include <WiFiUdp.h>
#define DHTPIN 2   //보드에서 9번
#define Soil A0
#define FAN 15      //보드에서 10
#define LED 13      //보드에서 11
#define PUMP 12       //보드에서 12

//#define

void OPERATING_FAN(float temp, int Soil_moisture);
void OPERATING_PUMP(int Soil_moisture);

DHT dht(DHTPIN, DHT11); 
String apiKey = "KH5P85EHIOQIHF5J" ;  // 항목_(2)
const char *ssid = "robotics524_2GHz" ; //"U+NetBE58";//"jjnet";//      "AndroidHotspot3618";
const char *pass =  "rotic524!" ;//   "1269002084";"1234567890";
const char *server = "api.thingspeak.com" ;
WiFiClient client;  
WiFiUDP ntpUDP;
WiFiServer server1(80);   // Web Server port 
NTPClient timeClient(ntpUDP, "pool.ntp.org", 3600*9, 60000);



int autodrive=LOW;
int ledvalue = LOW;
int pumpvalue=LOW;
int fanvalue=LOW;
float hum ; 
float temp ; 
int Soil_moisture ;


 void setup()  { 
 Serial.begin(115200); 

 
// pinMode(DHTPIN, INPUT_PULLUP);
 pinMode(FAN,OUTPUT);
  pinMode(LED,OUTPUT);
 pinMode(Soil,INPUT);
pinMode(PUMP,OUTPUT);
 
 //pinMode(SWPIN, INPUT_PULLUP); 
 dht.begin ( ) ;
 delay(1000); 
 Serial.print("Connecting to : "); 
 Serial.println(ssid); 
     
 WiFi.begin(ssid, pass);    // ** WiFi Start 
 while (WiFi.status() != WL_CONNECTED )  { 
   delay(2000); 
   Serial.println("....... not connected");   }
   
   Serial.println(""); 
   Serial.println("WiFi connected"); 

   
 server1.begin();
 Serial.println("Server started");

 // 접속할 IP 주소 표시
 Serial.print("Use this URL to connect: ");
 Serial.print("http://");
 Serial.print(WiFi.localIP());
 Serial.println("/");
 Serial.println("");

 timeClient.begin();

   } 
 
 void loop()  {  
 timeClient.update();


 SW();
 if(autodrive==HIGH){
   int currentSecond = timeClient.getSeconds();
  Serial.println(currentSecond);
  int currentHours=timeClient.getHours();
  if(currentHours>=8 && currentHours<=21){
    digitalWrite(LED, HIGH);
  }
  else{
    digitalWrite(LED, LOW);
  }
if(currentSecond / 10 == 0){  //1분마다 센서값 읽어옴(온습도, 토양)
  Serial.println("read sesorvalue!");
   hum = dht.readHumidity(); 
   temp = dht.readTemperature(); 
   Soil_moisture = analogRead(Soil); }

   if(temp >= 20 || Soil_moisture <= 400){ //통풍 잘 되는 환경을 만들어줘야 한대서
    digitalWrite(FAN, HIGH);
    digitalWrite(PUMP,LOW);
                  }    
  
   else if(Soil_moisture >=780){
    digitalWrite(FAN, LOW);
    digitalWrite(PUMP,HIGH);
    delay(5000);
    digitalWrite(PUMP,LOW);
                 } 

   else {
    digitalWrite(FAN, LOW);
    digitalWrite(PUMP,LOW);
   }
 }
 else{
  
 }


   //   기준값 : 780 물 없음, 400 충분

 
 /*int a=SW();
  Serial.println(a);
 if(a==1){
  digitalWrite(ledPin, HIGH);
 }
 else if(a==0){
  digitalWrite(ledPin, LOW);
 }
 else{
   Serial.println("no action");
 }*/






 
 Serial.print("온도:") ;
 Serial.print(temp); 
 Serial.print("'C ");
 Serial.print(" 습도:") ;
 Serial.print(hum); 
 Serial.println(" %"); 
 Serial.print(Soil_moisture); 
 Serial.println(" %"); 




     
   if (client.connect(server,80)) {  
      String postStr = apiKey ;      
      Serial.print("SmartFarm_N1 : ")    ; Serial.println(postStr) ;   
      postStr += "&field1="   ;             
      postStr += String(temp) ;          
      postStr += "&field2="   ;       
      postStr += String(hum) ;  
      postStr += "&field3="   ;        
      postStr += String(Soil_moisture) ;  
//      postStr += "&field4="   ;        
//      postStr += String(SW)  ;   
      
      postStr += "\r\n\r\n"   ; 
      Serial.print("SmartFarm_N2 : ")    ; Serial.print(postStr) ;       




     // Print data,followed by a carriage return and newline,to the server a client is connected to.
     client.print("POST /update HTTP/1.1\n");      
     client.print("Host: api.thingspeak.com\n"); 
     client.print("Connection: close\n"); 
     client.print("X-THINGSPEAKAPIKEY: "+apiKey+"\n"); 
     client.print("Content-Type: application/x-www-form-urlencoded\n"); 
     client.print("Content-Length: "); 
     client.print(postStr.length());    
     client.print("\n\n"); 
     client.print(postStr);   // **    
     } 
     client.stop(); 
     Serial.println("Waiting..."); 
     delay(2000);   
   }  


void SW(){
  WiFiClient client = server1.available();   

  if(!client) {  
    return ;   } 

  // Client가 Data를 보낼 때까지 기다림
  Serial.println("new client");
  /*while(!client.available()) {
  delay(1);                  }
*/
  // Request의 첫번째 줄을 읽어들임
  Serial.println(client.available());
  String request = client.readStringUntil('\n');
  Serial.println(request);
  client.flush();

  // Request에 따른 LED 제어
  if(request.indexOf("AUTO=ON") != -1)  {
    Serial.println("ONNNN!!!!");

    autodrive =HIGH;                       }
  else if(request.indexOf("AUTO=OFF") != -1) {
    Serial.println("OFFFF!!!!");
    digitalWrite(LED, LOW);
    digitalWrite(PUMP, LOW);
    digitalWrite(FAN, LOW);
    autodrive = LOW;
    ledvalue = LOW;
    pumpvalue=LOW;
    fanvalue=LOW;
    }

  else if(autodrive==HIGH){
    Serial.println("AUTO mode Driving!!!!");
  }
  else if(request.indexOf("LED=ON") != -1)  {
    Serial.println("ONNNN!!!!");
    digitalWrite(LED, HIGH);
    ledvalue =HIGH;                       }
  else if(request.indexOf("LED=OFF") != -1) {
    Serial.println("OFFFF!!!!");
    digitalWrite(LED, LOW);
    ledvalue = LOW;                        }

    
  else if(request.indexOf("PUMP=ON") != -1)  {
    Serial.println("ONNNN!!!!");
    digitalWrite(PUMP, HIGH);
    
    pumpvalue =HIGH;                       }
  else if(request.indexOf("PUMP=OFF") != -1) {
    Serial.println("OFFFF!!!!");
    digitalWrite(PUMP, LOW);
    pumpvalue = LOW;     }

    
  else if(request.indexOf("FAN=ON") != -1)  {
    Serial.println("ONNNN!!!!");
    digitalWrite(FAN, HIGH);
    fanvalue =HIGH;                       }
  else if(request.indexOf("/FAN=OFF") != -1) {
    Serial.println("OFFFF!!!!");
    digitalWrite(FAN, LOW);
    fanvalue = LOW;                        }





  // Client Request에 대한 응답 Message 
  // Client Request에 대한 응답 Message 
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println("");
  client.println("<!DOCTYPE HTML><html><head><meta charset='UTF-8'><meta name='viewport' content='width=device-width, initial-scale=1.0'><title>With Arduino</title><link rel='stylesheet' href='https://cdn.jsdelivr.net/npm/semantic-ui@2.4.2/dist/semantic.min.css'><script src='https://cdn.jsdelivr.net/npm/semantic-ui@2.4.2/dist/semantic.min.js'></script></head><body><div style='margin: 10px; display: flex; justify-content: center;' class='ui container'><div class='ui card'><div style='text-align: center' class='content'><span style='font-size: medium; color: rgb(237, 165, 247)'><b>Networking with Arduino</b></span></div><div class='image'><img src='https://usercontents-d.styleshare.io/images/32400633/640x640'></div><div style='text-align: center' class='content'><div class='header'>당신의 선택이<br>반영되고 있습니다!</div><br><div class='meta'><a>AUTO MODE is turned ");
  if(autodrive==LOW){
    client.print("OFF");
  }
  else{
    client.print("ON");
    ledvalue = LOW;
    pumpvalue=LOW;
    fanvalue=LOW;
  }
  client.println("</a></div><div class='description'>LED is turned ");
  if(ledvalue){
    client.print("ON");
  }
  else{ 
  client.print("OFF");
  }
  client.println("<br>PUMP is turned ");
  if(pumpvalue){
    client.print("ON");
  }
  else{
  client.print("OFF");
  }
  client.println("<br>FAN is turned ");
  if(fanvalue){
    client.print("ON");
  }
  else {
  client.print("OFF");
  }
  client.println("</div></div><div class='extra content'><span class='right floated'>2022 Robotics</span><span>Your Farm</span></div></div></div></body><script>setTimeout(function(){window.close()},1000);</script></html>");

  delay(1);
  Serial.println("Client disconnected!!");
  Serial.println();
}

void OPERATING_FAN(float temp, int Soil_moisture){  // 기존 HyoJin함수 
 
  if(temp >= 20 || Soil_moisture <= 400){ //통풍 잘 되는 환경을 만들어줘야 한대서
    digitalWrite(FAN, HIGH);
  }       
  
  else{
    digitalWrite(FAN, LOW);
  }
  delay(5000);
/*  // 온습도, 토양습도값으로 팬 작동
온도 : 15~20도, 18도가 적온
습도 : 너무 습하지 않아야 하는데 실험하면서 알아가봐야될거같아서 아직?
토습 : 흙 표면 만져봤을 때 건조하다고 생각될 때 줘야 함, 값이 낮을수록 수분 많은 것*/
}

void OPERATING_PUMP(int Soil_moisture){
  if(Soil_moisture >=780){
    for(int i = Soil_moisture; i >= 400; i--){
      digitalWrite(PUMP,HIGH);
    }
  }
  else
    digitalWrite(PUMP,LOW);
      
   //   기준값 : 780 물 없음, 400 충분
 }

/*void fun_Watering()
{
  Serial.println(" ** Watering System ReStart ** ");
  digitalWrite(Watering, LOW); // relay off + common situation
  
   if (Serial.available()) {
     String inString = Serial.readStringUntil('\n');
     Serial.println(inString);
     if (inString== "WATER_ON") {
       digitalWrite(Watering, HIGH); // relay on + watering start
       delay(5000); //(for 5 Second)
       Serial.println( " ** Watering is Finished ** ");
       break;
     }
   }
  
}*/
