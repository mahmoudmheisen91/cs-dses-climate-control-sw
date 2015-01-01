// ccslib.h:
// Climate Control Software Library: Header File:

#ifndef CCSLIB_H_
#define CCSLIB_H_

	// Define:
	#define FAIL exit(1)
	#define SUCCESS 0
	#define Kp 1
	#define Ki 0
	#define Kd 0

	// Global variables:
	FILE *sensorFile;
	FILE *knobFile;

	//double dt = 0.1;   // 100ms loop time

	// Functions prototypes:
	int init(void);
	FILE* openFile(char fileName[], char* mode);
	int readData(char fileName[], char* data);
	int writeData(char fileName[], char* data);
	double map(double temp);
	double PIDcontroller(double desired, double actual);

#endif // CCSLIB_H_


