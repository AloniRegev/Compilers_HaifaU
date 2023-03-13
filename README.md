# Haifa University Compilers Project
A compiler that converts C language code into P-code language, which is an assembly-like language used for educational purposes. This compiler was developed as part of the Compilers course at the University of Haifa.


### Run me:
1. Navigate to the folder where you saved the project and select the part of the project that you want to run by logging in to one of the "Part" folders.

- If you like to Compile the project by yourself use the following command: `gcc -o compiler ..\src\ctree.c ..\src\heap.c ..\src\nmetab.c ..\src\prnttree.c ..\src\symtab.c ..\src\token.c ..\src\tree.c ..\src\dsm_extension.c ..\src\treestk.c ..\src\lexer.c ..\src\gram.c CodeGenerator.c`
  - Note that the compiler file written is `compiler.exe`.

2. Run Commend`.\compiler.exe -Flag FileName.c`
   - FileName - The location of the C file you want to convert.
   - Flag - You can execute a command from the following using the appropriate flage:
     - Pcode - Print the P-code of C file.
     - AST - Print the AST tree of C file.
     - SymTab - Printing the Symbol Table of C file.

### Output:
- You can compile the P-code output obtained from the compiler at the [link](https://mw10.haifa.ac.il/pluginfile.php/164232/mod_assign/intro/Pmachine.html).
- You can check the desired output from the additional test files at the [link](https://text-compare.com).   
## Author:
- [Regev Aloni](https://github.com/AloniRegev)
