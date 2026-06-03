MY_CFLAGS = $(filter-out -g0, $(CFLAGS))
MY_CFLAGS = $(filter-out -O2, $(CFLAGS))
MY_CFLAGS += -Wall -Wextra -g -no-pie -O0

all: ../shared/pka-app ../shared/pka-test

../shared/pka-app: pka-app.c
	$(CC) -o $@ $(MY_CFLAGS) $(LDFLAGS) $<

../shared/pka-test:
	ln pka-test $@

clean:
	rm -f ../shared/pka-app ../shared/pka-test

.PHONY: all clean
