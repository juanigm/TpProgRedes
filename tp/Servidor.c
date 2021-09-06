/*
* Javier Abellan, 20 Jun 2000
*
* Programa Servidor de socket INET, como ejemplo de utilizacion de las
* funciones de sockets.
*/
#include <Socket_Servidor.h>
#include <Socket.h>
#include <string.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <net/ethernet.h>
#include <linux/if_packet.h>
#include <sys/socket.h>
#include <arpa/inet.h>

unsigned char MAC[6];


main ()
{
	/*
	* Descriptores de socket servidor y de socket con el cliente
	*/

	struct ifreq nic;

	struct sockaddr_in direccionCliente;
	unsigned int tamanoDireccion;

	int Socket_Servidor;
	int Socket_Cliente;
	char Cadena[100];
	char cadena2[100];
	int Cliente = accept(Socket_Servidor, (void*) &direccionCliente, &tamanoDireccion);
	int salida = 0;
	char error[100];
	int cero = 0;
	

	/*
	* Se abre el socket servidor, con el servicio "cpp_java" dado de
	* alta en /etc/services.
	*/
	Socket_Servidor = Abre_Socket_Inet ("cpp_java");
	if (Socket_Servidor == -1)
	{
		printf ("No se puede abrir socket servidor\n");
		exit (-1);
	}

	/*
	* Se espera un cliente que quiera conectarse
	*/
	Socket_Cliente = Acepta_Conexion_Cliente (Socket_Servidor);
	if (Socket_Servidor == -1)
	{
		printf ("No se puede abrir socket de cliente\n");
		exit (-1);
	}

	while(salida != 1){

	printf("Recibi una conexion en el socket %d\n", Cliente);
	
	Escribe_Socket (Socket_Cliente, "Ingrese interfaz de red", 100);

	/*
	* Se lee la informacion del cliente, suponiendo que va a enviar 
	* 5 caracteres.
	*/
	Lee_Socket (Socket_Cliente, Cadena, 5);
	strcpy(nic.ifr_name, Cadena);

	/*
	* Se escribe en pantalla la informacion que se ha recibido del
	* cliente
	*/
	printf ("Soy Servior, he recibido la interfaz: %s\n", Cadena);

	/*
	* Se prepara una cadena de texto para enviar al cliente. La longitud
	* de la cadena es 5 letras + \0 al final de la cadena = 6 caracteres
	*/
	//strcpy (Cadena, "Adios");

	if(ioctl(Socket_Cliente, SIOCGIFINDEX, &nic)==-1)
	{
		perror("Error al obtener el indice\n");
		sprintf(error, "%d", cero);
		Escribe_Socket(Socket_Cliente, error, 100);
	}else{
		printf("\nEl indice es: %d\n", nic.ifr_ifindex);

		sprintf(Cadena,"%d", nic.ifr_ifindex);

		Escribe_Socket (Socket_Cliente, Cadena, 100);
	}
	
	


	printf("\n");
	
	

	Lee_Socket(Socket_Cliente, Cadena, 5);

	salida = atoi(Cadena);
	
}

	/*
	* Se cierran los sockets
	*/
	close (Socket_Cliente);
	close (Socket_Servidor);
}
