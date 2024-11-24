#pragma once

#include "Config.h"

// System
#include <iostream>
#include <cassert>
#include <cstdlib>

// External
#include <QDebug>

#ifdef _WIN32
	#include <intrin.h>
	#define DEBUG_BREAK() __debugbreak()
#else
	#include <signal.h>
	#define DEBUG_BREAK() raise(SIGTRAP)
#endif

#ifdef DEBUG
#define ASSERT(condition, message)														\
	{																					\
		if (!(condition))																\
		{																				\
			std::cerr << "Assertion failed: " << (message) << std::endl					\
					  << "File: " << __FILE__ << std::endl								\
					  << "Line: " << __LINE__ << std::endl;								\
			DEBUG_BREAK();																\
			std::abort();																\
		}																				\
	}
#else
#define ASSERT(condition, message)														\
	{																					\
		if (!(condition))																\
		{																				\
			qWarning() << "Assertion failed: " << (message) << std::endl				\
					   << "File: " << __FILE__ << std::endl								\
					   << "Line: " << __LINE__ << std::endl;							\
		}																				\
	}
#endif

