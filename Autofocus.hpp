//Jacob Mader, 12/19/2021
//Developed for the purpose of reducing processing time of the Autofocus algorithm which had originally been developed in Python
#ifndef AUTOFOCUS_HPP
#define AUTOFOCUS_HPP


//need to review whether to include these in headers
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
#include<opencv2/opencv.hpp>
#include<cmath>
#include<vector>
#include<thread>
#include "PiDevice.hpp"


class Photo{
	public:
		short int position;
		double contrast;
		std::string file;
		cv::Mat image;
		void setContrast(double value){
			contrast = value;
		}
};

double FindContrast(cv::Mat image, short int step1, short int step2);

void ProcessContrast(Photo &pic, short int step1, short int step2);

short int LoadDataSetCoarseStage (std::string path, short int numImages, short int imagespacing, short int dataspacing, std::vector<Photo> &Photos);

short int LoadDataSetFineStage (std::string path, short int numImages, short int imagespacing, short int dataspacing1, short int dataspacing2, short int start, std::vector<Photo> &Photos, short int &lbound);

short int ImageCapture(PIDevice pidevice, cv::VideoCapture cap, short int imagespacing, std::vector<Photo> &Photos, double startpos, double* endpos, std::string axes);

double RunCoarseStageData(PIDevice pidevice, cv::VideoCapture cap, short int numImages, short int imagespacing, short int dataspacing, short int step1, short int step2);

auto RunFineStageData(short int numImages, short int imagespacing, short int dataspacing1, short int dataspacing2, short int start, short int step1, short int step2);

void RunAutoFocus(short int numImages, short int imagespacing, short int dataspacing1, short int dataspacing2, short int step1, short int step2);




#endif
