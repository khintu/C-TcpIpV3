
int passivesock(const char *service, const char *transport, const int qlen);

int passiveTCP(const char *service, const int qlen)
{
	return passivesock(service, "tcp", qlen);
}
