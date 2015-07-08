#include<stdio.h>
#include<getopt.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<ctype.h>

// head clone 0.1. matthew wilson. july 2015. 
// based on head from GNU Coreutils 8.2. 
// License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>

int leftover=0;
int n=0, q=0, v=0;
int capt_optarg; // for -n flag requiring argument

noargs() {
int c;
int b=1;
int stopper;

if (n!=1) {
	stopper=10;
}
else {
	stopper=capt_optarg;
}

if (v==1) {
	printf("==> standard input <==\n");
}

while (c!=EOF && b<=stopper) {
	c=getchar();
	if (c=='\n') {
		b++;
	}
	putchar(c);
}

exit(0);

}

thefunc(int argcc, char*argvv[], char*optargg) {
int i;
int x;
int y=0;
char* items[2048];
char* lineitems[9999];
char line[9999];
int linecnt=0;
FILE* fp;

// put exisiting files in items[y]
for (x=0; x<leftover; x++) {
	if (access(argvv[x], F_OK|R_OK) ==-1) {
		printf("no such file: '%s' \n", argvv[x]);
	}
	else {
		items[y]=malloc(strlen(argvv[x])+1);
		strcpy(items[y], argvv[x]);
		y++;	
	}
}

// count lines in each file and print
for (x=0; x<y; x++) {
	fp=fopen(items[x], "r");
	while (fgets(line, sizeof line, fp)!=NULL) {
		lineitems[linecnt]=strdup(line);
		linecnt++;
	}
	// verbose mode
	if ( (v==1 && q==1 && linecnt!=0) || (v==1 && q!=1) ||
	     (leftover>1 && q!=1) ) {
		printf("==> %s <==\n", items[x]);
	}	 

	if (n==1) { // using -N flag
		if (capt_optarg>linecnt) {
			for (i=0; i<linecnt; i++) {
				printf("%s", lineitems[i]);
			}
		}	
		else {
			for (i=0; i<capt_optarg; i++) {
				printf("%s", lineitems[i]);
			}
		}
	}
	else {
		if (linecnt>=10) {
			for (i=0; i<10; i++) {
				printf("%s", lineitems[i]);
			}		
		}
		if (linecnt<10) {
			for (i=0; i<linecnt; i++) {
				printf("%s", lineitems[i]);
			}
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
	{"n", required_argument, 0, 'n'}, 
    	{"q", no_argument, 0, 'q'}, 
    	{"v", no_argument, 0, 'v'},
	{"help", no_argument, &flag_help, 1}, 
   	{0, 0, 0, 0}			
};

usage() {
printf("usage: head [file1] [-nqv]\n");
exit(1);
}

main (int argc, char* argv[]) {
int optc;
int index;

while ((optc = getopt_long (argc, argv, "n:qv", long_options, (int *) 0)) !=EOF)    {
	switch (optc) {
	   case 0:
		break; 
	   case 'n': // N flag has required arg. converted to int. 
	        n=1;
		capt_optarg=atoi(optarg); 
		if (!(capt_optarg>=1)) {
			printf("err: invalid number of lines\n");
			exit(1);
		}
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
	printf("flags:\n-n num : print first n lines\n");
	printf("-q : quiet. do not print headers\n");
	printf("-v : verbose. print all headers\n");
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
		thefunc(argc, argv, optarg);
	}
	if (leftover==0) {
		noargs();
	}

}	

}
