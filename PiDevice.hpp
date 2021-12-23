//Jacob Mader, 12/13/2021
//edited 12/19/21
//edited 12/20/21
//Developed for the purpose of supporting operation of PI Devices via C++ in environments where the typical PI DLLs are not available
//Linux/ARM devices especially will require access to this source code as the uncompiled source code is unavailable at this time
#ifndef PIDEVICE_HPP
#define PIDEVICE_HPP




#include<stdio.h>
#include<string.h>	
#include<sys/socket.h>
#include<arpa/inet.h>	
#include<netdb.h> 
#include<unistd.h>
#include<iostream>
#include<string>
#include<cstring>
#include<sstream>
#include<cmath>
#include<vector>
#include<thread>
#include<time.h>
#include "SerialPort.hpp" 

//using namespace std; bad form for header to include namespace usage

class PIDevice{
		
		public:
		//Class Attributes
			//Device Attributes
			//vector<vector<char>> Attributes;
			//vector<vector<char>> Methods;
			std::string DeviceType;
			std::string ConnectionStatus;
			std::string ConnectionType;
			std::string LastCommand;
			/*vector<char> Axes;
			vector<char> CurrentPosition;
			vector<char> MaxPosition;
			vector<char> MinPosition;
			vector<char> CurrentVelocity;
			vector<char> MaxVelocity;
			vector<char> MinVelocity;
			vector<char> CurrentAcceleration;
			vector<char> MaxAcceleration;
			vector<char> MinAcceleration;
			vector<char> CurrentDeceleration;
			vector<char> MaxDeceleration;
			vector<char> MinDeceleration;
			vector<char> IsMoving;
			vector<char> IsMacroRunning;
			vector<char> TravelRange;
			vector<char> LoadCapacity;
			vector<char> Mass;
			vector<char> Load;
			vector<char> LengthUnit;
			vector<char> MinimumIncrementalMotion;*/
			
			//Connectivity Attributes
			
			//TCP Connection Attributes
			int socket_desc;
			struct sockaddr_in server;
			char *message , server_reply[10000];
			
			//Serial Connection Attributes
			
			mn::CppLinuxSerial::SerialPort serialPort;
			
		//Class Methods
		
			//User Methods
			int WaitOnTarget();
			
			//TCP Connection Methods
			int connectTCP();
			int sendTCP(std::string GCSCommand);
			int sendReadTCP(std::string GCSCommand);
			int closeTCP();
			
			//Serial Connection Methods
			int connectSerial(const std::string &device); 
			int sendSerial(std::string GCSCommand, int echo = 0);
			std::string sendReadSerial(std::string GCSCommand); 
			std::string read();
			int closeSerial(); 
			
			//GCS Methods
			double qPOS(std::string axes); 
			int MOV(std::string axes, double* targets); 
			
			/*
			//Connection Methods todo
			//void connectSPI();
			//void connectPCI();
			//todo
			
			//GCS Methods
			
			//Motion Methods
			void MOV();
			void MVR();
			
			//Setup Methods
			void SVO();
			void FRF();
			
			//Macro Methods
			//todo
			
			//Query Methods
			void TMX();
			void TMN();
			void POS();
			void SPA();
			void IDN();
			
			//Support Methods
			void HLP();
			void HPA();
			void MAN();*/
		
};

#endif //PIDEVICE_HPP
