# -*- coding: utf-8 -*-
"""
Created on Tue Mar 11 11:34:08 2014

@author: ediff
"""
import socket
import numpy as np
import struct
import serial
import time
import math
import mysql.connector
from datetime import date, datetime, timedelta
from threading import Thread
import threading

endthread = 0
threadLock = threading.Lock()
host = ('localhost',24002)
uploaddata = 1

#read from chiller
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
    
#Read from DAQ
def DAQreadwrite(soc,msg): 
    
    attempts = 0
    r = ''
    #try:
    bytessent = soc.send(msg)
    #except socket.error:
    #    print 'socket error on send'
    #    return 0,0
        
    time.sleep(0.1)
    try:
        #recieve header first
        rh = soc.recv(4)
        numbytes = struct.unpack('>I',rh)[0]
        #print str(numbytes)
        while True:
            r = r + soc.recv(numbytes)
            #print str(len(r))
            if(attempts>20):
                print 'could not get correct byte count'
                return 0,0
            if(len(r)==numbytes):
                break
            else:
                time.sleep(0.1)
                attempts = attempts+1
                
            
        print 'length received:' + str(len(r))
    except socket.timeout:
        print 'socket timeout on read.'
        return 0,0
    
    
    numbytes = len(r) - 4
    numvals = numbytes/8; #number of doubles
    #print numvals
    vals = np.zeros(numvals)
    for i in range(0,numvals):
        #vals[i] = struct.unpack('<d',r[i*8+4:i*8+12])[0]
        vals[i] = struct.unpack('<d',r[i*8:(i+1)*8])[0]
    return np.mean(vals),1/np.std(vals)

def threaded_function(sx):

    minsleeptime = 9
    maxsleeptime = 300
    sleeptime = minsleeptime

    
    
    soc = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    num_retries = 2

    soc.settimeout(3)
    for i in range(0,num_retries):
        try:
            soc.connect(host)
        except socket.error, msg:
            print 'could not connect to host, trying ' + str(num_retries - i) + ' more times...' 
            time.sleep(1)
            continue
        break

    
    while endthread==0:
        
        
        #print 'getting data and storing to sql server'
        #read temperature
        msgDAQ1 = 'getain_reading 0 1000\r\n'
        msgDAQ2 = 'getain_reading 2 10\r\n'
        msgDAQ3 = 'getain_reading_ext 1 25\r\n'
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
        print "\n"
        print time.strftime("%Y-%m-%d %H:%M:%S") 
        print "current temp is: " + str(thetemp)
        
        #send a test line to see if socket is connected, if not, attempt to reconnect
#        try:
#            bytes = soc.send("INVALID\r\n")
#            #r = soc.recv(64)
#        except socket.error:
#            print 'socket error... trying to reconnect'
#            soc.close()
#            try:
#                soc = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
#                soc.settimeout(3)
#                soc.connect(host)
#                bytes = soc.send("INVALID\r\n")
#                time.sleep(1)
#            except:
#                print 'reconnection attempt failed...'
        
        #get the DAQ values

        currentval = 0
        voltval = 0
        laserint = 0
        lasersn = 0        
        
        try:
            currentval = DAQreadwrite(soc,msgDAQ1)[0]
            voltval = DAQreadwrite(soc,msgDAQ2)[0]*32.5
            laserint,lasersn = DAQreadwrite(soc,msgDAQ3)
        except socket.error:
            print 'socket error... trying to reconnect'
            soc.close()
            try:
                soc = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
                soc.settimeout(3)
                soc.connect(host)
                #bytes = soc.send("INVALID\r\n")
                time.sleep(1)
                continue
            except:
                print 'reconnection attempt failed...' 
                continue
        
        
        if(laserint<0.5):
            lasersn = 0
        
        print "current is: " + str(currentval)
        print "voltval is: " + str(voltval)
        print "laser int: " + str(laserint)
        print "laser sn: " + str(lasersn)

        if(math.isnan(voltval) or  math.isnan(currentval) or math.isnan(laserint) or math.isnan(lasersn) or (lasersn < 0)):
            print "some value is nan, not storing them in SQL server"
            continue
        
        if(voltval<-100 or voltval > 0 or currentval > 100 or currentval < -100 or lasersn > 10000 or laserint > 5):
            print "some values read are out of range, not storing them in SQL server"
            continue
        
        if(uploaddata==1):
            try:
                cnx = mysql.connector.connect(**config)
                cursor = cnx.cursor()
        
                insert_q1 = ("INSERT INTO labdata " "(time, temp1, voltval, currentval, laserint, lasersn) " "VALUES (%s, %s, %s, %s, %s, %s)")
                thetime = time.strftime("%Y-%m-%d %H:%M:%S")
                thevalues = (thetime,str(thetemp),str(voltval),str(currentval),str(laserint),str(lasersn))
                cursor.execute(insert_q1, thevalues)
                cnx.commit()
                cursor.close()
                cnx.close()
            except mysql.connector.Error as err:
                print("Something went wrong: {}".format(err))
                if 'cnx' in locals():
                    cnx.close()
        
        if round(voltval) == 0 and round(laserint) == 0:
            if sleeptime < maxsleeptime:
                sleeptime = sleeptime + 10
            print "nothing happening, sleeping for: " + str(sleeptime) + " s"
        else:
            sleeptime = minsleeptime       
        
        time.sleep(sleeptime)
        
        

def turn_on(sx):
    print "Turning on Chiller"
    
    msg4 = "\xca\x00\x01\x81\x08\x01\x02\x02\x02\x02\x02\x02\x02\x66" #turn on
    serialreadwrite(sx,msg4)

def turn_off(sx):
    print "Turning off Chiller"
    msg3 = "\xca\x00\x01\x81\x08\x00\x02\x02\x02\x02\x02\x02\x02\x67" #turn off
    serialreadwrite(sx,msg3)




#get serial object
sx = serial.Serial(3,19200,timeout=0,parity='N',stopbits=1)


#connect to mysql server


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

sx.close()
soc.close()

print "stopping...exiting"

