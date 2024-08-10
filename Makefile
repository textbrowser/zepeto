CXX_OPTIONS = -D_FORTIFY_SOURCE=2 \
	      -Wall \
	      -Wconversion \
	      -Wdouble-promotion \
	      -Werror \
	      -Wextra \
	      -Wformat=2 \
	      -Wold-style-cast \
	      -Wpointer-arith \
	      -Wshadow \
	      -Wsign-conversion \
	      -Wstack-protector \
	      -Wstrict-overflow=5 \
	      -Wzero-as-null-pointer-constant \
	      -fPIE \
	      -fstack-protector-all \
	      -pedantic \
	      -std=c++17

SOURCE	= zepeto.cc \
	  zepeto_main.cc

all: $(SOURCE)
	$(CXX) $(CXX_OPTIONS) -o zepeto.bin $(SOURCE)

clean:
	rm -f core zepeto.bin zepeto.core

distclean: clean purge

purge:
	rm -f *~
