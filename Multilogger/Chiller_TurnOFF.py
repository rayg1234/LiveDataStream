# -*- coding: utf-8 -*-
"""
Created on Tue Mar 11 11:32:55 2014

@author: ediff
"""

import serial
sx = serial.Serial(3,19200,timeout=0,parity='N',stopbits=1)
msg3 = "\xca\x00\x01\x81\x08\x00\x02\x02\x02\x02\x02\x02\x02\x67" #turn off
sx.write(msg3)
sx.close()