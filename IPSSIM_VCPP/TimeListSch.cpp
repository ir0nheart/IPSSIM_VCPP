#include "TimeListSch.h"


TimeListSch::TimeListSch()
{
}


TimeListSch::~TimeListSch()
{
}

void TimeListSch::setCREFT(string CREFT){
	this->CREFT = CREFT;
}
void TimeListSch::setSCALT(double SCALT){
	this->SCALT = SCALT;
}
void TimeListSch::setNTLIST(int NTLIST){ this->NTLIST = NTLIST; }
void TimeListSch::addToTLIST(double step){ this->TLIST.push_back(step); }
string TimeListSch::getCREFT(){ return this->CREFT; }
double TimeListSch::getSCALT(){ return this->SCALT; }
int TimeListSch::getNTLIST(){ return this->NTLIST; }
vector<double> TimeListSch::getTLIST(){ return this->TLIST; }
void TimeListSch::setELAPSD(bool val){
	this->ELAPSD = val;
}
bool TimeListSch::getELAPSD(){ return this->ELAPSD; }
void TimeListSch::createTimeList(){
	for (vector<double>::iterator it = TLIST.begin(); it != TLIST.end(); ++it) {
		*it = *it * SCALT;
	}

	for (double x : TLIST){
		double time = x;
		double step = FRCSTP(time);
		sList.push_back(make_pair(time, step));
	}

}