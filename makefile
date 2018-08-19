all: pc send

pc:
	gcc pc.c parson.c embedded_linux.c -c -lpaho-mqtt3c; gcc pc.o parson.o embedded_linux.o -lpaho-mqtt3c -o pc.bin; ls pc.bin

bbb: 
	gcc bbb.c parson.c embedded_linux.c -c -lpaho-mqtt3c; gcc bbb.o parson.o embedded_linux.o -lpaho-mqtt3c -o bbb.bin; ls bbb.bin

send:
	scp -r ../controle_salas root@192.168.7.2:~/

folder: 
	mkdir -p obj APP

clean:
	rm *.o pc.bin bbb.bin