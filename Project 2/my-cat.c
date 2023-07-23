#include <stdio.h>
#include <stdlib.h>

#define MAXLEN 1024


// This function reads the file and prints it
void read_file(FILE *file) {
    char line[MAXLEN];

    while(fgets(line, MAXLEN, file) != NULL) {
        printf("%s", line);
    }
}

int main(int argc, char **argv) {
    FILE *file;

    if (argc == 1) { // no file to print
        exit(0);
    }

    for (int i=1; i<argc; i++) { // for every file given

        if ((file = fopen(argv[i], "r")) == NULL) {
            fprintf(stderr, "my-cat: cannot open file '%s'\n", argv[i]); 
            exit(1);
        }
        read_file(file); // reading and printing the file
        fclose(file);
    }
    return 0;
} 