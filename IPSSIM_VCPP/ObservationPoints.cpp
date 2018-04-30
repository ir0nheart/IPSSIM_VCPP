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
	P1 = P2 = U1 = U2 = C1 = C2 = E1 = E2 = R1 = R2 = S1 = S2 = 0;
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

void ObservationPoints::setP1(double val){ this->P1 = val; }
void ObservationPoints::setU1(double val){ this->U1 = val; }
void ObservationPoints::setC1(double val){ this->C1 = val; }
void ObservationPoints::setE1(double val){ this->E1 = val; }
void ObservationPoints::setR1(double val){ this->R1 = val; }
void ObservationPoints::setS1(double val){ this->S1 = val; }
double ObservationPoints::getC1(){ return this->C1; }
double ObservationPoints::getU1(){ return this->U1; }
double ObservationPoints::getP1(){ return this->P1; }
double ObservationPoints::getE1(){ return this->E1; }
double ObservationPoints::getR1(){ return this->R1; }
double ObservationPoints::getS1(){ return this->S1; }
void ObservationPoints::setP2(double val){ this->P2 = val; }
void ObservationPoints::setU2(double val){ this->U2 = val; }
void ObservationPoints::setC2(double val){ this->C2 = val; }
void ObservationPoints::setE2(double val){ this->E2 = val; }
void ObservationPoints::setR2(double val){ this->R2 = val; }
void ObservationPoints::setS2(double val){ this->S2 = val; }
double ObservationPoints::getC2(){ return this->C2; }
double ObservationPoints::getU2(){ return this->U2; }
double ObservationPoints::getP2(){ return this->P2; }
double ObservationPoints::getE2(){ return this->E2; }
double ObservationPoints::getR2(){ return this->R2; }
double ObservationPoints::getS2(){ return this->S2; }