#include "Layer.h"


Layer::Layer()
{
}

Layer::Layer(int layerNo, double unitWeight, double topLayer, double bottomLayer,int satCond)
{
	this->layerNo = layerNo;
	this->unitWeight = unitWeight;
	this->topLayer = topLayer;
	this->bottomLayer = bottomLayer;
	this->satCond = satCond;
}

int Layer::getSatCond(){ return this->satCond; }
int Layer::getLayerNumber(){
	return this->layerNo;
}

double Layer::getLayerUnitWeight(){
	return this->unitWeight;

}

double Layer::getLayerTop(){
	return this->topLayer;
}
double Layer::getLayerBottom(){
	return this->bottomLayer;
}
Layer::~Layer()
{
}

