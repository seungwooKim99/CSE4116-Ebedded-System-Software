default:
	arm-none-linux-gnueabi-gcc -static -o app app.c
push:
	adb push app /data/local/tmp
clean:
	-rm *.o