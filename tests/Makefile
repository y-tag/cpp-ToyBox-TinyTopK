SRCTOP = ..

include $(SRCTOP)/Makefile.defs

CXXSRC = cpp_unit_test_runner.cc \
         posting_list_test.cc \
         tiny_scorer_test.cc \

INCLUDES += -I../lib \

LIBS += -L../lib \
        -Wl,-rpath,../lib \
        -ltinytopk \
        -lcppunit \

include $(SRCTOP)/Makefile.rules

test:$(CXXOBJ)
	$(CXX) -Wall $^ -o CppUnitTestRunner $(INCLUDES) $(LIBS) $(CXXFLAG)
	./CppUnitTestRunner

clean:
	rm -f *~ *.o *.out
