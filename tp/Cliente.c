/*
* Javier Abellan, 20 Jun 2000
*
* Programa Cliente de un socket INET, como ejemplo de utilizacion
* de las funciones de sockets
*/
#include <stdio.h>
#include <Socket_Cliente.h>
#include <Socket.h>

main ()
{
	/*
	* Descriptor del socket y buffer para datos
	*/
	int Socket_Con_Servidor;
	char Cadena[100];
	int indice;
	int salida = 5;
	
	/*
	* Se abre la conexion con el servidor, pasando el nombre del ordenador
	* y el servicio solicitado.
	* "localhost" corresponde al nombre del mismo ordenador en el que
	* estamos corriendo. Esta dado de alta en /etc/hosts
	* "cpp_java" es un servicio dado de alta en /etc/services
	*/
	Socket_Con_Servidor = Abre_Conexion_Inet ("localhost", "cpp_java");
	if (Socket_Con_Servidor == 1)
	{
		printf ("No puedo establecer conexion con el servidor\n");
		exit (-1);
	}

	/*
	* Se prepara una cadena con 5 caracteres y se envia, 4 letras mas
	* el \0 que indica fin de cadena en C
	*/

	while(salida != 1){
	
	salida = 5;

	Lee_Socket (Socket_Con_Servidor, Cadena, 100);	
	
	printf("%s\n", Cadena);

	//printf("Introduce la interfaz de red: \n");
	scanf("%s", &Cadena);
	//strcpy (Cadena, "Hola");
	Escribe_Socket (Socket_Con_Servidor, Cadena, 5);

	/*
	* Se lee la informacion enviada por el servidor, que se supone es
	* una cadena de 6 caracteres.
	*/
	Lee_Socket (Socket_Con_Servidor, Cadena, 100);

	indice = atoi(Cadena);

	/*
	* Se escribe en pantalla la informacion recibida del servidor
	*/
	printf ("Soy cliente, He recibido el indice de la interfaz es : %d\n", indice);

	/*while(indice == 0){
	printf("Error, ingrese una interfaz de red valida: \n");
	scanf("%s", &Cadena);
	
	Escribe_Socket (Socket_Con_Servidor, Cadena, 5);

	Lee_Socket (Socket_Con_Servidor, Cadena, 100);	

	indice = atoi(Cadena);

	printf("%d", indice);
	}*/

	printf("Quiere seguir preguntando indices? 1-NO / 0-SI \n");

	scanf("%d", &salida);


	while (salida != 1 && salida != 0){
		printf("Incorrecto, vuelva a ingresar una opcion: \n");
		scanf("%d", &salida);
	}
	
	sprintf(Cadena,"%d", salida);
	Escribe_Socket(Socket_Con_Servidor, Cadena, 5);
	
	}

	/*
	* Se cierra el socket con el servidor
	*/
	close (Socket_Con_Servidor);
}
