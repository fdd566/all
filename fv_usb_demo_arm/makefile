CC= arm-linux-gcc

FLAGS = -Wall -g -o

INC=-I/mnt/hgfs/share/test/work/resource/curl/include
LDFLAGS=-L/mnt/hgfs/share/test/work/resource/curl/lib

SOURCE_FILE = ./*.c*

./usbTest: $(SOURCE_FILE)
	$(CC) $(FLAGS) $@ $(SOURCE_FILE) $(INC) $(LDFLAGS) -lrt -ldl -lpthread -lcurl

clean:
	rm -rf ./usbTest

.PHONY: clean
