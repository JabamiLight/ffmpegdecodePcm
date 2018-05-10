
cmake_minimum_required(VERSION 3.4.1)


include_directories(${PATH_TO_MEDIACORE}/libs/ffmpeg/include)# 包含.h文件include目录

add_subdirectory(./libffmpeg_decoder)


add_library( native-lib
             SHARED
             Mp3Decoder.cpp)


target_link_libraries( native-lib
                        log
                        z
                    libffmpeg_decoder
                       )