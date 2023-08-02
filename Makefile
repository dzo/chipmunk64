all:
	$(MAKE) -C psys/src LINUX=1
	$(MAKE) -C log/src LINUX=1
wasm:
	$(MAKE) -C psys/src WASM=1
	$(MAKE) -C log/src diglog.html WASM=1
clean:
	$(MAKE) -C psys/src clean
	$(MAKE) -C log/src clean
windows:
	$(MAKE) -C psys/src WIN32=1
	$(MAKE) -C log/src WIN32=1
upload:
	scp log/src/diglog.html log/src/diglog.js log/src/diglog.wasm mail.marginz.co.nz:html/log/
