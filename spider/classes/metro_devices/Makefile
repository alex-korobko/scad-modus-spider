include indPlatform

.PHONY: all clean test untest macos

all:
	for i in ${DIRS} ; do (cd $$i && $(MAKE) all) ; done

macos:
	@echo "Building for macOS with gcc..."
	@echo 'DIRS = gcc_macos' > indPlatform_macos
	@echo 'ALLDIRS = gcc_macos' >> indPlatform_macos
	@echo 'MYLPATH=-L. -L/usr/local/lib/ -L/opt/homebrew/lib/ -L/usr/lib/' > indLpath_macos
	@echo 'MYLIBS= -lm -lpthread' > indLfiles_macos
	@echo 'ifdef UNIT_TESTING' >> indLfiles_macos
	@echo 'MYLIBS+= -lcppunit' >> indLfiles_macos
	@echo 'endif' >> indLfiles_macos
	@echo 'CXX = g++' > indCompiler_macos
	@echo 'CXXFLAGS = -std=c++11 -fexceptions -pthread $(DEBUG) $(OPTIM) $(MYIPATH) $(MYLPATH)' >> indCompiler_macos
	cd gcc_macos && $(MAKE) -f ../gcc_macos/Makefile all

clean:
	for i in ${ALLDIRS} ; do (cd $$i && $(MAKE) clean) ; done

test: 
	echo  '//Don`t edit that file! It was created automatically\n#define __UNIT_TESTING__' > test_cond.h
	echo  '#Don`t edit that file! It was created automatically\nUNIT_TESTING = 1\n' > indTestMode

test-macos:
	@echo "Setting up unit tests for macOS..."
	echo  '//Don`t edit that file! It was created automatically\n#define __UNIT_TESTING__' > test_cond.h
	echo  '#Don`t edit that file! It was created automatically\nUNIT_TESTING = 1\n' > indTestMode
	$(MAKE) macos

untest: 
	echo  '//Don`t edit that file! It was created automatically' > test_cond.h
	echo  '#Don`t edit that file! It was created automatically\n' > indTestMode
