//Jacob Mader, 12/20/2021
//Developed for the purpose of reducing processing time of the Autofocus algorithm which had originally been developed in Python
#include "Autofocus.hpp"
#include "PiDevice.hpp"


using namespace std;
using namespace mn::CppLinuxSerial;
using namespace cv;



int main(int argc, char** argv)
{
	//RunAutoFocus(100, 5, 50, 5, 2, 2);
	PIDevice pidevice;
	VideoCapture cap;
	pidevice.connectSerial("/dev/ttyUSB0");
	double targets[6] = {0,0,0,0,0,0};
	pidevice.MOV("X Y Z U V W ", targets);
	//pidevice.sendReadSerial("*IDN?\n");
	//cout << pidevice.qPOS("X");
	short int pos = RunCoarseStageData(pidevice, cap, 100, 5, 50, 2, 2);
	/*VideoCapture cap;
	cap.open(0);
	 for (;;)
    {
		Mat frame;
        // wait for a new frame from camera and store it into 'frame'
        cap.read(frame);
        // check if we succeeded
        if (frame.empty()) {
            cerr << "ERROR! blank frame grabbed\n";
            break;
        }
        // show live and wait for a key with timeout long enough to show images
        imshow("Live", frame);
        if (waitKey(5) >= 0)
            break;
    }*/
    return 0;
}
