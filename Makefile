CC = g++
CFLAGS = -Wall -Werror -std=c++17
LIBS = -lcurl

TARGET = auto-toggle-vpn

# 检查处理器架构
ifeq ($(shell uname -p), arm)
	CFLAGS += -I/opt/homebrew/Cellar/nlohmann-json/3.11.2/include/
else
	CFLAGS += -I/usr/local/Cellar/nlohmann-json/3.11.2/include/
endif

.PHONY: all clean

all: $(TARGET)

$(TARGET): auto-toggle-vpn.cpp
	$(CC) $(CFLAGS) $< -o $@ $(LIBS)

clean:
	rm -f $(TARGET)
