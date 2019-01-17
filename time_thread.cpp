#include "time_thread.h"
#include <QTime>

Time_thread::Time_thread()
{
}

void Time_thread::run()
{
    m_stop=false;
    while(!m_stop)
    {
        emit send_health_data();
        sleep(5);
    }
    quit();
}

void Time_thread::stopThread()
{
    m_stop=true;
}



On_line_thread::On_line_thread()
{
}

void On_line_thread::run()
{
    m_stop=false;
    while(!m_stop)
    {
        emit send_on_line_station();
        sleep(2);
    }
    quit();
}

void On_line_thread::stopThread()
{
    m_stop=true;
}
