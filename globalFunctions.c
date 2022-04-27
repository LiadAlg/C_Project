#include "globalFunctions.h"

/*same as strcat, but using malloc and make sure success*/
char* strcatChecked(char* str1, char* str2){
	char* str = (char*)malloc(sizeof(char) * (strlen(str1) + strlen(str2) + 1));
	if (str == NULL) {
		printf("ERROR MALLOC: cannot allocating memory\n");
		exit(1);
	}
	strcpy(str, str1);
	strcat(str, str2);
	return str;
}

/*skip white spaces of a given line, and a given index to start from */
int skipWhiteSpaces(char* currentFileLine, int firstWordChar) {
	int indexOfNonWS = firstWordChar;
	while (isspace(currentFileLine[indexOfNonWS]))
		indexOfNonWS++;
	return indexOfNonWS;
}

/*skip non - white spaces of a given line, and a given index to start from */
int skipNonWS(char* currentFileLine, int firstNonWSChar) {
	int endOfWord = firstNonWSChar;
	while (!isspace(currentFileLine[firstNonWSChar]) && currentFileLine[firstNonWSChar++] != '\0')
		endOfWord++;
	return endOfWord - 1;
}

/*create a line from a given file and the length of the line, using malloc*/
char* createTempLine(FILE* sourceFile, int lineLength) {
	int i;
	char* tempLine = (char*)malloc(sizeof(char) * (lineLength + 1));
	if (tempLine == NULL) {
		printf("ERROR MALLOC: cannot allocating memory\n");
		exit(1);
	}
	for (i = 0; i < lineLength; i++)
		tempLine[i] = fgetc(sourceFile);
	tempLine[i] = '\0';
	return tempLine;
}

/*create a word from a given file and the length of the line, using malloc */
char* createTempWord(char* currentLine, int firstCharInLine, int lastCharInLine) {
	int i = 0, CharIndex;
	char* tempWord = (char*)malloc(sizeof(char) * (lastCharInLine - firstCharInLine + 2));
	if (tempWord == NULL) {
		printf("ERROR MALLOC: cannot allocating memory\n");
		exit(1);
	}
	for (CharIndex = firstCharInLine; CharIndex <= lastCharInLine; CharIndex++, i++)
		tempWord[i] = currentLine[CharIndex];
	tempWord[i] = '\0';
	return tempWord;
}

/*return the line Length of a given line from a file*/
int getLineLength(FILE* sourceFile) {
	char ch;
	int lineLength = 0;
	while ((ch = fgetc(sourceFile)) != EOF) {
		if (ch == '\n') {
			lineLength++;
			fseek(sourceFile, -1 * lineLength, SEEK_CUR);
			return lineLength;
		}
		lineLength++;
	}
	fseek(sourceFile, -1 * lineLength, SEEK_CUR);
	return lineLength;
}

/*return the number of lines in a given file*/
int getNumOfLines(FILE* sourceFile) {
	char ch;
	int numberOfLines = 1;
	while ((ch = fgetc(sourceFile)) != EOF)
		if (ch == '\n')
			numberOfLines++;
	rewind(sourceFile);
	return numberOfLines;
}

/*free all objects of symbolTable*/
void freeSymbolTable(SymbolTable* symbolTable) {
	int i;
	Symbol* tempSymbol = symbolTable->head->next;
	for (i = 0; i < symbolTable->numOfSymbols; i++) {
		free(symbolTable->head->name);
		free(symbolTable->head);
		if (tempSymbol == NULL) {
			symbolTable->numOfSymbols = 0;
			symbolTable->head = NULL;
		}
		else {
			symbolTable->head = tempSymbol;
			tempSymbol = tempSymbol->next;
		}
	}
}

/*free all objects of dataImg*/
void freeDataImg(DataImg* dataImg) {
	int i;
	CodeWord* tempDataWord = dataImg->head->next;
	for (i = 0; i < dataImg->dc; i++) {
		free(dataImg->head);
		if (tempDataWord == NULL) {
			dataImg->dc = 0;
			dataImg->head = NULL;
			dataImg->tail = NULL;
		}
		else {
			dataImg->head = tempDataWord;
			tempDataWord = tempDataWord->next;
		}
	}
}

/*free all objects of codeImg*/
void freeCodeImg(CodeImg* codeImg, int ic) {
	int i;
	CodeWord* tempCodeWord = codeImg->head->next;
	for (i = 0; i < ic; i++) {
		free(codeImg->head);
		if (tempCodeWord == NULL) {
			codeImg->head = NULL;
			codeImg->tail = NULL;
		}
		else {
			codeImg->head = tempCodeWord;
			tempCodeWord = tempCodeWord->next;
		}
	}
}

/*free all objects of externList*/
void freeExternList(ExternList* externList) {
	int i;
	Extern* tempExtern = externList->head->next;
	for (i = 0; i < externList->numberOfExtern; i++) {
		free(externList->head->name);
		free(externList->head);
		if (tempExtern == NULL) {
			externList->numberOfExtern = 0;
			externList->head = NULL;
			externList->tail = NULL;
		}
		else {
			externList->head = tempExtern;
			tempExtern = tempExtern->next;
		}
	}
}

/*free all linkedList used in the program*/
void freeLinkedLists(SymbolTable* symbolTable, CodeImg* codeImg, DataImg* dataImg, ExternList* externList, int ic) {
	if (symbolTable->head != NULL)
		freeSymbolTable(symbolTable);
	if (codeImg->head != NULL)
		freeCodeImg(codeImg, ic);
	if (dataImg->head != NULL)
		freeDataImg(dataImg);
	if (externList->head != NULL)
		freeExternList(externList);
}

/*all functions ahead are printing to stdout! for debug only!!!

void printCodeImg(CodeImg* codeImg, int codeImgIC) {
	CodeWord* newDataWord = codeImg->head;
	int i;
	printf("DC\tA value\tB value\tC value\tD value\tE value\n");
	for (i = 100; i < codeImgIC; i++) {
		printf("%d\t%x\t%x\t%x\t%x\t%x\n", i, newDataWord->A, newDataWord->B, newDataWord->C, newDataWord->D, newDataWord->E);
		newDataWord = newDataWord->next;
	}
}

void printDataImg(DataImg* dataImg, int ic) {
	CodeWord* newDataWord = dataImg->head;
	int i;
	printf("DC\tA value\tB value\tC value\tD value\tE value\n");
	for (i = ic; i < dataImg->dc + ic; i++) {
		printf("%d\t%x\t%x\t%x\t%x\t%x\n", i, newDataWord->A, newDataWord->B, newDataWord->C, newDataWord->D, newDataWord->E);
		newDataWord = newDataWord->next;
	}
}


void printSymbolTable(SymbolTable* symbolTable) {
	Symbol* tempSymbol = symbolTable->head;
	int i;
	printf("NAME\tVALUE\tBASE\tATTRIBUTES\n");
	for (i = 0; i < symbolTable->numOfSymbols; i++) {
		printf("%s\t%d\t%d\t%d\n", tempSymbol->name, tempSymbol->value, tempSymbol->base, tempSymbol->attributes);
		tempSymbol = tempSymbol->next;
	}
}

void printExternList(ExternList* externList) {
	Extern* newExternWord = externList->head;
	int i;
	printf("INDEX\tNAME\tBASE\tOFFSET\t\n");
	for (i = 0; i < externList->numberOfExtern; i++) {
		printf("%d\t%s\t%d\t%d\n", i, newExternWord->name, newExternWord->Base, newExternWord->Offset);
		newExternWord = newExternWord->next;
	}
}
*/
