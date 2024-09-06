OBJS=kissframe.o kissencap.o slipframe.o slipencap.o
PROGS=kissencap slipencap

.SUFFIXES: .c .o

.c.o:
	$(CC) -c $< -o $@

$(PROGS): $(OBJS)
	cc -o kissencap kissframe.o kissencap.o
	cc -o slipencap slipframe.o slipencap.o

.PHONY: clean

clean:
	rm -f $(OBJS) $(PROGS)
