#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pwd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>

//=====define=====
#define TRUE 1
#define STD_INPUT 0
#define STD_OUTPUT 1
#define MAXLINE 4096
#define MAXARG 20  //the maximum number of arguments

#define STRUCT_PARSE_INFO
#define BACKGROUND			1
#define IN_REDIRECT			2
#define OUT_REDIRECT		4
#define OUT_REDIRECT_APPEND	8
#define IS_PIPED			16

//=====declare=====
char *buffer;
char *command = NULL;
char **parameters;
int ParaNum;		 //# parameters
struct parse_info info;
struct passwd *pwd;  //information about a user account
struct parse_info {
    int flag;		 //indicates what features are used
    char* in_file;	 //inputfile
    char* out_file;  //outputfile
    char* command2;
	char** parameters2;
};

//=====function=====
void type_prompt();  //display prompt on the screen
int read_command(char **command, char **parameters);  //read input from terminal
int parse_info_init(struct parse_info *info);  //for initialization
int parsing(char **parameters, int ParaNum, struct parse_info *info);  //parsing & setting
void myexit(char *command);  //exit, quit

//=====main=====
int main(void) {
	int status;
	pid_t ChdPid1,ChdPid2;
	parameters = malloc(sizeof(char*)*(MAXARG+2));
	buffer = malloc(sizeof(char) * MAXLINE);
	if(parameters == NULL || buffer == NULL) {  //malloc failed
        printf("error: malloc failed.\n");
        exit(0);
    }

	while(TRUE) {  //repeat forever
		int fd[2];  //0 for read, 1 for write
		int in_fd,out_fd;

		type_prompt();  //display prompt on the screen
		ParaNum = read_command(&command, parameters);  //read input from terminal
		if(ParaNum == -1) {
			continue;
		}
		--ParaNum;  //count of units in buffer
		parsing(parameters, ParaNum, &info);  //parsing & setting
		myexit(command);  //for exit, quit

		if(info.flag & IS_PIPED) {  //command2 is not null. (pipe)
            if(pipe(fd) < 0) {  	//create a pipe
                printf("myshell error: pipe failed.\n");
                exit(0);
            }
        }  
        if((ChdPid1=fork()) != 0) {  //myshell (parent)
            if(info.flag & IS_PIPED) {
                if((ChdPid2=fork()) == 0) {  //command2. The pipe process must be a child process for the myshell process
                    close(fd[1]);		//doesn't need to write to pipe
                    close(STD_INPUT);	//prepare for new standard input
                    dup(fd[0]);			//set standard input to fd[0]
                    close(fd[0]);		//thsi file descriptor not needed any more
printf("---\n");
                    execvp(info.command2, info.parameters2);  //execute command
                }
                else {
                    close(fd[0]);
                    close(fd[1]);
//printf("@@@@\n");
                    waitpid(ChdPid2,&status,0);  //wait command2
                }
            }

            if(info.flag & BACKGROUND) {  //run in the background
                printf("Child pid:%u\n", ChdPid1);
            }
            else {        
                waitpid(ChdPid1, &status, 0);  //wait command1
            } 
        }
        else {  //command1. myshell's child process
            if(info.flag & IS_PIPED) {  //command2 is not null              
                if(!(info.flag & OUT_REDIRECT) && !(info.flag & OUT_REDIRECT_APPEND)) {  //only piped
                    close(fd[0]);		//doesn't need to read from pipe
                    close(STD_OUTPUT);	//prepare for new standard output
                    dup(fd[1]);			//set standard output to fd[1]
                    close(fd[1]);		//this file descriptor not needed any more
                }
                else {  //OUT_REDIRECT and PIPED
                    close(fd[0]);
                    close(fd[1]);  //send a EOF to command2
                    if(info.flag & OUT_REDIRECT) {
    	               out_fd = open(info.out_file, O_WRONLY|O_CREAT|O_TRUNC, 0666);
					}
                    else {
    	               out_fd = open(info.out_file, O_WRONLY|O_APPEND|O_TRUNC, 0666);
					}
                    close(STD_OUTPUT); 
                    dup2(out_fd, STD_OUTPUT);
                    close(out_fd);	        
                }
            }
            else {
                if(info.flag & OUT_REDIRECT) {  //OUT_REDIRECT without pipe
                    out_fd = open(info.out_file, O_WRONLY|O_CREAT|O_TRUNC, 0666);
                    close(STD_OUTPUT); 
                    dup2(out_fd, STD_OUTPUT);
                    close(out_fd);
                }
				if(info.flag & OUT_REDIRECT_APPEND) { // OUT_REDIRECT_APPEND without pipe
                    out_fd = open(info.out_file, O_WRONLY|O_CREAT|O_APPEND, 0666);
                    close(STD_OUTPUT); 
                    dup2(out_fd, STD_OUTPUT);
                    close(out_fd);
				}
            }
            
            if(info.flag & IN_REDIRECT) {  //IN_REDIRECT
				//0666 means that all users can read and write but cannot execute
                in_fd = open(info.in_file, O_CREAT |O_RDONLY, 0666);
                close(STD_INPUT);
                dup2(in_fd, STD_INPUT);
                close(in_fd); 
            }
            execvp(command, parameters);  //execute command
        }
    }
    free(parameters);
	free(buffer);
}

//=====function=====
void type_prompt() {  //display prompt on the screen
	const int host_len = 256;
	const int path_len = 1024;
	char hostname[host_len], pathname[path_len];

	pwd = getpwuid(getuid());	//get user information
	getcwd(pathname, path_len);	//get the path

	if(gethostname(hostname, host_len) == 0) {
        printf("[myshell]%s@%s:", pwd->pw_name, hostname);
	}
    else {
		printf("[myshell]%s@unknown:", pwd->pw_name);
	}

	printf("%s",pathname);

 	if(geteuid() == 0) {  //root
        printf("# ");
	}
    else {
		printf("$ ");
	}
}

/*
*	return value: number of parameters
*	0 represents only command without any parameters
*	-1 represents wrong input
*/
int read_command(char **command, char **parameters) {
	if(fgets(buffer, MAXLINE, stdin) == NULL) {  //read input from terminal
		printf("\n");
		exit(0);
	}
    if(buffer[0] == '\0') {
        return -1;
	}

	char *pStart,*pEnd;
    int count = 0;
	int isFinished = 0;
    pStart = pEnd = buffer;

    while(isFinished == 0) {
        while((*pEnd == ' ' && *pStart == ' ') || (*pEnd == '\t' && *pStart == '\t')) {  //ignore spaces and tab
            ++pStart;
            ++pEnd;
        }

        if(*pEnd == '\0' || *pEnd == '\n') {  //terminate character
            if(count == 0) {
                return -1;
			}
            break;
        }

        while(*pEnd != ' ' && *pEnd != '\0' && *pEnd != '\n') {
            ++pEnd;
		}

        if(count == 0) {
            char *p = pEnd;
            *command = pStart;  //command stored in @command

            while(p!=pStart && *p !='/') {
                p--;
			}
            if(*p == '/') {
                ++p;
			}
            parameters[0] = p;  //parameters stored in @parameters
            count += 2;
        }
        else if(count <= MAXARG) {
            parameters[count-1] = pStart;
            ++count;
        }
        else {
            break;
        }

        if(*pEnd == '\0' || *pEnd == '\n') {  //terminate character
            *pEnd = '\0';
            isFinished = 1;
        }
        else {
            *pEnd = '\0';
            ++pEnd;
			pStart = pEnd;
        }
    }

    parameters[count-1] = NULL;

	return count;
}

//for initialization
int parse_info_init(struct parse_info *info) {
    info->flag = 0;			//indicates what features are used
    info->in_file = NULL;	//inputfile
    info->out_file = NULL;	//outputfile
    info->command2 = NULL;
    info->parameters2 = NULL;

    return 0;
}

//parsing & setting
int parsing(char **parameters, int ParaNum, struct parse_info *info) {
    int i;
    parse_info_init(info);

    if(strcmp(parameters[ParaNum-1], "&") == 0) {  //command run in the background
        info->flag |= BACKGROUND;
        parameters[ParaNum-1] = NULL;
        --ParaNum;
    }
    for(i = 0; i < ParaNum; ) {
        if(strcmp(parameters[i],"<<")==0 || strcmp(parameters[i],"<")==0) {  //IN_REDIRECT
            info->flag |= IN_REDIRECT;
            info->in_file = parameters[i+1];  //filename
            parameters[i] = NULL;
            i += 2;
        }
        else if(strcmp(parameters[i], ">") == 0) {  //OUT_REDIRECT
            info->flag |= OUT_REDIRECT;
            info->out_file = parameters[i+1];  //filename
            parameters[i] = NULL;
            i += 2;
        }
		else if(strcmp(parameters[i], ">>") == 0) {
            info->flag |= OUT_REDIRECT_APPEND;
            info->out_file = parameters[i+1];
            parameters[i] = NULL;
            i += 2;
		}
        else if(strcmp(parameters[i], "|") == 0) {  //between process (pipe)
            char *pCh;
            info->flag |= IS_PIPED;
            parameters[i] = NULL;
            info->command2 = parameters[i+1];
            info->parameters2 = &parameters[i+1];
            for(pCh = info->parameters2[0]+strlen(info->parameters2[0]);
                    pCh!=&(info->parameters2[0][0]) && *pCh!='/'; pCh--);  //check '/'
            if(*pCh == '/') {
                ++pCh;
			}
            info->parameters2[0] = pCh;
            break;
        }
        else {
            ++i;
		}
    }

//checking

	printf("\nbackground:%s\n", info->flag&BACKGROUND?"yes":"no");
	printf("in redirect:");
	info->flag&IN_REDIRECT?printf("yes, file:%s\n", info->in_file):printf("no\n");
	printf("out redirect:");
	info->flag&OUT_REDIRECT?printf("yes, file:%s\n", info->out_file):printf("no\n");
	printf("pipe:");
	info->flag&IS_PIPED?printf("yes, command:%s\n\n", info->command2):printf("no\n\n");

    return 1;
}

//exit, quit
void myexit(char *command){
	if(strcmp(command,"exit")==0 || strcmp(command,"quit")==0) {
		exit(0);  //exit
	}
}

