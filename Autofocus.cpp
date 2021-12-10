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
		double contrast;
		string file;
		Mat image;
		
		void setContrast(double value){
			contrast = value;
		}
};

double SimpleFindContrast(Mat image, short int step1, short int step2){
	// Alternating 5x5 grid tiled kernel, Regions 1, 3, 5, 7, 9, 11, 13, 15, 17, 19, 21, 23, 25
	//cout << "do I get here?" << endl;
	double summation = 0;
	//cout << "row 1" << endl;
	for(short int row = 1; row < 480; row += step1){ // First Kernel Row
		for(short int column = 1; column < 640; column += step2){ // Region 1
			summation += pow((image.at<uchar>(row, column)) * 4 - (image.at<uchar>(row - 1, column)) - (image.at<uchar>(row + 1, column)) - (image.at<uchar>(row, column - 1)) - (image.at<uchar>(row, column + 1)), 2);
		}
	}
	summation = summation / ((480*640)/(step1*step2));
	//cout << "Contrast of " << summation << endl;
	return summation;
}

void FindContrastColumn1(Mat image, short int step1, short int step2, short int row, double &retc1){
	double summationc1 = 0;
	for(short int column = 1; column < 128; column += step2){ // Region 1
			summationc1 += pow((image.at<uchar>(row, column)) * 4 - (image.at<uchar>(row - 1, column)) - (image.at<uchar>(row + 1, column)) - (image.at<uchar>(row, column - 1)) - (image.at<uchar>(row, column + 1)), 2);
		}
	retc1 = summationc1;
}

void FindContrastColumn2(Mat image, short int step1, short int step2, short int row, double &retc2){
	double summationc2 = 0;
	for(short int column = 128; column < 255; column += step2){ // Region 7
			summationc2 += pow((image.at<uchar>(row, column)) * 4 - (image.at<uchar>(row - 1, column)) - (image.at<uchar>(row + 1, column)) - (image.at<uchar>(row, column - 1)) - (image.at<uchar>(row, column + 1)), 2);
		}
	retc2 = summationc2;
}

void FindContrastColumn3(Mat image, short int step1, short int step2, short int row, double &retc3){
	double summationc3 = 0;
	for(short int column = 255; column < 382; column += step2){ // Region 3
			summationc3 += pow((image.at<uchar>(row, column)) * 4 - (image.at<uchar>(row - 1, column)) - (image.at<uchar>(row + 1, column)) - (image.at<uchar>(row, column - 1)) - (image.at<uchar>(row, column + 1)), 2);
		}
	retc3 = summationc3;
}

void FindContrastColumn4(Mat image, short int step1, short int step2, short int row, double &retc4){
	double summationc4 = 0;
	for(short int column = 382; column < 509; column += step2){ // Region 9
			summationc4 += pow((image.at<uchar>(row, column)) * 4 - (image.at<uchar>(row - 1, column)) - (image.at<uchar>(row + 1, column)) - (image.at<uchar>(row, column - 1)) - (image.at<uchar>(row, column + 1)), 2);
		}
	retc4 = summationc4;
}

void FindContrastColumn5(Mat image, short int step1, short int step2, short int row, double &retc5){
	double summationc5 = 0;
	for(short int column = 509; column < 636; column += step2){ // Region 5
			summationc5 += pow((image.at<uchar>(row, column)) * 4 - (image.at<uchar>(row - 1, column)) - (image.at<uchar>(row + 1, column)) - (image.at<uchar>(row, column - 1)) - (image.at<uchar>(row, column + 1)), 2);
		}
	retc5 = summationc5;
}

void FindContrastRow1(Mat image, short int step1, short int step2, double &retr1){
	double summationr1 = 0;
	vector<thread> Threadsr1;
	vector<double> PartialSumsr1;
	
	for(short int row = 1; row < 96; row += step1){
		Threadsr1.push_back(thread(FindContrastColumn1, image, step1, step2, row, ref(PartialSumsr1[-3+3*row])));
		Threadsr1.push_back(thread(FindContrastColumn3, image, step1, step2, row, ref(PartialSumsr1[-2+3*row])));
		Threadsr1.push_back(thread(FindContrastColumn5, image, step1, step2, row, ref(PartialSumsr1[-1+3*row])));
	}
	for(short int threads; threads < Threadsr1.size(); threads++){
		Threadsr1[threads].join();
	}
	for(short int sums; sums < PartialSumsr1.size(); sums++){
		summationr1 += PartialSumsr1[sums];
	}
	retr1 = summationr1;
}

void FindContrastRow2(Mat image, short int step1, short int step2, double &retr2){
	double summationr2 = 0;
	vector<thread> Threadsr2;
	vector<double> PartialSumsr2;
	
	for(short int row = 1; row < 96; row += step1){
		Threadsr2.push_back(thread(FindContrastColumn2, image, step1, step2, row, ref(PartialSumsr2[-2+2*row])));
		Threadsr2.push_back(thread(FindContrastColumn4, image, step1, step2, row, ref(PartialSumsr2[-1+2*row])));
	}
	for(short int threads; threads < Threadsr2.size(); threads++){
		Threadsr2[threads].join();
	}
	for(short int sums; sums < PartialSumsr2.size(); sums++){
		summationr2 += PartialSumsr2[sums];
	}
	retr2 = summationr2;
}

void FindContrastRow3(Mat image, short int step1, short int step2, double &retr3){
	double summationr3 = 0;
	vector<thread> Threadsr3;
	vector<double> PartialSumsr3;
	
	for(short int row = 1; row < 96; row += step1){ 
		Threadsr3.push_back(thread(FindContrastColumn1, image, step1, step2, row, ref(PartialSumsr3[-3+3*row])));
		Threadsr3.push_back(thread(FindContrastColumn3, image, step1, step2, row, ref(PartialSumsr3[-2+3*row])));
		Threadsr3.push_back(thread(FindContrastColumn5, image, step1, step2, row, ref(PartialSumsr3[-1+3*row])));
	}
	for(short int threads; threads < Threadsr3.size(); threads++){
		Threadsr3[threads].join();
	}
	for(short int sums; sums < PartialSumsr3.size(); sums++){
		summationr3 += PartialSumsr3[sums];
	}
	retr3 = summationr3;
}

void FindContrastRow4(Mat image, short int step1, short int step2, double &retr4){
	double summationr4 = 0;
	vector<thread> Threadsr4;
	vector<double> PartialSumsr4;
	
	for(short int row = 1; row < 96; row += step1){ 
		Threadsr4.push_back(thread(FindContrastColumn2, image, step1, step2, row, ref(PartialSumsr4[-2+2*row])));
		Threadsr4.push_back(thread(FindContrastColumn4, image, step1, step2, row, ref(PartialSumsr4[-1+2*row])));
	}
	for(short int threads; threads < Threadsr4.size(); threads++){
		Threadsr4[threads].join();
	}
	for(short int sums; sums < PartialSumsr4.size(); sums++){
		summationr4 += PartialSumsr4[sums];
	}
	retr4 = summationr4;
}

void FindContrastRow5(Mat image, short int step1, short int step2, double &retr5){
	double summationr5 = 0;
	vector<thread> Threadsr5;
	vector<double> PartialSumsr5;
	
	for(short int row = 1; row < 96; row += step1){ 
		Threadsr5.push_back(thread(FindContrastColumn1, image, step1, step2, row, ref(PartialSumsr5[-3+3*row])));
		Threadsr5.push_back(thread(FindContrastColumn3, image, step1, step2, row, ref(PartialSumsr5[-2+3*row])));
		Threadsr5.push_back(thread(FindContrastColumn5, image, step1, step2, row, ref(PartialSumsr5[-1+3*row])));
	}
	for(short int threads; threads < Threadsr5.size(); threads++){
		Threadsr5[threads].join();
	}
	for(short int sums; sums < PartialSumsr5.size(); sums++){
		summationr5 += PartialSumsr5[sums];
	}
	retr5 = summationr5;
}

double FindContrastThreaded(Mat image, short int step1, short int step2){
	double summation = 0;
	vector<thread> Threads;
	vector<double> PartialSums;
	Threads.push_back(thread(FindContrastRow1, image, step1, step2, ref(PartialSums[0])));
	Threads.push_back(thread(FindContrastRow2, image, step1, step2, ref(PartialSums[1])));
	Threads.push_back(thread(FindContrastRow3, image, step1, step2, ref(PartialSums[2])));
	Threads.push_back(thread(FindContrastRow4, image, step1, step2, ref(PartialSums[3])));
	Threads.push_back(thread(FindContrastRow5, image, step1, step2, ref(PartialSums[4])));
	for(short int threads; threads < 5; threads++){
		Threads[threads].join();
	}
	for(short int sums; sums <= 5; sums++){
		summation += PartialSums[sums];
	}
	return summation;
}
double FindContrast(Mat image, short int step1, short int step2){
	// Alternating 5x5 grid tiled kernel, Regions 1, 3, 5, 7, 9, 11, 13, 15, 17, 19, 21, 23, 25
	//cout << "do I get here?" << endl;
	double summation = 0;
	//cout << "row 1" << endl;
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
	//cout << "row 2" << endl;
	for(short int row = 96; row < 191; row += step1){ // Second Kernel Row
		for(short int column = 128; column < 255; column += step2){ // Region 7
			summation += pow((image.at<uchar>(row, column)) * 4 - (image.at<uchar>(row - 1, column)) - (image.at<uchar>(row + 1, column)) - (image.at<uchar>(row, column - 1)) - (image.at<uchar>(row, column + 1)), 2);
		}
		for(short int column = 382; column < 509; column += step2){ // Region 9
			summation += pow((image.at<uchar>(row, column)) * 4 - (image.at<uchar>(row - 1, column)) - (image.at<uchar>(row + 1, column)) - (image.at<uchar>(row, column - 1)) - (image.at<uchar>(row, column + 1)), 2);
		}
	}
	//cout << "row 3" << endl;
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
	//cout << "row4" << endl;
	for(short int row = 286; row < 381; row += step1){ // Fourth Kernel Row
		for(short int column = 128; column < 255; column += step2){ // Region 17
			summation += pow((image.at<uchar>(row, column)) * 4 - (image.at<uchar>(row - 1, column)) - (image.at<uchar>(row + 1, column)) - (image.at<uchar>(row, column - 1)) - (image.at<uchar>(row, column + 1)), 2);
		}
		for(short int column = 382; column < 509; column += step2){ // Region 19
			summation += pow((image.at<uchar>(row, column)) * 4 - (image.at<uchar>(row - 1, column)) - (image.at<uchar>(row + 1, column)) - (image.at<uchar>(row, column - 1)) - (image.at<uchar>(row, column + 1)), 2);
		}
	}
	//cout << "row 5" << endl;
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
	//cout << "Contrast of " << summation << endl;
	return summation;
}

void ProcessContrast(Photo &pic, short int step1, short int step2){
	pic.setContrast(FindContrastThreaded(pic.image, step1, step2));
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
		//cout << "File: " << file << endl;
		//cout << "Loaded image " << i << ".jpeg" << endl;
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
	//cout << "Left bound " << lbound << endl;
	//cout << "Right bound " << rbound << endl;
	for(short int i = lbound; i <= rbound; i+= dataspacing2){
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
	vector<double> Contrasts;
	
	//Load dataset
	short int numLoaded = LoadDataSetCoarseStage("/home/pi/Desktop/C++/Autofocus Data Sets/100 Image Sets/2021-09-28-15-56-57", numImages, imagespacing, dataspacing, Photos);
	//short int numLoaded = LoadDataSetCoarseStage("/home/pi/Desktop/C++/Autofocus Data Sets/500 Image Sets/Pipeline2021-09-30-14-33-23", numImages, imagespacing, dataspacing, Photos);
	//cout << "Coarse Stage Data Successfully Loaded" << endl;
	//cout << "Loaded " << numLoaded << " Images" << endl;


	/*ProcessContrast(Photos[0], step1, step2);
	ProcessContrast(Photos[1], step1, step2);
	ProcessContrast(Photos[2], step1, step2);
	ProcessContrast(Photos[3], step1, step2);
	ProcessContrast(Photos[4], step1, step2);
	ProcessContrast(Photos[5], step1, step2);
	ProcessContrast(Photos[6], step1, step2);
	ProcessContrast(Photos[7], step1, step2);
	ProcessContrast(Photos[8], step1, step2);
	ProcessContrast(Photos[9], step1, step2);
	ProcessContrast(Photos[10], step1, step2);*/
	//cout << "begin processing  data" << endl;
	//Process dataset
	for(short int i = 0; i < numLoaded; i++){
		Threads.push_back(thread(ProcessContrast, ref(Photos[i]), step1, step2));
		//cout << "Photos[" << i << "] thread started " << endl;
	}
	for(short int i = 0; i < numLoaded; i++){
		Threads[i].join();
		//cout << "Photos[" << i << "] thread joined " << endl;
	}
	for(short int i = 0; i < numLoaded; i++){
		Contrasts.push_back(Photos[i].contrast);
		//cout << "Photos[" << i << "]: " << Photos[i].contrast << endl;
	}
	
	//Gather data
	double MaxContrast = *max_element(Contrasts.begin(), Contrasts.end());
	//cout << MaxContrast << endl;
	auto it = find(Contrasts.begin(), Contrasts.end(), MaxContrast);
	auto MaxFocus = distance(Contrasts.begin(), it) * dataspacing;
	//cout << MaxFocus << endl;
	return MaxFocus;
}

auto RunFineStageData(short int numImages, short int imagespacing, short int dataspacing1, short int dataspacing2, short int start, short int step1, short int step2){
	
	//Create internal storage
	vector<Photo> Photos;
	vector<thread> Threads;
	vector<double> Contrasts;
	short int lbound;
	
	//Load dataset
	short int numLoaded = LoadDataSetFineStage("/home/pi/Desktop/C++/Autofocus Data Sets/100 Image Sets/2021-09-28-15-56-57", numImages, imagespacing, dataspacing1, dataspacing2, start, Photos, lbound);
	//hort int numLoaded = LoadDataSetFineStage("/home/pi/Desktop/C++/Autofocus Data Sets/500 Image Sets/Pipeline2021-09-30-14-33-23", numImages, imagespacing, dataspacing1, dataspacing2, start, Photos, lbound);
	//cout << "Fine Stage Data Successfully Loaded" << endl;
	//cout << "Loaded " << numLoaded << " Images" << endl;

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
	double MaxContrast = *max_element(Contrasts.begin(), Contrasts.end());
	//cout << MaxContrast << endl;
	auto it = find(Contrasts.begin(), Contrasts.end(), MaxContrast);
	auto MaxFocus = distance(Contrasts.begin(), it) * dataspacing2 + lbound;
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
	short int fpos = RunFineStageData(numImages, imagespacing, dataspacing1, dataspacing2, ipos, step1, step2);
	cout << "Max Focus at position " << fpos << endl;
}


//Data Analysis End













int main(int argc, char** argv)
{
	RunAutoFocus(100, 5, 50, 5, 2, 2);
	//RunAutoFocus(500, 1, 50, 5, 16, 16);
	//RunDataAnalysis(500, 1, 2, 2, 2);
 return 0;
}
