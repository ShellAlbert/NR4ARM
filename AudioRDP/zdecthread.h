#ifndef ZDECTHREAD_H
#define ZDECTHREAD_H

#include <QThread>
#include <QByteArray>
#include <QTcpSocket>
#include <QQueue>
#include <QSemaphore>
class ZDecThread : public QThread
{
    Q_OBJECT
public:
    ZDecThread();
    qint32 ZStartThread(QQueue<QByteArray> *queue,QSemaphore *used,QSemaphore *notUsed,///<
                        QQueue<QByteArray> *queuePlay,QSemaphore *semaUsedPlay,QSemaphore *semaFreePlay);
    qint32 ZStopThread();
protected:
    void run();
private:
    QQueue<QByteArray> *m_queueRecv;
    QSemaphore *m_semaUsedRecv;
    QSemaphore *m_semaFreeRecv;

    QQueue<QByteArray> *m_queuePlay;
    QSemaphore *m_semaUsedPlay;
    QSemaphore *m_semaFreePlay;
    bool m_bExitFlag;
};

#endif // ZDECTHREAD_H
