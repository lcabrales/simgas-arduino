#include <Ethernet.h>
#include <SPI.h>
#include "RestClient.h"

RestClient client = RestClient("simgas-api.com",3000);

//Setup
void setup() {
  Serial.begin(9600);
  // Connect via DHCP
  Serial.println("connect to network");
  bool success = client.dhcp();
  Serial.print("Connected? ");
  Serial.println(success);
}

String response;

void loop(){
  //calculate ppm and send data to server
  readSensorData();

  //1 minute delay for each reading
  delay(57000);
}

void readSensorData() {
 ///////////////// DATOS DE CO - MQ7//////////////////////////////////////////
 int adc_MQ7 = analogRead(A1); //Leemos la salida analógica del MQ9
 float voltaje7 = ((adc_MQ7) * (5 / 1023.0)); //Convertimos la lectura en un valor de voltaje
 float RsRo7 = 1000 * ((5 - voltaje7) / voltaje7);  //Calculamos Rs con un RL de 1k
 double monoxido_carbono7 = (107.13 * (pow(RsRo7/5463 , -1.459)))/1000; // calculamos la concentración  de Monoxido de Carbono en el MQ7 con la ecuación obtenida.

 sendData("4288be2d-33a1-4684-a314-fb25b849a7c5", voltaje7, RsRo7, monoxido_carbono7);

  ///////////////// DATOS DE CO2 - MQ135//////////////////////////////////////////
  int adc_MQ135 = analogRead(A2); //Leemos la salida analógica del MQ135
  float voltaje135 = adc_MQ135 * (5 / 1023.0); //Convertimos la lectura en un valor de voltaje
  float mq135_resistencia = 1000 * ((5 - voltaje135) / voltaje135);
  double dioxidoDeCarbono = 245 * pow(mq135_resistencia/5463, -2.26);

  sendData("72DAAA03-DF42-4977-B7FA-CFD991617462", voltaje135, mq135_resistencia, dioxidoDeCarbono);

  ///////////////// DATOS DE SMOKE - MQ2//////////////////////////////////////////
  int adc_MQ2 = analogRead(A0); //Leemos la salida analógica del MQ2
  float voltaje2 = ((adc_MQ2) * (5 / 1023.0)); //Convertimos la lectura en un valor de voltaje
  float RsRo2 = 1000 * ((5 - voltaje2) / voltaje2);  //Calculamos Rs con un RL de 1k
  double smoke2 = ((3917) * (pow(RsRo2/5463 , -2.274)))/1000; // calculamos la concentración  de alcohol con la ecuación obtenida.

  sendData("45ED31EB-C08E-4E59-B43A-FFCC40E7C26C", voltaje2, RsRo2, smoke2);
}

void sendData(String sensorId, float volt, float RsRo, double gasPpm) {
  String buf;
  buf += "{\"SensorId\": \"" + sensorId + "\",\"ReadingVolts\":";
  buf += String(volt, 3);
  buf += F(", \"SensorResistance\":");
  buf += String(RsRo, 3);
  buf += F(", \"KnownConcentrationSensorResistance\":");
  buf += String(1, 3);
  buf += F(", \"GasPpm\":");
  buf += String(gasPpm , 3);
  buf += F("}");
  Serial.println(buf);
 
  char data[buf.length()];
  buf.toCharArray(data,buf.length() + 1);
  Serial.println(data);

  client.setContentType("application/json");

  response = "";
  int code = client.post("/SensorReading", data, &response);
  Serial.println("Status code from server: ");
  Serial.println(code);
  Serial.println("Response body from server: ");
  Serial.println(response);
  Serial.println();
  Serial.println();

  delay(1000);
}

