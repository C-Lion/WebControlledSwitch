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

	String CreateHTMLFromTemplate(const String& htmlTemplate, const std::map<String, String>& map)
	{
		String result;
		int index = 0;//begin(htmlTemplate);
		int end = -1;
		do
		{
			int beginVariable = htmlTemplate.indexOf('%', index); //search <%= by searching %
			int endVariable = -1;
			if (beginVariable != -1) //only if beginVariable didn't reach the end of html
				endVariable = htmlTemplate.indexOf('%', beginVariable + 1);

			if (beginVariable == -1 || endVariable == -1) //no more variables
			{
				result += htmlTemplate.substring(index); //add the template end
				break;
			}

			if (htmlTemplate[beginVariable - 1] != '<' || htmlTemplate[beginVariable + 1] != '=' || htmlTemplate[endVariable + 1] != '>') //not <%= ... %>
			{
				result += htmlTemplate[index];
				++index;
				continue;
			}
			String replacedValue = map.at(htmlTemplate.substring(beginVariable + 2, endVariable)); //extract only the variable name and replace it
			result += htmlTemplate.substring(index, beginVariable - 1) + replacedValue; //Add all text before the variable and the replacement
			index = endVariable + 2;
		} while (index != end);

		return result;
	}

	void software_Reboot() //rebot the board
	{
		ESP.restart();
	}

}