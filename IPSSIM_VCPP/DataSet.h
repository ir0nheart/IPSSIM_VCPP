#ifndef _DATASET_
#define _DATASET_
#include <string>
#include <vector>
#include <iostream>
#include <list>
#include "ControlParameters.h"
#include "InputFileParser.h"
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/spirit/include/qi_string.hpp>
#include<boost/spirit/include/qi.hpp>
using namespace std;
#pragma once
class ControlParameters;
class DataSet
{
public:

	DataSet(string dSName,size_t size=1);

	DataSet(DataSet const & other);
	DataSet& operator = (DataSet const&other);
	//Move Constructor
	DataSet(DataSet&& other);
	DataSet& operator = (DataSet&& other);

	////Copy Constructor
	//Node(Node const&rhs);
	//Node& operator = (Node const&rhs);
	//// Move Constructor
	//Node(Node &&rhs);
	//Node & operator = (Node &&rhs);


	virtual ~DataSet();

	void addData(string data);

	void parseDataSet_1();
	void parseDataSet_2A();
	void parseDataSet_2B();
	void parseDataSet_3();
	void parseDataSet_4();
	void parseDataSet_5();
	void parseDataSet_6();
	void parseDataSet_7A();
	void parseDataSet_7B();
	void parseDataSet_7C();
	void parseDataSet_8ABC();
	void parseDataSet_8D();
	void parseDataSet_8E_9_10_11();
	void parseDataSet_12_13_14A();
	void parseDataSet_14B();
	void parseDataSet_15A();
	void parseDataSet_15B();
	void parseDataSet_17_18(string key);
	void parseDataSet_19_20(string key);
	void parseDataSet_22();

	void parseAllLines();
	string getDataSetName();
	vector<string> * getData();
	void free();
private:
	string dataSetName = "";
	size_t numberOfLines;
	vector<string> *Data;
};

#endif