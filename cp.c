#include<stdio.h>
#include<getopt.h>
#include<stdlib.h>
#include<string.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<unistd.h>
#include<fcntl.h>
#include<time.h>
#define BUF_SIZE 9999

// cp clone 0.1.2. matthew wilson. 2015, 2016
// based on cp from GNU Coreutils 8.2. No warranty.
// License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>

int i = 0, v = 0, u = 0;

int thefunc(int argc, char*argv[]) {
	int x;
	struct stat fileStat;
	time_t raw[2];
	int c;
	ssize_t file_in, file_out;
	char buffer[BUF_SIZE];
	int sourcefile, destfile;

	// test source file
	sourcefile=open(argv[0], O_RDONLY);

	if (sourcefile == -1) {
		printf("cannot stat '%s': No such file or directory\n", argv[0]);
		return(1);
	}

	// check if dest file exists
	if (access(argv[1], F_OK|R_OK|W_OK) != -1) {
		// interactive mode
		if (i == 1 & u != 1) { 
			printf("overwrite '%s'? ", argv[1]);
			scanf("%s", &c);	
			if (c != 'y') {
				return(0);
			}
		}

		// update mode. compare times of source and dest files
		if (u == 1) {
			for (x = 0; x < argc; x++) {
				stat(argv[x], &fileStat);
				raw[x] = fileStat.st_mtime;
			}
			if (raw[0] < raw[1]) {
				printf("no overwrite (^.^)\n");
				return(0);
			}
		}
	}

	// first append if using ../
	if ((strncmp(argv[1], "../", 3) == 0) && strlen(argv[1]) == 3) {
		strcat(argv[1], argv[0]);
	}

	// now check if src and dest are same

	if (strcmp(argv[0], argv[1]) == 0) {
		printf("error: same file\n");
		return(1);
	}

	if (strstr(argv[1], "./")) {
		if (strcmp(argv[0], argv[1]+2) == 0) {
			printf("error: same file\n");
			return(1);
		}
	} 

	if (strstr(argv[0], "./")) {
		if (strcmp(argv[1], argv[0]+2) == 0) {
			printf("error: same file\n");
			return(1);
		}
	} 

	// otherwise do some copying

	destfile=creat(argv[1], 0666);

	while ((file_in = read(sourcefile, &buffer, BUF_SIZE)) > 0) {
		file_out = write(destfile, &buffer, (ssize_t) file_in);
		if (file_out != file_in) {
			printf("write error\n");
			return(1);
		}
	}

	// verbose mode
	if (v == 1) {
		printf("%s -> %s\n", argv[0], argv[1]);
	}

	close(sourcefile); close(destfile);
	return(0);
}

static int flag_help;

static struct option const long_options[] = 
{
	{"interactive", no_argument, 0, 'i'}, 
    	{"verbose", no_argument, 0, 'v'}, 
    	{"update", no_argument, 0, 'u'},
	{"help", no_argument, &flag_help, 1}, 
   	{0, 0, 0, 0}			
};

int usage() {
	printf("usage: cp [source file] [dest file] [-ivu]\n");
	if (flag_help) {
		exit(EXIT_SUCCESS);
	}
	else {
		exit(EXIT_FAILURE);
	}
}

int main (int argc, char* argv[]) {
	int optc;
	int index;

	while ((optc = getopt_long (argc, argv, "ivu", long_options, (int *) 0)) != EOF) {
		switch (optc) {
	    	    case 0:
		    	break; 
	    	    case 'i':
	              	i = 1;
		      	break;
	    	    case 'v':
	        	v = 1;
			break;
	    	    case 'u':
	            	u = 1;
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

	argc -= optind;
	argv += optind;
	
	for (index = optind; index < argc; index++) {
		;
	}
	
	if (argc < 2) {
		printf("cp: missing file operand\n");
		usage();
	}
	else if (argc == 1) {
		printf("cp: missing destination file operand after '%s'\n", argv[0]);
		usage();
	}
	else if (argc == 2) {
		if (thefunc(argc, argv) == 0) {
			exit(EXIT_SUCCESS);
		}
		else {
			exit(EXIT_FAILURE);
		}
	}
	else {
		usage();
	}
}
