#include "stdafx.h"
#include "../stb_image/stb_image.h"
#include "SimpleTexture.h"
#include <utility>
#include <cassert>
#include <iostream>

SimpleTexture::SimpleTexture(const char* fileName)
{
	assert(LoadTexture(fileName), "Failed to Load Texture");
}


void SimpleTexture::Sample(float u, float v, float& r, float& g, float& b)
{
    // Ensure u and v are within the range [0, 1]
    u = std::min(1.0f, std::max(u, 0.0f));
    v = std::min(1.0f, std::max(v, 0.0f));
    assert(0.0 <= u && u <= 1.0 && 0.0 <= v && v <= 1.0, "u v not within [0, 1]");

    // Convert u and v to pixel coordinates
    int x = static_cast<int>(u * mtexWidth);
    int y = static_cast<int>(v * mtexHeight);

    // Ensure x and y do not exceed image dimensions
    x = std::min(x, mtexWidth - 1);
    y = std::min(y, mtexHeight - 1);

    // Calculate the offset for the pixel
    int offset = (y * mtexWidth + x) * 4; // 4 channels per pixel (RGBA)

    // Extract the RGB values
    r = texture->data[offset] / 255.0f;
    g = texture->data[offset + 1] / 255.0f;
    b = texture->data[offset + 2] / 255.0f;
}

bool SimpleTexture::LoadTexture(const char* filename)
{
	int channels;
	unsigned char * data = stbi_load(filename, &mtexWidth, &mtexHeight, &channels, 4);
	if (data) {
        texture = std::make_shared<StbTextureWrapper>(data);
		return true;
	}
	return false;
}

StbTextureWrapper::~StbTextureWrapper()
{
    stbi_image_free(data);
}
