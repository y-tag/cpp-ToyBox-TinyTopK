SRCTOP = ..

include $(SRCTOP)/Makefile.defs

CXXSRC = posting_list_container.cc \
         posting_list.cc \
         tiny_taat_scorer.cc \
         tiny_daat_scorer.cc \
         tiny_wand_scorer.cc \

CXXFLAG += -fPIC -Wall

include $(SRCTOP)/Makefile.rules

lib:$(CXXOBJ)
	$(CXX) -shared -o libtinytopk.so $^ $(CXXFLAG)

clean:
	rm -f *~ *.o *.out *.so
