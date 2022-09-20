import cv2
import numpy as np
import threading
import time
import os
import pyautogui

#cap = cv2.VideoCapture(0)
#ret, img = cap.read()
#print(img.shape[0], img.shape[1])

def LoadScreen(message = "Physik Instrumente", timedelay = 10):
    img = np.zeros([480, 640, 3])
    img.fill(255)
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
    starttime = time.time()
    while (1):
        frame = img
        cv2.putText(frame, message, (160, 210), cv2.FONT_HERSHEY_SIMPLEX, 1, (255, 0, 0), 2)
        frame = cv2.resize(frame, dim, interpolation=cv2.INTER_LINEAR)
        cv2.imshow(window_name, frame)
        k = cv2.waitKey(1)
        if k == ord(" "):
            exit(1)
            break
        if time.time() - starttime > timedelay:
            break
    cv2.destroyAllWindows()

if __name__ == '__main__':
    LoadScreen()
    LoadScreen("Autofocus Demo", 5)
    LoadScreen("Initializing", 5)
    LoadScreen("Starting Voice Coil Image Capture", 3)
    LoadScreen("Done Voice Coil Image Capture", 3)
    LoadScreen("Starting Piezo Image Capture", 3)
    LoadScreen("Done Piezo Image Capture", 3)
    LoadScreen("Processing Data", 10)
    LoadScreen("Outputting Video Feed", 3)


