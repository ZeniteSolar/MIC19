cd lib/avr-can-lib/src/
make clean
make lib
cd ../../../
wget https://raw.githubusercontent.com/ZeniteSolar/CAN_IDS/new_ids/can_ids.h -O src/can_ids.h
