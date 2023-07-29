all:
	$(MAKE) -C psys/src
	$(MAKE) -C log/src
#diglog.html
clean:
	$(MAKE) -C psys/src clean
	$(MAKE) -C log/src clean
upload:
	scp log/src/diglog.html log/src/diglog.js log/src/diglog.wasm mail.marginz.co.nz:html/log/
