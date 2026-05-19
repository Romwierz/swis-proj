all: ../shared/pka-app ../shared/pka-test

../shared/pka-app: pka-app.c
	$(CC) -o $@ $(CFLAGS) $(LDFLAGS) $<

../shared/pka-test:
	ln pka-test $@

clean:
	rm -f ../shared/pka-app ../shared/pka-test

.PHONY: all clean
