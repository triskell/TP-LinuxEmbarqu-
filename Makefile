all:
	/opt/arm-unknown-linux-gnueabi/bin/arm-unknown-linux-gnueabi-g++ -std=c++11 main.cpp -o pwm
	cp pwm /tmp/beagle

mount: 
	mkdir /tmp/beagle
	sshfs root@172.18.2.49:/root /tmp/beagle
