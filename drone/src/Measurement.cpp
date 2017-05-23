// Measurement.cpp: implementation of the measurement functions.
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

#include "Measurement.h"

#define numberofvalues 20

using namespace std;

void measure(CoreAPI* api, Flight* flight, int fd, PositionData p, Modem myModem)
{	
	cout<<"\nMeasuring"<<endl;
	char cadcs[100];
	int signal[numberofvalues+1];
	double height[numberofvalues+1];
	double height_average=0, signal_average=0, height_sd=0, signal_sd=0;
	getData(height,signal,flight,p,myModem);
	average(height,signal,&height_average,&signal_average);
	standardDeviation(height,signal,height_average,signal_average,&height_sd,&signal_sd);
	sprintf(cadcs,"%lf;%lf;%lf;%lf\n",height_average,height_sd,signal_average,signal_sd);
	printf("\n%s\n\n",cadcs);
	write(fd,cadcs,strlen(cadcs));
}
void getData(double height[], int signal[], Flight* flight, PositionData p, Modem myModem)
{
	for(int i=0;i<numberofvalues+1;i++)
	{	
			p=flight->getPosition();
			height[i]=p.height;
			signal[i]=myModem.getSignalQuality();
			cout<<i<<"/"<<numberofvalues<<endl;
			usleep(500000);	
	}
}

void average(double height[], int signal[], double *height_average, double *signal_average)
{
	double height_sum=0,signal_sum=0;
	for(int j=1;j<numberofvalues+1;j++)
	{	
		height_sum+=height[j];
		signal_sum+=signal[j];		
	}
	*height_average=height_sum/numberofvalues;
	*signal_average=signal_sum/numberofvalues;
}

void standardDeviation(double height[], int signal[], double height_average, double signal_average, double *height_sd, double *signal_sd)
{
	double height_aux=0,signal_aux=0;	
	for(int i=1;i<numberofvalues+1;i++)
	{
		height_aux+=pow(height[i]-height_average,2);
		signal_aux+=pow(signal[i]-signal_average,2);
	}
	*height_sd=sqrt(height_aux/numberofvalues);
	*signal_sd=sqrt(signal_aux/numberofvalues);
}
