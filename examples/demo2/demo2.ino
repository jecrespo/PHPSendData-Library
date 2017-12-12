/*
  demo2
  Ejemplo de uso de la librer�a PhpSendData.
  Para el caso de convertir enteros a char y hacer que las claves y valores sean muy grandes usando for.
 
  This example code.
 */
 
#include <PhpSendData.h>
#include <dht.h>
#include <Ethernet.h>
#include <SPI.h>             //Manejo del bus SPI que usa el ethernet shield para comunicarse con Arduino.

#define N_RACKS 	4      			// Number of racks
#define N_SENSORS	2				// Number of t&h sensors for rack

uint8_t PIN_Sensor[N_RACKS][N_SENSORS];  	// Definition of digital PIN number for every t&h sensor
dht SensorD[N_RACKS][N_SENSORS];	        // t&y sensors	
extern volatile unsigned long timer0_overflow_count;

//Definicion de objetos
char *data_keys[N_RACKS*N_SENSORS*2];	//puesto que por cada sando saco el valor de temperatura y humedad
char *data_value[N_RACKS*N_SENSORS*2];
char char_temp_key[N_RACKS][N_SENSORS][4]; //puesto que las claves son de formato tij y hij y reservo este espacio de memoria
char char_humi_key[N_RACKS][N_SENSORS][4];
char char_temp[N_RACKS][N_SENSORS][6];	//reservo este espacio de memoria para los valores con formato 00.00
char char_humi[N_RACKS][N_SENSORS][6];
const char *file = "/my_path/grabar_datos.php";
char error[] = "error_sonda";
int data_num = N_RACKS*N_SENSORS*2;
PhpSendData data(data_keys,data_value,data_num);

//Datos de red
byte mac[]     = {0x90, 0xA2, 0xDA, 0x0D, 0x68, 0xA9};
byte ip[] = {192,168,1,11};   // Direccion IP de Arduino 
byte gateway[]  = {192,168,1,1};	// Mascara para pruebas desarrollo
byte subnet[]  = {255,255,255,0};
uint8_t servidor[]  = {192,168,1,10};   // Direccion IP del servidor donde esta el fichero

// the setup routine runs once when you press reset:
void setup() {
	Serial.begin(9600);
	//Defino Pines e inicializo sondas
	uint8_t pin = 2;
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
	//Inicializar Objetos
	for (uint8_t i=0; i < N_RACKS;i++) {
		for (uint8_t j=0; j < N_SENSORS;j++) {	//(t00, h00, t01, h01, t10, t11, h10,...)
			sprintf(char_temp_key[i][j],"t%i%i",i+1,j+1);	//Meto en el buffer en texto los datos que me da la gana...
			sprintf(char_humi_key[i][j],"h%i%i",i+1,j+1);
			data_keys[4*i+2*j] = char_temp_key[i][j];	//Asocio los punteros a caracter
			data_keys[4*i+2*j+1] = char_humi_key[i][j];
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
				dtostrf(temperatura,4,2,char_temp[i][j]);  // dtostrf(), a standard avr-libc function. Para convertir el double en string
				dtostrf(humedad,4,2,char_humi[i][j]);
				//sprintf(char_temp[i][j],"%f",temperatura);	//Esto no funciona, por ello usar dtostrf
				data_value[4*i+2*j] = char_temp[i][j];	//Lo asocio cada vez para poner poner errores
				data_value[4*i+2*j+1] = char_humi[i][j];
			}
			else {
				Serial.println("error");
                data_value[4*i+2*j] = error;
                data_value[4*i+2*j+1] = error;
			}
		}
	}
	
	Serial.println("-------------------------");
	for (uint8_t i=0; i < N_RACKS*N_SENSORS*2;i++) {
		Serial.print(data_keys[i]);			
	}
	Serial.println();
	for (uint8_t i=0; i < N_RACKS*N_SENSORS*2;i++) {
		Serial.println(data_value[i]);
	}
	Serial.println();
	
	Serial.println("Mando los datos al servidor...");
	
	int respuesta = data.SendData(servidor, file, post);  //saco por pantalla el resultado
	Serial.print("Respuesta del servidor: ");
	Serial.println(respuesta);
	
	Serial.print("Tiempo de envio y respuesta: ");
	Serial.print(timer0_overflow_count);
	Serial.println(" ms");

}

