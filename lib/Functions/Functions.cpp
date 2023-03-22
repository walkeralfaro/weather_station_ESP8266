#include <NTPClient.h>
#include <WiFiUdp.h>
#include <SFE_BMP180.h>

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, UDP_SERVER);

unsigned long getTime() {
  timeClient.update();
  unsigned long now = timeClient.getEpochTime();
  return now;
}

double* getTPA(SFE_BMP180 bmp180) {

  char status;
  double T,P;
  static double resultArr[3];
  
  status = bmp180.startTemperature();//Inicio de lectura de temperatura
  if (status != 0)
  {   
    delay(status); //Pausa para que finalice la lectura
    status = bmp180.getTemperature(T); //Obtener la temperatura
    resultArr[0] = T;
    if (status != 0)
    {
      status = bmp180.startPressure(3);//Inicio lectura de presión
      if (status != 0)
      {        
        delay(status);//Pausa para que finalice la lectura        
        status = bmp180.getPressure(P,T);//Obtenemos la presión
        resultArr[1] = P;
        if (status != 0)
        {                  
          resultArr[2] = bmp180.altitude(P,1013.25);
        }      
      }      
    }   
  } 

  return resultArr;

}