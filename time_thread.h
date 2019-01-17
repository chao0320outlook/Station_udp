#ifndef TIME_THREAD_H
#define TIME_THREAD_H

#include <QThread>

class On_line_thread : public QThread
{
    Q_OBJECT

private:
    bool m_stop=false;

public:
    On_line_thread();

    void stopThread();
    void run() Q_DECL_OVERRIDE;

signals:
    void send_on_line_station();
};



class Time_thread : public QThread
{
    Q_OBJECT

private:
    bool m_stop=false;

public:
    Time_thread();

    void stopThread();
    void run() Q_DECL_OVERRIDE;

signals:
    void send_health_data();
};


#endif // TIME_THREAD_H
