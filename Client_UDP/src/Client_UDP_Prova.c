/*
 ============================================================================
 Name        : Client_UDP_Prova.c
 Author      : Federico Calò
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================

  Progettare ed implementare un’applicazione UDP client/server, conforme al seguente protocollo applicativo:

1. Il client legge da tastiera il nome e il numero di porta del server UDP da contattare (Esempio: srv.di.uniba.it:56700).

2. Il client, invia il messaggio iniziale Helo al server.

3. Ricevuti i dati dal client, il server visualizza sullo std output un messaggio contenente sia il nome sia l’indirizzo dell’host del client (Esempio: Ricevuti dati dal client nome: pippo.uniba.it,
    indirizzo: 193.204.187.154).

4. Il client legge una stringa di caratteri dallo std input e la invia al server (Esempio: ciao).

5. Il server legge la stringa inviata dal client e la visualizza sullo std output; dopodiché, elimina tutte le vocali, la vistualliza sullo std output e la invia nuovamente al client
  (Esempio: Ricevuto 'ciao', inviato 'c').

6. Il server resta in attesa di altri dati.

7. Il client legge la risposta inviata dal server e visualizza la risposta sullo std output
  (Esempio Stringa 'c' ricevuta dal server nome: srv.di.uniba.it inidirizzo: 193.204.187.166); dopodiché, il client termina.

 */
#if defined WIN32
#include <winsock.h>
#else
#define closesocket close
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#endif
#include <stdio.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "protocol.h"

void ClearWinSock() {
	#if defined WIN32
		WSACleanup();
	#endif
}
int main(int argc, char** argv) {
	#if defined WIN32
		WSADATA wsa_data;
		int result = WSAStartup(MAKEWORD(2 ,2), &wsa_data);
		if (result != 0) {
			printf ("error at WSASturtup\n");
			return -1;
		}
	#endif
	int sock;
	struct sockaddr_in echoServAddr;
	struct sockaddr_in fromAddr;
	unsigned int fromSize;
	char echoString[ECHOMAX];
	char echoBuffer[ECHOMAX];
	int echoStringLen;
	int respStringLen;

	int port;
	struct hostent *remoteHost;
	struct in_addr addr;
	char* host_name="127.0.0.1";

	if(argc==2){
		port=argv[1];

	}else if(argc==3){
		port=atoi(argv[1]);
		host_name=argv[2];
		if(isalpha(host_name[0])){
			printf("Calling gethostbyname with %s\n", host_name);
			remoteHost=gethostbyname(host_name);
		}else{
			printf("Calling gethostbyaddr with %s\n", host_name);
    	    addr.s_addr = inet_addr(host_name);
			remoteHost = gethostbyaddr((char *) &addr, 4, AF_INET);
		}
	}else{
		port=PORT;
		addr.s_addr = inet_addr(host_name);
		remoteHost = gethostbyaddr((char *) &addr, 4, AF_INET);
	}
	//Socket init
	sock=socket(PF_INET, SOCK_DGRAM,IPPROTO_UDP);
	if(sock<0){
		printf("socket() failed");
	}

	//Costruzione dell'indirizzo del server
	memset(&echoServAddr,0,sizeof(echoServAddr));
	echoServAddr.sin_family=PF_INET;
	echoServAddr.sin_port=htons(port);
	//echoServAddr.sin_addr.s_addr=inet_addr(remoteHost);
	 memcpy((char *) &echoServAddr.sin_addr.s_addr, remoteHost->h_addr_list[0], remoteHost->h_length);

	printf("\n Inserisci la stringa: ");
	fflush(stdin);
	gets(echoString);
	echoStringLen=strlen(echoString);
	if(echoStringLen>ECHOMAX){
		printf("\n word too send too long");
	}
	//invio della stringa
	int sentmsg=sendto(sock,echoString,echoStringLen,0,(struct sockaddr*)&echoServAddr,sizeof(echoServAddr));
	if(sentmsg!=echoStringLen){
		printf("sendto() sent different number of bytes than expected");
	}

	fromSize=sizeof(fromAddr);
	respStringLen=recvfrom(sock, echoBuffer, ECHOMAX,0,(struct sockaddr*)&fromAddr, &fromSize);
	if(echoServAddr.sin_addr.s_addr!= fromAddr.sin_addr.s_addr){
		fprintf(stderr, "Error: received a packet from unknown source.");
		exit(EXIT_FAILURE);
	}

	echoBuffer[sizeof(echoBuffer)]='\0';
	printf("Received: %s\n",echoBuffer);
	closesocket(sock);
	ClearWinSock();

	system("pause");
	return EXIT_SUCCESS;
}
