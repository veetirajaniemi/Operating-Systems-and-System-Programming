/* Help from:
    https://www.scaler.com/topics/strstr-function-in-c/
*/

// Veeti Rajaniemi 300623

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Linked list struct
struct lines
{
	char * string; // pointer to the text
	struct lines * prev; // pointers to prev and next element of linked list
	struct lines * next;
};

// Reading a file and storing items to the linked list
int read_file(FILE *file, char *str, struct lines ** line) {
    char *fileline = NULL;
	size_t len = 0;
    char *found = "";
    struct lines * lp = NULL;
	struct lines * nextlp = NULL;

    while(getline(&fileline, &len, file) != -1) { // read a line
        if (strcmp(str, "") == 0) {
            continue; // empty lines are decided not to match any lines
        }
        if ((found = strstr(fileline, str)) != NULL) { // string found from the file --> added to the linked list
            if (*line == NULL) { // if this is the first item, (line = the full linked list)
                if ((*line = lp = malloc(sizeof(struct lines))) == NULL) {
                    fprintf(stderr, "malloc failed\n");
                    return -1;
                }
                lp->prev = lp->next = NULL;

		    } else { // not the first item
                if ( (lp->next = malloc(sizeof(struct lines))) == NULL) {
                    fprintf(stderr, "malloc failed\n");
                    return -1;
                }
                nextlp = lp->next; // set the new item to the list
                nextlp->prev = lp; // set current item as previous to new one
                lp = nextlp; // set new one as "lp"
                }
    
            if ( (lp->string = malloc(strlen(fileline) + 1)) == NULL) { 
                fprintf(stderr, "malloc failed\n");
                return -1;
            }
            
            strcpy(lp->string, fileline); // new line stored to the list

        }
    }
    if (*line == NULL) { // if line is still null, it doesnt need to be deleted!
        return -1;
    } else { // line is not null, it will be deleted and freed!
        return 0; 
    }
}

// Prints the linked list values to screen or write to output file
void print_lines(struct lines * line, FILE * output)
{
	struct lines * lp;

	lp = line; // pointer to the head of linked list (now not changing the original line pointer)
    fprintf(output, "%s", lp->string);
	while (lp->next != NULL) { // to the end
		lp = lp->next;
        fprintf(output, "%s", lp->string);
	}
}

// Deleting and freeing the linked list!
void delete_lines(struct lines * line)
{
	struct lines * lp;
	lp = line;
	while (lp != NULL) {
		line = lp->next;
		free(lp->string);
		free(lp);
		lp = line;
	}
}



int main(int argc, char **argv) {
    FILE *file; // file to search from

    if (argc == 1) { // not enough arguments
        fprintf(stderr, "my-grep: searchterm [file ...]\n");
        exit(1);
    }

    if (argc == 2) { // read from standard input
        struct lines * line = NULL; // linked list to store values of a file
        if (read_file(stdin, argv[1], &line) == 0) {
            print_lines(line, stdout);
            delete_lines(line); 
        }
    }

    for (int i = 2; i < argc; i++) { // read each file
        struct lines * line = NULL; // new linked list to each file
        if((file = fopen(argv[i], "r")) == NULL) {
            fprintf(stderr, "my-grep: cannot open file '%s'\n", argv[i]);
            exit(1);
        }
        if (read_file(file, argv[1], &line) == 0) {
            print_lines(line, stdout);
            delete_lines(line);
        }
        fclose(file);

    }
    return 0;
}