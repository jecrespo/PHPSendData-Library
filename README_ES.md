La librería PhpSendData sirve para mandar datos/variables desde arduino a un servidor ejecutando un script PHP.

La clase PhpSendData, tiene un método diferente por cada script PHP al que manda datos.
A cada método le paso como parametros si el envio es post o get y un puntero a un vector con los datos que mando.
Los datos que mando debe en es parejas variable-valor, que es lo que mando al PHP y el lo debe saber interpretar.
El número de variables es indeterminado y el PHP debe estar preparado para interpretarlo.

Siempre se debe construir el par funcion PHP - metodo para que funcionen conjuntamente

UML:
