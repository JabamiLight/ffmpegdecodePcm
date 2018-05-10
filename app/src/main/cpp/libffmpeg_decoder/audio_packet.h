//
// Created by TY on 2018/5/9.
//

#ifndef FFMPEGDECODER_AUDIO_PACKET_H
#define FFMPEGDECODER_AUDIO_PACKET_H


class AudioPacket {

public:
    static const int AUDIO_PACKET_ACTION_PLAY=0;
    static const int AUDIO_PACKET_ACTION_PAUSE=100;
    static const int AUDIO_PACKET_ACTION_SEEK=101;

    short* buffer;
    int size;
    float position;
    int action;

    float extra_param1;
    float extra_param2;


    AudioPacket(){
        buffer= nullptr;
        size=0;
        position=-1;
        action=0;
        extra_param1=0;
        extra_param2=0;
    }

    virtual ~AudioPacket() {
        if(nullptr!=buffer){
            delete [] buffer;
            buffer= nullptr;
        }
    }
};


#endif //FFMPEGDECODER_AUDIO_PACKET_H
