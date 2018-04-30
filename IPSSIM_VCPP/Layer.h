#pragma once
class Layer
{
public:
	Layer(); 
	Layer(int layerNo, double unitWeight, double topLayer, double bottomLayer,int satCond);
	virtual ~Layer();
	int getLayerNumber();
	double getLayerUnitWeight();
	double getLayerTop();
	double getLayerBottom();
	int getSatCond();
private:
	int layerNo;
	double unitWeight;
	double topLayer;
	double bottomLayer;
	int satCond;  // -1 above WT, 1 Totally under WT, 0 WT lays within layer
};

