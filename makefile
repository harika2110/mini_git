TARGET = mygit 
LDLIBS = -lcrypto

all: $(TARGET)

$(TARGET): $(SOURCES)
	g++ main.cpp -o $(TARGET) $(LDLIBS)

clean: 
	rm -f $(TARGET)

.PHONY: all clean