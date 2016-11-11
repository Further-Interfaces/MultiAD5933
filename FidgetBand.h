#ifndef FidgetBand_h
#define FidgetBand_h

#include <Arduino.h> 
#include "MultiAD5933.h"

#define MAX_NUM_SENSORS 1 //the maximum number of sensors supported by the fidget band

//CD74HC4051 multiplexer address select pins
const int S0 = 15;
const int S1 = 14;
const int S2 = 16; 


class FidgetBand{
	public:
		FidgetBand();
		FidgetBand(long startFreq, long freqStepVal, int numSteps);

		bool initBand(); //must be called first

		int getNumSensors();

		double * measure();
		bool reset();
		bool standby();

		bool setStartFreq(long startFreq);
		bool setFreqStepVal(long freqStepVal);
		bool setNumSteps(int numSteps);
		bool setNumSettlingTimeCycles(int numCycles);
		bool setOutVoltageRange(int range);
		bool setPGA(int x);

	private:
		AD5933 ad5933;
		int numSensors;
		int curSensor; //the index of the sensor that is currently being accessed
		bool isSensing;

		void setSensor(int sensorIdx);

		bool transmit(int sensorIdx);
		double receive(int sensorIdx);

};

#endif