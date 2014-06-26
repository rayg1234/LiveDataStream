# -*- coding: utf-8 -*-
"""
Created on Tue Mar 11 11:34:08 2014

@author: ediff
"""

import serial
import time
import mysql.connector
from datetime import date, datetime, timedelta
from threading import Thread
import threading

endthread = 0
threadLock = threading.Lock()

def serialreadwrite(sx,msg):
    threadLock.acquire(1)
    nread = 18
    r = sx.read(nread)
    sx.write(msg)
    while sx.inWaiting()<9 :
        time.sleep(0.01)
    r = sx.read(nread)
    threadLock.release()
    return r

def threaded_function(sx):
    
    while endthread==0:
        #print 'getting data and storing to sql server'
        #read temperature
        
        #r = sx.read(nread)
        #msg = "\xca\x00\x01\x20\x00\xde" #internal temp
        msg2  = "\xca\x00\x01\x21\x00\xdd" #external temp
        #sx.write(msg2)
        #while sx.inWaiting()<9 :
       #	time.sleep(0.01)
        #r = sx.read(nread)
        #print sx
        r = serialreadwrite(sx,msg2)
        resultnum = -1
        #for regular precision
        if r[0]=='\xca':
            resulttxt = "".join("%02x" % ord(c) for c in r)
            resultnum = int(resulttxt[12:16],16)
        
        thetemp = float(resultnum)/100
        print "current temp is: " + str(thetemp)

        insert_q1 = ("INSERT INTO vals1 " "(time, voltval) " "VALUES (%s, %s)")
        thetime = time.strftime("%Y-%m-%d %H:%M:%S")
        thevalues = (thetime,str(thetemp))
        cursor.execute(insert_q1, thevalues)
        cnx.commit()
        
        time.sleep(60)
        
        

def turn_on(sx):
    print "Turning on Chiller"
    
    msg4 = "\xca\x00\x01\x81\x08\x01\x02\x02\x02\x02\x02\x02\x02\x66" #turn on
    serialreadwrite(sx,msg4)

def turn_off(sx):
    print "Turning off Chiller"
    msg3 = "\xca\x00\x01\x81\x08\x00\x02\x02\x02\x02\x02\x02\x02\x67" #turn off
    serialreadwrite(sx,msg3)

config = {
  'user': 'rmgao_genuser',
  'password': 'testdk$%',
  'host': 'mysql.rmgao.com',
  'database': 'rmgao_gen2',
}


#get serial object
sx = serial.Serial(3,19200,timeout=0,parity='N',stopbits=1)

#connect to mysql server
cnx = mysql.connector.connect(**config)
cursor = cnx.cursor()

thread = Thread(target = threaded_function, args = (sx,))
thread.start()
while True:
    var = raw_input("")
    if var == 'QUIT':
        endthread = 1
        break;
    if var == 'ON':
        turn_on(sx)
        continue;
    if var == 'OFF':
        turn_off(sx)
        continue;

thread.join()
cursor.close()
cnx.close()
sx.close()

print "stopping...exiting"

