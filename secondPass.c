#include "globalFunctions.h"
#include "secondPass.h"
#include "analizeWord.h" 

/*complete the missing code word in the code image and return true if succeed*/
bool secondPassProccess(FILE* currentFile, int* ic, int* dc, SymbolTable* symbolTable, CodeImg* codeImg, DataImg* dataImg, ExternList* externList) {
	char* currentLine, * operand;
	short regDirectIndex;
	int i, numOfLines, lineLength, currentFileLine = 1, firstChar = 0, lastChar = 0;
	CodeWord* currentCodeWord = codeImg->head;
	numOfLines = getNumOfLines(currentFile) - 1;
	if (validEntries(currentFile, symbolTable, numOfLines) == false)/*in case there are invalid entries definitions*/
		return false;
	for (i = 100; i < (*ic); i++) {
		if (currentCodeWord->labelLine != -1) {/*look for the lines where label found*/
			moveToLine(currentFile, currentCodeWord->labelLine, currentFileLine);
			lineLength = getLineLength(currentFile);
			currentLine = createTempLine(currentFile, lineLength);
			currentFileLine = currentCodeWord->labelLine;
			firstChar = skipWhiteSpaces(currentLine, firstChar);
			firstChar = skipNonWS(currentLine, firstChar + 1);
			if (currentLine[firstChar] == ':') {/*if label defintion*/
				firstChar = skipWhiteSpaces(currentLine, firstChar + 1);
				firstChar = skipNonWS(currentLine, firstChar);
				lastChar = firstChar;
			}
			operand = getArg(currentLine, &firstChar, &lastChar, true);/*first operand*/
			if (operand[firstChar] == '#' || getRegister(operand) != -1) {/*already added in first pass*/
				free(operand);
				firstChar = lastChar;
				operand = getArg(currentLine, &firstChar, &lastChar, false);
				regDirectIndex = getRegDirectIndex(operand);
				if (regDirectIndex != 0) {/*if addressing mode is index*/
					operand[regDirectIndex] = '\0';
					if (isUpdateSucceed(operand, currentFileLine, currentCodeWord->ic, symbolTable, externList, codeImg) == false)
						return false;
				}
				else if (isUpdateSucceed(operand, currentFileLine, currentCodeWord->ic, symbolTable, externList, codeImg) == false)
					return false;
			}
			else {/*first operand is label*/
				regDirectIndex = getRegDirectIndex(operand);
				if (regDirectIndex != 0) {/*if addressing mode is index*/
					operand[regDirectIndex] = '\0';
					if (isUpdateSucceed(operand, currentFileLine, currentCodeWord->ic, symbolTable, externList, codeImg) == false)
						return false;
					firstChar = lastChar;
					operand = getArg(currentLine, &firstChar, &lastChar, false);/*get next operandd*/
					if (operand[0] != '\0') {/*there are two operands*/
						regDirectIndex = getRegDirectIndex(operand);
						if (regDirectIndex != 0) {/*if second addressing mode is index*/
							operand[regDirectIndex] = '\0';
							if (isUpdateSucceed(operand, currentFileLine, currentCodeWord->ic + 2, symbolTable, externList, codeImg) == false)
								return false;
						}
						else if (operand[firstChar] != '#' && getRegister(operand) == -1)/*if second operand is label*/
							if (isUpdateSucceed(operand, currentFileLine, currentCodeWord->ic + 2, symbolTable, externList, codeImg) == false)
								return false;
					}
				}
				else {/*if first operand is label*/
					if (isUpdateSucceed(operand, currentFileLine, currentCodeWord->ic, symbolTable, externList, codeImg) == false)
						return false;
					firstChar = lastChar;
					operand = getArg(currentLine, &firstChar, &lastChar, false);
					if (operand[0] != '\0') {/*there are two operands*/
						regDirectIndex = getRegDirectIndex(operand);
						if (regDirectIndex != 0) {/*if second addressing mode is index*/
							operand[regDirectIndex] = '\0';
							if (isUpdateSucceed(operand, currentFileLine, currentCodeWord->ic + 2, symbolTable, externList, codeImg) == false)
								return false;
						}
						else if (operand[0] != '#' && getRegister(operand) == -1)/*if second operand is label*/
							if (isUpdateSucceed(operand, currentFileLine, currentCodeWord->ic + 2, symbolTable, externList, codeImg) == false)
								return false;
					}
				}
			}
			currentFileLine++;
			free(currentLine);
		}
		firstChar = 0;
		lastChar = 0;
		currentCodeWord = currentCodeWord->next;
	}
	return true;
}

/*checks if all entry labels are defined and valid. return true if succeed*/
bool validEntries(FILE* currentFile, SymbolTable* symbolTable, int numOfLines) {
	char* labelName, * tempLine;
	int currentLineNumber, lineLength, firstChar, lastChar, symbolIndex;
	bool validEntry = true;
	Symbol* tempSymbol = symbolTable->head;
	for (currentLineNumber = 0; currentLineNumber < numOfLines; currentLineNumber++) {
		lineLength = getLineLength(currentFile);
		tempLine = createTempLine(currentFile, lineLength);
		firstChar = skipWhiteSpaces(tempLine, 0);
		lastChar = skipNonWS(tempLine, firstChar);
		labelName = createTempWord(tempLine, firstChar, lastChar);
		if (labelName[0] != '\0') {
			if (strcmp(labelName, ".entry") == 0) {
				free(labelName);
				firstChar = skipWhiteSpaces(tempLine, lastChar + 1);
				lastChar = skipNonWS(tempLine, firstChar);
				labelName = createTempWord(tempLine, firstChar, lastChar);
				firstChar = skipWhiteSpaces(tempLine, lastChar + 1);
				if (tempLine[firstChar] != '\0') {
					printf("LINE %d; ERROR .entry: too many arguments\n", currentLineNumber + 1);
					validEntry = false;
				}
				for (symbolIndex = 0; symbolIndex < symbolTable->numOfSymbols; symbolIndex++) {
					if (strcmp(tempSymbol->name, labelName) == 0) {
						if (tempSymbol->attributes == EXTERNAL) {
							printf("LINE %d; ERROR .entry: label already defined as .extern\n", currentLineNumber + 1);
							validEntry = false;
						}
						else {
							tempSymbol->attributes = ENTRY;
							symbolIndex = symbolTable->numOfSymbols;
						}
					}
					tempSymbol = tempSymbol->next;
				}
				if (tempSymbol == NULL) {
					printf("LINE %d; ERROR .entry: invalid label\n", currentLineNumber + 1);
					validEntry = false;
				}
				tempSymbol = symbolTable->head;
			}
		}
		free(tempLine);
		free(labelName);
		firstChar = 0;
		lastChar = 0;
		symbolIndex = 0;
	}
	rewind(currentFile);
	return validEntry;
}

/*return true if the missing code words were successfuly updated*/
bool isUpdateSucceed(char* labelName, int lineNumber, int currentIC, SymbolTable* symbolTable, ExternList* externList, CodeImg* codeImg) {
	int i;
	Symbol* currentSymbol = symbolTable->head;
	for (i = 0; i < symbolTable->numOfSymbols; i++) {
		if (strcmp(currentSymbol->name, labelName) == 0) {
			if (currentSymbol->attributes == EXTERNAL) {
				addExtern(labelName, false, currentIC, 0, 0, externList);
				addMissingCode(codeImg, currentIC, currentSymbol->value, currentSymbol->base, true);
			}
			else {
				addMissingCode(codeImg, currentIC, currentSymbol->value, currentSymbol->base, false);
				free(labelName);
			}
			return true;
		}
		currentSymbol = currentSymbol->next;
	}
	printf("LINE %d; ERROR label: undefined label", lineNumber);
	return false;
}

/*initialize and add two given code words to code image. represent label*/
void intializeWord(CodeWord* codeWord1, CodeWord* codeWord2, int base, int offset) {
	short hexadecimal[] = { 0, 0, 0, 0 }, hexadecimalIndex = 3;
	float baseValue = base;
	while (baseValue != 0) {
		baseValue /= 16;
		if (baseValue == (int)baseValue)
			hexadecimal[hexadecimalIndex] = 0;
		else {
			hexadecimal[hexadecimalIndex] = (baseValue - (int)baseValue) * 16;
			baseValue = (int)baseValue;
		}
		hexadecimalIndex--;
	}
	codeWord1->A = R;
	codeWord1->B = hexadecimal[0];
	codeWord1->C = hexadecimal[1];
	codeWord1->D = hexadecimal[2];
	codeWord1->E = hexadecimal[3];
	codeWord1->ic = 0;
	codeWord1->labelLine = -1;

	codeWord2->A = R;
	codeWord2->B = 0;
	codeWord2->C = 0;
	codeWord2->D = 0;
	codeWord2->E = offset;
	codeWord2->ic = 0;
	codeWord2->labelLine = -1;
}

/*add the two missing code words to the code image*/
void addMissingCode(CodeImg* codeImg, int currentIC, int base, int offset, bool isExtern) {
	int i;
	CodeWord* firstWord = codeImg->head;
	CodeWord* lastWord;
	CodeWord* newCodeWord1 = (CodeWord*)malloc(sizeof(CodeWord));
	CodeWord* newCodeWord2 = (CodeWord*)malloc(sizeof(CodeWord));
	if (newCodeWord1 == NULL) {
		printf("ERROR MALLOC: cannot allocating memory\n");
		exit(1);
	}
	if (newCodeWord2 == NULL) {
		printf("ERROR MALLOC: cannot allocating memory\n");
		exit(1);
	}
	if (isExtern == false)
		intializeWord(newCodeWord1, newCodeWord2, base, offset);
	else {
		newCodeWord1->A = E;
		newCodeWord1->B = 0;
		newCodeWord1->C = 0;
		newCodeWord1->D = 0;
		newCodeWord1->E = 0;
		newCodeWord1->ic = 0;
		newCodeWord1->labelLine = -1;

		newCodeWord2->A = E;
		newCodeWord2->B = 0;
		newCodeWord2->C = 0;
		newCodeWord2->D = 0;
		newCodeWord2->E = 0;
		newCodeWord2->ic = 0;
		newCodeWord2->labelLine = -1;
	}
	for (i = 100; i < currentIC; i++)
		firstWord = firstWord->next;
	lastWord = firstWord->next;
	newCodeWord2->next = lastWord;
	newCodeWord1->next = newCodeWord2;
	firstWord->next = newCodeWord1;
}

/*return the index of the char '[' in a given string, if '[' does not exist, return 0*/
short getRegDirectIndex(char* label) {
	short i = 0;
	while (label[i] != '\0') {
		if (label[i] == '[')
			return i;
		i++;
	}
	return 0;
}

/*return the argument of a given line*/
char* getArg(char* currentLine, int* firstChar, int* lastChar, bool isFirstArg) {
	char* nextArg;
	*firstChar = skipWhiteSpaces(currentLine, (*firstChar) + 1);
	*lastChar = *firstChar;
	if (isFirstArg == true) {
		while (currentLine[*lastChar] != ',' && currentLine[*lastChar] != '\0')
			(*lastChar)++;
		if (currentLine[*lastChar] == ',')
			nextArg = createTempWord(currentLine, *firstChar, (*lastChar) - 1);
		else {
			*lastChar = skipNonWS(currentLine, *firstChar);
			nextArg = createTempWord(currentLine, *firstChar, *lastChar);
		}
	}
	else {
		*firstChar = skipWhiteSpaces(currentLine, *firstChar);
		*lastChar = skipNonWS(currentLine, *firstChar);
		nextArg = createTempWord(currentLine, *firstChar, *lastChar);
	}
	return nextArg;
}

/*set the file cursor to a given line*/
void moveToLine(FILE* currentFile, int lineNumber, int lastLineChecked) {
	char ch;
	int lineCounter = lastLineChecked;
	while ((ch = fgetc(currentFile)) != EOF && lineNumber != lineCounter)
		if (ch == '\n')
			lineCounter++;
	if (isspace(ch))
		while (isspace((ch = fgetc(currentFile))));/*point to next char after first command, must be space*/
}

/*add external to the extern list*/
void addExtern(char* labelName, bool isEntry, int currentIC, int base, int offset, ExternList* externList) {
	Extern* newWord = (Extern*)malloc(sizeof(Extern));
	newWord->name = labelName;
	newWord->Base = currentIC + 1;
	newWord->Offset = currentIC + 2;
	newWord->next = NULL;
	if (externList->head == NULL) {
		externList->head = newWord;
		externList->tail = newWord;
	}
	else {
		externList->tail->next = newWord;
		externList->tail = externList->tail->next;
	}
	externList->numberOfExtern++;
}

