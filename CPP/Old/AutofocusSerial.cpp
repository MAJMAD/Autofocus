#include <CppLinuxSerial/SerialPort.hpp>
#include<stdio.h>
#include<string.h>
#include<unistd.h> 
#include<iostream>
#include<string>
#include<cstring>
#include<sstream>
#include<time.h>

using namespace std;
using namespace mn::CppLinuxSerial;

/*int main() {
	// Create serial port object and open serial port
	SerialPort serialPort("/dev/ttyUSB0", BaudRate::B_115200);
	// Use SerialPort serialPort("/dev/ttyACM0", 13000); instead if you want to provide a custom baud rate
	serialPort.SetTimeout(0); // Block when reading until any data is received//-1 blocks 0 does not 
	serialPort.Open();

	// Write some ASCII data
	serialPort.Write("*IDN?\n");
	

	// Read some data back (will block until at least 1 byte is received due to the SetTimeout(-1) call above)
	std::string readData;
	while (true){ 
		serialPort.Read(readData);
		cout << readData;
		if (readData == "\n"){
			break;
	}
}
	serialPort.Write("POS?\n");
	cout << "sending POS?" << endl;
	time_t starttime;
	time_t currenttime;
	while (true){
	while (true){ 
		time(&currenttime);
		serialPort.Read(readData);
		cout << readData;
		if (readData == ("\n") ){
			time(&starttime);
			break;
		}
		if (currenttime - starttime > 2){
			break;
		}
	}
	if (currenttime - starttime > 2){
			break;
		}
}
	//serialPort.Write("POS?");
	//serialPort.Read(readData);
	//cout << readData << endl;
	

	// Close the serial port
	serialPort.Close();
}*/
