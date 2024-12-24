#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <pwd.h>
#include <ctype.h>
#include <sys/wait.h>
#include <sys/stat.h>

//+
// File:	shell.c
//
// Pupose:	This program implements a simple shell program. It does not start
//		processes at this point in time. However, it will change directory
//		and list the contents of the current directory.
//
//		The commands are:
//		   cd name -> change to directory name, print an error if the directory doesn't exist.
//		              If there is no parameter, then change to the home directory.
//		   ls -> list the entries in the current directory.
//			      If no arguments, then ignores entries starting with .
//			      If -a then all entries
//		   pwd -> print the current directory.
//		   exit -> exit the shell (default exit value 0)
//				any argument must be numeric and is the exit value
//
//		if the command is not recognized an error is printed.
//-

#define CMD_BUFFSIZE 1024
#define MAXARGS 10

int splitCommandLine(char * commandBuffer, char* args[], int maxargs);
int doInternalCommand(char * args[], int nargs);
int doExternalCommand(char * args[], int nargs);

//+
// Function:	main
//
// Purpose:	The main function. Contains the read
//		eval print loop for the shell.
//
// Parameters:	(none)
//
// Returns:	integer (exit status of shell)
//-

int main() {

    static char commandBuffer[CMD_BUFFSIZE];
    // note the plus one, allows for an extra null
    char *args[MAXARGS+1];

    // print prompt.. fflush is needed because
    // stdout is line buffered, and won't
    // write to terminal until newline
    printf("%%> ");
    fflush(stdout);

    while(fgets(commandBuffer,CMD_BUFFSIZE,stdin) != NULL){
        //printf("%s",commandBuffer);

	// remove newline at end of buffer
	int cmdLen = strlen(commandBuffer);
	if (commandBuffer[cmdLen - 1] == '\n'){
		commandBuffer[cmdLen - 1] = '\0';
		cmdLen--;
	}

	// split command line into words.(Step 2)
	int nargs = splitCommandLine(commandBuffer, args, MAXARGS); // Number of arguments in the command buffer

	// add a null to end of array (Step 2)
	args[nargs] = NULL;


	// debugging
	//printf("%d\n", nargs);
	//int i;
	//for (i = 0; i < nargs; i++){
	//   printf("%d: %s\n",i,args[i]);
	//}
	// element just past nargs
	//printf("%d: %p\n",i, args[i]);

	if (nargs > 0){									// Check that there is at least one arg
	   int found_internal_command = doInternalCommand(args, nargs);			// Check if the given command has an implementation in this file
	   if (!found_internal_command){						// If the command doesn't have an implementation in this file | found_internal_command == 0
		   int found_external_command = doExternalCommand(args, nargs);		// Look through system for given command
		   if (!found_external_command){					// If given command was not found in the system | found_external_command == 0
			   fprintf(stderr, "ERROR:Command not found!\n");		// Prints error | command not found
		   }
	   }	   
	}
	else{
	   fprintf(stderr, "ERROR: Not enough arguments were given! | Need 1 or more!\n");	// Prints error too few args
	}
	
	// print prompt
	printf("%%> ");
	fflush(stdout);
    }
    return 0;
}

////////////////////////////// String Handling (Step 1) ///////////////////////////////////

//+
// Function:	skipChar
//
// Purpose:	This function skips over a given char in a string
//		For security, will not skip null chars.
//
// Parameters:
//    charPtr	Pointer to string
//    skip	character to skip
//
// Returns:	Pointer to first character after skipped chars
//		ID function if the string doesn't start with skip,
//		or skip is the null character
//-

char * skipChar(char * charPtr, char skip){
    
    if(skip == '\0'){			// If skip is the termination char '\0' then return the ptr to the str since it should be at the end of the sentence 
	    return charPtr;
    }

    for(; *charPtr == skip; charPtr++);	// Iterate through str ptr until it's not the same as skip
    
    return charPtr;
}

//+
// Funtion:	splitCommandLine
//
// Purpose:	Split the commandBuffer from the command line into individual words and storing ptrs to
// 		those words in the args array, while also returning the number of words/arguments in the
// 		commandBuffer
//
// Parameters:
//	@param commandBuffer: 	Sentence from command line to be split
//	@param args:		Array to store ptrs to the individual arguments from commandBuffer
//	@param maxargs:		The max number of arguments that can be stored in the args array
//
// Returns:	Number of arguments (< maxargs).
//
//-

int splitCommandLine(char * commandBuffer, char* args[], int maxargs){
   
   int word_count = 0;					// The number of arguments

   while (word_count < maxargs - 1){			// Loop until we reach the max number of allowed args (last one always has to be NULL hence -1)
      commandBuffer = skipChar(commandBuffer, ' ');	// Skip all spaces before the next char

      if (*commandBuffer == '\0'){ 			// If we reach the end of the commandBuffer break
	      break;
      }

      args[word_count++] = commandBuffer;		// Iterate through args

      commandBuffer = strchr(commandBuffer, ' ');	// Look for the next space char

      if (commandBuffer == NULL){ 			// Break if the command buffer is NULL
	      break;
      }

      *commandBuffer++ = '\0';				// Add NULL-char
   }

   if(word_count >= maxargs){				// Checks that the number of args doesn't exceed the max number allowed | if so print an error and return 0
      fprintf(stderr, "ERROR: The number of args has exceeded the upper limit!\n");
      return 0;
   }
   
   return word_count;
}

////////////////////////////// External Program  (Note this is step 4, complete doeInternalCommand first!!) ///////////////////////////////////

// list of directorys to check for command.
// terminated by null value
const char * const path[] = {
    ".",
    "/bin",
    "/usr/bin",
    NULL
};

//+
// Funtion:	doExternalCommand
//
// Purpose:	Looks through the system for the requested command implementation/execution
//
// Parameters:
//	@param args:	Array of command arguments | args[0] is the command to be executed
//	@param nargs:	Number of arguments in the args array
//
// Returns	int
//		1 = found and executed the file
//		0 = could not find and execute the file
//-

int doExternalCommand(char * args[], int nargs){
  char * cmd_path = NULL;							// Command path | str
  for(int i = 0; path[i] != NULL; i++){
	 cmd_path = (char *)malloc(strlen(path[i]) + strlen(args[0]) + 2); 	// Allocate memoryfor the command path | the +2 is for '/' and '\0' at the end
	 if (cmd_path == NULL){
		 fprintf(stderr, "ERROR: Command Path memory allocation failed!\n");
		 return 0;
	 }

	 sprintf(cmd_path, "%s/%s", path[i], args[0]);				// Copy path and given args to the command path

	 struct stat status;
	 int file_exists = stat(cmd_path, &status);				// Call stat struct using the command path | file_exists == 0 if it in fact exists
	 
	 // Check that file exists and that it is regular and executable
	 if (!file_exists && S_ISREG(status.st_mode) && (status.st_mode & S_IXUSR)){
		 break;
	 }

	 free(cmd_path);							// Free our command path memory
	 cmd_path = NULL;							// Update the command path ptr
  }
  
  if (cmd_path == NULL){							// If the command path is NULL we could not find and execute the file | return 0
	  return 0;
  }

  switch(fork()){
	  case 0: 								// If the current proc is the child
		  execv(cmd_path, args);					// Replace current proc with the command path program
		  break;
	  case -1:								// Failed to create the child proc | print error and return 0 
		  fprintf(stderr, "ERROR: Could not create child process!\n");
		  free(cmd_path);
		  return 0;
	  default: 								// If the proc is the parent proc then we wait :)
		  wait(NULL);							// Wait for the child to finish executing
		  break;
  }

  free(cmd_path);								// Again we gotta free our memory
  return 1;
 }

////////////////////////////// Internal Command Handling (Step 3) ///////////////////////////////////

// define command handling function pointer type
typedef void(*commandFunction)(char * args[], int nargs);

// associate a command name with a command handling function
struct cmdData{
   char 	* cmdName;
   commandFunction 	cmdFunc;
};

// prototypes for command handling functions
void exitFunc(char * args[], int nargs);
void pwdFunc(char * args[], int nargs);
void lsFunc(char * args[], int nargs);
void cdFunc(char * args[], int nargs);
int is_hidden(const struct dirent * d);

// list commands and functions
// must be terminated by {NULL, NULL} 
// in a real shell, this would be a hashtable.
struct cmdData commands[] = {
   {"exit", exitFunc},
   {"pwd", pwdFunc},
   {"ls", lsFunc},
   {"cd", cdFunc},
   { NULL, NULL}		// terminator
};

//+
// Function:	doInternalCommand
//
// Purpose:	Check the first element of args and check if it has an implementation in this file;
// 		if it does then execute the command if it doesnt then return 0
//
// Parameters:
//	@param args:	Array of command arguments
//	@param nargs:	Number of arguments in the args array
//
// Returns	int
//		1 = args[0] is an internal command
//		0 = args[0] is not an internal command
//-

int doInternalCommand(char * args[], int nargs){
    for(int i = 0; commands[i].cmdName != NULL; i++){
	    if (strcmp(commands[i].cmdName, args[0]) == 0){	// Checks if command is the same as the internal command
		    commands[i].cmdFunc(args, nargs);		// Call and exec the internall command function
		    return 1;
	    }
    }
    return 0;
}

///////////////////////////////
// comand Handling Functions //
///////////////////////////////

//+
// Function:	is_hidden
//
// Purpose:	Checks if the a file is hidden
//
// Parameters:
// 	@param d:	Ptr to the directory entry struct
//
// Returns	int
// 		1 = the file is hidden
// 		0 = the file is NOT hidden (wow)
// - 
int is_hidden(const struct dirent * d){
	return (d->d_name[0] == '.') ? 1 : 0;
}

//+
// Function:	exitFunc
//
// Purpose:	Exit the program shell
//
// Parameters:
// 	@param args:	Array of program arguments | should only be 1
// 	@param nargs:	Number of arguments in the args array | should be 1
//
// Returns:	void
//-
void exitFunc(char* args[], int nargs){
	if (nargs > 1){	// If more than one argument then print and error and do not execute the command
		fprintf(stderr, "ERROR: Exit has no arguments! | Expecting one word (exit)\n");
	}
	else{
		exit(0);
	}
	return;
}

//+
// Function:	pwdFunc
//
// Purpose:	Print the working directory
//
// Parameters:
// 	@param args:	Array of program arguments | should only be 1
// 	@param nargs:	Number of arguments in the args array | should only be 1
//
// Returns:	void
//-
void pwdFunc(char* args[], int nargs){
	if (nargs > 1){ // If more than one argument then print and error and do not execute the command
		fprintf(stderr, "ERROR: pwd has no arguments! | Expecting one word (pwd)\n");
	}
	else{		// print the working directory using the getcwd function
        	char * cwd = getcwd(NULL,0);
		printf("%s\n", cwd);
		free(cwd);
	}
        return;
}

//+
// Function:	lsFunc
//
// Purpose:	List the files in the current working directory | Option for hiddien files is available with '-a'
//
// Parameters:
// 	@param args:	Array of program arguments
// 	@param nargs:	Number of arguments in the args array
//
// Returns:	void
//-
void lsFunc(char* args[], int nargs){
        struct dirent ** namelist;
	int numEnts = scandir(".",&namelist,NULL,NULL);
	
	int list_hidden;					// 1 if we want to list hidden files 0 if we don't
	switch(nargs){
		case 1: list_hidden = 0;			// Just ls
			break;
		case 2: 					// ls -a | Show hidden files
			if(strcmp(args[1], "-a") != 0){		// Check that the second argument is allowed
				fprintf(stderr, "ERROR: Only allowed second parameter is \'-a\'!\n");
				return;
			}
			else{
				list_hidden = 1;
			}
			break;
		default:					// Too many arguments were given 
			fprintf(stderr, "ERROR: Invalid number of arguments provided! | Allowed:\'ls\' or \'ls -a\'\n");
			return;
	}

	for(int i = 0; i < numEnts; i++){
		if(!list_hidden && is_hidden(namelist[i])){	// Skips hidden files if list_hidden == 0 and is_hidden == 0
			continue;
		}
				
		printf("%s\n", namelist[i]->d_name);		// Prints the hidden file
	}

        return;
}

//+
// Function:	cdFunc
//
// Purpose:	Changes the directory
// 		cd: 		goes back to the home directory
// 		cd [path]: 	send you to that directory
//
// Parameters:
// 	@param args:	Array of program arguments
//	@param nargs:	Number of arguments in the args array
//
// Returns:	void
//-
void cdFunc(char* args[], int nargs){
        struct passwd *pw = getpwuid(getuid());				// Get the current users pswrd entry | has info like the home dir
	if (pw == NULL){						// Check if getpwuid failed
		fprintf(stderr, "ERROR: Failed to get the PWUID!\n");
		return;
	}
	
	char * target_dir;						// Ptr to target dir
	switch(nargs){
		case 1: target_dir = pw->pw_dir;			// Given one parameter | go to the home dir
			break;
		case 2: target_dir = args[1];				// Given two parameters | go to the requested dir
			break;
		default:						// If more than 2 arguments are given then print and error and do not execute
			fprintf(stderr, "ERROR: Invalid number of parameters given! | Expection 1 or 2!\n");
			return;
	}
	
	int changed_directory = chdir(target_dir);			// Change from the current working dir to the requested dir
	if (changed_directory != 0){					// If changing dir failed then print an error and return
		fprintf(stderr, "Was unable to change directory!\n");
		return;
	}

        return;
}
