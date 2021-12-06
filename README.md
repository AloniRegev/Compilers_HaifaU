# Haifa University Compilers Project
Exercise in building a compiler from C language to P-code language (assembly-like language for educational needs) from the Compilers course of the University of Haifa.


### Run me:
1. Navigate to the folder where you saved the project and select the part of the project that you want to run by logging in to one of the "Part" folders.
2. Compile the project by the following command: `gcc -o compiler ..\src\ctree.c ..\src\heap.c ..\src\nmetab.c ..\src\prnttree.c ..\src\symtab.c ..\src\token.c ..\src\tree.c ..\src\dsm_extension.c ..\src\treestk.c ..\src\lexer.c ..\src\gram.c CodeGenerator.c`
   - Note that the compiler file written is `compiler.exe`.

3. Run Commend`.\compiler.exe -Flag FileName.c`
   - FileName - The location of the C file you want to convert.
   - Flag - You can execute a command from the following using the appropriate flage:
     - Pcode - Print the P-code of C file.
     - AST - Print the AST tree of C file.
     - SymTab - Printing the Symbol Table of C file.
   
