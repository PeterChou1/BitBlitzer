#pragma once
#include <memory>


class SimpleTexture {
public:
	SimpleTexture() : texture(nullptr), mtexWidth(0), mtexHeight(0) {};
	SimpleTexture(const char* fileName);
	void Sample(float u, float v, float& r, float& g, float& b);
	std::shared_ptr<unsigned char[]> texture;
private:
	bool LoadTexture(const char* filename);
	int mtexWidth;
	int mtexHeight;
};

