#ifndef COMMANDDEF_H
#define COMMANDDEF_H

#define VERSION "March 12 2014, Multithreaded"
#define PORTNUM 24000

#define NCLIENTS 4 //max number of concurrent clients
#define RCVTIMEOUT 350000 //ms for the socket rcv timeout
#define GOOD_STATUS_BYTE 1

typedef enum SENDMETHOD {TERM_STD,HEADER_NOTERM};

#define DEFAULT_SEND_METHOD TERM_STD

SOCKET establish(unsigned short portnum);

#define MAX_OPTIONS 10 //max number of options per command
#define MAX_PARAMS 5 //max number of parameters per command


enum {INVALID,GETSTAT,GETAIN_READING,GETAIN_READING_EXT};


typedef struct{
	int commandName;
	char options[MAX_OPTIONS];
	char* params[MAX_PARAMS];
} Command;

#endif