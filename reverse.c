// Veeti Rajaniemi 290623

/* Help from:
    https://www.scaler.com/topics/fprintf-in-c/
	https://www.programiz.com/c-programming/library-function/string.h/strcmp
	https://c-for-dummies.com/blog/?p=1112
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>


struct lines
{
	char * string; // pointer to the text
	struct lines * prev; // pointers to prev and next element of linked list
	struct lines * next;
};


// Reads the lines of a file or stdout, prints them if printFile is 1, and stores the words to a linked list 
int read_lines(struct lines ** line, FILE * file, int printFile) // double pointer to struct element and pointer to file to read from
{
	struct lines * lp = NULL;
	struct lines * nextlp = NULL;
	char *word = NULL;
	size_t len = 0;

	while(getline(&word, &len, file) != -1) { // read a line
		if (printFile == 1) { // print if wanted (only input file given)
			fprintf(stdout, "%s", word);
		}

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
		
		if ( (lp->string = malloc(strlen(word) + 1)) == NULL) { 
			fprintf(stderr, "malloc failed\n");
			return -1;
		}
		
		strcpy(lp->string, word); // new item stored to the list
	}

	return 0;
}

// Prints the linked list values to screen or write to output file
void print_lines(struct lines * line, FILE * output)
{
	struct lines * lp;

	lp = line; // pointer to the head of linked list (now not changing the original line pointer)

	while (lp->next != NULL) { // to the end
		lp = lp->next;
	}

	while (lp != NULL) { // to the beginning
		fprintf(output, "%s", lp->string);
		lp = lp->prev;
	}
}

// Frees and deletes the linked list
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

int main(int argc, char **argv)
{
	struct lines * line = NULL; // linked list to store words
    FILE *file; // input
    FILE *opfile; // output
	int print = 0; // parameter to read_lines, prints the input file to stdout if 1
    
    if (argc >= 4) { // too many arguments
		fprintf(stderr, "usage: reverse <input> <output>\n");
		exit(1);
	}

	if (argc == 1) { // one input argument
        if (read_lines(&line, stdin, print) == -1) {
			exit(1);
			}
        print_lines(line, stdout);
    } else { // 2 or 3 parameters
		if ((file = fopen(argv[1], "r")) == NULL) { // input file handling
			fprintf(stderr, "error: cannot open file '%s'\n", argv[1]);
			exit(1);
		}
		if (argc == 3) {
			if (strcmp(argv[1],argv[2]) == 0) { // 3 parameters, check that input and output files not same
				fprintf(stderr, "Input and output file must differ\n");
				exit(1);
			}
			if (read_lines(&line, file, print) == -1) { 
				exit(1);
			}
			if ((opfile = fopen(argv[2], "a")) == NULL) { // output file handling
				fprintf(stderr, "error: cannot open file %s\n", argv[2]);
				exit(1);
			}
            print_lines(line, opfile); // write linked list words to output file
			fclose(opfile);
		} else { // 2 parameters
			print = 1; // read_lines now prints first input file normally
			if (read_lines(&line, file, print) == -1) {
				exit(1);
			}
			print_lines(line, stdout); // print linked list words to screen 
		}
		fclose(file);
	}
	delete_lines(line);
	return 0;
}
