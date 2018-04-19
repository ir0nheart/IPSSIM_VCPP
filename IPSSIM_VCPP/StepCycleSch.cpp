#include "StepCycleSch.h"


StepCycleSch::StepCycleSch()
{
}

StepCycleSch::~StepCycleSch()
{
}
void StepCycleSch::createTimeList(){
	sList.push_back(make_pair(ISTEPI, ISTEPI));
	double da = ISTEPI;
	for (int i = 1; i < NSMAX; i++){
		da = da + ISTEPC;
		sList.push_back(make_pair(da, da));
	}
}
void StepCycleSch::setISTEPC(int ISTEPC){
	this->ISTEPC = ISTEPC;
}
void StepCycleSch::setISTEPL(int ISTEPL){
	this->ISTEPL = ISTEPL;
}
void StepCycleSch::setISTEPI(int ISTEPI){
	this->ISTEPC = ISTEPC;
}
void StepCycleSch::setNSMAX(int NSMAX){
	this->NSMAX = NSMAX;
}

int StepCycleSch::getISTEPC(){
	return this->ISTEPC;
}

int StepCycleSch::getISTEPL(){
	return this->ISTEPL;
}
int StepCycleSch::getISTEPI(){
	return this->ISTEPI;
}
int StepCycleSch::getNSMAX(){
	return this->NSMAX;
}
