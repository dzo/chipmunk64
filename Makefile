all:
	$(MAKE) -C psys/src
	$(MAKE) -C log/src
clean:
	$(MAKE) -C psys/src clean
	$(MAKE) -C log/src clean
