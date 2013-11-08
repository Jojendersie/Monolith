#pragma once

/// \brief Returns the number of passed seconds since the last call.
/// \param [inout] _slot A local variable to store the last access to TimeQuery
///		in ticks. It is possible to measure different things at the same time
///		by using more slots.
typedef unsigned long long TimeQuerySlot;
double TimeQuery(TimeQuerySlot& _slot);

/// \brief Returns the time elapsed since the program was started in seconds.
float TimeSinceProgramStart();