#ifndef NIDAQSERVERFUNCTIONS_H
#define NIDAQSERVERFUNCTIONS_H


#include "CommandDef.h"

int CMD_GETAIN_READING(Command cmd, SOCKET s,int NumParams,int NumOptions);
int CMD_GETAIN_READING_EXT(Command cmd, SOCKET s,int NumParams,int NumOptions);

#endif