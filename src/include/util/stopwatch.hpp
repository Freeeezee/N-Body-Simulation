#pragma once
#include <chrono>

using namespace std::chrono;

class stopwatch {
	public:
	stopwatch();
	void start();
	void stop();
	long long elapsedMillis() const;
private:
	steady_clock::time_point starttime;
	steady_clock::time_point stoptime;
	bool running;
};