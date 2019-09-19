#include <Ethernet.h>
#include <SPI.h>
#include <ArduinoJson.h>
#include "RestClient.h"
#include "Servo.h"

#define IP "192.168.0.108"
#define PORT 3001

#define trigPin 22
#define echoPin 24

double duracion;
double distancia;

int servoPin = 26;

Servo Servo1;

RestClient client = RestClient(IP, PORT);

void setup()
{
  Serial.begin(9600);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  Servo1.attach(servoPin);
  
  Serial.println("connect to network");
  client.dhcp();

  Serial.print("IP de Placa Ethernet Shield ");
  Serial.println(Ethernet.localIP());
  
  Serial.println("Listo :D!");
}

void loop()
{
  Radar();
}

  int Radar()
  {   
    double angulo;
    
    double x = 0.0;
    double y = 0.0;
    double h = distancia;
  
    float ang_x;
    float ang_y;
    
      for(angulo = 0; angulo <= 90; angulo += 30)
      {
        Servo1.write(angulo);
      
        distancia = calcularDistancia();
        Serial.println("");
        Serial.print("Ángulo: ");
        Serial.print(angulo);
        Serial.print("°");
        Serial.print(" / ");
        Serial.print("Distancia: ");
        Serial.print(distancia);
        Serial.print(" cm");
        Serial.println("");
  
        Serial.print("SIN de y: ");
        ang_y = sin(angulo * DEG_TO_RAD);
        ang_y = (ang_y *180)/PI;
        Serial.print(ang_y);
        Serial.println("");
        y = distancia * sin(angulo * DEG_TO_RAD);
        Serial.println();
        Serial.print("y: ");
        Serial.print(y);
        Serial.println("");
  
        Serial.print("COS de x: ");
        ang_x = cos(angulo * DEG_TO_RAD);
        ang_x = (ang_x *180)/PI;
        Serial.print(ang_x);
        Serial.println("");
        x = distancia * cos(angulo * DEG_TO_RAD);
        Serial.println();
        Serial.print("x: ");
        Serial.print(x);
        Serial.println("");
  
        Serial.println();
        Serial.print("Punto (");
        Serial.print(x);
        Serial.print(",");
        Serial.print(y);
        Serial.print(")");
        Serial.println();
  
        h = distancia;
        
        postRadar(x, y, h);
     }
    
      for(angulo = 90; angulo >= 0; angulo -= 30)
      {
        Servo1.write(angulo);
      
        distancia = calcularDistancia();
        Serial.print("Ángulo: ");
        Serial.print(angulo);
        Serial.print(" °");
        Serial.print(" / ");
        Serial.print("Distancia: ");
        Serial.print(distancia);
        Serial.print(" cm");
        Serial.println("");
  
        Serial.print("Seno de y: ");
        ang_y = sin(angulo * DEG_TO_RAD);
        ang_y = (ang_y *180)/PI;
        Serial.print(ang_y);
        Serial.println("");
        y = distancia * sin(angulo * DEG_TO_RAD);
        Serial.println("y: ");
        Serial.print(y);
        Serial.println("");
  
        Serial.print("Coseno de x: ");
        ang_x = cos(angulo * DEG_TO_RAD);
        ang_x = (ang_x *180)/PI;
        Serial.print(ang_x);
        Serial.println("");
        x = distancia * cos(angulo * DEG_TO_RAD);
        Serial.println("x: ");
        Serial.print(x);
        Serial.println("");
  
        Serial.print("Punto ");
        Serial.print(x);
        Serial.print(",");
        Serial.print(y);
        
        h = distancia;
  
        postRadar(x, y, h);
      }
  }

  double calcularDistancia()
  {
    digitalWrite(trigPin, LOW);
    delayMicroseconds(1);

    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);

    duracion = pulseIn(echoPin, HIGH);

    distancia = (duracion/2) / 29.1;
    return distancia;
  }
  
  void postRadar(float x, float y, float h)
  {
    String responseRadar = "";
    
    client.setHeader("Content-Type: application/json");
  
    StaticJsonBuffer<200> jsonBuffer;
    char json[256];  
    JsonObject& root = jsonBuffer.createObject();
    root["x"] = x;
    root["y"] = y;
    root["h"] = h;
    root.printTo(json, sizeof(json));
    Serial.println(json);
    
    int statusCode = client.post("http://192.168.0.108:3001/api/radar/", json, &responseRadar);

    Serial.println();
    Serial.print("Status code from server: ");
    Serial.println(statusCode);
    Serial.println();
    Serial.print("responseRadar body from server: ");
    Serial.println(responseRadar);
    Serial.println();
    
    delay(3000);
  }
