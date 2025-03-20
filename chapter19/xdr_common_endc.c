#include <rpc/xdr.h>
#include <string.h>
#include <xdr_common_decl.h>

int xEncodeMsg(char *buffer, struct MsgXdr *m)
{
	XDR xdrs;
	char* p;
	xdrmem_create(&xdrs, buffer, sizeof (struct MsgXdr), XDR_ENCODE);
	xdr_int(&xdrs, &m->type);
	xdr_int(&xdrs, &m->usr_id);
	p = m->name;
	xdr_string(&xdrs, (char**)&p, strlen(m->name));
	p = m->number;
	xdr_string(&xdrs, (char**)&p, strlen(m->number));
	return 0;
}

int xDecodeMsg(char *buffer, struct MsgXdr *m)
{
	XDR xdrs;
	char* p;
	xdrmem_create(&xdrs, buffer, sizeof (struct MsgXdr), XDR_DECODE);
	xdr_int(&xdrs, &m->type);
	xdr_int(&xdrs, &m->usr_id);
	p = m->name;
	xdr_string(&xdrs, (char**)&p, 128);
	p = m->number;
	xdr_string(&xdrs, (char**)&p, 24);
	return 0;
}
