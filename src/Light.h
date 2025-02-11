#ifndef __LIGHT
#define __LIGHT

class Light
{
public:
	Vector3 getPosition() const { return this->position; }
	void setPosition(Vector3 position) { this->position = position; }

	size_t getMaterialId() const { return this->materialId; }
	void setMaterialId(size_t materialId) { this->materialId = materialId; }
	
private:
	Vector3 position;
	size_t materialId;
};

#endif

