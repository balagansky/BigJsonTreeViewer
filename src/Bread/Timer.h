#pragma once

#include <chrono>

struct Timer {
	Timer() {
		m_Start = std::chrono::high_resolution_clock::now();
	}

	void Reset() { m_Start = std::chrono::high_resolution_clock::now(); }

	double Ms(bool reset = true)
	{
		auto now = std::chrono::high_resolution_clock::now();
		double ms = std::chrono::duration<double>(
			std::chrono::high_resolution_clock::now() - m_Start).count() * 1000;
		if (reset)
			m_Start = now;
		return ms;
	}

	decltype(std::chrono::high_resolution_clock::now()) m_Start;
};