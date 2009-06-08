#define TFTP

/*
 * Tipos de paquetes
 */
#define RRQ	0x01
#define WRQ	0x02
#define DATA	0x03
#define ACK	0x04
#define ERROR	0x05

/*
 * Tipos de errores
 */
#define ENOTDEF		0x00
#define ENOTFOUND	0x01
#define EACCESS		0x02
#define EDISKFULL	0x03
#define EILEGALOP	0x04
#define EUTRANSFER	0x05
#define EAEXISTS	0x06
#define ENOUSER		0x07

/*
 * Tamano del paquete
 */
#define DATASIZE	512
#define PACKETSIZE	DATASIZE + 4

/*
 * Estructuras para el protocolo
 */
struct tftp_packet {
	u_int16_t opcode;
	union {
		u_int16_t block;	/* Numero de bloque */
		u_int16_t error;	/* Codigo de error */
		char request[1];	/* Archivo a transferir */
	} header;
	char data[1];			/* Datos recibidos o mensaje de error */
};

#define block	header.block
#define error	header.error
#define request	header.request


void send_request(int type, char file[50], char mode[50], int sd, struct sockaddr_in destination) {
	char *cp;
	int size, lon;
	struct tftp_packet *protocol;
	char *buffer[PACKETSIZE];

	char file1[] = "ascii.txt", mode1[] = "netascii";

	protocol = (struct tftp_packet *) buffer;
	protocol->opcode = htons(type);
	cp = (char *) protocol->request;
	strcpy(cp, file1);
	cp += strlen(file1);
	*cp++ = '\0';
	strcpy(cp, mode1);
	cp += strlen(mode1);
	*cp++ = '\0';
	
	size = cp - (char *) protocol;

	lon = sizeof(destination);
	sendto(sd, buffer, size, 0, (struct sockaddr *) &destination, lon);
}

void send_ack(int sd, struct sockaddr_in destination, int number) {
	struct tftp_packet *ack;
	char buffer[4];

	ack = (struct tftp_packet *) buffer;
	ack->opcode = htons(ACK);
	ack->block = htons(number);
	sendto (sd, buffer, 4, 0, (struct sockaddr *) &destination, sizeof(destination));
}
