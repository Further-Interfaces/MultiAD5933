#include <Arduino.h>
#include "MultiAD5933.h"

AD5933::AD5933(){

}

double AD5933::readTemp(){

}


/*
Sets the AD5933 control modes such as starting a frequency sweep,
reading temperature, power down, etc. 

See page 23 of data sheet.
*/
bool AD5933::setCtrlRegister(int mode){
	int ctrlReg = REG_CTRL0;
	if(mode == RESET || mode == EXT_SYS_CLK){
		ctrlReg = REG_CTRL1;
		if(mode == RESET)
			mode = 0x10;
	}

	int ctrlByte = getByteFromAddr(ctrlReg);
	ctrlByte |= mode;

	bool success = setByteToAddr(ctrlReg, ctrlByte);
	return success;
}

/*
Sets the address that the address pointer should
point to.

See page 28 of data sheet
*/
bool AD5933::setAddressRegister(int addr){
	Wire.beginTransmission(AD5933_ADDR);
	//write addr to the address register
	Wire.write(ADDR_POINTER);
	Wire.write(addr);

	Wire.endTransmission();
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
	Wire.endTransmission();
}


