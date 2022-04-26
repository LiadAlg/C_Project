#include "globalFunctions.h"
#include "firstPass.h"
#include "analizeWord.h"

/*creating code image without any label code word. creating data imge , symbol table and external list. return true if succeed*/
bool firstPassProccess(FILE* currentFile, int* ic, int* dc, SymbolTable* symbolTable, CodeImg* codeImg, DataImg* dataImg, ExternList* externList) {
	char* tempLine, * tempWord, * nextWord, * dataLine;
	int lineLength;
	int firstChar, lastChar, numOfLines, currentLineNumber;
	short symbolDefinitionState;
	numOfLines = getNumOfLines(currentFile) - 1;
	bool success = true;
	for (currentLineNumber = 0; currentLineNumber < numOfLines; currentLineNumber++) {/*for every single line in the source file*/
		symbolDefinitionState = NO_SYMBOL;
		lineLength = getLineLength(currentFile);
		tempLine = createTempLine(currentFile, lineLength);/*get the current line from the file*/
		firstChar = skipWhiteSpaces(tempLine, 0);
		lastChar = skipNonWS(tempLine, firstChar);
		tempWord = createTempWord(tempLine, firstChar, lastChar);/*get the first word in the line*/
		if (tempWord[0] != ';' && tempWord[0] != '\0') {/*if not empty or comment*/
			if (tempLine[lastChar] == ':') {
				tempWord[strlen(tempWord) - 1] = '\0';/*if label tempWord = label name*/
				symbolDefinitionState = isLabelDefinition(tempWord, currentLineNumber + 1, false, symbolTable);/*symbolDefinitionState = state as defined in global.h*/
			}
			if (symbolDefinitionState == INVALID_SYMBOL) {/* was symbol definition but the label is invalid*/
				free(tempWord);
				success = false;
			}
			else if (symbolDefinitionState == VALID_SYMBOL) {/*was symbol definition and label is valid*/
				firstChar = skipWhiteSpaces(tempLine, lastChar + 1);
				lastChar = skipNonWS(tempLine, firstChar);
				nextWord = createTempWord(tempLine, firstChar, lastChar);/*first word after definition*/
				firstChar = skipWhiteSpaces(tempLine, lastChar + 1);
				dataLine = createTempWord(tempLine, firstChar, lineLength - 1);/*the whole line after dafinition*/
				/*analize the line after symbol definition*/
				if (strcmp(nextWord, ".string") == 0 || strcmp(nextWord, ".data") == 0) {
					addSymbol(tempWord, *dc, 0, DATA, symbolTable);
					if (analizeData(dataLine, nextWord, dc, currentLineNumber + 1, firstChar, success, dataImg) == false)
						success = false;
				}
				else if (strcmp(nextWord, ".entry") == 0 || strcmp(nextWord, ".extern") == 0)
					printf("LINE %d; WARNING : label cannot defined as entry or extern type, ignore line %d\n", currentLineNumber + 1, currentLineNumber + 1);
				else {
					addSymbol(tempWord, (*ic) - getOffset(*ic), getOffset(*ic), CODE, symbolTable);
					if (analizeCommand(dataLine, nextWord, ic, currentLineNumber + 1, 0, codeImg) == false)
						success = false;
				}
			}
			else {/*it wasn't symbol definition*/
				firstChar = skipWhiteSpaces(tempLine, lastChar + 1);
				dataLine = createTempWord(tempLine, firstChar, lineLength - 1);/*current line*/
				/*analize the line*/
				if (strcmp(tempWord, ".string") == 0 || strcmp(tempWord, ".data") == 0) {
					if (analizeData(dataLine, tempWord, dc, currentLineNumber + 1, firstChar, success, dataImg) == false)
						success = false;
				}
				else if (strcmp(tempWord, ".extern") == 0) {
					lastChar = skipNonWS(tempLine, firstChar);
					nextWord = createTempWord(tempLine, firstChar, lastChar);
					firstChar = skipWhiteSpaces(tempLine, lastChar + 1);
					if (tempLine[firstChar] != '\0') {
						printf("LINE %d; ERROR .extern: too many arguments\n", currentLineNumber + 1);
						success = false;
					}
					else if (isLabelDefinition(nextWord, currentLineNumber + 1, true, symbolTable) == INVALID_SYMBOL) {
						free(nextWord);
						success = false;
					}
					else if (isLabelDefinition(nextWord, currentLineNumber + 1, true, symbolTable) == VALID_SYMBOL)
						addSymbol(nextWord, 0, 0, EXTERNAL, symbolTable);
				}
				else if (strcmp(tempWord, ".entry") != 0)
					if (analizeCommand(dataLine, tempWord, ic, currentLineNumber + 1, 0, codeImg) == false)
						success = false;
			}
			free(tempLine);
		}
	}
	if (*ic + *dc > MAX_RAM_SIZE) {/*in case  number of code word exeeded memory limit*/
		printf("ERROR RAM: number of code word exeeded memory limit\n");
		return false;
	}
	updateSymbols(ic, symbolTable);
	rewind(currentFile);
	return success;
}

/*checks if a given symbol name is valid, and return one of two Symbol's state - invalid or valid. 'symbolDefinition' defined in globals.h */
symbolDefinition isLabelDefinition(char* firstWord, int lineNumber, bool isExtern, SymbolTable* symbolTable) {
	int wordLength = strlen(firstWord);
	symbolDefinition state;
	if (wordLength > MAX_LABEL_LENGTH) {
		printf("LINE %d; ERROR LABEL:label's name is longer than %d chars\n", lineNumber, MAX_LABEL_LENGTH);
		return INVALID_SYMBOL;
	}
	if (isValidLabel(firstWord, wordLength) == false) {
		printf("LINE %d; ERROR LABEL:label's name is made up of non-alphabetic or numeric chars\n", lineNumber);
		return INVALID_SYMBOL;
	}
	if (isCommandOrInstruction(firstWord) == true) {
		printf("LINE %d; ERROR LABEL:label's name can't be a command, instruction or register name\n", lineNumber);
		return INVALID_SYMBOL;
	}
	if (symbolTable->head != NULL) {
		state = isDefinedLabel(firstWord, isExtern, symbolTable);
		if (state == INVALID_SYMBOL) {
			printf("LINE %d; ERROR LABEL:label's name has been already defined\n", lineNumber);
			return INVALID_SYMBOL;
		}
		if (isExtern == true)
			return isDefinedLabel(firstWord, true, symbolTable);
	}
	return VALID_SYMBOL;
}

/*checks if a given symbol name is defined, and return one of two Symbol's state - invalid or valid. 'symbolDefinition' defined in globals.h */
symbolDefinition isDefinedLabel(char* symbolName, bool isExtern, SymbolTable* symbolTable) {
	int i;
	Symbol* tempSymbol = symbolTable->head;
	for (i = 0; i < symbolTable->numOfSymbols; i++) {
		if (strcmp(tempSymbol->name, symbolName) == 0) {
			if (isExtern == true && tempSymbol->attributes == EXTERNAL)
				return NO_SYMBOL;
			return INVALID_SYMBOL;
		}
		tempSymbol = tempSymbol->next;
	}
	return VALID_SYMBOL;
}

/*return true if a given symbol name is semantically valid*/
bool isValidLabel(char* label, int labelLength) {
	int i;
	if (isalpha(label[0]) == 0)
		return false;
	for (i = 1; i < labelLength; i++) {
		if (isalpha(label[i]) == 0 && isdigit(label[i]) == 0)
			return false;
	}
	return true;
}

/*add symbol to symbol table*/
void addSymbol(char* symbolName, int symbolValue, int symbolBase, Attributes symbolAttributes, SymbolTable* symbolTable) {
	Symbol* newSymbol = (Symbol*)malloc(sizeof(Symbol));
	if (newSymbol == NULL) {
		printf("ERROR MALLOC: cannot allocating memory\n");
		exit(1);
	}
	newSymbol->name = symbolName;
	newSymbol->value = symbolValue;
	newSymbol->base = symbolBase;
	newSymbol->attributes = symbolAttributes;
	newSymbol->next = NULL;
	if (symbolTable->head == NULL) {
		symbolTable->head = newSymbol;
		symbolTable->tail = newSymbol;
	}
	else {
		symbolTable->tail->next = newSymbol;
		symbolTable->tail = symbolTable->tail->next;
	}
	symbolTable->numOfSymbols++;
}


/*return true if instruction line was valid and successfully added to data image*/
bool analizeData(char* dataLine, char* stringOrData, int* dc, int lineNumber, int columnNumber, bool success, DataImg* dataImg) {
	int firstChar = 0, currentInteger;
	short stringLen;
	bool isCommaLast = true, isNegative;
	if (strcmp(stringOrData, ".string") == 0) {/*it's instruction .string*/
		if ((stringLen = stringLength(dataLine)) == -1) {/*invalid string*/
			printf("LINE %d; ERROR .string: improper use of apostrophes\n", lineNumber);
			return false;
		}
		if (stringLen == 1) {
			printf("LINE %d; ERROR .string: the string is null\n", lineNumber);
			return false;
		}
		if (success == true)/*add string only if so far all lines are valid*/
			addString(dataLine, dc, stringLen, dataImg);
	}
	else if (strcmp(stringOrData, ".data") == 0) {/*it's instruction .data*/
		while (firstChar < strlen(dataLine)) {
			isNegative = false;
			if (dataLine[firstChar] == '+') {
				isNegative = false;
				firstChar = skipWhiteSpaces(dataLine, firstChar + 1);
			}
			else if (dataLine[firstChar] == '-') {
				isNegative = true;
				firstChar = skipWhiteSpaces(dataLine, firstChar + 1);
			}
			if (dataLine[firstChar] != 48) {/*atoi return 0 on unsuccessfull conversion. separate when number is 0*/
				if ((currentInteger = atoi(dataLine + firstChar)) == 0) {/*get the number from a given line*/
					printf("LINE %d; ERROR .data: invalid argument, must be an integer\n", lineNumber);
					return false;
				}
				if (currentInteger >= MAX_INT_VALUE || currentInteger < -1 * MAX_INT_VALUE) {
					printf("LINE %d ; ERROR .data: argument absolute value is bigger then maximum integer value\n", lineNumber);;
					return false;
				}
			}
			else
				currentInteger = 0;/*atoi return 0 on unsuccessfull conversion*/
			isCommaLast = false;
			firstChar += countDigit(currentInteger);/*get the index of the next char after number*/
			if (!isspace(dataLine[firstChar]) && dataLine[firstChar] != ',') {
				printf("LINE %d; ERROR .data: invalid argument, must be an integer\n", lineNumber);
				return false;
			}
			firstChar = skipWhiteSpaces(dataLine, firstChar);
			if (dataLine[firstChar] == ',')
				isCommaLast = true;
			else if (isdigit(dataLine[firstChar]) != 0) {
				printf("LINE %d; ERROR .data: missing comma\n", lineNumber);
				return false;
			}
			else if (dataLine[firstChar] != ',' && dataLine[firstChar] != '\0') {
				printf("LINE %d; ERROR .data: invalid argument\n", lineNumber);
				return false;
			}
			if (success == true)/*add number only if so far all lines are valid*/
				addNumber(currentInteger, 0, dc, isNegative, false, -1, NULL, dataImg);
			if (dataLine[firstChar] != '\0')
				firstChar = skipWhiteSpaces(dataLine, firstChar + 1);
		}
		if (isCommaLast == true) {
			printf("LINE %d; ERROR .data: last argument is a comma\n", lineNumber);
			return false;
		}
	}
	return true;
}

/*add a given number to code or data image*/
void addNumber(int currentNumber, int* ic, int* dc, bool isNegative, bool isCode, int lineNumber, CodeImg* codeImg, DataImg* dataImg) {
	short hexadecimalIndex = 3, hexadecimal[] = { 0,0,0,0 };
	float numValue = currentNumber;
	CodeWord* newIntegerWord = (CodeWord*)malloc(sizeof(CodeWord));
	if (newIntegerWord == NULL) {
		printf("ERROR MALLOC: cannot allocating memory\n");
		exit(1);
	}
	if (currentNumber < 0)
		numValue *= -1;
	while (numValue != 0) {/*get the hexadecimal value for each char*/
		numValue /= 16;
		if (numValue == (int)numValue)
			hexadecimal[hexadecimalIndex] = 0;
		else {
			hexadecimal[hexadecimalIndex] = (numValue - (int)numValue) * 16;
			numValue = (int)numValue;
		}
		hexadecimalIndex--;
	}
	if (isNegative == true) {
		hexadecimal[3] = 16 - hexadecimal[3];
		for (hexadecimalIndex = 2; hexadecimalIndex >= 0; hexadecimalIndex--) {
			hexadecimal[hexadecimalIndex] = 15 - hexadecimal[hexadecimalIndex];
			if (hexadecimal[hexadecimalIndex + 1] == 16) {
				hexadecimal[hexadecimalIndex]++;
				hexadecimal[hexadecimalIndex + 1] = 0;
			}
		}
	}
	/*initialize code word*/
	newIntegerWord->A = 4;
	newIntegerWord->B = hexadecimal[0];
	newIntegerWord->C = hexadecimal[1];
	newIntegerWord->D = hexadecimal[2];
	newIntegerWord->E = hexadecimal[3];
	newIntegerWord->labelLine = lineNumber;/*use that in second pass*/
	newIntegerWord->next = NULL;
	if (isCode == true) {/*add to code image*/
		newIntegerWord->ic = *ic;
		if (codeImg->head == NULL) {
			codeImg->head = newIntegerWord;
			codeImg->tail = newIntegerWord;
		}
		else {
			codeImg->tail->next = newIntegerWord;
			codeImg->tail = codeImg->tail->next;
		}
		(*ic)++;/*increasing ic value*/
	}
	else {/*add to data img*/
		if (dataImg->head == NULL) {
			dataImg->head = newIntegerWord;
			dataImg->tail = newIntegerWord;
		}
		else {
			dataImg->tail->next = newIntegerWord;
			dataImg->tail = dataImg->tail->next;
		}
		(*dc)++;/*increasing dc value*/
		dataImg->dc = *dc;
	}
}

/*add a given string to data image*/
void addString(char* string, int* dc, short stringLen, DataImg* dataImg) {
	bool counter, hexadecimalIndex;
	short hexadecimal[] = { 0,0 };
	int i;
	float charValue;
	for (i = 1; i <= stringLen; i++) {
		CodeWord* newDataWord = (CodeWord*)malloc(sizeof(CodeWord));
		if (newDataWord == NULL) {
			printf("ERROR MALLOC: cannot allocating memory\n");
			exit(1);
		}
		/*initialize code word*/
		newDataWord->A = 4;
		newDataWord->B = 0;
		newDataWord->C = 0;
		newDataWord->labelLine = -1;
		newDataWord->next = NULL;
		if (i == stringLen) {
			newDataWord->D = 0;
			newDataWord->E = 0;
		}
		else {
			counter = 0;
			hexadecimalIndex = 0;
			charValue = string[i];
			while (charValue != 0) {
				charValue /= 16;
				if (charValue == (int)charValue)
					hexadecimal[hexadecimalIndex] = 0;
				else {
					hexadecimal[hexadecimalIndex] = (charValue - (int)charValue) * 16;
					charValue = (int)charValue;
				}
				if (counter == 0) {
					newDataWord->E = hexadecimal[hexadecimalIndex];
					counter++;
					hexadecimalIndex++;
				}
				else
					newDataWord->D = hexadecimal[hexadecimalIndex];
			}
		}
		if (dataImg->head == NULL) {
			dataImg->head = newDataWord;
			dataImg->tail = newDataWord;
		}
		else {
			dataImg->tail->next = newDataWord;
			dataImg->tail = dataImg->tail->next;
		}
		(*dc)++;
	}
	dataImg->dc = *dc;
}

/*return true if command line was valid and successfully added to code image*/
bool analizeCommand(char* commandLine, char* commandName, int* ic, int lineNumber, int columnNumbervoid, CodeImg* codeImg) {
	short commandIndex, registerIndex1 = 0, registerIndex2 = 0;
	int firstChar = 0, firstInteger = 0, secondInteger = 0;
	bool isNegative1 = false, isNegative2 = false;
	char* operand;
	AddressingModes mode1, mode2;
	commandIndex = getCommandName(commandName);
	if (commandIndex != -1) {
		addCodeWord(A, 0, 0, 0, 0, -1, ic, commandIndex, true, codeImg);/*add opcode*/
		if ((commandIndex == 14 || commandIndex == 15) && commandLine[skipWhiteSpaces(commandLine, 0)] != '\0') {/*if command is rts or stop (third group)*/
			printf("LINE %d; ERROR command: too many argument\n", lineNumber);
			return false;
		}
		if (commandIndex <= 13 && commandIndex >= 5) {/*if second group - gets only one operand */
			operand = getNextWord(commandLine, 0, false, true, lineNumber);
			if (operand == '\0')/*make sure there is only one operand*/
				return false;
			free(operand);
			firstChar = skipNonWS(commandLine, firstChar);
			operand = createTempWord(commandLine, 0, firstChar);/*get operand name*/
			mode1 = getAddressingMode(operand);/*get addressing mode of that operand*/
			if (mode1 == INVALID_MODE) {
				printf("LINE %d; ERROR command: invalid argument\n", lineNumber);
				return false;
			}
			if (mode1 == IMMEDIATE) {
				if (commandIndex != PRN_OP) {
					printf("LINE %d; ERROR command: addressing mode cannot be immediate\n", lineNumber);
					return false;
				}
				firstInteger = getNum(operand, lineNumber);
				if (firstInteger == MAX_INT_VALUE)
					return false;
				if (firstInteger < 0)
					isNegative1 = true;
				addCodeWord(A, 0, 0, 0, mode1, -1, ic, commandIndex, false, codeImg);/**/
				addNumber(firstInteger, ic, 0, isNegative1, true, -1, codeImg, NULL);
			}
			else if (mode1 == REGISTER_DIRECT) {
				if (commandIndex == 9) {
					printf("LINE %d; ERROR command: addressing mode cannot be register direct\n", lineNumber);
					return false;
				}
				registerIndex1 = getRegister(operand);
				addCodeWord(A, 0, 0, registerIndex1, mode1, -1, ic, commandIndex, false, codeImg);
			}
			else if (mode1 == DIRECT) {
				addCodeWord(A, 0, 0, 0, mode1, lineNumber, ic, commandIndex, false, codeImg);
				(*ic) += 2;
			}
			else if (mode1 == INDEX) {
				firstChar = 0;
				while (operand[firstChar++] != '[');
				registerIndex1 = atoi(operand + firstChar + 1);
				addCodeWord(A, 0, 0, registerIndex1, mode1, lineNumber, ic, commandIndex, false, codeImg);
				(*ic) += 2;
			}
			free(operand);
		}
		else if (commandIndex <= 4) {/*if first group - gets two operands*/
			operand = getNextWord(commandLine, 0, true, true, lineNumber);
			if (operand == NULL)
				return false;
			mode1 = getAddressingMode(operand);
			if (mode1 == INVALID_MODE) {
				printf("LINE %d; ERROR command: invalid argument\n", lineNumber);
				return false;
			}
			if ((mode1 == IMMEDIATE || mode1 == REGISTER_DIRECT) && commandIndex == LEA_OP) {
				printf("LINE %d; ERROR command : addressing mode cannot be immediate or register direct\n", lineNumber);
				return false;
			}
			if (mode1 == IMMEDIATE) {
				firstInteger = getNum(operand, lineNumber);
				if (firstInteger == MAX_INT_VALUE)
					return false;
				if (firstInteger < 0)
					isNegative1 = true;
			}
			else if (mode1 == REGISTER_DIRECT)
				registerIndex1 = getRegister(operand);
			else if (mode1 == INDEX) {
				while (operand[firstChar++] != '[');
				registerIndex1 = atoi(commandLine + firstChar + 1);
				firstChar = 0;
			}
			operand = getNextWord(commandLine, strlen(operand), true, false, lineNumber);
			if (operand == NULL)
				return false;
			mode2 = getAddressingMode(operand);
			if (mode2 == INVALID_MODE) {
				printf("LINE %d; ERROR command: invalid second argument\n", lineNumber);
				return false;
			}
			if (mode2 == IMMEDIATE && commandIndex != CMP_OP) {
				printf("LINE %d; ERROR command: second addressing mode cannot be immediate\n", lineNumber);
				return false;
			}
			if (mode2 == IMMEDIATE) {
				secondInteger = getNum(operand, lineNumber);
				if (secondInteger == MAX_INT_VALUE)
					return false;
				if (secondInteger < 0)
					isNegative2 = true;
				if (mode1 == DIRECT || mode1 == INDEX) {
					addCodeWord(A, registerIndex1, mode1, registerIndex2, mode2, lineNumber, ic, commandIndex, false, codeImg);
					(*ic) += 2;
				}
				else
					addCodeWord(A, registerIndex1, mode1, registerIndex2, mode2, -1, ic, commandIndex, false, codeImg);
				if (mode1 == IMMEDIATE)
					addNumber(firstInteger, ic, 0, isNegative1, true, -1, codeImg, NULL);
				addNumber(secondInteger, ic, 0, isNegative2, true, -1, codeImg, NULL);
			}
			else if (mode2 == DIRECT) {
				if (mode1 == IMMEDIATE) {
					addCodeWord(A, registerIndex1, mode1, registerIndex2, mode2, -1, ic, commandIndex, false, codeImg);
					addNumber(firstInteger, ic, 0, isNegative1, true, lineNumber, codeImg, NULL);
					(*ic) += 2;
				}
				else {
					addCodeWord(A, registerIndex1, mode1, registerIndex2, mode2, lineNumber, ic, commandIndex, false, codeImg);
					if (mode1 == REGISTER_DIRECT)
						(*ic) -= 2;
					(*ic) += 4;
				}
			}
			else if (mode2 == INDEX) {
				while (operand[firstChar++] != '[');
				registerIndex2 = atoi(commandLine + firstChar + 1);
				if (mode1 == IMMEDIATE) {
					addCodeWord(A, registerIndex1, mode1, registerIndex2, mode2, -1, ic, commandIndex, false, codeImg);
					addNumber(firstInteger, ic, 0, isNegative1, true, lineNumber, codeImg, NULL);
					(*ic) += 2;
				}
				else {
					addCodeWord(A, registerIndex1, mode1, registerIndex2, mode2, lineNumber, ic, commandIndex, false, codeImg);
					if (mode1 == REGISTER_DIRECT)
						(*ic) -= 2;/*there is one lanel name*/
					(*ic) += 4;/*there are 2 label name*/
				}
			}
			else {
				registerIndex2 = getRegister(operand);
				if (mode1 == IMMEDIATE) {
					addCodeWord(A, registerIndex1, mode1, registerIndex2, mode2, -1, ic, commandIndex, false, codeImg);
					addNumber(firstInteger, ic, 0, isNegative1, true, -1, codeImg, NULL);
				}
				else {
					if (mode1 == REGISTER_DIRECT)
						addCodeWord(A, registerIndex1, mode1, registerIndex2, mode2, -1, ic, commandIndex, false, codeImg);
					else {
						addCodeWord(A, registerIndex1, mode1, registerIndex2, mode2, lineNumber, ic, commandIndex, false, codeImg);
						(*ic) += 2;
					}
				}
			}
			free(operand);
		}
		return true;
	}
	/*in case it is not a command*/
	printf("LINE %d; ERROR : invalid argument\n", lineNumber);
	return false;
}

/*return pointer to the next word of a given line. also get the word after, depands on 'isFirstOperand'*/
char* getNextWord(char* currentLine, int current, bool numOfOperands, bool isFirstOperand, int lineNumber) {
	char* newWord;
	int currentIndex = current, firstChar;
	if (isFirstOperand == false) {
		currentIndex = skipWhiteSpaces(currentLine, currentIndex);
		if (currentLine[currentIndex] == '\0') {
			printf("LINE %d; ERROR command: missing argument\n", lineNumber);
			return NULL;
		}
		if (currentLine[currentIndex] != ',') {
			printf("LINE %d; ERROR command: missing comma\n", lineNumber);
			return NULL;
		}
		currentIndex = skipWhiteSpaces(currentLine, currentIndex + 1);
	}
	firstChar = currentIndex;
	while (!isspace(currentLine[currentIndex]) && currentLine[currentIndex] != ',' && currentLine[currentIndex] != '\0')
		currentIndex++;
	newWord = createTempWord(currentLine, firstChar, currentIndex - 1);
	if (isFirstOperand == false || numOfOperands == false) {
		currentIndex = skipWhiteSpaces(currentLine, currentIndex);
		if (currentLine[currentIndex] != '\0') {
			printf("LINE %d; ERROR command: too many arguments\n", lineNumber);
			free(newWord);
			return NULL;
		}
	}
	return newWord;
}

/*return the addressing mode of a given operand. 'AddressingModes' defined in globals.h*/
AddressingModes getAddressingMode(char* operand) {
	short i, counter = 0, operandLength;
	if (operand[0] == '#')
		return IMMEDIATE;
	if (getRegister(operand) != -1)
		return REGISTER_DIRECT;
	operandLength = strlen(operand);
	if (isValidLabel(operand, operandLength) == true)
		return DIRECT;
	i = 0;
	while (operand[i] != '[' && operand[i++] != '\0')
		counter++;
	if (operand[counter] == '[' && operand[counter + 1] == 'r' && operand[counter + 2] == 49 && operand[counter + 3] >= 48 && operand[counter + 3] <= 53 && operand[counter + 4] == ']')
		if (counter != 0 && isValidLabel(operand, counter - 1) == true)
			return INDEX;
	return INVALID_MODE;
}

/*add a single code word to code or data image*/
void addCodeWord(short ARE, short sourceReg, short sourceAddress, short targetReg, short targetAddress, int currentLine, int* ic, short index, bool isOpCode, CodeImg* codeImg) {
	short opcode = getOpcode(index);
	short funct = getFunct(index);
	CodeWord* newCodeWord = (CodeWord*)malloc(sizeof(CodeWord));
	if (newCodeWord == NULL) {
		printf("ERROR MALLOC: cannot allocating memory\n");
		exit(1);
	}
	newCodeWord->A = ARE;
	newCodeWord->labelLine = currentLine;
	newCodeWord->next = NULL;
	if (isOpCode == true) {
		newCodeWord->B = 0;
		newCodeWord->C = 0;
		newCodeWord->D = 0;
		newCodeWord->E = 0;
		if (opcode < 4)
			newCodeWord->E = getopCodeHex(opcode);
		else if (opcode == 4)
			newCodeWord->D = 1;
		else if (opcode == 5)
			newCodeWord->D = 2;
		else if (opcode == 9)
			newCodeWord->C = 2;
		else {
			newCodeWord->B = getopCodeHex(opcode - 12);
		}
	}
	else {
		newCodeWord->B = funct;
		newCodeWord->C = sourceReg;
		newCodeWord->D = getDE(sourceAddress, targetReg, true);
		newCodeWord->E = getDE(targetAddress, targetReg, false);
	}
	newCodeWord->ic = *ic;
	if (codeImg->head == NULL) {
		codeImg->head = newCodeWord;
		codeImg->tail = newCodeWord;
	}
	else {
		codeImg->tail->next = newCodeWord;
		codeImg->tail = codeImg->tail->next;
	}
	(*ic)++;
}

/*return the value of D or E*/
short getDE(short addressingMode, short registerValue, bool isD) {
	short regValue;
	if (isD == true) {
		if (registerValue >= 12)
			regValue = 3;
		else if (registerValue >= 8)
			regValue = 2;
		else if (registerValue >= 4)
			regValue = 1;
		else
			regValue = 0;
		return addressingMode * 4 + regValue;
	}
	return (registerValue % 4) * 4 + addressingMode;
}

/*returns the hexadecimal value of a give opcode*/
short getopCodeHex(short opcode) {
	short x = 2, y;

	if (opcode == 0 || opcode == 4)
		return 1;
	for (y = opcode; y > 1; y--)
		x *= 2;
	return x;
}

/*return the number as written in a given line*/
int getNum(char* commandLine, int lineNumber) {
	bool isNegative = false;
	int firstChar = 1, currentInteger;
	if (commandLine[firstChar] == '+')
		firstChar = skipWhiteSpaces(commandLine, firstChar + 1);
	else if (commandLine[firstChar] == '-') {
		isNegative = true;
		firstChar = skipWhiteSpaces(commandLine, firstChar + 1);
	}
	if (commandLine[firstChar] != 48) {
		if ((currentInteger = atoi(commandLine + firstChar)) == 0) {
			printf("LINE %d; ERROR command: invalid argument, must be an integer\n", lineNumber);
			return MAX_INT_VALUE;
		}
		if (currentInteger >= MAX_INT_VALUE || currentInteger < -1 * MAX_INT_VALUE) {
			printf("LINE %d ; ERROR command: argument absolute value is bigger then maximum integer value\n", lineNumber);
			return MAX_INT_VALUE;
		}
	}
	else
		currentInteger = 0;
	if (isNegative == true)
		currentInteger *= -1;
	return currentInteger;
}

/*update the symbols that holds the attribute 'DATA', in order to separate data omg from code img*/
void updateSymbols(int* ic, SymbolTable* symbolTable) {
	int symbolIndex;
	Symbol* tempSymbol = symbolTable->head;
	for (symbolIndex = 0; symbolIndex < symbolTable->numOfSymbols; symbolIndex++) {
		if (tempSymbol->attributes == DATA) {
			tempSymbol->base = getOffset(tempSymbol->value + (*ic));
			tempSymbol->value = tempSymbol->value + (*ic) - tempSymbol->base;
		}
		tempSymbol = tempSymbol->next;
	}
}

/*return the offset of a given number*/
int getOffset(int icf) {
	int offset = icf / 16;
	offset = icf - offset * 16;
	return offset;
}

/*return the number of digit of a given number*/
int countDigit(int number) {
	int digitCount = 0;
	if (number == 0)
		return 1;
	while (number != 0) {
		number /= 10;
		digitCount++;
	}
	return digitCount;
}

/*checks if a given string is valid and return length of a given string, without apostrophes. return -1 if invalid*/
short stringLength(char* dataLine) {
	int i, apostrophesIndex;
	bool isApostrophes = false;
	if (dataLine[0] != '"')
		return -1;
	for (i = 1; i < strlen(dataLine); i++) {
		if (isApostrophes == false && dataLine[i] == '"') {
			apostrophesIndex = i;
			isApostrophes = true;
		}
		else if (isApostrophes == true && !isspace(dataLine[i]))
			isApostrophes = false;
	}
	if (isApostrophes == true)
		return apostrophesIndex;
	return -1;
}

