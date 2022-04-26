#include "globalFunctions.h"
#include "macroDeploy.h"
#include "analizeWord.h"


bool createAmFile(char* filename) {
	char* tempLine, * sourceFileName, * targetFileName, * tempWord, * macroData;
	int firstChar = 0, lastChar = 0, firstMacroChar = 0, macroDataLength = 0, lineLength = 0, numOfLines = 1, currentLineNumber, macroState;
	bool successMacroDeployed = true, macroIsOn = false;
	FILE* sourceFile, * targetFile;
	MacroList macroList = { 0,NULL };
	sourceFileName = strcatChecked(filename, ".as");/*add to the name ".as" in order to open the file*/
	sourceFile = fopen(sourceFileName, "r");
	if (sourceFile == NULL) {
		printf("Error opening the file %s\n", sourceFileName);
		free(sourceFileName);
		return NULL;
	}
	if (validLength(sourceFile, sourceFileName) == true) {/*before creating <filename>.am, make sure all lines are valid length wise*/
		targetFileName = strcatChecked(filename, ".am");/*add to the name ".am" in order to open the file*/
		printf("\nFILE %s:\n\n", filename);
		targetFile = fopen(targetFileName, "w");
		if (targetFile == NULL) {
			printf("Error creating the file %s\n", targetFileName);
			free(targetFileName);
			return NULL;
		}
		free(targetFileName);
		numOfLines = getNumOfLines(sourceFile) - 1;
		macroState = -1;
		for (currentLineNumber = 0; currentLineNumber < numOfLines; currentLineNumber++) {/*for every single line in the source file*/
			lineLength = getLineLength(sourceFile);
			tempLine = createTempLine(sourceFile, lineLength);/*get the current line from the file*/
			firstChar = skipWhiteSpaces(tempLine, 0);
			lastChar = skipNonWS(tempLine, firstChar);
			tempWord = createTempWord(tempLine, firstChar, lastChar);/*get the first word in the line*/
			if (macroIsOn == true) {/*in previous line macro declared*/
				macroDataLength += lineLength;/*saves the total length of the macro*/
				if (strcmp(tempWord, "endm") == 0) {/*assuming there is always "endm" at the end of macro*/
					fseek(sourceFile, firstMacroChar, SEEK_SET);
					macroData = createTempLine(sourceFile, macroDataLength - lineLength);/*creating the content of the macro*/
					fseek(sourceFile, lineLength, SEEK_CUR);
					macroList.head->data = macroData;/*the head initialized in isValidMacro*/
					firstMacroChar += macroDataLength;/*firstMacroChar indicates where the macro content start*/
					macroDataLength = 0;
					macroIsOn = false;
				}
			}
			else {
				firstMacroChar += lineLength;
				if (isMacroName(&macroList, tempWord) == true) {/*if the word is a defined macro -> deploy it with the macro list*/
					deployMacro(targetFile, &macroList, tempWord);
				}
				else {
					macroState = isValidMacro(tempLine, currentLineNumber, &macroList);/*checks if there is macro definition, and if the macro name is valid*/
					if (macroState == MACRO_VALID) {
						macroIsOn = true;
					}
					if (macroState == NO_MACRO)/*if there is no macro put the current line in the new file*/
						fputs(tempLine, targetFile);
					if (macroState == MACRO_INVALID) {/*the file corrupted due to invalid macro name*/
						fseek(targetFile, 0, SEEK_SET);
						fputs("\n------------CORRUPTED FILE------------\n", targetFile);
						successMacroDeployed = false;
						fputs(tempLine, targetFile);
					}
				}
			}
			free(tempLine);
			free(tempWord);
		}
		fclose(targetFile);
	}
	else {
		free(sourceFileName);
		return false;
	}
	free(sourceFileName);
	if (macroList.head != NULL)
		freeMacroList(&macroList);/*no need for macro list, free for next file*/
	fclose(sourceFile);
	return successMacroDeployed;
}

/*returns true if all lines in the file are valid length wise*/
bool validLength(FILE* sourceFile, char* sourceFlileName) {
	char ch;
	int nonWSCharsInLine = 0; /* number of non - white space chars in the current line read from the file.*/
	int lineNumber = 1;
	bool isValidFile = true;
	while (isspace((ch = fgetc(sourceFile))) && ch != EOF);
	if (ch == EOF) {/*checks if the file is empty*/
		printf("ERROR FILE: the file '%s' is empty\n\n", sourceFlileName);
		return false;
	}
	rewind(sourceFile);
	while ((ch = fgetc(sourceFile)) != EOF){
		if (ch == '\n'){
			if (isValidFile == false){
				excessiveLineLength(nonWSCharsInLine, lineNumber);
				lineNumber++;/*keep looking for errors*/
				nonWSCharsInLine = 0;
			}
			else{
				isValidFile = excessiveLineLength(nonWSCharsInLine, lineNumber);
				lineNumber++;
				nonWSCharsInLine = 0;
			}
		}
		else if (ch != ' ' && ch != '\t')
			nonWSCharsInLine++;
	}
	/*checks the last line in case reached EOF before '\n'*/
	if (isValidFile == true)
		isValidFile = excessiveLineLength(nonWSCharsInLine, lineNumber);
	else
		excessiveLineLength(nonWSCharsInLine, lineNumber);
	rewind(sourceFile);
	return isValidFile;
}

/*return true if a given line is no longer then maximum line length*/
bool excessiveLineLength(int nonWSCharsInLine, int lineNumber) {
	if (nonWSCharsInLine > MAX_LINE_LENGTH) {
		printf("LINE %d is longer then %d chars\n", lineNumber, MAX_LINE_LENGTH);
		return false;
	}
	return true;
}

/*checks if a given line is a macro definition and return one of three states - valid macro, invalid macro and no macro.  */
macroStates isValidMacro(char* currentFileLine, int lineNumber, MacroList* macroList) {
	int firstWordChar = 0, lastWordChar;
	char* tempWord;
	firstWordChar = skipWhiteSpaces(currentFileLine, firstWordChar);/*indicates the first char in a word*/
	lastWordChar = skipNonWS(currentFileLine, firstWordChar);/*indicates the last char in a word*/
	if (currentFileLine[lastWordChar] == ':') {/*it's a label*/
		firstWordChar = skipWhiteSpaces(currentFileLine, lastWordChar + 1);
		lastWordChar = skipNonWS(currentFileLine, firstWordChar);
	}/*move to next word*/
	if (lastWordChar - firstWordChar == 4) {
		tempWord = createTempWord(currentFileLine + firstWordChar, 0, 4);
		if (strcmp(tempWord, "macro") == 0) {
			free(tempWord);
			firstWordChar = skipWhiteSpaces(currentFileLine, lastWordChar + 1);
			lastWordChar = skipNonWS(currentFileLine, firstWordChar);
			tempWord = createTempWord(currentFileLine, firstWordChar, lastWordChar);/*get the next word after 'macro'*/
			/*checks if the macro is valid*/
			if (isCommandOrInstruction(tempWord) == true) {
				printf("LINE %d; ERROR MACRO: macro's name can't be a command, instruction or register name\n", lineNumber);
				return MACRO_INVALID;
			}
			if (tempWord[0] == '\0') {
				printf("LINE %d; ERROR MACRO: no name given to macro in macro definition\n", lineNumber);
				return MACRO_INVALID;
			}
			if (isMacroName(macroList, tempWord) == true) {
				printf("LINE %d; ERROR MACRO: the macro name '%s' is already defined as macro\n", lineNumber, tempWord);
				return MACRO_INVALID;
			}
			addMacro(tempWord, macroList);/*the macro is valid, add it*/
			return MACRO_VALID;
		}
		free(tempWord);
	}
	return NO_MACRO;
}

/*add new macro to macro list*/
void addMacro(char* tempWord, MacroList* macroList) {
	Macro* newMacro = (Macro*)malloc(sizeof(Macro));
	if (newMacro == NULL) {
		printf("ERROR MALLOC: cannot allocating memory\n");
		exit(1);
	}
	newMacro->name = tempWord;
	newMacro->next = macroList->head;
	macroList->head = newMacro;
	macroList->count++;
}

/*returns true if macro name is defined*/
bool isMacroName(MacroList* macroList, char* macroName) {
	Macro* tempMacro = macroList->head;
	while (tempMacro != NULL) {
		if (strcmp(macroName, tempMacro->name) == 0)
			return true;
		tempMacro = tempMacro->next;
	}
	return false;
}

/*replace a defined macro name to his content as described in the source file*/
void deployMacro(FILE* targetFile, MacroList* macroList, char* macroName) {
	Macro* tempMacro = macroList->head;
	int i;
	for (i = 0; i < macroList->count; i++) {
		if (strcmp(macroName, tempMacro->name) == 0) {
			fputs(tempMacro->data, targetFile);
			break;
		}
		tempMacro = tempMacro->next;
	}
}

/*free all objects in macro list*/
void freeMacroList(MacroList* macroList) {
	int i, numOfMacro = macroList->count;
	Macro* tempMacro = macroList->head->next;
	for (i = 0; i < numOfMacro; i++) {
		free(macroList->head->name);
		free(macroList->head->data);
		free(macroList->head);
		if (tempMacro == NULL) {
			macroList->count = 0;
			macroList->head = NULL;
			free(tempMacro);
		}
		else {
			macroList->head = tempMacro;
			tempMacro = macroList->head->next;
		}
	}
}




