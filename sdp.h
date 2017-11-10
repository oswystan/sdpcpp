/*
 **************************************************************************************
 *       Filename:  sdp.h
 *    Description:   header file
 *
 *        Version:  1.0
 *        Created:  2017-11-09 17:52:01
 *
 *       Revision:  initial draft;
 **************************************************************************************
 */

#ifndef SDP_H_INCLUDED
#define SDP_H_INCLUDED

#include <string>
#include <vector>

namespace sdp {

//======================================
//
//               enum(s)
//
//======================================
enum ENodeType {
    SDP_NODE_NONE = 0,
    SDP_NODE_SESSION,                // full sdp session
    SDP_NODE_VERSION,
    SDP_NODE_ORIGIN,
    SDP_NODE_SESSION_NAME,
    SDP_NODE_SESSION_INFORMATION,    // 5
    SDP_NODE_URI,
    SDP_NODE_EMAIL,
    SDP_NODE_PHONE,
    SDP_NODE_CONNECTION,
    SDP_NODE_BANDWIDTH,              // 10
    SDP_NODE_TZ,
    SDP_NODE_ENCRYPTION,
    SDP_NODE_TIME,
    SDP_NODE_REPEAT,
    SDP_NODE_MEDIA,                 // 15
    SDP_NODE_ATTRIBUTE
};
enum ENetType {
    SDP_NET_NONE = 0,
    SDP_NET_IN
};
enum EAddrType {
    SDP_ADDR_NONE = 0,
    SDP_ADDR_IP4,
    SDP_ADDR_IP6
};
enum EMediaType {
    SDP_MEDIA_NONE = 0,
    SDP_MEDIA_VIDEO,
    SDP_MEDIA_AUDIO,
    SDP_MEDIA_TEXT,
    SDP_MEDIA_APPLICATION,
    SDP_MEDIA_MESSAGE
};
enum EProtoType {
    SDP_PROTO_NONE = 0,
    SDP_PROTO_UDP,
    SDP_PROTO_RTP_AVP,
    SDP_PROTO_RTP_SAVP,
    SDP_PROTO_RTP_SAVPF,
    SDP_PROTO_RTP_UDP2SAVPF
};
enum EAttrType {
    SDP_ATTR_NONE = 0,

    // rfc4566
    SDP_ATTR_CAT,
    SDP_ATTR_KEYWDS,
    SDP_ATTR_TOOL,
    SDP_ATTR_PTIME,
    SDP_ATTR_MAXPTIME,
    SDP_ATTR_RTPMAP,
    SDP_ATTR_RECVONLY,
    SDP_ATTR_SENDRECV,
    SDP_ATTR_SENDONLY,
    SDP_ATTR_INACTIVE,
    SDP_ATTR_ORIENT,
    SDP_ATTR_TYPE,
    SDP_ATTR_CHARSET,
    SDP_ATTR_SDPLANG,
    SDP_ATTR_LANG,
    SDP_ATTR_FRAMERATE,
    SDP_ATTR_QUALITY,
    SDP_ATTR_FMTP,

    //rfc3605
    SDP_ATTR_RTCP,
    SDP_ATTR_CANDIDATE,
    SDP_ATTR_ICE_UFRAG,
    SDP_ATTR_ICE_PWD,
    SDP_ATTR_FINGERPRINT,
    SDP_ATTR_MSID_SEMANTIC,
};
enum ECandiType {
    SDP_CANDI_NONE,
    SDP_CANDI_HOST,
    SDP_CANDI_SRFLX,
    SDP_CANDI_PRFLX,
    SDP_CANDI_RELAY
};

//======================================
//
//               interfaces
//
//======================================
class SdpRoot;
class SdpNode;
class SdpWriter;
class SdpReader;
class SdpMedia;
class SdpAttr;

class SdpReader {
public:
    int parse(std::string& s, SdpRoot& sdp);
};
class SdpWriter {
public:
    int writeTo(std::string& s, SdpRoot& sdp);
};

class SdpNode {
public:
    SdpNode(ENodeType t) : nodeType(t) {}
    virtual ~SdpNode();
    virtual void dump();

    int add(SdpNode* n);
    int remove(SdpNode* n);

    int find(ENodeType t,  SdpNode*& n);
    int find(EAttrType t,  SdpAttr*& n);
    int find(EMediaType t, SdpMedia*& n);
    int find(ENodeType t,  std::vector<SdpNode*> v);
    int find(EAttrType t,  std::vector<SdpAttr*> v);
    int find(EMediaType t, std::vector<SdpMedia*> v);
    virtual int parse(std::string& l) { return -1; };

public:
    ENodeType nodeType;
    std::vector<SdpNode*> children;
};
class SdpRoot : public SdpNode {
public:
    SdpRoot() : SdpNode(SDP_NODE_SESSION) {}
};
class SdpFactory {
public:
    ENodeType getNodeType(std::string& l);
    EAttrType getAttrType(std::string& l);

    SdpNode* createNode(ENodeType type);
    SdpAttr* createAttr(EAttrType type);
};

//======================================
//
// sub class used for reader and writer
//
//======================================
class SdpVersion : public SdpNode {
public:
    SdpVersion() : SdpNode(SDP_NODE_VERSION) {}
public:
    int version;
};
class SdpOrigin : public SdpNode {
public:
    SdpOrigin() : SdpNode(SDP_NODE_ORIGIN) {}
    int parse(std::string& l);
public:
    std::string userName;
    std::string sid;
    uint64_t    sessVersion;
    ENetType    netType;
    EAddrType   addrType;
    std::string addr;
};
class SdpSessName : public SdpNode {
public:
    SdpSessName() : SdpNode(SDP_NODE_SESSION_NAME) {}
    int parse(std::string& l);
public:
    std::string name;
};
class SdpSessInfo : public SdpNode {
public:
    SdpSessInfo() : SdpNode(SDP_NODE_SESSION_INFORMATION) {}
    int parse(std::string& l);
public:
    std::string info;
};
class SdpUri : public SdpNode {
public:
    SdpUri() : SdpNode(SDP_NODE_URI) {}
    int parse(std::string& l);
public:
    std::string uri;
};
class SdpEmail : public SdpNode {
public:
    SdpEmail() : SdpNode(SDP_NODE_EMAIL) {}
    int parse(std::string& l);
public:
    std::string email;
};
class SdpPhone : public SdpNode {
public:
    SdpPhone() : SdpNode(SDP_NODE_PHONE) {}
    int parse(std::string& l);
public:
    std::string phone;
};
class SdpTime : public SdpNode {
public:
    SdpTime() : SdpNode(SDP_NODE_TIME) {}
    int parse(std::string& l);
public:
    uint64_t start;
    uint64_t stop;
};
class SdpConn : public SdpNode {
public:
    SdpConn() : SdpNode(SDP_NODE_CONNECTION) {}
    int parse(std::string& l);
public:
    ENetType    netType;
    EAddrType   addrType;
    std::string addr;
};
class SdpMedia : public SdpNode {
public:
    SdpMedia() : SdpNode(SDP_NODE_CONNECTION) {}
    int parse(std::string& l);
    int filter(int pt);
    int reject(int pt);

public:
    EMediaType       mediaType;
    uint16_t         port;
    EProtoType       proto;
    std::vector<int> pt;
};
class SdpAttr : public SdpNode {
public:
    SdpAttr(EAttrType t) : SdpNode(SDP_NODE_ATTRIBUTE), attrType(t) {}
    int parse(std::string& l);
public:
    EAttrType   attrType;
    std::string name;
    std::string val;
};
class SdpAttrRTCP : public SdpAttr {
public:
    SdpAttrRTCP() : SdpAttr(SDP_ATTR_RTCP) {}
    int parse(std::string& l);
public:
    uint16_t    port;
    ENetType    netType;
    EAddrType   addrType;
    std::string addr;
};
class SdpAttrCandi : public SdpAttr {
public:
    SdpAttrCandi() : SdpAttr(SDP_ATTR_CANDIDATE) {}
    int parse(std::string& l);
public:
    std::string foundation;
    uint64_t    compID;
    std::string transport;
    uint64_t    priority;
    std::string addr;
    uint16_t    port;
    ECandiType  candiType;
    std::string relAddr;
    uint16_t    relPort;
    std::string extName;
    std::string extVal;
};

}; //namespace sdp

#endif /*SDP_H_INCLUDED*/

/********************************** END **********************************************/
