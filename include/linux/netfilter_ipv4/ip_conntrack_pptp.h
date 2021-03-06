/* PPTP constants and structs */
#ifndef _CONNTRACK_PPTP_H
#define _CONNTRACK_PPTP_H

/* state of the control session */
enum pptp_ctrlsess_state {
	PPTP_SESSION_NONE,			/* no session present */
	PPTP_SESSION_ERROR,			/* some session error */
	PPTP_SESSION_STOPREQ,			/* stop_sess request seen */
	PPTP_SESSION_REQUESTED,			/* start_sess request seen */
	PPTP_SESSION_CONFIRMED,			/* session established */
};

/* state of the call inside the control session */
enum pptp_ctrlcall_state {
	PPTP_CALL_NONE,
	PPTP_CALL_ERROR,
	PPTP_CALL_OUT_REQ,
	PPTP_CALL_OUT_CONF,
	PPTP_CALL_IN_REQ,
	PPTP_CALL_IN_REP,
	PPTP_CALL_IN_CONF,
	PPTP_CALL_CLEAR_REQ,
};


/* conntrack private data */
struct ip_ct_pptp_master {
	enum pptp_ctrlsess_state sstate;	/* session state */

	/* everything below is going to be per-expectation in newnat,
	 * since there could be more than one call within one session */
	enum pptp_ctrlcall_state cstate;	/* call state */
	u_int16_t pac_call_id;			/* call id of PAC, host byte order */
	u_int16_t pns_call_id;			/* call id of PNS, host byte order */
};

/* conntrack_expect private member */
struct ip_ct_pptp_expect {
	enum pptp_ctrlcall_state cstate; 	/* call state */
	u_int16_t pac_call_id;			/* call id of PAC */
	u_int16_t pns_call_id;			/* call id of PNS */
};


#ifdef __KERNEL__

#include <linux/netfilter_ipv4/lockhelp.h>
DECLARE_LOCK_EXTERN(ip_pptp_lock);

#define IP_CONNTR_PPTP		PPTP_CONTROL_PORT

union pptp_ctrl_union {
                void				*rawreq;
		struct PptpStartSessionRequest	*sreq;
		struct PptpStartSessionReply	*srep;
		struct PptpStopSessionReqest	*streq;
		struct PptpStopSessionReply	*strep;
                struct PptpOutCallRequest       *ocreq;
                struct PptpOutCallReply         *ocack;
                struct PptpInCallRequest        *icreq;
                struct PptpInCallReply          *icack;
                struct PptpInCallConnected      *iccon;
		struct PptpClearCallRequest	*clrreq;
                struct PptpCallDisconnectNotify *disc;
                struct PptpWanErrorNotify       *wanerr;
                struct PptpSetLinkInfo          *setlink;
};



#define PPTP_CONTROL_PORT	1723

#define PPTP_PACKET_CONTROL	1
#define PPTP_PACKET_MGMT	2

#define PPTP_MAGIC_COOKIE	0x1a2b3c4d

struct pptp_pkt_hdr {
	__u16	packetLength;
	__u16	packetType;
	__u32	magicCookie;
};

/* PptpControlMessageType values */
#define PPTP_START_SESSION_REQUEST	1
#define PPTP_START_SESSION_REPLY	2
#define PPTP_STOP_SESSION_REQUEST	3
#define PPTP_STOP_SESSION_REPLY		4
#define PPTP_ECHO_REQUEST		5
#define PPTP_ECHO_REPLY			6
#define PPTP_OUT_CALL_REQUEST		7
#define PPTP_OUT_CALL_REPLY		8
#define PPTP_IN_CALL_REQUEST		9
#define PPTP_IN_CALL_REPLY		10
#define PPTP_IN_CALL_CONNECT		11
#define PPTP_CALL_CLEAR_REQUEST		12
#define PPTP_CALL_DISCONNECT_NOTIFY	13
#define PPTP_WAN_ERROR_NOTIFY		14
#define PPTP_SET_LINK_INFO		15

#define PPTP_MSG_MAX			15

/* PptpGeneralError values */
#define PPTP_ERROR_CODE_NONE		0
#define PPTP_NOT_CONNECTED		1
#define PPTP_BAD_FORMAT			2
#define PPTP_BAD_VALUE			3
#define PPTP_NO_RESOURCE		4
#define PPTP_BAD_CALLID			5
#define PPTP_REMOVE_DEVICE_ERROR	6

struct PptpControlHeader {
	__u16	messageType;
	__u16	reserved;
};

/* FramingCapability Bitmap Values */
#define PPTP_FRAME_CAP_ASYNC		0x1
#define PPTP_FRAME_CAP_SYNC		0x2

/* BearerCapability Bitmap Values */
#define PPTP_BEARER_CAP_ANALOG		0x1
#define PPTP_BEARER_CAP_DIGITAL		0x2

struct PptpStartSessionRequest {
	__u16	protocolVersion;
	__u8	reserved1;
	__u8	reserved2;
	__u32	framingCapability;
	__u32	bearerCapability;
	__u16	maxChannels;
	__u16	firmwareRevision;
	__u8	hostName[64];
	__u8	vendorString[64];
};

/* PptpStartSessionResultCode Values */
#define PPTP_START_OK			1
#define PPTP_START_GENERAL_ERROR	2
#define PPTP_START_ALREADY_CONNECTED	3
#define PPTP_START_NOT_AUTHORIZED	4
#define PPTP_START_UNKNOWN_PROTOCOL	5

struct PptpStartSessionReply {
	__u16	protocolVersion;
	__u8	resultCode;
	__u8	generalErrorCode;
	__u32	framingCapability;
	__u32	bearerCapability;
	__u16	maxChannels;
	__u16	firmwareRevision;
	__u8	hostName[64];
	__u8	vendorString[64];
};

/* PptpStopReasons */
#define PPTP_STOP_NONE			1
#define PPTP_STOP_PROTOCOL		2
#define PPTP_STOP_LOCAL_SHUTDOWN	3

struct PptpStopSessionRequest {
	__u8	reason;
};

/* PptpStopSessionResultCode */
#define PPTP_STOP_OK			1
#define PPTP_STOP_GENERAL_ERROR		2

struct PptpStopSessionReply {
	__u8	resultCode;
	__u8	generalErrorCode;
};

struct PptpEchoRequest {
	__u32 identNumber;
};

/* PptpEchoReplyResultCode */
#define PPTP_ECHO_OK			1
#define PPTP_ECHO_GENERAL_ERROR		2

struct PptpEchoReply {
	__u32	identNumber;
	__u8	resultCode;
	__u8	generalErrorCode;
	__u16	reserved;
};

/* PptpFramingType */
#define PPTP_ASYNC_FRAMING		1
#define PPTP_SYNC_FRAMING		2
#define PPTP_DONT_CARE_FRAMING		3

/* PptpCallBearerType */
#define PPTP_ANALOG_TYPE		1
#define PPTP_DIGITAL_TYPE		2
#define PPTP_DONT_CARE_BEARER_TYPE	3

struct PptpOutCallRequest {
	__u16	callID;
	__u16	callSerialNumber;
	__u32	minBPS;
	__u32	maxBPS;
	__u32	bearerType;
	__u32	framingType;
	__u16	packetWindow;
	__u16	packetProcDelay;
	__u16	reserved1;
	__u16	phoneNumberLength;
	__u16	reserved2;
	__u8	phoneNumber[64];
	__u8	subAddress[64];
};

/* PptpCallResultCode */
#define PPTP_OUTCALL_CONNECT		1
#define PPTP_OUTCALL_GENERAL_ERROR	2
#define PPTP_OUTCALL_NO_CARRIER		3
#define PPTP_OUTCALL_BUSY		4
#define PPTP_OUTCALL_NO_DIAL_TONE	5
#define PPTP_OUTCALL_TIMEOUT		6
#define PPTP_OUTCALL_DONT_ACCEPT	7

struct PptpOutCallReply {
	__u16	callID;
	__u16	peersCallID;
	__u8	resultCode;
	__u8	generalErrorCode;
	__u16	causeCode;
	__u32	connectSpeed;
	__u16	packetWindow;
	__u16	packetProcDelay;
	__u32	physChannelID;
};

struct PptpInCallRequest {
	__u16	callID;
	__u16	callSerialNumber;
	__u32	callBearerType;
	__u32	physChannelID;
	__u16	dialedNumberLength;
	__u16	dialingNumberLength;
	__u8	dialedNumber[64];
	__u8	dialingNumber[64];
	__u8	subAddress[64];
};

/* PptpInCallResultCode */
#define PPTP_INCALL_ACCEPT		1
#define PPTP_INCALL_GENERAL_ERROR	2
#define PPTP_INCALL_DONT_ACCEPT		3

struct PptpInCallReply {
	__u16	callID;
	__u16	peersCallID;
	__u8	resultCode;
	__u8	generalErrorCode;
	__u16	packetWindow;
	__u16	packetProcDelay;
	__u16	reserved;
};

struct PptpInCallConnected {
	__u16	peersCallID;
	__u16	reserved;
	__u32	connectSpeed;
	__u16	packetWindow;
	__u16	packetProcDelay;
	__u32	callFramingType;
};

struct PptpClearCallRequest {
	__u16	callID;
	__u16	reserved;
};

struct PptpCallDisconnectNotify {
	__u16	callID;
	__u8	resultCode;
	__u8	generalErrorCode;
	__u16	causeCode;
	__u16	reserved;
	__u8	callStatistics[128];
};

struct PptpWanErrorNotify {
	__u16	peersCallID;
	__u16	reserved;
	__u32	crcErrors;
	__u32	framingErrors;
	__u32	hardwareOverRuns;
	__u32	bufferOverRuns;
	__u32	timeoutErrors;
	__u32	alignmentErrors;
};

struct PptpSetLinkInfo {
	__u16	peersCallID;
	__u16	reserved;
	__u32	sendAccm;
	__u32	recvAccm;
};


struct pptp_priv_data {
	__u16	call_id;
	__u16	mcall_id;
	__u16	pcall_id;
};

#endif /* __KERNEL__ */
#endif /* _CONNTRACK_PPTP_H */
