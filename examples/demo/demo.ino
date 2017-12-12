/*
  demo
  Ejemplo de uso de la librer�a PhpSendData.
  Recoge datos de una sonda dht y los manda a una BdD
 
  This example code.
 */
 
#include <PhpSendData.h>
#include <dht.h>
#include <Ethernet.h>
#include <SPI.h>             //Manejo del bus SPI que usa el ethernet shield para comunicarse con Arduino.

#define PIN_Sensor_Sonda 7       // Sensor Sonda temperatura y humedad

dht sonda;		  //sonda de temperatura
double temperatura;  //almaceno en cada bucle la temperatura
double humedad;      //almaceno en cada bucle la humedad
extern volatile unsigned long timer0_overflow_count;

// Datos de la clase PhpSendData
//Claves
char *datos_claves[] = {"temperatura", "humedad"};	//defino las claves
int num = sizeof(datos_claves)/sizeof(*datos_claves);
//Valores
char char_temperatura[6]; // variable para los valores en char -- formato del char 00.00 y finalizador /0
char char_humedad[6];
char *datos_valores[] = {char_temperatura, char_humedad};	//Defino la varable de los valores
//Objeto 
PhpSendData datos(datos_claves,datos_valores,num);	//Defino el objeto
//Datos del PHP para enviar datos
const char *file = "/arduino/grabar_datos_azotea.php";	//Defino el fichero al que mando los datos
uint8_t servidor[]  = {192,168,1,10};   // Direccion IP del servidor donde esta el fichero

//Datos de red
byte mac[]     = {0x90, 0xA2, 0xDA, 0x0D, 0x68, 0xA9};
byte ip[] = {192,168,1,11};   // Direccion IP de Arduino
byte gateway[]  = {192,168,1,1};	// Mascara para pruebas desarrollo
byte subnet[]  = {255,255,255,0};


// the setup routine runs once when you press reset:
void setup() {
	Serial.begin(9600);
	//Compruebo que la sonda funciona
	switch (sonda.read22(PIN_Sensor_Sonda)) {
    case 0:
      Serial.println("DHTLIB_OK");
	  temperatura=sonda.temperature;
	  humedad=sonda.humidity;
	  Serial.print("Temperatura Sonda: ");
	  Serial.println(temperatura);
	  Serial.print("Humedad Sonda: ");
	  Serial.println(humedad);
      break;
    case -1:
      Serial.println("DHTLIB_ERROR_CHECKSUM");
      break;
    case -2:
      Serial.println("DHTLIB_ERROR_TIMEOUT");
      break;
	}
	Ethernet.begin(mac, ip, gateway, gateway, subnet);	
}

// the loop routine runs over and over again forever:
void loop() {
Serial.println("-- Nuevo Envio --");
timer0_overflow_count = 0;
if(!sonda.read22(PIN_Sensor_Sonda)){
	temperatura = sonda.temperature;
	humedad = sonda.humidity;
    Serial.print("Temperatura Sonda: ");
	Serial.println(temperatura);
	Serial.print("Humedad Sonda: ");
	Serial.println(humedad);
    
	//Asigno a los puntero de los valores, los datos leidos como double, por lo tamt5o actualizo la propiedad valores del objeto
    dtostrf(temperatura,4,2,char_temperatura);  // dtostrf(), a standard avr-libc function. Para convertir el double en string
	dtostrf(humedad,4,2,char_humedad);
	
	Serial.println("Mando los datos al servidor...");
	
	int respuesta = datos.SendData(servidor, file, post);  //saco por pantalla el resultado
	Serial.print("Respuesta del servidor: ");
	Serial.println(respuesta);
	
	Serial.print("Tiempo de envio y respuesta: ");
	Serial.print(timer0_overflow_count);
	Serial.println(" ms");
}
delay(5000);               // wait for 5 seconds
}

