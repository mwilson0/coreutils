#include<stdio.h>
#include<getopt.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>

// tail clone 0.1. matthew wilson. july 2015. 
// based on tail from GNU Coreutils 8.2. No warranty.
// License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>

int leftover=0;
int q=0, v=0;

noargs() {
#define MAX_LEN 4096
char lin[MAX_LEN];
char* linitems[MAX_LEN];
int x=0, qq=0;

if (v==1) {
	printf("==> standard input <==\n");
}
	
while (fgets(lin, MAX_LEN, stdin)!=NULL) {
	linitems[x]=strdup(lin);  
	x++;
}

// print last 10 lines of array
if (x>10) {
	for (qq=(x-10); qq<x; qq+1) {
        	printf("%s", linitems[qq]);
        	qq++;
        }
}

else {
	for (qq=0; qq<x; qq++)
        	printf("%s", linitems[qq]); {
        	qq++;
	}
}

exit(0);
}


thefunc(int argcc, char*argvv[]) {
int x;
int y=0;
int ch=0;
FILE* fp;
char* items[2048];
char* lineitems[9999];
char line[9999];
int linecnt=0;
int cc;

// put existing files in items[y]
for (x=0; x<leftover; x++) {
	if (access(argvv[x], F_OK|R_OK)==-1) {
		printf("no such file: '%s'\n", argvv[x]);
	}
	else {
		items[y]=malloc(strlen(argvv[x])+1);
		strcpy(items[y], argvv[x]);
		y++;
	}
}

// count the lines in each file
for (x=0; x<y; x++) {
	fp=fopen(items[x], "r");
	while (fgets(line, sizeof line, fp)!=NULL) {
		lineitems[linecnt]=strdup(line);
		linecnt++;
	}
	// then print out the lines
	// verbose mode
	if  ( (v==1 && q==1 && linecnt!=0) || (v==1 && q!=1) || 
	      (leftover>1 && q!=1) ) { 
		printf("==> %s <==\n", items[x]);
	}	
	// print out bottom 10 lines	
	if (linecnt>=10) {
		for (cc=(linecnt-10); cc<linecnt; cc++) {
			printf("%s", lineitems[cc]);
		}
	}
	else if (linecnt<10) {
		for (cc=0; cc<linecnt; cc++) {
			printf("%s", lineitems[cc]);
		}
	}

	linecnt=0;
	fclose(fp);
}

exit(0);
}

static int flag_help;

static struct option const long_options[] = 
{
	{"quiet", no_argument, 0, 'q'}, 
    	{"verbose", no_argument, 0, 'v'}, 
    	{"help", no_argument, &flag_help, 1}, 
   	{0, 0, 0, 0}			
};

usage() {
printf("usage: tail [file1] [file2] [-qv]\n");
exit(1);
}

main (int argc, char* argv[]) {
int optc;
int index;

while ((optc = getopt_long (argc, argv, "qv", long_options, (int *) 0)) !=EOF) {
	switch (optc) {
	   case 0:
		break; 
	   case 'q':
	        q=1;
		break;
	   case 'v':
	        v=1;
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
if (argc==1) {
	noargs();
}

else if (argc>1) {
	for (index=optind; index<argc; index++) {
		;
	}

	argc -= optind;
	argv += optind;
	leftover=index-optind;

	if (leftover>=1) {
		thefunc(argc, argv);
	}
	if (leftover==0) {
		noargs();
	}		
}

}
