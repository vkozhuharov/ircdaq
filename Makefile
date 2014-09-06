#
# Makefile for compiling GANDALF DAQ
#
# Get OS type from uname shell command
SHELL   = /bin/sh
OSTYPE = $(shell uname)
HOSTNAME = $(shell hostname)




#Source directories
TOPDIR  = .
INCDIR  = $(TOPDIR)/inc
SRCDIR  = $(TOPDIR)/src
OBJDIR  = $(TOPDIR)/obj
DEPDIR  = $(TOPDIR)/dep

LIBS     = $(SYSLIBS)  -L$(DIMDIR)/linux -ldim -lpthread  -L -lconthost
#LIBS     = $(SYSLIBS) -lusb-1.0 -L$(DIMDIR)/linux -ldim -lpthread  -L -lconthost

#-lconthost

DEFINES  = -DUSE_DIM -I$(DIMDIR)/dim

CPPFLAGS = -I. -I$(INCDIR) $(DEFINES)
CFLAGS = 

CC     = gcc
CPP = gcc -E


# Define search paths for different type of files
vpath %.c $(SRCDIR)
vpath %.h $(INCDIR)
vpath %.o $(OBJDIR)
vpath %.d $(DEPDIR)

SRC = daq.c\
      getData.c \
      processData.c \
      sendData.c \
      runcontrol.c \
      daqControl.c \
      net.c

#OBJ = $(SRC:$(SRCDIR)/%.c=$(OBJDIR)/%.o)
OBJ = $(SRC:.c=.o)
DEPENDS  = $(SRC:%.c=$(DEPDIR)/%.d)

#DEPENDS  = $(SRC:$(SRCDIR)/%.c=$(DEPDIR)/%.d)
all:
	gmake daq

default:
	gmake daq

daq: $(OBJ)
	mkdir -p data
	$(CC) $(CFLAGS) -o daq $(OBJ:%=$(OBJDIR)/%) $(LIBS) 


clean:
	$(RM) $(OBJDIR)/*.o
	$(RM) $(DEPDIR)/*.d
	$(RM) daq

include $(DEPENDS) 

$(DEPDIR)/%.d: %.c
	mkdir -p $(DEPDIR)
	$(SHELL) -ec '$(CC) -MM $(CPPFLAGS) $< \
              | sed s/$*.o/$*.o\ $*.d/ > $@'

$(SRC):

$(OBJ):
	mkdir -p $(OBJDIR)
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $(OBJDIR)/$(*F).o



FORCE:





