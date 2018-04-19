#include <string>
#include "Element.h"

#pragma once
using namespace std;
class ObservationPoints
{
public:
	ObservationPoints(string OBSNAM,string schedule,string OBSFMT,double XOBS,double YOBS,double ZOBS = 0.0);
	~ObservationPoints();
	string getOBSNAM();
	string getOBSFMT();
	string getSchedule();
	double getXOBS();
	double getYOBS();
	double getZOBS();

	void setXSI(double XSI); 
	void setETA(double ETA);
	void setZET(double ZET);
	void setObsElement(int ElNo);
	
	double getXSI(); 
	double getETA(); 
	double getZET(); 
	int getObsElement();
	void print_Info();
private:
	string OBSNAM;
	double XOBS;
	double YOBS;
	double ZOBS;
	string schedule;
	string OBSFMT;
	double XSI =0;
	double ZET=0;
	double ETA=0;
	int obsElement;
};

