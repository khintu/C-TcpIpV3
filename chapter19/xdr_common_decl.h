#ifndef XDR_COMMON_DECL_H
#define XDR_COMMON_DECL_H

struct MsgXdr {
	int type;
	int usr_id;
	char name[128];
	char number[24];
};


int xEncodeMsg(char *buffer, struct MsgXdr *m);
int xDecodeMsg(char *buffer, struct MsgXdr *m);

#endif /* XDR_COMMON_DECL_H */
