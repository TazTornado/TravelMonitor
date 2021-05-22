#####################################################################################
# Compilation Options:																#
# 	  Command					Action												#
# ------------------------------------------------------------------------- 		#
# >> make all	  			=> all source files and creates all executables			#
# >> make travelMonitor 	=> travelMonitor and its dependencies					#
# >> make Monitor			=> Monitor and its dependencies							#											#
# >> make clean				=> removes all object and all executable files			#
#####################################################################################

IDIR = ../include
ODIR = ../obj
BDIR = ../bin
CC = gcc
CFLAGS = -ggdb -Wall -I$(IDIR)


_DEPS = datatypes.h bloom_filter.h skip_list.h travelMon_utilities.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

$(ODIR)/%.o: %.c $(DEPS)
	@$(CC) -c -o $@ $< $(CFLAGS)

all: Monitor travelMonitor

Monitor: $(ODIR)/Monitor.o $(ODIR)/bloom_filter.o $(ODIR)/skip_list.o #$(ODIR)/monitor_utilities.o
	@echo creating Monitor..
	@$(CC) -o $(BDIR)/$@ $^ $(CFLAGS)

travelMonitor: $(ODIR)/travelMonitor.o $(ODIR)/bloom_filter.o $(ODIR)/skip_list.o $(ODIR)/travelMon_utilities.o
	@echo creating travelMonitor..
	@$(CC) -o $(BDIR)/$@ $^ $(CFLAGS)

.PHONY: clean

clean:
	@echo Removing object files and executable files
	@rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~ $(BDIR)/*
	@echo Done!
