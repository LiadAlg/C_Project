# C_Project

# about
This project is an assignment in a C programming course at the open unvieristy. The purpose of this project is to build an assembler for 16-instruction asssembly language, for an imaginary 20-bit CPU and 20 KB of memory size. 

# Source structure (Modules)
1. **assembler** - main function definition, argv & argc processing, single file processing.
2. **first_pass** - contains functions for processing a single line in the first pass and a code line in the first pass.
3. **second_pass** - contains function for processing a single line in second pass and replacing symbol by it's address.
4. **globalFunctions** - contains useful functions for processing code.
5. **macroDeploy** - contains useful function for deploy all macros.
6. **global.h** - contains type and constant definitions.
7. **writefiles** - contains methods for writing output files: *.ob, *.ext and *.ent
