#pragma once

template<typename Type>
typename std::enable_if<std::is_integral<Type>::value, Type>::type
getRange(Type min, Type max, float width){
    double range = max - min;
    range /= width*4;
    return std::max(range,1.0);
}

template<typename Type>
typename std::enable_if<std::is_floating_point<Type>::value, Type>::type
getRange(Type min, Type max, float width){
    double range = max - min;
    range /= width*4;
    return range;
}

template<typename Type>
std::string toString(Type t){
    return ofToString(t);
}

template<>
std::string toString(uint8_t t){
    return ofToString((int) t);
}

template<>
std::string toString(int8_t t){
    return ofToString((int) t);
}

template<>
std::string toString(std::string t){
    return t;
}

inline static ofMesh rectangle(const ofRectangle & r, const ofFloatColor & c){
	ofMesh mesh;
	mesh.addVertex(r.position);
	mesh.addVertex(glm::vec3(r.x + r.width, r.y, 0));
	mesh.addVertex(glm::vec3(r.x + r.width, r.y + r.height, 0));

	mesh.addVertex(glm::vec3(r.x + r.width, r.y + r.height, 0));
	mesh.addVertex(glm::vec3(r.x, r.y + r.height, 0));
	mesh.addVertex(glm::vec3(r.x, r.y, 0));

	mesh.addColor(c);
	mesh.addColor(c);
	mesh.addColor(c);

	mesh.addColor(c);
	mesh.addColor(c);
	mesh.addColor(c);

	return mesh;
}
