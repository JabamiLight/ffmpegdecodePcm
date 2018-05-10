//
// Created by TY on 2018/5/9.
//

#include "accompany_decoder.h"


AccompanyDecoder::AccompanyDecoder() {

    seek_seconds = 0.0f;
    seek_req = false;
    seek_resp = false;
    accompanyFilePath = nullptr;
}

AccompanyDecoder::~AccompanyDecoder() {

    if (nullptr != accompanyFilePath) {
        delete[] accompanyFilePath;
        accompanyFilePath = nullptr;
    }

}


int AccompanyDecoder::GetMusicMeta(const char *fileString, int *metaData) {
    Init(fileString);
    int sampleRate = avCodecContext->sample_rate;
    LOGI("sampleRate is %d", sampleRate);
    int bitRate = avCodecContext->bit_rate;
    LOGI("bitRate is %d", bitRate);
    Destroy();
    metaData[0] = sampleRate;
    metaData[1] = bitRate;
    return 0;
}

int AccompanyDecoder::Init(const char *fileString, int packetBufferSizeParam) {
    Init(fileString);
    packetBufferSize = packetBufferSizeParam;
    return 0;
}

int AccompanyDecoder::Init(const char *audioFile) {
    LOGI("enter AccompanyDecoder::init");
    audioBuffer = nullptr;
    position = -1.0f;
    audioBufferCursor = 0;
    audioBufferSize = 0;
    swrContext = nullptr;
    swrBuffer = nullptr;
    swrBufferSize = 0;
    seek_success_read_frame_success = true;
    isNeedFirstFrameCorrectFlag = true;
    firstFrameCorrectionInSecs = 0.0f;
   LOGI("avcodec_version() %d",avcodec_version()) ;
    avcodec_register_all();
    av_register_all();

    avFormatContext = avformat_alloc_context();
    // 打开输入文件
    LOGI("open accompany file %s....", audioFile);

    if (nullptr == audioFile) {
        int length = strlen(audioFile);
        accompanyFilePath = new char[length + 1];
        memset(accompanyFilePath, 0, length + 1);
        memcpy(accompanyFilePath, audioFile, length + 1);
        return -1;
    }
    int result = avformat_open_input(&avFormatContext, audioFile, nullptr, nullptr);
    if (result != 0) {
        LOGI("can't open file %s result is %d", audioFile, result);
        return -1;
    } else {
        LOGI("open file %s success and result is %d", audioFile, result);
    }

    avFormatContext->max_analyze_duration = 50000;
    //检查在文件中的流的信息
    result = avformat_find_stream_info(avFormatContext, nullptr);
    if (result < 0) {
        LOGI("fail avformat_find_stream_info result is %d", result);
        return -1;
    } else {
        LOGI("sucess avformat_find_stream_info result is %d", result);
    }
    stream_index = av_find_best_stream(avFormatContext, AVMEDIA_TYPE_AUDIO, -1, -1, nullptr, 0);
    LOGI("stream_index is %d", stream_index);
    //未找到音频
    if (stream_index < 0) {
        LOGI("no audio stream");
        stream_index = 0;
        return -1;
    }
    //拿到音频流
    AVStream *audioStream = avFormatContext->streams[stream_index];
    if (audioStream->time_base.den && audioStream->time_base.num) {
        timeBase = av_q2d(audioStream->time_base);
    } else if (audioStream->codec->time_base.den && audioStream->codec->time_base.num) {
        timeBase = av_q2d(audioStream->codec->time_base);
    }
    //获得音频流的解码器上下文
    avCodecContext = audioStream->codec;
    // 根据解码器上下文找到解码器
    LOGI("avCodecContext->codec_id is %d AV_CODEC_ID_AAC is %d", avCodecContext->codec_id,
         AV_CODEC_ID_AAC);
    AVCodec *avCodec = avcodec_find_decoder(avCodecContext->codec_id);
    if (avCodec == nullptr) {
        LOGI("Unsupported codec ");
        return -1;
    }
    //打开解码器
    result = avcodec_open2(avCodecContext, avCodec, nullptr);
    if (result < 0) {
        LOGI("fail avformat_find_stream_info result is %d", result);
        return -1;
    } else {
        LOGI("sucess avformat_find_stream_info result is %d", result);
    }
    //4判断是否需要resampler
    if (!AudioCodecISSupported()) {
        LOGI("because of audio Codec Is Not Supported so we will init swresampler...");
        /**
		 * 初始化resampler
		 * @param s               Swr context, can be NULL
		 * @param out_ch_layout   output channel layout (AV_CH_LAYOUT_*)
		 * @param out_sample_fmt  output sample format (AV_SAMPLE_FMT_*).
		 * @param out_sample_rate output sample rate (frequency in Hz)
		 * @param in_ch_layout    input channel layout (AV_CH_LAYOUT_*)
		 * @param in_sample_fmt   input sample format (AV_SAMPLE_FMT_*).
		 * @param in_sample_rate  input sample rate (frequency in Hz)
		 * @param log_offset      logging level offset
		 * @param log_ctx         parent logging context, can be NULL
		 */
        swrContext = swr_alloc_set_opts(nullptr,
                                        av_get_default_channel_layout(OUT_PUT_CHANNELS),
                                        AV_SAMPLE_FMT_S16,
                                        avCodecContext->sample_rate,
                                        avCodecContext->channel_layout,
                                        avCodecContext->sample_fmt,
                                        avCodecContext->sample_rate,
                                        0,
                                        nullptr
        );
        if (!swrContext || swr_init(swrContext)) {
            if (swrContext) {
                swr_free(&swrContext);
            }
            avcodec_close(avCodecContext);
            LOGI("init resampler failed...");
            return -1;
        }


    }
    LOGI("channels is %d sampleRate is %d", avCodecContext->channels, avCodecContext->sample_rate);
    pAudioFrame = avcodec_alloc_frame();

    return 0;

}

int AccompanyDecoder::ReadSameple(short *samples, int size) {
    if (seek_req) {
        audioBufferCursor = audioBufferSize;
    }
    //计算的
    int sampleSize = size;
    while (size > 0) {
        LOGI("size %d",size);
        LOGI("audioBufferCursor %d audioBufferSize %d",audioBufferCursor ,audioBufferSize);
        if (audioBufferCursor < audioBufferSize) {
            int audioBufferDataSize = audioBufferSize - audioBufferCursor;
            int copySize = MIN(size, audioBufferDataSize);
            memcpy(samples + (sampleSize - size), audioBuffer + audioBufferCursor, copySize * 2);
            size -= copySize;
            audioBufferCursor += copySize;
        } else {
            LOGI("readFrame");
            if (ReadFrame() < 0) {
                break;
            }
        }
    }
    int fillSize = sampleSize - size;
    if (fillSize == 0) {
        return -1;
    }

    return fillSize;
}

int AccompanyDecoder::ReadFrame() {
    if (seek_req) {
        this->Seek_frame();
    }
    int ret = 1;
    av_init_packet(&packet);
    int gotframe = 0;
    int readFrameCode = -1;
    while (true) {
        readFrameCode = av_read_frame(avFormatContext, &packet);
        if (readFrameCode >= 0) {
            if (packet.stream_index == stream_index) {
                int len = avcodec_decode_audio4(avCodecContext, pAudioFrame, &gotframe, &packet);
                if (len < 0) {
                    LOGI("decode audio error, skip packet");
                }
                if (gotframe) {
                    int numChannels = OUT_PUT_CHANNELS;
                    int numFrames = 0;
                    void *audioData;
                    //需要转换数据
                    if (swrContext) {
                        //提供更多的转换输出空间
                        const int ratio = 2;
                        const int bufSize = av_samples_get_buffer_size(
                                nullptr,
                                numChannels,
                                pAudioFrame->nb_samples * ratio,
                                AV_SAMPLE_FMT_S16,
                                1
                        );
                        if (!swrBuffer || swrBufferSize < bufSize) {
                            swrBufferSize = bufSize;
                            swrBuffer = realloc(swrBuffer, swrBufferSize);
                        }
                        byte *outbuf[2] = {(byte *) swrBuffer, nullptr};
                        numFrames = swr_convert(swrContext,
                                                outbuf,
                                                pAudioFrame->nb_samples * ratio,
                                                (const uint8_t **) pAudioFrame->data,
                                                pAudioFrame->nb_samples
                        );
                        if (numFrames < 0) {
                            LOGI("fail resample audio");
                            ret = -1;
                            break;
                        }
                        audioData = swrBuffer;
                    } else {
                        if (avCodecContext->sample_fmt != AV_SAMPLE_FMT_S16) {
                            LOGI("bucheck, audio format is invalid");
                            ret = -1;
                            break;
                        }
                        audioData=pAudioFrame->data[0];
                        numFrames=pAudioFrame->nb_samples;
                    }
                    if(isNeedFirstFrameCorrectFlag&&position>=0){
                        float expectedPosition=position+duration;
                        float actualPosition=av_frame_get_best_effort_timestamp(pAudioFrame)*timeBase;
                        firstFrameCorrectionInSecs=actualPosition-expectedPosition;
                        isNeedFirstFrameCorrectFlag=false;
                    }
                    duration=av_frame_get_pkt_duration(pAudioFrame)*timeBase;
                    position = av_frame_get_best_effort_timestamp(pAudioFrame) * timeBase - firstFrameCorrectionInSecs;
                    if(!seek_success_read_frame_success){
                        LOGI("position is %.6f", position);
                        actualSeekPosition=position;
                        seek_success_read_frame_success=true;
                    }
                    audioBufferSize=numFrames*numChannels;
                    audioBuffer=(short*)audioData;
                    audioBufferCursor=0;
                    break;
                }
            }

        } else {
            ret = -1;
            break;
        }
    }
    av_free_packet(&packet);

    return ret;
}

bool AccompanyDecoder::AudioCodecISSupported() {
    if (avCodecContext->sample_fmt == AV_SAMPLE_FMT_S16) {
        return true;
    }
    return false;
}


AudioPacket *AccompanyDecoder::DecodePacket() {
    short *samples = new short[packetBufferSize];
    int stereoSampleSize = ReadSameple(samples, packetBufferSize);
    AudioPacket *samplePacket = new AudioPacket();
    if (stereoSampleSize > 0) {
        //构造成一个packet
        samplePacket->buffer = samples;
        samplePacket->size = stereoSampleSize;
        /** 这里由于每一个packet的大小不一样有可能是200ms 但是这样子position就有可能不准确了 **/
        samplePacket->position = position;;

    } else {
        samplePacket->size = -1;
    }
    return samplePacket;
}

void AccompanyDecoder::Destroy() {
    if (nullptr != swrBuffer) {
        free(swrBuffer);
        swrBuffer = nullptr;
        swrBufferSize = 0;
    }
    if (nullptr != swrContext) {
        swr_free(&swrContext);
        swrContext = nullptr;
    }
    if (nullptr != pAudioFrame) {
        av_free(pAudioFrame);
        pAudioFrame = nullptr;
    }
    if (nullptr != avCodecContext) {
        avcodec_close(avCodecContext);
        avCodecContext = nullptr;
    }
    if (nullptr != avFormatContext) {
        LOGI("leave LiveReceiver::destory");
        avformat_close_input(&avFormatContext);
    }

}

void AccompanyDecoder::Seek_frame() {
    LOGI("\n seek frame firstFrameCorrectionInSecs is %.6f, seek_seconds=%f, position=%f \n",
         firstFrameCorrectionInSecs, seek_seconds, position);
    float targetPosition = seek_seconds;
    float currentPosition = position;
    float frameDuration = duration;
    if (targetPosition < currentPosition) {
        this->Destroy();
        this->Init(accompanyFilePath);
        currentPosition = 0.0;
    }
    int readFrameCode = -1;
    while (true) {
        av_init_packet(&packet);

    }

}
