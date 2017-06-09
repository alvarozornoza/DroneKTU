// SerialInterface.cpp: implementation of the SerialInterface class.
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

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SerialInterface::SerialInterface(void)
{
}

SerialInterface::~SerialInterface(void)
{
}

//////////////////////////////////////////////////////////////////////
// Methods
//////////////////////////////////////////////////////////////////////

int SerialInterface::open_serial(char* serial_dev)
{
	fd=open(serial_dev,O_RDWR|O_NOCTTY|O_NDELAY);
	if (fd<0)
		return 0;
	return 1;
}


int SerialInterface::serial_init()
{
	struct termios termios;
	int ret = 0;

	ret = tcgetattr (fd, &termios);
	if(ret < 0){
	printf("tcgetattr failed\n");
	return -1;
	}

	cfsetispeed(&termios, B115200);
	cfsetospeed(&termios, B115200);

	termios.c_iflag &= ~(IXANY | IXON | IXOFF);
	termios.c_lflag &= ~(ICANON | ECHO);
	termios.c_cflag |= (CLOCAL | CREAD);

	termios.c_cc[VMIN]  = 1;
	termios.c_cc[VTIME] = 10;

	ret = tcsetattr (fd, TCSANOW, &termios);
	if(ret < 0){
		printf("tcsetattr failed\n");
		return -1;
	}
	return 0;
}


int SerialInterface::read_port(int ret)
{
	int rv = 0;
	char buf[500];
	respuesta=buf;
	if(ret)
		rv = read(fd, &buf, sizeof(buf));
	return rv;
}


int SerialInterface::write_port(char * cmd, int len)
{
	if(fd < 0){
		printf("Invalid fd\n");
		return -1;
	}

	int ret = 0;
	ret = write(fd, cmd, len);
	if(ret <= 0)
		printf("write failed\n");
	return ret;
}


void SerialInterface::set_get_cmd(char * cmd_str)
{
	int ret;
	char cmd[500];
	set_cmd(cmd, cmd_str);
	ret = write_port(cmd, strlen(cmd));
	usleep(100000);
	read_port(ret);	
}


void SerialInterface::set_cmd(char * cmd, char * cmd_str)
{
	memset(cmd, 0, sizeof(cmd));
    	strcpy(cmd, cmd_str);
}


void SerialInterface::close_serial(void)
{
	close(fd);
}
