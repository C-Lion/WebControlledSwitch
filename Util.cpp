// 
// 
// 

#include "Util.h"

using namespace std;

namespace Util
{
	/*int findChar(const String& str, char c, int from, int to)
	{
		while (from < to)
		{
			if (str[from] == c)
				break;
			++from;
		}
		return from;
	}*/

	

	void software_Reboot() //rebot the board
	{
		ESP.restart();
	}

}