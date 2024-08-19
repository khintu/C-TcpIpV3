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

exc_13_1:
	gcc chapter9/passivesock.c chapter9/passiveUDP.c chapter10/passiveTCP.c chapter7/errexit.c chapter13/exc_13_1.c -o daytimed
	gcc chapter7/connectsock.c chapter7/connectTCP.c chapter7/errexit.c chapter7/TCPdaytime.c -o daytime

exc_13_3:
	gcc chapter9/passivesock.c chapter9/passiveUDP.c chapter10/passiveTCP.c chapter7/errexit.c chapter13/exc_13_3.c -o daytimed
	gcc chapter7/connectsock.c chapter7/connectTCP.c chapter7/errexit.c chapter7/TCPdaytime.c -o daytime

exc_13_4:
	gcc chapter9/passivesock.c chapter9/passiveUDP.c chapter10/passiveTCP.c chapter7/errexit.c chapter13/exc_13_4.c -o daytimed
	gcc chapter7/connectsock.c chapter7/connectUDP.c chapter7/errexit.c chapter7/UDPdaytime.c -o daytimeu
	gcc chapter7/connectsock.c chapter7/connectTCP.c chapter7/errexit.c chapter7/TCPdaytime.c -o daytimet

exc_13_5:
	gcc chapter9/passivesock.c chapter9/passiveUDP.c chapter10/passiveTCP.c chapter7/errexit.c chapter13/exc_13_5.c -o daytimed
	gcc chapter7/connectsock.c chapter7/connectTCP.c chapter7/errexit.c chapter7/TCPdaytime.c -o daytimet
	gcc chapter7/connectsock.c chapter7/connectUDP.c chapter7/errexit.c chapter7/udpreadtimed.c chapter7/exc_13_5.c -o daytimeu

exc_14_1:
	gcc chapter9/passivesock.c chapter9/passiveUDP.c chapter10/passiveTCP.c chapter7/errexit.c chapter14/exc_14_1.c chapter14/sv_funcs.c -o superd

exc_14_3:
	gcc chapter9/passivesock.c chapter9/passiveUDP.c chapter10/passiveTCP.c chapter7/errexit.c chapter14/exc_14_3.c chapter14/sv_funcs.c -o superd
	gcc chapter7/connectsock.c chapter7/connectTCP.c chapter7/errexit.c chapter14/TCPchargen2.c -o chargen

exc_14_4:
	gcc chapter9/passivesock.c chapter9/passiveUDP.c chapter10/passiveTCP.c chapter7/errexit.c chapter14/exc_14_4.c chapter14/sv_funcs.c -o superd
	gcc chapter7/connectsock.c chapter7/connectUDP.c chapter7/errexit.c chapter7/UDPtime.c -o time

# Defaults client/servers
daytimetcp:
	gcc chapter7/connectsock.c chapter7/connectTCP.c chapter7/errexit.c chapter7/TCPdaytime.c -o daytime

daytimedtcp:
	gcc chapter9/passivesock.c chapter10/passiveTCP.c chapter7/errexit.c chapter10/TCPdaytimed.c -o daytimed

timetcp:
	gcc chapter7/connectsock.c chapter7/connectTCP.c chapter7/errexit.c chapter7/TCPtime.c -o time

timeudp:
	gcc chapter7/connectsock.c chapter7/connectUDP.c chapter7/errexit.c chapter7/UDPtime.c -o time

timedudp:
	gcc chapter9/passivesock.c chapter9/passiveUDP.c chapter7/errexit.c chapter8.20/UDPtimed.c -o timed

echotcp:
	gcc chapter7/connectsock.c chapter7/connectTCP.c chapter7/errexit.c chapter7/TCPecho.c -o echo	

echodtcp:
	gcc chapter7/errexit.c chapter9/passivesock.c chapter10/passiveTCP.c chapter11/TCPechod.c -o echod

mechodtcp:
	gcc chapter7/errexit.c chapter9/passivesock.c chapter10/passiveTCP.c chapter12/TCPmechod.c -o mechod

mdaytimedtcp:
	gcc chapter9/passivesock.c chapter10/passiveTCP.c chapter7/errexit.c chapter12/TCPmdaytimed.c -o mdaytimed

mdaytimed:
	gcc chapter9/passivesock.c chapter9/passiveUDP.c chapter10/passiveTCP.c chapter7/errexit.c chapter13/daytimed.c -o mdaytimed

msvcdudp:
	gcc chapter9/passivesock.c chapter9/passiveUDP.c chapter7/errexit.c chapter14/UDPmultisvcd.c -o msvcd
	gcc chapter7/connectsock.c chapter7/connectUDP.c chapter7/errexit.c chapter7/UDPdaytime.c -o daytime
	gcc chapter7/connectsock.c chapter7/connectUDP.c chapter7/errexit.c chapter7/UDPtime.c -o time
	gcc chapter7/connectsock.c chapter7/connectUDP.c chapter7/errexit.c chapter7/UDPecho.c -o echo	

superd:
	gcc chapter9/passivesock.c chapter9/passiveUDP.c chapter10/passiveTCP.c chapter7/errexit.c chapter14/superd.c chapter14/sv_funcs.c -o superd
	gcc chapter7/connectsock.c chapter7/connectTCP.c chapter7/errexit.c chapter7/TCPecho.c -o echo	
	gcc chapter7/connectsock.c chapter7/connectTCP.c chapter7/errexit.c chapter7/TCPdaytime.c -o daytime
	gcc chapter7/connectsock.c chapter7/connectTCP.c chapter7/errexit.c chapter7/TCPtime.c -o time
	gcc chapter7/connectsock.c chapter7/connectTCP.c chapter7/errexit.c chapter14/TCPchargen.c -o chargen

all: daytimetcp daytimedtcp timeudp timetcp timedudp echotcp echodtcp mechodtcp mdaytimedtcp mdaytimed msvcdudp superd

clean:
	rm -f a.out time timed daytimed daytime echo echod mechod mdaytimed exc12_3 daytimet daytimeu msvcd chargen superd
