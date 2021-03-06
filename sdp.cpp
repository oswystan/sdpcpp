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
#include <openssl/rand.h>
#include <string.h>
#include <algorithm>
#include <string>
#include <sstream>
#include <memory>

#define LOG_TAG "sdp"
#include "log.h"
#include "sdp.h"

namespace sdp {

//==================================================
//     type 2 str utils
//==================================================
#define _UNUSED_ __attribute__((unused))
#define BUILD_TYPE(s,t,n) {.type = t,  .str = s, .node = new n}
#define BUILD_TYPESTR(s,t) {.type = t,  .str = s, .node = NULL}
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
    BUILD_TYPE("b=", SDP_NODE_BANDWIDTH, SdpBandWidth),
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
    BUILD_TYPE("a=rtpmap:",        SDP_ATTR_RTPMAP, SdpAttrRtpMap),
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
    BUILD_TYPE("a=fmtp:",          SDP_ATTR_FMTP, SdpAttrFmtp),
    BUILD_TYPE("a=rtcp:",          SDP_ATTR_RTCP, SdpAttrRTCP),
    BUILD_TYPE("a=candidate:",     SDP_ATTR_CANDIDATE, SdpAttrCandi),
    BUILD_TYPE("a=ice-ufrag:",     SDP_ATTR_ICE_UFRAG, SdpAttr(SDP_ATTR_ICE_UFRAG)),
    BUILD_TYPE("a=ice-pwd:",       SDP_ATTR_ICE_PWD, SdpAttr(SDP_ATTR_ICE_PWD)),
    BUILD_TYPE("a=ice-options:",   SDP_ATTR_ICE_OPTIONS, SdpAttr(SDP_ATTR_ICE_OPTIONS)),
    BUILD_TYPE("a=fingerprint:",   SDP_ATTR_FINGERPRINT, SdpAttr(SDP_ATTR_FINGERPRINT)),
    BUILD_TYPE("a=setup:",         SDP_ATTR_SETUP, SdpAttr(SDP_ATTR_SETUP)),
    BUILD_TYPE("a=group:",         SDP_ATTR_GROUP, SdpAttr(SDP_ATTR_GROUP)),
    BUILD_TYPE("a=mid:",           SDP_ATTR_MID, SdpAttr(SDP_ATTR_MID)),
    BUILD_TYPE("a=extmap:",        SDP_ATTR_EXTMAP, SdpAttr(SDP_ATTR_EXTMAP)),
    BUILD_TYPE("a=rtcp-mux",       SDP_ATTR_RTCPMUX, SdpAttr(SDP_ATTR_RTCPMUX)),
    BUILD_TYPE("a=rtcp-rsize",     SDP_ATTR_RTCPRSIZE, SdpAttr(SDP_ATTR_RTCPRSIZE)),
    BUILD_TYPE("a=rtcp-fb:",       SDP_ATTR_RTCPFB, SdpAttrRTCPFB),
    BUILD_TYPE("a=crypto:",        SDP_ATTR_CRYPTO, SdpAttr(SDP_ATTR_CRYPTO)),
    BUILD_TYPE("a=ssrc:",          SDP_ATTR_SSRC, SdpAttrSsrc),
    BUILD_TYPE("a=ssrc-group:",    SDP_ATTR_SSRC_GROUP, SdpAttrSsrcGrp),
    BUILD_TYPE("a=msid:",          SDP_ATTR_MSID, SdpAttr(SDP_ATTR_MSID)),
    BUILD_TYPE("a=msid-semantic:", SDP_ATTR_MSID_SEMANTIC, SdpAttr(SDP_ATTR_MSID_SEMANTIC)),
    BUILD_TYPE("a=sctpmap:",       SDP_ATTR_SCTPMAP, SdpAttr(SDP_ATTR_SCTPMAP)),
    BUILD_TYPE("a=cliprect:",      SDP_ATTR_CLIPRECT, SdpAttrClipRect),
};
Type2Str gnets[] = {
    BUILD_TYPESTR("IN", SDP_NET_IN)
};
Type2Str gaddrs[] = {
    BUILD_TYPESTR("IP4", SDP_ADDR_IP4),
    BUILD_TYPESTR("IP6", SDP_ADDR_IP6)
};
Type2Str gmedias[] = {
    BUILD_TYPESTR("video", SDP_MEDIA_VIDEO),
    BUILD_TYPESTR("audio", SDP_MEDIA_AUDIO),
    BUILD_TYPESTR("text", SDP_MEDIA_TEXT),
    BUILD_TYPESTR("application", SDP_MEDIA_APPLICATION),
    BUILD_TYPESTR("message", SDP_MEDIA_MESSAGE)
};
Type2Str gprotos[] = {
    BUILD_TYPESTR("UDP/TLS/RTP/SAVPF", SDP_PROTO_RTP_UDP2SAVPF),
    BUILD_TYPESTR("RTP/SAVPF", SDP_PROTO_RTP_SAVPF),
    BUILD_TYPESTR("RTP/SAVP", SDP_PROTO_RTP_SAVP),
    BUILD_TYPESTR("RTP/AVP", SDP_PROTO_RTP_AVP),
    BUILD_TYPESTR("udp", SDP_PROTO_UDP),
};
Type2Str gcandis[] = {
    BUILD_TYPESTR("host", SDP_CANDI_HOST),
    BUILD_TYPESTR("srflx", SDP_CANDI_SRFLX),
    BUILD_TYPESTR("prflx", SDP_CANDI_PRFLX),
    BUILD_TYPESTR("relay", SDP_CANDI_RELAY),
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

    ENetType   readNetType(char delim = ' ');
    EAddrType  readAddrType(char delim = ' ');
    EMediaType readMediaType(char delim = ' ');
    EProtoType readProtoType(char delim = ' ');
    ECandiType readCandiType(char delim = ' ');
private:
    std::string readToken(char delim = ' ');

public:
    std::string& val;
    size_t pos;
};
void LineReader::skip(char delim) {
    size_t i = val.find_first_of(delim, pos);
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
    size_t old = pos;
    size_t i = val.find_first_of(delim, pos);
    if (i != std::string::npos) {
        pos = i+1;
        return val.substr(old, i-old);
    } else {
        pos = val.size();
        return val.substr(old, pos-old);
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
        it++;
    }
    return strtoull(str.c_str(), NULL, 10);
}

ENetType LineReader::readNetType(char delim) {
    std::string str = readToken(delim);
    if (str.size() == 0) {
        throw ParseException("no token found");
    }
    int t = str2type(str.c_str(), gnets, ARR_LEN(gnets));
    if (0 == t) {
        throw ParseException("unknow net type");
    }
    return (ENetType)t;
}
EAddrType  LineReader::readAddrType(char delim) {
    std::string str = readToken(delim);
    if (str.size() == 0) {
        throw ParseException("no token found");
    }
    int t = str2type(str.c_str(), gaddrs, ARR_LEN(gaddrs));
    if (0 == t) {
        throw ParseException("unknow addr type");
    }
    return (EAddrType)t;
}
EMediaType LineReader::readMediaType(char delim) {
    std::string str = readToken(delim);
    if (str.size() == 0) {
        throw ParseException("no token found");
    }
    int t = str2type(str.c_str(), gmedias, ARR_LEN(gmedias));
    if (0 == t) {
        throw ParseException("unknow media type");
    }
    return (EMediaType)t;
}
EProtoType LineReader::readProtoType(char delim) {
    std::string str = readToken(delim);
    if (str.size() == 0) {
        throw ParseException("no token found");
    }
    int t = str2type(str.c_str(), gprotos, ARR_LEN(gprotos));
    if (0 == t) {
        throw ParseException("unknow proto type");
    }
    return (EProtoType)t;
}
ECandiType LineReader::readCandiType(char delim) {
    std::string str = readToken(delim);
    if (str.size() == 0) {
        throw ParseException("no token found");
    }
    int t = str2type(str.c_str(), gcandis, ARR_LEN(gcandis));
    if (0 == t) {
        throw ParseException("unknow candidate type");
    }
    return (ECandiType)t;
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
            loge("invalid sdp: [%s] @ line: %d", lines[i].c_str(), i+1);
            return -1;
        }
        if (nType != SDP_NODE_ATTRIBUTE) {
            node = SdpFactory::createNode(nType);
            ret = node->parse(lines[i]);
            if (ret != 0) {
                loge("fail to parse: [%s] @ line: %d", lines[i].c_str(), i+1);
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
            loge("invalid attribute: [%s] @ line: %d", lines[i].c_str(), i+1);
            return -1;
        }
        node = SdpFactory::createAttr(aType);
        ret = node->parse(lines[i]);
        if (ret != 0) {
            loge("fail to parse: [%s] @ line: %d", lines[i].c_str(), i+1);
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
        loge("pos[%lu]:%s", lr.pos, e.what());
        return -1;
    }
    return 0;
}
int SdpOrigin::parse(std::string& l) {
    LineReader lr(l);
    lr.skip('=');
    try {
        userName = lr.readStr();
        sid = lr.readStr();
        sessVersion = lr.readUint64();
        netType = lr.readNetType();
        addrType = lr.readAddrType();
        addr = lr.readStr();
    } catch (std::exception& e) {
        loge("pos[%lu]:%s", lr.pos, e.what());
        return -1;
    }
    return 0;
}
int SdpSessName::parse(std::string& l) {
    LineReader lr(l);
    lr.skip('=');
    try {
        name = lr.readStr();
    } catch (std::exception& e) {
        loge("pos[%lu]:%s", lr.pos, e.what());
        return -1;
    }
    return 0;
}
int SdpSessInfo::parse(std::string& l) {
    LineReader lr(l);
    lr.skip('=');
    try {
        info = lr.readStr();
    } catch (std::exception& e) {
        loge("pos[%lu]:%s", lr.pos, e.what());
        return -1;
    }
    return 0;
}
int SdpUri::parse(std::string& l) {
    LineReader lr(l);
    lr.skip('=');
    try {
        uri = lr.readStr();
    } catch (std::exception& e) {
        loge("pos[%lu]:%s", lr.pos, e.what());
        return -1;
    }
    return 0;
}
int SdpEmail::parse(std::string& l) {
    LineReader lr(l);
    lr.skip('=');
    try {
        email = lr.readStr();
    } catch (std::exception& e) {
        loge("pos[%lu]:%s", lr.pos, e.what());
        return -1;
    }
    return 0;
}
int SdpPhone::parse(std::string& l) {
    LineReader lr(l);
    lr.skip('=');
    try {
        phone = lr.readStr();
    } catch (std::exception& e) {
        loge("pos[%lu]:%s", lr.pos, e.what());
        return -1;
    }
    return 0;
}
int SdpTime::parse(std::string& l) {
    LineReader lr(l);
    lr.skip('=');
    try {
        start = lr.readUint64();
        stop = lr.readUint64();
    } catch (std::exception& e) {
        loge("pos[%lu]:%s", lr.pos, e.what());
        return -1;
    }
    return 0;
}
int SdpConn::parse(std::string& l) {
    LineReader lr(l);
    lr.skip('=');
    try {
        netType = lr.readNetType();
        addrType = lr.readAddrType();
        addr = lr.readStr();
    } catch (std::exception& e) {
        loge("pos[%lu]:%s", lr.pos, e.what());
        return -1;
    }
    return 0;
}
int SdpMedia::parse(std::string& l) {
    LineReader lr(l);
    lr.skip('=');
    try {
        mediaType = lr.readMediaType();
        port = lr.readInt();
        proto = lr.readProtoType();
        while(lr.pos < lr.val.size()) {
            int pload = lr.readInt();
            supportedPTs.push_back(pload);
        }
    } catch (std::exception& e) {
        loge("pos[%lu]:%s", lr.pos, e.what());
        return -1;
    }
    return 0;
}
int SdpBandWidth::parse(std::string& l) {
    LineReader lr(l);
    lr.skip('=');
    try {
        type = lr.readStr(':');
        bw = lr.readInt();
    } catch (std::exception& e) {
        loge("pos[%lu]:%s", lr.pos, e.what());
        return -1;
    }
    return 0;
}
int SdpAttr::parse(std::string& l) {
    LineReader lr(l);
    lr.skip(':');
    if (lr.pos >= lr.val.size()) {
        val = "";
    } else {
        val = lr.val.substr(lr.pos);
    }
    return 0;
}
int SdpAttrRtpMap::parse(std::string& l) {
    LineReader lr(l);
    lr.skip(':');
    try {
        pt = lr.readInt();
        enc = lr.readStr();
        if (lr.pos < lr.val.size()) {
            param = lr.readStr();
        }
    } catch (std::exception& e) {
        loge("pos[%lu]:%s", lr.pos, e.what());
        return -1;
    }
    return 0;
}
int SdpAttrRTCPFB::parse(std::string& l) {
    LineReader lr(l);
    lr.skip(':');
    try {
        pt = lr.readInt();
        param = lr.val.substr(lr.pos);
    } catch (std::exception& e) {
        loge("pos[%lu]:%s", lr.pos, e.what());
        return -1;
    }
    return 0;
}
int SdpAttrFmtp::parse(std::string& l) {
    LineReader lr(l);
    lr.skip(':');
    try {
        pt = lr.readInt();
        param = lr.val.substr(lr.pos);
    } catch (std::exception& e) {
        loge("pos[%lu]:%s", lr.pos, e.what());
        return -1;
    }
    return 0;
}
int SdpAttrRTCP::parse(std::string& l) {
    LineReader lr(l);
    lr.skip(':');
    try {
        port = lr.readInt();
        netType = lr.readNetType();
        addrType = lr.readAddrType();
        addr = lr.readStr();
    } catch (std::exception& e) {
        loge("pos[%lu]:%s", lr.pos, e.what());
        return -1;
    }
    return 0;
}
int SdpAttrCandi::parse(std::string& l) {
    LineReader lr(l);
    std::string tmp;
    lr.skip(':');
    try {
        foundation = lr.readStr();
        compID = lr.readUint64();
        transport = lr.readStr();
        priority = lr.readUint64();
        addr = lr.readStr();
        port = lr.readInt();
        lr.skip(' ');
        candiType = lr.readCandiType();
        while(lr.pos < lr.val.size()) {
            tmp = lr.readStr();
            if (tmp == "raddr") {
                relAddr = lr.readStr();
                lr.skip(' ');
                relPort = lr.readInt();
            } else {
                extName = tmp;
                extVal = lr.readStr();
            }
        }
    } catch (std::exception& e) {
        loge("pos[%lu]:%s", lr.pos, e.what());
        return -1;
    }
    return 0;
}
int SdpAttrSsrc::parse(std::string& l) {
    LineReader lr(l);
    lr.skip(':');
    try {
        ssrc = lr.readInt();
        attr = lr.readStr(':');
        if (lr.pos < lr.val.size()) {
            val = lr.val.substr(lr.pos);
        }
    } catch (std::exception& e) {
        loge("pos[%lu]:%s", lr.pos, e.what());
        return -1;
    }
    return 0;
}
int SdpAttrSsrcGrp::parse(std::string& l) {
    LineReader lr(l);
    lr.skip(':');
    try {
        semantics = lr.readStr();
        while(lr.pos < lr.val.size()) {
            ssrcs.push_back(lr.readInt());
        }
    } catch (std::exception& e) {
        loge("pos[%lu]:%s", lr.pos, e.what());
        return -1;
    }
    return 0;
}
int SdpAttrClipRect::parse(std::string& l) {
    LineReader lr(l);
    lr.skip(':');
    try {
        x = lr.readInt(',');
        y = lr.readInt(',');
        w = lr.readInt(',');
        h = lr.readInt();
    } catch (std::exception& e) {
        loge("pos[%lu]:%s", lr.pos, e.what());
        return -1;
    }
    return 0;
}

int SdpVersion::write(std::string& l) {
    std::stringstream ss;
    ss << type2str(nodeType, gnodes, ARR_LEN(gnodes))
        << version << "\r\n";
    l += ss.str();
    return 0;
}
int SdpOrigin::write(std::string& l) {
    std::stringstream ss;
    ss << type2str(nodeType, gnodes, ARR_LEN(gnodes))
        << userName << " "
        << sid << " "
        << sessVersion << " "
        << type2str(netType, gnets, ARR_LEN(gnets)) << " "
        << type2str(addrType, gaddrs, ARR_LEN(gaddrs)) << " "
        << addr << "\r\n";
    l += ss.str();
    return 0;
}
int SdpSessName::write(std::string& l) {
    std::stringstream ss;
    ss << type2str(nodeType, gnodes, ARR_LEN(gnodes))
        << name << "\r\n";
    l += ss.str();
    return 0;
}
int SdpSessInfo::write(std::string& l) {
    std::stringstream ss;
    ss << type2str(nodeType, gnodes, ARR_LEN(gnodes))
        << info << "\r\n";
    l += ss.str();
    return 0;
}
int SdpUri::write(std::string& l) {
    std::stringstream ss;
    ss << type2str(nodeType, gnodes, ARR_LEN(gnodes))
        << uri << "\r\n";
    l += ss.str();
    return 0;
}
int SdpEmail::write(std::string& l) {
    std::stringstream ss;
    ss << type2str(nodeType, gnodes, ARR_LEN(gnodes))
        << email << "\r\n";
    l += ss.str();
    return 0;
}
int SdpPhone::write(std::string& l) {
    std::stringstream ss;
    ss << type2str(nodeType, gnodes, ARR_LEN(gnodes))
        << phone << "\r\n";
    l += ss.str();
    return 0;
}
int SdpTime::write(std::string& l) {
    std::stringstream ss;
    ss << type2str(nodeType, gnodes, ARR_LEN(gnodes))
        << start << " " << stop << "\r\n";
    l += ss.str();
    return 0;
}
int SdpConn::write(std::string& l) {
    std::stringstream ss;
    ss << type2str(nodeType, gnodes, ARR_LEN(gnodes))
        << type2str(netType, gnets, ARR_LEN(gnets)) << " "
        << type2str(addrType, gaddrs, ARR_LEN(gaddrs)) << " "
        << addr << "\r\n";
    l += ss.str();
    return 0;
}
int SdpMedia::write(std::string& l) {
    std::stringstream ss;
    ss << type2str(nodeType, gnodes, ARR_LEN(gnodes))
        << type2str(mediaType, gmedias, ARR_LEN(gmedias)) << " "
        << port << " "
        << type2str(proto, gprotos, ARR_LEN(gprotos));
    for (unsigned int i = 0; i < supportedPTs.size(); i++) {
        ss << " " << supportedPTs[i];
    }
    ss << "\r\n";
    l += ss.str();
    for (unsigned int i = 0; i < children.size(); i++) {
        children[i]->write(l);
    }

    return 0;
}
int SdpBandWidth::write(std::string& l) {
    std::stringstream ss;
    ss << type2str(nodeType, gnodes, ARR_LEN(gnodes));
    ss << type << ":" << bw << "\r\n";
    l += ss.str();
    return 0;
}
int SdpAttr::write(std::string& l) {
    std::stringstream ss;
    ss << type2str(attrType, gattrs, ARR_LEN(gattrs));
    if (val.size() > 0) {
        ss << val;
    }
    ss << "\r\n";
    l += ss.str();
    return 0;
}
int SdpAttrRtpMap::write(std::string& l) {
    std::stringstream ss;
    ss << type2str(attrType, gattrs, ARR_LEN(gattrs))
        << pt << " "
        << enc;
    if (param.size()) {
        ss << " " << param;
    }
    ss << "\r\n";
    l += ss.str();
    return 0;
}
int SdpAttrRTCPFB::write(std::string& l) {
    std::stringstream ss;
    ss << type2str(attrType, gattrs, ARR_LEN(gattrs))
        << pt << " "
        << param
        << "\r\n";
    l += ss.str();
    return 0;
}
int SdpAttrFmtp::write(std::string& l) {
    std::stringstream ss;
    ss << type2str(attrType, gattrs, ARR_LEN(gattrs))
        << pt << " "
        << param
        << "\r\n";
    l += ss.str();
    return 0;
}
int SdpAttrRTCP::write(std::string& l) {
    std::stringstream ss;
    ss << type2str(attrType, gattrs, ARR_LEN(gattrs))
        << port << " "
        << type2str(netType, gnets, ARR_LEN(gnets)) << " "
        << type2str(addrType, gaddrs, ARR_LEN(gaddrs)) << " "
        << addr
        << "\r\n";
    l += ss.str();
    return 0;
}
int SdpAttrCandi::write(std::string& l) {
    std::stringstream ss;
    ss << type2str(attrType, gattrs, ARR_LEN(gattrs))
        << foundation << " "
        << compID << " "
        << transport << " "
        << priority << " "
        << addr << " "
        << port << " "
        << "typ " << type2str(candiType, gcandis, ARR_LEN(gcandis)) << " ";
    if (relAddr.size()) {
        ss << "raddr " << relAddr << " "
            << "rport " << relPort << " ";
    }
    if (extName.size()) {
        ss << extName << " " << extVal;
    }
    ss  << "\r\n";
    l += ss.str();
    return -1;
}
int SdpAttrSsrc::write(std::string& l) {
    std::stringstream ss;
    ss << type2str(attrType, gattrs, ARR_LEN(gattrs))
        << ssrc << " "
        << attr;
    if (val.size()) ss << ":" << val;
    ss << "\r\n";
    l += ss.str();
    return 0;
}
int SdpAttrSsrcGrp::write(std::string& l) {
    std::stringstream ss;
    ss << type2str(attrType, gattrs, ARR_LEN(gattrs))
        << semantics;
    for (unsigned int i = 0; i < ssrcs.size(); i++) {
        ss <<" " << ssrcs[i];
    }
    ss << "\r\n";
    l += ss.str();
    return 0;
}
int SdpAttrClipRect::write(std::string& l) {
    std::stringstream ss;
    ss << type2str(attrType, gattrs, ARR_LEN(gattrs))
       << x << "," << y << "," << w << "," << h
       << "\r\n";
    l += ss.str();
    return 0;
}

int SdpMedia::filter(int pt) {
    for(int i=supportedPTs.size()-1; i>=0; i--) {
        if (supportedPTs[i] == pt) {
            supportedPTs.erase(supportedPTs.begin()+i);
        }
    }
    if (!children.size()) {
       return 0;
    }

    //remove the attributes releated to the given pt
    for (int i = children.size()-1; i >= 0; i--) {
        if (children[i]->nodeType != SDP_NODE_ATTRIBUTE) {
            continue;
        }
        SdpAttr* attr = (SdpAttr*)children[i];
        if (attr->attrType == SDP_ATTR_RTPMAP) {
            SdpAttrRtpMap* rtpmap = (SdpAttrRtpMap*)attr;
            if (rtpmap->pt == pt) {
                children.erase(children.begin() + i);
                delete rtpmap;
            }
            continue;
        }
        if (attr->attrType == SDP_ATTR_FMTP) {
            SdpAttrFmtp* fmtp = (SdpAttrFmtp*)attr;
            if (fmtp->pt == pt) {
                children.erase(children.begin() + i);
                delete fmtp;
            }
            continue;
        }
        if (attr->attrType == SDP_ATTR_RTCPFB) {
            SdpAttrRTCPFB* rtcpfb = (SdpAttrRTCPFB*)attr;
            if (rtcpfb->pt == pt) {
                children.erase(children.begin() + i);
                delete rtcpfb;
            }
            continue;
        }
    }

    return 0;
}
int SdpMedia::filter(EAttrType aType) {
    //remove the attributes of given type
    for (int i = children.size()-1; i >= 0; i--) {
        if (children[i]->nodeType != SDP_NODE_ATTRIBUTE) {
            continue;
        }
        SdpAttr* attr = (SdpAttr*)children[i];
        if (attr->attrType == aType) {
            children.erase(children.begin() + i);
            delete attr;
        }
    }

    return 0;
}
int SdpMedia::addCandidate(SdpNode* n) {
    children.push_back(n);
    return 0;
}
int SdpMedia::updateIce(std::string ufrag, std::string pwd, std::string fp) {
    SdpAttr* ice = NULL;

    find(SDP_ATTR_ICE_UFRAG, ice);
    if (ice == NULL) {
        ice = new SdpAttr(SDP_ATTR_ICE_UFRAG);
        children.push_back(ice);
    }
    ice->val = ufrag;

    ice = NULL;
    find(SDP_ATTR_ICE_PWD, ice);
    if (ice == NULL) {
        ice = new SdpAttr(SDP_ATTR_ICE_PWD);
        children.push_back(ice);
    }
    ice->val = pwd;

    ice = NULL;
    find(SDP_ATTR_FINGERPRINT, ice);
    if (ice == NULL) {
        ice = new SdpAttr(SDP_ATTR_FINGERPRINT);
        children.push_back(ice);
    }
    ice->val = fp;
    return 0;
}
int SdpMedia::reject() {
    port = 0;
    return 0;
}
int SdpMedia::getPT(std::string& codec) {
    int pt = -1;
    auto matcher = [&pt, &codec](SdpNode* n) {
        if (n->nodeType != SDP_NODE_ATTRIBUTE) {
            return;
        }
        SdpAttr* attr = (SdpAttr*)n;
        if (attr->attrType != SDP_ATTR_RTPMAP) {
            return;
        }
        SdpAttrRtpMap* rtpmap = (SdpAttrRtpMap*)attr;
        if (rtpmap->enc.find(codec) != std::string::npos) {
            pt = rtpmap->pt;
        }
    };
    std::for_each(children.begin(), children.end(), matcher);
    return pt;
}
uint32_t SdpMedia::ssrc() {
    auto matcher = [](SdpNode* n) {
        if (n->nodeType != SDP_NODE_ATTRIBUTE) return false;
        if(((SdpAttr*)n)->attrType != SDP_ATTR_SSRC) return false;
        return true;
    };
    auto it = children.end();
    it = std::find_if(children.begin(), children.end(), matcher);
    if (it != children.end()) {
        return ((SdpAttrSsrc*)(*it))->ssrc;
    } else {
        return 0;
    }
}
std::vector<uint32_t> SdpMedia::ssrcGrp() {
    std::vector<uint32_t> vec;
    auto matcher = [](SdpNode* n) {
        if (n->nodeType != SDP_NODE_ATTRIBUTE) return false;
        if(((SdpAttr*)n)->attrType != SDP_ATTR_SSRC_GROUP) return false;
        return true;
    };
    auto it = children.end();
    it = std::find_if(children.begin(), children.end(), matcher);
    if (it != children.end()) {
        return ((SdpAttrSsrcGrp*)(*it))->ssrcs;
    } else {
        return vec;
    }
}
int SdpMedia::inactive() {
    for(auto it=children.begin(); it!=children.end(); it++) {
        if((*it)->nodeType == SDP_NODE_ATTRIBUTE) continue;
        SdpAttr* attr = (SdpAttr*)(*it);
        if(attr->attrType == SDP_ATTR_INACTIVE) return 0;
    }

    //not find inactive then create one
    SdpAttr* attr = new SdpAttr(SDP_ATTR_INACTIVE);
    children.push_back(attr);
    return 0;
}
uint32_t SdpMedia::bandwidth() {
    for(auto it=children.begin(); it!=children.end(); it++) {
        if((*it)->nodeType != SDP_NODE_BANDWIDTH) continue;
        SdpBandWidth* bw = (SdpBandWidth*)(*it);
        return bw->bw;
    }
    return 0;
}

static const char kBase64[64] = {
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M',
    'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
    'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm',
    'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '/'};
static const char kHex[16] = {'0', '1', '2', '3', '4', '5', '6', '7',
                              '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};
static const char kUuidDigit17[4] = {'8', '9', 'a', 'b'};

static std::string randomString(uint32_t len) {
    std::string str;
    str.reserve(len);
    std::unique_ptr<uint8_t[]> bytes(new uint8_t[len]);
    RAND_bytes(bytes.get(), len);
    for(unsigned int i=0; i<len; i++) {
        str.push_back(kBase64[bytes[i]%ARR_LEN(kBase64)]);
    }
    return str;
}
static std::string randomUUID() {
    std::string str;
    str.reserve(36);
    std::unique_ptr<uint8_t[]> bytes(new uint8_t[31]);
    for (size_t i = 0; i < 8; ++i) {
        str.push_back(kHex[bytes[i] % 16]);
    }
    str.push_back('-');
    for (size_t i = 8; i < 12; ++i) {
        str.push_back(kHex[bytes[i] % 16]);
    }
    str.push_back('-');
    str.push_back('4');
    for (size_t i = 12; i < 15; ++i) {
        str.push_back(kHex[bytes[i] % 16]);
    }
    str.push_back('-');
    str.push_back(kUuidDigit17[bytes[15] % 4]);
    for (size_t i = 16; i < 19; ++i) {
        str.push_back(kHex[bytes[i] % 16]);
    }
    str.push_back('-');
    for (size_t i = 19; i < 31; ++i) {
        str.push_back(kHex[bytes[i] % 16]);
    }
    return str;
}

static const uint32_t kCrc32Polynomial = 0xEDB88320;
static uint32_t kCrc32Table[256] = {0};
static void EnsureCrc32TableInited() {
    if (kCrc32Table[ARR_LEN(kCrc32Table) - 1])
        return;  // already inited
    for (uint32_t i = 0; i < ARR_LEN(kCrc32Table); ++i) {
        uint32_t c = i;
        for (size_t j = 0; j < 8; ++j) {
            if (c & 1) {
                c = kCrc32Polynomial ^ (c >> 1);
            } else {
                c >>= 1;
            }
        }
        kCrc32Table[i] = c;
    }
}

uint32_t UpdateCrc32(uint32_t start, const void* buf, size_t len) {
    EnsureCrc32TableInited();

    uint32_t c = start ^ 0xFFFFFFFF;
    const uint8_t* u = static_cast<const uint8_t*>(buf);
    for (size_t i = 0; i < len; ++i) {
        c = kCrc32Table[(c ^ u[i]) & 0xFF] ^ (c >> 8);
    }
    return c ^ 0xFFFFFFFF;
}

std::string generateCname() {
    return randomString(16);
}

std::string generateLabel() {
    return randomUUID();
}

std::string generateMslabel() {
    return randomString(36);
}

std::string generateFoundation(const std::string& type,
    const std::string& protocol,
    const std::string& relay_protocol,
    const std::string& address) {
    std::ostringstream oss;
    oss << type << protocol << relay_protocol << address;
    uint32_t crc = UpdateCrc32(0, oss.str().c_str(), oss.str().size());
    return std::to_string(crc);
}

};

/********************************** END **********************************************/

