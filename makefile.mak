COMPILER = g++
CC_V = -std=c++11
CCFLAGS = -o

lexico: LexicalAnalyzer.cpp
	${COMPILER} ${CC_V} LexicalAnalyzer.cpp ${CCFLAGS} Lexico 

clean:
	$(RM) Lexico