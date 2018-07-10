#include "zdecthread.h"
#include <opus/opus.h>
#include <opus/opus_multistream.h>
#include <opus/opus_defines.h>
#include <opus/opus_types.h>
#include <zgblparam.h>
ZDecThread::ZDecThread()
{
    this->m_bExitFlag=false;
}
qint32 ZDecThread::ZStartThread(QQueue<QByteArray> *queue,QSemaphore *used,QSemaphore *notUsed,///<
                                QQueue<QByteArray> *queuePlay,QSemaphore *semaUsedPlay,QSemaphore *semaFreePlay)
{
    this->m_queueRecv=queue;
    this->m_semaUsedRecv=used;
    this->m_semaFreeRecv=notUsed;

    this->m_queuePlay=queuePlay;
    this->m_semaUsedPlay=semaUsedPlay;
    this->m_semaFreePlay=semaFreePlay;

    this->start();
    return 0;
}
qint32 ZDecThread::ZStopThread()
{
    this->m_bExitFlag=true;
    return 0;
}
void ZDecThread::run()
{
    char *pcmData=new char[BLOCK_SIZE];
    int err;
    OpusMSDecoder *decoder;
    int streams=1;
    int coupled_streams=1;
    unsigned char stream_map[255];
    stream_map[0]=0;
    stream_map[1]=1;
    //decoder=opus_decoder_create(SAMPLE_RATE,CHANNELS_NUM,&err);
    decoder=opus_multistream_decoder_create(SAMPLE_RATE,CHANNELS_NUM,streams,coupled_streams,stream_map,&err);
    if(err!=OPUS_OK || decoder==NULL)
    {
        qDebug()<<"<error>:error at opus_decoder_create(),"<<opus_strerror(err);
        return;
    }

    while(!this->m_bExitFlag)
    {
        if(this->m_semaUsedRecv->tryAcquire())//已用信号量减1
        {
            //fetch data.
            QByteArray baOpusData;
            baOpusData=this->m_queueRecv->dequeue();
            this->m_semaFreeRecv->release();//空闲信号量加1.

            qDebug()<<"get opus data:"<<baOpusData.size();

            qint32 nDecodeSamples=opus_multistream_decode(decoder,(const unsigned char*)baOpusData.data(),baOpusData.size(),(opus_int16*)pcmData,OPUS_BLKFRM_SIZEx2,0);
            if(nDecodeSamples<0)
            {
                qDebug()<<"decode error:"<<opus_strerror(nDecodeSamples);
            }else{
                qint32 nDecodeBytes=nDecodeSamples*CHANNELS_NUM*sizeof(opus_int16);
                qDebug()<<"decode:"<<baOpusData.size()<<"->"<<nDecodeBytes;

                //put data to play queue.
                this->m_semaFreePlay->acquire();//空闲信号量减1
                QByteArray baPCMData(pcmData,nDecodeBytes);
                this->m_queuePlay->enqueue(baPCMData);
                this->m_semaUsedPlay->release();//已用信号量加1.
            }
        }
    }
    opus_multistream_decoder_destroy(decoder);
}
