//
// Created by TY on 2018/5/9.
//

#ifndef FFMPEGDECODER_ACCOMPANY_DECODER_CONTROLLER_H
#define FFMPEGDECODER_ACCOMPANY_DECODER_CONTROLLER_H

#include "unistd.h"
#include "accompany_decoder.h"

#define CHANNEL_PER_FRAME	2
#define BITS_PER_CHANNEL		16
#define BITS_PER_BYTE		8
/** decode data to queue and queue size **/
#define QUEUE_SIZE_MAX_THRESHOLD 25
#define QUEUE_SIZE_MIN_THRESHOLD 20


class AccompanyDecoderController {
protected:
    FILE* pcmFile;

    AccompanyDecoder* accompanyDecoder;

    int accompanySampleRate;
    int accompanyPacketBufferSize;
public:
    AccompanyDecoderController();
    ~AccompanyDecoderController();

    void Init(const char* accompanyPath,const char* pcmFilePath);

    void Decode();

    void Destory();

};


#endif //FFMPEGDECODER_ACCOMPANY_DECODER_CONTROLLER_H
