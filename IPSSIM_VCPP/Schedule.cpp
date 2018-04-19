#include "Schedule.h"
#include "ControlParameters.h"


Schedule::Schedule()
{
}


Schedule::~Schedule()
{
}
Schedule::Schedule(string schName) :scheduleName(schName)
{
}

vector<pair<double, double>> Schedule::getSList(){
	return this->sList;
}
void Schedule::addToSList(double time, double step){
	pair<double, double>p = pair<double, double>(time, step);
	sList.push_back(p);
}
void Schedule::setScheduleName(string schName){
	this->scheduleName = schName;
}
void Schedule::setScheduleType(string schType){
	this->scheduleType = schType;
}
string Schedule::getScheduleName(){
	return this->scheduleName;
}
string Schedule::getScheduleType(){
	return this->scheduleType;
}

void Schedule::createTimeList(){}

double Schedule::TIMETS(int STEP){
	ControlParameters * CP = ControlParameters::Instance();
	TimeCycleSch * TIME_STEPS = CP->getTIMESTEPSSchedule();

	if (TIME_STEPS == nullptr)
		return 0;

	if (STEP < 0){
		double d = DBL_MAX;
		return (-1 * d);
	}
	
	if (STEP > TIME_STEPS->getSList().size()){
		double d = DBL_MAX;
		return (d);
	}

	
	return TIME_STEPS->getSList()[STEP].first;
}

void Schedule::setSBASED(bool val){ this->SBASED = val; }
bool Schedule::getSBASED(){ return this->SBASED; }
void Schedule::setELAPSED(bool val){ this->ELAPSED = val; }
bool Schedule::getELAPSED(){ return this->ELAPSED; }
void Schedule::reConstructSList(double tref){
	double time;
	double step;
	vector<pair<double, double>>tempList = sList;
	int size = tempList.size();
	sList.clear();
	if (SBASED){
		for (int i = 0; i<size; i++){
			if (i > 0){
				if (tempList[i].second == tempList[i-1].second){
					ControlParameters::Instance()->exitOnError("INP-6-12");
				}

			}
			step = tempList[i].second;
			if (step<0 || step > ControlParameters::Instance()->getITMAX())
				continue;

			time = TIMETS(step);
			sList.push_back(make_pair(time, step));
		}

	}
	else{
		for (int i = 0; i< size; i++){
			if (i > 0){
				if (tempList[i].first == tempList[i-1].first){
					ControlParameters::Instance()->exitOnError("INP-6-12");
				}

			}

			time = tref + sList[i].first;

			if (time<ControlParameters::Instance()->getTSTART() || time> ControlParameters::Instance()->getTFINISH())
				continue;

			step = FRCSTP(time);
			sList[i] = make_pair(time, step);
		}
	}

}

double Schedule::FRCSTP(double TIME){
	pair <double, double> a = sList.at(0);
	double T1 = a.first;
	if (TIME == T1){
		return 0.0;
	}
	else if (TIME < T1){
		double d = DBL_MAX;
		return (-1 * d);
	}

	for (int i = 1; i < sList.size() - 1; i++){
		double T2 = sList.at(i).first;
		if (TIME == T2){
			return (double)i;
		}
		else if (TIME < T2){
			double WT = (TIME - T1) / (T2 - T1);
			double S1 = (i - 1);
			double S2 = i;
			return ((1.0 - WT)*S1 + WT*S2);
		}

	}
	return   DBL_MAX;
}