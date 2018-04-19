#include "ObservationPoints.h"


ObservationPoints::ObservationPoints(string OBSNAM, string schedule, string OBSFMT, double XOBS, double YOBS, double ZOBS)
{
	this->OBSNAM = OBSNAM;
	this->schedule = schedule;
	this->OBSFMT = OBSFMT;
	this->XOBS = XOBS;
	this->YOBS = YOBS;
	this->ZOBS = ZOBS;
	this->obsElement = -1;
}

string ObservationPoints::getOBSNAM(){
	return this->OBSNAM;
}

void ObservationPoints::print_Info(){
	cout << setfill('#') << setw(80) << ""<< endl;
	cout << setfill('*') << setw(55);
	cout << "Observation Point Information ";
	cout << setfill('*') << setw(25) << ""<< endl;
	cout << "\t OBS Name     : " << OBSNAM << endl;
	cout << "\t OBS Format   :" << OBSFMT << endl;
	cout << "\t OBS Schedule :" << schedule << endl;
	cout << "\t OBS X-Coord  :" << XOBS << endl;
	cout << "\t OBS Y-Coord  :" << YOBS << endl;
	cout << "\t OBS Z-Coord  :" << ZOBS << endl;
	cout << "\t OBS Element  :" << obsElement << endl;
	cout << "\t OBS XSI      :" << XSI << endl;
	cout << "\t OBS ETA      :" << ETA << endl;
	cout << "\t OBS ZET      :" << ZET << endl;
	cout << setfill('#') << setw(80) << ""<< endl;
	cout << endl;
}
string ObservationPoints::getSchedule(){
	return this->schedule;
}
string ObservationPoints::getOBSFMT(){
	return this->OBSFMT;
}
double ObservationPoints::getXOBS(){
	return this->XOBS;
}
double ObservationPoints::getYOBS(){
	return this->YOBS;
}
double ObservationPoints::getZOBS(){
	return this->ZOBS;
}

ObservationPoints::~ObservationPoints()
{
}

void ObservationPoints::setXSI(double XSI){ this->XSI = XSI; }
void ObservationPoints::setETA(double ETA){ this->ETA = ETA; }
void ObservationPoints::setZET(double ZET){ this->ZET = ZET; }
void ObservationPoints::setObsElement(int ElNo){ this->obsElement = ElNo; }

double ObservationPoints::getXSI(){ return this->XSI; }
double ObservationPoints::getETA(){ return this->ETA; }
double ObservationPoints::getZET(){ return this->ZET; }
int ObservationPoints::getObsElement(){ return this->obsElement; }