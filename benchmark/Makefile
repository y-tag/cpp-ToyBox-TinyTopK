SRCTOP = ..

include $(SRCTOP)/Makefile.defs

CXXSRC = bench_tinytopk.cc \
         posting_list_container.cc \
         posting_list.cc \
         tiny_taat_scorer.cc \
         tiny_daat_scorer.cc \
         tiny_wand_scorer.cc \

INCLUDES += -I../lib \

LIBS += -L../lib \

include $(SRCTOP)/Makefile.rules

bench_tinytopk:$(CXXOBJ)
	$(CXX) $^ -o $@ $(INCLUDES) $(LIBS) $(CXXFLAG)

clean:
	rm -f *~ *.o *.out
