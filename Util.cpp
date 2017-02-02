// 
// 
// 

#include "Util.h"

using namespace std;

namespace Util
{
	
	string __attribute__((section(".irom0_0_seg"))) CreateHTMLFromTemplate(const string& htmlTemplate, const std::map<string, string>& map)
	{
		string result;
		auto index = begin(htmlTemplate);
		do
		{
			auto beginTemplate = find(index, end(htmlTemplate), '%'); //search <%= by searching %
			auto endTemplate = end(htmlTemplate);
			if (beginTemplate != endTemplate) //only if beginTemplate didn't reach the end of html
				endTemplate = find(beginTemplate + 1, end(htmlTemplate), '%');

			if (beginTemplate == end(htmlTemplate) || endTemplate == end(htmlTemplate)) //no more variables
			{
				result += string(index, end(htmlTemplate)); //add the template end
				break;
			}

			if (*(beginTemplate - 1) != '<' || *(beginTemplate + 1) != '=' || *(endTemplate + 1) != '>') //not <%= ... %>
			{
				result += *index;
				++index;
				continue;
			}
			string replacedValue = map.at(string(beginTemplate + 2, endTemplate)); //extract only the variable name and replace it
			result += string(index, beginTemplate - 1) + replacedValue; //Add all text before the variable and the replacement
			index = endTemplate + 2;
		} while (index != end(htmlTemplate));

		return result;
	}

	void software_Reboot() //rebot the board
	{
		ESP.restart();
	}

}