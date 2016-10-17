#include<stdio.h>
#include<getopt.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<errno.h>
#include<signal.h>

// TEE clone 0.1. MWilson, Oct 2016.
// based on Tee from GNU Coreutils. 
// License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>

void sigproc();
int leftover=0;
int a=0;
int ii=0;
FILE* fp;

thefunc(int argcc, char*argvv[]) {
int x;

// file modes: append or overwrite

char* themode;
if (a == 1) {
	themode = "a";
}
else {
	themode = "w";
}

char* thefiles[50];
int filecnt = 0;

// loop thru args. keep files that do not exist/or have no permissions problems

for (x = 0; x < leftover; x++) {
	if ( (access(argvv[x], F_OK) == -1) || (access(argvv[x], R_OK|W_OK) != -1) ) {
		thefiles[filecnt] = malloc(strlen(argvv[x]) + 1);
		strcpy(thefiles[filecnt], argvv[x]);
		filecnt++;	
	}
	else {
		fprintf(stderr, "%s: %s\n", argvv[x], strerror(errno));
	}

}

// collect and display the stdin lines
char line[999];
int inputlines=0;
char* mylines[99];

while (fgets(line, sizeof line, stdin) != NULL) {
	printf("%s", line);
	mylines[inputlines] = malloc(strlen(line) + 1);
	strcpy(mylines[inputlines], line);
	inputlines++;
}

// then write lines to files

int y;
for (x = 0; x < filecnt; x++) {
	fp = fopen(thefiles[x], themode);
	
	for (y = 0; y < inputlines; y++) {
		fprintf(fp, "%s", mylines[y]);
	}
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
exit(EXIT_FAILURE);
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
	            a=1;
		    break;
	   	case 'i':
	            ii=1;
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

if (argc > 1) {
	for (index = optind; index < argc; index++) {
		;
	}

	argc -= optind;
	argv += optind;
	leftover = index-optind;
}

// ignore signal ^C
if (ii == 1) {
	signal(SIGINT, sigproc);
}

// call primary function
thefunc(argc, argv);
exit(EXIT_SUCCESS);
}
