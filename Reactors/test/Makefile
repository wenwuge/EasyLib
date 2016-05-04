TARGET=test
LIBS=-lpthread   
INCLUDE=-I./ -I../base/ -I../net/
CFLAGS=-g -D MUDUO_STD_STRING 

TARGET :
	#g++ -c test.cc -o test.o $(LIBS) $(INCLUDE) $(CFLAGS) 
	#g++ -o test test.o -L../base -lbase -L../net -ltcpserver   -I../net -I../base -DMUDUO_STD_STRING -lpthread
	g++ -o server test.cc    ../net/libnet.a ../base/libbase.a  -I../base -I../net -lpthread -D MUDUO_STD_STRING
