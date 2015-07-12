CC         = gcc
CFLAGS     = -std=c11 -g
LIBS       = -lSDL2
OBJS       = main.o loader.o util/common.o
EXECUTABLE = ccraft

SRCS       = $(OBJS:.o=.c)


all: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(LIBS) -o $(EXECUTABLE)

depend: .depend

.depend: $(SRCS)
	rm -f ./.depend
	$(CC) $(CFLAGS) -MM $^ > ./.depend;

include .depend

clean:
	find . -name '*.o' -type f -delete
	rm ccraft
