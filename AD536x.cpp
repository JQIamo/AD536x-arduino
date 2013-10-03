/*
   AD536x.cpp - AD536x family DAC control library 
   Created by Alessandro Restelli, 2013
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

// include core Arduino API
#include "Arduino.h"

// include this library's header
#include "Test.h"

// additional header files if used

//The AD536x uses SPI interface
#include "SPI.h"

// Constructor
// some parameters related to the particular hardware implementation

//The physical pin used to enable the SPI device
#define SPI_DEVICE 4

//The clock frequency for the SPI interface
#define AD536x_CLOCK_DIVIDER_WR SPI_CLOCK_DIV2
#define AD536x_CLOCK_DIVIDER_RD SPI_CLOCK_DIV4 //that (Assuming and Arduino clocked at 80MHz will set the clock of the SPI to 20 MHz
											   //AD536x can operate to up to 50 MHz for write operations and 20MHz for read operations.





AD536x::AD536x(void)
{
  /*not really sure what to put here
  It should be possibile to call the initialization of the SPI port by uncommenting the followingrow */

//initialize_SPI(void);

	
	
	
}






// Public Methods
// Function used by this sketch and external sketches 

AD536x::initialize_SPI(void)
{

//First let us chose the channel	
	SPI.begin(SPI_DEVICE);
	//Initialize the bus for a device on pin SPI_DEVICE
	SPI.setClockDivider(SPI_DEVICE,AD536x_CLOCK_DIVIDER_WR);
	SPI.setDataMode(SPI_DEVICE,SPI_MODE1);// this setting needs to be double checked.
	//Set between little endian and big endian notation
	SPI.setBitOrder(SPI_DEVICE,MSBFIRST);
	
}



void AD536x::write(void)
{

	
	
}


void AD53x::setVoltageWord(int bank, int channel, unsigned int voltageWord)
{
    unsigned long int command = (AD536x_WRITE_DAC | AD536x_BANK(bank) | AD536x_CHANNEL(channel) | voltageWord);
    //AD536x::write(command);
}


// Private Methods
// Functions used only in this library and not accessibile outside. 

//void AD536x::function(void)
{
  
}

