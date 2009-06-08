#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>          
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>

#include "tftp.h"

int main(int argc, char *argv[]) {

    int sd, lon, v1, v2;
    char buffer[600], name[255] = "localhost", file[] = "ascii.txt", mode[] = "netascii";
    struct sockaddr_in origen, destination;
    struct tftp_packet *protocol;
    struct hostent *h;

    if (argc >= 2)
        snprintf(name, 255, argv[1]);


    destination.sin_family = AF_INET;
    destination.sin_port = htons(69);
    lon = sizeof(destination);

    h = gethostbyname(name);

    if (h != NULL) {
        memcpy(&destination.sin_addr, h->h_addr, h->h_length);
    } else {
        printf("Error conectando con %s\n",name);
        exit(-1);
    }

    sd = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP );

    send_request(RRQ, file, mode, sd, destination);
    int n = 0;
    int size;
    size = recvfrom(sd, buffer, PACKETSIZE, 0, (struct sockaddr *) &destination, &lon); // recibe la respuesta
    
    FILE *fp = fopen("temporal.txt", "wb");
    protocol = (struct tftp_packet *) buffer;
    
    while (size == PACKETSIZE) {
	    ++n;
	    send_ack(sd, destination, n);
	    if (fwrite(protocol->data, size-4, 1, fp) <= 0) {
		    perror("error al escribir");
	    }
	    printf("Size: %d\n", size-4);
	    size = recvfrom(sd, buffer, PACKETSIZE, 0, (struct sockaddr *) &destination, &lon);
    }
    
    if (size > 0) {
	    send_ack(sd, destination, n+1);
	    fwrite(protocol->data, size-4, 1, fp);
	    printf("Size: %d\n", size-4);
    }

    close(fp);
    close (sd);
    return 0;
}
