#pragma once

#include <QThread.h>
#include <QWaitCondition>

class NetworkThread : public QThread
{
    Q_OBJECT

protected:
    QWaitCondition m_transferStarted;

public:
    virtual ~NetworkThread()
    {}

    void startTransfer()
    {
        m_transferStarted.wakeAll();
    }

signals:
    void progressChange(int percentDone);
    void log(const QString &msg);
};
