#include<stdio.h>
#include<getopt.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<errno.h>

// tail clone 0.2. matthew wilson. July 2015. Jan 2016.
// based on tail from GNU Coreutils 8.2. No warranty.
// License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>

void usage();
int noargs();
int tailfunc();

int q = 0, v = 0, n = 0;
int fail = 0;
int capt_optarg = 0;

int noargs() {
	#define MAX_LEN 4096
	char lin[MAX_LEN];
	char* linitems[MAX_LEN];
	int x = 0, qq = 0;

	if (v == 1) {
		printf("==> standard input <==\n");
	}
	
	while (fgets(lin, MAX_LEN, stdin) != NULL) {
		linitems[x] = strdup(lin);  
		x++;
	}
	// print last 10 lines of array if not using -n flag
	int lastlns;
	if (n == 1) {
		lastlns = x - capt_optarg;
	}	
	else {
		lastlns = x - 10;
	}
	// or print whole array if line count is < 10
	if (x < 10 && n != 1) {
		lastlns = 0;
	}
	for (qq = lastlns; qq < x; qq++ ) {
        	printf("%s", linitems[qq]);
        }
	return(0);
}

int tailfunc(int argc, char*argv[]) {
	int x;
	int y = 0;
	FILE* fp;
	char* items[2048];
	char* lineitems[9999];
	char line[9999];
	int linecnt = 0;
	int cc;

	// put existing files in items[y]
	for (x = 0; x < argc; x++) {
		if (access(argv[x], F_OK|R_OK) == -1) {
			fprintf(stderr, "tail: %s: %s\n", argv[x], strerror(errno)); 
			fail = 1;
		}
		else {
			items[y] = malloc(strlen(argv[x]) + 1);
			strcpy(items[y], argv[x]);
			y++;
		}
	}

	// count the lines in each file
	for (x = 0; x < y; x++) {
		fp = fopen(items[x], "r");
		
		while (fgets(line, sizeof line, fp) != NULL) {
			lineitems[linecnt] = strdup(line);
			linecnt++;
		}
		// then print out the lines
		// verbose mode
		if  ( (v == 1 && q == 1 && linecnt != 0) || (v == 1 && q != 1) || (argc > 1 && q != 1) ) { 
			printf("==> %s <==\n", items[x]);
		}	
		// print out bottom 10 lines	
		if (linecnt >= 10) {
			int lastlns;
			// or last n lines depending if -n flag was used		
			if (n == 1) {
				lastlns = linecnt - capt_optarg;
			}
			else {
				lastlns = linecnt - 10;
			}
			for (cc = lastlns; cc < linecnt; cc++) {
				printf("%s", lineitems[cc]);
			}
		}
		else if (linecnt < 10) {
			for (cc = 0; cc < linecnt; cc++) {
				printf("%s", lineitems[cc]);
			}
		}
		linecnt = 0;
		fclose(fp);
	}
	if (fail == 1) {
		return(1);
	}
	else {
		return(0);
	}
}

static int flag_help;

static struct option const long_options[] = 
{
	{"quiet", no_argument, 0, 'q'}, 
    	{"verbose", no_argument, 0, 'v'},
	{"lines", no_argument, 0, 'n'},
    	{"help", no_argument, &flag_help, 1}, 
   	{0, 0, 0, 0}			
};

void usage() {
	fprintf(stderr, "usage: tail [file1] [file2] [-q | --quiet] [-v | --verbose] [-n [num] (number of last) | --lines [num]\n");
	
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

	while ((optc = getopt_long (argc, argv, "qvn:", long_options, (int *) 0)) != EOF) {
		switch (optc) {
	   		case 0:
		    	    break; 
	   	    	case 'q':
	            	    q = 1;
			    break;
	   		case 'v':
	          	    v = 1;
			    break;
	   		case 'n':
	            	    n = 1;
			    capt_optarg = atoi(optarg); 
			    if (! (capt_optarg > 0)) {
					printf("err: invalid number of lines\n");
					exit(EXIT_FAILURE);
				}    
			    break;
			case '?':
			    usage();
			    break;
	   		default:	
			    usage();
			    break;
		}
	}
	
	for (index = optind; index < argc; index++) {
		;
	}

	argc -= optind;
	argv += optind;

	if (flag_help) {
		usage();
	}
	if (argc == 0) {
		noargs();
	}
	if (argc > 0) {
		if (tailfunc(argc, argv) != 0) {
			exit(EXIT_FAILURE);
		}
	}
	exit(EXIT_SUCCESS);
}
