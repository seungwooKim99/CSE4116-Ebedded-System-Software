default: main.o io.o device.o ipc.o merge.o
	arm-none-linux-gnueabi-gcc -static -o 20181610_asg1.out main.o device.o io.o ipc.o merge.o -lpthread

main.o: default.h main.h main.c
	arm-none-linux-gnueabi-gcc -static -c -o main.o main.c -lpthread

io.o: default.h ./io/io.h ./io/io.c
	arm-none-linux-gnueabi-gcc -static -c -o io.o ./io/io.c -lpthread

device.o: default.h ./device/device.h ./device/device.c
	arm-none-linux-gnueabi-gcc -static -c -o device.o ./device/device.c -lpthread

ipc.o: default.h ./ipc/ipc.h ./ipc/ipc.c
	arm-none-linux-gnueabi-gcc -static -c -o ipc.o ./ipc/ipc.c -lpthread

merge.o: default.h ./merge/merge.h ./merge/merge.c
	arm-none-linux-gnueabi-gcc -static -c -o merge.o ./merge/merge.c -lpthread

push:
	adb push 20181610_asg1.out /data/local/tmp

clean:
	-rm *.o
	-rm *.out