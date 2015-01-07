/*
   AD536x.h  - AD536x family DAC control library for Arduino. 
   
   Should work with Analog devices AD5360, AD5361, AD5362, AD5363, 
   and possibly others.
   
   Created by Alessandro Restelli, 2013
   Re-written by Neal Pisenti, 2015
   JQI - Joint Quantum Institute

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.


   This library works for both the AD536x and AD5361 chips.
   To properly use the library with AD5361 it is imperative to set the two LSB to 0
   Setting those values to 1 is deprecated because those bits are reserved for future use in the AD5361

*/

// ensure this library description is only included once
#ifndef AD536x_h
#define AD536x_h

// include types & constants of Wiring core API
#include "Arduino.h"
#include "SPI.h"

// put these in if-then statements for given DAC type...
#define AD536x_MAX_CHANNELS 4
#define AD536x_RESOLUTION 16
#define AD536x_DATA_MASK 0xFFFF
#define AD536x_CH_MASK 0x03	// or 0x07 for 8 ch/bank

// note especially, these are different for 16/14-bit versions.
#define AD536x_DEFAULT_DAC 0x8000
#define AD536x_DEFAULT_OFFSET 0x8000
#define AD536x_DEFAULT_GAIN 0xFFFF
#define AD536x_DEFAULT_GLOBALOFFSET 0x2000


//Definition of constants and AD536x registers
/*

AD536x (AD5361) has a 24 bit instruction/data frame organized like this:

23	22	21	20	19	18	17	16	15	14	13	12	11	10	9	8	7	6	5	4	3	2	1		0
M1	M0	A5	A4	A3	A2	A1	A0	D15	D14	D13	D12	D11 D10 D9	D8	D7	D6	D5	D4	D3	D2	D1(0)	D0(0)

*/

// first two control bits M1, M0
#define AD536x_SPECIAL_FUNCTION 0
#define	AD536x_WRITE_DAC 		3<<22
#define AD536x_WRITE_OFFSET 	2<<22
#define	AD536x_WRITE_GAIN 		1<<22

//Then I will define constats used to generate the address or the special codes.

//When AD536x_SPECIAL_FUNCTION is not used address bits A4 and A3 are important to select which bank of DAC to control.
// A5 is not in use and must be always set to 0

// Select which bank to address
/* 
	21	   20 19			 18	17 16
	A5	| (A4|A3) 		| 	(A2|A1|A0)
	0	| 00 -- All		|	000 -> All banks; 001 -> bank 0; 010 -> bank 1
		| 01 -- Bank 0	|	dac #, bank 0
		| 10 -- Bank 1	|	dac #, bank 1
*/

#define AD536x_BANK0		1 << 19
#define AD536x_BANK1		2 << 19
#define AD536x_ALL_DACS		0
#define AD536x_ALL_BANK0	1 << 16
#define AD536x_ALL_BANK1	2 << 16


// alternative format for addressing bank 1 or 0
#define AD536x_BANK(bank)   (bank+1)<<19

// specific channel, if desired.
#define AD546x_CH0		0
#define AD546x_CH1		1<<16
#define AD546x_CH2		2<<16
#define AD546x_CH3		3<<16
#define AD546x_CH4		4<<16
#define AD546x_CH5		5<<16
#define AD546x_CH6		6<<16
#define AD546x_CH7		7<<16

// When AD536x_SPECIAL_FUNCTION is used it is possibile to give complex commands
// including reading from registers!
// In Special function mode the frame is organized in this way:

// 23	22	21	20	19	18	17	16	15	14	13	12	11	10	9	8	7	6	5	4	3	2	1	0
//  0	 0	S5	S4	S3	S2	S1	S0	F15	F14	F13	F12	F11 F10 F9	F8	F7	F6	F5	F4	F3	F2	F1	F0

#define AD536x_NOP	0
#define AD536x_WR_CR 1<<15 //Write on control register
	//Let's set the FLAGS for the control register
	//Flags can be combined!!!
	#define AD536x_X1B 4
	#define AD536x_X1A 0
	#define AD536x_T_SHTDWN_EN 2
	#define AD536x_T_SHTDWN_DIS 0	
	#define AD536x_SOFT_PWR_UP 1	
	#define AD536x_SOFT_PWR_DWN 0	
#define AD536x_WRITE_OFS0 2<<15 //Writes in the OFFSET 0 ANALOG DAC. the data is a 14 bit variable
#define AD536x_WRITE_OFS1 3<<15 //Writes in the OFFSET 1 ANALOG DAC. the data is a 14 bit variable
#define AD536x_READ_REG 5<<15 //Select which register to read
	//This is the set of commends that select a particular register
	#define AD536x_READ_X1A(channel) 0|(channel+8)<<6
	#define AD536x_READ_X1B(channel) (1<<12)|(channel+8)<<6
	#define AD536x_READ_C(channel) (2<<12)|(channel+8)<<6
	#define AD536x_READ_M(channel) (3<<12)|(channel+8)<<6
	#define AD536x_READ_CR (3<<12)|(1<<6) //Read the control register: my favorite!
		//Flags defined for register Writing can be used for interrogation of the state
		//In addition the following flags can be used for read-only interrogations
		#define AD536x_CR_OVERTEMP 16 
		#define AD536x_CR_PEC 8
	#define AD536x_READ_OFS0 (3<<12)|(2<<6)						 
	#define AD536x_READ_OFS1 (3<<12)|(3<<6)							 
	#define AD536x_READ_AB_0 (3<<12)|(6<<6)
	#define AD536x_READ_AB_1 (3<<12)|(7<<6)	 
	#define AD536x_READ_GPIO (3<<12)|(11<<6) //F6 to F0 SHOULD be 0
#define AD536x_WR_AB_SELECT_0 6<<15 //F7 to F0 select registers X2A or X2B for bank 0 A is0 and B is 1
#define AD536x_WR_AB_SELECT_0 11<<15 //F7 to F0 select registers X2A or X2B for bank 0 A is0 and B is 1
#define AD536x_BLOCK_WR_AB_SELECT 19<<15 //Block write AB
#define AD536x_MON 			12<<15 //Additional monitor commands specified below
	#define AD536x_CMD_MON_ENABLE 1<<4
	#define AD536x_CMD_MON_DISABLE 1<<4
	#define AD536x_CMD_MON_IN_PIN_SEL(pin) (1<<4)|pin // pin can be 0 or 1 and selects the input pin from MON_IN0 or MON_IN1								 
	#define AD536x_CMD_MON_DAC_CH_SEL(channel) channel // F3:F0 selects the input channel. If a number greater than 15 is used will cause errors!
#define	AD536x_WR_GPIO 13<<15 //F1=1 sets the GPIO as output F1=0 sets GPIO as input F0 contains the status.


// register types
enum AD536x_reg_t { DAC, OFFSET, GAIN };

// Bank types
enum AD536x_bank_t { BANK0, BANK1, BANKALL };

// channel types
enum AD536x_ch_t { CH0, CH1, CH2, CH3, CH4, CH5, CH6, CH7, CHALL };


// library interface description
class AD536x
{
  // user-accessible "public" interface
  public:
  	
  	//! Constructor for AD536x object.
  	/*!
  		takes as arguments pin assignments for CS, CLR, LDAC, and RESET pins.
  	*/
  	AD536x(int cs, int clr, int ldac, int reset);
  	
  	//! Write 16-bit tuning word to DAC, and update output
  	/*!
  		bank: BANK0, BANK1, or BANKALL
  		ch: CH0 .. CH7 (or .. CH3), or CHALL for all channels.
  		data: unsigned int data payload. See datasheet for details.
  		
  		DAC transfer function is given by
  		
  		VOUT = 4*VREF*(DAC_CODE/2^16 - OFFSET_CODE/2^14)
  		DAC_CODE = data*(M+1)/2^16 + (C - 2^15)
  		
  		M and C can be written using writeGain and writeOffset
  		
  		See: writeDACHold	
  	*/
    void writeDAC(AD536x_bank_t bank, AD536x_ch_t ch, unsigned int data);

    
    //! Write 16-bit tuning word to DAC, but do not issue IO update.
    /*! 
    	bank: BANK0, BANK1, or BANKALL
  		ch: CH0 .. CH7 (or .. CH3), or CHALL for all channels.
  		data: unsigned int data payload. See datasheet for details.
  		
  		See: writeDAC
    */
    void writeDACHold(AD536x_bank_t bank, AD536x_ch_t ch, unsigned int data);


	//! Read DAC value
	/*!
		bank: BANK0, BANK1, or BANKALL
  		ch: CH0 .. CH7 (or .. CH3), or CHALL for all channels.
  		
  		Reads locally stored DAC value for given channel.
  		
  		See: writeDAC
	*/
    unsigned int readDAC(AD536x_bank_t bank, AD536x_ch_t ch);
    

	//! Write Offset trim value
	/*!
		bank: BANK0, BANK1, or BANKALL
  		ch: CH0 .. CH7 (or .. CH3), or CHALL for all channels.
  		data: unsigned int data payload. See datasheet for details.
  		
  		See: readOffset
	*/	
	void writeOffset(AD536x_bank_t bank, AD536x_ch_t ch, unsigned int data);


	//! Read Offset trim value.
	/*! 
		bank: BANK0, BANK1, or BANKALL
  		ch: CH0 .. CH7 (or .. CH3), or CHALL for all channels. 
		
		See: writeOffset
	*/
	unsigned int readOffset(AD536x_bank_t bank, AD536x_ch_t ch);
	
		
	//! Write Gain trim value
	/*!
		bank: BANK0, BANK1, or BANKALL
  		ch: CH0 .. CH7 (or .. CH3), or CHALL for all channels.
  		data: unsigned int data payload. See datasheet for details.
  		
  		See: readGain
	*/	
	void writeGain(AD536x_bank_t bank, AD536x_ch_t ch, unsigned int data);


	//! Read gain trim value.
	/*! 
		bank: BANK0, BANK1, or BANKALL
  		ch: CH0 .. CH7 (or .. CH3), or CHALL for all channels.
		
		See: writeGain
	*/
	unsigned int readGain(AD536x_bank_t bank, AD536x_ch_t ch);
	
	

    
	//! Write a particular voltage to DAC, and update output.
	/*!
		
		bank: BANK0, BANK1, or BANKALL
  		ch: CH0 .. CH7 (or .. CH3), or CHALL for all channels.
  		voltage: double-precision voltage value.
  		
  		Before using setVoltage, be sure to configure the AD536x instance
  		with appropriate vref.
	*/
    void setVoltage(AD536x_bank_t bank, AD536x_ch_t ch, double voltage);


	//! Write a particular voltage to DAC, but do not issue IO update.
	/*!
		bank: BANK0, BANK1, or BANKALL
  		ch: CH0 .. CH7 (or .. CH3), or CHALL for all channels.
  		voltage: double-precision voltage value.
  		
  		If channel is not provided, will update entire bank
  		If bank is not provided, will update all banks, all channels
  		
  		Before using setVoltage, be sure to configure the AD536x instance
  		with appropriate vref, gain, and offset.
	*/
	void setVoltageHold(AD536x_bank_t bank, AD536x_ch_t ch, double voltage);



	
    //! Issue an IO update
    /*!
    	Must call IO update manually if using writeHold.
    */
    void IOUpdate();


	//! Reset DAC. See datasheet for details.
	void reset();
	
	//! assert ~CLR. See datasheet for details.
	void assertClear(int state);



	//! Change global offset DAC for given bank
	/*!
		Writes to registers OFS0 or OFS1, depending on bank.
		
		bank: BANK0 or BANK1
		data: 14-bit offset word
		
		See datasheet for more info.
	*/
	void writeGlobalOffset(AD536x_bank_t bank, unsigned int data);

	//! Read local value for global offset.
	/*!
		bank: BANK0 or BANK1
		
		returns 14-bit offset word.
	*/
	unsigned int readGloablOffset(AD536x_bank_t bank);


	//! Write an arbitrary command. 
	/*! 
		Pass in an arbitrary (3 bytes) command. See datasheet for more info.
	*/
	void writeCommand(unsigned long cmd);
  
  
  private:
  
  	//! digital pins for DAC I/O interface
  	int _sync, _ldac, _clr, _reset;
  
  	//! DAC values
  	unsigned int _dac[2][AD536x_MAX_CHANNELS];
  
  	
  	//! Offset trim code array. First index is bank, second index is channel.
  	unsigned int _offset[2][AD536x_MAX_CHANNELS];
  	
  	//! Gain trim code array. First index is bank, second index is channel.
  	unsigned int _gain[2][AD536x_MAX_CHANNELS];
  	
  	//! 14-bit global offset
  	unsigned int _globalOffset[2];
  	
  	//! Vref for each bank.
  	double _vref[2];
  	
  	//! Private implementation to write DAC registers.
  	/*!
		reg: DAC, OFFSET, or GAIN
		bank: BANK0, BANK1, or BANKALL
  		ch: CH0 .. CH7 (or .. CH3), or CHALL for all channels.
  		data: 14 or 16 bit data payload.
  		
  		For public interface, use writeDAC or writeDACHold.
  		
  	*/
	void write(AD536x_reg_t reg, AD536x_bank_t bank, AD536x_ch_t ch, unsigned int data);
    

};

#endif

