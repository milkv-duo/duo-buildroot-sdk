CXX ?= g++
CXXFLAGS += "--std=c++11"
NANDBOOT_V2 ?= 0

ifeq ($(NANDBOOT_V2), 1)
	CXXFLAGS += "-DNANDBOOT_V2"
endif

all:create_sv

create_sv:create_sv.cpp create_sv.h crc16.cpp crc.h
	$(CXX) create_sv.cpp  crc16.cpp -o create_sv $(CXXFLAGS)

run:create_sv
	./create_sv

clean:
	rm -rf create_sv
