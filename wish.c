/* Help from:
    Toni Koskinen - structure of code (exec, fork and wait -commands), redirection
    https://stackoverflow.com/questions/42795727/add-string-to-array-c-pointers
    https://www.geeksforgeeks.org/chdir-in-c-language-with-examples/
    https://linuxhint.com/c-execve-function-usage/
    https://stackoverflow.com/questions/41884685/implicit-declaration-of-function-wait
    https://stackoverflow.com/questions/19099663/how-to-correctly-use-fork-exec-wait
    https://www.geeksforgeeks.org/wait-system-call-c/
    https://stackoverflow.com/questions/122616/how-do-i-trim-leading-trailing-whitespace-in-a-standard-way
    https://stackoverflow.com/questions/41830461/allocating-string-with-malloc
    https://www.geeksforgeeks.org/strdup-strdndup-functions-c/
    https://stackoverflow.com/questions/1720535/practical-examples-use-dup-or-dup2
*/

// Veeti Rajaniemi 230723


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <ctype.h>
#include <fcntl.h>

// Function to make cd commands
void cd (char *cmdname) {
    cmdname = strtok(0, " "); 
    if (cmdname == 0) { // only "cd" nothing else given
        char error_message[30] = "Error: Give a directory!\n";
        write(STDERR_FILENO, error_message, strlen(error_message));
    } else {
        char *cmd2 = strtok(0, " ");
        if (cmd2 != 0) { // too many args after "cd" 
            char error_message[40] = "Error: Too many directories given!\n";
            write(STDERR_FILENO, error_message, strlen(error_message));
        }
        else {
            if (chdir(cmdname) != 0) { // changing dir, if doesn't return 0, invalid dir
                char error_message[30] = "Error: Ivalid directory!\n";
                write(STDERR_FILENO, error_message, strlen(error_message));
            }
        }
    }
}

int main(int argc, char **argv) {
    char *inputline = NULL;
    char *path[20] = {NULL}; // array for paths, max 20
    char **pptr = path;
    pptr[0] = "/bin"; // default path
    int paths = 1; // path count
    int args = 0; // arg count
    size_t len = 0;
    FILE *file;

    if (argc > 2) { // too many input arguments
        char error_message[40] = "Error: Too many input arguments!\n";
        write(STDERR_FILENO, error_message, strlen(error_message));
        exit(1);
    } else if (argc == 1) { // interactive shell
        args = 1;
    } else { // shell with batch input
        if((file = fopen(argv[1], "r")) == NULL) {
            char error_message[40] = "Error: Invalid input file!\n";
            write(STDERR_FILENO, error_message, strlen(error_message));
            exit(1);
        }
    }

    while(1) { // functionality of the shell within the loop

        if (args == 1) { // interactive mode, input from stdin
            printf("wish> ");
            if (getline(&inputline, &len, stdin) == -1) {
                exit(0);
            } 
        } else { // input from batch file
            if (getline(&inputline, &len, file) == -1) {
                exit(0);
            }
        }
        char *input = strdup(inputline); // copy of the input line to use

        // removing whitespace from the beginning of the input
        while (isspace(*input) != 0) {
                    input++;
                }
                char *end = input + strlen(input) - 1;
                while(end > input && isspace(*end)) {
                    end--;
                }
                end[1] = '\0';

        // exit command exits the shell
        if ((strcmp(input, "exit\n") == 0) | (strcmp(input, "exit") == 0)) {
            exit(0);
        }

        // empty inputs -> new input
        if ((strcmp(input, "\n") == 0) | (strcmp(input, "") == 0)) {
            continue;
        }

        // initializing variables
        char *allcmds[10] = {NULL}; // max 10 commands on one line
        char *cmds = NULL;
        cmds = strtok(input, "\n"); // input without \n
        char *manycmd = NULL;
        char *onecmd = NULL;
        int cmdcount = 0; // count of commands in input line

        manycmd = strchr(cmds, '&'); 
        
        if (manycmd != NULL) { // if '&' included
            onecmd = strtok(cmds, "&"); // first command
            while (onecmd != NULL) { // loop while new command
                while (isspace(*onecmd) != 0) { // remove whitespace from beginning and end
                    onecmd++;
                }
                char *end = onecmd + strlen(onecmd) - 1;
                while(end > onecmd && isspace(*end)) {
                    end--;
                }
                end[1] = '\0';

                allcmds[cmdcount] = onecmd; // add command to command list
                cmdcount += 1; // update command count
                onecmd = strtok(0, "&"); // parse commands
            }
        } else { // only one command

            while (isspace(*cmds) != 0) { // remove whitespace from beginning and end
                    cmds++;
                }
                char *end = cmds + strlen(cmds) - 1;
                while(end > cmds && isspace(*end)) {
                    end--;
                }
                end[1] = '\0';
            cmdcount = 1;
        }


        if (cmdcount == 1) { // only one command
            char *cmdcall = NULL;
            cmdcall = malloc(strlen(cmds)+1);
            strcpy(cmdcall, cmds); // copy the whole command with arguments to cmdcall
            char *cmdname = NULL;
            cmdname = strtok(cmds, " "); // command without arguments

            char *cmdrd = NULL;
            char *isrd = NULL;
            char *fname = NULL;
            int isredir = 0; // redirecting flag
            cmdrd = strdup(cmdcall); // new copy of cmdcall 
            isrd = strchr(cmdrd, '>'); // checking if there is redirecting

            if (isrd != NULL) {
                isredir = 1; 
                cmdcall = strtok(cmdrd, ">"); // command 
                fname = strtok(0, " ");
                if (fname == NULL) { // no output file given
                    char error_message[40] = "Error: No output file given!\n";
                    write(STDERR_FILENO, error_message, strlen(error_message));
                    continue;
                }
                // remove whitespace from end of the command
                char *end = cmdcall + strlen(cmdcall) - 1;
                while(end > cmdcall && isspace(*end)) {
                    end--;
                }
                end[1] = '\0';
                
            }

            if (strcmp(cmdname, "path") == 0) { // built-in path
                cmdname = strtok(0, " "); // path name
                
                paths = 0;
                for (int i = 0; i < 10; i++) {
                        if (pptr[i] == NULL) {
                            break;
                        }
                        pptr[i] = NULL;
                    }

                while(cmdname != 0) { // adding all paths to the list 
                        pptr[paths] = cmdname; 
                        paths += 1; // update count
                        cmdname = strtok(0, " ");
                    }
            } else if (strcmp(cmdname, "cd") == 0) { 
                cd(cmdname); // built-in cd-function

            } else { // any other command

                if (paths == 0) { // if no paths are given, can't execute commands!
                    char error_message[30] = "Error: No paths given!\n";
                    write(STDERR_FILENO, error_message, strlen(error_message));
                    continue;
                }

                char *comall;
                char *com;
                char *args[10] = {NULL}; // list of arguments
                char **pargs = args;
                comall = strtok(cmdcall, " ");
                com = malloc(strlen(comall)+1);
                strcpy(com, comall);
                int argcount = 0;

                while(comall != NULL) { // add arguments to the list, first is the name of command
                    pargs[argcount] = comall;

                    comall = strtok(NULL, " ");
                    argcount += 1;
                }


                for (int i = 0; i < paths; i++) { // go through path list
                    char curpath[255] = "";
                    snprintf(curpath, sizeof(curpath), "%s/%s", pptr[i], com); // create the current path to search command from
             
                    if (access(curpath, X_OK) == 0) { // command found from path
                        int pid = fork(); 
                        if (pid == 0) { // child process
                            if (isredir == 1) { // if redirection needed
                                int fd;
                                // opening the file to redirect to with needed rights
                                if ((fd = open(fname, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR)) == -1) {
                                    exit(0);
                                }
                                dup2(fd, 1); // redirect output to file
                                dup2(1, 2); // redirect errors to file
                                close(fd); // close the redirection file
                            } 
                            
                            if (execv(curpath, args) == -1) { // execute command
                                char error_message[40] = "Error: Can't execute command!\n";
                                write(STDERR_FILENO, error_message, strlen(error_message));
                            }  else {
                                exit(0); // exit after execution
                            }
                        }
                        wait(NULL); // wait child process
                        break;
                    } else if (i == paths-1) { // all paths tried, no file found
                        char error_message[50] = "Error: Command not found! Check command and path.\n";
                        write(STDERR_FILENO, error_message, strlen(error_message));
                    }
                }
                free(com);
            }
            free(cmdcall);
            

        } else { // many commands
            int forks = 0; // child process count
            for (int i = 0; i < cmdcount; i++) { // looping through all commands
                char *cmdcall;
                cmdcall = malloc(strlen(allcmds[i])+1);
                strcpy(cmdcall, allcmds[i]);
                char *cmdname = strtok(allcmds[i], " ");

                char *cmdrd = NULL;
                char *isrd = NULL;
                char *fname = NULL;
                int isredir = 0;
                cmdrd = strdup(cmdcall);
                isrd = strchr(cmdrd, '>'); // checking redirection

                if (isrd != NULL) { // if redirection, check file name and remove whitespace from command
                    isredir = 1;
                    cmdcall = strtok(cmdrd, ">");
                    fname = strtok(0, " ");
                    char *end = cmdcall + strlen(cmdcall) - 1;
                    while(end > cmdcall && isspace(*end)) {
                        end--;
                    }
                    end[1] = '\0';
                }

                if ((strcmp(cmdname, "exit") == 0) | (strcmp(cmdname, "exit\n") == 0)) { // exit the program
                    for (int i = 0; i < forks; i++) { // wait for possible child processes 
                        wait(NULL);
                    } 
                    exit(0);
                }

                if (strcmp(cmdname, "path") == 0) { // built-in path
                    cmdname = strtok(0, " ");
                    paths = 0;
                    for (int i = 0; i < 10; i++) { // empty path
                            if (pptr[i] == NULL) {
                                break;
                            }
                            pptr[i] = NULL;
                        }

                    while(cmdname != 0) { // add new paths
                            pptr[paths] = cmdname; 
                            paths += 1; // update path count
                            cmdname = strtok(0, " ");
                        } 

                } else if (strcmp(cmdname, "cd") == 0) { // built-in cd

                    cd(cmdname);

                } else {

                    if (paths == 0) { // if no paths are given, can't execute commands!
                        char error_message[30] = "Error: No paths given!\n";
                        write(STDERR_FILENO, error_message, strlen(error_message));
                        continue;
                    }

                    char *comall;
                    char *com;
                    char *args[10] = {NULL};
                    char **pargs = args;
                    comall = strtok(cmdcall, " ");
                    com = malloc(strlen(comall)+1);
                    strcpy(com, comall);


                    int argcount = 0;

                    while(comall != NULL) { // add all arguments, command first

                        pargs[argcount] = comall;

                        comall = strtok(NULL, " ");
                        argcount += 1;
                    }


                    for (int i = 0; i < paths; i++) { // go through all paths
                        char curpath[255] = "";
                        snprintf(curpath, sizeof(curpath), "%s/%s", pptr[i], com); // create path to search from

                        if (access(curpath, X_OK) == 0) { // command found from path
                            
                            pargs[argcount] = NULL;
                            forks += 1;
                            int pid = fork();
                            if (pid == 0) { // child process
                                if (isredir == 1) { // possible redirection
                                int fd;

                                if ((fd = open(fname, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR)) == -1) {
                                    char error_message[45] = "Error: Can't open the redirection file!\n";
                                    write(STDERR_FILENO, error_message, strlen(error_message));
                                    exit(0);
                                }
                                dup2(fd, 1);
                                dup2(1, 2);
                                close(fd);
                                } 
                                if (execv(curpath, args) == -1) { // execute command
                                    char error_message[40] = "Error: Can't execute command!\n";
                                    write(STDERR_FILENO, error_message, strlen(error_message));
                                }  else {
                                    exit(0);
                                }
                            }
                            break;
                        } else if (i == paths-1) { // command not found
                            char error_message[50] = "Error: Command not found! Check command and path.\n";
                            write(STDERR_FILENO, error_message, strlen(error_message));
                        }
                    }
                    free(com);
                }
                free(cmdcall);
            }
            for (int i = 0; i < forks; i++) { // wait child processes
                wait(NULL);
            }
        }

    }
    return 0;
}