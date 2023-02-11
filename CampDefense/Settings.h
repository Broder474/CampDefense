#include "nlohmann/json.hpp"

using json = nlohmann::json;

class Settings
{
public:
	void parse_json(json settings);
	json convert_to_json();

	float getScale() { return scale; }
	void setScale(float scale) { this->scale = scale; }
	std::string getLang() { return lang; }
	void setLang(std::string lang) { this->lang = lang; }
private:
	float scale = 0.0f;
	std::string lang = "";
};