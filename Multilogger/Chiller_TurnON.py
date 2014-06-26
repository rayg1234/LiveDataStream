# -*- coding: utf-8 -*-
"""
Created on Tue Mar 11 11:32:01 2014

@author: ediff
"""

import serial
sx = serial.Serial(3,19200,timeout=0,parity='N',stopbits=1)
msg4 = "\xca\x00\x01\x81\x08\x01\x02\x02\x02\x02\x02\x02\x02\x66" #turn on
sx.write(msg4)
sx.close()