// SerialInterface.cpp: interface for SerialInterface class.
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

#include <stdio.h>
#include <string.h>
#include <fcntl.h>   /* File Control Definitions           */
#include <termios.h> /* POSIX Terminal Control Definitions */
#include <unistd.h>  /* UNIX Standard Definitions 	   */ 
#include <errno.h>   /* ERROR Number Definitions           */

#ifndef INC_SERIALINTERFACE_H_
#define INC_SERIALINTERFACE_H_

class SerialInterface
{
public:
	SerialInterface(void);
	virtual ~SerialInterface(void);

	int fd;
	char* respuesta;
	
	int open_serial(char* serial_dev);
	int serial_init();
	int read_port(int ret);
	int write_port(char* cmd, int len);
	void set_get_cmd(char* cmd_str);
	void set_cmd(char* cmd, char * cmd_str);
	void close_serial(void);
};

#endif /* INC_SERIALINTERFACE_H_ */
