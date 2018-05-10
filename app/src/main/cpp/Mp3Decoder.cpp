#include <jni.h>
#include <string>
#include "com_example_ty_ffmpegdecoder_Mp3Decoder.h"
#include "libffmpeg_decoder/accompany_decoder_controller.h"

AccompanyDecoderController* decoderController;

JNIEXPORT jint JNICALL Java_com_example_ty_ffmpegdecoder_Mp3Decoder_init
        (JNIEnv *env, jobject jobj, jstring mp3PathParam, jstring pcmPathParam) {
    const char *mp3path = env->GetStringUTFChars(mp3PathParam, JNI_FALSE);
    const char *pcmpath = env->GetStringUTFChars(pcmPathParam, JNI_FALSE);
    decoderController=new AccompanyDecoderController();
    decoderController->Init(mp3path,pcmpath);
    env->ReleaseStringUTFChars(mp3PathParam, mp3path);
    env->ReleaseStringUTFChars(pcmPathParam, pcmpath);
    return 0;
}

JNIEXPORT void JNICALL Java_com_example_ty_ffmpegdecoder_Mp3Decoder_decode
        (JNIEnv *, jobject) {
    LOGI("enter Java_com_phuket_tour_decoder_Mp3Decoder_decode...");
    if(decoderController) {
        decoderController->Decode();
    }
    LOGI("leave Java_com_phuket_tour_decoder_Mp3Decoder_decode...");
}

JNIEXPORT void JNICALL Java_com_example_ty_ffmpegdecoder_Mp3Decoder_destroy
        (JNIEnv *, jobject) {
    if(decoderController){
        decoderController->Destory();
        delete decoderController;
        decoderController= nullptr;
    }

}