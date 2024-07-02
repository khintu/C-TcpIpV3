int connectsock(const char *,const char *,const char *);

int connectUDP(const char *host, const char *service)
{
	return connectsock(host, service, "udp");
}
