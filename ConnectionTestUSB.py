
'''import cv2
import numpy as np'''
from pipython.pidevice.interfaces.piserial import PISerial
from pipython.pidevice.interfaces.piusb import PIUSB
from pipython.pidevice.interfaces.pisocket import PISocket
from pipython.pidevice.gcsmessages import GCSMessages
from pipython.pidevice.gcscommands import GCSCommands
import threading
import time
import os

E709path = "/dev/ttyUSB2" #ftdi usb
E709baud = 460800

C413path = "/dev/ttyUSB0"#ftdi usb
C413baud = 115200

C414host = "172.31.0.127"

def WaitForMotionDone(device, axis):
    isMoving = True
    while isMoving:
        isMoving = device.IsMoving(axis)[axis]

def ConnectSerial(path, baudrate):
    gateway = PISerial(path, baudrate)
    messages = GCSMessages(gateway)
    ControllerName = GCSCommands(gcsmessage = messages)
    print(ControllerName.qIDN())
    return ControllerName
    
def ConnectUSB(serialnumber, productid):
    gateway = PIUSB().connect(serialnumber, productid)
    messages = GCSMessages(gateway)
    ControllerName = GCSCommands(gcsmessage = messages)
    print(ControllerName.qIDN())
    return ControllerName

def ConnectTCP(C414host):
    gateway = PISocket(C414host, 50000)
    messages = GCSMessages(gateway)
    ControllerName = GCSCommands(gcsmessage = messages)
    print(ControllerName.qIDN())
    return ControllerName


def Autofocus():
    #############################################
    e709 = ConnectSerial(E709path,E709baud)
    c413 = ConnectSerial(C413path,C413baud)
    c414 = ConnectTCP(C414host)
    ############################################
    e709servo = e709.qSVO()['1']
    c413servo = c413.qSVO()['1']
    c414servo = c414.qSVO()['1']
    #############################################
    if e709servo != 1:
        print('Servoing on P-725')
        e709.SVO('1', 1)
    if c413servo != 1:
        print('Servoing on V-528')
        c413.SVO('1', 1)
    if c414servo != 1:
        print('Servoing on V-308')
        c414.SVO('1', 1)
    ############################################
    c414reference = c414.FRF(1)
    #e709reference = e709.FRF(1)
    c413reference = c413.FRF(1)
    #############################################
    time.sleep(10)
    ############################################
    e709max = e709.qTMX()['1']
    e709min = e709.qTMN()['1']
    
    c413max = c413.qTMX()['1']
    c413min = c413.qTMN()['1']
    
    c414max = c414.qTMX()['1']
    c414min = c414.qTMN()['1']
    
    print(f' E-709: {e709max} and {e709min} ')
    print(f' C-413: {c413max} and {c413min} ')
    print(f' C-414: {c414max} and {c414min} ')
    ###############################################
    while 1:
        e709.MOV('1', e709min)
        c413.MOV('1', c413min)
        c414.MOV('1', c414min)
        WaitForMotionDone(e709, 1)
        WaitForMotionDone(c413, 1)
        WaitForMotionDone(c414, 1)
        WaitForMotionDone(e709, 1)
        WaitForMotionDone(c413, 1)
        WaitForMotionDone(c414, 1)
        e709.MOV('1', e709max)
        c413.MOV('1', c413max)
        c414.MOV('1', c414max)
        WaitForMotionDone(e709, 1)
        WaitForMotionDone(c413, 1)
        WaitForMotionDone(c414, 1)
        WaitForMotionDone(e709, 1)
        WaitForMotionDone(c413, 1)
        WaitForMotionDone(c414, 1)
    ##############################################
if __name__ == '__main__':
    Autofocus()