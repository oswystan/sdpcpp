/*
 **************************************************************************************
 *       Filename:  sdp.cpp
 *    Description:   source file
 *
 *        Version:  1.0
 *        Created:  2017-11-09 17:52:50
 *
 *       Revision:  initial draft;
 **************************************************************************************
 */
#define LOG_TAG "sdp"
#include "log.h"
#include "sdp.h"

namespace sdp {

int SdpReader::parse(std::string& s, SdpRoot& sdp) {
    return 0;
}

int SdpWriter::writeTo(std::string& s, SdpRoot& sdp) {
    return 0;
}

SdpNode::~SdpNode() {
    for (unsigned int i = 0; i < children.size(); i++) {
        delete children[i];
    }
    children.clear();
}
void SdpNode::dump() {
}

int SdpNode::add(SdpNode* n) {
    if (n) children.push_back(n);
    return 0;
}
int SdpNode::remove(SdpNode* n) {
    auto it = children.begin();
    while(it != children.end()) {
        if (*it == n) {
            children.erase(it);
            break;
        }
    }
    return 0;
}

int SdpNode::find(ENodeType t,  SdpNode*& n) {
    n = NULL;
    for (unsigned int i = 0; i < children.size(); i++) {
        if (children[i]->nodeType == t) {
            n = children[i];
            return 0;
        }
    }
    return -1;
}
int SdpNode::find(EAttrType t,  SdpAttr*& n) {
    n = NULL;
    SdpAttr* attr = NULL;
    find(SDP_NODE_ATTRIBUTE, (SdpNode*&)attr);
    if (attr && attr->attrType == t) {
        n = attr;
        return 0;
    }
    return -1;
}
int SdpNode::find(EMediaType t, SdpMedia*& n) {
    n = NULL;
    SdpMedia* media = NULL;
    find(SDP_NODE_MEDIA, (SdpNode*&)media);
    if (media && media->mediaType == t) {
        n = media;
        return 0;
    }
    return -1;
}
int SdpNode::find(ENodeType t,  std::vector<SdpNode*> v) {
    return 0;
}
int SdpNode::find(EAttrType t,  std::vector<SdpNode*> v) {
    return 0;
}
int SdpNode::find(EMediaType t, std::vector<SdpMedia*> v) {
    return 0;
}

};

/********************************** END **********************************************/

