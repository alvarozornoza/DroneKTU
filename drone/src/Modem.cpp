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

struct antenna_c
{
	char cellid[5];
	char rxl[3];
};
struct antenna
{
	long cellid;
	int rxl;
}; 

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

using namespace std;

Modem::Modem()
{	
}

Modem::~Modem()
{
}

//////////////////////////////////////////////////////////////////////
// Methods
//////////////////////////////////////////////////////////////////////

int Modem::begin()
{
	int setup;
	setup=SIM.open_serial(DEFAULT_PI_SERIAL_PORT);
	
	char command[]="AT\r\n";
	SIM.write_port(command,strlen(command));
	usleep(100000);
	char command5[]="AT+CENG=3\r\n";
	SIM.write_port(command5,strlen(command5));
	usleep(100000);
	char command2[]="AT+COPS=2\r\n";
	SIM.write_port(command2,strlen(command2));
	usleep(2000000);
	char command3[]="AT+CLTS=1\r\n";
	SIM.write_port(command3,strlen(command3));
	usleep(100000);
	char command4[]="AT+COPS=0\r\n";
	SIM.write_port(command4,strlen(command4));
	usleep(3000000);

	if(setup)
		SIM.serial_init();
	return setup;
}

void Modem::finish()
{
	SIM.close_serial();
}

void Modem::getInfo1(struct antenna *antennas, int len)
{	
	// With AT+CENG it is possible to get RSSI from 7 antenna at the same antenna. The first one (CENG 0) is always the one that it is connected with and the most powerful.s
	struct antenna_c antennas_c[len];
	char command3[]="AT+CENG?\r\n";
	SIM.set_get_cmd(command3);
	int rv=strlen(SIM.respuesta);
	
	for (int i=0; i<len; i++)
	{
		int cont=0;
		for (int j=0; j<rv; j++)
		{	
			if(SIM.respuesta[j]==',')
			{			
				cont++;
				if(cont==(6*i+5))
				{	
					for(int k=0;k<4;k++)
						antennas_c[i].cellid[k]=SIM.respuesta[j+k+1];
					for(int z=0;z<2;z++)
						antennas_c[i].rxl[z]=SIM.respuesta[j+z+9];
				}
			}
		}
	}
	for (int j=0; j<len; j++)
	{
		char aux[2];
		for(int z=0;z<2;z++)
			aux[z]=antennas_c[j].rxl[z];
		antennas[j].rxl=(-113+atoi(aux));
	}	
	for (int j=0; j<len; j++)
	{	
		char aux[4];
		for(int z=0;z<4;z++)
		{
			aux[z]=antennas_c[j].cellid[z];
		}
		aux[4]='\0';	
		antennas[j].cellid=strtol(aux,NULL,16);
	}		
	
}

void Modem::getTime(char* time)
{
	
	char command5[]="AT+CCLK?\r\n";
	SIM.set_get_cmd(command5);
	int rv=strlen(SIM.respuesta);
	
	int i,j;
	char aux[18];
	char *pch;

	pch=strstr(SIM.respuesta,"+CCLK: ");
	int pos=pch-SIM.respuesta;

	for(j=0;j<17;j++)
	{
		aux[j]=SIM.respuesta[pos+j+8];
	}
	for(j=0;j<17;j++)
	{
		if(aux[j]=='/')	
			time[j]='-';
		else if(aux[j]==',')
			time[j]='_';
		else if(aux[j]==':')
			time[j]='.';
		else
			time[j]=aux[j];
	}
	time[17]='\0';
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

