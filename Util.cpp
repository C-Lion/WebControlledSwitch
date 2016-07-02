// 
// 
// 

#include "Util.h"
namespace Util
{
	void software_Reboot() //rebot the board
	{
		wdt_enable(WDTO_15MS);
		while (1)
		{
		}
	}

}