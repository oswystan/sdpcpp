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
#include <algorithm>

#define LOG_TAG "sdp"
#include "log.h"
#include "sdp.h"

namespace sdp {

//==================================================
//     type 2 str utils
//==================================================
#define BUILD_TYPE(s,t) {type:t, str:s}
struct Type2Str {
    int type;
    const char* str;
};
Type2Str gnodes[] = {
    BUILD_TYPE("v", SDP_NODE_VERSION),
    BUILD_TYPE("o", SDP_NODE_ORIGIN),
    BUILD_TYPE("s", SDP_NODE_SESSION_NAME),
    BUILD_TYPE("i", SDP_NODE_SESSION_INFORMATION),
    BUILD_TYPE("u", SDP_NODE_URI),
    BUILD_TYPE("e", SDP_NODE_EMAIL),
    BUILD_TYPE("p", SDP_NODE_PHONE),
    BUILD_TYPE("c", SDP_NODE_CONNECTION),
    BUILD_TYPE("b", SDP_NODE_BANDWIDTH),
    BUILD_TYPE("z", SDP_NODE_TZ),
    BUILD_TYPE("k", SDP_NODE_ENCRYPTION),
    BUILD_TYPE("t", SDP_NODE_TIME),
    BUILD_TYPE("r", SDP_NODE_REPEAT),
    BUILD_TYPE("m", SDP_NODE_MEDIA),
    BUILD_TYPE("a", SDP_NODE_ATTRIBUTE)
};
Type2Str gattrs[] = {
    BUILD_TYPE("a=rtcp:", SDP_ATTR_RTCP),
};

//==================================================
//     sdp reader and writer
//==================================================
int SdpReader::parse(std::string& s, SdpRoot& sdp) {
    return 0;
}

int SdpWriter::writeTo(std::string& s, SdpRoot& sdp) {
    return 0;
}

//==================================================
//     sdp node
//==================================================
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
int SdpNode::find(EAttrType t,  SdpAttr*& attr) {
    attr = NULL;
    for (unsigned int i = 0; i < children.size(); i++) {
        if (children[i]->nodeType == SDP_NODE_ATTRIBUTE &&
            ((SdpAttr*)children[i])->attrType == t) {
            attr = (SdpAttr*)children[i];
            return 0;
        }
    }
    return -1;
}
int SdpNode::find(EMediaType t, SdpMedia*& media) {
    media = NULL;
    for (unsigned int i = 0; i < children.size(); i++) {
        if (children[i]->nodeType == SDP_NODE_MEDIA &&
            ((SdpMedia*)children[i])->mediaType == t) {
            media = (SdpMedia*)children[i];
            return 0;
        }
    }

    return -1;
}
int SdpNode::find(ENodeType t,  std::vector<SdpNode*> v) {
    v.clear();
    auto matcher = [t,&v](SdpNode* n) {
        if (n->nodeType == t) {
            v.push_back(n);
        }
    };
    std::for_each(children.begin(), children.end(), matcher);
    return 0;
}
int SdpNode::find(EAttrType t,  std::vector<SdpAttr*> v) {
    v.clear();
    auto matcher = [t,&v](SdpNode* n) {
        if (n->nodeType == SDP_NODE_ATTRIBUTE) {
            SdpAttr* attr = (SdpAttr*)n;
            if (attr->attrType == t) {
                v.push_back(attr);
            }
        }
    };
    std::for_each(children.begin(), children.end(), matcher);
    return 0;
}
int SdpNode::find(EMediaType t, std::vector<SdpMedia*> v) {
    v.clear();
    auto matcher = [t,&v](SdpNode* n) {
        if (n->nodeType == SDP_NODE_MEDIA) {
            SdpMedia* media = (SdpMedia*)n;
            if (media->mediaType == t) {
                v.push_back(media);
            }
        }
    };
    std::for_each(children.begin(), children.end(), matcher);
    return 0;
}

//==================================================
//     sdp factory
//==================================================
ENodeType SdpFactory::getNodeType(std::string& l) {
    return SDP_NODE_NONE;
}
EAttrType SdpFactory::getAttrType(std::string& l) {
    return SDP_ATTR_NONE;
}
SdpNode* SdpFactory::createNode(ENodeType type) {
    return NULL;
}
SdpAttr* SdpFactory::createAttr(EAttrType type) {
    return NULL;
}

};

/********************************** END **********************************************/

