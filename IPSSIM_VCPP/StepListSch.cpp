#include "StepListSch.h"


StepListSch::StepListSch()
{
}


StepListSch::~StepListSch()
{
}

void StepListSch::setNSLIST(int NSLIST){
	this->NSLIST = NSLIST;
}

void StepListSch::addToISLIST(double step){
	this->ISLIST.push_back(step);
}

int StepListSch::getNSLIST(){
	return this->NSLIST;
}

list<double> StepListSch::getISLIST(){
	return this->ISLIST;
}

void StepListSch::createTimeList(){
	for (double x : ISLIST){
		sList.push_back(make_pair(x, x));
	}

}