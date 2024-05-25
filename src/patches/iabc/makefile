
IABC_DIR := ./src/iabc
MAIN_DIR := ./src/main

all:
	./wx_test.bash
	cd $(IABC_DIR);$(MAKE) -f makefile;cd ${HERE}
	cd $(MAIN_DIR);$(MAKE) -f makefile;cd ${HERE}

clean:
	cd $(IABC_DIR);$(MAKE) -f makefile clean;cd ${HERE}
	cd $(MAIN_DIR);$(MAKE) -f makefile clean;cd ${HERE}
