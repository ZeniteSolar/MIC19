
#cppcheck  --force --std=c99 --language=c src/
make clean
rm -rf bin obj
make all
#fuser -k /dev/ttyACM1
avrdude -c arduino -P /dev/ttyACM1 -p m328p -D -U flash:w:bin/firmware.elf
stty 57600 < /dev/ttyACM1 && cat /dev/ttyACM1  