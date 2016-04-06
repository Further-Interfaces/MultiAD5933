MultiAD5933
======
This is an Arduino library for controlling multiple AD5933 chips.

The AD5933 is an IC used for impedance analysis and has a built in frequency generator, 12-bit ADC and DSP used to perform DFT on signals.
While the AD5933 can be communicated with over I2C, its address is hard coded, making it more difficult to communicate with multiple AD5933s 
from a single I2C master. However, by using an analog multiplexer/demultiplexer (such as the CD74HCT4051), an SDA signal can be sent to the 
selected AD5933 (all AD5933s should be connected to a common SCL). 

This library is set to use the CD74HCT4051 multiplexer/demultiplexer which can support up to eight AD5933s. 
