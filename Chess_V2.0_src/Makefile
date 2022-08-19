#########################################################################
# 		HeroChess V2.0 by The Avengineers			#
# 		EECS 22L - University of California, Irvine		#
#									#
# Makefile: contains all commands needed for compilation		#
# 	and creating necessary executables for the game, HeroChess	#
#									#
# Modifications:							#
# Date (YYYY/MM/DD)	Author			Description		#
# 2021/06/06		Rachel Villamor		Final Version		#
# 2021/04/19		Rachel Villamor		Initial Version		#
#########################################################################

#libs

# gcc and tags
CC     = gcc
DEBUG  = -DDEBUG 
CFLAGS = -ansi -std=c99 -Wall -c
LFLAGS = -ansi -std=c99 -Wall

# executables and replay files
DESIGN	= bin/HeroChessV2_Server bin/HeroChessV2_Client
TEST	= bin/TestHeroChessV2
SRC	= src/*.o

all: TestHeroChessV2 HeroChessV2_Server HeroChessV2_Client 

########### generate object files ###########

#target to generate settings.o
settings.o: src/settings.h src/settings.c
	$(CC) $(CFLAGS) src/settings.c -o src/settings.o

#target to generate tree.o
tree.o: src/move.h src/tree.h src/tree.c
	$(CC) $(CFLAGS) src/tree.c -o src/tree.o

#target to generate piece.o
piece.o: src/piece.h src/piece.c
	$(CC) $(CFLAGS) src/piece.c -o src/piece.o 

#target to generate move.o
move.o: src/move.h src/move.c
	$(CC) $(CFLAGS) src/move.c -o src/move.o 

#target to generate game.o
game.o: src/settings.h src/piece.h src/movelist.h src/game.h src/game.c s
	$(CC) $(CFLAGS) src/game.c -o src/game.o

#target to generate board.o
board.o: src/settings.h src/piece.h src/board.h src/board.c 
	$(CC) $(CFLAGS) src/board.c -o src/board.o

#target to generate ai.o
ai.o:	src/tree.h src/movelist.h src/game.h src/settings.h src/piece.h src/ai.h src/ai.c
	$(CC) $(CFLAGS) src/ai.c -o src/ai.o

#target to generate movelist.o
movelist.o: src/piece.h src/move.h src/movelist.h src/movelist.c
	$(CC) $(CFLAGS) src/movelist.c -o src/movelist.o

#target to generate replay.o
replay.o: src/board.h src/piece.h src/move.h src/game.h src/board.h src/movelist.h 
	$(CC) $(CFLAGS) src/replay.c -o src/replay.o

#target to generate main.o
main.o: src/main.c src/settings.h src/piece.h src/move.h src/game.h src/board.h src/ai.h src/movelist.h src/replay.h
	$(CC) $(CFLAGS) src/main.c -o src/main.o

#target to generate server.o
server.o: src/server.c src/piece.h src/game.h src/board.h src/movelist.h src/settings.h src/replay.h src/ai.h src/move.h src/tree.h
	$(CC) $(CFLAGS) -DDEBUG src/server.c -o src/server.o


#target to generate client.o
client.o: src/client.c src/game.h src/board.h src/piece.h src/movelist.h src.move.h
	$(CC) $(CFLAGS) -DDEBUG src/client.c -o src/client.o

#target to generate testmain.o
src/testmain.o: src/main.c src/settings.h src/piece.h src/move.h src/game.h src/board.h src/ai.h src/movelist.h src/replay.h
	gcc -c -DDEBUG -Wall -std=c99 src/main.c -o src/testmain.o


############# generate executables #############
	
# target to generate HeroChessV2 with test mode on
TestHeroChessV2: src/testmain.o src/tree.o src/settings.o src/piece.o src/move.o src/game.o src/board.o src/ai.o src/movelist.o src/replay.o
	$(CC) $(LFLAGS) src/testmain.o src/tree.o src/settings.o src/piece.o src/move.o src/game.o src/board.o src/ai.o src/movelist.o src/replay.o -o bin/TestHeroChessV2 -lm

#target to generate Server
HeroChessV2_Server:	src/server.o src/settings.o src/game.o src/board.o src/piece.o src/movelist.o src/replay.o src/ai.o src/move.o src/tree.o
	$(CC) $(LFLAGS) -DDEBUG src/tree.o src/server.o src/piece.o src/game.o src/board.o src/movelist.o src/settings.o src/replay.o src/ai.o src/move.o -o bin/HeroChessV2_Server -lm

#target to generat Client
HeroChessV2_Client: src/client.o src/game.o src/board.o src/piece.o src/movelist.o src/move.o
	$(CC) $(LFLAGS) -DDEBUG src/client.o src/game.o src/board.o src/piece.o src/movelist.o src/move.o -o bin/HeroChessV2_Client -lm

# delete all executables currently in the source and binary directories
clean:
	rm -f $(DESIGN) $(TEST) $(SRC)

# test game
test: 
	@echo "To run the testing, first start the server in one terminal,"
	@echo "then start the client in another."
	@echo
	@echo "For example:"
	@echo "zuma# ./HeroChessV2_Server 11919"
	@echo "zuma# ./HeroChessV2_Client 11919"
	@echo
	@echo "Different servers may be used as well:"
	@echo "crystalcove# ./HeroChessV2_Server 11919"

# tar the source code package
tar:
	tar zcvf Chess_V2.0_src.tar.gz Chess_V2.0_src   
