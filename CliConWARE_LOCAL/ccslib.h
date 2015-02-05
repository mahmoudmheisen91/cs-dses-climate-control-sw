// ccslib.h:
// Climate Control Software Library: Header File:

#ifndef CCSLIB_H_
#define CCSLIB_H_

    // Define Constants:
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
	typedef struct tm CurrentTime;

	// Define Macro Functions:
	// input parameter:
	//       type : type of logging: WARN, ERROR, INFO
	//       msg  : message to be printed in log file and/or screen
	#define logging(type, msg) \
		if ((strcmp(type, WARN) == 0) || (strcmp(type, ERROR) == 0)) { \
			logFile = fopen("temp_log", "a");\
			fprintf(logFile, "[%s]\t\t[%s]\t\t[%d]\t\t[%s]\t\t\t%s\n", \
                          	  type, __FILE__, __LINE__, __func__, msg); \
            fclose(logFile); \
		} \
		if ((strcmp(type, INFO) == 0) || (strcmp(type, ERROR) == 0)) \
        	printf("%s\n", msg); \
        if (strcmp(type, ERROR) == 0) \
        	FAIL;

	// Global variables:
	FILE *logFile;

	// Functions Prototypes:
	int init(void);
	int initLogging(char fileName[]);
	int initCSVFile(char fileName[]);
	int initConfigFile(char fileName[]);
	FILE* openFile(char fileName[], char* mode);
	int readData(char fileName[], char* data);
	int writeData(char fileName[], char* data);
	int writeCSV(char fileName[], double data);
	bool isEvent(double *prev, double now);
	int map(double temp);
	int PIDcontroller(double desired, double actual);
	int getHour(void);
	int getMin(void);
	float getDesired(char fileName[]);

#endif // CCSLIB_H_
