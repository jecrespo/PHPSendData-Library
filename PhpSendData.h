// 
// FILE: PhpSendData.h
// VERSION: 1.0
// PURPOSE: Send Data to a PHP script from Arduino
//
// HISTORY:
// see PhpSendData.cpp file
//
//	TODO LIST:
//	* Como y donde documentar la funcion y las propiedades segun el libro de estilo
//	* En el metodo SendData, pasar otro parametro de acuse de recibo para que recoja respuesta o no. (ver linea 92 de PhpSendData.cpp)
//	* En el constructor poner la cadena de caracteres que el acuse de recibo considera correcta
//	* Porque tarda mas de 10 segundos el servidor en dar respuesta al PHP. Mira con otros que usas el leer linea (ver linea 94 de PhpSendData.cpp)
//	* Quitando los return SEND_OK y SEND_KO, tarda unos 6 segundos. (ver linea 94 de PhpSendData.cpp)
//	* Calcular la propiedad privada num_parametros con el valor que paso de claves. (ver linea 27 de PhpSendData.cpp)
//	* Hacer esta libreria con el objeto String en lugar de un array de char, es posible que sea más sencillo
//
// Libreria creada según tutorial http://arduino.cc/en/Hacking/LibraryTutorial
// Librería creada segun guia de estilo http://arduino.cc/en/Reference/APIStyleGuide
// 

#ifndef PhpSendData_h
#define PhpSendData_h

#if ARDUINO < 100
#include <WProgram.h>
#else
#include <Arduino.h>
#endif
#include <Ethernet.h>
#include <SPI.h>             //Manejo del bus SPI que usa el ethernet shield para comunicarse con Arduino.

#define PHPSENDDATA_LIB_VERSION "1.0"
#define SEND_OK 1
#define SEND_KO 0
#define get		1
#define post	0

class PhpSendData
{
	public:
		char **claves;
		char **valores;
		PhpSendData(char *_claves[], char *_valores[], int _num_param);	//Constructor
		uint8_t SendData(uint8_t *server, const char *ruta, int method);	//Manda los datos
	private:
		int num_parametros;
};
#endif
//
// END OF FILE
//