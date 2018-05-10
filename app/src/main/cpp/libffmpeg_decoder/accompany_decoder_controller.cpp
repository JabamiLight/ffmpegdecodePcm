//
// Created by TY on 2018/5/9.
//

#include "accompany_decoder_controller.h"

AccompanyDecoderController::AccompanyDecoderController() {
    accompanyDecoder = nullptr;
    pcmFile = nullptr;
}

AccompanyDecoderController::~AccompanyDecoderController() {

}

void AccompanyDecoderController::Init(const char *accompanyPath, const char *pcmFilePath) {
    //初始化两个decoder
    AccompanyDecoder *tempDecoder = new AccompanyDecoder();
    int accompanyMetaData[2];
    tempDecoder->GetMusicMeta(accompanyPath, accompanyMetaData);
    delete tempDecoder;
    accompanySampleRate = accompanyMetaData[0];
    int accompanyByteCountPerSec =
            accompanySampleRate * CHANNEL_PER_FRAME * BITS_PER_CHANNEL / BITS_PER_BYTE;
    accompanyPacketBufferSize = static_cast<int>(accompanyByteCountPerSec / 2 * 0.2);
    accompanyDecoder = new AccompanyDecoder();
    accompanyDecoder->Init(accompanyPath, accompanyPacketBufferSize);
    pcmFile = fopen(pcmFilePath, "wb+");

}

void AccompanyDecoderController::Decode() {
    while (true) {
        AudioPacket *accompanyPacket = accompanyDecoder->DecodePacket();
        if (-1 == accompanyPacket->size) {
            break;
        }
        fwrite(accompanyPacket->buffer, sizeof(short), accompanyPacket->size, pcmFile);
    }

}

void AccompanyDecoderController::Destory() {
    if (nullptr != accompanyDecoder) {
        accompanyDecoder->Destroy();
        delete accompanyDecoder;
        accompanyDecoder = nullptr;
    }
    if (nullptr != pcmFile) {
        fclose(pcmFile);
        pcmFile = nullptr;
    }

}
