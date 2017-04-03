#ifndef HUD_OBJECT_H
#define HUD_OBJECT_H

#include <glm/glm.hpp>

//used for rendering UI
class HUDObject {
public:
	HUDObject(glm::vec3 posN, glm::vec3 scaleN, std::string displayN) {
		pos = posN;
		scale = scaleN;
		display = displayN;
	};
	glm::vec3 getPos() { return pos; }
	glm::vec3 getScale() { return scale; }
	std::string getDisplay() { return display; }

private:
	glm::vec3 pos;
	glm::vec3 scale;
	std::string display;
};

#endif
