/*
 ============================================================================
 Name        : Server_UDP_Prova.c
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
#include <stdlib.h>
#include <string.h>
#include "protocol.h"

void ClearWinSock() {
	#if defined WIN32
		WSACleanup();
	#endif
}

int confronto(char x);

int main() {
	#if defined WIN32
		WSADATA wsaData;
		int iResult=WSAStartup(MAKEWORD(2,2),&wsaData);
		if(iResult!=0){
			printf("\n error at WSASturtup");
			return EXIT_FAILURE;
		}
	#endif

	int sock;
	struct sockaddr_in echoServAddr;
	struct sockaddr_in echoClntAddr;
	unsigned int cliAddrLen;
	char echoBuffer[ECHOMAX];

	sock=socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if(sock<0){
		printf("\n socket() failed");
	}

	memset(&echoServAddr, 0, sizeof(echoServAddr));
	echoServAddr.sin_family=AF_INET;
	echoServAddr.sin_port=htons(PORT);
	echoServAddr.sin_addr.s_addr=inet_addr("127.0.0.1");

	int bind_socket= bind(sock,(struct sockaddr *)&echoServAddr, sizeof(echoServAddr));
	if(bind_socket<0){
		printf("\nbind() failed");
	}
	int recvMsgSize;
	printf("Waiting for a client to connect...\n");
	while(1){
		cliAddrLen=sizeof(echoClntAddr);

		recvMsgSize=recvfrom(sock,echoBuffer,ECHOMAX,0,(struct sockaddr*)&echoClntAddr,&cliAddrLen);

		printf("Handling client %s\n", inet_ntoa(echoClntAddr.sin_addr));
		printf("Received: %s\n", echoBuffer);
		echoBuffer[sizeof(echoBuffer)]='/0';

		char str2[255];
		int j=0;


		for(int i=0;echoBuffer[i]!='\0';i++){
			printf("\n Analzzo la lettera: %c", echoBuffer[i]);

			if(confronto(echoBuffer[i])==1){
				printf("\nla lettera è una consonante");
				str2[j]=echoBuffer[i];
				j++;
			}else{
				printf("\nla lettera è una vocale");
			}
		}
		str2[j]='\0';
		printf("\n Copio la sttringa senza vocali nella stringa originaria");
		printf("\n Stringa convertita: %s",str2);
		strcpy(echoBuffer, str2);
		printf("\nCopia ok");
		printf("\n Stringa copiata: %s", echoBuffer);


		int send_check=sendto(sock,echoBuffer, recvMsgSize,0,(struct sockaddr*)&echoClntAddr, sizeof(echoClntAddr));
		if(send_check!=recvMsgSize){
			printf("\nsendto() sent different number of bytes than expected");
		}else{
			printf("\ninvio riuscito");
		}
	}

	ClearWinSock();
	system("pause");

	return EXIT_SUCCESS;
}

int confronto(char x){
	 if (x == 'a' || x == 'A' || x == 'e' || x == 'E' || x == 'i' || x == 'I' || x =='o' || x=='O' || x == 'u' || x == 'U'){
		return 0;
	}else{
		return 1;
	}

}
