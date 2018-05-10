//
// Created by TY on 2018/5/9.
//

#ifndef FFMPEGDECODER_ACCOMPANY_DECODER_H
#define FFMPEGDECODER_ACCOMPANY_DECODER_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "../common/CommonTools.h"
#include "audio_packet.h"

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswresample/swresample.h>
};

#ifdef UINT64_C
//#define UINT64_C(value)__CONCAT(value,ULL)
#endif


#ifndef INT64_MIN
#define INT64_MIN  (-9223372036854775807LL - 1)
#endif

#ifndef INT64_MAX
#define INT64_MAX	9223372036854775807LL
#endif

#define OUT_PUT_CHANNELS 2
#define LOG_TAG "AccompanyDecoder"

class AccompanyDecoder {
private:
    bool seek_req;
    bool seek_resp;
    float seek_seconds;

    float actualSeekPosition;

    AVFormatContext *avFormatContext;
    AVCodecContext *avCodecContext;
    int stream_index;
    float timeBase;
    AVFrame *pAudioFrame;
    AVPacket packet;

    char *accompanyFilePath;

    bool seek_success_read_frame_success;
    int packetBufferSize;
    /** 每次解码出来的audioBuffer以及这个audioBuffer的时间戳以及当前类对于这个audioBuffer的操作情况 **/
    short *audioBuffer;
    float position;
    int audioBufferCursor;
    int audioBufferSize;
    float duration;
    bool isNeedFirstFrameCorrectFlag;
    float firstFrameCorrectionInSecs;

    SwrContext *swrContext;
    void *swrBuffer;
    int swrBufferSize;

    int Init(const char *fileString);

    int ReadSameple(short *samples, int size);

    int ReadFrame();

    bool AudioCodecISSupported();

public:

    AccompanyDecoder();

    virtual ~AccompanyDecoder();

    virtual int GetMusicMeta(const char *fileString, int *metaData);

    virtual int Init(const char *fileString, int packetBufferSizeParam);

    virtual AudioPacket *DecodePacket();

    virtual void Destroy();

    void setSeekReq(bool seekReqParam) {
        seek_req = seekReqParam;
        if (seek_req) {
            seek_resp = false;
        }
    };

    bool hasSeekReq() {
        return seek_req;
    };

    bool hasSeekResp() {
        return seek_resp;
    };

    /** 设置到播放到什么位置，单位是秒，但是后边3位小数，其实是精确到毫秒 **/
    void setPosition(float seconds) {
        actualSeekPosition = -1;
        this->seek_seconds = seconds;
        this->seek_req = true;
        this->seek_resp = false;
    };


    float GetActualSeekPosition() {
        float ret = actualSeekPosition;
        if (ret != -1) {
            actualSeekPosition = -1;
        }
        return ret;
    }

    virtual void Seek_frame();


};


#endif //FFMPEGDECODER_ACCOMPANY_DECODER_H
