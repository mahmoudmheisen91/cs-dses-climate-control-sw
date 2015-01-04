// ccslib.c:
// Climate Control Software Library: C File:

// TODO: THROW ERRORS
// TODO: Split Library
// TODO: functionalities
// TODO: Messy code
// TODO: BUGS
// TODO: LINUX/KERNAL
// TODO: GLOBAL VAR's
// TODO: Comments

// Including Libraries:
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "ccslib.h"

// Software Initialization:
// input parameter:
//       void : nothing
// return type:
//       int  : return SUCCESS
int init(void) {

	// TODO: MESSY
	int command;
	double actual;
	char sensorTempData[5];

	initLogging();
	initCSVFile();
	initConfigFile();

	if (extern_argc != 2) {
		logging(WARN, "Error running software!, Mismatch number of argument!!!");
		logging(WARN, "E.g run as follow: ./climateControlSoftware 22.5");
		logging(WARN, "Terminating program..........");
		logging(ERROR, "Terminating program..........Done");
	}

	logging(TRACE, "Entering init() function....");
	logging(INFO, "Initializing Climate Control Software....");

	sleep(1);
	logging(INFO, "Initializing Climate Control Software....Done");
	sleep(1);

	logging(INFO, "Checking sensor device file....");
	sleep(1);
	logging(INFO, "Openning /dev/temp_sensor....");
	FILE* file = openFile("/dev/temp_sensor", "r");
	readData("/dev/temp_sensor", sensorTempData);
	actual = (double) atof(sensorTempData)/ 1000;
	sleep(1);
	//logging(INFO, "Initializing Climate Control Software....Done");
	printf("Current sensor data is.... %.2f°C\n", actual);
	sleep(1);
	fclose(file);
	logging(INFO, "Checking sensor device file....Done");
	sleep(1);

	logging(INFO, "Checking knob device file....");
	sleep(1);
	logging(INFO, "Openning /dev/temp_knob....");
	file = openFile("/dev/temp_knob", "r");
	command = map(actual);
	sleep(1);
	//logging(INFO, "Initializing Climate Control Software....Done");
	printf("Current knob level is.... %d\n", command);
	sleep(1);
	fclose(file);
	logging(INFO, "Checking knob device file....Done");
	sleep(1);

	logging(INFO, "The PID Controller will start in 5 seconds.....");
	sleep(5);
	logging(INFO, "The PID Controller will start in 5 seconds.....Done");
	logging(TRACE, "Exiting init() function....");

	return SUCCESS;
}

// Logging Function Initialization:
// input parameter:
//       void : nothing
// return type:
//       int  : return SUCCESS
int initLogging(void) {

	// TODO: print time and date:
	logFile = fopen("log", "a");
	fprintf(logFile, "\n##################################################\n");
	fprintf(logFile, "#################### [NEW RUN] ###################\n");
	fprintf(logFile, "##################################################\n\n");
	fclose(logFile);

    return SUCCESS;
}

// CSV File Initialization:
// input parameter:
//       void : nothing
// return type:
//       int  : return SUCCESS
int initCSVFile(void) {
	logging(TRACE, "Entering initCSVFile() function....");

	// TODO: functionality

	logging(TRACE, "Exiting initCSVFile() function....");

	return SUCCESS;
}

// Config File Initialization:
// input parameter:
//       void : nothing
// return type:
//       int  : return SUCCESS
int initConfigFile(void) {
	logging(TRACE, "Entering initConfigFile() function....");

	// TODO: functionality

	logging(TRACE, "Exiting initConfigFile() function....");

	return SUCCESS;
}

// Open file:
// input parameter:
//       char fileName[]: string representation of file dir and name
//       char* mode     : string representation or r/w
// return type:
//       FILE*          : pointer to the wanted file
FILE* openFile(char fileName[], char* mode) {
	logging(TRACE, "Entering openFile() function....");

	// TODO: R_OK, W_OK
	// Check if the file exists:
	if (access(fileName, F_OK) == -1 ) {
		logging(WARN, "Error opening file!, file name does not exists!!!");
		logging(WARN, "Terminating program..........");
		logging(ERROR, "Terminating program..........Done");
	}

    FILE* file = fopen(fileName, mode);

    logging(TRACE, "Exiting openFile() function....");

    return file;
}

// Read data:
// input parameter:
//       char fileName[]: string representation of file dir and name
//       char* data     : char array of length 5 to put data in it
// return type:
//       int            : return SUCCESS, data array contain sensor data
int readData(char fileName[], char* data) {
	logging(TRACE, "Entering readData() function....");

	FILE* file = openFile(fileName, "r");
    fscanf(file, "%s", data);
    fclose(file);

    logging(TRACE, "Exiting readData() function....");

    return SUCCESS;
}

// Write data:
// input parameter:
//       char fileName[]: string representation of file dir and name
//       char* data     : char array of length 2 containing the data
// return type:
//       int            : return SUCCESS, file contain temp level
int writeData(char fileName[], char* data) {
	logging(TRACE, "Entering writeData() function....");

	// TODO: BUGGY

	FILE* file = openFile(fileName, "w");
    fprintf(file, "%c", *data);
    if (data[1] >= '0')
        fprintf(file, "%c\n", *(data+1));
    fclose(file);

    logging(TRACE, "Exiting writeData() function....");

    return SUCCESS;
}

// Map temp to levels:
// input parameter:
//       double temp : temp in millidegree
// return type:
//       double      : return level between 0-99
double map(double temp) {
	logging(TRACE, "Entering map() function....");

	int val = (int)(temp*1000);
	if (val > 32800 || val < 13000) {
		logging(WARN, "Error temp out of range!");
		logging(WARN, "Terminating program..........");
		logging(ERROR, "Terminating program..........Done");
	}

	double level = 99 - ((32800 - val)/200);

	// TODO: CHECK LEVEL
	logging(TRACE, "Exiting map() function....");

	return level;
}

// PID Controller:
// input parameter:
//       double desired : desired value to reach
//       double actual  : actual value of the sensor
// return type:
//       double         : return command (set level) to knob
double PIDcontroller(double desired, double actual) {
	logging(TRACE, "Entering PIDcontroller() function....");

	// TODO: BUGGY if's
	// TODO: TESTS
    double command = 0;
    double proportional;
    static double dt = 0.01;
    static double integral = 0;
    double derivative;
    static double preError = 0;
    double error = desired - actual;

    // Calculate P,I,D:
    proportional = error;
    integral += error * dt;
    derivative = (error - preError) / dt;

    // Command:
    command = Kp * proportional + Ki * integral + Kd * derivative;

    // Update:
    preError = error;
    dt += 0.01;

    if (error > 1)
    	command = 99;

    if (command <= 0)
    	command = map(actual) - 1;
    else if (command > 99)
    	command = 99;
    else if (command < map(actual))
    	command = map(actual) + 1;

    logging(TRACE, "Exiting PIDcontroller() function....");

    return command;
}

// TASK 1: The Local Controller:
// input parameter:
//       double desired : desired value to reach
// return type:
//       int            : return SUCCESS after waiting one second
int localController(double desired) {

	logging(TRACE, "Entering localController() function....");

	// TODO: functionality

	logging(TRACE, "Exiting localController() function....");

	return SUCCESS;
}

// TASK 2: The Wired Controller:
// input parameter:
//       double desired : desired value to reach
// return type:
//       int            : return SUCCESS after waiting one second
int wiredController(double desired) {
	logging(TRACE, "Entering wiredController() function....");

	// TODO: functionality

	logging(TRACE, "Exiting wiredController() function....");

	return SUCCESS;
}

// TASK 3: The Wireless Controller:
// input parameter:
//       double desired : desired value to reach
// return type:
//       int            : return SUCCESS after waiting one second
int wirelessController(double desired) {
	logging(TRACE, "Entering wirelessController() function....");

	// TODO: functionality

	logging(TRACE, "Exiting wirelessController() function....");

	return SUCCESS;
}

// TASK 4: The Real Controller:
// input parameter:
//       double desired : desired value to reach
// return type:
//       int            : return SUCCESS after waiting one second
int realController(double desired) {
	logging(TRACE, "Entering realController() function....");

	// TODO: functionality

	logging(TRACE, "Exiting realController() function....");

	return SUCCESS;
}

// Sensor Driver:
// input parameter:
//       void : nothing
// return type:
//       int  : return SUCCESS
int sensorDriver(void) {
	logging(TRACE, "Entering sensorDriver() function....");

	// TODO: functionality

	logging(TRACE, "Exiting sensorDriver() function....");

	return SUCCESS;
}

// Knob Driver:
// input parameter:
//       void : nothing
// return type:
//       int  : return SUCCESS
int knobDriver(void) {
	logging(TRACE, "Entering knobDriver() function....");

	// TODO: functionality

	logging(TRACE, "Exiting knobDriver() function....");

	return SUCCESS;
}
