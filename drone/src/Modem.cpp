// Modem.cpp: implementation of the SIM800L class.
//
//////////////////////////////////////////////////////////////////////
//
// This file is part of DroneKTU, which is a Undergraduated degree 
// Final Project developed at Kaunas University of Technology (KTU)
// by Alvaro Zornoza, student of Degree in Industrial Electrical 
// Engineering and Automation.
//
// DroneKTU consists on an app developed in C/C++ to control RF 
// signal measurements with the embedded-system computer
// DJI Manifold attached to the drone DJI Matrice 100
//
//////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2017 Alvaro Zornoza
//
// This app included some libraries and code from DJI. DJI is the 
// only owner of next subfolders and their respective files:
// /contrib, /osdk-core, /osdk-wrapper, /platform
//
// DroneKTU is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published 
// by the Free Software Foundation, either version 3 of the License, 
// or (at your option) any later version.
//
// DroneKTU is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU,
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.If not, see <http://www.gnu.org/licenses/>.
//
//
//////////////////////////////////////////////////////////////////////

#include "Modem.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

using namespace std;

Modem::Modem()
{	
	begin();
}

Modem::~Modem()
{
	SIM.Close();
}

//////////////////////////////////////////////////////////////////////
// Methods
//////////////////////////////////////////////////////////////////////

void Modem::begin()
{
	SIM.Open("/dev/ttyS0");
	SIM.SetBaudRate(SerialStreamBuf::BAUD_115200);
	SIM.SetCharSize(SerialStreamBuf::CHAR_SIZE_8);
	SIM.SetNumOfStopBits(1);
	SIM.SetParity(SerialStreamBuf::PARITY_ODD);
	SIM.SetFlowControl(SerialStreamBuf::FLOW_CONTROL_HARD);
}

void Modem::getSignalQuality()
{
/*response 
+CSQ: <rssi>,<ber>Parameters
<rssi>
0 -115 dBm or less
1 -111 dBm or less
1...30 -110...-54 dBm
31 -52 dBm or greater
99 not known or not detectable
<ber> (in percent)
0...7 As RXQUAL values in the table in GSM 05.08 [20] subclause 7.2.4
99 not known or not detectable
*/

	char command[]="AT+CSQ\r\n";
	SIM.write(command,sizeof(command));
	const int BUFFER_SIZE=256;
	char input_buffer[BUFFER_SIZE];
	SIM.read(input_buffer,BUFFER_SIZE);
	//Serial
}


