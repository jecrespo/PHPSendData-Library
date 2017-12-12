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

#define N_RACKS 	2      			// Number of racks to monitor
#define N_SENSORS	2				// Number of t&h sensors for rack

uint8_t PIN_Sensor[N_RACKS][N_SENSORS];  	// Definition of digital PIN number for every t&h sensor
dht SensorD[N_RACKS][N_SENSORS];	        // t&y sensors	
char error[] = "error_sonda";

extern volatile unsigned long timer0_overflow_count;

// Datos de la clase PhpSendData
//Claves
char *datos_claves[] = {"t11", "h11", "t12", "h12", "t21", "h21", "t22", "h22"};	//defino las claves
int num = sizeof(datos_claves)/sizeof(*datos_claves);
//Valores
char char_temperatura[4][6]; // variable para los valores en char -- formato del char 00.00 y finalizador /0
char char_humedad[4][6];
char *datos_valores[N_RACKS*N_SENSORS*2];	//Defino la varable de los valores
//Objeto 
PhpSendData datos(datos_claves,datos_valores,num);	//Defino el objeto
//Datos del PHP para enviar datos
const char *file = "/my_path/grabar_datos.php";	//Defino el fichero al que mando los datos
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
	uint8_t pin = 5;
	for (uint8_t i=0; i < N_RACKS;i++) {
		for (uint8_t j=0; j < N_SENSORS;j++) {
			PIN_Sensor[i][j] = pin;
			switch (SensorD[i][j].read22(PIN_Sensor[i][j]))
				{
				case 0:
					Serial.println("DHTLIB_OK");
					Serial.print("Temperatura Sonda: ");
					Serial.println(SensorD[i][j].temperature);
					Serial.print("Humedad Sonda: ");
					Serial.println(SensorD[i][j].humidity);
					break;
				case -1:
					Serial.println("DHTLIB_ERROR_CHECKSUM");
					break;
				case -2:
					Serial.println("DHTLIB_ERROR_TIMEOUT");
					break;
				}
			pin++;
		}
	}
	Ethernet.begin(mac, ip, gateway, gateway, subnet);	
}

// the loop routine runs over and over again forever:
void loop() {
Serial.println("-- Nuevo Envio --");
timer0_overflow_count = 0;

for (uint8_t i=0; i < N_RACKS;i++) {
	for (uint8_t j=0; j < N_SENSORS;j++) {
		if(!(SensorD[i][j].read22(PIN_Sensor[i][j]))){
			double temperatura = SensorD[i][j].temperature;
			double humedad = SensorD[i][j].humidity;
			Serial.print("Temperatura Sonda: ");
			Serial.println(temperatura);
			Serial.print("Humedad Sonda: ");
			Serial.println(humedad);
                        dtostrf(temperatura,4,2,char_temperatura[2*i+j]);
  		        dtostrf(humedad,4,2,char_humedad[2*i+j]);
                        datos_valores[4*i+2*j] = char_temperatura[2*i+j];
                        datos_valores[4*i+2*j+1] = char_humedad[2*i+j];
		}
                else{
                    Serial.println("error");
                    datos_valores[4*i+2*j] = error;
                    datos_valores[4*i+2*j+1] = error;
                }
         }
}
	
Serial.println("Mando los datos al servidor...");
int respuesta = datos.SendData(servidor, file, post);  //saco por pantalla el resultado
Serial.print("Respuesta del servidor: ");
Serial.println(respuesta);
	
Serial.print("Tiempo de envio y respuesta: ");
Serial.print(timer0_overflow_count);
Serial.println(" ms");

delay (5000);
}

