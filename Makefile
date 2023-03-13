CCPPC:=gcc
CCPP:=g++

CXXFLAGSC = $(shell pkg-config --cflags libcbor)
LDFLAGSC = $(shell pkg-config --libs libcbor)



CXXFLAGS = $(shell pkg-config --cflags cairo gtk+-3.0 librsvg-2.0 tinyxml2 libcbor)
LDFLAGS = $(shell pkg-config --libs cairo gtk+-3.0 librsvg-2.0 tinyxml2 libcbor)

svgclient : client_sin_wave
client_sin_wave: client_sin_wave.o
	$(CCPPC) $(CXXFLAGSC) -o $@ $< $(LDFLAGSC) -lm
%.o : %.cpp
	$(CCPPC) $(CXXFLAGSC) -o $@ -c $<
clean:
	


svgserver : Programme_de_depart
Programme_de_depart : Programme_de_depart.o
	$(CCPP) $(CXXFLAGS) -o $@ $< $(LDFLAGS)
%.o : %.cpp
	$(CCPP) $(CXXFLAGS) -o $@ -c $<
clean:
	rm -f *.o 
fclean : clean
	rm -f Programme_de_depart client_sin_wave
