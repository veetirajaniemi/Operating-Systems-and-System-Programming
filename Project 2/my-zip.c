/* Help from:
    https://www.fresh2refresh.com/c-programming/c-file-handling/fgetc-function-c/
*/

// Veeti Rajaniemi 030723

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Reads the file and writes a zipped one
int read_file(FILE *file, FILE *output) {
    char c;
    char cc;
   

    if ((cc = fgetc(file)) == EOF) { // first char of file to cc, return if empty file
        return 0;
    }

    int count = 1; // count 1 after first char
    
    while(1) {
        if ((c = fgetc(file)) == EOF) { // read new char to c
            fwrite(&count, sizeof(int), 1, output); // if file ended, write last count and char
            fwrite(&cc, sizeof(char), 1, output);
            break;
        } 
        if (c == cc) { // if new char is same than last one, update count
            count += 1;
        }
        else { // new char is different, first write the count of prev char and the char
            fwrite(&count, sizeof(int), 1, output);
            fwrite(&cc, sizeof(char), 1, output);
            count = 1; // count of new char now 1
            cc = c; // update cc to the new char for comparison
        }

    }

    return 0;
}

int main(int argc, char **argv) {
    FILE *file; // file to zip

    if (argc == 1) { // not file given
        fprintf(stderr, "my-zip: file1 [file2 ...]\n");
        exit(1);
    }

    for (int i = 1; i < argc; i++) { // for each input file
        if ((file = fopen(argv[i], "r")) == NULL) {
            fprintf(stderr, "cannot open file '%s", argv[i]);
            exit(1);
        }
        read_file(file, stdout); // read the file and zip
        fclose(file); // close the file
    }
    return 0;
}