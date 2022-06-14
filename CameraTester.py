import cv2
import time

def camera():
    
    cap = cv2.VideoCapture(0)
    #cap2 = cv2.VideoCapture(1)
    while(1):
        ret, img = cap.read()
        #ret2, img2 = cap2.read()
        cv2.imshow("camera1",img)
        #cv2.imshow("Camera2",img2)
        if cv2.waitKey(1) & 0xFF == ord('q'):
             break
    #cap.release()
    
    
    cap2 = cv2.VideoCapture(2)
    while(1):
        ret2, img2 = cap2.read()
        cv2.imshow("Camera2",img2)
        if cv2.waitKey(1) & 0xFF == ord('q'):
             break
    #cap.release()
    cap2.release()
    cv2.destroyAllWindows()
    
camera()
#cap.release()
#cap2.release()