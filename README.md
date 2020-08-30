# encryptor


Code is still in POC mode it is in rough shape. 

The idea is that we can take arbitrary C file, and mark certain functions as encrypted. This is simply
done by adding the directive 'ENCRYPTED_FUNCTION' at the top of the function. Ideally this would work
when declared anywhere within the function, but I first need to get the POC working. After declaring
a method as 'ENCRYPTED_FUNCTION', a special build process will be undertaken in which an ecnrypted 
program can run by decrypting each of its methods as they are called.


Basic workflow is describe in 'build.sh'
1. The file is compiled into assembly code
2. The php script(dont ask why...) from assemblyModification folder is run against the assembly, adding labels where appropriate to aid the decrypting function
3. The assmebly file is assembled, making sure to use the '-g -L' flags to preserve the labels in the ELF symbol table
4. Next the program in binaryModification is run on the object file that was output in step 3. This program actually encrypts the sections marked as encryptable using the encryption routine
5. The object file is now linked into a running executable.
6. Finally the program in elfeditor is run against the executable. This marks the relevant text sections as writeable, otherwise the the decryption process would never work.



Future work:
As a colleague of mine pointed out, if the idea is to make the program unidentifiable by scanners, then each encrypted version of the program needs to be encrypted differently. Of course each version of the program must also be able to decrypt itself. This is much more interesting work than the POC, but the POC needs to work first.






Forgive my C trangressions! I am a Java programmer by nature

