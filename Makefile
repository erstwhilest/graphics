CXX = g++
EXE = main
INCLUDE = -Iglad\include -Iglfw\include -Iglm\include -Iimgui -Istb_image\include
LINK = -Lglad -Lglfw\lib-mingw-w64 -lglad -lglfw3 -lgdi32 -lopengl32 -limm32
OPTIONS = -Wall -g
TARGETS = $(subst src/, , $(wildcard src/*.cpp))
TARGETS += $(subst imgui/, , $(wildcard imgui/*.cpp))

all: clean $(TARGETS:.cpp=.o)
	$(CXX) $(TARGETS:.cpp=.o) -o $(EXE) $(LINK) $(OPTIONS)
	./main

%.o: src/%.cpp
	$(CXX) -c $< -o $@ $(INCLUDE) $(OPTIONS)

%.o: imgui/%.cpp
	$(CXX) -c $< -o $@ $(INCLUDE) $(OPTIONS)

clean:
	rm -f *.exe main.o