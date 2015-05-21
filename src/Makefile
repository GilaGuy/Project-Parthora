CXX=g++
CPPFLAGS=-std=c++11 -g
CLIENT_EXE=ProjectParthora
SERVER_EXE=ProjectParthoraServer

## FILES

FILES_COMMON=	GameSettings.o \
				net/Packet.o				

FILES_CLIENT=	core/object/BGO.o core/object/SGO.o core/object/TGO.o \
				core/Renderer.o \
				effect/ParticleSystem.o \
				engine/AppWindow.o engine/Scene.o \
				graphics/Fireball.o \
				net/client/Connection.o \
				scenes/ParticleSystemPlayground.o \
				Game.o

FILES_SERVER=	net/server/Server.o \
				Game-Server.o

## Targets

all: server client

client: $(FILES_COMMON) $(FILES_CLIENT)
	$(CXX) $(CPPFLAGS) \
	$(FILES_COMMON) $(FILES_CLIENT) \
	-o $(CLIENT_EXE) -lpthread -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio -lsfml-network

server: $(FILES_COMMON) $(FILES_SERVER)
	$(CXX) $(CPPFLAGS) \
	$(FILES_COMMON) $(FILES_SERVER) \
	-o $(SERVER_EXE) -lpthread -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio -lsfml-network

%.o: %.cpp
	$(CXX) $(CPPFLAGS) -c $< -o $@

clean:
	find . -name "*.o" -type f -delete
	rm -f $(CLIENT_EXE) $(SERVER_EXE)
