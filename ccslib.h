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
		if ((strcmp(type, INFO) == 0) || (strcmp(type, WARN) == 0) \
				|| (strcmp(type, ERROR) == 0)) \
        	printf("%s\n", msg); \
        if (strcmp(type, ERROR) == 0) \
        	FAIL;

	// Global variables:
	FILE *logFile;
	FILE *sensorFile;
	FILE *knobFile;
	int extern_argc;

	// Functions Prototypes:
	int init(void);
	int initLogging(void);
	int initCSVFile(void);
	int initConfigFile(void);
	FILE* openFile(char fileName[], char* mode);
	int readData(char fileName[], char* data);
	int writeData(char fileName[], char* data);
	double map(double temp);
	double PIDcontroller(double desired, double actual);
	int localController(double desired);
	int wiredController(double desired);
	int wirelessController(double desired);
	int realController(double desired);
	int sensorDriver(void);
	int knobDriver(void);

#endif // CCSLIB_H_


