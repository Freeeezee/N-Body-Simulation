#include "util/stopwatch.hpp"
#include <iostream>
#include <chrono>

using namespace std::chrono;

stopwatch::stopwatch():
	running(false), starttime(steady_clock::now()), stoptime(steady_clock::now())
{
}

void stopwatch::start()
{
	if (running) {
		std::cout << "Stopwatch has already been started.";
	}
	else {
		running = true;
		starttime = steady_clock::now();
	}
}

void stopwatch::stop() 
{
	stoptime = steady_clock::now();
	if (!running) {
		std::cout << "Stopwatch wasn't running.";
	}
	running = false;
}

long long stopwatch::elapsedMillis() const
{
	if (running) {
		std::cout << "stopwatch is still running.";
		return duration_cast<std::chrono::milliseconds>(stoptime - starttime).count();
	}
	else {
		return duration_cast<std::chrono::milliseconds>(stoptime - starttime).count();
	}
}
