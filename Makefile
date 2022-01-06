OBJS	= src/client.cpp.o \
	  src/server.cpp.o \
	  src/cs_base.cpp.o \
	  src/message.cpp.o \
	  src/service.cpp.o \

SHARED  = SimpleNet.so
WIN_SHARED = SimpleNet.dll
LIB     = SimpleNet.a
CXX	= g++
WIN_CXX = x86_64-w64-mingw32-g++
CC      = gcc
WIN_CC  = x86_64-w64-mingw32-gcc
BUILD_CXX_FLAGS	 = -Wall -std=c++20 -fPIC -Iinclude
WIN_BUILD_CXX_FLAGS = -Wall -std=c++20 -fPIC -Iinclude
BULID_CC_FLAGS   = -fPIC
WIN_BUILD_CC_FLAGS = -fPIC
LINK_OPTS	 = -lpthread -lSDL2 -lSDL2_net
WIN_LINK_OPTS = -lpthread -ldl -LLibraries/Win/lib -lopengl32 -l:OpenGL32.Lib -w -Wl,-subsystem,windows -lmingw32  -lSDL2main -lSDL2 -lSDL2.dll -lSDL2_net -lfreetype -lbz2 -lzip -static -static-libgcc -static-libstdc++ -lhid -ldinput8 -ldxguid -ldxerr8 -luser32 -lgdi32 -lwinmm -limm32 -lole32 -loleaut32 -lshell32 -lsetupapi -lversion -luuid -lglu32 -lopengl32 -fno-stack-protector -fstack-protector #kinda just threw a whole bunch here and hoped that it would be good enough
OS     = Linux

all: $(OBJS)
ifeq ($(OS), Linux)
	$(AR) rvs $(LIB) $(OBJS)
	$(CXX) -shared -g -o $(SHARED) $(OBJS) $(BUILD_CXX_FLAGS) $(LINK_OPTS)
else
	$(AR) rvs $(LIB) $(OBJS)
	$(WIN_CXX) -shared -g -o $(WIN_SHARED) $(OBJS) $(WIN_BUILD_CXX_FLAGS) $(WIN_LINK_OPTS)
endif


%.cpp.o: %.cpp
ifeq ($(OS), Linux)
	$(CXX) $< $(BUILD_CXX_FLAGS) $(LINK_OPTS) -g -c -o $@
else
	$(WIN_CXX) $< $(WIN_BUILD_CXX_FLAGS) $(WIN_LINK_OPTS) -g -c -o $@
endif

%.c.o: %.c
ifeq ($(OS), Linux)
	$(CXX) $< $(BUILD_CXX_FLAGS) $(LINK_OPTS) -g -c -o $@
else
	$(WIN_CXX) $< $(WIN_BUILD_CXX_FLAGS) $(WIN_LINK_OPTS) -g -c -o $@
endif

clean:
	rm -f $(OBJS) $(LIB) $(SHARED)
	rm -f $(WIN_SHARED)

mingw:
	$(MAKE) OS=Win

mingwj:
	$(MAKE) OS=Win -j

