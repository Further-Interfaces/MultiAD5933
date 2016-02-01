#include <Arduino.h>
#include "MultiAD5933.h"

AD5933::AD5933(){
	clockFreq = 16000000;//16776000;
}

/*
Reads the current temperature of the device and
returns the value in celcius
*/
double AD5933::readTemp(){
	//send temp measure commmand to control register
	if(setCtrlRegister(MEASURE_TEMP)){
		//wait for temp measure to complete
		while(!isMeasurementComplete(VALID_TEMP_MEASURE)){
			;
		}

		int temp0 = getByteFromAddr(REG_TEMP0);
		int temp1 = getByteFromAddr(REG_TEMP1);

		//convert to celcius
		int maxBits = 14;
		int total = 0;
		int idx = 0;
		while(idx < maxBits){
			if(idx < 8){
				if(temp0 & 1){
					total += pow(2,idx);
				}
				temp0 >>= 1;
			}else{
				if(temp1 & 1){
					total += pow(2,idx);
				}
				temp1 >>= 1;
			}
			idx++;
		}
		return (double)total/32.0;
	}
}

/*
reads the start frequency stored in the start frequecy registers
*/
/*long AD5933::readStartFreq(){
	long high = getByteFromAddr(REG_START_FREQ0) << 16;
	int mid = getByteFromAddr(REG_START_FREQ1) << 8;
	int low = getByteFromAddr(REG_START_FREQ2);
	Serial.println(high ,HEX);

	long freqHEX = high | mid | low;
	Serial.print("read: ");
	Serial.println(freqHEX,HEX);
	long freqInt = ((double)freqHEX / pow(2,27)) * (clockFreq / 4.0);
	return freqInt;
}*/

/*
writes the start frequency to the start frequency registers

see page 24 of data sheet
*/    	
bool AD5933::writeStartFreq(long freq){
	long freqHEX = freqToHex(freq);
	
	int high = (freqHEX & 0xFF0000) >> 16; 
	int mid = (freqHEX & 0xFF00) >> 8;
	int low = freqHEX & 0xFF;

	bool reg0 = setByteToAddr(REG_START_FREQ0, high);
	bool reg1 = setByteToAddr(REG_START_FREQ1, mid);
	bool reg2 = setByteToAddr(REG_START_FREQ2, low);

	if(reg0 && reg1 && reg2){
		startFreq = freq;
		return true;
	}else{
		#if LOG_ENABLED
			Serial.print("ERROR Writing Freq: ");
			Serial.print(freq);
		#endif
		return false;
	}
}

/*
writes the frequency increment value

see page 25 of data sheet
*/
bool AD5933::writeFreqStepVal(long freq){
	long freqHEX = freqToHex(freq);
	
	int high = (freqHEX & 0xFF0000) >> 16; 
	int mid = (freqHEX & 0xFF00) >> 8;
	int low = freqHEX & 0xFF;

	bool reg0 = setByteToAddr(REG_FREQ_INC0, high);
	bool reg1 = setByteToAddr(REG_FREQ_INC1, mid);
	bool reg2 = setByteToAddr(REG_FREQ_INC2, low);

	if(reg0 && reg1 && reg2){
		freqStepVal = freq;
		return true;
	}else{
		#if LOG_ENABLED
			Serial.print("ERROR Writing Freq Step Val: ");
			Serial.print(freq);
		#endif
		return false;
	}
}

/*
Converts the given frequency into a hex representation defined by
the formula given on page 24/25 of the data sheet. 
*/
long AD5933::freqToHex(long freq){
	long freqHEX = (freq / ((double)clockFreq / 4.0)) * pow(2,27);
	return freqHEX;
}

/*
Writes the number of freqeuncy steps to the num frequency step registers. 
Technically, this number can be between 0 and 511 (inclusive) however, for this
implementation i am only supporting 8 bits (0 - 255) because i am lazy.
*/
bool AD5933::writeNumSteps(int steps){
	if(setByteToAddr(REG_NUM_INCS1, steps)){
		numSteps = steps;
		return true;
	}else{
		#if LOG_ENABLED
			Serial.print("ERROR Writting num steps: ");
			Serial.println(steps);
		#endif
		return false;
	}
}

/*
Writes the number of settling time cycles. A description of this can be seen on
page 25 of the datasheet. While 2 bytes are available to set (for larger values), 
my implementation will, again, only support 1 byte (sorry).
*/
bool AD5933::writeSettlingTimeCycles(int numCycles){
	if(setByteToAddr(REG_SETTLE1, numCycles)){
		numSettlingCycles = numCycles;
		return true;
	}else{
		#if LOG_ENABLED
			Serial.print("ERROR Writting num settling cycles: ");
			Serial.println(numCycles);
		#endif
		return false;
	}
}

/*
Reset the AD5933
*/
bool AD5933::reset(){
	int addr = REG_CTRL1;
	int ctrlByte = getByteFromAddr(addr);
	ctrlByte &= 0x04; //keep D3 only
	ctrlByte |= RESET; //set D4 (reset)
	return setByteToAddr(addr, ctrlByte);
}

/*
Set if an external system clock is being used
*/
bool AD5933::setExtSysClock(bool usingExt){
	if(usingExt){
		return setByteToAddr(REG_CTRL1, EXT_SYS_CLK);
	}else{
		return setByteToAddr(REG_CTRL1, 0x00);
	}
}

bool AD5933::initStartFreq(){
	if(setCtrlRegister(INIT_START_FREQ)){
		return true;
	}else{
		return false;
	}
}

bool AD5933::startFreqSweep(){
	if(setCtrlRegister(START_FREQ_SWEEP)){
		return true;
	}else{
		return false;
	}
}

bool AD5933::repeatFreq(){
	if(setCtrlRegister(REPEAT_FREQ)){
		return true;
	}else{
		return false;
	}
}

long AD5933::readMagnitude(){
	if(isMeasurementComplete(VALID_DATA)){
		//read values from real and imaginary registers
		int realHigh = getByteFromAddr(REG_REAL0);
		int realLow = getByteFromAddr(REG_REAL1);
		int imgHigh = getByteFromAddr(REG_IMG0);
		int imgLow = getByteFromAddr(REG_IMG1);

		int real = realLow + (realHigh * 256);
		if(real > 0x7FFF){
			real &= 0x7FFF;
			real -= 65536;
		}

		int img = imgLow + (imgHigh * 256);
		if(img > 0x7FFF){
			img &= 0x7FFF;
			img -= 65536;
		} 

		//cacluclate magnitude
		long magnitude = pow((pow(real,2) + pow(img,2)), 0.5);
		return magnitude;
	}else{
		return 0;
	}
}


/*
Sets the AD5933 control modes such as starting a frequency sweep,
reading temperature, power down, etc. 

See page 23 of data sheet.
*/
bool AD5933::setCtrlRegister(int mode){
	int ctrlByte = getByteFromAddr(REG_CTRL0);
	ctrlByte &= 0x0F; //only keep D8-D11
	ctrlByte |= mode;

	bool success = setByteToAddr(REG_CTRL0, ctrlByte);

	#if LOG_ENABLED
		if(!success){
			Serial.print("ERROR Setting Ctrl Reg: ");
			Serial.println(ctrlByte, HEX);
		}
	#endif

	return success;
}

/*
Sets the address that the address pointer should
point to.

See page 28 of data sheet
*/
bool AD5933::setAddressRegister(int addr){
	Wire.beginTransmission(AD5933_ADDR);
	//write addr to the address pointer register
	Wire.write(ADDR_POINTER);
	Wire.write(addr);

	Wire.endTransmission();
}

/*
Polls the status register to check if a certain
measurement (mode) is complete. 
*/
bool AD5933::isMeasurementComplete(int mode){
	//get byte from status register
	int status = getByteFromAddr(REG_STATUS);
	//Serial.println(status,HEX);
	if(mode == VALID_TEMP_MEASURE || mode == VALID_DATA || mode == VALID_FREQ_SWEEP){
		return status & mode == mode;
	}else{
		#if LOG_ENABLED
			Serial.println("ERROR invalid mode for polling status register");
		#endif
		return false;
	}
}

/*
Gets a single byte that is stored at address addr
*/
int AD5933::getByteFromAddr(int addr){
	setAddressRegister(addr);

	Wire.beginTransmission(AD5933_ADDR);
	Wire.requestFrom(AD5933_ADDR, 1);

	int val;
	if(Wire.available()){
		val = Wire.read();
	}else{
		val -1;;
	}

	Wire.endTransmission();
	return val;
}

/*
Sets a single byte to address addr
*/
bool AD5933::setByteToAddr(int addr, int val){
	Wire.beginTransmission(AD5933_ADDR);
	Wire.write(addr);
	Wire.write(val);
	int i2cReturn = Wire.endTransmission();

	if(i2cReturn == 0){
		return true;
	}else{
		#if LOG_ENABLED
			Serial.print("ERROR Setting Byte: ");
			Serial.print(addr, HEX);
			Serial.println(val, HEX);
		#endif
		return false;
	}
}


