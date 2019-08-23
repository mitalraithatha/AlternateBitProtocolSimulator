$(shell mkdir -p bin)
$(shell mkdir -p build)

CC=g++
CFLAGS=-std=c++17

INCLUDECADMIUM=-I lib\cadmium\include
INCLUDEDESTIMES= -I lib\DESTimes\include

all: build\main_receiver.o build\main_sender.o build\main_subnet.o build\main_top_model.o build\message.o build\format_convert.o
	$(CC) -g -o bin\RECEIVER_TEST build\main_receiver.o build\message.o build\format_convert.o
	$(CC) -g -o bin\SENDER_TEST build\main_sender.o build\message.o build\format_convert.o
	$(CC) -g -o bin\SUBNET_TEST build\main_subnet.o build\message.o build\format_convert.o
	$(CC) -g -o bin\ABP build\main_top_model.o build\message.o build\format_convert.o


build\main_receiver.o: test\src\receiver\main.cpp
	$(CC) -g -c $(CFLAGS) $(INCLUDECADMIUM) $(INCLUDEDESTIMES) test\src\receiver\main.cpp -o build\main_receiver.o

build\main_sender.o: test\src\sender\main.cpp
	$(CC) -g -c $(CFLAGS) $(INCLUDECADMIUM) $(INCLUDEDESTIMES) test\src\sender\main.cpp -o build\main_sender.o
	
build\main_subnet.o: test\src\subnet\main.cpp
	$(CC) -g -c $(CFLAGS) $(INCLUDECADMIUM) $(INCLUDEDESTIMES) test\src\subnet\main.cpp -o build\main_subnet.o
	
build\main_top_model.o: src\top_model\main.cpp
	$(CC) -g -c $(CFLAGS) $(INCLUDECADMIUM) $(INCLUDEDESTIMES) src\top_model\main.cpp -o build\main_top_model.o

build\format_convert.o:
	$(CC) -g -c $(CFLAGS) $(INCLUDECADMIUM) $(INCLUDEDESTIMES) src\top_model\format_convert.cpp -o build\format_convert.o

build\message.o:
	$(CC) -g -c $(CFLAGS) $(INCLUDECADMIUM) $(INCLUDEDESTIMES) src\data_structures\message.cpp -o build\message.o
clean:
	rm -rf build *.o *~
	rm -rf bin *.o *~


