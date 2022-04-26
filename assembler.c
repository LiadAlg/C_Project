#include "global.h"
#include "macroDeploy.h"
#include "firstPass.h"
#include "secondPass.h"
#include "writeFiles.h"
#include "globalFunctions.h"


int main(int argc, char* argv[]) {
	char* fileName_am;
	int i, IC, DC;
	FILE* file_am;
	/*intialize all linked lists*/
	SymbolTable symbolTable = { 0, NULL , NULL };
	CodeImg codeImg = { NULL, NULL };
	DataImg dataImg = { 0, NULL, NULL };
	ExternList externList = { 0, NULL, NULL };
	for (i = 1; i < argc; i++) {
		if (createAmFile(argv[i]) == true) {/*macro deployed successfully*/
			IC = 100;
			DC = 0;
			fileName_am = strcatChecked(argv[i], ".am");
			file_am = fopen(fileName_am, "r");
			if (file_am == NULL) {
				printf("Error opening the file %s\n", fileName_am);
				free(fileName_am);
			}
			else {
				if (firstPassProccess(file_am, &IC, &DC, &symbolTable, &codeImg, &dataImg, &externList) == true) {
					if (secondPassProccess(file_am, &IC, &DC, &symbolTable, &codeImg, &dataImg, &externList) == true) {
						if (writeFileProccess(argv[i], IC, DC, &symbolTable, &codeImg, &dataImg, &externList) == true)
							printf("Translated successfully!\n");/*in case all succeed*/
					}
				}
				fclose(file_am);
			}
			freeLinkedLists(&symbolTable, &codeImg, &dataImg, &externList, IC);
		}
		else if (i != argc - 1)
			printf("continue to next file\n");
	}
	printf("\nThank you for using my assembler! goodbye\n");
	return 0;
}


