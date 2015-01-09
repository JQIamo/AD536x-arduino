/*
   AD536x.cpp - AD536x family DAC control library for Arduino.
   
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

*/

#include "AD536x.h"

// Constructor
// some parameters related to the particular hardware implementation

//The physical pin used to enable the SPI device
//#define SPI_DEVICE 4

//The clock frequency for the SPI interface
//#define AD536x_CLOCK_DIVIDER_WR SPI_CLOCK_DIV2
//#define AD536x_CLOCK_DIVIDER_RD SPI_CLOCK_DIV4 //that (Assuming and Arduino clocked at 80MHz will set the clock of the SPI to 20 MHz
											   //AD536x can operate to up to 50 MHz for write operations and 20MHz for read operations.



// constructor...
AD536x::AD536x(int cs, int clr, int ldac, int reset)
{

	_sync = cs;
	_clr = clr;
	_ldac = ldac;
	_reset = reset;
	
	
	// Default to 5V reference... can change with setGlobalVref[bank]
	_vref[0] = 5.0;
	_vref[1] = 5.0;
	

	
	/*
	// make pins output, and initialize to startup state
	pinMode(_sync, OUTPUT);
	pinMode(_clr, OUTPUT);
	pinMode(_ldac, OUTPUT);
	pinMode(_reset, OUTPUT);
	
	digitalWrite(_sync, HIGH);
	digitalWrite(_ldac, HIGH);
	digitalWrite(_clr, HIGH);
	digitalWrite(_reset, HIGH);
	
	AD536x::reset();
	
	SPI.begin();
	
	// want to do this better long-term; AD536x can handle 50MHz clock though.
	SPI.setClockDivider(SPI_CLOCK_DIV2);
    SPI.setBitOrder(MSBFIRST);
  	SPI.setDataMode(SPI_MODE1);*/

}


// Public Methods
/*********************************************/


/**************************
		DAC funcs
***************************/
void AD536x::writeDAC(AD536x_bank_t bank, AD536x_ch_t ch, unsigned int data){
	AD536x::write(DAC, bank, ch, data);
	AD536x::IOUpdate();	
}

void AD536x::writeDACHold(AD536x_bank_t bank, AD536x_ch_t ch, unsigned int data){
	AD536x::write(DAC, bank, ch, data);
}

unsigned int AD536x::getDAC(AD536x_bank_t bank, AD536x_ch_t ch){
	return _dac[bank][ch];
}

void AD536x::setVoltage(AD536x_bank_t bank, AD536x_ch_t ch, double voltage){
	unsigned int data = AD536x::voltageToDAC(bank, ch, voltage);
	AD536x::writeDAC(bank, ch, data);
}

void AD536x::setVoltageHold(AD536x_bank_t bank, AD536x_ch_t ch, double voltage){
	unsigned int data = AD536x::voltageToDAC(bank, ch, voltage);
	AD536x::writeDACHold(bank, ch, data);
}

/**************************
		Offset funcs
***************************/
void AD536x::writeOffset(AD536x_bank_t bank, AD536x_ch_t ch, unsigned int data){
	AD536x::write(OFFSET, bank, ch, data);
	AD536x::IOUpdate();
}

unsigned int AD536x::getOffset(AD536x_bank_t bank, AD536x_ch_t ch){
	return _offset[bank][ch];
}


/**************************
		Gain funcs
***************************/
void AD536x::writeGain(AD536x_bank_t bank, AD536x_ch_t ch, unsigned int data){
	AD536x::write(OFFSET, bank, ch, data);
	AD536x::IOUpdate();
}

unsigned int AD536x::getGain(AD536x_bank_t bank, AD536x_ch_t ch){
	return _gain[bank][ch];
}



/**************************
		Misc funcs
***************************/
void AD536x::IOUpdate(){
	digitalWrite(_ldac, LOW);
	// delay(1);
	digitalWrite(_ldac, HIGH);
}

void AD536x::reset(){
	digitalWrite(_reset, LOW);
	//delay(1);
	digitalWrite(_reset, HIGH);
	
	// reset DAC, OFFSET, GAIN to default values
	for (int c = 0; c < AD536x_MAX_CHANNELS; c++){
		_dac[0][c] = AD536x_DEFAULT_DAC;
		_dac[1][c] = AD536x_DEFAULT_DAC;
		
		_offset[0][c] = AD536x_DEFAULT_OFFSET;
		_offset[1][c] = AD536x_DEFAULT_OFFSET;
		
		_gain[0][c] = AD536x_DEFAULT_GAIN;
		_gain[1][c] = AD536x_DEFAULT_GAIN;
		
		_globalOffset[0] = AD536x_DEFAULT_GLOBALOFFSET;
		_globalOffset[1] = AD536x_DEFAULT_GLOBALOFFSET;
		
		// resets max/min boundaries.
		_max[0][c] = AD536x_DEFAULT_MAX;
		_min[0][c] = AD536x_DEFAULT_MIN;
		
		_max[1][c] = AD536x_DEFAULT_MAX;
		_min[1][c] = AD536x_DEFAULT_MIN;
	}
}

void AD536x::assertClear(int state){
	switch (state){
		case 1:
			digitalWrite(_clr, HIGH);
			break;
		case 0:
			digitalWrite(_clr, LOW);
			break;
		default:
			break;
	}
}

void AD536x::writeGlobalOffset(AD536x_bank_t bank, unsigned int data){
	
	unsigned long cmd = 0;
	data = data & 0x3FFF; 	// 14-bit mask
	switch (bank) {
		case BANK0:
			cmd = (cmd | AD536x_WRITE_OFS0 | data);
			_globalOffset[0] = data;
			break;
		case BANK1:
			cmd = (cmd | AD536x_WRITE_OFS1 | data);
			_globalOffset[1] = data;
			break;
		default:
			// bad bank; return early
			return;
	}
			
	
	// Make sure you assert clear while adjusting range to avoid glitches
	// see datasheet
	
	// AD536x::assertClear(0);
	AD536x::writeCommand(cmd);
	//AD536x::assertClear(1);

}

unsigned int AD536x::getGlobalOffset(AD536x_bank_t bank){
	return _globalOffset[bank];
}


void AD536x::setGlobalVref(AD536x_bank_t bank, double voltage){
	switch (bank) {
		case BANK0:
			_vref[0] = voltage;
			break;
		case BANK1:
			_vref[1] = voltage;
			break;
		default:
			break;
	}
}

double AD536x::getGlobalVref(AD536x_bank_t bank){
	return _vref[bank];
}



void AD536x::writeCommand(unsigned long cmd){

	digitalWrite(_sync, LOW);
	
	// write MSBFIRST
	SPI.transfer((cmd >> 16) & 0xFF);
	SPI.transfer((cmd >> 8) & 0xFF);
	SPI.transfer(cmd & 0xFF);
	
	digitalWrite(_sync, HIGH);
}



// Private Methods
/*********************************************/


void AD536x::write(AD536x_reg_t reg, AD536x_bank_t bank, AD536x_ch_t ch, unsigned int data){

	data = data & AD536x_DATA_MASK; 	// bitmask ensure data has proper 
									 	// resolution
	
	unsigned int payload;
	
	// if 14 bit DAC, coerce to right form
	#ifdef AD536x_14BIT
		payload = (data << 2) & 0xFFFF;
	#else
		payload = data;
	#endif
	
	// pointer for where to store channel data 
	// for reference.
	// see: http://stackoverflow.com/questions/21488179/c-how-to-declare-pointer-to-2d-array
	unsigned int  (*localData)[2][AD536x_MAX_CHANNELS]; 	
								
	
	unsigned long cmd = 0;		// var for building command.
		
	// for validation purposes...
	// don't want to validate for entire bank writes.
	// (in the sense that it's too costly)		
	#ifdef AD536x_VALIDATE
		bool singleCh = false;
	#endif
	
	
	// add register header M1, M0	
	switch (reg) {
		case DAC:
			cmd = cmd | AD536x_WRITE_DAC;
			localData = &_dac;
			break;
		case OFFSET:
			cmd = cmd | AD536x_WRITE_OFFSET;
			localData = &_offset;
			break;
		case GAIN:
			cmd = cmd | AD536x_WRITE_GAIN;
			localData = &_gain;
			break;
		default:
			// bad register; return early.
			// might want to notify user...???
			return;
	}
	
	


	// check to make sure channel in range, if not, return early.
	if (ch > AD536x_MAX_CHANNELS && ch != CHALL){
		return;
	}
	
	
	if (ch == CHALL){
		// if writing all channels, figure out which bank to address
		// and update local reference data.
		switch (bank){
			case BANK0:
				cmd = cmd | AD536x_ALL_BANK0;
				for (int c = 0; c < AD536x_MAX_CHANNELS; c++){
					(*localData)[0][c] = data;
				}
				break;
			
			case BANK1:
				cmd = cmd | AD536x_ALL_BANK1;
				for (int c = 0; c < AD536x_MAX_CHANNELS; c++){
					(*localData)[0][c] = data;
				}
				break;
			
			case BANKALL:
				// all banks, all channels
				// address bits are zero, so do nothing to cmd.
			    for (int c = 0; c < AD536x_MAX_CHANNELS; c++){
					(*localData)[0][c] = data;
					(*localData)[1][c] = data;
				}
				break;
				
			default:
				// not valid address, so return early
				// note, no way to (natively) address, eg, 
				// BANKALL, CH2
				return;
		}
	} else {
		
		// else, write particular bank/channel
		switch (bank){
			case BANK0:
				cmd = cmd | AD536x_BANK0 | (ch << 16);
				(*localData)[0][ch] = data;
				
				#ifdef AD536x_VALIDATE
					singleCh = true;
				#endif
				
				break;
			
			case BANK1:
				cmd = cmd | AD536x_BANK1 | (ch << 16);
				(*localData)[1][ch] = data;
				
				#ifdef AD536x_VALIDATE
					singleCh = true;
				#endif
				
				break;
			
			default:
				// not valid address
				// return early
				return;
		}
	}
	
	#ifdef AD536x_VALIDATE
		// if writing a single channel, validate your data.
		if(singleCh){
			int valid = AD536x::validateData(bank, ch, data);
			
			// if data out of range, return early
			// not sure how to best notify user of this.
			if (valid != 1){
				return;
			}
		}
	#endif
	 
	// update command with data packet, and write to dac.
	cmd = cmd | payload;
	AD536x::writeCommand(cmd);
}

//fix...
unsigned int AD536x::voltageToDAC(AD536x_bank_t bank, AD536x_ch_t ch, double voltage){
	  
	/* 	
		Transfer function:	
		VOUT = 4*VREF*(DAC_CODE/2^16 - OFFSET_CODE/2^14)
  		DAC_CODE = data*(M+1)/2^16 + (C - 2^15)
  	*/
  	/*
    double m = _gain[bank][ch]/(2.0^16);
    double c = _offset[bank][ch];
	double offset = _globalOffset[bank];
	
	unsigned int dac_code;
	
	//dac_code = (unsigned int)(voltage/(4*_vref[bank])
	*/
	return 0;

}

double AD536x::dacToVoltage(AD536x_bank_t bank, AD536x_ch_t ch, unsigned int data){
	return 0.0;
}


int AD536x::validateData(AD536x_bank_t bank, AD536x_ch_t ch, unsigned int data){
	unsigned int max = _max[bank][ch];
	unsigned int min = _min[bank][ch];
	
	if (data <= max && data >= min){
		return 1;
	} else {
		return 0;
	}
}

