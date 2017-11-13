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
    std::string in = "v=0\r\n";
    std::string out;
    int ret = reader.parse(in, root);
    if (ret != 0) {
        loge("fail to parse.");
        return ret;
    }
    ret = writer.write(out, root);
    if (ret != 0) {
        loge("fail to write.");
        return ret;
    }
    if (in != out) {
        loge("in != out");
        return -1;
    }
    logi("success");
    return 0;
}

/********************************** END **********************************************/

