# minimalist makefile
.SUFFIXES:
#
.SUFFIXES: .cpp .o .c .h
CXXFLAGS = -std=c++1z -fPIC -I../../dependencies/dimsum  -I../../dependencies/libsimdpp  -O3  -Wall -Wextra -Wshadow

UNAMEM := $(shell uname -m)
ifneq ($(UNAMEM),aarch64)
  CXXFLAGS += -march=native
endif
HEADERS := $(shell ls *.h)

EXE :=  bench

all:  $(EXE)

display:
	echo $(UNAMEM)


bench: $(HEADERS) bench.cpp
	$(CXX) $(CXXFLAGS) -o bench bench.cpp





clean:
	rm -f $(EXE)
