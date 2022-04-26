assembler: assembler.o globalFunctions.o macroDeploy.o firstPass.o analizeWord.o secondPass.o writeFiles.o 
	gcc -Wall -ansi -pedantic -g assembler.o globalFunctions.o macroDeploy.o firstPass.o analizeWord.o secondPass.o writeFiles.o -o assembler 
assembler.o: assembler.c globalFunctions.c firstPass.c secondPass.c writeFiles.c global.h macroDeploy.h
	gcc -c -Wall -ansi -pedantic assembler.c -o assembler.o
globalFunctions.o: globalFunctions.c globalFunctions.h global.h
	gcc -c -Wall -ansi -pedantic globalFunctions.c -o globalFunctions.o
macroDeploy.o: macroDeploy.c macroDeploy.h global.h
	gcc -c -Wall -ansi -pedantic macroDeploy.c -o macroDeploy.o
firstPass.o: firstPass.c firstPass.h global.h
	gcc -c -Wall -ansi -pedantic firstPass.c -o firstPass.o
analizeWord.o: analizeWord.c analizeWord.h global.h
	gcc -c -Wall -ansi -pedantic analizeWord.c -o analizeWord.o
secondPass.o: secondPass.c secondPass.h global.h
	gcc -c -Wall -ansi -pedantic secondPass.c -o secondPass.o
writeFiles.o: writeFiles.c writeFiles.h global.h
	gcc -c -Wall -ansi -pedantic writeFiles.c -o writeFiles.o


