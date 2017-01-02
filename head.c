#include<stdio.h>
#include<getopt.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<errno.h>

// head clone 0.2. matthew wilson. 
// based on head from GNU Coreutils 8.2. 
// License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>
// No warranty.

int noargs();
void usage();
int headfunc();

int n, q, v, fail = 0;
int capt_optarg; // for -n flag requiring argument

int noargs() {
	int c;
	int b = 1;
	int stopper;

	if (n != 1) {
		stopper = 10;
	}
	else {
		stopper = capt_optarg;
	}

	if (v == 1) {
		printf("==> standard input <==\n");
	}

	while (c != EOF && b <= stopper) {
		c = getchar();
		if (c == '\n') {
			b++;
		}
		putchar(c);
	}
	return(0);
}

int headfunc(int argc, char*argv[]) {
	int i;
	int x;
	int y = 0;
	char* items[2048];
	char* lineitems[9999];
	char line[9999];
	int linecnt = 0;
	FILE* fp;

	// put exisiting files in items[y]
	for (x = 0; x < argc; x++) {
		if (access(argv[x], F_OK|R_OK) ==-1) {
			fprintf(stderr, "head: %s: %s \n", argv[x], strerror(errno));
			fail = 1;
		}
		else {
			items[y] = malloc(strlen(argv[x]) + 1);
			strcpy(items[y], argv[x]);
			y++;	
		}
	}

	// count lines in each file and print
	for (x = 0; x < y; x++) {
		fp = fopen(items[x], "r");
		while (fgets(line, sizeof line, fp) != NULL) {
			lineitems[linecnt] = strdup(line);
			linecnt++;
		}
		// verbose mode
		if ( (v == 1 && q == 1 && linecnt != 0) || (v == 1 && q != 1) || (argc > 1 && q != 1) ) {
			printf("==> %s <==\n", items[x]);
		}	 
		// using -N flag
		if (n == 1) { 			
			if (capt_optarg > linecnt) {
				for (i = 0; i < linecnt; i++) {
					printf("%s", lineitems[i]);
				}
			}	
			else {
				for (i = 0; i < capt_optarg; i++) {
					printf("%s", lineitems[i]);
				}
			}
		}
		else {
			if (linecnt >= 10) {
				for (i = 0; i < 10; i++) {
					printf("%s", lineitems[i]);
				}		
			}
			if (linecnt < 10) {
				for (i = 0; i < linecnt; i++) {
					printf("%s", lineitems[i]);
				}
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
	{"lines", required_argument, 0, 'n'}, 
    	{"quiet", no_argument, 0, 'q'}, 
    	{"verbose", no_argument, 0, 'v'},
	{"help", no_argument, &flag_help, 1}, 
   	{0, 0, 0, 0}			
};

void usage() {
	fprintf(stderr, "usage: head [file 1] ... [file 2] [-n | --lines] [-q | --quiet] [-v | --verbose] [--help]\n");
	if (! flag_help) {
		exit(EXIT_FAILURE);
	}
}

int main (int argc, char* argv[]) {
	int optc;
	int index;

	while ((optc = getopt_long (argc, argv, "n:qv", long_options, (int *) 0)) != EOF) {
		switch (optc) {
	   		case 0:
			    break; 
	   		case 'n': // N flag has required arg. converted to int. 
	         	    n = 1;
			    capt_optarg = atoi(optarg); 
			    if (! (capt_optarg >= 1)) {
			    	    printf("err: invalid number of lines\n");
			            exit(EXIT_FAILURE);
			    }
			    break;
	   		case 'q':
	        	    q = 1;
			    break;
	   		case 'v':
	        	    v = 1;
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
		fprintf(stderr, "head clone!\n");
		usage();
		exit(EXIT_SUCCESS);
	}
	if (argc == 0) {
		noargs();
	}
	else {
		if (headfunc(argc, argv) != 0) {
			exit(EXIT_FAILURE);
		}
	}
	exit(EXIT_SUCCESS);
}
