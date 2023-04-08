CCPPC:=gcc
CCPP:=g++

CXXFLAGSC = $(shell pkg-config --cflags libcbor)
LDFLAGSC = $(shell pkg-config --libs libcbor)

CXXFLAGS = $(shell pkg-config --cflags cairo gtk+-3.0 librsvg-2.0 tinyxml2 libcbor) -w
LDFLAGS = $(shell pkg-config --libs cairo gtk+-3.0 librsvg-2.0 tinyxml2 libcbor) -w

svgclient : svgclient
svgclient: svgclient.o
	$(CCPPC) $(CXXFLAGSC) -o $@ $< $(LDFLAGSC) -lm
%.o : %.cpp
	$(CCPPC) $(CXXFLAGSC) -o $@ -c $<
	
svgserver : svgserver
svgserver : svgserver.o func.o func.h
	$(CCPP) -g $(CXXFLAGS) -o $@ $< func.o $(LDFLAGS)
	
func: func.o func.h
	$(CCPP) $(CXXFLAGS) -c $< -o $@
%.o : %.cpp
	$(CCPP) -g $(CXXFLAGS) -o $@ -c $<

clean:
	rm -f *.o 
fclean : clean
	rm -f svgclient svgserver
