// Led.cpp: implementation of the Led class.
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
////////////////////////////////////////////////////////////////////////


#include <iostream>
#include <string>
#include <cstring>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>

#include "Led.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


Led::Led(int gpio):gpio_port(gpio)
{
	char buf[100];
	sprintf(buf,"/sys/class/gpio/gpio158/value");
	fd=open(buf, O_WRONLY);
}

Led::~Led()
{
	close(fd);
}

//////////////////////////////////////////////////////////////////////
// Methods
//////////////////////////////////////////////////////////////////////

/*int Led::LedStatus()
{
	char value;
	read(fd,&value,1)
	if(value
	{
		cout<<"LED is ON\n";
		return 1;	
	}
	else
	{
		cout<<"LED is OFF\n";
		return 0;
	}
}*/

void Led::LedOn()
{
	write(fd,"1",1);
}

void Led::LedOff()
{

	write(fd,"0",1);
}

void Led::LedBlink(int times, int time)
{
	for(int i=0;i<times;i++)
	{	
		LedOn();
		usleep(time/2);
		LedOff();
		usleep(time/2);
	}
}




