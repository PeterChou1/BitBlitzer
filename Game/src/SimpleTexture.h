#pragma once
#include <memory>

class StbTextureWrapper {
public:
	StbTextureWrapper(unsigned char* data) : data(data) {}
	~StbTextureWrapper();
	unsigned char* data;
};


class SimpleTexture {
public:
	SimpleTexture() : texture(nullptr), mtexWidth(0), mtexHeight(0) {};
	SimpleTexture(const char* fileName);
	void Sample(float u, float v, float& r, float& g, float& b);
	std::shared_ptr<StbTextureWrapper> texture;
private:
	bool LoadTexture(const char* filename);
	int mtexWidth;
	int mtexHeight;
};

