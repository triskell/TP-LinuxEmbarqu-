all:
	/opt/arm-unknown-linux-gnueabi/bin/arm-unknown-linux-gnueabi-g++ -std=c++11 main.cpp -o pwm -lpthread


mount: 
	mkdir /tmp/beagle
	sshfs root@172.17.0.0:/root /tmp/beagle

send:
	cp pwm /tmp/beagle