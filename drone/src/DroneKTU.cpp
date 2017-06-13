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
#include "Measurement.h"

#define DEFAULT_PENDRIVE "/media/ubuntu/4CE4-CD325/results/"
#define altitude1 100
#define interval 5
#define altitude2 20
#define battery_min_capacity 0

using namespace std;
using namespace DJI;
using namespace DJI::onboardSDK;

void flight_mode1(CoreAPI* api, Flight* flight,int fd,Modem myModem, Protoboard myProto, PositionData position, EulerAngle angle);
void flight_mode2(CoreAPI* api, Flight* flight,int fd,Modem myModem, Protoboard myProto ,PositionData position, EulerAngle angle);
inline double to_degrees(double radians);
void countdown(int seconds, bool led, Protoboard myProto);

int main(int argc,char* argv[])
{
	
	//Managing the modem.
	Modem myModem;

	//Managing the protoboard.
   	Protoboard myProto; 

	//Managing the connection with M100.
	LinuxSerialDevice* serialDevice = new LinuxSerialDevice(UserConfig::deviceName, UserConfig::baudRate);
	CoreAPI* api = new CoreAPI(serialDevice);
	Flight* flight = new Flight(api);
	WayPoint* waypointObj = new WayPoint(api);
	Camera* camera = new Camera(api);
	LinuxThread read(api, 2);
	sleep(2);

	//Managing the telemetry data.
	PositionData position;
	BatteryData battery;
	EulerAngle angle;
	
	//Starting the process.
	while(1) // Although it is a bad programming practice, it is the only way since I do not have a screen during the flight.
	{
		myProto.MyLed.LedOn();
		
		std::cout << "|---------------------------------------------------------------------------|"<< std::endl;
		std::cout << "|------------------DroneKTU. Copyright (C) 2017 Alvaro Zornoza--------------|"<< std::endl;
		std::cout << "|---------------------------------------------------------------------------|"<< std::endl;
		std::cout<<""<<std::endl<<std::endl;		
		std::cout << "|---------------------------------------------------------------------------|" << std::endl;
		std::cout << "|-Please press the button to run the process or Ctrl+C to close the program-|" << std::endl;
		std::cout << "|---------------------------------------------------------------------------|" << std::endl;
		
		while(!(myProto.MyButton.ButtonStatus()))
		{
			usleep(500000);
		}

	
		//Setup.
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

		//Set broadcast Freq Defaults.
		unsigned short broadcastAck = api->setBroadcastFreqDefaults(1);
	  	usleep(500000);

		//Managing local time for file name.
		char time[18];
		myModem.getTime(time);
	
		//Managing the opening of the file.
		int fd;
		char buf[100];
		sprintf(buf,"%s%s",DEFAULT_PENDRIVE,time);
		fd=open(buf,O_WRONLY|O_CREAT|O_TRUNC,0666);
		if(fd<0)
		{
			std::cout << "ERROR: It was not possible to create to the file or access the pendrive. This program will exit now. \n"; //The led blinks six times to show the error.
			myProto.MyLed.LedBlink(6);
			continue;
		}
		//Main process.
		char intro[1000];

		//Measuring time, langitude, and longitude with the drone disarmed.
		position=flight->getPosition();
		
		//Measuring battery status.
		battery=api->getBatteryCapacity();
		
		//If battery is not charged enough the program will not start and record it in the log.
		if(battery<battery_min_capacity)
		{
			sprintf(intro,"\nERROR: Battery status is %i%. It is not charged enough to start the experiment.\n",battery);
			write(fd,intro,strlen(intro));
			printf("%s",intro);
			myProto.MyLed.LedBlink(8); //The led blinks eight times to show the error.
			continue;
		}
		
		//Headlines of the file.
		sprintf(intro,"\n|------------------------------------------------|\n|-DroneKTU. Copyright (C) 2017 Alvaro Zornoza----|\n|------------------------------------------------|\n\n");
		write(fd,intro,strlen(intro));
		printf("%s",intro);

		sprintf(intro,"|-DroneKTU experiment-----------------------|\n-Latitude:%lf \n-Longitude:%lf \n-Date and time: %s \n|------------------------------------------------|\n\n",to_degrees(position.latitude),to_degrees(position.longitude),time);
		write(fd,intro,strlen(intro));
		printf("%s",intro);

		std::cout <<"Safety time before taking off (10s countdown)" << std::endl;
		countdown(10,true,myProto);
		
		//Taking off.
		monitoredTakeoff(api, flight);
		sleep(2);

		//Measuring euler angle.
		angle=flight->getEulerAngle();

		//Executing flight mode number one
		flight_mode1(api,flight,fd,myModem,myProto,position,angle);

		//Executing flight mode number two
		flight_mode2(api,flight,fd,myModem,myProto,position,angle);

		//Landing at the take-off point
		goHome(flight);
		//landing(api,flight);
	
		//Managing the closing of the file.
		char end[1000];
		sprintf(end,"\nExperiment runned sucessfully. \n");
		write(fd,end,strlen(end));
		printf("%s",end);
		close(fd);

		//Managing desconecction with Matrice 100.
		int cleanupStatus = cleanup(serialDevice, api, flight, &read);
		if (cleanupStatus == -1)
		{
			std::cout << "Unable to cleanly destroy OSDK infrastructure. There may be residual objects in the system memory.\n";
			return 0;
		}
		std::cout << "Program exited successfully." << std::endl;

		//Managing desconecction with SIM800L.
		myModem.finish();
		sleep(2);
	}
}

void flight_mode1(CoreAPI* api, Flight* flight, int fd, Modem myModem, Protoboard myProto, PositionData position, EulerAngle angle)
{
	//Calculating the number of steps based on the altitude and the interval desired.
	int steps=altitude1/interval;	

	//Main loop. Lifting up and measeuring in each step.	
	for(int m=0;m<steps;m++)
	{
		std::cout<<"Ascending to "<<5*(m+1)<<" meters\n"<< std::endl;
		position=flight->getPosition();
		moveByPositionOffset2(api, flight,(((5*(m+1))-position.height)*1.033),to_degrees(angle.yaw));
		cout<<"Waiting 10 seconds before measuring"<<endl;
		countdown(10,false,myProto);
		measure1(api,flight,fd,position,myModem);
	}
}

void flight_mode2(CoreAPI* api, Flight* flight, int fd, Modem myModem, Protoboard myProto, PositionData position, EulerAngle angle)
{
	//Ascending to 20 meters
	std::cout<<"Ascending to "<<altitude2<<" meters\n"<< std::endl;
	position=flight->getPosition();
	moveByPositionOffset2(api,flight,((altitude2-position.height)*1.033),to_degrees(angle.yaw));
		
	//Main loop. Manouvering and measeuring in each step.
	int i,j;
	for(i=1;i<=7;i++)
	{
		for(j=1;j<=5;j++)
		{	
			cout<<"Position "<<j<<"/"<<i<<" (5/7)"<<endl;
			cout<<"Waiting 10 seconds before measuring"<<endl;
			countdown(10,false,myProto);
			measure2(api,flight,fd,position,myModem,i,j);
			if(j<5)
			{
				cout<<"Moving to position "<<j+2<<"/"<<i+1<<" (5/7)"<<endl;
				if(i==1||i==3||i==5||i==7)
			
					moveByPositionOffset(api,flight,-12,0,0,to_degrees(angle.yaw));
				else	
					moveByPositionOffset(api,flight,12,0,0,to_degrees(angle.yaw));		
			}		
		}	
		if(i<7)
		{
			cout<<"Moving to position "<<j+1<<"/"<<i+2<<" (5/7)"<<endl;
			moveByPositionOffset(api,flight,0,17,0,to_degrees(angle.yaw));
		}	
	}
}

inline double to_degrees(double radians)
{
	return radians*(180.0/M_PI);
}

void countdown(int seconds, bool led, Protoboard myProto)
{
	int halfsecond=500000;
	for(int i=seconds;i>0;i--)
	{
		std::cout<<i<<" seconds"<<std::endl;
		if(led==true)
			myProto.MyLed.LedOn();
		usleep(halfsecond);
		if(led==true)
			myProto.MyLed.LedOff();
		usleep(halfsecond);
	}
}
