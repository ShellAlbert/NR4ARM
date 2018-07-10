#ifndef ZMAINUI_H
#define ZMAINUI_H

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QQueue>
#include <QSemaphore>
#include <zrecvthread.h>
#include <zdecthread.h>
#include <zaudioplaythread.h>
class ZMainUI : public QWidget
{
    Q_OBJECT

public:
    ZMainUI(QWidget *parent = 0);
    ~ZMainUI();

private:
    QLabel *m_llTitle;
    QVBoxLayout *m_vLayout;

    QQueue<QByteArray> *m_queueRecv;
    QSemaphore *m_semaUsedRecv;
    QSemaphore *m_semaFreeRecv;

    QQueue<QByteArray> *m_queuePlay;
    QSemaphore *m_semaUsedPlay;
    QSemaphore *m_semaFreePlay;

private:
    ZRecvThread *m_threadRecv;
    ZDecThread *m_threadDec;
    ZAudioPlayThread *m_threadPlay;
};

#endif // ZMAINUI_H
