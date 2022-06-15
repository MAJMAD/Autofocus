#Autofocus Semicon West Demo 2022 Jacob Mader Billy Machado Dave Rego Finalized 6/14/2022
import cv2
import numpy as np
from pipython.pidevice.interfaces.piserial import PISerial
from pipython.pidevice.interfaces.pisocket import PISocket
from pipython.pidevice.gcsmessages import GCSMessages
from pipython.pidevice.gcscommands import GCSCommands
import threading
import time
import os
import pyautogui

E709path = "/dev/ttyUSB1" #ftdi usb1
E709baud = 460800
C413path = "/dev/ttyUSB0"#ftdi usb0
C413baud = 115200
C414host = "172.31.0.227"

xvel = 250
xsteps = 4
xstepsize = -1
xstagemin = -1
xstagemax = -1

zsteps = 100
zstepsize = -1
zstagemin = -1
zstagemax = -1

zsteps2 = 100
zstepsize2 = -1
zstage2min = -1
zstage2max = -1

speed = 2.5
formatter = "{0:.2f}"
whichcamera = 1
cam1 = 2
cam2 = 0


class Photo():
    def __init__(self):
        self.position = 0
        self.time = 0
        self.contrast = 0
        self.file = ""
        self.image = np.zeros((480,640))
        
def ConnectTCP(host):
    gateway = PISocket(host, 50000)
    messages = GCSMessages(gateway)
    ControllerName = GCSCommands(gcsmessage = messages)
    print(ControllerName.qIDN())
    return ControllerName
        
def ConnectSerial(path, baudrate):
    gateway = PISerial(path, baudrate)
    messages = GCSMessages(gateway)
    ControllerName = GCSCommands(gcsmessage = messages)
    print(ControllerName.qIDN())
    return ControllerName

def StefanContrastRestructuredV25X5A(image,step1,step2):
    # Alternating 5x5 grid tiled kernel, Regions 1, 3, 5, 7, 9, 11, 13, 15, 17, 19, 21, 23, 25
    summation = 0
    for row in range(1, 96, step1):  # First Kernel Row
        for pixel in range(1, 128, step2):  # Region 1
            summation += ((int(image[row, pixel])) * 4 - int(image[row - 1, pixel]) - int(image[row + 1, pixel]) - int(
                image[row, pixel - 1]) - int(image[row, pixel + 1])) ** 2
        for pixel in range(255, 382, step2):  # Region 3
            summation += ((int(image[row, pixel])) * 4 - int(image[row - 1, pixel]) - int(image[row + 1, pixel]) - int(
                image[row, pixel - 1]) - int(image[row, pixel + 1])) ** 2
        for pixel in range(509, 636, step2):  # Region 5
            summation += ((int(image[row, pixel])) * 4 - int(image[row - 1, pixel]) - int(image[row + 1, pixel]) - int(
                image[row, pixel - 1]) - int(image[row, pixel + 1])) ** 2
    for row in range(96, 191, step1):  # Second Kernel Row
        for pixel in range(128, 255, step2):  # Region 7
            summation += ((int(image[row, pixel])) * 4 - int(image[row - 1, pixel]) - int(image[row + 1, pixel]) - int(
                image[row, pixel - 1]) - int(image[row, pixel + 1])) ** 2
        for pixel in range(382, 509, step2):  # Region 9
            summation += ((int(image[row, pixel])) * 4 - int(image[row - 1, pixel]) - int(image[row + 1, pixel]) - int(
                image[row, pixel - 1]) - int(image[row, pixel + 1])) ** 2
    for row in range(191, 286, step1):  # Third Kernel Row
        for pixel in range(1, 128, step2):  # Region 11
            summation += ((int(image[row, pixel])) * 4 - int(image[row - 1, pixel]) - int(image[row + 1, pixel]) - int(
                image[row, pixel - 1]) - int(image[row, pixel + 1])) ** 2
        for pixel in range(255, 382, step2):  # Region 13
            summation += ((int(image[row, pixel])) * 4 - int(image[row - 1, pixel]) - int(image[row + 1, pixel]) - int(
                image[row, pixel - 1]) - int(image[row, pixel + 1])) ** 2
        for pixel in range(509, 636, step2):  # Region 15
            summation += ((int(image[row, pixel])) * 4 - int(image[row - 1, pixel]) - int(image[row + 1, pixel]) - int(
                image[row, pixel - 1]) - int(image[row, pixel + 1])) ** 2
    for row in range(286, 381, step1):  # Fourth Kernel Row
        for pixel in range(128, 255, step2):  # Region 17
            summation += ((int(image[row, pixel])) * 4 - int(image[row - 1, pixel]) - int(image[row + 1, pixel]) - int(
                image[row, pixel - 1]) - int(image[row, pixel + 1])) ** 2
        for pixel in range(382, 509, step2):  # Region 19
            summation += ((int(image[row, pixel])) * 4 - int(image[row - 1, pixel]) - int(image[row + 1, pixel]) - int(
                image[row, pixel - 1]) - int(image[row, pixel + 1])) ** 2
    for row in range(381, 476, step1):  # Fifth Kernel Row
        for pixel in range(1, 128, step2):  # Region 21
            summation += ((int(image[row, pixel])) * 4 - int(image[row - 1, pixel]) - int(image[row + 1, pixel]) - int(
                image[row, pixel - 1]) - int(image[row, pixel + 1])) ** 2
        for pixel in range(255, 382, step2):  # Region 23
            summation += ((int(image[row, pixel])) * 4 - int(image[row - 1, pixel]) - int(image[row + 1, pixel]) - int(
                image[row, pixel - 1]) - int(image[row, pixel + 1])) ** 2
        for pixel in range(509, 636, step2):  # Region 25
            summation += ((int(image[row, pixel])) * 4 - int(image[row - 1, pixel]) - int(image[row + 1, pixel]) - int(
                image[row, pixel - 1]) - int(image[row, pixel + 1])) ** 2
    L = summation / ((126*94*13)/(step1*step2))
    return L

def ProcessContrast(ContrastProc, pic, step1, step2):
    pic.contrast = ContrastProc(pic.image, step1, step2)
    
def CreateDataFile(StartTime):
    timed = time.strftime("%Y-%m-%d-%H-%M-%S")
    path0 = "Semicon West/Runs/Semicon" + str(StartTime)
    if not os.path.exists(path0):
        os.makedirs(path0)
    path = "Semicon West/Runs/Semicon" + str(StartTime) + "/Semicon" + timed
    os.mkdir(path)
    filename = path +"/Autofocus" + timed + ".csv"
    return filename, path

def WaitForMotionDone(device, axis):
    isMoving = True
    while isMoving:
        isMoving = device.IsMoving(axis)[axis]
        
def LoadDataSetCoarseStage(path, numpositions, spacing, Photos):
    #print("debug2.21")
    #print('debug2.22')
    #print(numpositions)
    #print(spacing)
    pic = Photo()
    #print('debug2.23')
    pic.image = cv2.cvtColor(cv2.imread(path + "/" + "0.00" + ".jpeg"), cv2.COLOR_BGR2GRAY)
    pic.position = 0
    pic.file = path + "/" + '0.00' + ".jpeg"
    Photos.append(pic)
    for t in range(1, 11):
        print('debug2.32')
        print(t)
        print(numpositions)
        print(spacing)
        pic = Photo()
        print('debug2.33')
        print(path + "/" + str(t*spacing) + ".jpeg")
        print(formatter.format(t*spacing, ':.2f'))
        pic.image = cv2.cvtColor(cv2.imread(path + "/" + formatter.format(t*spacing, ':.2f') + ".jpeg"), cv2.COLOR_BGR2GRAY)
        pic.position = t
        pic.file = path + "/" + formatter.format(t*spacing, ':.2f') + ".jpeg"
        Photos.append(pic)
        
def RunAutofocusCoarseStage(dataset, datasetnumimages, datasetspacing, contrastProc, step1, step2, whichstage ):
    # Create Internal Data Storage
    #print("debug2.1")
    Photos = []
    Processes = []
    Contrast = []
    # Load Data
    #print("debug2.2")
    LoadDataSetCoarseStage(dataset, datasetnumimages, datasetspacing, Photos)
    # Process Images
    #print("debug2.3")
    for pic in Photos:
        process = threading.Thread(target=ProcessContrast, args=(contrastProc, pic, step1, step2))
        process.start()
        Processes.append(process)
    for process in Processes:
        process.join()
    for pic in Photos:
        Contrast.append(pic.contrast)
    # Find Maximum
    print("debug2.4")
    MaxContrast = max(Contrast)
    print(MaxContrast)
    print(Contrast)
    print("debug2.5")
    AutofocusPosition = Photos[Contrast.index(MaxContrast)].position * datasetspacing
    print('index of max contrast')
    print(Contrast.index(MaxContrast))
    print('AutofocusPosition')
    print(AutofocusPosition)
    # Return Maximum
    return AutofocusPosition

def LoadDataSetFineStage(path, spacing1, spacing2, apos, Photos, whichstage):
    print('LoadDataSetFineStage')
    if whichstage == 1:
        thesezsteps = zsteps
        thiszstagemin = zstagemin
        thiszstagemax = zstagemax
    else:
        thesezsteps = zsteps2
        thiszstagemin = zstage2min
        thiszstagemax = zstage2max
    looprange = -1
    leftrb = apos - spacing1
    rightrb = apos + spacing1
    if leftrb < thiszstagemin:
        leftrb = thiszstagemin
        looprange = 1
    if rightrb > thiszstagemax:
        rightrb = thiszstagemax
        looprange = 1
    if looprange == 1:
        looprange = (rightrb - leftrb) / (spacing2 * 2)
    else:
        looprange = (rightrb - leftrb) / spacing2 
    print(spacing1)
    print(spacing2)
    print(looprange)
    print(leftrb)
    print(rightrb)
    for i in range(1, int(looprange)):
        pic = Photo()
        pic.image = cv2.cvtColor(cv2.imread(path + "/" + formatter.format(leftrb+i*spacing2, ':.2f') + ".jpeg"), cv2.COLOR_BGR2GRAY)
        pic.position = i
        pic.file = path + "/" + str(i) + ".jpeg"
        Photos.append(pic)
    return leftrb
        
def RunAutofocusFineStage(path, datasetspacing1, datasetspacing2, apos, contrastProc, step1, step2, zid):
    # Create Internal Data Storage
    Photos = []
    Processes = []
    Contrast = []
    # Load Data
    lb = LoadDataSetFineStage(path, datasetspacing1, datasetspacing2, apos, Photos, zid)
    # Process Images
    for pic in Photos:
        process = threading.Thread(target=ProcessContrast, args=(contrastProc, pic, step1, step2))
        process.start()
        Processes.append(process)
    for process in Processes:
        process.join()
    for pic in Photos:
        Contrast.append(pic.contrast)
    # Find Maximum
    MaxContrast = max(Contrast)
    print(Contrast)
    print('lb')
    print(lb)
    AutofocusPosition = lb + Photos[Contrast.index(MaxContrast)].position * datasetspacing2
    print(AutofocusPosition)
    # Return Maximum
    return AutofocusPosition

def MoveXStage(spacing, xstage):
    xstage.MVR('1', spacing)
    WaitForMotionDone(xstage, '1')
    
def HomeXStage(xstage):
    xstage.MOV('1', float(formatter.format(xstagemin, '.2f')))
    WaitForMotionDone(xstage, '1')
    
def ImageCapture(spacing, StartTime, pidevice, cap, whichstage):
    if whichstage == 1:
        thesezsteps = zsteps
        thiszstagemin = zstagemin
        thiszstagemax = zstagemax
    else:
        thesezsteps = zsteps2
        thiszstagemin = zstage2min
        thiszstagemax = zstage2max
    thiszstageminstring = str(thiszstagemin)
    thiszstagemaxstring = str(thiszstagemax)
    # First Read Operation
    ret, frame = cap.read()
    # Move to Start Position (Z-Min)
    #print(formatter.format(zstagemin, '.2f'))
    pidevice.MOV('1', float(formatter.format(thiszstagemin, '.2f')))
    WaitForMotionDone(pidevice, '1')
    # First Significant Read Operation (First is shit second is hit)
    ret, frame = cap.read()
    # Create Data Folder
    filename, path = CreateDataFile(StartTime)
    # First Read Operation
    imagefile = path + "/" + thiszstageminstring + '0' + ".jpeg"
    cv2.imwrite(imagefile, frame)
    # Move to Start Position (Z-Min)
    #pidevice.MOV('1', 0)
    #WaitForMotionDone(pidevice, '1')
    for i in range(1, thesezsteps):
        # Step Through Z Positions
        #print(i)
        pidevice.MVR('1', spacing)
        WaitForMotionDone(pidevice, '1')
        ret, frame = cap.read()
        imagefile = path + "/" + str(format(i*spacing, '.2f')) + ".jpeg"
        cv2.imwrite(imagefile, frame)
    # Last Read Operation
    pidevice.MOV('1', float(formatter.format(thiszstagemax, '.2f')))
    WaitForMotionDone(pidevice, '1')
    ret, frame = cap.read()
    imagefile = path + "/" + thiszstagemaxstring + "0" + ".jpeg" 
    cv2.imwrite(imagefile, frame)
    print('Done image capture')
    return filename, path

'''def ImageCaptureOnTheFly(spacing, StartTime, pidevice, cap):
    # First Read Operation
    ret, frame = cap.read()
    # Create Data Folder
    filename, path = CreateDataFile(StartTime)
    # First Read Operation
    imagefile = path + "/" + "0" + ".jpeg"
    cv2.imwrite(imagefile, frame)
    # Move to Start Position (Z-Min)
    pidevice.MOV('1', 0)
    WaitForMotionDone(pidevice, '1')
    motionflag = True
    keyposition = spacing
    pidevice.MOV('1', 800)
    while motionflag:
        if keyposition < pidevice.qPOS('1')['1']:
            pic = Photo()
            ret, frame = cap.read()
            imagefile = path + "/" + str(keyposition) + ".jpeg"
            cv2.imwrite(imagefile, frame)
            keyposition = keyposition + spacing
        if round(pidevice.qPOS('1')['1']) == 800:
            motionflag = False
    # Last Read Operation
    ret, frame = cap.read()
    imagefile = path + "/800.jpeg"
    cv2.imwrite(imagefile, frame)
    print('Done image capture')
    return filename, path'''

def RunAutofocusDemo(kpos, xstage, zstage, zstage2):
    
    while True:
        
        #Assign Camera
        globals()['whichcamera'] = 1
        
        # Control Start Procedure
        zstage.MOV('1', zstagemin)
        xstage.MOV('1', xstagemin)
        WaitForMotionDone(zstage, '1')
        WaitForMotionDone(xstage, '1')
        
        
        # "Scan" Procedure 1
        #zstage.MOV('1', zstagemax)
        #WaitForMotionDone(zstage, '1')
        zstage.MOV('1', kpos[0])
        WaitForMotionDone(zstage, '1')
        #time.sleep(10)
        time.sleep(1/speed)
        
        # "Set Up" Procedure 1
        zstage.MOV('1', zstagemin)
        xstage.MVR('1', xstepsize)
        WaitForMotionDone(zstage, '1')
        WaitForMotionDone(xstage, '1')
        
        # "Scan" Procedure 2
        #zstage.MOV('1', zstagemax)
        #WaitForMotionDone(zstage, '1')
        zstage.MOV('1', kpos[1])
        WaitForMotionDone(zstage, '1')
        #time.sleep(10)
        time.sleep(1/speed)
        
        # "Set Up" Procedure 2
        zstage.MOV('1', zstagemin)
        xstage.MVR('1', xstepsize)
        WaitForMotionDone(zstage, '1')
        WaitForMotionDone(xstage, '1')
        
        # "Scan" Procedure 3
        #zstage.MOV('1', zstagemax)
        #WaitForMotionDone(zstage, '1')
        zstage.MOV('1', kpos[2])
        WaitForMotionDone(zstage, '1')
        #time.sleep(10)
        time.sleep(1/speed)
        
        # "Set Up" Procedure 3
        zstage.MOV('1', zstagemin)
        xstage.MVR('1', xstepsize)
        WaitForMotionDone(zstage, '1')
        WaitForMotionDone(xstage, '1')
        
        # "Scan" Procedure 4
        #zstage.MOV('1', zstagemax)
        #WaitForMotionDone(zstage, '1')
        zstage.MOV('1', kpos[3])
        WaitForMotionDone(zstage, '1')
        #time.sleep(10)
        time.sleep(1/speed)
        
        # "Set Up" Procedure 4
        zstage.MOV('1', zstagemin)
        xstage.MVR('1', xstepsize)
        WaitForMotionDone(zstage, '1')
        WaitForMotionDone(xstage, '1')
        
        #Flex Travel
        zstage.MOV('1', zstagemax)
        WaitForMotionDone(zstage, '1')
        zstage.MOV('1', zstagemin)
        WaitForMotionDone(zstage, '1')
        zstage.MOV('1', zstagemax)
        WaitForMotionDone(zstage, '1')
        zstage.MOV('1', zstagemin)
        WaitForMotionDone(zstage, '1')
        xstage.MOV('1', xstagemin)
        WaitForMotionDone(xstage, '1')
        xstage.MOV('1', xstagemax)
        WaitForMotionDone(xstage, '1')
        
        #Switch Camera
        globals()['whichcamera'] = 2
        
        # Control Start Procedure
        zstage.MOV('1', zstage2min)
        xstage.MOV('1', xstagemin)
        WaitForMotionDone(zstage2, '1')
        WaitForMotionDone(xstage, '1')
        
        
        # "Scan" Procedure 5
        #zstage.MOV('1', zstagemax)
        #WaitForMotionDone(zstage, '1')
        zstage2.MOV('1', kpos[4])
        WaitForMotionDone(zstage2, '1')
        #time.sleep(10)
        time.sleep(1/speed)
        
        # "Set Up" Procedure 5
        zstage2.MOV('1', zstage2min)
        xstage.MVR('1', xstepsize)
        WaitForMotionDone(zstage2, '1')
        WaitForMotionDone(xstage, '1')
        
        # "Scan" Procedure 6
        #zstage.MOV('1', zstagemax)
        #WaitForMotionDone(zstage, '1')
        zstage2.MOV('1', kpos[5])
        WaitForMotionDone(zstage2, '1')
        #time.sleep(10)
        time.sleep(1/speed)
        
        # "Set Up" Procedure 6
        zstage2.MOV('1', zstage2min)
        xstage.MVR('1', xstepsize)
        WaitForMotionDone(zstage2, '1')
        WaitForMotionDone(xstage, '1')
        
        # "Scan" Procedure 7
        #zstage.MOV('1', zstagemax)
        #WaitForMotionDone(zstage, '1')
        zstage2.MOV('1', kpos[6])
        WaitForMotionDone(zstage2, '1')
        #time.sleep(10)
        time.sleep(1/speed)
        
        # "Set Up" Procedure 7
        zstage2.MOV('1', zstage2min)
        xstage.MVR('1', xstepsize)
        WaitForMotionDone(zstage2, '1')
        WaitForMotionDone(xstage, '1')
        
        # "Scan" Procedure 8
        #zstage.MOV('1', zstagemax)
        #WaitForMotionDone(zstage, '1')
        zstage2.MOV('1', kpos[7])
        WaitForMotionDone(zstage2, '1')
        #time.sleep(10)
        time.sleep(1/speed)
        
        # "Set Up" Procedure 8
        zstage2.MOV('1', zstage2min)
        xstage.MVR('1', xstepsize)
        WaitForMotionDone(zstage2, '1')
        WaitForMotionDone(xstage, '1')
        
        #Flex Travel
        zstage2.MOV('1', zstage2max)
        WaitForMotionDone(zstage2, '1')
        zstage2.MOV('1', zstage2min)
        WaitForMotionDone(zstage2, '1')
        zstage2.MOV('1', zstage2max)
        WaitForMotionDone(zstage2, '1')
        zstage2.MOV('1', zstage2min)
        WaitForMotionDone(zstage2, '1')
        xstage.MOV('1', xstagemin)
        WaitForMotionDone(xstage, '1')
        xstage.MOV('1', xstagemax)
        WaitForMotionDone(xstage, '1')
        
        
        
        
        
def Autofocus():
    #############################################
    SetupStartTime = time.time()
    #############################################
    #Connect Stages
    zstage = ConnectTCP(C414host)
    zstage2 = ConnectSerial(E709path,E709baud)
    xstage = ConnectSerial(C413path,C413baud)
    ############################################
    #Servo Control
    zstage2servo = zstage2.qSVO()['1']
    xstageservo = xstage.qSVO()['1']
    zstageservo = zstage.qSVO()['1']
    if zstage2servo != 1:
        #print('Servoing on P-725')
        zstage2.SVO('1', 1)
    if xstageservo != 1:
        #print('Servoing on V-528')
        xstage.SVO('1', 1)
    if zstageservo != 1:
        #print('Servoing on V-308')
        zstage.SVO('1', 1)
    #zstage2servo = zstage2.qSVO()['1']
    #xstageservo = xstage.qSVO()['1']
    #zstageservo = zstage.qSVO()['1']
    #print(f' E-709 Servo Status: {zstage2servo}')
    #print(f' C-413 Servo Status: {xstageservo}')
    #print(f' C-414 Servo Status: {zstageservo}')
    ############################################
    #Reference Control
    zstage.FRF(1)
    xstage.FRF(1)
    time.sleep(15)
    #zstagereference = zstage.qFRF()['1']
    #xstagereference = xstage.qFRF()['1']
    #print(f' E-709 Reference Status: NA')
    #print(f' C-413 Reference Status: {xstagereference}')
    #print(f' C-414 Reference Status: {zstagereference}')
    #############################################
    #AutoZero Control
    zstage.ATZ()
    zstage2.ATZ()
    time.sleep(10)
    #zstageautozero = zstage.qATZ()['1']
    #zstage2autozero = zstage2.qATZ()['1']
    #xstageautozero = xstage.qATZ()['1']
    #print(f' E-709 AutoZero Status: {zstage2autozero}')
    #print(f' C-413 Reference Status: {xstageautozero}')
   #print(f' C-414 AutoZero Status: {zstageautozero}')
    #############################################
    time.sleep(10)
    ############################################
    #Velocity Control
    xstage.VEL('1', xvel)
    ############################################
    globals()['zstage2max'] = zstage2.qTMX()['1']
    globals()['zstage2min'] = zstage2.qTMN()['1']
    globals()['zstepsize2'] = ((zstage2max-zstage2min)/zsteps2)
    
    globals()['xstagemax'] = xstage.qTMX()['1']
    globals()['xstagemin'] = xstage.qTMN()['1']
    globals()['xstepsize'] = ((xstagemax-xstagemin)/xsteps)
    
    globals()['zstagemax'] = zstage.qTMX()['1']
    globals()['zstagemin'] = zstage.qTMN()['1']
    globals()['zstepsize'] = ((zstagemax-zstagemin)/zsteps)
    #############################################
    
    #print(f' E-709: {zstage2max} and {zstage2min} ')
    #print(f' C-413: {xstagemax} and {xstagemin} ')
    #print(f' C-414: {zstagemax} and {zstagemin} ')
    ###############################################
    #Setup Camera
    cap = cv2.VideoCapture(cam1)
    cap2 = cv2.VideoCapture(cam2)
    #Move Stages to Start Position
    xstage.MOV('1', xstagemin)
    WaitForMotionDone(xstage, '1')
    #Create Internal Storage
    filenames = []
    paths = []
    kpos = []
    # Grab Start Time
    ImageCaptureStartTime = time.time()
    # Image Capture Procedure 1
    filename, path = ImageCapture(zstepsize, ImageCaptureStartTime, zstage, cap2, 1)
    MoveXStage(xstepsize, xstage)
    filenames.append(filename)
    paths.append(path)
    # Image Capture Procedure 2
    filename, path = ImageCapture(zstepsize, ImageCaptureStartTime, zstage, cap2, 1)
    MoveXStage(xstepsize, xstage)
    filenames.append(filename)
    paths.append(path)
    # Image Capture Procedure 3
    filename, path = ImageCapture(zstepsize, ImageCaptureStartTime, zstage, cap2, 1)
    MoveXStage(xstepsize, xstage)
    filenames.append(filename)
    paths.append(path)
    # Image Capture Procedure 4
    filename, path = ImageCapture(zstepsize, ImageCaptureStartTime, zstage, cap2, 1)
    MoveXStage(xstepsize, xstage)
    filenames.append(filename)
    paths.append(path)
    #Switch Z Stages
    HomeXStage(xstage)
    # Image Capture Procedure 5
    filename, path = ImageCapture(zstepsize2, ImageCaptureStartTime, zstage2, cap, 2)
    MoveXStage(xstepsize, xstage)
    filenames.append(filename)
    paths.append(path)
    # Image Capture Procedure 6
    filename, path = ImageCapture(zstepsize2, ImageCaptureStartTime, zstage2, cap, 2)
    MoveXStage(xstepsize, xstage)
    filenames.append(filename)
    paths.append(path)
    # Image Capture Procedure 7
    filename, path = ImageCapture(zstepsize2, ImageCaptureStartTime, zstage2, cap, 2)
    MoveXStage(xstepsize, xstage)
    filenames.append(filename)
    paths.append(path)
    # Image Capture Procedure 8
    filename, path = ImageCapture(zstepsize2, ImageCaptureStartTime, zstage2, cap, 2)
    MoveXStage(xstepsize, xstage)
    filenames.append(filename)
    paths.append(path)
    ImageCaptureEndTime = time.time()
    # Data Processing DaC Procedure 1
    print("debug1")
    dp2StartTime = time.time()
    print("debug2")
    print(paths)
    ipos = RunAutofocusCoarseStage(paths[0], zstagemax - zstagemin , ((zstagemax - zstagemin) / 10), StefanContrastRestructuredV25X5A, 2, 2, 1)
    print('ipos')
    print(ipos)
    print("debug3")
    fpos = RunAutofocusFineStage(paths[0], ((zstagemax - zstagemin) / 10), ((zstagemax - zstagemin) / 100), ipos, StefanContrastRestructuredV25X5A, 2, 2, 1)
    #print("debug4")
    kpos.append(fpos)
    #print("debug5")
    # Data Processing DaC Procedure 2
    ipos = RunAutofocusCoarseStage(paths[1], zstagemax - zstagemin , ((zstagemax - zstagemin) / 10), StefanContrastRestructuredV25X5A, 2, 2, 1)
    fpos = RunAutofocusFineStage(paths[1], ((zstagemax - zstagemin) / 10), ((zstagemax - zstagemin) / 100), ipos, StefanContrastRestructuredV25X5A, 2, 2, 1)
    kpos.append(fpos)
    # Data Processing DaC Procedure 3
    iipos = RunAutofocusCoarseStage(paths[2], zstagemax - zstagemin , ((zstagemax - zstagemin) / 10), StefanContrastRestructuredV25X5A, 2, 2, 1)
    fpos = RunAutofocusFineStage(paths[2], ((zstagemax - zstagemin) / 10), ((zstagemax - zstagemin) / 100), ipos, StefanContrastRestructuredV25X5A, 2, 2, 1)
    kpos.append(fpos)
    # Data Processing DaC Procedure 4
    ipos = RunAutofocusCoarseStage(paths[3], zstagemax - zstagemin , ((zstagemax - zstagemin) / 10), StefanContrastRestructuredV25X5A, 2, 2, 1)
    fpos = RunAutofocusFineStage(paths[3], ((zstagemax - zstagemin) / 10), ((zstagemax - zstagemin) / 100), ipos, StefanContrastRestructuredV25X5A, 2, 2, 1)
    kpos.append(fpos)
    # Data Processing DaC Procedure 5
    ipos = RunAutofocusCoarseStage(paths[4], zstage2max - zstage2min , ((zstage2max - zstage2min) / 10), StefanContrastRestructuredV25X5A, 2, 2, 2)
    fpos = RunAutofocusFineStage(paths[4], ((zstage2max - zstage2min) / 10), ((zstage2max - zstage2min) / 100), ipos, StefanContrastRestructuredV25X5A, 2, 2, 2)
    kpos.append(fpos)
    # Data Processing DaC Procedure 6
    iipos = RunAutofocusCoarseStage(paths[5], zstage2max - zstage2min , ((zstage2max - zstage2min) / 10), StefanContrastRestructuredV25X5A, 2, 2, 2)
    fpos = RunAutofocusFineStage(paths[5], ((zstage2max - zstage2min) / 10), ((zstage2max - zstage2min) / 100), ipos, StefanContrastRestructuredV25X5A, 2, 2, 2)
    kpos.append(fpos)
    # Data Processing DaC Procedure 7
    ipos = RunAutofocusCoarseStage(paths[6], zstage2max - zstage2min , ((zstage2max - zstage2min) / 10), StefanContrastRestructuredV25X5A, 2, 2, 2)
    fpos = RunAutofocusFineStage(paths[6], ((zstage2max - zstage2min) / 10), ((zstage2max - zstage2min) / 100), ipos, StefanContrastRestructuredV25X5A, 2, 2, 2)
    kpos.append(fpos)
    # Data Processing DaC Procedure 8
    ipos = RunAutofocusCoarseStage(paths[7], zstage2max - zstage2min , ((zstage2max - zstage2min) / 10), StefanContrastRestructuredV25X5A, 2, 2, 2)
    fpos = RunAutofocusFineStage(paths[7], ((zstage2max - zstage2min) / 10), ((zstage2max - zstage2min) / 100), ipos, StefanContrastRestructuredV25X5A, 2, 2, 2)
    kpos.append(fpos)
    dp2EndTime = time.time()
    SetupEndTime = time.time()
    #print(kpos)
    print("Image Capture time: " + str(ImageCaptureEndTime - ImageCaptureStartTime))
    print("Data Processing time: " + str(dp2EndTime-dp2StartTime))
    print("Setup time: " + str(SetupEndTime - SetupStartTime))
    print("Lost time: " + str((SetupEndTime - SetupStartTime)-((dp2EndTime-dp2StartTime)+(ImageCaptureEndTime - ImageCaptureStartTime))))
    
    
    # Simulated Autofocus Procedure
    process = threading.Thread(target=RunAutofocusDemo, args=(kpos, xstage, zstage, zstage2))
    process.start()
    
    
    
    # Display Procedure
    '''window_name = 'screen'
    cv2.namedWindow(window_name, cv2.WND_PROP_FULLSCREEN)
    cv2.moveWindow(window_name, 0, 0)
    cv2.setWindowProperty(window_name, cv2.WND_PROP_FULLSCREEN, cv2.WINDOW_FULLSCREEN)'''
    ret, img = cap.read()
    window_name = "window"
    screen_res = pyautogui.size()
    scale_width = screen_res[0] / img.shape[1]
    scale_height = screen_res[1] / img.shape[0]
    window_width = int(img.shape[1] * scale_width)
    window_height = int(img.shape[0] * scale_height)
    dim = (window_width, window_height)
    cv2.namedWindow(window_name, cv2.WND_PROP_FULLSCREEN)
    cv2.setWindowProperty(window_name, cv2.WND_PROP_FULLSCREEN, cv2.WINDOW_FULLSCREEN)
    cv2.namedWindow(window_name, cv2.WINDOW_AUTOSIZE)
    print(whichcamera)
    while (1):
        while (whichcamera == 1):
            ret, frame = cap2.read()
            cv2.putText(frame, 'Voice Coil', (10,30), cv2.FONT_HERSHEY_SIMPLEX, 1, (255,255,255), 2)
            frame = cv2.resize(frame, dim, interpolation= cv2.INTER_LINEAR)
            cv2.imshow(window_name, frame)
            k = cv2.waitKey(1)
            if k == ord(" "):
                exit(1)
                break
        while (whichcamera == 2):
            ret, frame = cap.read()
            cv2.putText(frame, 'Piezo', (10,30), cv2.FONT_HERSHEY_SIMPLEX, 1, (255,255,255), 2)
            frame = cv2.resize(frame, dim, interpolation= cv2.INTER_LINEAR)
            cv2.imshow(window_name, frame)
            k = cv2.waitKey(1)
            if k == ord(" "):
                exit(1)
                break
    cap.release()
    cap2.release()
    cv2.destroyAllWindows()
if __name__ == '__main__':
    Autofocus()