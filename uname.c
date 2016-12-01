#include<stdio.h>
#include<getopt.h>
#include<stdlib.h>
#include<sys/utsname.h>
#include<string.h>

// uname clone. 0.2. mwilson. 2015-16
// uname clone from spec as used in GNU coreutils 8.23. No warranty. 
// License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>

static int flag_help;
static int fail = 0;
char* program_name;

// for the non-standard -o option to print the os name. 
int os_nonstandard() {
	FILE* fp;
	char line[100];
	char* newline;
	int len;
	static int found = 0;
	// reading os-release file
	fp = fopen("/etc/os-release", "r");

	if (fp == NULL) { 
		return(1);	
	}
	else {
		while (fgets(line, sizeof (line), fp) != NULL) {
			line[strlen(line) - 1] = '\0';
			// copy after NAME=" and finish at last "
			if (strncmp(line, "NAME", 4) == 0) {	
				found = 1;
				newline = (line + 6);
				len = strlen(newline);
				newline[len - 1] = '\0';
				printf("%s", newline);
			}
		}
		fclose(fp);
		
		if (found == 1) 
			return(0);
		else 
			return(1);
	}
}

static struct option const long_options[] = 
{
    {"all", no_argument, 0, 'a'},
    {"kernel-name", no_argument, 0, 's'},
    {"nodename", no_argument, 0, 'n'},	
    {"kernel-release", no_argument, 0, 'r'},
    {"machine", no_argument, 0, 'm'},	
    {"kernel-version", no_argument, 0, 'v'}, 
    {"operating-system", no_argument, 0, 'o'},
    {"help", no_argument, &flag_help, 1},
    {0, 0, 0, 0}			
};

void usage() {
	fprintf(stderr, "Usage: %s [-a | --all] [-s | --kernel-name] [-n | --nodename] [-r | --kernel-release] [-m | --machine] [-v | --kernel-version] [-o | --operating-system] [--help]\n", program_name);

	if ((flag_help) && (fail != 1))
		exit(EXIT_SUCCESS);
	else
		exit(EXIT_FAILURE);
}

int main (int argc, char* argv[]) {
	program_name = argv[0];
	int index;
	int optc;
	int x;
	struct utsname info;
	uname(&info);
	char* items[5] = {info.sysname, info.nodename, info.release, info.version, info.machine};

	int a = 0, s = 0, n = 0, r = 0, m = 0, v = 0, o = 0;

	while ((optc = getopt_long (argc, argv, "asnrmvo", long_options, (int *) 0)) != EOF) {
		switch (optc) {
	    		case 0:
	        	    break;
	    		case 'a':
	        	    a = 1;
			    break;
	    		case 's':
			    s = 1;	        
	        	    break;	
   	    		case 'n':
			    n = 1;	        
	        	    break;	
	    		case 'r':
			    r = 1;
			    break; 	
	    		case 'm':
			    m = 1;
			    break; 	
	      		case 'v':
			    v = 1;
                	    break;
			case 'o':
			    o = 1;
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

	for (index=optind; index<argc; index++) {
		;
	}

	argc -= optind;
	argv += optind;

	if ((argc == 0) && ((a != 1) && (s != 1) && (n != 1) && (r != 1) && (m != 1) && (v != 1) && (o != 1))) {
		printf("%s\n", items[0]);
		exit(EXIT_SUCCESS);
	}

	if (argc > 0) {
		printf("uname: extra operand '%s'\n", argv[0]);
		usage();
	}

	if (a == 1) {
		for (x = 0; x < 5; x++) {
			printf("%s ", items[x]);	
		}
		if (os_nonstandard() == 1) 
			printf("%s ", items[0]);
		printf("\n");
		exit(EXIT_SUCCESS);	
	}
        
	if (s == 1) 
        	printf("%s ", items[0]); 
	if (n == 1) 
        	printf("%s ", items[1]); 
	if (r == 1) 
        	printf("%s ", items[2]); 
	if (v == 1) 
        	printf("%s ", items[3]); 
	if (m == 1) 
        	printf("%s ", items[4]); 
	if (o == 1) 
		if (os_nonstandard() == 1)
			printf("%s ", items[0]);
	printf("\n");
	exit(EXIT_SUCCESS);
}
