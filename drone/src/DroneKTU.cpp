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
#include <time.h>

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

//User Headers

#include "Protoboard.h"

//using namespace std;

using namespace std;
using namespace DJI;
using namespace DJI::onboardSDK;

void clean_stdin(void);

int main(int argc,char* argv[])
{
	//Managing the connection with M100

	LinuxSerialDevice* serialDevice = new LinuxSerialDevice(UserConfig::deviceName, UserConfig::baudRate);
	CoreAPI* api = new CoreAPI(serialDevice);
	Flight* flight = new Flight(api);
	WayPoint* waypointObj = new WayPoint(api);
	Camera* camera = new Camera(api);
	LinuxThread read(api, 2);

	//Managing the telemetry data

	PositionData p;

	//Managing the attahed protoboard
   	Protoboard myproto; 

	//Proto.MyLed.LedOn();

	std::cout<<"DroneKTU. Copyright (C) 2017 Alvaro Zornoza"<<std::endl<<std::endl;
	

	// Setup
	int setupStatus = setup(serialDevice, api, &read);
	if (setupStatus == -1)
	{
	std::cout << "This program will exit now. \n";
	return 0;
	}

	//! Set broadcast Freq Defaults
	unsigned short broadcastAck = api->setBroadcastFreqDefaults(1);
  	usleep(500000);

	//Managing local time for file name
	time_t tiempo = time(0);
	struct tm *tlocal = localtime(&tiempo);
	char output[128];
	strftime(output, 128, "%d-%m-%y_%H.%M.%S", tlocal);
	//printf("Date and time (DD-MM-YYYY_HH.MM.SS): %s\n\n",output);
	
	
	int fd;
	char buf[100];
	sprintf(buf,"../../results/%s",output);
	fd=open(buf,O_WRONLY|O_CREAT|O_TRUNC,0666);
	if(fd<0)
		std::cout<<"Error creando el archivo..."<<std::endl;

	char cadcs[200];
	sprintf(cadcs,"Latitude;Longitude;Altitude;Height;Health\n");
	write(fd,cadcs,strlen(cadcs));
	for(int i=0;i<10;i++)
	{	
		p=flight->getPosition();
		sprintf(cadcs,"%f;%f;%f;%f;%d\n",p.latitude,p.longitude,p.altitude,p.height,p.health);
		write(fd,cadcs,strlen(cadcs));
		usleep(100);
		//std::cout<<"Error escribiendo en el archivo..."<<std::endl;
	}
		
	close(fd);

	/*std::cout<<"Please press the button to run the process..."<<std::endl;
	while(1)
	{
		if(!(myproto.MyButton.ButtonStatus()))
			break;
	}*/

	
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


