CC=arm-linux-gcc
light: light_sensor.o luxcalc.o
	$(CC) light_sensor.o luxcalc.o -s -o light_sensor
light.o: light_sensor.c
	$(CC) -c light_sensor.c
luxcalc.o: luxcalc.c
	$(CC) -c luxcalc.c -o luxcalc.o
clean:
	rm -rf *.o light_sensor
