#pragma once
#ifndef _STLPERFCOUNTER_
#define _STLPERFCOUNTER_

#include <string>
#include <chrono>

class StlPerfCounter
{
public:
    StlPerfCounter(std::string name);

    void StartCounter();
    void EndCounter();

    std::string GetName();

    double GetElapsedMicroseconds();
    double GetElapsedMilliseconds();

private:
    typedef std::chrono::high_resolution_clock::time_point tp;

    std::string m_name;
    tp m_startPoint;
    tp m_endPoint;
    bool m_on;
};



#endif // _STLPERFCOUNTER_
