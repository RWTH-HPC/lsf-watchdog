CXX=g++

CFLAGS=-I/opt/lsf/9.1/include -Wall -g
LDFLAGS=-L/opt/lsf/9.1/linux2.6-glibc2.3-x86_64/lib -llsf -lbat -lnsl


%.o: %.c
	${CXX} -o $@ -c ${CFLAGS} $+


lsf_watchdog: lsf_watchdog.o config.o
	${CXX} -o $@ ${LDFLAGS} $+


clean:
	rm -f *.o lsf_efficiency
.PHONY: clean
