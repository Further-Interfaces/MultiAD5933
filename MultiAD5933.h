#ifndef AD5933_h
#define AD5933_h

#include<Wire.h> // I2C
#include<Arduino.h> 

#define LOG_ENABLED 1

#define AD5933_ADDR 0x0D
#define ADDR_POINTER 0xB0

//register map (data sheet p23)
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

//control register map (data sheet p23)
const int INIT_START_FREQ = 0x10;
const int START_FREQ_SWEEP = 0x20;
const int INC_FREQ = 0x30;
const int REPEAT_FREQ = 0x40;
const int MEASURE_TEMP = 0x90;
const int POWER_DOWN = 0xA0;
const int STANDBY = 0xB0;
const int RANGE_1 = 0x00;
const int RANGE_2 = 0x06;
const int RANGE_3 = 0x04;
const int RANGE_4 = 0x02;
const int RESET = 0x01; //actual value should be 0x10 (already taken)
const int EXT_SYS_CLK = 0x08;

class AD5933{
    public:
        AD5933();
    	//temperature
    	double readTemp();

    	//start frequency
    	/*bool readStartFreq();
    	bool writeStartFreq(long freq);

    	//frequency increment
    	bool readFreqInc();
    	bool writeFreqInc(long freq);

    	//number of frequency increments
    	bool readNumFreqInc();
    	bool writeNumFreqInc(int num);*/

    	//settling time cycles
    	//bool readSettlingTimeCycles();
    	//bool writeSettlingTimeCycles(int);

    private:
    	int getByteFromAddr(int addr);
    	bool setByteToAddr(int addr, int val);

        bool setAddressRegister(int addr);
    	
    	bool setCtrlRegister(int mode);
};

#endif