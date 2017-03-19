// DroneKTU.cpp: implementation of the main file
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
// DroneKTU. Copyright (C) 2017 Alvaro Zornoza
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

//System Headers
#include <iostream>
#include <string>
#include <cstring>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>

//DJI Linux Application Headers
#include "LinuxSerialDevice.h"
#include "LinuxThread.h"
#include "LinuxSetup.h"
#include "LinuxCleanup.h"
#include "ReadUserConfig.h"
#include "LinuxMobile.h"
#include "LinuxFlight.h"
//#include "LinuxInteractive.h"
#include "LinuxWaypoint.h"
#include "LinuxCamera.h"

//DJI OSDK Library Headers
#include <DJI_Follow.h>
#include <DJI_Flight.h>
#include <DJI_Version.h>
#include <DJI_WayPoint.h>

//User's Headers

#include "Protoboard.h"

//using namespace std;

using namespace std;
using namespace DJI;
using namespace DJI::onboardSDK;

void clean_stdin(void);

int main(int argc,char* argv[])
{
	Protoboard Proto; 

	LinuxSerialDevice* serialDevice = new LinuxSerialDevice(UserConfig::deviceName, UserConfig::baudRate);
	CoreAPI* api = new CoreAPI(serialDevice);
	Flight* flight = new Flight(api);
	WayPoint* waypointObj = new WayPoint(api);
	Camera* camera = new Camera(api);
	LinuxThread read(api, 2);
	PositionData p;

	//Proto.MyLed.LedOn();

	std::cout<<"DroneKTU. Copyright (C) 2017 Alvaro Zornoza"<<std::endl<<std::endl;

	//! Setup
	int setupStatus = setup(serialDevice, api, &read);
	if (setupStatus == -1)
	{
	std::cout << "This program will exit now. \n";
	return 0;
	}

	//! Set broadcast Freq Defaults
	unsigned short broadcastAck = api->setBroadcastFreqDefaults(1);
  	usleep(500000);

	p=flight->getPosition();

	int cleanupStatus = cleanup(serialDevice, api, flight, &read);
	if (cleanupStatus == -1)
	{
		std::cout << "Unable to cleanly destroy OSDK infrastructure. There may be residual objects in the system memory.\n";
		return 0;
	}
	std::cout << "Program exited successfully." << std::endl;

	
	std::cout<<"Press enter to continue ..."<<std::endl;
	std::cin.get();
	return 0;
}

void clean_stdin(void)
{
	int c;
	do {
		c=getchar();
	} while ((c!='\n')&&(c!=EOF));
}


