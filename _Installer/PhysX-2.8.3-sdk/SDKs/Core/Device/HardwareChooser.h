#ifndef HARDWARECHOOSER_H
#define HARDWARECHOOSER_H

#include "NxMutex.h"

class HardwareChooser
{
	static int		sDevice;
public:
	static const int NO_DEVICE = -1;
	static const int UNINITIALIZED = -2;

	static int getDevice();
};

#endif