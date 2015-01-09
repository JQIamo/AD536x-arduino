/*
   settings.h  - AD536x family DAC control library for Arduino. 
   
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

// Modify this file to get your settings correct...

// Which DAC are you using?
#define AD536x_AD5362

// uncomment the following line to validate DAC data ranges...
//#define AD536x_VALIDATE