#include "zrecvthread.h"
#define MAX_RECV_BUF_SIZE   48000
QByteArray qint32ToQByteArray(qint32 val)
{
    QByteArray ba;
    ba.resize(4);
    ba[3]=(uchar)((0x000000FF & val)>>0);
    ba[2]=(uchar)((0x0000FF00 & val)>>8);
    ba[1]=(uchar)((0x00FF0000 & val)>>16);
    ba[0]=(uchar)((0xFF000000 & val)>>24);
    return ba;
}
qint32 QByteArrayToqint32(QByteArray ba)
{
    qint32 val=0;
    val|=((ba[3]<<0)&0x000000FF);
    val|=((ba[2]<<8)&0x0000FF00);
    val|=((ba[1]<<16)&0x00FF0000);
    val|=((ba[0]<<24)&0xFF000000);
    return val;
}
ZRecvThread::ZRecvThread()
{
    this->m_bExitFlag=false;
}
qint32 ZRecvThread::ZStartThread(QQueue<QByteArray> *queue,QSemaphore *used,QSemaphore *notUsed)
{
    this->m_queueRecv=queue;
    this->m_semaUsedRecv=used;
    this->m_semaFreeRecv=notUsed;
    this->start();
    return 0;
}
qint32 ZRecvThread::ZStopThread()
{
    this->m_bExitFlag=true;
}
void ZRecvThread::run()
{
    this->m_pRecvBuffer=new char[MAX_RECV_BUF_SIZE];
    this->m_nRecvBufLen=0;
    this->m_tcpSocket=new QTcpSocket;
    this->m_tcpSocket->connectToHost("192.168.10.23",6800);
    QObject::connect(this->m_tcpSocket,SIGNAL(connected()),this,SLOT(ZSlotConnected()));
    QObject::connect(this->m_tcpSocket,SIGNAL(readyRead()),this,SLOT(ZSlotReadData()));
    QObject::connect(this->m_tcpSocket,SIGNAL(disconnected()),this,SLOT(ZSlotDisconnect()));
    //enter event-loop,until exit() was called.
    this->exec();
    //do some clean.
    this->m_tcpSocket->close();
    delete this->m_tcpSocket;
    delete [] this->m_pRecvBuffer;
    return;
}
void ZRecvThread::ZSlotConnected()
{
    qDebug()<<"socket connected";
}
void ZRecvThread::ZSlotReadData()
{
    qint64 nMaxRead=MAX_RECV_BUF_SIZE-this->m_nRecvBufLen;
    qint64 nCanRead=0;
    if(this->m_tcpSocket->bytesAvailable()>nMaxRead)
    {
        nCanRead=nMaxRead;
    }else{
        nCanRead=this->m_tcpSocket->bytesAvailable();
    }
    //read data to recv buffer.
    qint32 nRealRead=this->m_tcpSocket->read(this->m_pRecvBuffer+this->m_nRecvBufLen,nCanRead);
    if(nRealRead<0)
    {
        qDebug()<<"<error>:socket read error.";
        this->exit(-1);
        return;
    }
    this->m_nRecvBufLen+=nRealRead;

    //split frames.
    this->ZScanRecvBufferAndSplitFrame();
}
void ZRecvThread::ZSlotDisconnect()
{
    qDebug()<<"socket disconnected";
}
void ZRecvThread::ZScanRecvBufferAndSplitFrame()
{
    qint32 nParsedBytes=0;
    while(1)
    {
        //check length.
        if(this->m_nRecvBufLen-nParsedBytes<4)
        {
            qDebug()<<"we need more data to parse out length.";
            if(nParsedBytes>0)
            {
                memmove(this->m_pRecvBuffer,this->m_pRecvBuffer+nParsedBytes,this->m_nRecvBufLen-nParsedBytes);
                this->m_nRecvBufLen-=nParsedBytes;
                return;
            }
        }

        //frame length.
        QByteArray baOpusFrmLen;
        baOpusFrmLen.append(this->m_pRecvBuffer[nParsedBytes+0]);
        baOpusFrmLen.append(this->m_pRecvBuffer[nParsedBytes+1]);
        baOpusFrmLen.append(this->m_pRecvBuffer[nParsedBytes+2]);
        baOpusFrmLen.append(this->m_pRecvBuffer[nParsedBytes+3]);
        nParsedBytes+=4*sizeof(char);
        //frame data.
        qint32 nOpusFrmLen=QByteArrayToqint32(baOpusFrmLen);
        if(nOpusFrmLen<=0)
        {
            qDebug()<<"<error>:opus frm length <=0 ,reset recv buffer!";
            this->m_nRecvBufLen=0;
            return;
        }

        //check the remaining data length.
        if((this->m_nRecvBufLen-nParsedBytes)<nOpusFrmLen)
        {
            qDebug()<<"we need more data,process next time.";
            //we keep the 4 bytes length to process next time.
            nParsedBytes-=4*sizeof(char);
            memmove(this->m_pRecvBuffer,this->m_pRecvBuffer+nParsedBytes,this->m_nRecvBufLen-nParsedBytes);
            this->m_nRecvBufLen-=nParsedBytes;
            return;
        }

        //put to queue.
        QByteArray baOpusFrm(this->m_pRecvBuffer+nParsedBytes,nOpusFrmLen);
        nParsedBytes+=nOpusFrmLen;

        //put opus pkt to queue.
        this->m_semaFreeRecv->acquire();//-1.
        this->m_queueRecv->enqueue(baOpusFrm);
        this->m_semaUsedRecv->release();//+1.
    }
}
