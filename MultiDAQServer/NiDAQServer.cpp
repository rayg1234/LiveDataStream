// NiDAQ.cpp : Defines the entry point for the console application.
//

#include "NiDAQServer.h"
#include "CommandDef.h"



#define MAXBUFFER 255

using namespace std;

CRITICAL_SECTION CriticalSection; 
int Tokenize(const char* const s,int len,int n,char* r,const char t) {
	
	int i,count = 0; //count is # of tokens (not counting contiguous tokens) seen
	int left=-1; //left is start of token
	int rlen=0;  //rlen is the right position of the token
	
	//printf("strlen is %d, string is %s\n",len,s);

	for(i=0;i<len;i++) {
	//printf("char %d\n",s[i]);
		if(s[i]==t || i==0) {
			
			//skip for contiguous tokens
			int k=i+1;
			while(k<len && s[k++]==t) {
				i++; 
			}	
		
			if(count<(n-1)) {
				count++;	//keep skipping tokens until we get to the one we want to extract
			} else if(count==n-1) {
				left=i;
				int j;
				
				for(j=i+1;j<len;j++,rlen++) {	//grab the token
					//printf("%c\n",s[j]);
					if(s[j]==t) break;
				}
				break;
				
			}
		}
	}
	if(left==-1)
		return 0;
	rlen++;

	if(i!=0)
		left++;
	
	memcpy(r,s+left,rlen);
	r[rlen] = 0;
	//printf("left: %d, rlen: %d, token is %s\n",left,rlen,r);
	
	return rlen;
	
}

SOCKET establish(unsigned short portnum) { 
	char myname[256]; 
	SOCKET s; 
	struct sockaddr_in sa; 
	struct hostent *hp; 
	memset(&sa, 0, sizeof(struct sockaddr_in)); /* clear our address */ 
	gethostname(myname, sizeof(myname)); /* who are we? */ 
	hp = gethostbyname(myname); /* get our address info */ 
	if (hp == NULL) /* we don't exist !? */ 
		return(INVALID_SOCKET); 
	sa.sin_family = hp->h_addrtype; /* this is our host address */ 
	sa.sin_port = htons(portnum); /* this is our port number */ 
	s = socket(AF_INET, SOCK_STREAM, 0); /* create the socket */ 
	if (s == INVALID_SOCKET) 
		return INVALID_SOCKET; /* bind the socket to the internet address */ 
	if (bind(s, (struct sockaddr *)&sa, sizeof(struct sockaddr_in)) == SOCKET_ERROR) { 
		closesocket(s); 
		return(INVALID_SOCKET); 
	} 
	listen(s, 0); /* max # of queued connects */ 
	return(s); 
}

int SndLine(SOCKET s,const char* buff,int len,SENDMETHOD m) {
	int numbytes=0;

	if(m == TERM_STD) {
		
		if( (numbytes = send(s,buff,len,0) )!=len) {
			perror("Error occurred while attempting to send line through socket\n");
			return -1;
		}
		char term[2];
		term[0] = '\r';
		term[1] = '\n';
		numbytes+=send(s,term,2,0);

		
		//printf("Sent %d bytes of data\n",len + 2);
	}
	if(m == HEADER_NOTERM) {
		int count = len;
		if( (numbytes = send(s,(char*)&count,sizeof(int),0)) !=sizeof(int)) {
			perror("Error occurred while attempting to send header through socket\n");
			return -1;
		}
		if( (numbytes = send(s,buff,len,0) )!=len) {
			perror("Error occurred while attempting to send line through socket\n");
			return -1;
		}
		//printf("Sent %d bytes of data\n",numbytes+sizeof(int));
	}
	
	return numbytes;
}

int SndData(SOCKET s,const char* buff,int len,unsigned short xsize,unsigned short ysize) {
	int numbytes;
	unsigned short x = htons(xsize);
	unsigned short y = htons(ysize);
	numbytes = 0;
	
	//printf("%d %d\n",x,y);
	char my_array[] = "abcdefg";

	if(len<=0) {
		numbytes = 0;
		send(s,(char*)&numbytes,sizeof(int),0);
		return 0;
	}
	
	printf("xy %d %d\n",x,y);

	if( (numbytes = send(s,(char*)&x,sizeof(unsigned short),0)) !=sizeof(unsigned short)) {
		perror("Error(1) occurred while attempting to send header through socket\n");
		return -1;
	}

	if( (numbytes = send(s,(char*)&y,sizeof(unsigned short),0)) !=sizeof(unsigned short)) {
		perror("Error(2) occurred while attempting to send header through socket\n");
		return -1;
	}

	/*if( (numbytes = send(s,(char*)&my_array,8,0) )!=8) {
		perror("Error(3) occurred while attempting to send data through socket\n");
		return -1;
	}*/

	if( (numbytes = send(s,buff,len,0) )!=len) {
		perror("Error(3) occurred while attempting to send data through socket\n");
		return -1;
	}

	//printf("Sent %d bytes of data\n",len+2*sizeof(unsigned short));

	return (numbytes+2*sizeof(unsigned short));
}

int SndData2(SOCKET s,const char* buff,int len) {
	int numbytes;
	numbytes = 0;
	
	//printf("%d %d\n",x,y);
	//char my_array[] = "abcdefg";
	unsigned int sendlen = htonl(len);
	//cout << sizeof(sendlen);


	if(len<=0) {
		numbytes = 0;
		send(s,(char*)&numbytes,sizeof(int),0);
		return 0;
	}
	

	if( (numbytes = send(s,(char*)&sendlen,sizeof(sendlen),0)) !=sizeof(sendlen)) {
		perror("Error(1) occurred while attempting to send header through socket\n");
		return -1;
	}

	if( (numbytes = send(s,buff,len,0) )!=len) {
		perror("Error(3) occurred while attempting to send data through socket\n");
		return -1;
	}

	//printf("Sent %d bytes of data\n",len);

	return (numbytes);
}


int SndByte(SOCKET s,const char c) {
	char byte = c;
	int numbytes;
	if( (numbytes = send(s,&byte,sizeof(char),0)) !=sizeof(char)) {
		perror("Error occurred while attempting to send byte through socket\n");
		return -1;
	}
	return 0;
}

int RcvLine(SOCKET s,char* buff) {

	int curcount = 0;
	int totalcount = 0;

	do {
        curcount = recv(s, buff + totalcount , MAXBUFFER, 0);
		if ( curcount > 0 ) {
			totalcount += curcount;
			if(buff[totalcount - 2] == '\r' && buff[totalcount - 1] == '\n') // newline, end transmission
				break;
		}
		else if ( curcount == 0 ) {
            printf("Connection closed\n");
			return 0;
		}
		else {
            printf("recv failed: %d\n", WSAGetLastError());
			return -1;
		}

    } while( curcount > 0 );

	buff[totalcount] = 0;
	//printf("totalcount:%d\n",totalcount);
	return totalcount;

}

void Process_Request(SOCKET s) {
	

	char buff[MAXBUFFER];
	
	char* p; //pointer to parameters

	while(RcvLine(s,buff)>0) {
	
		
		//printf("received raw packet:%s",buff);

		char *cp;
		/*
		* Chop off EOL.
		*/
		if ((cp = strchr(buff, '\r')) != 0)
			*cp = 0;
		if ((cp = strchr(buff, '\n')) != 0)
			*cp = 0;

		if(strcmp(_strupr(buff),"QUIT")==0) {
			printf("QUIT Command Receieved, Disconnecting Client...");
			return;
		}
		

		//printf("New Command Received: %s\n",buff);

		char r[50];
		int TokenCnt = 1;
		int y = 0;
		int numParams = 0;
		int numOptions = 0;
		Command newCommand;
		newCommand.commandName = INVALID;
		
		while( (y=Tokenize(buff,strlen(buff),TokenCnt,r,' '))!=0) {
			
			//printf("the token is: %s\n",r);
			//token 1 = command
			if(TokenCnt==1) {	
				/*if(strcmp(_strupr(r),"INIT")==0) {
					newCommand.commandName = INIT;
					
				} else 
				if(strcmp(_strupr(r),"GETSTAT")==0) {
					newCommand.commandName = GETSTAT;
					
				} else 
				if(strcmp(_strupr(r),"EXPOSE")==0) {
					newCommand.commandName = EXPOSE;
					
				} else
				if(strcmp(_strupr(r),"GETIMAGE")==0) {
					newCommand.commandName = GETIMAGE;
				} else
				if(strcmp(_strupr(r),"SETROI")==0) {
					newCommand.commandName = SETROI;
					
				}
					//add more here
				
				else { 
					printf("Invalid Command\n");
					break; //invalid command
				}*/
				
			}
			
			//'-' = options
			if(TokenCnt>1 && r[0]=='-') {
						
				for(int i=0;i<strlen(r) && i<MAX_OPTIONS;i++) {
					newCommand.options[i] = r[i];
					numOptions++;
				}
				
				break;
			}
			
			//rest of tokens = params
			if(TokenCnt>1 && numParams<MAX_PARAMS) {
				p = new char[strlen(r)+1];	//freed later
				//memcpy(p,r,strlen(r)+1);
				strcpy(p,r);
				newCommand.params[numParams] = p;
				numParams++;
			}
			//printf("token count %d\n",TokenCnt);
			TokenCnt++;
		}
		EnterCriticalSection(&CriticalSection);
		if((y=Tokenize(buff,strlen(buff),1,r,' '))!=0) {
				if(strcmp(_strupr(r),"GETAIN_READING")==0) {
					newCommand.commandName = GETAIN_READING;
					
					CMD_GETAIN_READING(newCommand,s,numParams,numOptions);
					
				} else if(strcmp(_strupr(r),"GETAIN_READING_EXT")==0){
					newCommand.commandName = GETAIN_READING_EXT;
					
					CMD_GETAIN_READING_EXT(newCommand,s,numParams,numOptions);

				} else if(strcmp(_strupr(r),"INVALID")==0){
					
				} else {

					printf("Invalid Command\n");
				}
		}
		LeaveCriticalSection(&CriticalSection);

		for(int i=0;i<strlen(r) && i<MAX_OPTIONS;i++) {
			newCommand.options[i] = 0;
		}

		for(i=0;i<numParams;i++) {
			delete[] newCommand.params[i];
			//free(newCommand.params[i]);
		}
		for(i=0;i<MAXBUFFER;i++) {
			buff[i] = 0;
			//free(newCommand.params[i]);
		}
		
	}
	
	return;
}

int main(int argc, char* argv[])
{
	

	cout << "NiDAQ Server Version " << VERSION << " Started...\n" << endl;
	

	WORD wVersionRequested;
	WSADATA wsaData;
	int err;
	

	wVersionRequested = MAKEWORD( 2, 2 );
	 
	err = WSAStartup( wVersionRequested, &wsaData );
	if ( err != 0 ) {
		/* Tell the user that we could not find a usable */
		/* WinSock DLL.                                  */
		return 1;
	}
	 
	/* Confirm that the WinSock DLL supports 2.2.*/
	/* Note that if the DLL supports versions greater    */
	/* than 2.2 in addition to 2.2, it will still return */
	/* 2.2 in wVersion since that is the version we      */
	/* requested.                                        */
	 
	if ( LOBYTE( wsaData.wVersion ) != 2 ||
			HIBYTE( wsaData.wVersion ) != 2 ) {
		/* Tell the user that we could not find a usable */
		/* WinSock DLL.                                  */
		WSACleanup( );
		return 1; 
	}
	 
	/* The WinSock DLL is acceptable. Proceed. */
	/* Port nubmer = PORTNUM (25000) + cameranubmer, so the first camera uses port 25000. */
	

	HANDLE hThread[NCLIENTS];
    //unsigned threadID;

    // Create the second thread.
    
	//WaitForSingleObject( hThread, INFINITE );

	//CloseHandle( hThread );
	//SOCKET s;
	//if ((s = establish(24000)) == INVALID_SOCKET) { 
	//		printf("error establishing socket on PORT"); 
	//		exit(1); 
	//}else{
	//		printf("Socket initialized...waiting for client...\n");
	//}
	InitializeCriticalSection(&CriticalSection);

	for (int i=0;i<NCLIENTS;i++) { 
	/* loop for clients */ 
		hThread[i] = (HANDLE)_beginthread(ServerThread,0,(void*)i);
		
	} 
	
	for (int j=0;j<NCLIENTS;j++) {
		WaitForSingleObject( hThread[j], INFINITE );
	}
	
	DeleteCriticalSection(&CriticalSection);
	WSACleanup();

	return 0;
}

//Thread Routine
//DWORD ThreadProc (LPVOID lpdwThreadParam )
//{
	//Print Thread Number
//	printf("hello w\n");
//	//printf ("Thread #: %d\n", *((int*)lpdwThreadParam));
//	//Reduce the count
	//nGlobalCount--;
//	//ENd of thread
//	return 0;
//}

void ServerThread(void* threadnum)
{
    //cout << "In thread function" << endl;
    //Sleep(1000); // sleep for 1 second
	int timeout = RCVTIMEOUT;

	SOCKET s;

	if ((s = establish(PORTNUM+(int)threadnum)) == INVALID_SOCKET) { 
		printf("error establishing socket %d on PORT %d\n",(int)threadnum,PORTNUM+(int)threadnum); 
		//exit(1); 
		return;
	}else{
		printf("Socket %d initialized on Port %d...waiting for client...\n",(int)threadnum,PORTNUM+(int)threadnum);
	}
	

	for(;;){
		
		

		SOCKET new_sock = accept(s, NULL, NULL);

		
		if (s == INVALID_SOCKET) { 
			fprintf(stderr, "Error waiting for new connection!\n"); 
			return;
		} 
		
		setsockopt(new_sock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(int));

		printf("Client %d Connected!\n",(int)threadnum);
		
		Process_Request(new_sock);
		
		closesocket(new_sock); 
		printf("Client %d disconnected!\n",(int)threadnum);
		Sleep(1000);
		
	}

    //cout << "Thread function ends" << endl;
    _endthread();
}