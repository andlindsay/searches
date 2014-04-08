#ifndef TIMER_H
#define TIMER_H

#include <time.h>

class Timer
{
public:
	Timer(): m_start(0), m_stop(0) {}
	
	void start()
	{ 
		time(&m_start);
		m_running = true; 
	}
	
	void stop()
	{ 
		time(&m_stop);
		m_running = false; 
	}
	
	bool isRunning(){ return m_running; }
	
	time_t lapTime()
	{
		m_lastLap = m_lap;
		time(&m_lap);
		return m_lap - m_lastLap;
	}
	
	double elapsed()
	{
		if( !m_running )
			return difftime(m_stop, m_start);
		else
		{
			time_t current;
			time(&current);
			return current - m_start;
		}
	}
	
private:
	time_t m_start, m_stop, m_lap, m_lastLap;
	bool m_running;
};

#endif