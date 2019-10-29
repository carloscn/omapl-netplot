QMAKE = /opt/qt-arm-4.8.3/bin/qmake

all:
	make -C dsp all
	cd host && ${QMAKE} ad7606.pro && make

clean:
	make -C dsp clean
	cd host && ${QMAKE} ad7606.pro && make clean && rm Makefile && rm ~Makefile
