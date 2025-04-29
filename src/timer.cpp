#include "timer.h"
#include <iostream>

Timer::Timer(const std::string& name) : m_name(name), m_start(std::chrono::high_resolution_clock::now()) {}

Timer::~Timer() {
    if (!m_stopped) {
        stop();
    }
}

void Timer::stop() {
    m_end = std::chrono::high_resolution_clock::now();
    m_stopped = true;
    
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(m_end - m_start).count();
    std::cout << m_name << " took " << duration << "ms\n";
}

double Timer::elapsedMilliseconds() const {
    auto end = m_stopped ? m_end : std::chrono::high_resolution_clock::now();
    return std::chrono::duration_cast<std::chrono::milliseconds>(end - m_start).count();
}
