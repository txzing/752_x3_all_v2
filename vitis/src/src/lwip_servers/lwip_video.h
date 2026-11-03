#ifndef __LWIP_VIDEO_H__

#if defined (UDP_VIDEO) || defined (TCP_VIDEO)

#define __LWIP_VIDEO_H__


#define HEADER_SIZE 			sizeof(packet_header)
#define HEADER_ID      			0xAA5555AA
typedef struct __attribute__((packed))
{
 u32 HEAD_ID;//0xAA5555AA
 u32 Channel_ID;//图像通道
 u32 SEND_FLAG;//发送标志，0：图片，1：视频，2：错误帧
 u32 Width;////图像宽带
 u32 Height;////图像宽带
 u32 total;      //一张图片总字节数   例如  19200*1080*3
 u32 offset;     //当前数据偏移量   例如，第一帧数据为0， 第n帧数据为 (n-1)*framesize
 u32 picseq;     //图片序号，第几张图片
 u32 frameseq;   //一张图片发送的帧序号，当前图片的第n帧 ,从1~ 1920*3
 u32 framesize;  //当前帧图片数据大小 例如每一次都发有效图片数据大小为1200
}packet_header;



extern packet_header packet_p;
extern u8 err_ch;

void lwip_video_transfer(void);
int handle_err_pic_transfer(int channel);
int handle_channel_transfer(void);


#endif //#if defined (UDP_VIDEO) || defined (TCP_VIDEO)
#endif // #ifndef __LWIP_VIDEO_H__
