// ccslib.c:
// Climate Control Software Library: C File:

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

	int command;
	double actual;
	char sensorTempData[5];

	printf("\nInitializing Climate Control Software....\n");
	sleep(1);

	printf("Initializing Climate Control Software....Done\n");
	sleep(1);
	printf("Checking sensor device file....\n");
	sleep(1);
	printf("Openning /dev/temp_sensor....\n");
	FILE* file = openFile("/dev/temp_sensor", "r");
	readData("/dev/temp_sensor", sensorTempData);
	actual = (double) atof(sensorTempData)/ 1000;
	sleep(1);
	printf("Current sensor data is.... %.2f°C\n", actual);
	sleep(1);
	fclose(file);
	printf("Checking sensor device file....Done\n");
	sleep(1);

	printf("Checking knob device file....\n");
	sleep(1);
	printf("Openning /dev/temp_knob....\n");
	file = openFile("/dev/temp_knob", "r");
	command = map(actual);
	sleep(1);
	printf("Current knob level is.... %d\n", command);
	sleep(1);
	fclose(file);
	printf("Checking knob device file....Done\n");
	sleep(1);

	printf("The PID Controller will start in 5 seconds.....\n");
	sleep(5);
	printf("The PID Controller will start in 5 seconds.....Done\n\n");

	return SUCCESS;
}

// Open file:
// input parameter:
//       char fileName[]: string representation of file dir and name
//       char* mode     : string representation or r/w
// return type:
//       FILE*          : pointer to the wanted file
FILE* openFile(char fileName[], char* mode) {
    FILE* file = fopen(fileName, "r");
    if (file == NULL) {
        fprintf(stderr, "Error opening file!, ");
        fprintf(stderr, "file name does not exists\n");
        fprintf(stderr, "Terminating program..........\n");
        fprintf(stderr, "Terminating program..........Done\n");
        FAIL;
    }

    fclose(file);
    file = fopen(fileName, mode);

    return file;
}

// Read data:
// input parameter:
//       char fileName[]: string representation of file dir and name
//       char* data     : char array of length 5 to put data in it
// return type:
//       int            : return SUCCESS, data array contain sensor data
int readData(char fileName[], char* data) {
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

// Map temp to levels:
// input parameter:
//       double temp : temp in millidegree
// return type:
//       double      : return level between 0-99
double map(double temp) {
	int val = (int)(temp*1000);
	if (val > 32800 || val < 13000) {
	    fprintf(stderr, "Error temp out of range!\n");
	    fprintf(stderr, "Terminating program..........\n");
	    fprintf(stderr, "Terminating program..........Done\n");
	    FAIL;
	}

	double level = 99 - ((32800 - val)/200);

	return level;
}

// PID Controller:
// input parameter:
//       double desired : desired value to reach
//       double actual  : actual value of the sensor
// return type:
//       double         : return command (set level) to knob
double PIDcontroller(double desired, double actual) {
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

    // Update error:
    preError = error;

    if (error > 1)
    	command = 99;

    if (command <= 0)
    	command = map(actual) - 1;
    else if (command > 99)
    	command = 99;
    else if (command < map(actual))
    	command = map(actual) + 1;

    return command;
}

