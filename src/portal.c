#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "portal.h"
#include "serial.h"

#define STRUCT_SIZE 3000
#define FALSE 0
#define TRUE 1

int USBfileDescriptor;

int * rawData;
int * cps;
int * itg;

int sigma = 5;

int IN_ALARM = FALSE;

// HEADER IMPLEMENTATION
int init();
int close();
int * update();
void changeSigma(int newSigma);

// CONTROLLER
int addToCps(int * vals); 
void addToIntegration(int val);
int getBackValue();
int getAlarmValue(int back);
int getLastMax();
int checkAlarm(int vals_max, int alarm_value); 

int init(){

    USBfileDescriptor = openAndSetTermiosDevice();
    if(USBfileDescriptor < 0) return USBfileDescriptor;

    rawData = (int *) calloc(STRUCT_SIZE * 5, sizeof(int));
    if(rawData == NULL) return 0;

    cps = (int *) calloc(STRUCT_SIZE, sizeof(int));
    if(cps == NULL) return 0;

    itg = (int *) calloc(STRUCT_SIZE, sizeof(int));
    if(itg == NULL) return 0;

    return 1;
}

int close(){
    
   return closeFD(USBfileDescriptor);

}


int * update(){

    int * last5Val = (int *) calloc( 5 , sizeof(int)); 

    if(getData(USBfileDescriptor, last5Val) == FALSE) return NULL;  

    int lastCps = addToCps(last5Val);

    free(last5Val);
    
    if(!IN_ALARM) addToIntegration(lastCps);

    int back_value = getBackValue();

    int alarm_value =  getAlarmValue(back_value);

    IN_ALARM = checkAlarm(lastCps, alarm_value);

    int * result = (int *) calloc(4, sizeof(int)); 

    result[0] = lastCps;
    result[1] = back_value;
    result[2] = alarm_value;
    result[3] = IN_ALARM;

    return result;
}

void changeSigma(int newSigma){
    sigma = newSigma;
}

int getLastMax(){

    int max = -1;

    for (size_t i = 0; i < 5; i++)
    {
        if(cps[STRUCT_SIZE - 1 - i]>max) max = cps[STRUCT_SIZE - 1 -i]; 
    }
    
    return max;
}

/*
IN :
    200ms values array, length 5
OUT : 
    last cps Val
*/
int addToCps(int * vals ){

    // SHIFT RawList 5 SX 
    for (size_t i = 0; i < STRUCT_SIZE * 5 - 5; i++)
    {
        rawData[i] = rawData[i+5];
    }

    // ADD last 5 200ms values to RawList 
    rawData[STRUCT_SIZE * 5 - 1] = vals[4];
    rawData[STRUCT_SIZE * 5 - 2] = vals[3];
    rawData[STRUCT_SIZE * 5 - 3] = vals[2];
    rawData[STRUCT_SIZE * 5 - 4] = vals[1];
    rawData[STRUCT_SIZE * 5 - 5] = vals[0];

    // SHIFT CPS list SX 
    for (size_t i = 0; i < STRUCT_SIZE - 1; i++)
    {
        cps[i] = cps[i+1];
    }

    int cps_val = vals[4] + vals[3] + vals[2] + vals[1] + vals[0];

    cps[STRUCT_SIZE - 1] = cps_val;

    return cps_val;
}

void addToIntegration(int val){

    // SHIFT 1 SX
    for (size_t i = 0; i < STRUCT_SIZE - 1; i++)
    {
        itg[i] = itg[i+1];
    }

    itg[STRUCT_SIZE - 1] = val;
} 

int getBackValue(){

    int sum = 0, counter = 0;

    for (size_t i = 0; i < STRUCT_SIZE; i++)
    {
        if(itg[i]!=0){
            counter++;
            sum += itg[i];
        }
    }
    
    // counter = 10 
    if(counter > 0)
        return (int)sum/counter;
    else
        return 0;
} 

int getAlarmValue(int back_value){
    return ( (int)( sqrt(back_value) * sigma ) + back_value ); 
}

int checkAlarm(int lastCps, int alarm_value){
    if(lastCps>alarm_value) return TRUE;
    else return FALSE;
}

   

