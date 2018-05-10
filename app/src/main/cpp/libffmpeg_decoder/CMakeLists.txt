
set(PATH_TO_NATIVE ${PATH_TO_MEDIACORE}/src/main/cpp)
set(PATH_TO_PRE_BUILT ${PATH_TO_MEDIACORE}/libs/${ANDROID_ABI})


file(GLOB  FFMPEG_DECODE_SOURCE "*.cpp")#包含当前目录的cpp文件
add_library(  libffmpeg_decoder
             STATIC
            ${FFMPEG_DECODE_SOURCE}
            )

target_link_libraries(  libffmpeg_decoder
                        #导入ffmpeg静态库，这样导入必须使用绝对路径，相对路径失效
                       ${PATH_TO_PRE_BUILT}/libavfilter.a
                       ${PATH_TO_PRE_BUILT}/libavformat.a
                       ${PATH_TO_PRE_BUILT}/libavcodec.a
                       ${PATH_TO_PRE_BUILT}/libpostproc.a
                       ${PATH_TO_PRE_BUILT}/libswresample.a
                       ${PATH_TO_PRE_BUILT}/libswscale.a
                       ${PATH_TO_PRE_BUILT}/libavutil.a
                       ${PATH_TO_PRE_BUILT}/libfdk-aac.a
                       ${PATH_TO_PRE_BUILT}/libvo-aacenc.a
                       ${PATH_TO_PRE_BUILT}/libx264.a
                       )