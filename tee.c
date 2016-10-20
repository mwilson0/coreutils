#include<stdio.h>
#include<sys/stat.h>
#include<string.h>
#include<getopt.h>
#include<unistd.h>
#include<errno.h>
#include<stdlib.h>
#include<signal.h>
#include<fcntl.h>

// TEE clone 0.2. MWilson, Oct 2016.
// based on Tee from GNU Coreutils. 
// License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>

// order of ops: main to filechecker. get actual files and go to charcatcher. after every line entered, go to filewriter.

int a = 0;
int ii = 0;
static int errcode = 0;
static int repeatchars = 0;

filechecker(int argc, char*argv[]) {
	struct stat sb;
	int x;
	char* filestowrite[999];
	int numfiles = 0;

	for (x = 0; x < argc; x++) {
		// catch doubling of putchar when using '-' as argv
		if (strcmp(argv[x], "-") == 0) {
			repeatchars = 1;
			continue;
		}
		if ((stat(argv[x], &sb) == 0) && (S_ISDIR(sb.st_mode))) { 
			printf("error: %s is a directory\n", argv[x]);
			errcode = 1;
			continue;
		}
		// disregard files without r/w. non-exist files may pass
		if ((access(argv[x], R_OK|W_OK) == -1) && (access(argv[x], F_OK) != -1)) {
	        	fprintf(stderr, "%s: %s\n", argv[x], strerror(errno));
			errcode = 1;
			continue;
		}					
		filestowrite[numfiles] = malloc(strlen(argv[x]) + 1);
		strcpy(filestowrite[numfiles], argv[x]);
		numfiles++;
	}
	
	int fd;	

	for (x = 0; x < numfiles; x++) {
		// if not appending truncate the files first
		if (a != 1) {
			fd = open(filestowrite[x], O_TRUNC);
			close(fd);
		}		
		
	}
	// now visit getchar/putchar routine
	charcatcher(x, numfiles, filestowrite);
	return(0);
}

charcatcher(int x, int numfiles, char*filestowrite[999]) {
	char line[999];
        int linecnt = 0;
        // print the stdin and write to file immediately
        while (fgets(line, sizeof line, stdin) != NULL) {
		printf("%s", line);
                
		for (x = 0; x < numfiles; x++) { 
                	filewriter(x, numfiles, filestowrite, line);
                }
		//double print if "-" on command line
		if (repeatchars == 1) {                	
			printf("%s", line);
               	}
        }
	return(0);
}

filewriter(int x, int numfiles, char*filestowrite[999], char line[999]) {
	FILE* fp;
	for (x = 0; x < numfiles; x++) {
		fp = fopen(filestowrite[x], "a");
		fprintf(fp, "%s", line);
		fclose(fp);
	}
	return(0);
}

static int flag_help;

static struct option const long_options[] = 
{
	{"append", no_argument, 0, 'a'},
	{"ignore interrupt", no_argument, 0, 'i'},
 	{"help", no_argument, &flag_help, 1}, 
   	{0, 0, 0, 0}			
};

usage() {
	printf("usage: tee [file1] [file2] -a (append) -i (ignore interrupt)\n");
	if (flag_help) {
		exit(EXIT_SUCCESS);
	}
	else {
		exit(EXIT_FAILURE);
	}
}

void sigproc() {
	signal(SIGINT, sigproc);
}

int main (int argc, char* argv[]) {
	int optc;
	int index;

	while ((optc = getopt_long (argc, argv, "ai", long_options, (int *) 0)) != EOF) {
		switch (optc) {
			case 0:
		    	    break; 
	   		case 'a':
	            	    a = 1;
		    	    break;
	   		case 'i':
	            	    ii = 1;
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
	// ignore signal ^C
	if (ii == 1) {
		signal(SIGINT, sigproc);
	}

	for (index = optind; index < argc; index++) {
		;
	}

	argc -= optind;
	argv += optind;

	if (filechecker(argc, argv) != 0) {
		exit(EXIT_FAILURE);
	}
	else if (errcode != 0) {
		exit(EXIT_FAILURE);
	}
	else {
		exit(EXIT_SUCCESS);
	}
}
