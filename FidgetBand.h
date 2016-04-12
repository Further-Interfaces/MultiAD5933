/*

PUBLIC
===========
measure(freq) - takes impedance measurements using the given frequency on all electrodes in order starting with 0. The 0 index electrode will act as the transmiter and electrodes 1-n will recieve. Then the 1 index electrode will act as the transmiter and electrodes 0,2-n will recieve, etc.
	return: an array containing the impedance measurements between each electrode
			with order [0-1, 0-2, 0-3, 1-2, 1-3, 2-3] in the case of 4 electrodes.
			**Should it be sent over serial by default??

getNumSensors() - returns the number of sensors (AD5933 chips) currently connected to the system


PRIVATE
===========
setSensor(i) - allows communications with the ith sensor

*/

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

		double * measure(long freq);
		bool reset();

		bool setStartFreq(long startFreq);
		bool setFreqStepVal(long freqStepVal);
		bool setNumSteps(int numSteps);

	private:
		int numSensors;
		int curSensor; //the index of the sensor that is currently being accessed
		bool isSensing;
		AD5933 ad5933;

		bool setSensor(int sensorIdx);

		bool transmit(int sensorIdx);
		double receive(int sensorIdx);

};

#endif