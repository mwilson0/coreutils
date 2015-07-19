#include<stdio.h>
#include<getopt.h>
#include<stdlib.h>
#include<time.h>
#include<unistd.h>
#include<string.h>
#include<utime.h>
#include<sys/stat.h>

// touch clone 0.1. matthew wilson. july 2015. 
// based on touch from GNU Coreutils 8.24. 
// License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>

int leftover=0;
int a=0, c=0, m=0;

thefunc(int argcc, char*argvv[]) {

int x;
char* items[999];

// leftover filenames in items[x]

for (x=0; x<leftover; x++) {
	items[x]=malloc(strlen(argvv[x])+1);
	strcpy(items[x], argvv[x]);
}
 
struct stat sb;
struct utimbuf new_times;

// touch files

for (x=0; x<leftover; x++) {
	if (access(items[x], F_OK) != -1) {
		if (access(items[x], W_OK) != -1) {
			; 
		}
		else {
			printf("cannot touch '%s': ", items[x]); 
			printf("Permission denied.\n");
			continue;
		}
	
	// change modtime
	if (a!=1&&m==1)  {
		if (stat(items[x], &sb) ==-1) {
			printf("file error\n");
		}
		else {
			new_times.actime=sb.st_atime; // atime unchanged
			new_times.modtime=time(NULL); // mtime to current
			utime(items[x], &new_times);	
		}
	}	

	// change atime
	if (a==1&&m!=1)  {
		if (stat(items[x], &sb) ==-1) {
			printf("file error\n");
		}
		else {
			new_times.actime=time(NULL); // atime to current
			new_times.modtime=sb.st_mtime; // mtime unchanged
			utime(items[x], &new_times);	
		}
	}	

	//default
	if ((a==0&&m==0)||(a==1&&m==1)) {
		if (stat(items[x], &sb) ==-1) {
			printf("file error\n");
		}
		else {
			new_times.actime=time(NULL); // atime to current
			new_times.modtime=time(NULL); // mtime to current
			utime(items[x], &new_times);	
		}
	}

}	// create files long as C flag not used
	else if ((access(items[x], F_OK) == -1) && c!=1) {	
		creat(items[x], 0666);
	}
}


exit(0);
}

static int flag_help;

static struct option const long_options[] = 
{
	{"accesstime", no_argument, 0, 'a'}, 
    	{"nocreate", no_argument, 0, 'c'}, 
    	{"modtime", no_argument, 0, 'm'},
	{"help", no_argument, &flag_help, 1}, 
   	{0, 0, 0, 0}			
};

usage() {
printf("usage: 'touch file1 file2 [-acm]'\n");
exit(1);
}

main (int argc, char* argv[]) {
int optc;
int index;

while ((optc = getopt_long (argc, argv, "acm", long_options, (int *) 0)) !=EOF) {
	switch (optc) {
	    case 0:
		break; 
	    case 'a':
	        a=1;
		break;
	    case 'c':
	        c=1;
		break;
	    case 'm':
	        m=1;
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
	printf("a=access time. m=mod time. c=no create.\n");
	usage();
}

if (argc==1) {
	printf("touch: missing file operand\n");
	usage();
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
		usage();
	}

}

}
