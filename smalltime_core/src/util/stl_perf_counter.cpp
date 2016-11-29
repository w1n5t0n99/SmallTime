#include "../../include/util/stl_perf_counter.h"

//==============================================
// Ctor - set counter name
//==============================================
StlPerfCounter::StlPerfCounter(std::string name) : m_name(name), m_startPoint(), m_endPoint(), m_on(false)
{

}

//=======================================================
// Set start timepoint
//========================================================
void StlPerfCounter::StartCounter()
{
    if(m_on)
        return;

    m_startPoint = std::chrono::high_resolution_clock::now();
    m_endPoint = m_startPoint;
    m_on = true;

}

//======================================================
// set end timepoint
//==========================================================
void StlPerfCounter::EndCounter()
{
    if(!m_on)
        return;

    m_endPoint = std::chrono::high_resolution_clock::now();
    m_on = false;

}

//=========================================================
// return elapseded time in microseconds
//=========================================================
double StlPerfCounter::GetElapsedMicroseconds()
{
    if(m_on)
        return 0.0;

    std::chrono::duration<double,  std::micro> fpUs = m_endPoint - m_startPoint;
    return fpUs.count();

}

//=========================================================
// return elapseded time in mmilliseconds
//=========================================================
double StlPerfCounter::GetElapsedMilliseconds()
{
    if(m_on)
        return 0.0;

    std::chrono::duration<double,  std::milli> fpUs = m_endPoint - m_startPoint;
    return fpUs.count();

}

//======================================================
// return counter name
//=====================================================
std::string StlPerfCounter::GetName()
{
    return m_name;
}

