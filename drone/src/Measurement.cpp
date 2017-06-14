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

using namespace std;

struct antenna
{
	long cellid;
	int rxl;
} ; 

void measure1(CoreAPI* api, Flight* flight, int fd, PositionData p, Modem myModem)
{	
	char cadcs[100];
	char santennas[7][100];
	double height=0;
	long cellid[len];
	int rxl[numberofvalues][len];
	double rxl_avg[]={0,0,0,0,0,0,0};
	double rxl_sd[]={0,0,0,0,0,0,0};

	cout<<"\nMeasuring"<<endl;
	getData(&height,rxl,cellid,flight,p,myModem);
	average(rxl,rxl_avg);
	standardDeviation(rxl,rxl_avg,rxl_sd);
	sprintf(cadcs,"Altitude %lf\n",height);
	printf("\n%s",cadcs);
	write(fd,cadcs,strlen(cadcs));
	for(int i=0;i<len;i++)
	{
		sprintf(santennas[i],"%li;%lf;%lf\n",cellid[i],rxl_avg[i],rxl_sd[i]);
		printf("\n%s\n",santennas[i]);
		write(fd,santennas[i],strlen(santennas[i]));
	}
}

void measure2(CoreAPI* api, Flight* flight, int fd, PositionData p, Modem myModem, int i, int j)
{	
	
	char cadcs[100];
	double latitude=0,longitude=0;
	int rxl[numberofvalues];
	double rxl_avg=0;
	double rxl_sd=0;

	cout<<"\nMeasuring"<<endl;
	getData(&latitude,&longitude,rxl,flight,p,myModem);
	average(rxl,&rxl_avg);
	standardDeviation(rxl,rxl_avg,&rxl_sd);
	sprintf(cadcs,"Position: %i/%i \nLatitude: %lf\nLongitude: %lf\nRSSI(avg): %lf\nRSSI(sd): %lf\n",j,i,latitude,longitude,rxl_avg,rxl_sd);
	printf("\n%s",cadcs);
	write(fd,cadcs,strlen(cadcs));
}

void getData(double *height,int rxl[numberofvalues][len], long cellid[], Flight* flight, PositionData p, Modem myModem)
{	
	*height=flight->getPosition().height;
	struct antenna antennas[len];
	for(int i=0;i<numberofvalues;i++)
	{	
		myModem.getInfo(antennas,len);	
		for(int j=0;j<len;j++)
		{
			rxl[i][j]=antennas[j].rxl;
			if(i==0)
			 	cellid[j]=antennas[j].cellid;
		}
		cout<<i<<"/"<<numberofvalues<<endl;
		usleep(200000);
	}	
}

void getData(double *latitude,double *longitude,int rxl[numberofvalues], Flight* flight, PositionData p, Modem myModem)
{
	*latitude=flight->getPosition().latitude;
	*longitude=flight->getPosition().longitude;
	for(int i=0;i<numberofvalues;i++)
	{
		rxl[i]=myModem.getSignalQuality();
		cout<<i<<"/"<<numberofvalues<<endl;
		usleep(500000);
	}
}

void average(int rxl[numberofvalues][len], double rxl_avg[])
{

	double rxl_sum[]={0,0,0,0,0,0,0};
	for(int j=0;j<len;j++)	
	{	
		printf("\n");
		for(int i=0;i<numberofvalues;i++)
		{
			rxl_sum[j]+=rxl[i][j];
			//printf("%lf\n",rxl_sum[j]);
		}
	}	
	for(int j=0;j<len;j++)
	{
		rxl_avg[j]=rxl_sum[j]/numberofvalues;
	}
}

void average(int rxl[numberofvalues], double *rxl_avg)
{
	double rxl_sum=0;
	for(int j=0;j<numberofvalues;j++)
	{
		rxl_sum+=rxl[j];
	}
	*rxl_avg=rxl_sum/numberofvalues;
}


void standardDeviation(int rxl[numberofvalues][len], double rxl_avg[], double rxl_sd[])
{
	double rxl_aux[]={0,0,0,0,0,0,0};
	for(int j=0;j<len;j++)
	{
		for(int i=0;i<numberofvalues;i++)
		{		
			rxl_aux[j]+=pow(rxl[i][j]-rxl_avg[j],2);
		}
	}	
	for(int j=0;j<len;j++)
	{
		rxl_sd[j]=sqrt(rxl_aux[j]/numberofvalues);
	}
}

void standardDeviation(int rxl[numberofvalues], double rxl_avg, double *rxl_sd)
{

	double rxl_aux=0;
	for(int j=0;j<numberofvalues;j++)
	{
		rxl_aux+=pow(rxl[j]-rxl_avg,2);
	}
	*rxl_sd=sqrt(rxl_aux/numberofvalues);
}
