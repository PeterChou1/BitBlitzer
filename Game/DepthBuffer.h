#pragma once
#include <memory>


class DepthBuffer {
public:
	DepthBuffer(int height, int width) : m_height(height), m_width(width) {
		mDepthBuffer = std::unique_ptr<float[]>(new float[height * width]);
	}

	void SetBuffer(int x, int y, float depth) {
		mDepthBuffer[x * m_width + y] = depth;
	}

	float GetBuffer(int x, int y) {
		return mDepthBuffer[x * m_width + y];
	}

	void ClearBuffer() {
		for (int i = 0; i < m_height * m_width; i++) {
			mDepthBuffer[i] = 0.0f;
		}
	}
private:
	int m_height;
	int m_width;
	std::unique_ptr<float[]> mDepthBuffer;
};