#pragma once
#ifndef CPUWALLTIME_H
#define CPUWALLTIME_H
#include <Windows.h>
#include <iostream>
//clock code from https://stackoverflow.com/questions/17432502/how-can-i-measure-cpu-time-and-wall-clock-time-on-both-linux-windows
double get_wall_time() {
	LARGE_INTEGER time, freq;
	if (!QueryPerformanceFrequency(&freq)) {
		//  Handle error
		return 0;
	}
	if (!QueryPerformanceCounter(&time)) {
		//  Handle error
		return 0;
	}
	return (double)time.QuadPart / freq.QuadPart;
};

double get_cpu_time() {
	FILETIME a, b, c, d;
	if (GetProcessTimes(GetCurrentProcess(), &a, &b, &c, &d) != 0) {
		//  Returns total user time.
		//  Can be tweaked to include kernel times as well.
		return
			(double)(d.dwLowDateTime |
			((unsigned long long)d.dwHighDateTime << 32)) * 0.0000001;
	}
	else {
		//  Handle error
		return 0;
	}
};

class cpuWalltime
{
public:
	cpuWalltime() {}
	~cpuWalltime() {}
	void timeStart() {
		startTimeCPU = get_cpu_time();
		startTimeWall = get_wall_time();
	}
	void timeEnd() {
		endTimeCPU = get_cpu_time();
		endTimeWall = get_wall_time();
		totTimeCPU += endTimeCPU - startTimeCPU;
		totTimeWall += endTimeWall - startTimeWall;
		amountOfTime++;
	}
	void printTime() {
		std::cout << "CPU time: " << totTimeCPU / amountOfTime << std::endl << "Wall time: " << totTimeWall / amountOfTime << std::endl;
	}
	void cleanTime() {
		totTimeCPU = 0.0;
		totTimeWall = 0.0;
		amountOfTime = 0.0f;
	}

private:
	double startTimeCPU;
	double endTimeCPU;
	double totTimeCPU = 0.0;
	double startTimeWall;
	double endTimeWall;
	double totTimeWall = 0.0;
	float amountOfTime = 0.0f;
};

#endif