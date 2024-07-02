int connectsock(const char *,const char *,const char *);

int connectTCP(const char *host, const char *service)
{
	return connectsock(host, service, "tcp");
}
