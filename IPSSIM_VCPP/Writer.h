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
	static Writer* Instance();
	void addToWriteContainer(string str);
	deque<string> getWriteContainer();
	void popFrontWriteContainer();
	deque<string>writeContainer;
private:
	
	Writer();
	virtual ~Writer();
	static Writer * m_pInstance;
};

#endif