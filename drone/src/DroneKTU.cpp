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
#include <math.h>

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
#include "Modem.h"

#define DEFAULT_PENDRIVE "/media/ubuntu/4CE4-CD324/results/"

using namespace std;
using namespace DJI;
using namespace DJI::onboardSDK;

inline double to_degrees(double radians);
void measure(CoreAPI* api, Flight* flight, int fd, PositionData p, Modem myModem);

int main(int argc,char* argv[])
{
	//Managing the protoboard
   	Protoboard myProto; 
	myProto.MyLed.LedOn();
	std::cout << "|---------------------------------------------------------------------------|"<< std::endl;
	std::cout << "|------------------DroneKTU. Copyright (C) 2017 Alvaro Zornoza--------------|"<< std::endl;
	std::cout << "|---------------------------------------------------------------------------|"<< std::endl;
	std::cout<<""<<std::endl<<std::endl;

	//Starting the process	
	while(1) // Although is a bad programming practice, in the only way since I do not have a screen during the flight.
	{
		std::cout << "|---------------------------------------------------------------------------|" << std::endl;
		std::cout << "|-Please press the button to run the process or Ctrl+C to close the program-|" << std::endl;
		std::cout << "|---------------------------------------------------------------------------|" << std::endl;
		while(!(myProto.MyButton.ButtonStatus()))
		{
			usleep(500000);
		}

		//Managing the connection with M100
		LinuxSerialDevice* serialDevice = new LinuxSerialDevice(UserConfig::deviceName, UserConfig::baudRate);
		CoreAPI* api = new CoreAPI(serialDevice);
		Flight* flight = new Flight(api);
		WayPoint* waypointObj = new WayPoint(api);
		Camera* camera = new Camera(api);
		LinuxThread read(api, 2);
		sleep(2);

		//Managing the telemetry data
		PositionData p;

		//Managing the modem
		Modem myModem;

		// Setup
		int setupModem = myModem.begin();
		if (setupModem = 0)
		{
			std::cout << "ERROR: It was not possible to connect with the Modem SIM800L. This program will exit now. \n"; //The led blinks twice to show the error.
			myProto.MyLed.LedBlink(2);
			continue;
		}

		int setupStatus = setup(serialDevice, api, &read);
		if (setupStatus == -1)
		{
			std::cout << "ERROR: It was not possible to connect with the Matrice 100.This program will exit now. \n"; //The led blinks four times to show the error.
			myProto.MyLed.LedBlink(4);
			continue;
		}

		//Set broadcast Freq Defaults
		unsigned short broadcastAck = api->setBroadcastFreqDefaults(1);
	  	usleep(500000);

		//Managing local time for file name
		time_t tiempo = time(0);
		struct tm *tlocal = localtime(&tiempo);
		char output[128];
		strftime(output, 128, "%d-%m-%y_%H.%M.%S", tlocal);
	
		//Managing the opening of the file
		int fd;
		char buf[100];
		sprintf(buf,"%s%s",DEFAULT_PENDRIVE,output);
		fd=open(buf,O_WRONLY|O_CREAT|O_TRUNC,0666);
		if(fd<0)
		{
			std::cout << "ERROR: It was not possible to create to the file or access the pendrive. This program will exit now. \n"; //The led blinks six times to show the error.
			myProto.MyLed.LedBlink(6);
			continue;
		}
	
		//Main process
		char intro[1000];
		//Measuring time, langitude and longitude, with the drone disarmed
		p=flight->getPosition();
		sprintf(intro,"|------------------------------------------------|\n|-DroneKTU. Copyright (C) 2017 Alvaro Zornoza----|\n|------------------------------------------------|\n");
		write(fd,intro,strlen(intro));
		printf("%s",intro);
		//|-Time:%lf --------------------------------------|\n
		sprintf(intro,"|-DroneKTU v2.0 experiment-----------------------|\n|-Latitude:%lf ----------------------------------|\n|-Longitude:%lf ---------------------------------|\n |------------------------------------------------|\n",to_degrees(p.latitude),to_degrees(p.longitude));
		write(fd,intro,strlen(intro));
		printf("%s",intro);
		std::cout <<  "|-Safety time before taking off (10s countdown)--|" << std::endl;
		for(int i=10;i>0;i--)
		{
			std::cout<<i<<" seconds\n"<< std::endl;
			myProto.MyLed.LedOn();
			usleep(500000);
			myProto.MyLed.LedOff();
			usleep(500000);
		}
		//measure(api,flight,fd,p,myModem);
		monitoredTakeoff(api, flight);
		usleep(5000000);
		for(int i=0;i<5;i++)
		{
			std::cout<<"Ascending to "<<5*(i+1)<<"meters/n"<< std::endl;
			moveByPositionOffset(api, flight, 0, 0, 5*(1.033), 0);
			usleep(10000000);
			measure(api,flight,fd,p,myModem);	
		}
		landing(api,flight);
		
		

		/*while(1)
		{	
			int counter=0;
			char cadcs[100];
			p=flight->getPosition();
			sprintf(cadcs,"%lf;%lf;%lf;%lf;%i;%i\n",to_degrees(p.latitude),to_degrees(p.longitude),p.altitude,p.height,p.health,myModem.getSignalQuality());
			printf("%s",cadcs);  //At the same time, the result is shown in the screen and recorded in a file.
			write(fd,cadcs,strlen(cadcs));
			usleep(500000);
			if((myProto.MyButton.ButtonStatus()))   //Trying to avoid gathering
			{
				for(int i=0;i<10;i++)
				{
					usleep(100000);
					if((myProto.MyButton.ButtonStatus()))
						counter++;
					else		
						continue;
				}
				if(counter==10)
					break;
				
			}
			else
				continue;
		}
		*/
		//Managing the closing of the file
		close(fd);

		//Managing desconecction with Matrice 100
		int cleanupStatus = cleanup(serialDevice, api, flight, &read);
		if (cleanupStatus == -1)
		{
			std::cout << "Unable to cleanly destroy OSDK infrastructure. There may be residual objects in the system memory.\n";
			return 0;
		}
		std::cout << "Program exited successfully." << std::endl;
	
		sleep(2);

	}
}

inline double to_degrees(double radians)
{
	return radians*(180.0/M_PI);
}

void measure(CoreAPI* api, Flight* flight, int fd, PositionData p, Modem myModem)
{	
	cout<<"|------------Measuring---------------------------|"<<endl;
	int i=0,j=0;
	char cadcs[100];
	int signal[20];
	double height[20];
	int signal_sum=0,signal_average=0;
	double height_sum=0,height_average=0;
	for(i=0;i<20;i++)
	{	
		p=flight->getPosition();
		height[i]=p.height;
		signal[i]=myModem.getSignalQuality();
		cout<<i<<"/20"<<endl;
		usleep(500000);	
	}
	for(j=0;j<20;j++)
	{	
		height_sum+=height[j];
		signal_sum+=signal[j];		
	}
	height_average=height_sum/20.0;
	signal_average=signal_sum/20;
	sprintf(cadcs,"%lf;%i\n",height_average,signal_average);
	printf("%s",cadcs);
	write(fd,cadcs,strlen(cadcs));
}
