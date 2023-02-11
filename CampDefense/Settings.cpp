#include "Settings.h"
#include <iostream>

void Settings::parse_json(json settings)
{
	settings["scale"].get_to(scale);
	settings["lang"].get_to(lang);
}

json Settings::convert_to_json()
{
	json settings;
	settings["scale"] = this->getScale();
	settings["lang"] = this->getLang();
	return settings;
}