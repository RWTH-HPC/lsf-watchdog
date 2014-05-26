CXX=g++

# change CFLAGS & LDFLAGS for your environment!
CFLAGS=-I/opt/lsf/9.1/include -Wall -g
LDFLAGS=-L/opt/lsf/9.1/linux2.6-glibc2.3-x86_64/lib -llsf -lbat -lnsl


# link binary
lsf_watchdog: lsf_watchdog.o check_scripts.o lsf_helper.o
	${CXX} -o $@ ${LDFLAGS} $+

# include header dependencys
-include *.d

# compile source
%.o: %.c
	$(CXX) -o $@ -c $(CFLAGS) $<
	$(CXX) -MM $(CFLAGS) $< > $*.d


# cleanup
clean:
	rm -f *.o *.d lsf_watchdog
.PHONY: clean
