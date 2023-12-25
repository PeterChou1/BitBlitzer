#pragma once
#include <memory>


class DepthBuffer {
public:
	DepthBuffer() : m_height(0), m_width(0) {}

	DepthBuffer(int width, int height) : m_height(height), m_width(width) {
		m_DepthBuffer = std::shared_ptr<float[]>(new float[(height + 1) * (width + 1)]);
	}

	void SetBuffer(int x, int y, float depth) {
		assert(0 <= x && x <= m_width && 0 <= y && y <= m_height, "out of bounds index");
		m_DepthBuffer[y * m_width + x] = depth;
	}

	float GetBuffer(int x, int y) {
		assert(0 <= x && x <= m_width && 0 <= y && y <= m_height, "out of bounds index");
		return m_DepthBuffer[y * m_width + x];
	}

	void ClearBuffer() {
		for (int i = 0; i < m_height * m_width; i++) {
			m_DepthBuffer[i] = 0.0f;
		}
	}
private:
	int m_height;
	int m_width;
	std::shared_ptr<float[]> m_DepthBuffer;
};