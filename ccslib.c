// ccslib.c:
// Climate Control Software Library: C File:

// TODO: functionalities
// TODO: BUGS
// TODO: LINUX/KERNAL

// Including Libraries:
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
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

	initLogging("G44/log");
	initCSVFile();
	initConfigFile();

	if (extern_argc != 2) {
		logging(WARN, "Error running software!, Mismatch number of argument!!!");
		logging(WARN, "E.g run as follow: ./climateControlSoftware 22.5");
		logging(WARN, "Terminating program..........");
		logging(ERROR, "Terminating program..........Done");
	}

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

	return SUCCESS;
}

// Logging Function Initialization:
// input parameter:
//       void : nothing
// return type:
//       int  : return SUCCESS
int initLogging(char fileName[]) {

	// TODO: print time and date:
	logFile = openFile(fileName, "a");
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
	//logging(TRACE, "Entering initCSVFile() function....");

	// TODO: functionality

	//logging(TRACE, "Exiting initCSVFile() function....");

	return SUCCESS;
}

// Config File Initialization:
// input parameter:
//       void : nothing
// return type:
//       int  : return SUCCESS
int initConfigFile(void) {
	//logging(TRACE, "Entering initConfigFile() function....");

	// TODO: functionality

	//logging(TRACE, "Exiting initConfigFile() function....");

	return SUCCESS;
}

// Open file:
// input parameter:
//       char fileName[]: string representation of file dir and name
//       char* mode     : string representation or r/w
// return type:
//       FILE*          : pointer to the wanted file
FILE* openFile(char fileName[], char* mode) {

	// Check if the file exists:
	if (access(fileName, F_OK) == -1 ) {
		logging(WARN, "Error opening file!, file name does not exists!!!");
		logging(WARN, "Terminating program..........");
		logging(ERROR, "Terminating program..........Done");
	}

	// Open the file:
    FILE* file = fopen(fileName, mode);

    return file;
}

// Read data:
// input parameter:
//       char fileName[]: string representation of file dir and name
//       char* data     : char array of length 5 to put data in it
// return type:
//       int            : return SUCCESS, data array contain sensor data
int readData(char fileName[], char* data) {

	// Open, read and close:
	FILE* file = openFile(fileName, "r");
    fscanf(file, "%s", data);
    fclose(file);

    return SUCCESS;
}

// Write data:
// input parameter:
//       char fileName[]: string representation of file dir and name
//       char* data     : char array of length 2 containing the data
// return type:
//       int            : return SUCCESS, file contain temp level
int writeData(char fileName[], char* data) {

	// TODO: BUGGY
	// Write maximum of two characters:
	FILE* file = openFile(fileName, "w");
    fprintf(file, "%c", *data);
    if (data[1] >= '0')
        fprintf(file, "%c\n", *(data+1));
    fclose(file);

    return SUCCESS;
}

// Map temp to levels:
// input parameter:
//       double temp : temp in millidegree
// return type:
//       int         : return level between 0-99
int map(double temp) {

	// Transfer temp to int for safe comparing:
	temp = (int) (temp * 1000);
	if (temp > 32800 || temp < 13000) {
		logging(WARN, "Error temp out of range!");
		logging(WARN, "Terminating program..........");
		logging(ERROR, "Terminating program..........Done");
	}

	// Map temp to level between 0-99:
	int level = 99 - ((32800 - temp)/200);

	return level;
}

// PID Controller:
// input parameter:
//       double desired : desired value to reach
//       double actual  : actual value of the sensor
// return type:
//       double         : return command (set level) to knob
double PIDcontroller(double desired, double actual) {

	// TODO: BUGGY if's: less than 15;
    double level = 0.0;
    double proportional;
    static double dt = 0.01;
    static double integral = 0.0;
    double derivative;
    static double preError = 0.0;
    double error = desired - actual;

    // Calculate P,I,D:
    proportional = error;
    integral += error * dt;
    derivative = (error - preError) / dt;

    // Command:
    level = Kp * proportional + Ki * integral + Kd * derivative;

    // Update:
    preError = error;
    dt += 0.01;

    // if diff. between desired and actual is large, set level to max:
    if (error > 1)
    	level = 99;

    // if level more than the max, set level to max:
    if (level > 99)
    	level = 99;

    if (level <= 0 && map(actual) > 0)
       	level = map(actual) - 1;
    else if (level <= 0 && map(actual) <= 0)
        level = map(actual);
    else if (level < map(actual) && map(actual) < 99)
    	level = map(actual) + 1;
    else if (level < map(actual) && map(actual) >= 99)
    	level = map(actual);

    return level;
}

// Get Local time:
// input parameter:
//       void : nothing
// return type:
//       int  : return integer between 0 and 23 indicating current hour
int getTime(void) {
    time_t mytime;
    time(&mytime);

    struct tm *now = localtime(&mytime);
    int hour = now->tm_hour;

    return hour;
}

// Search desired temp:
// input parameter:
//       int   : current hour
// return type:
//       float : return correspondence temp from config file:
float search(int hour) {
    FILE* configFile = openFile("config", "r");

    int config_hour;
    float desired;

    while (fscanf(configFile, "%d %f", &config_hour, &desired) != EOF) {
        if (config_hour == hour)
            break;
    }

    return desired;
}

// Get desired temp:
// input parameter:
//       void   : nothing
// return type:
//       double : return desired temp from config file:
double getDesired(void) {

	int hour = getTime();
    double desired = search(hour);

    return desired;
}

// TASK 4: The Real Controller:
// input parameter:
//       double desired : desired value to reach
// return type:
//       int            : return SUCCESS after waiting one second
int realController(double desired) {
	//logging(TRACE, "Entering realController() function....");

	// TODO: functionality

	//logging(TRACE, "Exiting realController() function....");

	return SUCCESS;
}
