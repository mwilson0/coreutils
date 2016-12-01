#include<stdio.h>
#include<getopt.h>
#include<stdlib.h>
#include<time.h>
#include<unistd.h>
#include<string.h>
#include<utime.h>
#include<sys/stat.h>

// touch clone 0.1.2. matthew wilson. 2015-16 
// based on touch from GNU Coreutils 8.24. 
// License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>

static int permission_denied = 0;
static int fail = 0;
int a = 0, c = 0, m = 0;
char* program_name;

int touch_func(int argc, char* argv[]) {
	int x;
	char* items[999];

	// leftover filenames in items[x]
	for (x = 0; x < argc; x++) {
		items[x] = malloc(strlen(argv[x]) + 1);
		strcpy(items[x], argv[x]);
	}
 
	struct stat sb;
	struct utimbuf new_times;

	// touch files
	for (x = 0; x < argc; x++) {
		if (access(items[x], F_OK) != -1) {
			if (access(items[x], W_OK) != -1) {
				; 
			}
			else {
				printf("cannot touch '%s': ", items[x]); 
				printf("Permission denied.\n");
				permission_denied = 1;
				continue;
			}
			// change modtime
			if (a != 1 && m == 1)  {
				if (stat(items[x], &sb) == -1) {
					printf("file error\n");
				}
				else {
					new_times.actime=sb.st_atime; // atime unchanged
					new_times.modtime=time(NULL); // mtime to current
					utime(items[x], &new_times);	
				}
			}	
			// change atime
			if (a == 1 && m != 1)  {
				if (stat(items[x], &sb) == -1) {
					printf("file error\n");
				}
				else {
					new_times.actime=time(NULL); // atime to current
					new_times.modtime=sb.st_mtime; // mtime unchanged
					utime(items[x], &new_times);	
				}
			}	
			//default
			if ((a == 0 && m == 0) || (a == 1 && m == 1)) {
				if (stat(items[x], &sb) == -1) {
					printf("file error\n");
				}
				else {
					new_times.actime=time(NULL); // atime to current
					new_times.modtime=time(NULL); // mtime to current
					utime(items[x], &new_times);	
				}
			}
		}	
		// create files long as C flag not used
		else if ((access(items[x], F_OK) == -1) && c != 1) {	
				creat(items[x], 0666);
			}
	}

	if (permission_denied)
		return(1);
	else
		return(0);
}

static int flag_help;

static struct option const long_options[] = 
{
	{"accesstime", no_argument, 0, 'a'}, 
    	{"no-create", no_argument, 0, 'c'}, 
    	{"modtime", no_argument, 0, 'm'},
	{"help", no_argument, &flag_help, 1}, 
   	{0, 0, 0, 0}			
};

void usage() {
	fprintf(stderr, "%s [file1] [file2] ...\n[-a | --accesstime (change only access time)]\n[-c | --no-create (don't create any files)]\n[-m | --modtime (change only modification time)]\n[--help]\n", program_name);
	if ((flag_help) && (fail != 1))
		exit(EXIT_SUCCESS);
	else
		exit(EXIT_FAILURE);
}

int main (int argc, char* argv[]) {
	program_name = argv[0];
	int optc;
	int index;

	while ((optc = getopt_long (argc, argv, "acm", long_options, (int *) 0)) != EOF) {
		switch (optc) {
	        	case 0:
		    	    break; 
	    		case 'a':
	        	    a = 1;
			    break;
	    		case 'c':
	        	    c = 1;
			    break;
	    		case 'm':
	        	    m = 1;
			    break;
	    		case '?':
			    fail = 1;
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

	if (argc == 0) {
		printf("%s: missing file operand\n", program_name);
		usage();
	}
	if (argc >= 1) {
		if (touch_func(argc, argv) == 0) 
			exit(EXIT_SUCCESS);
		else
			exit(EXIT_FAILURE);
	}
	else {
		usage();
	}
}
