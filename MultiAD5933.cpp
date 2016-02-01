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
*/    	
bool AD5933::writeStartFreq(long freq){
	//see datasheep page 24 for formula
	long freqHEX = (freq / ((double)clockFreq / 4.0)) * pow(2,27);

	#if LOG_ENABLED
		Serial.print("Writing Freq: ");
		Serial.print(freqHEX, HEX);
	#endif

	int low = freqHEX & 0xFF;
	int mid = (freqHEX & 0xFF00) >> 8;
	int high = (freqHEX & 0xFF0000) >> 16;

	bool reg0 = setByteToAddr(REG_START_FREQ0, high);
	bool reg1 = setByteToAddr(REG_START_FREQ1, mid);
	bool reg2 = setByteToAddr(REG_START_FREQ2, low);

	if(reg0 && reg1 && reg2){
		return true;
	}else{
		#if LOG_ENABLED
			Serial.print("ERROR Writing Freq: ");
			Serial.print(freqHEX, HEX);
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


