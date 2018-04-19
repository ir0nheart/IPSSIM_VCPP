#pragma once
class Layer
{
public:
	Layer(); 
	Layer(int layerNo, double unitWeight, double topLayer, double bottomLayer);
	virtual ~Layer();
	int getLayerNumber();
	double getLayerUnitWeight();
	double getLayerTop();
	double getLayerBottom();
private:
	int layerNo;
	double unitWeight;
	double topLayer;
	double bottomLayer;
};

