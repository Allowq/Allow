#ifndef NETWORKTRANSFER_H
#define NETWORKTRANSFER_H

#include <QtGui>
//#include "ui_networktransferclass.h"

class NetworkThread;

class NetworkTransfer : QObject
{
    Q_OBJECT

public:
    NetworkTransfer(const QString&, const quint16&, const qint64&);
    ~NetworkTransfer();

protected slots:
    void progressChange(int progress);
    void logMessage(const QString &msg);
    void startTransfer();
    void connectionLost();

private:
    NetworkThread *m_net;

};

#endif // NETWORKTRANSFER_H
