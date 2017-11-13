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
            printf("[FAILED] %s\n", fl[i].c_str());
            return -1;
        }
        std::string rst;
        writer.write(rst, session);
        if (str != rst) {
            printf("[FAILED] %s rst != src", fl[i].c_str());
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
        printf("[SUCCESS] %s\n", fl[i].c_str());
        free(ptr);
    }
    return 0;
}

int testMedia(int argc, const char* argv[]) {
    return 0;
}

typedef int (*tc)(int argc, const char* argv[]);
tc utcs[] = {
    parseFiles,
    testMedia
};

int main(int argc, const char *argv[]) {
    for (unsigned int i = 0; i < sizeof(utcs)/sizeof(utcs[0]); i++) {
        if (0 != (*utcs[i])(argc, argv)) {
            return -1;
        }
    }
    return 0;
}

/********************************** END **********************************************/

