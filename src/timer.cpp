#include <ctime>
#include "timer.hpp"

#ifdef WINDOWS
#include "windows.h"
#endif
const double CLOCKS_PER_SEC_INV = 1.0/CLOCKS_PER_SEC;	// Pre calculated for performance

double TimeQuery(TimeQuerySlot& _slot)
{
	// Remember old timestamp and sample new one
	TimeQuerySlot oldTime = _slot;
#ifdef WINDOWS
	TimeQuerySlot qwF;
	QueryPerformanceFrequency((LARGE_INTEGER*)&qwF);
	QueryPerformanceCounter((LARGE_INTEGER*)&_slot);
#else
	g_QueryCounter[_Slot] = clock();
#endif
	// Convert difference to seconds
	return (_slot - oldTime)/double(qwF);
}


float TimeSinceProgramStart()
{
	return float(clock()*CLOCKS_PER_SEC_INV);
}