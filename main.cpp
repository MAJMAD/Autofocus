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
	//Controlled start
	PIDevice e709;
	PIDevice c891;
	VideoCapture cap;
	vector<string> e709responses;
	vector<string> c891responses;
	string e709response;
	string c891response;
	int e709index;
	int c891index;
	c891.connectSerial("/dev/ttyUSB0");
	c891.DeviceType = "C-891";
	//c891.sendReadSerial("*IDN?\n");
	//c891.sendReadSerial("POS?\n");
	e709.connectSerial("/dev/ttyUSB1");
	e709.DeviceType = "E-709";
	
	//c891response = c891.read();
	//cout << c891response << "\n";
	
	
	
	
	c891.sendSerial("RBT\n");
	e709.sendSerial("RBT\n");
	cout << "Sleeping for 10 seconds while rebooting" << "\n"; 
	sleep(10);
	c891.connectSerial("/dev/ttyUSB0");
	e709.connectSerial("/dev/ttyUSB1");
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	e709.sendSerial("SVO 1 1\n");
	e709.sendSerial("MOV 1 0\n");
	c891.sendSerial("EAX 1 1\n");
	c891.sendSerial("FRF 1\n");
	cout << "Sleeping for 10 seconds while referencing" << "\n"; 
	sleep(10);
	
	
	
	
	
	
	
	c891.sendSerial("SVO 1 1\n");
	
	c891.sendSerial("MOV 1 -6\n");
	c891.WaitOnTarget();
	
	c891.sendSerial("POS?\n");
	c891response = c891.read();
	cout << c891response;
	
	c891.sendSerial("MOV 1 0\n");
	c891.WaitOnTarget();
	
	c891.sendSerial("POS?\n");
	c891response = c891.read();
	cout << c891response;
	
	
	
	/*
	
	
	
	
	
	
	c891response = c891.sendReadSerial("ONT?\n");
	//cout << c891response << "\n";
	c891response = c891.read();
	cout << c891response << "\n";
	c891response = c891.read();
	cout << c891response << "\n";
	int loopcount = 0;
	
	
	
	
	
	while (true){
		//c891.sendReadSerial("POS?\n");
		c891.sendSerial("MOV 1 -6\n");
		c891.WaitOnTarget();
		c891.sendSerial("POS?\n");
		c891response = c891.read();
		cout << c891response;
		//c891.sendReadSerial("POS?\n");
		//cout << "Sleeping for 1 second while in motion" << "\n"; 
		//sleep(1);
	
		//c891.sendReadSerial("POS?\n");
		c891.sendSerial("MOV 1 0\n");
		c891.WaitOnTarget();
		c891.sendSerial("POS?\n");
		c891response = c891.read();
		cout << c891response;
		//c891.sendReadSerial("POS?\n");
		cout << "Sleeping for 1 second while in motion" << "\n"; 
		sleep(1);
		loopcount++;
		while(true){
			c891response = c891.sendReadSerial("ONT?\n");
			c891response = c891.read();
			cout << c891response << "\n\n";
			if (c891response == "C-891: 1=1\n"){
				cout << c891response;
				break;
			}
		}
		break;
	}
	cout << "Buffer clear: " << c891.sendReadSerial("\n\n\n\n\n\n\n");
	cout << c891.sendReadSerial("ONT?\n");
	
	
	
	
	
	//e709.sendReadSerial("*IDN?\n");
	//e709.sendReadSerial("*IDN?\n");
	//double targets[1] = {0};
	//pidevice.MOV("1 ", targets);
	//double target[1] = {5};
	//pidevice.MOV("1 ", target);
	//e709.sendReadSerial("*IDN?\n");
	//e709.sendReadSerial("*IDN?\n");
	//cout << pidevice.qPOS("X");
	//short int pos = RunCoarseStageData(pidevice, cap, 100, 5, 50, 2, 2);
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
    c891.closeSerial();
    e709.closeSerial();
    return 0;
}
