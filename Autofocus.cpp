//Jacob Mader, 12/10/2021
//Developed for the purpose of reducing processing time of the Autofocus algorithm which had originally been developed in Python
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

using namespace std;
using namespace cv;

class Photo{
	public:
		short int position;
		double contrast;
		string file;
		Mat image;
		void setContrast(double value){
			contrast = value;
		}
};

double FindContrast(Mat image, short int step1, short int step2){
	// Alternating 5x5 grid tiled kernel, Regions 1, 3, 5, 7, 9, 11, 13, 15, 17, 19, 21, 23, 25
	double summation = 0;
	for(short int row = 1; row < 96; row += step1){ // First Kernel Row
		for(short int column = 1; column < 128; column += step2){ // Region 1
			summation += pow((image.at<uchar>(row, column)) * 4 - (image.at<uchar>(row - 1, column)) - (image.at<uchar>(row + 1, column)) - (image.at<uchar>(row, column - 1)) - (image.at<uchar>(row, column + 1)), 2);
		}
		for(short int column = 255; column < 382; column += step2){ // Region 3
			summation += pow((image.at<uchar>(row, column)) * 4 - (image.at<uchar>(row - 1, column)) - (image.at<uchar>(row + 1, column)) - (image.at<uchar>(row, column - 1)) - (image.at<uchar>(row, column + 1)), 2);
		}
		for(short int column = 509; column < 636; column += step2){ // Region 5
			summation += pow((image.at<uchar>(row, column)) * 4 - (image.at<uchar>(row - 1, column)) - (image.at<uchar>(row + 1, column)) - (image.at<uchar>(row, column - 1)) - (image.at<uchar>(row, column + 1)), 2);
		}
	}
	for(short int row = 96; row < 191; row += step1){ // Second Kernel Row
		for(short int column = 128; column < 255; column += step2){ // Region 7
			summation += pow((image.at<uchar>(row, column)) * 4 - (image.at<uchar>(row - 1, column)) - (image.at<uchar>(row + 1, column)) - (image.at<uchar>(row, column - 1)) - (image.at<uchar>(row, column + 1)), 2);
		}
		for(short int column = 382; column < 509; column += step2){ // Region 9
			summation += pow((image.at<uchar>(row, column)) * 4 - (image.at<uchar>(row - 1, column)) - (image.at<uchar>(row + 1, column)) - (image.at<uchar>(row, column - 1)) - (image.at<uchar>(row, column + 1)), 2);
		}
	}
	for(short int row = 191; row < 286; row += step1){ // Third Kernel Row
		for(short int column = 1; column < 128; column += step2){ // Region 11
			summation += pow((image.at<uchar>(row, column)) * 4 - (image.at<uchar>(row - 1, column)) - (image.at<uchar>(row + 1, column)) - (image.at<uchar>(row, column - 1)) - (image.at<uchar>(row, column + 1)), 2);
		}
		for(short int column = 255; column < 382; column += step2){ // Region 13
			summation += pow((image.at<uchar>(row, column)) * 4 - (image.at<uchar>(row - 1, column)) - (image.at<uchar>(row + 1, column)) - (image.at<uchar>(row, column - 1)) - (image.at<uchar>(row, column + 1)), 2);
		}
		for(short int column = 509; column < 636; column += step2){ // Region 15
			summation += pow((image.at<uchar>(row, column)) * 4 - (image.at<uchar>(row - 1, column)) - (image.at<uchar>(row + 1, column)) - (image.at<uchar>(row, column - 1)) - (image.at<uchar>(row, column + 1)), 2);
		}
	}
	for(short int row = 286; row < 381; row += step1){ // Fourth Kernel Row
		for(short int column = 128; column < 255; column += step2){ // Region 17
			summation += pow((image.at<uchar>(row, column)) * 4 - (image.at<uchar>(row - 1, column)) - (image.at<uchar>(row + 1, column)) - (image.at<uchar>(row, column - 1)) - (image.at<uchar>(row, column + 1)), 2);
		}
		for(short int column = 382; column < 509; column += step2){ // Region 19
			summation += pow((image.at<uchar>(row, column)) * 4 - (image.at<uchar>(row - 1, column)) - (image.at<uchar>(row + 1, column)) - (image.at<uchar>(row, column - 1)) - (image.at<uchar>(row, column + 1)), 2);
		}
	}
	for(short int row = 381; row < 476; row += step1){ // Fifth Kernel Row
		for(short int column = 1; column < 128; column += step2){ // Region 21
			summation += pow((image.at<uchar>(row, column)) * 4 - (image.at<uchar>(row - 1, column)) - (image.at<uchar>(row + 1, column)) - (image.at<uchar>(row, column - 1)) - (image.at<uchar>(row, column + 1)), 2);
		}
		for(short int column = 255; column < 382; column += step2){ // Region 23
			summation += pow((image.at<uchar>(row, column)) * 4 - (image.at<uchar>(row - 1, column)) - (image.at<uchar>(row + 1, column)) - (image.at<uchar>(row, column - 1)) - (image.at<uchar>(row, column + 1)), 2);
		}
		for(short int column = 509; column < 636; column += step2){ // Region 25
			summation += pow((image.at<uchar>(row, column)) * 4 - (image.at<uchar>(row - 1, column)) - (image.at<uchar>(row + 1, column)) - (image.at<uchar>(row, column - 1)) - (image.at<uchar>(row, column + 1)), 2);
		}
	}
	summation = summation / ((126*94*13)/(step1*step2));
	return summation;
}

void ProcessContrast(Photo &pic, short int step1, short int step2){
	pic.setContrast(FindContrast(pic.image, step1, step2));
}
	
/*short int LoadDataSetCoarseStage (string path, short int numImages, short int imagespacing, short int dataspacing, vector<Photo> &Photos){
	short int loaded = 0;
	for(short int i = 0; i <= numImages*imagespacing; i+= dataspacing){
		Photo pic;
		stringstream stream;
		stream << path << '/' << to_string(i) << ".jpeg";
		string file = stream.str();
		pic.file = file;
		pic.position = i;
		pic.image = imread(file, IMREAD_GRAYSCALE);
		Photos.push_back(pic);
		loaded++;
	}
	return loaded;
}
	
short int LoadDataSetFineStage (string path, short int numImages, short int imagespacing, short int dataspacing1, short int dataspacing2, short int start, vector<Photo> &Photos, short int &lbound){
	short int loaded = 0;
	lbound = start - dataspacing1;
	short int rbound = start + dataspacing1;
	if (lbound < 0){lbound = 0;}
	if (rbound > numImages*imagespacing){rbound = numImages*imagespacing;}
	for(short int i = lbound; i <= rbound; i+= dataspacing2){
		Photo pic;
		stringstream stream;
		stream << path << '/' << to_string(i) << ".jpeg";
		string file = stream.str();
		pic.file = file;
		pic.position = i;
		pic.image = imread(file, IMREAD_GRAYSCALE);
		Photos.push_back(pic);
		loaded++;
	}
	return loaded;
}*/

void showImage(Mat image){
	for (;;)
    {
        // show live and wait for a key with timeout long enough to show images
        imshow("Live", image);
        if (waitKey(5) >= 0)
            break;
    }
}

short int ImageCapture(PIDevice &pidevice, VideoCapture &cap, double dataspacing, vector<Photo> &Photos, double startpos, double endpos){
	pidevice.sendSerial("MOV 1 "  +  to_string(startpos) + "\n");
	cout << "MOV 1 "  +  to_string(startpos) + "\n";
	pidevice.WaitOnTarget();
	int index = 0;
	for (double i = startpos; i <= endpos; i += dataspacing){
		cout << i << endl;
		Photo pic;
		for (int i = 0; i < 6; i++){
			cap >> pic.image;
		}
		cvtColor(pic.image, pic.image, cv::COLOR_BGR2GRAY);
		Photos.push_back(pic);
		index++;
		pidevice.sendSerial("MVR 1 " +  to_string(dataspacing) + "\n");
		pidevice.WaitOnTarget();
	}
	return index;
}

short int ImageCaptureOnTheFly(PIDevice &pidevice, VideoCapture &cap, double dataspacing, vector<Photo> &Photos, double startpos, double endpos){
	pidevice.sendSerial("MOV 1 "  +  to_string(startpos) + "\n");
	pidevice.WaitOnTarget();
	int index = 0;
	pidevice.sendSerial("MOV 1 "  +  to_string(endpos) + "\n");
	for (double i = startpos; i <= endpos; i += dataspacing){
		bool captured = false;
		Photo pic;
		string response;
		while (!captured){
			pidevice.sendSerial("POS?\n", 0);
			response = pidevice.read();
			cout << response << endl;
			if (stod(response.substr(9,4)) > i){
				captured = true;
				index++;
				for (int i = 0; i < 6; i++){
					cap >> pic.image;
				}
				cvtColor(pic.image, pic.image, cv::COLOR_BGR2GRAY);
				Photos.push_back(pic);
				break;
			}
		}
	}
	return index;
}

/*short int ImageCaptureOnTheFly(PIDevice pidevice, VideoCapture cap, short int imagespacing, vector<Photo> &Photos, double startpos, double* endpos, string axis){
	cout << "here?" << endl;
	cout << axis << endl;
	pidevice.MOV(axis, endpos);
	cout << "here2?" << endl;
	short int count = 0;
	for (double i = startpos; i <= endpos[0]; i += imagespacing){
		cout << "here3?" << endl;
		bool captured = false;
		while (!captured){
			cout << "here4?" << endl;
			if (i > pidevice.qPOS(axis)){
				cout << "here5?" << endl;
				cap.read(Photos[count].image);
				captured = true;
				count++;
			}
		} 
	}
	return count;
}*/

double RunStage(PIDevice &pidevice, VideoCapture &cap, double dataspacing, double start, double end, short int step1, short int step2){
	//Create internal storage
	vector<Photo> Photos;
	vector<thread> Threads;
	vector<double> Contrasts;
	//Collect dataset
	short int numLoaded = ImageCapture(pidevice, cap, dataspacing, Photos, start, end);
	//Process dataset
	for(short int i = 0; i < numLoaded; i++){
		Threads.push_back(thread(ProcessContrast, ref(Photos[i]), step1, step2));
	}
	for(short int i = 0; i < numLoaded; i++){
		Threads[i].join();
	}
	for(short int i = 0; i < numLoaded; i++){
		Contrasts.push_back(Photos[i].contrast);
	}
	for(short int i = 0; i < numLoaded; i++){
		cout << i*dataspacing + start << ": " << Photos[i].contrast << endl;
	}
	//Gather data
	double MaxContrast = *max_element(Contrasts.begin(), Contrasts.end());
	auto it = find(Contrasts.begin(), Contrasts.end(), MaxContrast);
	double MaxFocus = (double)(distance(Contrasts.begin(), it) * dataspacing + start);
	return MaxFocus;
}

/*auto RunFineStageData(short int numImages, short int imagespacing, short int dataspacing1, short int dataspacing2, short int start, short int step1, short int step2){
	//Create internal storage
	vector<Photo> Photos;
	vector<thread> Threads;
	vector<double> Contrasts;
	short int lbound;
	//Load dataset
	short int numLoaded = LoadDataSetFineStage("/home/pi/Desktop/C++/Autofocus Data Sets/100 Image Sets/2021-09-28-15-56-57", numImages, imagespacing, dataspacing1, dataspacing2, start, Photos, lbound);
	//Process dataset
	for(short int i = 0; i < numLoaded; i++){
		Threads.push_back(thread(ProcessContrast, ref(Photos[i]), step1, step2));
	}
	for(short int i = 0; i < numLoaded; i++){
		Threads[i].join();
	}
	for(short int i = 0; i < numLoaded; i++){
		Contrasts.push_back(Photos[i].contrast);
	}
	//Gather data
	double MaxContrast = *max_element(Contrasts.begin(), Contrasts.end());
	auto it = find(Contrasts.begin(), Contrasts.end(), MaxContrast);
	auto MaxFocus = distance(Contrasts.begin(), it) * dataspacing2 + (double)lbound;
	return MaxFocus;
}*/

double RunAutoFocus(PIDevice &pidevice, VideoCapture &cap, double start, double end, short int step1, short int step2){
	double dataspacing = (end - start) / 10;
	double CoarseMax1 = RunStage(pidevice, cap, dataspacing, start, end, step1, step2); // Outer Coarse Stage
	double CoarseMax2 = RunStage(pidevice, cap, dataspacing, start + dataspacing / 2, end - dataspacing / 2, step1, step2); // Inner Coarse Stage
	double FineStart;
	double FineEnd;
	
	if (CoarseMax1 > CoarseMax2){
		FineEnd = CoarseMax1;
		FineStart = CoarseMax2;
	}
	else{
		FineEnd = CoarseMax2;
		FineStart = CoarseMax1;
	}
	dataspacing = (FineEnd - FineStart) / 5;
	double fpos = RunStage(pidevice, cap, dataspacing, FineStart, FineEnd, step1, step2); //Fine stage*/
	cout << "Max Focus at position " + to_string(fpos) << endl;
	return fpos;
}

/*int main(int argc, char** argv)
{
	RunAutoFocus(100, 5, 50, 5, 2, 2);
    return 0;
}*/
