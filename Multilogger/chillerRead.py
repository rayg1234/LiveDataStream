# -*- coding: utf-8 -*-
"""
Spyder Editor

This temporary script file is located here:
C:\Documents and Settings\Administrator\.spyder2\.temp.py
"""
import serial
sx = serial.Serial(3,19200,timeout=0,parity='N',stopbits=1)
msg = "\xca\x00\x01\x20\x00\xde" #internal temp
msg2  = "\xca\x00\x01\x21\x00\xdd" #external temp
cmd_off_chksum = cheksum('00 01 81 08 02 02 02 02 02 01 02 02'.replace(" ",""))
msg3 = "\xca\x00\x01\x81\x08\x00\x02\x02\x02\x02\x02\x02\x02\x67" #turn off
msg4 = "\xca\x00\x01\x81\x08\x01\x02\x02\x02\x02\x02\x02\x02\x66" #turn on
msg5 = "\xca\x00\x01\x81\x08\x02\x02\x02\x02\x02\x01\x02\x02\x66" #0.01 precision not working?

sx.write(msg)
r = sx.read(64)
resultnum = -1
#for regular precision
if r[0]=='\xca':
    resulttxt = "".join("%02x" % ord(c) for c in r)
    resultnum = int(resulttxt[12:16],16)

sx.close()


#check sum use ~a & 0xFF

#check sum string accepted by chiller
def cheksum(r):
    #r = "".join("%02x" % ord(c) for c in msg)
    tot = 0
    
    for i in range(0,int(len(r)/2)):
        tot = tot + int(r[i*2:i*2+2],16)
    return hex(~tot & 0xFF)