// ccslib.h:
// Climate Control Software Library: Header File:

// TODO: linux app
// TODO: low level i/o

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
	#define true 		1
	#define false 		0

	// type definition:
	typedef int bool;

	// TODO: comments
	// TODO: pragma
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

	// TODO: comments:
	// TODO: correct here (0.1)
	#define isEvent(old, new) \
		if (firstLoop) { \
			old = new + 0.01; \
			firstLoop = false; \
		} \
		if (new == old) \
			continue;

	// Global variables:
	FILE *logFile;

	// TODO
	int extern_argc;

	// Functions Prototypes:
	int init(void);
	int initLogging(char fileName[]);
	int initCSVFile(void);
	int initConfigFile(void);
	FILE* openFile(char fileName[], char* mode);
	int readData(char fileName[], char* data);
	int writeData(char fileName[], char* data);
	int map(double temp);
	double PIDcontroller(double desired, double actual);
	int getTime(void);
	int realController(double desired);

#endif // CCSLIB_H_
