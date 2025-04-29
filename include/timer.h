#pragma once

#include <chrono>
#include <string>

class Timer {
public:
    Timer(const std::string& name);
    ~Timer();
    
    void stop();
    double elapsedMilliseconds() const;
    
private:
    std::string m_name;
    std::chrono::time_point<std::chrono::high_resolution_clock> m_start;
    std::chrono::time_point<std::chrono::high_resolution_clock> m_end;
    bool m_stopped = false;
};
