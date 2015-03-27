CXX_OPTIONS = -Wall -Wconversion -Werror -Wextra -Wformat=2 \
	      -Wpointer-arith -Wsign-conversion \
	      -Wstack-protector -Wstrict-overflow=5 \
	      -fPIE -fstack-protector-all -pedantic

SOURCE	= zepeto.cc \
	  zepeto_main.cc

all: $(SOURCE)
	$(CXX) $(CXX_OPTIONS) -o zepeto.bin $(SOURCE)

clean:
	rm -f core zepeto.bin zepeto.core

distclean: clean purge

purge:
	rm -f *~
