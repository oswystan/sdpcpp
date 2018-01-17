/*
 **************************************************************************************
 *       Filename:  ut.cpp
 *    Description:  source file
 *
 *        Version:  1.0
 *        Created:  2017-11-02 17:49:25
 *
 *       Revision:  initial draft;
 **************************************************************************************
 */
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>

#include "sdp.h"

#define MAX_SIZE (256*1024)
char* readFile(const char* f) {
    char* content = (char*)malloc(MAX_SIZE);
    if (!content) {
        return NULL;
    }

    memset(content, 0x00, MAX_SIZE);
    FILE* fp = fopen(f, "r");
    if (!fp) {
        printf("ERROR: fail to open %s\n", f);
        free(content);
        return NULL;
    }
    fread(content, 1, MAX_SIZE-1, fp);
    fclose(fp);
    return content;
}

const char* writerFileName = "data/_writer_.sdp";
void writeFile(const char* ptr, int size) {
    FILE* fp = fopen(writerFileName, "w");
    if (!fp) {
        printf("ERROR: fail to open writer file %s\n", writerFileName);
        return;
    }
    fwrite(ptr, 1, size, fp);
    fclose(fp);
}
int listFiles(std::vector<std::string> &fl) {
    DIR *dirp;
    struct dirent *direntp;
    if ((dirp = opendir("data")) == NULL) {
        printf("ERROR: fail to open data dir\n");
        return -1;
    }
    std::string f;
    while((direntp = readdir(dirp)) != NULL) {
        if (strcmp(direntp->d_name, ".") == 0 || strcmp(direntp->d_name, "..") == 0 ) {
            continue;
        }
        f = "data/";
        f += direntp->d_name;
        fl.push_back(f);
    }
    return 0;
}

int parseFiles(int argc, const char* argv[]) {
    std::vector<std::string> fl;
    unlink(writerFileName);
    if (argc == 2) {
        fl.push_back(argv[1]);
    } else {
        if (0 != listFiles(fl)) {
            return -1;
        }
    }

    for (unsigned int i = 0; i < fl.size(); i++) {
        sdp::SdpRoot session;
        sdp::SdpReader reader;
        sdp::SdpWriter writer;

        char* ptr = readFile(fl[i].c_str());
        if (!ptr) {
            return -1;
        }

        std::string str = ptr;
        int ret = reader.parse(str, session);
        if (ret != 0) {
            printf("[FAIL] %s\n", fl[i].c_str());
            return -1;
        }
        std::string rst;
        writer.write(rst, session);
        if (str != rst) {
            printf("[FAIL] %s rst != src", fl[i].c_str());
            int sz1 = str.size();
            int sz2 = rst.size();
            writeFile(rst.c_str(), sz2);
            if (sz1 != sz2) {
                printf(" len: %d %d\n", sz1, sz2);
            }
            printf("*** *** *** *** *** ** *** *** ***\n");
            printf("RUN THE FOLLOWING COMMAND TO ANALYSE:\n");
            printf("    diff %s %s\n", writerFileName, fl[i].c_str());
            printf("*** *** *** *** *** ** *** *** ***\n");
            printf("\n");
            return -1;
        }
        printf("[SUCC] %s\n", fl[i].c_str());
        free(ptr);
    }
    return 0;
}

int testMedia(int argc, const char* argv[]) {
    std::string src = \
		"v=0\r\n"
		"o=- 2178921783902026370 2 IN IP4 127.0.0.1\r\n"
		"m=audio 9 UDP/TLS/RTP/SAVPF 111 103\r\n"
		"a=rtpmap:111 opus/48000/2\r\n"
		"a=rtpmap:103 ISAC/16000\r\n"
		"a=rtcp-fb:111 transport-cc\r\n"
        "b=AS:50\r\n"
		"a=fmtp:111 minptime=10;useinbandfec=1\r\n"
        "a=candidate:1 1 udp 2130706431 10.1.13.16 54781 typ host generation 0\r\n"
		"a=ssrc:2252595259 cname:0/4IiULXLQNUx66b\r\n"
		"a=ssrc:2252595259 msid:lmeZp9JTa2DAZOiTnGGwMaJlapGUxznQGIzA fca1a679-2996-4078-afe9-3c7d850b6568\r\n"
		"a=ssrc:2252595259 mslabel:lmeZp9JTa2DAZOiTnGGwMaJlapGUxznQGIzA\r\n"
		"a=ssrc:2252595259 label:fca1a679-2996-4078-afe9-3c7d850b6568\r\n"
		"m=video 9 UDP/TLS/RTP/SAVPF 100\r\n"
		"a=rtpmap:100 H264/90000\r\n"
		"a=rtcp-fb:100 ccm fir\r\n"
		"a=rtcp-fb:100 nack\r\n"
		"a=rtcp-fb:100 nack pli\r\n"
		"a=rtcp-fb:100 goog-remb\r\n"
		"a=rtcp-fb:100 transport-cc\r\n"
		"a=cliprect:0,0,1280,720\r\n"
		"a=fmtp:100 level-asymmetry-allowed=1;packetization-mode=1;profile-level-id=42e01f\r\n"
        "a=ssrc-group:FID 475778974 3581258384\r\n"
        "a=ssrc:475778974 cname:0/4IiULXLQNUx66b\r\n"
        "a=ssrc:475778974 msid:lmeZp9JTa2DAZOiTnGGwMaJlapGUxznQGIzA 611901ad-da6e-42d8-884f-df151bb07b04\r\n"
        "a=ssrc:475778974 mslabel:lmeZp9JTa2DAZOiTnGGwMaJlapGUxznQGIzA\r\n"
        "a=ssrc:475778974 label:611901ad-da6e-42d8-884f-df151bb07b04\r\n"
        "a=ssrc:3581258384 cname:0/4IiULXLQNUx66b\r\n"
        "a=ssrc:3581258384 msid:lmeZp9JTa2DAZOiTnGGwMaJlapGUxznQGIzA 611901ad-da6e-42d8-884f-df151bb07b04\r\n"
        "a=ssrc:3581258384 mslabel:lmeZp9JTa2DAZOiTnGGwMaJlapGUxznQGIzA\r\n"
        "a=ssrc:3581258384 label:611901ad-da6e-42d8-884f-df151bb07b04\r\n";
    std::string dst = \
		"v=0\r\n"
		"o=- 2178921783902026370 2 IN IP4 127.0.0.1\r\n"
		"m=audio 9 UDP/TLS/RTP/SAVPF 111\r\n"
		"a=rtpmap:111 opus/48000/2\r\n"
		"a=rtcp-fb:111 transport-cc\r\n"
        "b=AS:50\r\n"
		"a=fmtp:111 minptime=10;useinbandfec=1\r\n"
		"a=ssrc:2252595259 cname:0/4IiULXLQNUx66b\r\n"
		"a=ssrc:2252595259 msid:lmeZp9JTa2DAZOiTnGGwMaJlapGUxznQGIzA fca1a679-2996-4078-afe9-3c7d850b6568\r\n"
		"a=ssrc:2252595259 mslabel:lmeZp9JTa2DAZOiTnGGwMaJlapGUxznQGIzA\r\n"
		"a=ssrc:2252595259 label:fca1a679-2996-4078-afe9-3c7d850b6568\r\n"
        "a=inactive\r\n"
		"m=video 0 UDP/TLS/RTP/SAVPF 100\r\n"
		"a=rtpmap:100 H264/90000\r\n"
		"a=rtcp-fb:100 ccm fir\r\n"
		"a=rtcp-fb:100 nack\r\n"
		"a=rtcp-fb:100 nack pli\r\n"
		"a=rtcp-fb:100 goog-remb\r\n"
		"a=rtcp-fb:100 transport-cc\r\n"
		"a=cliprect:0,0,1280,720\r\n"
		"a=fmtp:100 level-asymmetry-allowed=1;packetization-mode=1;profile-level-id=42e01f\r\n"
        "a=ssrc-group:FID 475778974 3581258384\r\n"
        "a=ssrc:475778974 cname:0/4IiULXLQNUx66b\r\n"
        "a=ssrc:475778974 msid:lmeZp9JTa2DAZOiTnGGwMaJlapGUxznQGIzA 611901ad-da6e-42d8-884f-df151bb07b04\r\n"
        "a=ssrc:475778974 mslabel:lmeZp9JTa2DAZOiTnGGwMaJlapGUxznQGIzA\r\n"
        "a=ssrc:475778974 label:611901ad-da6e-42d8-884f-df151bb07b04\r\n"
        "a=ssrc:3581258384 cname:0/4IiULXLQNUx66b\r\n"
        "a=ssrc:3581258384 msid:lmeZp9JTa2DAZOiTnGGwMaJlapGUxznQGIzA 611901ad-da6e-42d8-884f-df151bb07b04\r\n"
        "a=ssrc:3581258384 mslabel:lmeZp9JTa2DAZOiTnGGwMaJlapGUxznQGIzA\r\n"
        "a=ssrc:3581258384 label:611901ad-da6e-42d8-884f-df151bb07b04\r\n"
        "a=ice-ufrag:qhEz\r\n"
        "a=ice-pwd:0pXkZeRzvMuO7ID0ZVCU+K\r\n"
        "a=fingerprint:sha-256 A1:BF:26:D9:0C:41:A5:AB:4A:BE:34:73:AE:13:8E:00:76:57:C8:94:D2:AB:E5:33:10:0B:3B:9C:2C:67:30:FF\r\n";

    sdp::SdpRoot root;
    sdp::SdpReader reader;
    sdp::SdpWriter writer;
    reader.parse(src, root);
    sdp::SdpMedia* vid;
    sdp::SdpMedia* aud;
    root.find(sdp::SDP_MEDIA_VIDEO, vid);
    root.find(sdp::SDP_MEDIA_AUDIO, aud);
    aud->filter(sdp::SDP_ATTR_CANDIDATE);
    aud->inactive();
    std::string codec = "ISAC";
    std::string ufrag = "qhEz";
    std::string pwd = "0pXkZeRzvMuO7ID0ZVCU+K";
    std::string fp = "sha-256 A1:BF:26:D9:0C:41:A5:AB:4A:BE:34:73:AE:13:8E:00:76:57:C8:94:D2:AB:E5:33:10:0B:3B:9C:2C:67:30:FF";
    vid->updateIce(ufrag, pwd, fp);
    uint32_t ssrc = aud->ssrc();
    if (ssrc != 2252595259) {
        printf("ERROR: invalid ssrc: [%u!=2252595259]\n", ssrc);
		return -1;
    }
    uint32_t bandwidth = aud->bandwidth();
    if(bandwidth != 50) {
        printf("ERROR: incorrect audio bandwidth: %d!=50", bandwidth);
        return -1;
    }
    std::vector<uint32_t> ssrcs = vid->ssrcGrp();
    if (ssrcs.size() != 2 || ssrcs[0] != 475778974 || ssrcs[1] != 3581258384) {
        printf("ERROR ssrcgrp not correct\n");
        return -1;
    }
    vid->reject();
    int pt = aud->getPT(codec);
    aud->filter(pt);
    std::string out;
    writer.write(out, root);
    if (out != dst) {
        printf("*** *** *** *** *** *** *** *** *** ***\n");
        printf("%s\n", dst.c_str());
        printf("%s\n", out.c_str());
        printf("*** *** *** *** *** *** *** *** *** ***\n");
        return -1;
    }

    return 0;
}

#define BUILD_UTC(f) { .name=#f, .func=f }
struct UTC {
    const char* name;
    int (*func)(int argc, const char* argv[]);
};
UTC utcs[] = {
    BUILD_UTC(testMedia),
    BUILD_UTC(parseFiles)
};

int main(int argc, const char *argv[]) {
    for (unsigned int i = 0; i < sizeof(utcs)/sizeof(utcs[0]); i++) {
        if (0 != (*utcs[i].func)(argc, argv)) {
            printf("[FAIL] %s\n", utcs[i].name);
            return -1;
        } else {
            printf("[SUCC] %s\n", utcs[i].name);
        }
    }
    return 0;
}

/********************************** END **********************************************/

