/*
 **************************************************************************************
 *       Filename:  main.cpp
 *    Description:   source file
 *
 *        Version:  1.0
 *        Created:  2017-11-09 19:29:09
 *
 *       Revision:  initial draft;
 **************************************************************************************
 */

#define LOG_TAG "main"
#include "log.h"
#include "sdp.h"

int main(int argc, const char *argv[]) {
    sdp::SdpRoot root;
    sdp::SdpReader reader;
    sdp::SdpWriter writer;
    std::string in = "v=0\r\n" \
                    "o=- 2178921783902026370 2 IN IP4 127.0.0.1\r\n" \
                    "s=-\r\n" \
                    "i=session-info\r\n" \
                    "u=www.bat.com/coo\r\n" \
                    "e=aaa@aaa.com\r\n" \
                    "p=+8613322221123\r\n" \
                    "t=0 0\r\n" \
                    "c=IN IP4 10.1.1.10\r\n" \
                    "m=audio 9 UDP/TLS/RTP/SAVPF 111 103 104 9 0 8 106 105 13 110 112 113 126\r\n" \
                    ;
    std::string out;
    int ret = reader.parse(in, root);
    if (ret != 0) {
        loge("fail to parse.");
        return ret;
    }
    logi("parse ok");
    ret = writer.write(out, root);
    if (ret != 0) {
        loge("fail to write.");
        return ret;
    }
    logi("write ok");
    if (in != out) {
        loge("in != out");
        loge("in:%s", in.c_str());
        loge("out:%s", out.c_str());
        return -1;
    }
    logi("success");
    return 0;
}

/********************************** END **********************************************/

