//Jacob Mader, 12/13/2021
//Developed for the purpose of supporting operation of PI Devices via C++ in environments where the typical PI DLLs are not available
//Linux/ARM devices especially will require access to this source code as the uncompiled source code is unavailable at this time
#include<stdio.h>
#include<string.h>	//strlen
#include<sys/socket.h>
#include<arpa/inet.h>	//inet_addr
#include<netdb.h> // hostent
#include<unistd.h> // close()
#include<iostream> //cin
#include<string>
#include<cstring>
#include<sstream>
#include<cmath>
#include<vector>
#include<thread>
#include "PiDevice.hpp"

using namespace std;
using namespace mn::CppLinuxSerial;



//TCP Methods
int PIDevice::connectTCP(){
			//Create socket
			socket_desc = socket(AF_INET , SOCK_STREAM , 0);
			if (socket_desc == -1)
			{
				printf("Could not create socket");
			}
			server.sin_addr.s_addr = inet_addr("169.254.7.154");
			server.sin_family = AF_INET;
			server.sin_port = htons( 50000 );
			//Connect to remote server
			if (connect(socket_desc , (struct sockaddr *)&server , sizeof(server)) < 0)
			{
				puts("connect error");
				return 1;
			}
			puts("Connected\n");
			ConnectionStatus = "Connected";
			ConnectionType = "TCPIP";
			return 0;
}
			
int PIDevice::sendTCP(string GCSCommand){
			stringstream strm;
			strm << GCSCommand;
			//strm << "\n";
			string s = strm.str();
			char* command = new char[s.length() + 1];
			strcpy(command, s.c_str());
			//Send some data
			message = command;
			cout << message << endl;
			if( send(socket_desc , message , strlen(message) , 0) < 0)
			{
				puts("Send failed");
				return 1;
			}
			puts("Data Sent\n");
			delete [] command;
			LastCommand = GCSCommand;
			return 0;
}
		
int PIDevice::sendReadTCP(string GCSCommand){
			//Send Command
			sendTCP(GCSCommand);
			//Receive a reply from the server
			cout << server_reply << endl;
			if( recv(socket_desc, server_reply , 2000 , 0) < 0)
			{
				puts("recv failed");
				puts(server_reply);
			}
			puts("Reply received\n");
			puts(server_reply);
			return 0;
}

int PIDevice::closeTCP(){
	close(socket_desc);
	ConnectionStatus = "Disconnected";
	ConnectionType = "None";
	return 0;
}
// end TCP Methods



//Serial Methods
int PIDevice::connectSerial(const std::string &device){
	// Create serial port object and open serial port
	serialPort = SerialPort (device, BaudRate::B_115200);
	// Use SerialPort serialPort("/dev/ttyACM0", 13000); instead if you want to provide a custom baud rate
	serialPort.SetTimeout(0); // Block when reading until any data is received//-1 blocks 0 does not 
	serialPort.Open();
	ConnectionStatus = "Connected";
	ConnectionType = "Serial";
	return 0;
}
			
int PIDevice::sendSerial(string GCSCommand, int echo){
	serialPort.Write(GCSCommand);
	if (echo == 0){
		cout << DeviceType << ": " << GCSCommand << '\n';
	}
	LastCommand = GCSCommand;
	return 0;
}

std::string PIDevice::sendReadSerial(string GCSCommand){
	stringstream strm;
	strm << DeviceType << ": ";
	sendSerial(GCSCommand);
	std::string readData;
	int readcount = 0;
	do{ 
		serialPort.Read(readData);
		strm << readData;
		if (readData == ("\n") ){
			//cout << readData;
			break;
		}
		else { readcount++;}
		if (readcount >= 10){
			break;
		}
	}
	while (readData.length() > 0);
	string s = strm.str();
	//cout << s << '\n';
	return s;
}

std::string PIDevice::read(){
	sleep(1);
	stringstream strm;
	strm << DeviceType << ": ";
	std::string readData;
	int readcount =0;
	do{ 
		serialPort.Read(readData);
		strm << readData;
		if (readData == ("*\n") ){
			//cout << readData;
			break;
		}
		else { readcount++;}
		if (readcount >= 10){
			break;
		}
	}
	while (readData.length() > 0);
	string s = strm.str();
	return s;
}

void PIDevice::WaitOnTarget(){
	string response;
	while(true){
		sendSerial("ONT?\n", 0);
		response = read();
		if (response == DeviceType + ": " + "1=1\n"){
			//cout << response;
			break;
		}
	}
}



/*std::string PIDevice::sendReadSerial(string GCSCommand){ multiline responses no block
	cout << GCSCommand << '\n';
	stringstream strm;
	sendSerial(GCSCommand);
	std::string readData;
	time_t starttime;
	time_t currenttime;
	while (true){
	while (true){ 
		time(&currenttime);
		serialPort.Read(readData);
		strm << readData;
		//cout << readData;
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
	string s = strm.str();
	cout << s << '\n';
	return s;
}*/

int PIDevice::closeSerial(){
	serialPort.Close();
	ConnectionStatus = "Disconnected";
	ConnectionType = "None";
	return 0;
	}
// end Serial Methods

// GCS Methods
double PIDevice::qPOS(string axes){
	/*char* axis = new char[axes.length() + 1];
	if (axes == "Z"){
		strcpy(axis, axes.c_str());
	}*/
	cout << axes << endl;
	if (ConnectionType == "TCPIP"){
		sendReadTCP("POS?\n");
	}
	else if (ConnectionType == "Serial"){
		stringstream gcs;
		gcs << "POS? " << axes << "\n";
		string gcsstring = gcs.str();
		cout << gcsstring << endl;
		cout << "found it" << endl;
		string pos = sendReadSerial(gcsstring);
		cout << "right here " << endl;
		cout << gcsstring << endl;
		cout << pos << endl;
		stringstream strm;
		int i = 1;
		while (true){
			cout << "good til here?" << endl;
			if(pos[i] == '=' ){
				cout << "here?" << endl;
				int j = i+1;
				while (pos[j] != '\n'){
					strm << pos[j];
					j++;
				}
				break;
			}
		}
		string posret = strm.str();
		double numpos = stod(posret);
		return numpos;
		}
		return 0;
}

int PIDevice::MOV(string axes, double* targets){
	stringstream strm;
	strm << "MOV ";
	for(int i = 0; i < axes.length(); i+=2){
		strm << axes[i] << " " << targets[i/2];
		if (i < axes.length() - 1){
			strm << " ";
		}
	}
	strm << "\n";
	string s = strm.str();
	
	if (ConnectionType == "TCPIP"){
		sendTCP(s);
	}
	else if (ConnectionType == "Serial"){
		sendSerial(s);
	}
	return 0;
}

//end GCS Methods
		/*
		//Connection Methods
		//void connectTCPIP();
		//void connectRS232();
		//void connectUSB();
		//void connectSPI();
		//void connectPCI();
		//todo
		
		//GCS Methods
		
		//Motion Methods
		int MOV(PIDevice pidevice, string axes, double* targets){
			stringstream strm;
			strm << "MOV ";
			for(int i = 0; i < axes.length(); i+=2){
				strm << axes[i] << " " << targets[i/2];
				if (i < axes.length() - 1){
					strm << " ";
				}
			}
			strm << "\n";
			string s = strm.str();
			char* command = new char[s.length() + 1];
			strcpy(command, s.c_str());
			//Send some data
			pidevice.message = command;
			cout << pidevice.message << endl;
			if( send(pidevice.socket_desc , pidevice.message , strlen(pidevice.message) , 0) < 0)
			{
				puts("Send failed");
				return 1;
			}
			puts("Data Sent\n");
			delete [] command;
			return 0;
			};
			
		int qMOV(PIDevice pidevice, string axes, double *targets){
			stringstream strm;
			strm << "MOV? ";
			for(int i = 0; i < axes.length(); i+=2){
				strm << axes[i];
				if (i < axes.length() - 2){
					strm << " ";
				}
			}
			strm << "\n";
			string s = strm.str();
			char* command = new char[s.length() + 1];
			strcpy(command, s.c_str());
			//Send some data
			pidevice.message = command;
			cout << pidevice.message << endl;
			if( send(pidevice.socket_desc , pidevice.message , strlen(pidevice.message) , 0) < 0)
			{
				puts("Send failed");
				return 1;
			}
			puts("Data Sent\n");
			//Receive a reply from the server
			cout << pidevice.server_reply << endl;
			
			if( recv(pidevice.socket_desc, pidevice.server_reply , 2000 , 0) < 0)
			{
				puts("recv failed");
				puts(pidevice.server_reply);
			}
			puts("Reply received\n");
			puts(pidevice.server_reply);
			delete [] command;
			return 0;
			};
			
		int MVR(PIDevice pidevice, string axes, double* targets){
			stringstream strm;
			strm << "MVR ";
			for(int i = 0; i < axes.length(); i+=2){
				strm << axes[i] << " " << targets[i/2];
				if (i < axes.length() - 1){
					strm << " ";
				}
			}
			strm << "\n";
			string s = strm.str();
			char* command = new char[s.length() + 1];
			strcpy(command, s.c_str());
			//Send some data
			pidevice.message = command;
			cout << pidevice.message << endl;
			if( send(pidevice.socket_desc , pidevice.message , strlen(pidevice.message) , 0) < 0)
			{
				puts("Send failed");
				return 1;
			}
			puts("Data Sent\n");
			delete [] command;
			return 0;
			};
		
		//Setup Methods
		int SVO(PIDevice pidevice, string szAxes, string pbValueArray){
			stringstream strm;
			strm << "SVO ";
			for(int i = 0; i < szAxes.length(); i+=2){
				strm << szAxes[i] << " " << pbValueArray[i];
				if (i < szAxes.length() - 2){
					strm << " ";
				}
			}
			strm << "\n";
			string s = strm.str();
			char* command = new char[s.length() + 1];
			strcpy(command, s.c_str());
			//Send some data
			pidevice.message = command;
			cout << pidevice.message << endl;
			if( send(pidevice.socket_desc , pidevice.message , strlen(pidevice.message) , 0) < 0)
			{
				puts("Send failed");
				return 1;
			}
			puts("Data Sent\n");
			///Receive a reply from the server
			cout << pidevice.server_reply << endl;
			
			if( recv(pidevice.socket_desc, pidevice.server_reply , 2000 , 0) < 0)
			{
				puts("recv failed");
				puts(pidevice.server_reply);
			}
			puts("Reply received\n");
			puts(pidevice.server_reply);*/
			/*delete [] command;
			return 0;
			};
			
		int qSVO(PIDevice pidevice, string szAxes, int *pbValueArray){
			stringstream strm;
			strm << "SVO? ";
			for(int i = 0; i < szAxes.length(); i+=2){
				strm << szAxes[i];
				if (i < szAxes.length() - 2){
					strm << " ";
				}
			}
			strm << "\n";
			string s = strm.str();
			char* command = new char[s.length() + 1];
			strcpy(command, s.c_str());
			//Send some data
			pidevice.message = command;
			cout << pidevice.message << endl;
			if( send(pidevice.socket_desc , pidevice.message , strlen(pidevice.message) , 0) < 0)
			{
				puts("Send failed");
				return 1;
			}
			puts("Data Sent\n");
			//Receive a reply from the server
			cout << pidevice.server_reply << endl;
			
			if( recv(pidevice.socket_desc, pidevice.server_reply , 2000 , 0) < 0)
			{
				puts("recv failed");
				puts(pidevice.server_reply);
			}
			puts("Reply received\n");
			puts(pidevice.server_reply);
			delete [] command;
			return 0;
			};
		void FRF();
		
		//Macro Methods
		//todo
		
		//Query Methods
		void TMX();
		void TMN();
		
		
		int qPOS(PIDevice pidevice){
			//Send some data
			pidevice.message = (char*)"POS?\n";
			if( send(pidevice.socket_desc , pidevice.message , strlen(pidevice.message) , 0) < 0)
			{
				puts("Send failed");
				return 1;
			}
			puts("Data Sent\n");
			//Receive a reply from the server
			cout << pidevice.server_reply << endl;
			
			if( recv(pidevice.socket_desc, pidevice.server_reply , 2000 , 0) < 0)
			{
				puts("recv failed");
				puts(pidevice.server_reply);
			}
			puts("Reply received\n");
			puts(pidevice.server_reply);
			return 0;
		};
		
		
		/*void SPA(){
			//Send some data
			pidevice.message = (char*)"SPA\n";
			if( send(pidevice.socket_desc , pidevice.message , strlen(pidevice.message) , 0) < 0)
			{
				puts("Send failed");
				return 1;
			}
			puts("Data Sent\n");
			//Receive a reply from the server
			cout << pidevice.server_reply << endl;
			
			if( recv(pidevice.socket_desc, pidevice.server_reply , 2000 , 0) < 0)
			{
				puts("recv failed");
				puts(pidevice.server_reply);
			}
			puts("Reply received\n");
			puts(pidevice.server_reply);
			return 0;
		};
		todo
		*/
		
		/*void qIDN(){
			//Send some data
			pidevice.message = (char*)"*IDN?\n";
			if( send(pidevice.socket_desc , pidevice.message , strlen(pidevice.message) , 0) < 0)
			{
				puts("Send failed");
				return 1;
			}
			puts("Data Sent\n");
			//Receive a reply from the server
			cout << pidevice.server_reply << endl;
			
			if( recv(pidevice.socket_desc, pidevice.server_reply , 2000 , 0) < 0)
			{
				puts("recv failed");
				puts(pidevice.server_reply);
			}
			puts("Reply received\n");
			puts(pidevice.server_reply);
			return 0;
		};
		
		//Support Methods
		int qHPA(PIDevice pidevice){
			//Send some data
			pidevice.message = (char*)"HPA?\n";
			if( send(pidevice.socket_desc , pidevice.message , strlen(pidevice.message) , 0) < 0)
			{
				puts("Send failed");
				return 1;
			}
			puts("Data Sent\n");
			//Receive a reply from the server
			cout << pidevice.server_reply << endl;
			
			if( recv(pidevice.socket_desc, pidevice.server_reply , 40000 , 0) < 0)
			{
				puts("recv failed");
				puts(pidevice.server_reply);
			}
			puts("Reply received\n");
			puts(pidevice.server_reply);
			return 0;
		};

		int qHLP(PIDevice pidevice){
			//Send some data
			pidevice.message = (char*)"HLP?\n";
			if( send(pidevice.socket_desc , pidevice.message , strlen(pidevice.message) , 0) < 0)
			{
				puts("Send failed");
				return 1;
			}
			puts("Data Sent\n");
			//Receive a reply from the server
			cout << pidevice.server_reply << endl;
			
			if( recv(pidevice.socket_desc, pidevice.server_reply , 40000 , 0) < 0)
			{
				puts("recv failed");
				puts(pidevice.server_reply);
			}
			puts("Reply received\n");
			puts(pidevice.server_reply);
			return 0;
		};
		void MAN();*/
