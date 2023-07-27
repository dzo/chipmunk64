all:
	$(MAKE) -C psys/src
	$(MAKE) -C log/src diglog.html
clean:
	$(MAKE) -C psys/src clean
	$(MAKE) -C log/src clean
