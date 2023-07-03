/* Help from:
    https://stackoverflow.com/questions/15697783/how-does-fread-know-when-the-file-is-over-in-c
*/

// Veeti Rajaniemi 030723

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Reads the zipped files and prints it unzipped 
int read_file(FILE *file) {
    char c[1]; 
    int d;
    
    while(fread(&d, sizeof(d), 1, file) == 1) { // first read the count integer (breaks if file ends)
        printf("%d", d); // print the count
        if (fread(c, sizeof(c), 1, file) == 1) { // then read the character
            printf("%s", c); // print the character
        }
        else { // if file ends (shouldn't end before the character), breaks 
            break;
        }
    }

    return 0;
} 


int main(int argc, char **argv) {
    FILE *file; // input file

    if (argc == 1) { // not file given
        fprintf(stderr, "my-unzip: file1 [file2 ...]\n");
        exit(1);
    }

    for (int i = 1; i < argc; i++) { // for each input file
        if ((file = fopen(argv[i], "r")) == NULL) {
            fprintf(stderr, "cannot open file '%s", argv[i]);
            exit(1);
        }
        read_file(file); // read and unzip file
        fclose(file); // close file
    }
    return 0;
}