#ifndef AD5933
#define AD5933

#include<Wire.h> // I2C
#include<Arduino.h> 

#define LOG_ENABLED 1

#define AD5933_ADR 0x0D;

//register map
#define REG_CTRL0 0x80
#define REG_CTRL1 0x81

#define REG_START_FREQ0 0x82
#define REG_START_FREQ1 0x83
#define REG_START_FREQ2 0x84

#define REG_FREQ_INC0 0x85
#define REG_FREQ_INC1 0x86
#define REG_FREQ_INC2 0x87

#define REG_NUM_INCS0 0x88
#define REG_NUM_INCS1 0x89

#define REG_SETTLE0 0x8A
#define REG_SETTLE1 0x8B

#define REG_STATUS 0x8F

#define REG_TEMP0 0x92
#define REG_TEMP1 0x93

#define REG_REAL0 0x94
#define REG_REAL1 0x95

#define REG_IMG0 0x96
#define REG_IMG1 0x97

class AD5933{
    public:
    	//temperature
    	bool readTemp();

    	//start frequency
    	bool readStartFreq();
    	bool writeStartFreq(long);

    	//frequency increment
    	bool readFreqInc();
    	bool writeFreqInc(long);

    	//number of frequency increments
    	bool readNumFreqInc();
    	bool writeNumFreqInc(int);

    	//settling time cycles
    	bool readSettlingTimeCycles();
    	bool writeSettlingTimeCycles(int);
}