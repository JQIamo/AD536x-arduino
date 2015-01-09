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
#include "settings.h"


	// note, in 2x banks
	#define AD536x_MAX_CHANNELS 4
	#define AD536x_RESOLUTION 16
	#define AD536x_DATA_MASK 0xFFFF
	#define AD536x_CH_MASK 0x03	
	
	#define AD536x_DEFAULT_DAC 0x8000
	#define AD536x_DEFAULT_OFFSET 0x8000
	#define AD536x_DEFAULT_GAIN 0xFFFF
	#define AD536x_DEFAULT_GLOBALOFFSET 0x2000
	
	#define AD536x_DEFAULT_MAX 0xFFFF
	#define AD536x_DEFAULT_MIN 0x0000


/*
// User must define a dac type before including AD536x library.
#ifndef AD536x_MODEL
//! Pre-processor macro defining AD536x type.
/*!
	AD5360: 16ch, 16bit
	AD5361: 16ch, 14bit
	AD5362: 8ch, 16bit
	AD5363: 8ch, 14bit
	
	Defaults to AD5362, since that was what I prototyped with :P
	
	In your sketch, include a line like:
	
	    #define AD536x_MODEL 0	// AD5360
	
	before calling `#include "AD536x.h"` to select which chip version you have.
*/
/*
#define AD536x_MODEL 2	// Defaults to AD5362
#endif
*/

//! AD5360 DAC
#ifdef AD536x_AD5360
	// note, in 2x banks
	#define AD536x_MAX_CHANNELS 8
	#define AD536x_RESOLUTION 16
	#define AD536x_DATA_MASK 0xFFFF
	#define AD536x_CH_MASK 0x07
	
	#define AD536x_DEFAULT_DAC 0x8000
	#define AD536x_DEFAULT_OFFSET 0x8000
	#define AD536x_DEFAULT_GAIN 0xFFFF
	#define AD536x_DEFAULT_GLOBALOFFSET 0x2000
	
	#define AD536x_DEFAULT_MAX 0xFFFF
	#define AD536x_DEFAULT_MIN 0x0000
#endif

//! AD5361 DAC
#ifdef AD536x_AD5361
	// note, in 2x banks
	#define AD536x_MAX_CHANNELS 8
	#define AD536x_RESOLUTION 14
	#define AD536x_DATA_MASK 0x3FFF
	#define AD536x_CH_MASK 0x07
	#define AD536x_14BIT
	
	#define AD536x_DEFAULT_DAC 0x2000
	#define AD536x_DEFAULT_OFFSET 0x2000
	#define AD536x_DEFAULT_GAIN 0x3FFF
	#define AD536x_DEFAULT_GLOBALOFFSET 0x2000
	
	#define AD536x_DEFAULT_MAX 0x3FFF
	#define AD536x_DEFAULT_MIN 0x0000
#endif

//! AD5362 DAC
#ifdef AD536x_AD5362
	// note, in 2x banks
	#define AD536x_MAX_CHANNELS 4
	#define AD536x_RESOLUTION 16
	#define AD536x_DATA_MASK 0xFFFF
	#define AD536x_CH_MASK 0x03	
	
	#define AD536x_DEFAULT_DAC 0x8000
	#define AD536x_DEFAULT_OFFSET 0x8000
	#define AD536x_DEFAULT_GAIN 0xFFFF
	#define AD536x_DEFAULT_GLOBALOFFSET 0x2000
	
	#define AD536x_DEFAULT_MAX 0xFFFF
	#define AD536x_DEFAULT_MIN 0x0000
#endif

//! AD5363 DAC
#ifdef AD536x_AD5363
	// note, in 2x banks
	#define AD536x_MAX_CHANNELS 4
	#define AD536x_RESOLUTION 14
	#define AD536x_DATA_MASK 0x3FFF
	#define AD536x_CH_MASK 0x03
	#define AD536x_14BIT
	
	#define AD536x_DEFAULT_DAC 0x2000
	#define AD536x_DEFAULT_OFFSET 0x2000
	#define AD536x_DEFAULT_GAIN 0x3FFF
	#define AD536x_DEFAULT_GLOBALOFFSET 0x2000
	
	#define AD536x_DEFAULT_MAX 0x3FFF
	#define AD536x_DEFAULT_MIN 0x0000
#endif



//Definition of constants and AD536x registers
/*

AD536x (AD5361) has a 24 bit instruction/data frame organized like this:

23	22	21	20	19	18	17	16	15	14	13	12	11	10	9	8	7	6	5	4	3	2	1		0
M1	M0	A5	A4	A3	A2	A1	A0	D15	D14	D13	D12	D11 D10 D9	D8	D7	D6	D5	D4	D3	D2	D1(0)	D0(0)

*/

// first two control bits M1, M0
// NOTE: since int type (default interpretation of, eg, "3" << 22) is 16 bit in ATMEGA chips, make sure
// you explicitly indicate unsigned long by 3UL (unsigned long is 32-bit variable type). Otherwise,
// Bad Things (TM).
#define AD536x_SPECIAL_FUNCTION 0
#define	AD536x_WRITE_DAC 		3UL << 22
#define AD536x_WRITE_OFFSET 	2UL << 22
#define	AD536x_WRITE_GAIN 		1UL << 22

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

#define AD536x_BANK0		1UL << 19
#define AD536x_BANK1		2UL << 19
#define AD536x_ALL_DACS		0
#define AD536x_ALL_BANK0	1UL << 16
#define AD536x_ALL_BANK1	2UL << 16


// alternative format for addressing bank 1 or 0
#define AD536x_BANK(bank)   (bank+1)UL << 19

// specific channel, if desired.
#define AD546x_CH0		0
#define AD546x_CH1		1UL << 16
#define AD546x_CH2		2UL << 16
#define AD546x_CH3		3UL << 16
#define AD546x_CH4		4UL << 16
#define AD546x_CH5		5UL << 16
#define AD546x_CH6		6UL << 16
#define AD546x_CH7		7UL << 16

// When AD536x_SPECIAL_FUNCTION is used it is possibile to give complex commands
// including reading from registers!
// In Special function mode the frame is organized in this way:

// 23	22	21	20	19	18	17	16	15	14	13	12	11	10	9	8	7	6	5	4	3	2	1	0
//  0	 0	S5	S4	S3	S2	S1	S0	F15	F14	F13	F12	F11 F10 F9	F8	F7	F6	F5	F4	F3	F2	F1	F0

#define AD536x_NOP	0
#define AD536x_WR_CR 1UL << 15 //Write on control register
	//Let's set the FLAGS for the control register
	//Flags can be combined!!!
	#define AD536x_X1B 4
	#define AD536x_X1A 0
	#define AD536x_T_SHTDWN_EN 2
	#define AD536x_T_SHTDWN_DIS 0	
	#define AD536x_SOFT_PWR_UP 1	
	#define AD536x_SOFT_PWR_DWN 0	
#define AD536x_WRITE_OFS0 2UL << 16 //Writes in the OFFSET 0 ANALOG DAC. the data is a 14 bit variable
#define AD536x_WRITE_OFS1 3UL << 16 //Writes in the OFFSET 1 ANALOG DAC. the data is a 14 bit variable


/***********************************************
 these all might be wrong..... check bit shifts before using!!
************************************************/
/*
#define AD536x_READ_REG 5UL << 15 //Select which register to read
	//This is the set of commends that select a particular register
	#define AD536x_READ_X1A(channel) 0|(channel+8)<<6
	#define AD536x_READ_X1B(channel) (1UL << 12)|(channel+8)UL << 6
	#define AD536x_READ_C(channel) (2UL << 12)|(channel+8)UL << 6
	#define AD536x_READ_M(channel) (3UL << 12)|(channel+8)UL << 6
	#define AD536x_READ_CR (3UL << 12)|(1UL << 6) //Read the control register: my favorite!
		//Flags defined for register Writing can be used for interrogation of the state
		//In addition the following flags can be used for read-only interrogations
		#define AD536x_CR_OVERTEMP 16 
		#define AD536x_CR_PEC 8
	#define AD536x_READ_OFS0 (3UL << 12)|(2UL << 6)						 
	#define AD536x_READ_OFS1 (3UL << 12)|(3UL << 6)							 
	#define AD536x_READ_AB_0 (3UL << 12)|(6UL << 6)
	#define AD536x_READ_AB_1 (3UL << 12)|(7UL << 6)	 
	#define AD536x_READ_GPIO (3UL << 12)|(11UL << 6) // F6 to F0 SHOULD be 0

// F7 to F0 select registers X2A or X2B for bank 0 A is0 and B is 1
#define AD536x_WRITE_AB_SELECT_0 6UL << 15 
#define AD536x_WRITE_AB_SELECT_0 11UL << 15 // F7 to F0 select registers X2A or X2B for bank 0 A is0 and B is 1
#define AD536x_BLOCK_WRITE_AB_SELECT 19UL << 15 // Block write AB
#define AD536x_MON 	12UL << 15 // Additional monitor commands specified below
	#define AD536x_CMD_MON_ENABLE 1UL << 4
	#define AD536x_CMD_MON_DISABLE 1UL << 4
	
	// pin can be 0 or 1 and selects the input pin from MON_IN0 or MON_IN1			
	#define AD536x_CMD_MON_IN_PIN_SEL(pin) (1UL << 4)|pin 					 
	
	// F3:F0 selects the input channel. If a number greater than 15 is used will cause errors!
	#define AD536x_CMD_MON_DAC_CH_SEL(channel) channel 

// F1=1 sets the GPIO as output F1=0 sets GPIO as input F0 contains the status.
#define	AD536x_WRITE_GPIO 13UL << 15 
*/

// register types
enum AD536x_reg_t { DAC, OFFSET, GAIN };

// Bank types
enum AD536x_bank_t { BANK0, BANK1, BANKALL };

// channel types
enum AD536x_ch_t { CH0, CH1, CH2, CH3, CH4, CH5, CH6, CH7, CHALL };


// library interface description
class AD536x
{

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


	//! Get DAC value
	/*!
		bank: BANK0, BANK1, or BANKALL
  		ch: CH0 .. CH7 (or .. CH3), or CHALL for all channels.
  		
  		Gets locally stored DAC value for given channel.
  		
  		See: writeDAC
	*/
    unsigned int getDAC(AD536x_bank_t bank, AD536x_ch_t ch);
    

	//! Write Offset trim value
	/*!
		bank: BANK0, BANK1, or BANKALL
  		ch: CH0 .. CH7 (or .. CH3), or CHALL for all channels.
  		data: unsigned int data payload. See datasheet for details.
  		
  		See: getOffset
	*/	
	void writeOffset(AD536x_bank_t bank, AD536x_ch_t ch, unsigned int data);


	//! Get Offset trim value.
	/*! 
		bank: BANK0, BANK1, or BANKALL
  		ch: CH0 .. CH7 (or .. CH3), or CHALL for all channels. 
		
		See: writeOffset
	*/
	unsigned int getOffset(AD536x_bank_t bank, AD536x_ch_t ch);
	
		
	//! Write Gain trim value
	/*!
		bank: BANK0, BANK1, or BANKALL
  		ch: CH0 .. CH7 (or .. CH3), or CHALL for all channels.
  		data: unsigned int data payload. See datasheet for details.
  		
  		See: getGain
	*/	
	void writeGain(AD536x_bank_t bank, AD536x_ch_t ch, unsigned int data);


	//! Get gain trim value.
	/*! 
		bank: BANK0, BANK1, or BANKALL
  		ch: CH0 .. CH7 (or .. CH3), or CHALL for all channels.
		
		See: writeGain
	*/
	unsigned int getGain(AD536x_bank_t bank, AD536x_ch_t ch);
	
    
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


	//! Set maximum DAC tuning word allowable
	/*!
		bank: BANK0, BANK1, or BANKALL
  		ch: CH0 .. CH7 (or .. CH3), or CHALL for all channels.
  		data: DAC tuning word
  		
  		This function sets an internal (software) limit on the max voltage
  		commanded by the DAC.
  		
  		See: writeDAC, setMinDAC
	*/
	void setMaxDAC(AD536x_bank_t bank, AD536x_ch_t ch, unsigned int data);
	
	
	//! Set minimum DAC tuning word allowable
	/*!
		bank: BANK0, BANK1, or BANKALL
  		ch: CH0 .. CH7 (or .. CH3), or CHALL for all channels.
  		data: DAC tuning word
  		
  		This function sets an internal (software) limit on the min voltage
  		commanded by the DAC.
  		
  		See: writeDAC, setMaxDAC
	*/
	void setMinDAC(AD536x_bank_t bank, AD536x_ch_t ch, unsigned int data);


	//! Set maximum DAC voltage allowable
	/*!
		bank: BANK0, BANK1, or BANKALL
  		ch: CH0 .. CH7 (or .. CH3), or CHALL for all channels.
  		voltage: double-precision voltage value.
  		
  		This function sets an internal (software) limit on the max voltage
  		commanded by the DAC.
  		
  		See: setVoltage, setMinVoltage
	*/
	void setMaxVoltage(AD536x_bank_t bank, AD536x_ch_t ch, double voltage);
	
	
	//! Set minimum DAC voltage allowable
	/*!
		bank: BANK0, BANK1, or BANKALL
  		ch: CH0 .. CH7 (or .. CH3), or CHALL for all channels.
  		voltage: double-precision voltage value.
  		
  		This function sets an internal (software) limit on the min voltage
  		commanded by the DAC.
  		
  		See: setVoltage, setMaxVoltage
	*/
	void setMinVoltage(AD536x_bank_t bank, AD536x_ch_t ch, double voltage);
	

	
    //! Issue an IO update
    /*!
    	Must call IO update manually if using writeHold.
    */
    void IOUpdate();


	//! Reset DAC. See datasheet for details.
	/*!
		Note, this also resets the max/min values.
		
		See: _max, _min, setMaxDAC, setMinDAC, setMaxVoltage, setMinVoltage
	*/
	void reset();
	
	//! assert ~CLR. See datasheet for details.
	void assertClear(int state);



	//! Change global offset DAC for given bank
	/*!
		Writes to registers OFS0 or OFS1, depending on bank.
		
		bank: BANK0 or BANK1
		data: 14-bit offset word
		
		When you write the global offset for a given bank, make
		sure to call assertClear before doing so, as per datasheet.
		
		For example, to re-zero the DAC to unipolar output, do 
		something like
		
		dac.assertClear(0);
		dac.writeGlobalOffset(BANK0, 0x00);
		dac.writeDAC(BANK0, CHALL, 0x00);
		dac.assertClear(1);
		
		This way, you don't glitch to the (non-zero) default DAC code, 
		and instead stay at zero potential until you're ready to update
		with a new value.
		
		See datasheet for more info.
	*/
	void writeGlobalOffset(AD536x_bank_t bank, unsigned int data);

	//! Get local value for global offset.
	/*!
		bank: BANK0 or BANK1
		
		returns 14-bit offset word.
	*/
	unsigned int getGlobalOffset(AD536x_bank_t bank);


	//! Informs library about the global reference voltage for a given bank.
	/*!
		bank: BANK0 or BANK1
		voltage: double-precision voltage value
		
		Defaults to 5.0 volts. This is only used by the library to calculate
		voltage <-> DAC tuning word conversions.
		
		See: getGlobalVref
	*/
	void setGlobalVref(AD536x_bank_t bank, double voltage);


	//! Get internally set reference voltage for a given bank.
	/*!
		bank: BANK0 or BANK1
		
		Gets internally stored vref value for given bank.
		
		See: setGlobalVref
	*/
	double getGlobalVref(AD536x_bank_t bank);


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
  	/*!
  		defaults to 5.0 volts.
  		
  		See: setGlobalVref
  	*/
  	double _vref[2];
  	
  	//! Maximum allowed DAC values
  	unsigned int _max[2][AD536x_MAX_CHANNELS];
  	
  	//! Minimum allowed DAC values
  	unsigned int _min[2][AD536x_MAX_CHANNELS];
  	
  	//! Private implementation to write DAC registers.
  	/*!
		reg: DAC, OFFSET, or GAIN
		bank: BANK0, BANK1, or BANKALL
  		ch: CH0 .. CH7 (or .. CH3), or CHALL for all channels.
  		data: 14 or 16 bit data payload.
  		
  		For public interface, use writeDAC or writeDACHold.
  		
  	*/
	void write(AD536x_reg_t reg, AD536x_bank_t bank, AD536x_ch_t ch, unsigned int data);
	
	//! Calculate a DAC tuning word based on desired voltage.
	/*!
		Uses the transfer function (in reverse):
		
		VOUT = 4*VREF*(DAC_CODE/2^16 - OFFSET_CODE/2^14)
  		DAC_CODE = data*(M+1)/2^16 + (C - 2^15)
  		
  		See: writeDAC, writeOffset, writeGain, setVoltage
	*/
	unsigned int voltageToDAC(AD536x_bank_t bank, AD536x_ch_t ch, double voltage);
	
	//! Calculate a voltage based on a DAC tuning word
	/*!
		Uses the transfer function:
				
		VOUT = 4*VREF*(DAC_CODE/2^16 - OFFSET_CODE/2^14)
  		DAC_CODE = data*(M+1)/2^16 + (C - 2^15)
  		
  		See: writeDAC, writeOffset, writeGain, setVoltage
  	*/
	double dacToVoltage(AD536x_bank_t bank, AD536x_ch_t ch, unsigned int data);
  
  	//! Validates new DAC value against _max and _min for given channel.
  	/*!
  		bank: BANK0, BANK1, or BANKALL
  		ch: CH0 .. CH7 (or .. CH3), or CHALL for all channels.
  		data: DAC value.
  		
  		Returns 1 if valid, 0 if invalid (outside range).
  		
  		Note, validation must be turned on by defining AD536x_VALIDATE
  		before including the AD536x library, eg,
  		
  			#define AD536x_VALIDATE
  			#include "AD536x.h"
  	*/
  	int validateData(AD536x_bank_t bank, AD536x_ch_t ch, unsigned int data);
  
};



#endif