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

#include "SerialInterface.h"
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
	SIM.close_serial();
}

//////////////////////////////////////////////////////////////////////
// Methods
//////////////////////////////////////////////////////////////////////

int Modem::begin()
{
	int setup;
	setup=SIM.open_serial(DEFAULT_PI_SERIAL_PORT);
	if(setup)
		SIM.serial_init();
	return setup;
}

int Modem::getSignalQuality()
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
	char response[64];
	SIM.set_get_cmd(command);
	int rv=strlen(SIM.respuesta);
	int i=0;
	for(i=0; i<rv; i++){
		if((SIM.respuesta[i] >= '0' && SIM.respuesta[i] <= '9'))
			break;
	}
	int size=rv-10-i;
	char char_number[size];
	int j=0;
	for(j=0;j<size;j++)
	{
		char_number[j]=SIM.respuesta[i+j];
	}	
	int num;
	num=atoi(char_number);
	return(-112+num*2);
}

