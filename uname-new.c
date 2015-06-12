#include<stdio.h>
#include<getopt.h>
#include<stdlib.h>
#include<sys/utsname.h>

// uname clone. 0.1. matthew wilson. june 2015. 
// uname clone from spec as used in GNU coreutils 8.23. No warranty. 
// License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>

int leftover=0;
static int flag_help;

static struct option const long_options[] = 
{
    {"all", no_argument, 0, 'a'},
    {"kn", no_argument, 0, 's'},
    {"nodename", no_argument, 0, 'n'},	
    {"release", no_argument, 0, 'r'},
    {"machine", no_argument, 0, 'm'},	
    {"platform", no_argument, 0, 'i'},
    {"version", no_argument, 0, 'v'}, 
    {"help", no_argument, &flag_help, 1},
    {0, 0, 0, 0}			
};

usage() {
printf("usage: 'uname -[a][s][n][r][m][i][v]'\n");
exit(1);
}

main (int argc, char* argv[]) {

int optc;
int x;
struct utsname info;
uname(&info);
char* items[5];
items[0]=info.sysname;
items[1]=info.nodename;
items[2]=info.release;
items[3]=info.version;
items[4]=info.machine;
int a=0, s = 0, n = 0, r = 0, m = 0, i = 0, v = 0;
int index;

while ((optc = getopt_long (argc, argv, "asnrmiv", long_options, (int *) 0)) !=EOF) {
	switch (optc) {
	    case 0:
	        break;
	    case 'a':
	        a=1;
		break;
	    case 's':
		s=1;	        
	        break;	
   	    case 'n':
		n=1;	        
	        break;	
	    case 'r':
		r=1;
		break; 	
	    case 'm':
		m=1;
		break; 	
	    case 'i':
		i=1;
		break; 
            case 'v':
		v=1;
                break;
	    case '?':
		printf("try uname --help for more info.\n");
		exit(1);
		break;
	    default:	
		usage();
		break;
	}
}

if (flag_help) {
	usage();
}

if (argc==1) {
	printf("%s\n", items[0]);
	exit(0);
}


else if (argc>1) {

//re-position arg vectors after removing flags
for (index=optind; index<argc; index++) {
	;
}

argc -= optind;
argv += optind;
leftover=index-optind;

// KEY: leftover argument vectors not allowed while running this command
if (leftover>=1) {
	printf("uname: extra operand '%s'\n", argv[0]);
	usage();
}

if (leftover==0) {
	if (a==1) {
		for (x=0; x<5; x++) {
			printf("%s ", items[x]);	
		}
	printf("\n");
	exit(0);	
	}        
	if (s==1) {
        	printf("%s ", items[0]); }
	if (n==1) {
        	printf("%s ", items[1]); }
	if (r==1) {
        	printf("%s ", items[2]); }
	if (v==1) {
        	printf("%s ", items[3]); }
	if (m==1) {
        	printf("%s ", items[4]); }
	if (i==1) {
        	printf("%s ", items[4]); }

	printf("\n");

}

}

}
