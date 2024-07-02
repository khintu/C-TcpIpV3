
int passivesock(const char *service, const char *transport, const int qlen);

int passiveUDP(const char *service)
{
	return passivesock(service, "udp", 0);
}
