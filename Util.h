// Util.h

#ifndef _UTIL_h
#define _UTIL_h

#include "arduino.h"
#include <string>
#include <algorithm>

namespace Util
{
	void software_Reboot(); //rebot the board

	template <typename T, std::size_t n>
	void String2Array(const std::string &str, T(&arr)[n])
	{
		auto length = std::min(n, str.length());
		std::copy_n(begin(str), length, arr);
		arr[length] = 0; //make sure it is null terminated string
	}
}


#endif

