// ccslib.h:
// Climate Control Software Library: Header File:

#ifndef CCSLIB_H_
#define CCSLIB_H_

	// Define Constants::
	#define TRACE 		"trace"
	#define DEBUG 		"debug"
	#define INFO  		"info"
	#define WARN  		"warn"
	#define ERROR 		"error"
	#define FATAL 		"fatal"
	#define FAIL 		exit(1)
	#define SUCCESS 	0
	#define Kp 			1
	#define Ki 			0
	#define Kd 			0

	// Define Macro Functions:
	#define logging(type, msg) \
		logFile = fopen("log", "a");\
		fprintf(logFile, "[%s]\t\t[%s]\t\t[%d]\t\t[%s]\t\t\t%s\n", \
                          type, __FILE__, __LINE__, __func__, msg); \
		fclose(logFile); \
		if (type == INFO || type == WARN || type == ERROR) \
        	printf("%s\n", msg); \
        if (type == ERROR) \
        	FAIL;

	// Global variables:
	FILE *logFile;
	FILE *sensorFile;
	FILE *knobFile;
	int extern_argc;


	// Functions prototypes:
	int init(void);
	int initLogging(void);
	FILE* openFile(char fileName[], char* mode);
	int readData(char fileName[], char* data);
	int writeData(char fileName[], char* data);
	double map(double temp);
	double PIDcontroller(double desired, double actual);
	int localController(double desired);

#endif // CCSLIB_H_


