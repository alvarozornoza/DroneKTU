// Button.cpp: implementation of the Led class.
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
#include <fstream>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>

#include "Button.h"

//int fd;	

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Button::Button(int gpio):gpio_port(gpio)
{
	char buf[100];
	sprintf(buf,"/sys/class/gpio%d/value",(this->gpio_port));
	fd=open(buf, O_RDONLY);
}


Button::~Button()
{
	close(fd);
}

//////////////////////////////////////////////////////////////////////
// Methods
//////////////////////////////////////////////////////////////////////	

int  Button::ButtonStatus()
{
	char value;	
	read(fd,&value,1);
	if(value == '0')
	{ 	
		return 0; // Current GPIO status low
	     
	}
	else
	{
		return 1; // Current GPIO status high
	}
}

