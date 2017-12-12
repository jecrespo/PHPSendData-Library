//
// FILE: PhpSendData.cpp
// VERSION: 1.0
// PURPOSE: Send Data to a PHP script from Arduino
//
// HISTORY:
// 0.1 by ecrespo (12/08/2014)
// 0.2 añado post y get y la respuesta de la función (13/08/2014)
// 0.3 creo las propiedades claves y valores que son de tipo *char y son las que usa el metodo Save2DataBase()
// Se añade el parametro ruta al metodo Save2DataBase(), que junto con el servidor identifica el php al que mando las propiedades.
// Se añade el constructor de la clase, donde le paso las claves y los valores.
// 1.0 Version en producción funcional. Demo hecha como ejemplo de uso.

#include "PhpSendData.h"

#define VERBOSE 1	// Modo que manda más información, para debug

/////////////////////////////////////////////////////
//
// PUBLIC
//

//Constructor
PhpSendData::PhpSendData(char *_claves[], char *_valores[], int _num_param)
{
	claves = _claves;
	valores = _valores;
	num_parametros = _num_param;
	//num_parametros = sizeof(claves)/sizeof(*claves); //Le tengo que pasar num_param, porque así no lo calcula bien.
}

// Metodo SendData
// return values: SEND_OK y SEND_KO
uint8_t PhpSendData::SendData(uint8_t *server, const char *ruta, int method)
{	
	EthernetClient client; 
	client.connect(server,80);	//Mando siempre por el puerto 80
	if (method == post) {
		//metodo POST
		client.print("POST ");
		client.print(ruta);
		client.println(" HTTP/1.1"); 
		client.println("Host: Arduino");
		client.println("Connection: Close");
		client.println("Content-Type: application/x-www-form-urlencoded");
		client.println("Content-Length: 350");	//Este dato se puede ajustar
		client.println("");
		for (uint8_t i=0; i < num_parametros ;i++) {		// Mando algo del tipo: clave1=valor1&clave2=valor2&clave3=valor3&...
			client.print(claves[i]);
			client.print("=");  
			client.print(valores[i]);
			client.print("&");
		}
		client.println("");
		client.println("");
		
		#if VERBOSE 
		Serial.print("POST ");
		Serial.print(ruta);
		Serial.println(" HTTP/1.1"); 
		Serial.println("Host: Arduino");
		Serial.println("Connection: Close");
		Serial.println("Content-Type: application/x-www-form-urlencoded");
		Serial.println("Content-Length: 350");	//Este dato se puede ajustar
		Serial.println("");
		for (uint8_t i=0; i < num_parametros ;i++) {		// Mando algo del tipo: clave1=valor1&clave2=valor2&clave3=valor3&...
			Serial.print(claves[i]);
			Serial.print("=");  
			Serial.print(valores[i]);
			Serial.print("&");
		}
		Serial.println("");
		Serial.println("");
		#endif
		
	}
	else if (method == get) {
		//metodo GET
		client.print("GET ");
		client.print(ruta);
		client.print("?");
		for (uint8_t i=0; i < num_parametros ;i++) {		// Mando algo del tipo: clave1=valor1&clave2=valor2&clave3=valor3&...
			client.print(claves[i]);
			client.print("=");  
			client.print(valores[i]);
			client.print("&");
		}	
		client.println(" HTTP/1.0");
		client.println("Host: Arduino");
		client.println("Connection: Close");
		client.println("");
		
		#if VERBOSE
		Serial.print("GET ");
		Serial.print(ruta);
		Serial.print("?");
		for (uint8_t i=0; i < num_parametros ;i++) {		// Mando algo del tipo: clave1=valor1&clave2=valor2&clave3=valor3&...
			Serial.print(claves[i]);
			Serial.print("=");  
			Serial.print(valores[i]);
			Serial.print("&");
		}	
		Serial.println(" HTTP/1.0");
		Serial.println("Host: Arduino");
		Serial.println("Connection: Close");
		Serial.println("");
		#endif
	}
	
	
	//Otra opcion para que no tarde 15 segundos es cerrar la comunicacion y crear un servidor y que espere a que este avaliable.
	
	// Esta parte solo para tener acuse de recibo, ralentiza mucho cada envío.
	if (true){	//Si quiero ver la respuesta del servidor
		while (client.connected()){		//Tarda unos 5-10 segundos hasta que recibe respuesta, no se porqué.
			if (client.available()){	//Espero hasta que detecto respuesta
				Serial.println();
				Serial.println("Respuesta del servidor:");
				while (client.available()){
					char c = client.read();	//Saco por el puerto serie lo que devuelve el servidor
					Serial.print(c);
					if (c == 'D'){
						char c2 = client.read();
						Serial.print(c2);
						if (c2 == 'B'){
						client.stop();	//Si recibo "DB" del servidor, es que ha grabado con exito en la BdD
						Serial.println();
						Serial.println("Envio correcto...");
						return SEND_OK;	//Y salgo de la función
						}
					}
				}
				Serial.println("Envio NO correcto. Desconecto...");	//Puesto que no he recibido el DB
				client.stop();
				return SEND_KO;
			}
			Serial.print(".");
			delay(1000);	//Saco un punto cada segundo mientras espero
		}
	}
	Serial.println("No hay respuesta del servidor");
	client.stop();
	return SEND_OK;		//Si llego hasta el final devuelvo que OK
}
//
// END OF FILE
//