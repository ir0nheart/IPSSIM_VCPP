#include <process.h>
#include <Windows.h>
#include <iostream>
#include <cassert>
#include <memory>
#include "Runnable.h"
#pragma once
class Thread
{
public:
	Thread(std::auto_ptr<Runnable> run);
	Thread();
	virtual ~Thread();
	void start();
	void* join();
private:
	HANDLE hThread;
	unsigned wThreadID;
	// runnable object will be deleted automatically
	std::auto_ptr<Runnable> runnable;
	Thread(const Thread&);
	const Thread& operator=(const Thread&);
	// called when run() completes
	void setCompleted();
	// stores return value from run()
	void* result;
	virtual void* run() { return 0; }
	static unsigned WINAPI startThreadRunnable(LPVOID pVoid);
	static unsigned WINAPI startThread(LPVOID pVoid);
	void printError(LPTSTR lpszFunction, LPSTR fileName, int lineNumber);
};

