#Autofocus Semicon Demo 2021 Jacob Mader Billy Machado Dave Rego Finalized 11/11/2021
import cv2
import numpy as np
from pipython.pidevice.interfaces.piserial import PISerial
from pipython.pidevice.gcsmessages import GCSMessages
from pipython.pidevice.gcscommands import GCSCommands
import threading
import time
import os
import pyautogui
class Photo():
    def __init__(self):
        self.position = 0
        self.time = 0
        self.contrast = 0
        self.file = ""
        self.image = np.zeros((480,640))
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
    path0 = "Autofocus/Semicon/Semicon" + str(StartTime)
    if not os.path.exists(path0):
        os.makedirs(path0)
    path = "Autofocus/Semicon/Semicon" + str(StartTime) + "/Semicon" + timed
    os.mkdir(path)
    filename = path +"/Autofocus" + timed + ".csv"
    return filename, path
def WaitForMotionDone(device, axis):
    isMoving = True
    while isMoving:
        isMoving = device.IsMoving(axis)[axis]
def LoadDataSetCoarseStage(path, numpositions, spacing, Photos):
    print("debug2.21")
    for t in range(0, numpositions+spacing, spacing):
        print('debug2.22')
        print(t)
        print(numpositions)
        print(spacing)
        pic = Photo()
        print('debug2.23')
        pic.image = cv2.cvtColor(cv2.imread(path + "/" + str(t) + ".jpeg"), cv2.COLOR_BGR2GRAY)
        pic.position = t
        pic.file = path + "/" + str(t) + ".jpeg"
        Photos.append(pic)
def RunAutofocusCoarseStage(dataset, datasetnumimages, datasetspacing, contrastProc, step1, step2):
    # Create Internal Data Storage
    print("debug2.1")
    Photos = []
    Processes = []
    Contrast = []
    # Load Data
    print("debug2.2")
    LoadDataSetCoarseStage(dataset, datasetnumimages, datasetspacing, Photos)
    # Process Images
    print("debug2.3")
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
    print("debug2.5")
    AutofocusPosition = Photos[Contrast.index(MaxContrast)].position
    # Return Maximum
    return AutofocusPosition
def LoadDataSetFineStage(path, spacing1, spacing2, apos, Photos):
    leftrb = apos - spacing1
    rightrb = apos + spacing1
    if leftrb < 0: leftrb = 0
    if rightrb > 800: rightrb = 800
    for i in range(leftrb, rightrb + 1, spacing2):
        pic = Photo()
        pic.image = cv2.cvtColor(cv2.imread(path + "/" + str(i) + ".jpeg"), cv2.COLOR_BGR2GRAY)
        pic.position = i
        pic.file = path + "/" + str(i) + ".jpeg"
        Photos.append(pic)
def RunAutofocusFineStage(path, datasetspacing1, datasetspacing2, apos, contrastProc, step1, step2):
    # Create Internal Data Storage
    Photos = []
    Processes = []
    Contrast = []
    # Load Data
    LoadDataSetFineStage(path, datasetspacing1, datasetspacing2, apos, Photos)
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
    AutofocusPosition = Photos[Contrast.index(MaxContrast)].position
    # Return Maximum
    return AutofocusPosition
def MoveV408(spacing, v408):
    v408.MVR('1', spacing)
    WaitForMotionDone(v408, '1')
def ImageCapture(spacing, StartTime, pidevice, cap):
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
    for i in range(1, 80):
        # Step Through Z Positions
        pidevice.MVR('1', 10)
        WaitForMotionDone(pidevice, '1')
        ret, frame = cap.read()
        imagefile = path + "/" + str(i*spacing) + ".jpeg"
        cv2.imwrite(imagefile, frame)
    # Last Read Operation
    ret, frame = cap.read()
    imagefile = path + "/800.jpeg"
    cv2.imwrite(imagefile, frame)
    print('Done image capture')
    return filename, path
def ImageCaptureOnTheFly(spacing, StartTime, pidevice, cap):
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
    return filename, path
def RunAutofocusDemo(kpos, c891, e709):
    while True:
        # Control Start Procedure
        e709.MOV('1', 0)
        c891.MOV('1', -6)
        WaitForMotionDone(e709, '1')
        WaitForMotionDone(c891, '1')
        # "Scan" Procedure 1
        e709.MOV('1', 800)
        WaitForMotionDone(e709, '1')
        e709.MOV('1', kpos[0])
        WaitForMotionDone(e709, '1')
        time.sleep(0.25)
        e709.MOV('1', 0)
        c891.MVR('1', 3)
        WaitForMotionDone(e709, '1')
        WaitForMotionDone(c891, '1')
        # "Scan" Procedure 2
        e709.MOV('1', 800)
        WaitForMotionDone(e709, '1')
        e709.MOV('1', kpos[1])
        WaitForMotionDone(e709, '1')
        time.sleep(0.25)
        e709.MOV('1', 0)
        c891.MVR('1', 3)
        WaitForMotionDone(e709, '1')
        WaitForMotionDone(c891, '1')
        # "Scan" Procedure 3
        e709.MOV('1', 800)
        WaitForMotionDone(e709, '1')
        e709.MOV('1', kpos[2])
        WaitForMotionDone(e709, '1')
        time.sleep(0.25)
        e709.MOV('1', 0)
        c891.MVR('1', 3)
        WaitForMotionDone(e709, '1')
        WaitForMotionDone(c891, '1')
        # "Scan" Procedure 4
        e709.MOV('1', 800)
        WaitForMotionDone(e709, '1')
        e709.MOV('1', kpos[3])
        WaitForMotionDone(e709, '1')
        time.sleep(0.25)
        e709.MOV('1', 0)
        c891.MVR('1', 3)
        WaitForMotionDone(e709, '1')
        WaitForMotionDone(c891, '1')
        c891.MOV('1',-12.5)
        WaitForMotionDone(c891, '1')
        c891.MOV('1',12.5)
        WaitForMotionDone(c891, '1')
def Autofocus():
    #time.sleep(10)
    SetupStartTime = time.time()
    #Setup E709
    gateway = PISerial("/dev/ttyUSB0", 115200)
    messages = GCSMessages(gateway)
    e709 = GCSCommands(gcsmessage = messages)
    print(e709.qIDN())
    #Setup C-891
    gateway2 = PISerial("/dev/ttyUSB1", 115200)
    messages2 = GCSMessages(gateway2)
    c891 = GCSCommands(gcsmessage = messages2)
    #Power cycle in case already powered on
    e709.RBT()
    c891.RBT()
    time.sleep(10)
    #Setup E709
    gateway = PISerial("/dev/ttyUSB0", 115200)
    messages = GCSMessages(gateway)
    e709 = GCSCommands(gcsmessage = messages)
    #Setup C-891
    gateway2 = PISerial("/dev/ttyUSB1", 115200)
    messages2 = GCSMessages(gateway2)
    c891 = GCSCommands(gcsmessage = messages2)
    #Setup Camera
    cap = cv2.VideoCapture(0)
    #Move Stages to Start Position
    e709.SVO('1', 1)
    e709.MOV('1', 0)
    c891.EAX('1', 1)
    c891.RON('1', 1)
    c891.FRF(1)
    referencing = False
    while not referencing:
        referencing = c891.IsControllerReady()
    c891.SVO('1', 1)
    c891.MOV('1', -6)
    #Create Internal Storage
    filenames = []
    paths = []
    kpos = []
    # Grab Start Time
    ImageCaptureStartTime = time.time()
    # Image Capture Procedure 1
    filename, path = ImageCapture(10, ImageCaptureStartTime, e709, cap)
    MoveV408(3, c891)
    filenames.append(filename)
    paths.append(path)
    # Image Capture Procedure 2
    filename, path = ImageCapture(10, ImageCaptureStartTime, e709, cap)
    MoveV408(3, c891)
    filenames.append(filename)
    paths.append(path)
    # Image Capture Procedure 3
    filename, path = ImageCapture(10, ImageCaptureStartTime, e709, cap)
    MoveV408(3, c891)
    filenames.append(filename)
    paths.append(path)
    # Image Capture Procedure 4
    filename, path = ImageCapture(10, ImageCaptureStartTime, e709, cap)
    MoveV408(3, c891)
    filenames.append(filename)
    paths.append(path)
    ImageCaptureEndTime = time.time()
    # Data Processing DaC Procedure 1
    print("debug1")
    dp2StartTime = time.time()
    print("debug2")
    print(paths)
    ipos = RunAutofocusCoarseStage(paths[0], 800, 80, StefanContrastRestructuredV25X5A, 2, 2)
    print("debug3")
    fpos = RunAutofocusFineStage(paths[0], 80, 10, ipos, StefanContrastRestructuredV25X5A, 2, 2)
    print("debug4")
    kpos.append(fpos)
    print("debug")
    # Data Processing DaC Procedure 2
    ipos = RunAutofocusCoarseStage(paths[1], 800, 80, StefanContrastRestructuredV25X5A, 2, 2)
    fpos = RunAutofocusFineStage(paths[1], 80, 10, ipos, StefanContrastRestructuredV25X5A, 2, 2)
    kpos.append(fpos)
    # Data Processing DaC Procedure 3
    ipos = RunAutofocusCoarseStage(paths[2], 800, 80, StefanContrastRestructuredV25X5A, 2, 2)
    fpos = RunAutofocusFineStage(paths[2], 80, 10, ipos, StefanContrastRestructuredV25X5A, 2, 2)
    kpos.append(fpos)
    # Data Processing DaC Procedure 4
    ipos = RunAutofocusCoarseStage(paths[3], 800, 80, StefanContrastRestructuredV25X5A, 2, 2)
    fpos = RunAutofocusFineStage(paths[3], 80, 10, ipos, StefanContrastRestructuredV25X5A, 2, 2)
    kpos.append(fpos)
    dp2EndTime = time.time()
    SetupEndTime = time.time()
    print(kpos)
    print("Image Capture time: " + str(ImageCaptureEndTime - ImageCaptureStartTime))
    print("Data Processing time: " + str(dp2EndTime-dp2StartTime))
    print("Setup time: " + str(SetupEndTime - SetupStartTime))
    print("Lost time: " + str((SetupEndTime - SetupStartTime)-((dp2EndTime-dp2StartTime)+(ImageCaptureEndTime - ImageCaptureStartTime))))
    # Simulated Autofocus Procedure
    process = threading.Thread(target=RunAutofocusDemo, args=(kpos, c891, e709))
    process.start()
    # Display Procedure
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
    while (True):
        ret, frame = cap.read()
        frame = cv2.resize(frame, dim, interpolation= cv2.INTER_LINEAR)
        cv2.imshow(window_name, frame)
        k = cv2.waitKey(1)
        if k == ord(" "):
            break
    cap.release()
    cv2.destroyAllWindows()
if __name__ == '__main__':
    Autofocus()