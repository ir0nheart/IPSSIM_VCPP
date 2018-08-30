#ifndef _WRITER
#define _WRITER
#include <string>
#include <deque>
#include <mutex>
#pragma once
using namespace std;
class Writer
{
public:
	static Writer* LSTInstance();
	static Writer* NODInstance();
	static Writer* ELEInstance();
	static Writer* OBSInstance();
	static Writer* IAInstance();
	void addToWriteContainer(string str);
	deque<string> getWriteContainer();
	void popFrontWriteContainer();
	deque<string>writeContainer;
private:
	
	Writer();
	virtual ~Writer();
	static Writer * mLST_pInstance;
	static Writer * mNOD_pInstance;
	static Writer * mELE_pInstance;
	static Writer * mOBS_pInstance;
	static Writer * mIA_pInstance;
};

#endif