# Makefile.inc
#



# make options
MAKEFLAGS+= -s

BUILD_PATH=build



all: debug

style:
	command -v astyle >/dev/null 2>&1 && astyle --style=ansi -r -n "./*.h" "./*.cpp" || echo "Warning: can't find astyle executable!";

debug:		style
	mkdir -p $(BUILD_PATH)/debug;
	cd $(BUILD_PATH)/debug; cmake ../.. -DCMAKE_BUILD_TYPE=debug;
	make -C $(BUILD_PATH)/debug;

release:	style
	mkdir -p $(BUILD_PATH)/release;
	cd $(BUILD_PATH)/release; cmake ../.. -DCMAKE_BUILD_TYPE=release;
	make -C $(BUILD_PATH)/release;

install:	release
	make -C $(BUILD_PATH)/release $(MAKECMDGOALS);

install_debug:	debug
	make -C $(BUILD_PATH)/debug install;

clean:
	test -d $(BUILD_PATH)/release && make -C $(BUILD_PATH)/release $(MAKECMDGOALS) || true;
	test -d $(BUILD_PATH)/debug && make -C $(BUILD_PATH)/debug $(MAKECMDGOALS) || true;
	test -d install/ && rm -f install/* || true;

purge:
	test -d $(BUILD_PATH) && rm -rf ./$(BUILD_PATH) || true;
	test -d install && rm -rf install || true;



# End
