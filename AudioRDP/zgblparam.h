#ifndef ZGBLPARAM_H
#define ZGBLPARAM_H
//当声卡工作时，数据总是连续地在硬件缓冲区与应用程序之间传输。
//在录音时，如果应用程序读取数据不够快，将导致缓冲区旧数据被新数据覆盖，这种数据丢失叫overrun.
//在回放时，如果应用程序填充硬件缓冲区速度不够快，将导致缓冲区被饿死，这种现在叫underrun.

//* The sample rate of the audio codec **
#define     SAMPLE_RATE      48000

//frame帧是播放样本的一个计量单位，由通道数和比特数决定。
//立体声48KHz 16-bit的PCM，那么一帧的大小就是4字节(2 Channels*16-bit=32bit/8bit=4 bytes)
//5.1通道48KHz 16-bit的PCM，那么一帧的大小就是12字节(5.1这里取6,6Channels*16bit=96bit/8bit=12 bytes)
#define CHANNELS_NUM    2
#define BYTES_PER_FRAME 4

//period:周期，是指每两个硬件中断之间的帧数，poll会在每个周期返回一次.
//alsa将内部的缓冲区拆分成period(周期）又叫fragments（片断）
//alsa以period为单元来传送数据，一个period存储一些frames,一个frames中包含了左右声道的数据采样点.
//硬件的buffer被拆分成period，一个period包含多个frame，一个frame包含多个数据采样点.
#define ALSA_PERIOD     4


//立体声，16-bit采样位数，44.1KHz采样率
//立体声，ChannelNum=2
//1次模拟采样就是16-bit（2字节），因为是双通道，所以是4字节
//1个frame中最小的传输单位，
//1 frame=(ChannelNum)*(1 sample in bytes)=(2通道）×16bit=4字节。
//为了达到2*44.1KHz的采样率，系统必须达到该值的传输速率
//传输速率=(ChannelNum)*(1 sample in bytes)*(SampleRate)
//=2通道*一次采样16bit×44.1KHz采样率
//=2×16bit×44100Hz=1411200bits_per_seconds=176400 Bytes_per_second.
//现在，如果ALSA每秒中断一次，那么我们就至少需要准备好176400字节数据发送给声卡，才不会导致播放卡
//如果中断每500ms发生一次，那么我们就至少需要176400Bytes/(1s/500ms)=88200bytes的数据
//如果中断每100ms发生一次，那么我们就至少要准备176400Bytes/(1s/100ms)=17640Bytes的数据
///////////////////////////////////////////////////////////////////////////////////
//这里我们使用48KHz的采样率，双声道，采样位数16bit
//则有bps=2*16bit*48000=1536000bits/s=192000Bytes
//这里我们设置period为4，即1秒发生4次中断，则中断间隔为1s/4=250ms.
//则每次中断发生时，我们至少需要填充192000Bytes/(1s/250ms)=48000Bytes
#define     BLOCK_SIZE        48000	// Number of bytes


#define TCP_PORT    6800

//for opus encode/decode.
#define OPUS_SAMPLE_FRMSIZE     (2880) //frame size in 16 bit sample.
#define OPUS_BLKFRM_SIZEx2      (OPUS_SAMPLE_FRMSIZE*CHANNELS_NUM*sizeof(opus_int16)) //2 channels.
#define OPUS_BITRATE            64000
#endif // ZGBLPARAM_H
