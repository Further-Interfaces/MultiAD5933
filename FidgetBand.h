#ifndef FidgetBand_h
#define FidgetBand_h

#include <Arduino.h> 
#include "MultiAD5933.h"

#define MAX_NUM_SENSORS 8 //the maximum number of sensors supported by the fidget band

//CD74HC4051 multiplexer address select pins
const int S2 = 16; 
const int S1 = 14;
const int S0 = 15;

class FidgetBand{
	public:
		FidgetBand();
		FidgetBand(long startFreq, long freqStepVal, int numSteps);

		int getNumSensors();

		double * measure();
		bool reset();
		bool standby();

		bool setStartFreq(long startFreq);
		bool setFreqStepVal(long freqStepVal);
		bool setNumSteps(int numSteps);
		bool setOutVoltageRange(int range);
		bool setPGA(int x);

	private:
		int numSensors;
		int curSensor; //the index of the sensor that is currently being accessed
		bool isSensing;

		bool setSensor(int sensorIdx);

		bool transmit(int sensorIdx);
		double receive(int sensorIdx);

};

#endif