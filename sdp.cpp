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
#include <string.h>
#include <algorithm>
#include <string>
#include <sstream>

#define LOG_TAG "sdp"
#include "log.h"
#include "sdp.h"

namespace sdp {

//==================================================
//     type 2 str utils
//==================================================
#define _UNUSED_ __attribute__((unused))
#define BUILD_TYPE(s,t,n) {.type = t,  .str = s, .node = new n}
#define ARR_LEN(x) (sizeof(x)/sizeof(x[0]))
struct Type2Str {
    int type;
    const char* str;
    SdpNode* node;
};
Type2Str gnodes[] = {
    BUILD_TYPE("v=", SDP_NODE_VERSION, SdpVersion),
    BUILD_TYPE("o=", SDP_NODE_ORIGIN, SdpOrigin),
    BUILD_TYPE("s=", SDP_NODE_SESSION_NAME, SdpSessName),
    BUILD_TYPE("i=", SDP_NODE_SESSION_INFORMATION, SdpSessInfo),
    BUILD_TYPE("u=", SDP_NODE_URI, SdpUri),
    BUILD_TYPE("e=", SDP_NODE_EMAIL, SdpEmail),
    BUILD_TYPE("p=", SDP_NODE_PHONE, SdpPhone),
    BUILD_TYPE("c=", SDP_NODE_CONNECTION, SdpConn),
    BUILD_TYPE("b=", SDP_NODE_BANDWIDTH, SdpNone),
    BUILD_TYPE("z=", SDP_NODE_TZ, SdpNone),
    BUILD_TYPE("k=", SDP_NODE_ENCRYPTION, SdpNone),
    BUILD_TYPE("t=", SDP_NODE_TIME, SdpTime),
    BUILD_TYPE("r=", SDP_NODE_REPEAT, SdpNone),
    BUILD_TYPE("m=", SDP_NODE_MEDIA, SdpMedia),
    BUILD_TYPE("a=", SDP_NODE_ATTRIBUTE, SdpAttr(SDP_ATTR_NONE))
};
Type2Str gattrs[] = {
    BUILD_TYPE("a=cat:",           SDP_ATTR_CAT, SdpAttr(SDP_ATTR_CAT)),
    BUILD_TYPE("a=keywds:",        SDP_ATTR_KEYWDS, SdpAttr(SDP_ATTR_KEYWDS)),
    BUILD_TYPE("a=tool:",          SDP_ATTR_TOOL, SdpAttr(SDP_ATTR_TOOL)),
    BUILD_TYPE("a=ptime:",         SDP_ATTR_PTIME, SdpAttr(SDP_ATTR_PTIME)),
    BUILD_TYPE("a=maxptime:",      SDP_ATTR_MAXPTIME, SdpAttr(SDP_ATTR_MAXPTIME)),
    BUILD_TYPE("a=rptmap:",        SDP_ATTR_RTPMAP, SdpAttr(SDP_ATTR_RTPMAP)),
    BUILD_TYPE("a=recvonly",       SDP_ATTR_RECVONLY, SdpAttr(SDP_ATTR_RECVONLY)),
    BUILD_TYPE("a=sendrecv",       SDP_ATTR_SENDRECV, SdpAttr(SDP_ATTR_SENDRECV)),
    BUILD_TYPE("a=sendonly",       SDP_ATTR_SENDONLY, SdpAttr(SDP_ATTR_SENDONLY)),
    BUILD_TYPE("a=inactive",       SDP_ATTR_INACTIVE, SdpAttr(SDP_ATTR_INACTIVE)),
    BUILD_TYPE("a=orient:",        SDP_ATTR_ORIENT, SdpAttr(SDP_ATTR_ORIENT)),
    BUILD_TYPE("a=orient:",        SDP_ATTR_TYPE, SdpAttr(SDP_ATTR_TYPE)),
    BUILD_TYPE("a=charset:",       SDP_ATTR_CHARSET, SdpAttr(SDP_ATTR_CHARSET)),
    BUILD_TYPE("a=sdplang:",       SDP_ATTR_SDPLANG, SdpAttr(SDP_ATTR_SDPLANG)),
    BUILD_TYPE("a=lang:",          SDP_ATTR_LANG, SdpAttr(SDP_ATTR_LANG)),
    BUILD_TYPE("a=framerate:",     SDP_ATTR_FRAMERATE, SdpAttr(SDP_ATTR_FRAMERATE)),
    BUILD_TYPE("a=quality:",       SDP_ATTR_QUALITY, SdpAttr(SDP_ATTR_QUALITY)),
    BUILD_TYPE("a=fmtp:",          SDP_ATTR_FMTP, SdpAttr(SDP_ATTR_FMTP)),
    BUILD_TYPE("a=rtcp:",          SDP_ATTR_RTCP, SdpAttrRTCP),
    BUILD_TYPE("a=candidate:",     SDP_ATTR_CANDIDATE, SdpAttrCandi),
    BUILD_TYPE("a=ice-ufrag:",     SDP_ATTR_ICE_UFRAG, SdpAttr(SDP_ATTR_ICE_UFRAG)),
    BUILD_TYPE("a=ice-pwd:",       SDP_ATTR_ICE_PWD, SdpAttr(SDP_ATTR_ICE_PWD)),
    BUILD_TYPE("a=fingerprint:",   SDP_ATTR_FINGERPRINT, SdpAttr(SDP_ATTR_FINGERPRINT)),
    BUILD_TYPE("a=msid-semantic:", SDP_ATTR_MSID_SEMANTIC, SdpAttr(SDP_ATTR_MSID_SEMANTIC)),
};
static int str2type(const char* s, Type2Str* ts, unsigned int size) {
    for (unsigned int i = 0; i < size; i++) {
        if (strncmp(ts[i].str, s, strlen(ts[i].str)) == 0) {
            return ts[i].type;
        }
    }
    return 0;
}
static _UNUSED_ const char* type2str(int type, Type2Str* ts, unsigned int size) {
    for (unsigned int i = 0; i < size; i++) {
        if (ts[i].type == type) {
            return ts[i].str;
        }
    }
    return 0;
}
static SdpNode* type2node(int type, Type2Str* ts, unsigned int size) {
    for (unsigned int i = 0; i < size; i++) {
        if (ts[i].type == type) {
            return ts[i].node;
        }
    }
    return NULL;
}
static SdpAttr* type2attr(int type, Type2Str* ts, unsigned int size) {
    for (unsigned int i = 0; i < size; i++) {
        if (ts[i].type == type) {
            return (SdpAttr*)ts[i].node;
        }
    }
    return NULL;
}

//==================================================
//     string utils
//==================================================
void ltrim(std::string& s) {
    s.erase(0, s.find_first_not_of(' '));
}
void rtrim(std::string& s) {
    s.erase(s.find_last_not_of('\r') + 1);
    s.erase(s.find_last_not_of(' ') + 1);
}
void trim(std::string& s) {
    ltrim(s);
    rtrim(s);
}
void split(std::string& s, std::vector<std::string>& lines) {
    size_t last = 0;
    size_t idx = s.find_first_of("\n");
    std::string str;
    while(idx != std::string::npos) {
        str = s.substr(last, idx-last);
        trim(str);
        lines.push_back(str);
        last = idx + 1;
        idx = s.find_first_of("\n", last);
    }
}

//==================================================
//     line reader
//==================================================
class ParseException : public std::exception {
public:
    ParseException(const char* str) : s(str) {}
    const char* what() const throw() { return s.c_str(); }
private:
    std::string s;
};
class LineReader {
public:
    LineReader(std::string& s): val(s), pos(0) {}
    void skip(char delim = ' ');
    void forward(unsigned int to);
    std::string readStr(char delim = ' ');
    int readInt(char delim = ' ');
    uint64_t readUint64(char delim = ' ');
private:
    std::string readToken(char delim = ' ');

public:
    std::string& val;
    unsigned int pos;
};
void LineReader::skip(char delim) {
    unsigned int i = val.find_first_of(delim, pos);
    if (i != std::string::npos) {
        pos = i+1;
    } else {
        pos = val.size();
    }
}
void LineReader::forward(unsigned int to) {
    if (to >= val.size()) {
        pos = val.size();
    } else {
        pos = to;
    }
}
std::string LineReader::readToken(char delim) {
    unsigned int old = pos;
    unsigned int i = val.find_first_of(delim, pos);
    if (i != std::string::npos) {
        pos = i+1;
        return val.substr(old, i);
    } else {
        pos = val.size();
        return val.substr(old, i);
    }
}
std::string LineReader::readStr(char delim) {
    std::string str = readToken(delim);
    if (str.size() == 0) {
        throw ParseException("no token found");
    }
    return str;
}
int LineReader::readInt(char delim) {
    std::string str = readToken(delim);
    if (str.size() == 0) {
        throw ParseException("no token found");
    }
    auto it = str.begin();
    while(it != str.end()) {
        if (!std::isdigit(*it)) {
            throw ParseException("Not a digital");
            break;
        }
        it++;
    }
    return atoi(str.c_str());
}
uint64_t LineReader::readUint64(char delim) {
    std::string str = readToken(delim);
    if (str.size() == 0) {
        throw ParseException("no token found");
    }
    auto it = str.begin();
    while(it != str.end()) {
        if (!std::isdigit(*it)) {
            throw ParseException("Not a digital");
            break;
        }
    }
    return strtoull(str.c_str(), NULL, 10);
}

//==================================================
//     sdp reader and writer
//==================================================
int SdpReader::parse(std::string& s, SdpRoot& root) {
    std::vector<std::string> lines;
    ENodeType nType;
    EAttrType aType;
    SdpNode* node = NULL;
    SdpNode* parent = &root;
    int ret = 0;

    split(s, lines);
    for (unsigned int i = 0; i < lines.size(); i++) {
        //skip empty line
        if (lines[i].size() == 0) {
            continue;
        }

        //create node
        nType = SdpFactory::getNodeType(lines[i]);
        if (nType == SDP_NODE_NONE) {
            loge("invalid sdp: [%s] @ %d", lines[i].c_str(), i+1);
            return -1;
        }
        if (nType != SDP_NODE_ATTRIBUTE) {
            node = SdpFactory::createNode(nType);
            ret = node->parse(lines[i]);
            if (ret != 0) {
                loge("fail to parse: [%s] @ %d", lines[i].c_str(), i+1);
                delete node;
                return -1;
            }
            if (nType == SDP_NODE_MEDIA) {
                root.children.push_back(node);
                parent = node;
            } else {
                parent->children.push_back(node);
            }
            continue;
        }

        //create attributes if node type is SDP_ATTRIBUTE
        aType = SdpFactory::getAttrType(lines[i]);
        if (aType == SDP_ATTR_NONE) {
            loge("invalid sdp: [%s] @ %d", lines[i].c_str(), i+1);
            return -1;
        }
        node = SdpFactory::createAttr(aType);
        ret = node->parse(lines[i]);
        if (ret != 0) {
            loge("fail to parse: [%s] @ %d", lines[i].c_str(), i+1);
            delete node;
            return -1;
        }
        parent->children.push_back(node);
    }

    return 0;
}

int SdpWriter::write(std::string& s, SdpRoot& sdp) {
    return sdp.write(s);
}

//==================================================
//     sdp factory
//==================================================
ENodeType SdpFactory::getNodeType(std::string& l) {
    return (ENodeType)str2type(l.c_str(), gnodes, ARR_LEN(gnodes));
}
EAttrType SdpFactory::getAttrType(std::string& l) {
    return (EAttrType)str2type(l.c_str(), gattrs, ARR_LEN(gattrs));
}
SdpNode* SdpFactory::createNode(ENodeType type) {
    SdpNode* n = type2node(type, gnodes, ARR_LEN(gnodes));
    if (n) return n->clone();
    return NULL;
}
SdpAttr* SdpFactory::createAttr(EAttrType type) {
    SdpAttr* attr = type2attr(type, gattrs, ARR_LEN(gattrs));
    if (attr) {
        return (SdpAttr*)(attr->clone());
    }
    return NULL;
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
int SdpNode::parse(std::string& l) {
    return -1;
}
int SdpNode::write(std::string& l) {
    for (unsigned int i = 0; i < children.size(); i++) {
        children[i]->write(l);
    }
    return 0;
}

//==================================================
//     sdp sub classes
//==================================================
int SdpVersion::parse(std::string& l) {
    LineReader lr(l);
    lr.skip('=');
    try {
        version = lr.readInt();
    } catch (std::exception& e) {
        loge("%s", e.what());
        return -1;
    }
    return 0;
}
int SdpOrigin::parse(std::string& l) {
    return -1;
}
int SdpSessName::parse(std::string& l) {
    return -1;
}
int SdpSessInfo::parse(std::string& l) {
    return -1;
}
int SdpUri::parse(std::string& l) {
    return -1;
}
int SdpEmail::parse(std::string& l) {
    return -1;
}
int SdpPhone::parse(std::string& l) {
    return -1;
}
int SdpTime::parse(std::string& l) {
    return -1;
}
int SdpConn::parse(std::string& l) {
    return -1;
}
int SdpMedia::parse(std::string& l) {
    return -1;
}
int SdpAttr::parse(std::string& l) {
    return -1;
}
int SdpAttrRTCP::parse(std::string& l) {
    return -1;
}
int SdpAttrCandi::parse(std::string& l) {
    return -1;
}

int SdpVersion::write(std::string& l) {
    std::stringstream ss;
    ss << type2str(nodeType, gnodes, ARR_LEN(gnodes))
        << version << "\r\n";
    l += ss.str();
    return 0;
}
int SdpOrigin::write(std::string& l) {
    return -1;
}
int SdpSessName::write(std::string& l) {
    return -1;
}
int SdpSessInfo::write(std::string& l) {
    return -1;
}
int SdpUri::write(std::string& l) {
    return -1;
}
int SdpEmail::write(std::string& l) {
    return -1;
}
int SdpPhone::write(std::string& l) {
    return -1;
}
int SdpTime::write(std::string& l) {
    return -1;
}
int SdpConn::write(std::string& l) {
    return -1;
}
int SdpMedia::write(std::string& l) {
    return -1;
}
int SdpAttr::write(std::string& l) {
    return -1;
}
int SdpAttrRTCP::write(std::string& l) {
    return -1;
}
int SdpAttrCandi::write(std::string& l) {
    return -1;
}

};

/********************************** END **********************************************/

