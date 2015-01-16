#!/bin/bash

echo '// ccslib.h:
// Climate Control Software Library: Header File:

#ifndef CCSLIB_H_
#define CCSLIB_H_

    #include <signal.h>
    #include <unistd.h>
    #include <sys/mman.h>
    #include <native/task.h>
    #include <native/timer.h>
    #include <rtdk.h>

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
    #define TASK_PRIO   99
    #define TASK_MODE   T_FPU|T_CPU(0)
    #define TASK_STKSZ  4096

    // Types definition:
    typedef int bool;
    typedef struct tm CurrentTime;
    typedef struct _Param {
        int level;
        double desired;
        double actual;
        double prevTemp;
        double hour;
        double prevHour;
    }Param;

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
    RT_TASK CliConWARE_TASK;

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
    int init_xenomai(void);
    void catch_signal(int sig);
    void ctrl_c_module(void);
    void cleanup_module(RT_TASK CliConWARE_TASK);
    void init_task_module(void);
    void realControllerTask(void *arg);

#endif // CCSLIB_H_' > ccslib.h

































echo '// ccslib.c:
// Climate Control Software Library: C File:

// Including Libraries:
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>
#include <unistd.h>
#include <sys/mman.h>
#include <native/task.h>
#include <native/timer.h>
#include <rtdk.h>
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

    logging(INFO, "\nStarting CliConWARE_RT_TASK....");

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

    logging(INFO, "The TASK will start in 3 seconds.....\n");
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

    if (data[1] >= 0)
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
//       float           : return desired temp from config file
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

// XENOMAI Initialization:
// input parameter:
//       void : nothing
// return type:
//       int  : return SUCCESS, init XENOMAI
int init_xenomai(void) {
    // Perform auto-init of rt_print buffers if the task doesnt do so:
    rt_print_auto_init(1);

    // Lock memory - avoid memory swapping for this program:
    mlockall(MCL_CURRENT|MCL_FUTURE);

    return SUCCESS;
}

// Empty signal handler:
// input parameter:
//       int sig  : signal to handle
// return type:
//       void     : nothing
void catch_signal(int sig) {
}

/* catch signals, instead of relying on default OS signal handlers
   the default os behavior is to end program immediately, however
   we install an empty signal handler to allow execution of our
   cleanup code after the signal handler
 */
void ctrl_c_module(void) {
    signal(SIGTERM, catch_signal);
    signal(SIGINT, catch_signal);
    pause();
}

// Cleanup Module:
//    deleting task CliConWARE
void cleanup_module (RT_TASK CliConWARE_TASK) {
    rt_task_delete(&CliConWARE_TASK);
}

// Task init() module
//    creating task name CliConWARE
//    starting task
void init_task_module(void) {
    // CliConWARE_RT_TASK:
    {
        int error;
        Param RTparameters;
        char  taskName[10] = "CliConWARE";
        // int rt_task_create (RT_TASK *task, const char *name, int stack_size, int priority, int mode):
        // Arguments: &task, name, stack size (0=default), priority, mode (FPU, start suspended, ...):
        error = rt_task_create(&CliConWARE_TASK, taskName, TASK_STKSZ, TASK_PRIO, TASK_MODE);

        if (!error) {
            // int rt_task_start (RT_TASK *task, void(*task_func)(void *arg), void *arg):
            // Arguments: &task, task function, function argument:
            rt_task_start(&CliConWARE_TASK, &realControllerTask, &RTparameters);
        }
    }
}

// Empty signal handler:
// input parameter:
//       void *arg : universal data type (Param Struct)
// return type:
//       void      : nothing
void realControllerTask(void *arg) {

    // Inquire current task:
    RT_TASK *curtask;
    RT_TASK_INFO curtaskinfo;
    curtask = rt_task_self();
    rt_task_inquire(curtask, &curtaskinfo);

    // Parameters Data Structure:
    Param *RTparameters = (Param *) arg;

    // Derefrencing Parameters:
    int level = RTparameters->level;
    double desired = RTparameters->desired;
    double actual = RTparameters->actual;
    double prevTemp = RTparameters->prevTemp;
    double hour = RTparameters->hour;
    double prevHour = RTparameters->prevHour;

    char sensorTempData[5];
    char levelStr[15];

    // Starting Task:
    printf("Starting task %s....\n", curtaskinfo.name);

    // Read desired temp from Config file:
    desired = getDesired("temp_config");

    while (1) {
        // Sense an event change in current hour:
        hour = getHour();
        if (isEvent(&prevHour, hour))
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
        printf("Sensor Reading = %.2f°C  ==>  Knob Level = %d\n", actual, level);
    }
}' > ccslib.c





















echo '// Climate Control Software:

// Including libraries:
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>
#include <unistd.h>
#include <sys/mman.h>
#include <native/task.h>
#include <native/timer.h>
#include <rtdk.h>
#include "ccslib.h"

// main function:
int main(void) {

    // XENOMAI Initialization:
    init_xenomai();

    // Software Initialization:
    init();

    // CliConWARE_RT_TASK:
    init_task_module();

    // Exit:
    ctrl_c_module();
    cleanup_module(CliConWARE_TASK);
    printf("\n\nTerminating of CliConWARE_RT_TASK....\n\n");

    return SUCCESS;
}' > main.c



































xeno-config --xeno-cflags
xeno-config --xeno-ldflags

export CFLAGS=`xeno-config --xeno-cflags`
export LDFLAGS=`xeno-config --xeno-ldflags`

gcc $CFLAGS $LDFLAGS -lnative ccslib.c main.c -o CliConWARE_RT_TASK

rm ccslib.h
rm ccslib.c
rm main.c

./CliConWARE_RT_TASK
