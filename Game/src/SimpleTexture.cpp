#include "stdafx.h"
#include "../stb_image/stb_image.h"
#include "SimpleTexture.h"
#include <utility>
#include <cassert>
#include <iostream>


// Custom deleter function for stbi_image_free
struct STBImageDeleter {
    void operator()(unsigned char* data) const {
        stbi_image_free(data);
    }
};


SimpleTexture::SimpleTexture(const char* fileName)
{
    bool texLoaded = LoadTexture(fileName);
	assert(texLoaded, "Failed to Load Texture");
}


void SimpleTexture::Sample(float u, float v, float& r, float& g, float& b)
{
    // Convert u and v to pixel coordinates
    int x = static_cast<int>(u * mtexWidth);
    int y = static_cast<int>(v * mtexHeight);

    // Ensure x and y do not exceed image dimensions
    x = std::min(x, mtexWidth - 1);
    y = std::min(y, mtexHeight - 1);

    // Calculate the offset for the pixel
    int offset = (y * mtexWidth + x) * 4; // 4 channels per pixel (RGBA)
    // Extract the RGB values
    r = texture[offset];
    g = texture[offset + 1];
    b = texture[offset + 2];
}

bool SimpleTexture::LoadTexture(const char* filename)
{
	int channels;
    int height;
	unsigned char* data = stbi_load(filename, &mtexWidth, &mtexHeight, &channels, 4);
	if (data) {
        texture = std::shared_ptr<unsigned char[]>(data, STBImageDeleter());
		return true;
	}
	return false;
}