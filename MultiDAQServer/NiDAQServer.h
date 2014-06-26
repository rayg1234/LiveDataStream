/*
*NiDAQServer.h
*
*This defines most of the essential functions needed to run the server 
*/

#ifndef NIDAQSERVER_H
#define NIDAQSERVER_H

#include <crtdbg.h>
#include <stdio.h>
#include <iostream>
#include <assert.h>
#include <sys/timeb.h>
#include <memory.h>
#include <winsock2.h>
#include <process.h>
#include <windows.h>
//#include "CameraService.h"
#include <string>
#include "NIDAQmx.h"

#define DAQmxErrChk(functionCall) if( DAQmxFailed(error=(functionCall)) ) goto Error; else

#include "NiDAQServerFunctions.h"
#include "CommandDef.h"



//=============================================================================
// Server Functions
//=============================================================================
/*
*Tokenize
*
*The thread that the server runs on, each connection runs a separate instance 
*of this thread
*
*Pre: 
*Post: 
*
*Returns: 
*
*/
void ServerThread(void* threadnum);

/*
*Tokenize
*
*A helper function that breaks up a string into many substrings 
*based on a delimiter character [t]. 
*String Tokenizers are popular concepts and tools in expression parsing.
*This particular version returns [n]th token [r] of the string [s] of
*length [len] delimited by the character [t].
*
*Multiple contiguous delimiters found in the string
*are not considered as a token and discarded.
*
*Pre: 
*Post: 
*
*Returns: The length of [r]
*
*/
int Tokenize(const char* const s,int len,int n,char* r,const char t);

/*
*RcvLine
*
*Receives a line of text from the socket [s] and places in a buffer [buff]
*
*
*Pre: 
*Post: 
*
*Returns: The number of bytes read, or -1 if an error occured
*
*/
int RcvLine(SOCKET s,char* buff);

/*
*SendLine
*
*sends a line of text stored in [buff] from the socket [s] of length [len]
*
*SendMethod [m] can be either 
TERM_STD: standard newline/carriage return is appened to the text
HEADER_NOTERM: a 2 byte header contain the number of bytes following
*
*Pre: 
*Post: 
*
*Returns: The number of bytes sent, or -1 if an error occured
*
*/
int SndLine(SOCKET s,const char* buff,int len,SENDMETHOD m);

/*
*SendData
*
*sends binary image data stored in [buff] from the socket [s] with horizontal and vertical size [xsize] and [ysize]
*A 4 byte header that contains xsize and ysize is send first.
*
*Pre: 
*Post: 
*
*Returns: The number of bytes sent, or -1 if an error occured
*
*/
int SndData(SOCKET s,const char* buff,int len,unsigned short xsize,unsigned short ysize);

/*
*SendData
*
*sends binary data stored in [buff] from the socket [s] 
*
*Pre: 
*Post: 
*
*Returns: The number of bytes sent, or -1 if an error occured
*
*/
int SndData2(SOCKET s,const char* buff,int len);


/*
*SendByte
*
*sends a single byte of data in [c]
*
*Pre: 
*Post: 
*
*Returns: The number of bytes sent, or -1 if an error occured
*
*/
int SndByte(SOCKET s,const char c);

/*
*Process_Request
*
*processes a text command from the client by parsing the command into command name and parameters 
*and then performs tasks based on the command. Requires the FlyCaptureContext [c] to be passed to it to 
*communicate with the camera.
*
*Pre: 
*Post: 
*
*/
void Process_Request(SOCKET s);



#endif

