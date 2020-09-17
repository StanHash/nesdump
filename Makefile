
CXXFLAGS = -Wall -Wextra -Werror -pedantic -std=c++17 -O3

HEADERS := \
  nes.hh

SOURCES := \
  nesdump.cc \
  nes.cc

nesdump: $(SOURCES) $(HEADERS)
	$(CXX) $(CXXFLAGS) $(SOURCES) -o $@

clean:
	rm -f nesdump

.PHONY: clean
