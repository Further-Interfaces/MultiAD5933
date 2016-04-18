#include "FidgetBand.h"

//======================
//PUBLIC
//======================
namespace{
	AD5933 ad5933;
}

FidgetBand::FidgetBand(){
	numSensors = -1;
	curSensor = -1;
	isSensing = false;

	getNumSensors();

	reset();
	standby();
	setOutVoltageRange(1);
	setPGA(1);
	setStartFreq(30000);
	setFreqStepVal(5000);
	setNumSteps(0);
}

FidgetBand::FidgetBand(long startFreq, long freqStepVal, int numSteps){
	numSensors = -1;
	curSensor = -1;
	isSensing = false;

	getNumSensors();

	//setup each sensor
	reset();
	standby();
	setOutVoltageRange(1);
	setPGA(1);
	setStartFreq(startFreq);
	setFreqStepVal(freqStepVal);
	setNumSteps(numSteps);
}

/*
Takes a cross sectional impedance measurment using all connected electrodes. Returns a pointer
to an array containing the impedance measurements between each electrode
with order [0-1, 0-2, 0-3, 1-2, 1-3, 2-3] in the case of 4 electrodes.
*/
double * FidgetBand::measure(){
	isSensing = true;
	int numMeasurements = (numSensors * (numSensors - 1)) / 2;
	double values[numMeasurements];
	int valueIdx = 0;
	Serial.println(numSensors);

	for(int i = 0; i < numSensors-1; i++){
		transmit(i);

		for(int j = i+1; j < numSensors; j++){
			double val = receive(j);
			values[valueIdx] = val;
			valueIdx++;
			Serial.println(val);
		}
	}

}

/*
returns the number of sensors (Ad5933 chips) currently connected
*/
int FidgetBand::getNumSensors(){
	int count = 0;
	for(int i = 0; i < MAX_NUM_SENSORS; i++){
		setSensor(i);
		if(ad5933.reset()){
			count++;
		}
	}
	numSensors = count;
	return count;
}

bool FidgetBand::reset(){
	for(int i = 0; i < numSensors; i++){
		setSensor(i);
		ad5933.reset();
	}
}

bool FidgetBand::standby(){
	for(int i = 0; i < numSensors; i++){
		setSensor(i);
		ad5933.standby();
	}
}

/*
Sets the start frequency of each connected sensor to startFreq HZ. Should
only be called when the device is not sensing.
*/
bool FidgetBand::setStartFreq(long startFreq){
	if(!isSensing){
		for(int i = 0; i < numSensors; i++){
			setSensor(i);
			bool success = ad5933.writeStartFreq(startFreq);
			if(!success)
				return false;
		}
		return true;
	}
	return false;
}

/*
Sets the frequency step value of each connected sensor to freqStepVal HZ. Should
only be called when the device is not sensing.
*/
bool FidgetBand::setFreqStepVal(long freqStepVal){
	if(!isSensing){
		for(int i = 0; i < numSensors; i++){
			setSensor(i);
			bool success = ad5933.writeFreqStepVal(freqStepVal);
			if(!success)
				return false;
		}
		return true;
	}
	return false;
}

/*
Sets the number of frequency steps for each connected sensor to numSteps. Should
only be called when the device is not sensing.
*/
bool FidgetBand::setNumSteps(int numSteps){
	if(!isSensing){
		for(int i = 0; i < numSensors; i++){
			setSensor(i);
			bool success = ad5933.writeNumSteps(numSteps);
			if(!success)
				return false;
		}
		return true;
	}
	return false;
}

/*

*/
bool FidgetBand::setOutVoltageRange(int range){
	if(!isSensing){
		if(range > 4 || range < 1)
			return false;
		else{
			for(int i = 0; i < numSensors; i++){
				setSensor(i);
				bool success = false;
				
				if(range == 1)
					success = ad5933.setOutVoltageRange1();
				else if(range == 2)
					success = ad5933.setOutVoltageRange2();
				else if(range == 3)
					success = ad5933.setOutVoltageRange3();
				else if(range == 4)
					success = ad5933.setOutVoltageRange4();

				if(!success)
					return false;
			}
			return true;
		}
	}
	return false;
}

/*

*/
bool FidgetBand::setPGA(int x){
	if(!isSensing){
		if(x == 1 || x == 5){
			for(int i = 0; i < numSensors; i++){
				setSensor(i);
				bool success = false;
				
				if(x == 1)
					success = ad5933.setPGAx1();
				else if(x == 5)
					success = ad5933.setPGAx5();

				if(!success)
					return false;
			}
			return true;
		}
		return false;
	}
	return false;
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

bool FidgetBand::transmit(int sensorIdx){
	setSensor(sensorIdx);
	ad5933.initStartFreq();
    ad5933.startFreqSweep();
}

double FidgetBand::receive(int sensorIdx){
	setSensor(sensorIdx);
	return ad5933.readMagnitude();
}

