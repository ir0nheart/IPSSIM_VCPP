#include "TimeCycleSch.h"
#include "ControlParameters.h"

TimeCycleSch::TimeCycleSch()
{
}


TimeCycleSch::~TimeCycleSch()
{
}

void TimeCycleSch::setCREFT(string CREFT){ this->CREFT = CREFT; }
void TimeCycleSch::setSCALT(double SCALT){ this->SCALT = SCALT; }
void TimeCycleSch::setNTMAX(int NTMAX){ this->NTMAX = NTMAX; }
void TimeCycleSch::setTIMEI(double TIMEI){ this->TIMEI = TIMEI; }
void  TimeCycleSch::setTIMEL(double TIMEL){ this->TIMEL = TIMEL; }
void TimeCycleSch::setTIMEC(double TIMEC){ this->TIMEC = TIMEC; }
void TimeCycleSch::setNTCYC(int NTCYC){ this->NTCYC = NTCYC; }
void TimeCycleSch::setTCMULT(double TCMULT){ this->TCMULT = TCMULT; }
void TimeCycleSch::setTCMIN(double TCMIN){ this->TCMIN = TCMIN; }
void TimeCycleSch::setTCMAX(double TCMAX){ this->TCMAX = TCMAX; }

string TimeCycleSch::getCREFT(){ return this->CREFT; }
double TimeCycleSch::getSCALT(){ return this->SCALT; }
int TimeCycleSch::getNTMAX(){ return this->NTMAX; }
double TimeCycleSch::getTIMEI(){ return this->TIMEI; }
double TimeCycleSch::getTIMEL(){ return this->TIMEL; }
double TimeCycleSch::getTIMEC(){ return this->TIMEC; }
int TimeCycleSch::getNTCYC(){ return this->NTCYC; }
double TimeCycleSch::getTCMULT(){ return this->TCMULT; }
double TimeCycleSch::getTCMIN(){ return this->TCMIN; }
double TimeCycleSch::getTCMAX(){ return this->TCMAX; }

void TimeCycleSch::setELAPSD(bool val){
	this->ELAPSD = val;
}
bool TimeCycleSch::getELAPSD(){
	return this->ELAPSD;
}
double TimeCycleSch::FRCSTP(double TIME){
	vector<Schedule *> vS = ControlParameters::Instance()->getListOfSchedules();
	int ind = -1;
	for (int i = 0; i < vS.size();i++){

		if (vS[i]->getScheduleName() == "TIME_STEPS"){
			ind = i;
			break;
		}
	}
	if (ind == -1){
		ControlParameters::Instance()->exitOnError("NotFound ListOfSchedules");
	}

	vector<pair<double, double>> esList = vS[ind]->getSList();
	pair <double, double> a = esList[0];
	double T1 = a.first;
	if (TIME == T1){
		return 0.0;
	}
	else if (TIME < T1){
		double d = DBL_MAX;
		return (-1 * d);
	}

	for (int i = 1; i <= esList.size()-1;i++){
		double T2 = esList[i].first;
		if (TIME == T2){
			return (double)i;
		}
		else if (TIME < T2){
			double WT = (TIME - T1) / (T2 - T1);
			double S1 = (i - 1);
			double S2 =i;
			return ((1.0 - WT)*S1 + WT*S2);
		}
	
	}
	return   DBL_MAX;
}
void TimeCycleSch::createTimeList(){

	// SCALE ALL TIME SPECS
	this->TIMEI = this->TIMEI * this->SCALT;
	this->TIMEL = this->TIMEL * this->SCALT;
	this->TIMEC = this->TIMEC * this->SCALT;
	this->TCMIN = this->TCMIN * this->SCALT;
	this->TCMAX = this->TCMAX * this->SCALT;

	double TIME = TIMEI;
	double STEP =TIME;
	double DTIME = TIMEC;
	sList.push_back(make_pair(TIME, STEP));
	int TCOUNTER = 0;
	int TCCHNG = ControlParameters::Instance()->getTimeStepDivide();
	for (int i = 1; i <= NTMAX; i++){
	/*	if (((i - 1) % NTCYC) == 0 && i>1){
			DTIME = DTIME*TCMULT;
			TCOUNTER = 0;
		}*/

		if ((TCOUNTER% NTCYC) ==0 && i > 1){
			DTIME = DTIME*TCMULT;
		}
			if (TCCHNG > 0 && i == TCCHNG){
				DTIME = TIMEC;
				TCOUNTER = 0;
			}

			if (DTIME > TCMAX){ DTIME = TCMAX; }
			if (DTIME < TCMIN){ DTIME = TCMIN; }

			TIME = TIME + DTIME;
			TCOUNTER = TCOUNTER + 1;
			STEP = FRCSTP(TIME);
			sList.push_back(make_pair(TIME, STEP));
			if (TIME >= TIMEL){ break; }
		}
	
	
}
void TimeCycleSch::changeListElement(int index,double time,double step){
	sList[index] = make_pair(time, step);

}