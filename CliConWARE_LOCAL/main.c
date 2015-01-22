// Climate Control Software Client Code:

// Including libraries:
#include <stdio.h>
#include <stdlib.h>
#include "ccslib.h"

// Global Variables:
int level;
double hour;
double mins;
double prevHour;
double prevMins;
double desired;
double actual;
double prevTemp;
char sensorTempData[5];
char levelStr[15];

// main function:
int main(int argc, char **argv) {

	// Software Initialization:
	init();

	// Read desired temp from Config file:
	desired = getDesired("temp_config");

	// Infinite Loop:
	while (1) {
		// Sense an event change in current min:
        mins = getMin();
		if (isEvent(&prevMins, mins))
			desired = getDesired("temp_config");

		// Read actual temp from sensor, transfer it to double:
		readData("/dev/temp_sensor", sensorTempData);
		actual = (double) atof(sensorTempData) / 1000;

		// Sense an event change in sensor:
		if (!isEvent(&prevTemp, actual))
			continue;

		// Calculate knob level:
		level = PIDcontroller(desired, actual);

		// Write level to knob after transfering it to string:
		sprintf(levelStr, "%d", level);
		writeData("/dev/temp_knob", levelStr);

		// Print to screen:
		printf("Sensor Reading = %.2f°C  ==>  Knob Level = %d\n",
				actual, level);

		// Write data to CSV file:
		writeCSV("temp_csv", actual);
	}

	return SUCCESS;
}
