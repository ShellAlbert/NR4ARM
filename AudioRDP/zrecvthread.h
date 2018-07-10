#ifndef ZRECVTHREAD_H
#define ZRECVTHREAD_H

#include <QThread>
#include <QByteArray>
#include <QTcpSocket>
#include <QQueue>
#include <QSemaphore>
class ZRecvThread : public QThread
{
    Q_OBJECT
public:
    ZRecvThread();
    qint32 ZStartThread(QQueue<QByteArray> *queue,QSemaphore *used,QSemaphore *notUsed);
    qint32 ZStopThread();
protected:
    void run();
private slots:
    void ZSlotConnected();
    void ZSlotReadData();
    void ZSlotDisconnect();
private:
    void ZScanRecvBufferAndSplitFrame();
private:
    QTcpSocket *m_tcpSocket;
    QQueue<QByteArray> *m_queueRecv;
    QSemaphore *m_semaUsedRecv;
    QSemaphore *m_semaFreeRecv;
    bool m_bExitFlag;
private:
    char *m_pRecvBuffer;
    qint32 m_nRecvBufLen;
};

#endif // ZRECVTHREAD_H
