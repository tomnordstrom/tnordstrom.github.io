

CC = /opt/cross-pi-gcc/bin/arm-linux-gnueabihf-gcc
#LDFLAGS = /home/paukim/raspi/rootfs/usr/lib/arm-linux-gnueabihf/libmodbus.so.5
LDFLAGS = 
BLDDIR = /
INCDIR = /home/paukim/raspi/rootfs/usr/include
#INCDIR = /home/paukim/raspi/u9u40
SRCDIR = ./
OBJDIR = ./
CFLAGS = -c -Wall -Wno-strict-prototypes -Wno-format -I$(INCDIR) -L/home/paukim/raspi/rootfs/usr/lib/
SRC = $(wildcard $(SRCDIR)/*.c)
OBJ = $(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(SRC))
EXE = $(OBJDIR)/simulator

all: clean $(EXE) 
    
$(EXE): $(OBJ) 
	$(CC) $(LDFLAGS) $(OBJDIR)/*.o -o $@ -lpthread

$(OBJDIR)/%.o : $(SRCDIR)/%.cpp
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $< -o $@

clean:
	-rm -f $(OBJDIR)/*.o $(EXE)
