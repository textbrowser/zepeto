CXX_OPTIONS = -Wall \
	      -Wconversion \
	      -Wdouble-promotion \
	      -Werror \
	      -Wextra \
	      -Wformat=2 \
	      -Wpointer-arith \
	      -Wshadow \
	      -Wsign-conversion \
	      -Wstack-protector \
	      -Wstrict-overflow=5 \
	      -fPIE \
	      -fstack-protector-all \
	      -pedantic \
	      -std=c++11

SOURCE	= zepeto.cc \
	  zepeto_main.cc

all: $(SOURCE)
	$(CXX) $(CXX_OPTIONS) -o zepeto.bin $(SOURCE)

clean:
	rm -f core zepeto.bin zepeto.core

distclean: clean purge

purge:
	rm -f *~
