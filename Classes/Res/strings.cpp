#include "strings.h"
USING_NS_CC;

std::string GetText(const std::string& txt)
{

	static auto strings = FileUtils::getInstance()->getValueMapFromFile("xml/string.xml");

	std::string btnText = strings[txt].asString();

	return btnText;

}