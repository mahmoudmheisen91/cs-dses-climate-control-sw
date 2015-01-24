// ccslib.c:
// Climate Control Software Library: C File:

// Including Libraries:
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include "ccslib.h"

// Software Initialization:
// input parameter:
//       void : nothing
// return type:
//       int  : return SUCCESS, init Software
int init(void) {

	int level;
	double actual, desired;
	char sensorTempData[5];

	initLogging("temp_log");
	initCSVFile("temp_csv");
	initConfigFile("temp_config");

	logging(INFO, "\nStarting CliConWARE Software....");

	// Printing current hour:
	printf("    -- Current hour        : %d:%02d\n", getHour(), getMin());

	// Printing actual temperature:
	readData("/dev/temp_sensor", sensorTempData);
	actual = (double) atof(sensorTempData)/ 1000;
	printf("    -- Actual temperature  : %.2f°C\n", actual);

	// Printing desired temperature:
	desired = getDesired("temp_config");
	printf("    -- Desired temperature : %.2f°C\n", desired);

	// Printing current knob level:
	level = map(actual);
	printf("    -- Current knob level  : %d\n", level);

	logging(INFO, "The Software will start in 3 seconds.....\n");
	sleep(3);

	return SUCCESS;
}

// Logging Function Initialization:
// input parameter:
//       char fileName[]: string representation of file dir and name
// return type:
//       int            : return SUCCESS, log file has been initialized
int initLogging(char fileName[]) {

	char sysTime[25];

	time_t timer;
	time(&timer);

	CurrentTime* tm_info;
	tm_info = localtime(&timer);
	strftime(sysTime, 25, "%Y:%m:%d %H:%M:%S", tm_info);

	logFile = fopen(fileName, "a");
	fprintf(logFile, "\n##################################################\n");
	fprintf(logFile, "#################### [NEW RUN] ###################\n");
	fprintf(logFile, "############### %s ##############\n", sysTime);
	fprintf(logFile, "##################################################\n\n");
	fclose(logFile);

    return SUCCESS;
}

// CSV File Initialization:
// input parameter:
//       char fileName[] : string representation of file dir and name
// return type:
//       int             : return SUCCESS, insure their is csv file
int initCSVFile(char fileName[]) {
	// Check if the file exists:
	FILE* CSVFile = openFile(fileName, "w");
	fclose(CSVFile);

	return SUCCESS;
}

// Config File Initialization:
// input parameter:
//       char fileName[] : string representation of file dir and name
// return type:
//       int             : return SUCCESS, config file has been initialized
int initConfigFile(char fileName[]) {
    // Check if the file exists:
    if (access(fileName, F_OK) == -1 ) {
        FILE* config = openFile(fileName, "a");

        // write pre data:
        int i;
        double temp = 22.5;
        for (i = 0; i < 24; i++) {
            fprintf(config, "%d\t\t%.2f\n", i, temp);
            temp = (temp == 22.5) ? 23.5 : 22.5;
        }
        fclose(config);
    }

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
		logging(WARN, "Creating new file.....");
		FILE* file = fopen(fileName, "a");
		fclose(file);
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

	FILE* file = openFile(fileName, "w");
    fprintf(file, "%c", *data);

    if (data[1] >= '0')
        fprintf(file, "%c\n", *(data+1));
    fclose(file);

    return SUCCESS;
}

// Write data:
// input parameter:
//       char fileName[]: string representation of file dir and name
//       double data    : data to be written in the file
// return type:
//       int            : return SUCCESS, file contain actual temp
int writeCSV(char fileName[], double data) {

	char dataStr[15];
	sprintf(dataStr, "%f", data);

	FILE* CSVFile = openFile(fileName, "w");
	fputs(dataStr, CSVFile);
	fclose(CSVFile);

	return SUCCESS;
}

// isEvent:
// input parameter:
//       double *prev : prev value
//       double  now  : actual value
// return type:
//       bool         : return true if an event happend
bool isEvent(double *prev, double now) {
	bool event = (now != *prev) ? true : false;
	*prev = now;
	return event;
}

// Map temp to levels:
// input parameter:
//       double temp : temp in degree
// return type:
//       int         : return level between 0-99
int map(double temp) {

	// Transfer temp to int for safe comparing:
	temp = (int) (temp * 1000);
	if (temp > 32800 || temp < 13000) {
		logging(WARN, "WARNNING:   Temprature is out of range!!");
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
//       int            : return command (set level) to knob
int PIDcontroller(double desired, double actual) {

    double error, proportional, level;
    static double integral = 0.0;
    static double derivative = 0.0;
    static double preError = 0.0;

    // Calculate P,I,D:
    error = desired - actual;
    proportional = error;
    integral += error;
    derivative = error - preError;

    // Command:
    level = Kp * proportional + Ki * integral + Kd * derivative;

    // Update:
    preError = error;

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
    else
    	level = (int) level;

    return level;
}

// Get Local time:
// input parameter:
//       void : nothing
// return type:
//       int  : return integer between 0 and 23 indicating current hour
int getHour(void) {
    time_t mytime;
    time(&mytime);

    CurrentTime now = *(localtime(&mytime));
    int hour = now.tm_hour;

    return hour;
}

// Get Local time:
// input parameter:
//       void : nothing
// return type:
//       int  : return integer between 0 and 59 indicating current min
int getMin(void) {
    time_t mytime;
    time(&mytime);

    CurrentTime now = *(localtime(&mytime));
    int min = now.tm_min;

    return min;
}

// Get desired temp:
// input parameter:
//       char fileName[] : string representation of file dir and name
// return type:
//       float           : return desired temp from config file:
float getDesired(char fileName[]) {

	float desired;
	int config_hour;
	int current_hour = getHour();
	FILE* configFile = openFile(fileName, "r");

	while (fscanf(configFile, "%d %f", &config_hour, &desired) != EOF) {
	    if (config_hour == current_hour)
	        break;
	}

	fclose(configFile);

	return desired;
}

void send_to(int socket, char buffer[256]) {
	write(socket,buffer,strlen(buffer));
	printf("Send: %s\n", buffer);
}

double receive_from(int socket) {
	char buffer[256] = {0};
	read(socket, buffer, 255);
	printf("Recieved: %s\n",buffer);

	double actual = (double) atof(buffer);
	return actual;
}

