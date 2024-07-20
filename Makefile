default:

# Exercises
exc_11_1:
	gcc chapter7/errexit.c chapter9/passivesock.c chapter10/passiveTCP.c chapter11/exc_11_1.c -o echod
	gcc chapter7/connectsock.c chapter7/connectTCP.c chapter7/errexit.c chapter7/exc_11_1.c -o echo

exc_11_3:
	gcc chapter7/errexit.c chapter9/passivesock.c chapter10/passiveTCP.c chapter11/exc_11_3.c -o echod
	gcc chapter7/connectsock.c chapter7/connectTCP.c chapter7/errexit.c chapter7/TCPecho.c -o echo

exc_12_3:
	gcc chapter12/exc_12_3.c -o exc12_3

# Defaults client/servers
daytimetcp:
	gcc chapter7/connectsock.c chapter7/connectTCP.c chapter7/errexit.c chapter7/TCPdaytime.c -o daytime

daytimedtcp:
	gcc chapter9/passivesock.c chapter10/passiveTCP.c chapter7/errexit.c chapter10/TCPdaytimed.c -o daytimed

daytimeudp:
	gcc chapter7/connectsock.c chapter7/connectUDP.c chapter7/errexit.c chapter7/UDPtime.c -o daytime

daytimedudp:
	gcc chapter9/passivesock.c chapter9/passiveUDP.c chapter7/errexit.c chapter8.20/UDPtimed.c -o daytimed

echotcp:
	gcc chapter7/connectsock.c chapter7/connectTCP.c chapter7/errexit.c chapter7/TCPecho.c -o echo	

echodtcp:
	gcc chapter7/errexit.c chapter9/passivesock.c chapter10/passiveTCP.c chapter11/TCPechod.c -o echod

mechodtcp:
	gcc chapter7/errexit.c chapter9/passivesock.c chapter10/passiveTCP.c chapter12/TCPmechod.c -o mechod

mdaytimedtcp:
	gcc chapter9/passivesock.c chapter10/passiveTCP.c chapter7/errexit.c chapter12/TCPmdaytimed.c -o mdaytimed

all: daytimetcp daytimedtcp daytimeudp daytimedudp echotcp echodtcp mechodtcp mdaytimedtcp

clean:
	rm -f a.out daytimed daytime echo echod mechod mdaytimed exc12_3
