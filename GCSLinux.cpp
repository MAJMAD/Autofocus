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

using namespace std;

/*
Convert to string
* 
* stringstream strm;
* strm << int_val << ' ' << 'whatever you want man';
* string s = strm.str();



*/

class Photo{
	public:
		int position;
		double contrast;
		vector<char> file;
		Mat image;
}

class PIController{ //todo
	public:
	
		void SendCommand(char* command){
			char* gcs;//todo
		}
		void MVR(char axis, int distance){
		//todo
		}
}


double FindContrast(Mat image, int step1, int step2){
	// Alternating 5x5 grid tiled kernel, Regions 1, 3, 5, 7, 9, 11, 13, 15, 17, 19, 21, 23, 25
	double summation = 0;
	for(int row = 1; row < 96; row += step1){ // First Kernel Row
		for(int column = 1; column < 128; column += step2){ // Region 1
			summation += pow(((int(image[row, pixel])) * 4 - int(image[row - 1, pixel]) - int(image[row + 1, pixel]) - int(image[row, pixel - 1]) - int(image[row, pixel + 1])), 2);
		}
		for(int column = 255; column < 382; column += step2){ // Region 3
			summation += pow(((int(image[row, pixel])) * 4 - int(image[row - 1, pixel]) - int(image[row + 1, pixel]) - int(image[row, pixel - 1]) - int(image[row, pixel + 1])), 2);
		}
		for(int column = 509; column < 636; column += step2){ // Region 5
			summation += pow(((int(image[row, pixel])) * 4 - int(image[row - 1, pixel]) - int(image[row + 1, pixel]) - int(image[row, pixel - 1]) - int(image[row, pixel + 1])), 2);
		}
	}
	for(int row = 96; row < 191; row += step1){ // Second Kernel Row
		for(int column = 128; column < 255; column += step2){ // Region 7
			summation += pow(((int(image[row, pixel])) * 4 - int(image[row - 1, pixel]) - int(image[row + 1, pixel]) - int(image[row, pixel - 1]) - int(image[row, pixel + 1])), 2);
		}
		for(int column = 382; column < 509; column += step2){ // Region 9
			summation += pow(((int(image[row, pixel])) * 4 - int(image[row - 1, pixel]) - int(image[row + 1, pixel]) - int(image[row, pixel - 1]) - int(image[row, pixel + 1])), 2);
		}
	}
	for(int row = 191; row < 286; row += step1){ // Third Kernel Row
		for(int column = 1; column < 128; column += step2){ // Region 11
			summation += pow(((int(image[row, pixel])) * 4 - int(image[row - 1, pixel]) - int(image[row + 1, pixel]) - int(image[row, pixel - 1]) - int(image[row, pixel + 1])), 2);
		}
		for(int column = 255; column < 382; column += step2){ // Region 13
			summation += pow(((int(image[row, pixel])) * 4 - int(image[row - 1, pixel]) - int(image[row + 1, pixel]) - int(image[row, pixel - 1]) - int(image[row, pixel + 1])), 2);
		}
		for(int column = 509; column < 636; column += step2){ // Region 15
			summation += pow(((int(image[row, pixel])) * 4 - int(image[row - 1, pixel]) - int(image[row + 1, pixel]) - int(image[row, pixel - 1]) - int(image[row, pixel + 1])), 2);
		}
	}
	for(int row = 286; row < 381; row += step1){ // Fourth Kernel Row
		for(int column = 128; column < 255; column += step2){ // Region 17
			summation += pow(((int(image[row, pixel])) * 4 - int(image[row - 1, pixel]) - int(image[row + 1, pixel]) - int(image[row, pixel - 1]) - int(image[row, pixel + 1])), 2);
		}
		for(int column = 382; column < 509; column += step2){ // Region 19
			summation += pow(((int(image[row, pixel])) * 4 - int(image[row - 1, pixel]) - int(image[row + 1, pixel]) - int(image[row, pixel - 1]) - int(image[row, pixel + 1])), 2);
		}
	}
	for(int row = 381; row < 476; row += step1){ // Fifth Kernel Row
		for(int column = 1; column < 128; column += step2){ // Region 21
			summation += pow(((int(image[row, pixel])) * 4 - int(image[row - 1, pixel]) - int(image[row + 1, pixel]) - int(image[row, pixel - 1]) - int(image[row, pixel + 1])), 2);
		}
		for(int column = 255; column < 382; column += step2){ // Region 23
			summation += pow(((int(image[row, pixel])) * 4 - int(image[row - 1, pixel]) - int(image[row + 1, pixel]) - int(image[row, pixel - 1]) - int(image[row, pixel + 1])), 2);
		}
		for(int column = 509; column < 636; column += step2){ // Region 25
			summation += pow(((int(image[row, pixel])) * 4 - int(image[row - 1, pixel]) - int(image[row + 1, pixel]) - int(image[row, pixel - 1]) - int(image[row, pixel + 1])), 2);
		}
	}
	double L = summation / ((126*94*13)/(step1*step2));
	return L;
}

void ProcessContrast(){//todo}

CreateDataFile(){//todo}

void WaitForMotionDone(device, axis){//todo}

void LoadDataSetCoarseStage(vector<char> path, int numpositions, int spacing, vector<Mat> Photos){
	for(int i = 0; i < numpositions + spacing; i+= spacing){
		vector<char> file = path;
		Photo pic;
		//pic.image = //todo
		pic.position = i;
		strm << path << '/' << i << ".jpeg";
		file.push_back('/');
		file.push_back(('0' + i));
		file.push_back('.');
		file.push_back('j');
		file.push_back('p');
		file.push_back('e');
		file.push_back('g');
		pic.file = file
		Photos.push_back(pic);
	}
	}
	
long RunAutofocusCoarseStage(vector<char> dataset, long datasetnumimages, int datasetspacing, vector<char>contrastProc, int step1, int step2){
//todo
}

void LoadDataSetFineStage(vector<char> path, int spacing1, int spacing2, int apos, vector<Mat> Photos){
//todo
	int leftbound = apos - spacing;
	int rightbound = apos + spacing1;
	if (leftbound < 0){
		leftbound = 0;
	}
	if (rightbound > 800){
		rightbound = 800;
	}
	for(int i = leftbound; i < rightbound+1; i+= spacing2){
		vector<char> file = path;
		Photo pic;
		//pic.image = //todo
		pic.position = i;
		strm << path << '/' << i << ".jpeg";
		file.push_back('/');
		file.push_back(('0' + i));
		file.push_back('.');
		file.push_back('j');
		file.push_back('p');
		file.push_back('e');
		file.push_back('g');
		pic.file = file
		Photos.push_back(pic);
}

long RunAutofocusFineStage(vector<char> path, int spacing1, int spacing2, int apos, vector<char>contrastProc, int step1, int step2){
//todo
}

void Move(int spacing, PIController pidevice){
	pidevice.MVR('1', spacing);
	












int main(int argc , char *argv[])
{
	int socket_desc;
	struct sockaddr_in server;
	char *message , server_reply[2000];
	
	//Create socket
	socket_desc = socket(AF_INET , SOCK_STREAM , 0);
	if (socket_desc == -1)
	{
		printf("Could not create socket");
	}
		
	server.sin_addr.s_addr = inet_addr(argv[1]);
	server.sin_family = AF_INET;
	server.sin_port = htons( 50000 );

	//Connect to remote server
	if (connect(socket_desc , (struct sockaddr *)&server , sizeof(server)) < 0)
	{
		puts("connect error");
		return 1;
	}
	
	puts("Connected\n");
	
	//Send some data
	message = (char*)"*IDN?\n";
	std::cout << message << std::endl;
	std::cout << (int*)message << std::endl;
	if( send(socket_desc , message , strlen(message) , 0) < 0)
	{
		puts("Send failed");
		return 1;
	}
	puts("Data Send\n");
	
	//Receive a reply from the server
	if( recv(socket_desc, server_reply , 2000 , 0) < 0)
	{
		puts("recv failed");
		puts(server_reply);
	}
	puts("Reply received\n");
	puts(server_reply);
	
	std::cout << "Enter a command: " << std::endl;
	
	string command;
	while(true){
		getline (cin, command);
		char c[command.size() + 2];
		command.copy(c, command.size() + 2);
		c[command.size()] = '\n';
		c[command.size()+1] = '\0';
		cout << c << '\n';
		message = (char*) c;
		if( send(socket_desc , message , strlen(message) , 0) < 0)
		{
			puts("Send failed");
			return 1;
		}
		puts("Data Send\n");
		
		std::cout << message << std::endl;
		std::cout << (int*)message << std::endl;
		
		//Receive a reply from the server
		if( recv(socket_desc, server_reply , 2000 , 0) < 0)
		{
			puts("recv failed");
			puts(server_reply);
		}
		puts("Reply received\n");
		puts(server_reply);
		}
	char key = 'a';
	while (key != 'q'){
		std::cin >> key;
	}
	close(socket_desc);
	return 0;
}
