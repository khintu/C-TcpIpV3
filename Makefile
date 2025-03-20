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

exc_14_6:
	gcc chapter9/passivesock.c chapter9/passiveUDP.c chapter10/passiveTCP.c chapter7/errexit.c chapter14/exc_14_6.c chapter14/sv_funcs.c -o superd
	gcc chapter7/connectsock.c chapter7/connectTCP.c chapter7/errexit.c chapter7/TCPecho.c -o echo	
	gcc chapter14/inetd_echod.c chapter7/errexit.c -o inetd_echod

exc_udp_shm:
	gcc -I./chapter15 chapter9/passivesock.c chapter9/passiveUDP.c chapter7/errexit.c chapter15/glb_shm_obj.c chapter15/exc_15_udp_shm.c -o timed

exc_udp_shm2:
	gcc -I./chapter15 chapter9/passivesock.c chapter9/passiveUDP.c chapter7/errexit.c chapter15/glb_shm_obj.c chapter15/exc_15_udp_shm2.c -o timed

exc_15_6:
	gcc chapter7/errexit.c chapter9/passivesock.c chapter10/passiveTCP.c chapter15/exc_15_6.c -o echod

exc_15_7:
	gcc chapter9/passivesock.c chapter9/passiveUDP.c chapter7/errexit.c chapter15/exc_15_7.c -o msvcd
	gcc chapter7/connectsock.c chapter7/connectUDP.c chapter7/errexit.c chapter7/UDPdaytime.c -o daytime
	gcc chapter7/connectsock.c chapter7/connectUDP.c chapter7/errexit.c chapter7/UDPtime.c -o time
	gcc chapter7/connectsock.c chapter7/connectUDP.c chapter7/errexit.c chapter7/UDPecho.c -o echo	

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

inetd_daytimed:
	gcc chapter14/inetd_daytimed.c -o inetd_daytimed
	sudo kill -s SIGHUP `pidof inetd`

pre_echodtcp:
	gcc chapter7/errexit.c chapter9/passivesock.c chapter10/passiveTCP.c chapter15/tcp_preallctn_echod.c -o echod

pre_timedudp:
	gcc chapter9/passivesock.c chapter9/passiveUDP.c chapter7/errexit.c chapter15/udp_preallctn_timed.c -o timed

delyd_echodtcp:
	gcc chapter7/errexit.c chapter9/passivesock.c chapter10/passiveTCP.c chapter15/tcp_delayed_allctn.c -o echod

delyd_ad_persist_echodtcp:
	gcc chapter7/errexit.c chapter9/passivesock.c chapter10/passiveTCP.c chapter15/tcp_delayed_allctn_ad_persist.c -o echod

delyd_ad_prst_shm_echodtcp:
	gcc -I./chapter15 chapter7/errexit.c chapter9/passivesock.c chapter10/passiveTCP.c chapter15/glb_shm_obj.c chapter15/tcp_delayed_allctn_ad_persist_shm.c -o echod

techotcp:
	gcc chapter16/connectsock.c chapter16/connectTCP.c chapter7/errexit.c chapter16/TCPtecho.c -o techo

techotcp2:
	gcc chapter16/connectsock.c chapter16/connectTCP.c chapter7/errexit.c chapter16/TCPtecho2.c -o techo

techotcp3:
	gcc chapter16/connectsock.c chapter16/connectTCP.c chapter7/errexit.c chapter16/TCPtecho3.c -o techo

tcpxdr:
	gcc -I/usr/include/tirpc -I./chapter19 chapter19/xdr_common_endc.c chapter19/connectsock.c chapter19/connectTCP.c chapter7/errexit.c chapter19/TCP_xdr_client.c -ltirpc -o xclient
	
tcpxdrd:
	gcc -I/usr/include/tirpc -I./chapter19 chapter19/xdr_common_endc.c chapter19/passivesock.c chapter19/passiveTCP.c chapter7/errexit.c chapter19/TCP_xdr_server.c -ltirpc -o xserver

all: daytimetcp daytimedtcp timeudp timetcp timedudp echotcp echodtcp mechodtcp mdaytimedtcp mdaytimed msvcdudp superd inetd_daytimed inetd_echod techo xclient xserver

clean:
	rm -f a.out time timed daytimed daytime echo echod mechod mdaytimed exc12_3 daytimet daytimeu msvcd chargen superd inetd_daytimed inetd_echod techo xclient xserver
