#include "Layer.h"


Layer::Layer()
{
}

Layer::Layer(int layerNo, double unitWeight, double topLayer, double bottomLayer)
{
	this->layerNo = layerNo;
	this->unitWeight = unitWeight;
	this->topLayer = topLayer;
	this->bottomLayer = bottomLayer;
}


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

