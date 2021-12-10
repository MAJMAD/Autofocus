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
#include<opencv2/opencv.hpp>
#include<cmath>
#include<vector>
#include<thread>

using namespace std;
using namespace cv;

/*stringstream stream;
* strm << int_val << ' ' << 'whatever you want man';
* string s = strm.str();*/

class Photo{
	public:
		short int position;
		float contrast;
		string file;
		Mat image;
		
		void setContrast(float value){
			contrast = value;
		}
};



float FindContrast(Mat image, short int step1, short int step2){
	// Alternating 5x5 grid tiled kernel, Regions 1, 3, 5, 7, 9, 11, 13, 15, 17, 19, 21, 23, 25
	float summation = 0;
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
	//cout << "Contrast of " << L << endl;
	return summation;
}

void ProcessContrast(Photo &pic, short int step1, short int step2){
	pic.setContrast(FindContrast(pic.image, step1, step2));
	//cout << "picture contrast of " << pic.contrast << endl;
}

//Data Analysis Start
/*void LoadDataSet(string path, int numImages, int spacing, vector<Photo> &Photos, short int step1, short int step2){
	for(int i = 0; i < numImages*spacing; i+= spacing){
		Photo pic;
		stringstream stream;
		stream << path << '/' << to_string(i) << ".jpeg";
		string file = stream.str();
		pic.file = file;
		pic.position = i;
		pic.image = imread(file);
		//pic.contrast = FindContrast(pic.image, step1, step2);
		Photos.push_back(pic);
		//cout << "File: " << file << endl;
		//cout << "Loaded image " << i << ".jpeg" << endl;
	}
	}*/
	
short int LoadDataSetCoarseStage (string path, short int numImages, short int imagespacing, short int dataspacing, vector<Photo> &Photos){
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
		cout << "File: " << file << endl;
		cout << "Loaded image " << i << ".jpeg" << endl;
		loaded++;
	}
	return loaded;
	}
	
short int LoadDataSetFineStage (string path, short int numImages, short int imagespacing, short int dataspacing, short int start, vector<Photo> &Photos){
	short int loaded = 0;
	short int lbound = start - dataspacing;
	short int rbound = start + dataspacing;
	if (lbound < 0){lbound = 0;}
	if (rbound > numImages*imagespacing){rbound = numImages*imagespacing;}
	//cout << "Left bound " << lbound << endl;
	//cout << "Right bound " << rbound << endl;
	for(short int i = lbound; i <= rbound; i+= dataspacing){
		Photo pic;
		stringstream stream;
		stream << path << '/' << to_string(i) << ".jpeg";
		string file = stream.str();
		pic.file = file;
		pic.position = i;
		pic.image = imread(file, IMREAD_GRAYSCALE);
		Photos.push_back(pic);
		//cout << "File: " << file << endl;
		//cout << "Loaded image " << i << ".jpeg" << endl;
		loaded++;
	}
	return loaded;
	}
	
auto RunCoarseStageData(short int numImages, short int imagespacing, short int dataspacing, short int step1, short int step2){
	
	//Create internal storage
	vector<Photo> Photos;
	vector<thread> Threads;
	vector<float> Contrasts;
	
	//Load dataset
	short int numLoaded = LoadDataSetCoarseStage("/home/pi/Desktop/C++/Autofocus Data Sets/100 Image Sets/2021-09-28-15-56-57", numImages, imagespacing, dataspacing, Photos);
	//short int numLoaded = LoadDataSetCoarseStage("/home/pi/Desktop/C++/Autofocus Data Sets/500 Image Sets/Pipeline2021-09-30-14-33-23", numImages, imagespacing, dataspacing, Photos);
	cout << "Coarse Stage Data Successfully Loaded" << endl;
	cout << "Loaded " << numLoaded << " Images" << endl;

	//Process dataset
	for(short int i = 0; i < numLoaded; i++){
		Threads.push_back(thread(ProcessContrast, ref(Photos[i]), step1, step2));
	}
	for(short int i = 0; i < numLoaded; i++){
		Threads[i].join();
	}
	for(short int i = 0; i < numLoaded; i++){
		Contrasts.push_back(Photos[i].contrast);
		cout << "Photos[" << i << "]: " << Photos[i].contrast << endl;
	}
	
	//Gather data
	float MaxContrast = *max_element(Contrasts.begin(), Contrasts.end());
	//cout << MaxContrast << endl;
	auto it = find(Contrasts.begin(), Contrasts.end(), MaxContrast);
	auto MaxFocus = distance(Contrasts.begin(), it) * dataspacing;
	//cout << MaxFocus << endl;
	return MaxFocus;
}

auto RunFineStageData(short int numImages, short int imagespacing, short int dataspacing, short int start, short int step1, short int step2){
	
	//Create internal storage
	vector<Photo> Photos;
	vector<thread> Threads;
	vector<float> Contrasts;
	
	//Load dataset
	short int numLoaded = LoadDataSetFineStage("/home/pi/Desktop/C++/Autofocus Data Sets/100 Image Sets/2021-09-28-15-56-57", numImages, imagespacing, dataspacing, start, Photos);
	//short int numLoaded = LoadDataSetFineStage("/home/pi/Desktop/C++/Autofocus Data Sets/500 Image Sets/Pipeline2021-09-30-14-33-23", numImages, imagespacing, dataspacing, start, Photos);
	//cout << "Fine Stage Data Successfully Loaded" << endl;

	//Process dataset
	
	for(short int i = 0; i < numLoaded; i++){
		Threads.push_back(thread(ProcessContrast, ref(Photos[i]), step1, step2));
		//cout << "seg faulting here?" << endl;
	}
	for(short int i = 0; i < numLoaded; i++){
		Threads[i].join();
	}
	for(short int i = 0; i < numLoaded; i++){
		Contrasts.push_back(Photos[i].contrast);
		//cout << "Photos[" << i << "]: " << Photos[i].contrast << endl;
	}
	
	//Gather data
	float MaxContrast = *max_element(Contrasts.begin(), Contrasts.end());
	//cout << MaxContrast << endl;
	auto it = find(Contrasts.begin(), Contrasts.end(), MaxContrast);
	auto MaxFocus = distance(Contrasts.begin(), it) * dataspacing + start;
	//cout << MaxFocus << endl;
	return MaxFocus;
}

/*void RunDataAnalysis(int numImages, int spacing, int step1, int step2, int throttle){
	
	//Create internal storage
	vector<Photo> Photos;
	vector<thread> Threads;
	vector<long> Contrasts;
	
	//Load dataset
	//LoadDataSet("/home/pi/Desktop/C++/Autofocus Data Sets/100 Image Sets/2021-09-28-15-56-57", numImages, spacing, Photos, step1, step2);
	LoadDataSet("/home/pi/Desktop/C++/Autofocus Data Sets/500 Image Sets/Pipeline2021-09-30-14-33-23", numImages, spacing, Photos, step1, step2);

	//Process dataset
	for(int j = 0; j < throttle; j++){
		for(int i = (numImages/throttle)*j; i < (numImages/throttle)*(j+1); i++){
			Threads.push_back(thread(ProcessContrast, ref(Photos[i]), step1, step2));
		}
		for(int i = (numImages/throttle)*j; i < (numImages/throttle)*(j+1); i++){
			Threads[i].join();
		}
	}
	for(int i = 0; i < numImages; i++){
		Contrasts.push_back(Photos[i].contrast);
		cout << "Photos[" << i << "]: " << Photos[i].contrast << endl;
	}
	
	///Gather data
	double MaxContrast = *max_element(Contrasts.begin(), Contrasts.end());
	cout << MaxContrast << endl;
	auto it = find(Contrasts.begin(), Contrasts.end(), MaxContrast);
	auto MaxFocus = distance(Contrasts.begin(), it);
	
	//Present data
	cout << "Max Contrast of " << MaxContrast << "at position " << MaxFocus*spacing;
}*/

void RunAutoFocus(short int numImages, short int imagespacing, short int dataspacing1, short int dataspacing2, short int step1, short int step2){
	short int ipos = RunCoarseStageData(numImages, imagespacing, dataspacing1, step1, step2);
	short int fpos = RunFineStageData(dataspacing1, imagespacing, dataspacing2, ipos, step1, step2);
	cout << "Max Focus at position " << fpos << endl;
}


//Data Analysis End













int main(int argc, char** argv)
{
	RunAutoFocus(100, 5, 50, 5, 2, 2);
	//RunDataAnalysis(500, 1, 2, 2, 2);
 return 0;
}
