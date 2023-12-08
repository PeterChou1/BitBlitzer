#pragma once
#include <memory>


class DepthBuffer {
public:
	DepthBuffer() {}

	DepthBuffer(int height, int width) : m_height(height), m_width(width) {
		mDepthBuffer = std::shared_ptr<float[]>(new float[height * width]);
	}

	void SetBuffer(int x, int y, float depth) {
		mDepthBuffer[y * m_width + x] = depth;
	}

	float GetBuffer(int x, int y) {
		return mDepthBuffer[y * m_width + x];
	}

	void ClearBuffer() {
		for (int i = 0; i < m_height * m_width; i++) {
			mDepthBuffer[i] = 0.0f;
		}
	}
private:
	int m_height;
	int m_width;
	std::shared_ptr<float[]> mDepthBuffer;
};