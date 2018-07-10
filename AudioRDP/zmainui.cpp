#include "zmainui.h"

ZMainUI::ZMainUI(QWidget *parent)
    : QWidget(parent)
{
    this->setWindowTitle(tr("Audio Receive/Decode/Playback"));

    this->m_queueRecv=new QQueue<QByteArray>;
    this->m_semaUsedRecv=new QSemaphore(0);
    this->m_semaFreeRecv=new QSemaphore(100);

    this->m_queuePlay=new QQueue<QByteArray>;
    this->m_semaUsedPlay=new QSemaphore(0);
    this->m_semaFreePlay=new QSemaphore(100);

    this->m_threadRecv=new ZRecvThread;
    this->m_threadDec=new ZDecThread;
    this->m_threadPlay=new ZAudioPlayThread("front:CARD=PCH,DEV=0");

    this->m_threadDec->ZStartThread(this->m_queueRecv,this->m_semaUsedRecv,this->m_semaFreeRecv,///<
                                    this->m_queuePlay,this->m_semaUsedPlay,this->m_semaFreePlay);

    this->m_threadRecv->ZStartThread(this->m_queueRecv,this->m_semaUsedRecv,this->m_semaFreeRecv);
    this->m_threadPlay->ZStartThread(this->m_queuePlay,this->m_semaUsedPlay,this->m_semaFreePlay);
}

ZMainUI::~ZMainUI()
{

}
