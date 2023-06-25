# compile command
arm-none-linux-gnueabi-gcc -static -o {app} {app.c}

# push to board
adb push {app} /data/local/tmp