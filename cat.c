#include<stdio.h>
#include<getopt.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<errno.h>
#include<sys/stat.h>

// cat clone 0.2.2 matthew wilson. dec 2016. 
// based on cat from GNU Coreutils 8.23. 
// License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>

/* guide to ops: from main, to vectorchecker, then to charcatcher if reading from stdin or to filechecker if checking on file. when in filechecker, any non-exist file (or any existing directory) will cause a permanent errcode to return a non-zero to main. in filechecker, it will open filereader to actually read lines from files. */

void charcatcher();
void filereader(); 
void filechecker();
int vectorchecker();
void usage();

char* program_name;
static int errcode = 0;
int n = 0, e = 0, b = 0;

void charcatcher(void) {
	int c;
	while ((c = getchar()) != EOF) {
		putchar(c);
	}
}

int vectorchecker(int argc, char*argv[]) {
	int x;
	for (x = 0; x < argc; x++) {
		if ((strcmp(argv[x], "-") == 0)) {
			charcatcher();
		}
		else {
			filechecker(argc, argv, x);
		}
	}
	// this returns to main the error code below. 
	// any missing file results in non-zero
	if (errcode == -1) {
		return(-1);
	}
	else {
		return(0);
	}
}

void filechecker(int argc, char*argv[], int x) {
	struct stat fileStat;
	if (access(argv[x], F_OK|R_OK) == -1) { 
		fprintf(stderr, "%s: %s: %s\n", program_name, argv[x], strerror(errno));
		errcode = -1;
	}
	// if file exists And is directory, throw the error
	else if ((stat(argv[x], &fileStat) == 0) && (S_ISDIR(fileStat.st_mode))) {
		//printf("error: %s is a directory\n", argv[x]);	
		fprintf(stderr, "%s: %s: is a directory\n", program_name, argv[x]);
		errcode = -1;
	}
	else {
		filereader(argv, x);	
	}
}

void filereader(char*argv[], int x) {
	int linecnt = 0; // lines of each file
	int metalinecnt = 1; // total lines of many files
	FILE *inputfile;
	char *lineitems[9999];
	char line[9999];
	int blnk = 0;

	inputfile = fopen(argv[x], "r");

	while (fgets(line, sizeof line, inputfile) != NULL) {
		lineitems[linecnt] = strdup(line);
		if (b == 1) {
			if (strcmp(lineitems[linecnt], "\n") == 0) {
				blnk++;
				if (e == 1) {
					printf("$");
				}		
				printf("\n");
				continue;
			}
		}
		//remove newline
		lineitems[linecnt][strlen(lineitems[linecnt]) - 1] = '\0';
		if (b == 1 && e == 1) {
			printf("%6d  %s$\n", metalinecnt, lineitems[linecnt]); 
		}
		else if (e == 1 && n == 0) {
			printf("%s$\n", lineitems[linecnt]); 
		}
		else if (e == 1 && n == 1) {
			printf("%6d  %s$\n", metalinecnt, lineitems[linecnt]); 
		}
		else if (n == 1 || b == 1) {
               		printf("%6d  %s\n", metalinecnt, lineitems[linecnt]); 
		}
		else {
			printf("%s\n", lineitems[linecnt]); 
		}
		linecnt++;
		metalinecnt++;
	}
	fclose(inputfile);
}

static int flag_help;

static struct option const long_options[] = 
{
    	{"number", no_argument, 0, 'n'}, 
    	{"show-ends", no_argument, 0, 'E'}, 
    	{"number-nonblank", no_argument, 0, 'b'},
	{"help", no_argument, &flag_help, 1}, 
    	{0, 0, 0, 0}			
};

void usage(void) {
	printf("concatenate files and print on standard output!\n");
	printf("%s usage: 'cat file1 file2 ... [-nEb]'\n", program_name);
	printf("[-n | --number] (number all output lines)\n[-E | --show-ends] (display dollar sign at end of each line)\n[-b | --number-nonblank] (number only non-empty output lines)\n");
	if (flag_help) {
		exit(EXIT_SUCCESS);
	}
	else { 
		exit(EXIT_FAILURE);
	}
}

int main (int argc, char* argv[]) {
	program_name = argv[0];
	int optc;
	int index;

	while ((optc = getopt_long (argc, argv, "nEb", long_options, (int *) 0)) != EOF) {
		switch (optc) {
	    		case 0:
		    	    break; 
	    		case 'n':
		    	    n = 1;
		    	    break;
	    		case 'E':
	            	    e = 1;
		    	    break;
	    		case 'b':
		    	    b = 1;
		    	    break;	
	    		case '?':
		    	    usage();
		    	    break;
	    		default:	
		    	    usage();
		    	    break;
		}
	}

	if (flag_help) {
		usage();
	}

	for (index = optind; index < argc; index++) {
		;
	}

	argc -= optind;
	argv += optind;

	if (argc == 0) {
		charcatcher();
	}
	else if (argc >= 1) {
		if (vectorchecker(argc, argv) != 0) {
			exit(EXIT_FAILURE);
		}
	}
	else {
 		usage();
	}
	exit(EXIT_SUCCESS);
}
