#include "FidgetBand.h"

//======================
//PUBLIC
//======================
FidgetBand::FidgetBand(){
	numSensors = -1;
	curSensor = -1;
}

double * FidgetBand::measure(freq){
	int numMeasurements = (numSensors * (numSensors - 1)) / 2;
	static double values[numMeasurements];
	int valueIdx = 0;

	for(int i = 0; i < numSensors-1; i++){
		setSensor(i);

		for(int j = i+1; j < numSensors; j++){

		}
	}

}

int FidgetBand::getNumSensors(){
	int count = 0;
	for(int i = 0; i < MAX_NUM_SENSORS; i++){
		setSensor(i);
		if(ad5933.reset()){
			count++;
		}
	}
	numSensors = count
	return count;
}

//======================
//PRIVATE
//======================
bool FidgetBand::setSensor(int sensorIdx){
	digitalWrite(S0, (sensorIdx & 1) ? HIGH : LOW);
  	sensorIdx >>= 1;
  	digitalWrite(S1, (sensorIdx & 1) ? HIGH : LOW);
  	sensorIdx >>= 1;
  	digitalWrite(S2, (sensorIdx & 1) ? HIGH : LOW);

  	curSensor = sensorIdx;
}

bool FidgetBand::reset(){
	for(int i = 0; i < numSensors; i++){
		setSensor(i);
		ad5933.reset();
	}
}