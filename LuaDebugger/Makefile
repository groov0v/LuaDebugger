CC=g++
CFLAGS=-g -llua -lpthread
SRC=LuaDebugger.cpp main.cpp
TARGET=LuaDebugger
TARGETDIR=../Debug

all : $(TARGET)

$(TARGET) : $(SRC)
	$(CC) -o $(TARGETDIR)/$(TARGET) $(SRC) $(CFLAGS)

clean : 
	rm -rf $(TARGETDIR)/$(TARGET)
