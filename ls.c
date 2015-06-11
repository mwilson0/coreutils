#include<stdio.h>
#include<getopt.h>
#include<stdlib.h>
#include<sys/types.h>
#include<dirent.h>
#include<string.h>
#include<sys/stat.h>
#include<time.h>
#include<grp.h>
#include<pwd.h>
#include<unistd.h>

// ls clone. 0.1. matthew wilson. june 2015.
// ls clone from spec as used in GNU coreutils 8.23. No warranty. 
// License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>

int noargs=0, l = 0, a = 0, h = 0;
int leftover=0;

//used for qsort
int compf(const void* x, const void* y) {
return (strcmp(*(const char**)x, *(const char **)y));
}

thefunc2(int argcc, char* argvv[]) {
int zz;
char* somearg[5096]; // incoming filename
struct stat fileStat;

// used for stepping thru directories
int x=0; // also using this as array counter
int i;
char* items[5096];
unsigned long int itemsize[5096];
unsigned long int rfitemsize[5096];
char* concatitems[5096];

DIR *pDir;
struct dirent *pDirent; //structure has file info(d_name[])

// used for getting raw time from files
time_t raw;
struct tm timeinfo;
char buffer[80];

// used for getting gid and uid from files
struct group *grp;
struct passwd *pwd;
char* uname;
char* grname;

for (zz=0; zz<leftover; zz++) {
	somearg[zz]=malloc(strlen(argvv[zz])+1);
        strcpy(somearg[zz], argvv[zz]);
	stat(somearg[zz], &fileStat);

	if (access(somearg[zz], F_OK|R_OK)!=-1) {
	
		if (S_ISREG (fileStat.st_mode)) {
		//printf("Regfile!\n");
               	// -l flag - perms, size, date
		
		if (l==1) {
		printf( (S_ISDIR(fileStat.st_mode) ) ? "d" : "-");
		printf( (fileStat.st_mode & S_IRUSR) ? "r" : "-");
		printf( (fileStat.st_mode & S_IWUSR) ? "w" : "-");
		printf( (fileStat.st_mode & S_IXUSR) ? "x" : "-");
		printf( (fileStat.st_mode & S_IRGRP) ? "r" : "-");
		printf( (fileStat.st_mode & S_IWGRP) ? "w" : "-");
		printf( (fileStat.st_mode & S_IXGRP) ? "x" : "-");
		printf( (fileStat.st_mode & S_IROTH) ? "r" : "-");
		printf( (fileStat.st_mode & S_IWOTH) ? "w" : "-");
		printf( (fileStat.st_mode & S_IXOTH) ? "x" : "-");

		raw=fileStat.st_mtime; // raw time from each file
		localtime_r(&raw, &timeinfo);
		pwd=getpwuid(fileStat.st_uid);
		grp=getgrgid(fileStat.st_gid);

		strftime(buffer, 80, "%b %d %I:%M", &timeinfo); 
		
		rfitemsize[zz]=fileStat.st_size;

		printf(" %-10s %-10s", pwd->pw_name, grp->gr_name);

		if (h==1) {
		if (rfitemsize[zz]<=999) {
			printf("%5lu", rfitemsize[zz]);
		}
		if (rfitemsize[zz]>=1000 && rfitemsize[zz]<=999999) {
			printf("%4.1fK", (float)rfitemsize[zz]/1024);
		}
		if (rfitemsize[zz]>=1000000 && rfitemsize[zz]<=999999999) {
			printf("%4.1fM", (float)rfitemsize[zz]/1024/1024);
		}	
		if (rfitemsize[zz]>=1000000000) {
			printf("%4.1fG", (float)rfitemsize[zz]/1024/1024/1024);
		}
		} else { printf("%10lu", rfitemsize[zz]); }
	
		printf(" %10s %s\n", buffer, somearg[zz]);
		} 
		else { 
			printf("%s\n", somearg[zz]); 
		}

		}
		
		if (S_ISDIR (fileStat.st_mode)) {
        		//printf("Dir!\n");
			// print dirname if multiple dirs to be listed
			if (leftover>1) {
			printf("%s:\n", somearg[zz]);
			}

		strcat(somearg[zz], "/");

		pDir=opendir(somearg[zz]);
		while ((pDirent=readdir(pDir)) !=NULL) {
        	
      		if (a!=1) {
        		if (
			strcmp(pDirent->d_name, ".")==0 ||
                	strcmp(pDirent->d_name, "..")==0 || 
			pDirent->d_name[0]=='.' )
			{
                	continue; // skips over . and .. listings
                	}
        	}
        	
		// pull filename out of struct
		items[x]=malloc(strlen(pDirent->d_name)+1);
        	strcpy(items[x], pDirent->d_name);
        	// concat full path on items to allow filesize
		concatitems[x]=malloc(1+strlen(somearg[zz])+strlen(items[x]));
		strcpy(concatitems[x], somearg[zz]);
		strcat(concatitems[x], items[x]);
		x++;
        	}
		
// sort files alphabetically

qsort(items, x, sizeof items[0], &compf);
qsort(concatitems, x, sizeof concatitems[0], &compf);

//finally print list files alphabetically with OR without size or other info
for (i=0; i<x; i++) {
		
// GET SIZE OF CONCAT ITEMS IF L=1
if (l==1) {
//stat(items[i], &fileStat); //old

stat(concatitems[i], &fileStat);

itemsize[i]=fileStat.st_size;

printf( (S_ISDIR(fileStat.st_mode) ) ? "d" : "-");
printf( (fileStat.st_mode & S_IRUSR) ? "r" : "-");
printf( (fileStat.st_mode & S_IWUSR) ? "w" : "-");
printf( (fileStat.st_mode & S_IXUSR) ? "x" : "-");
printf( (fileStat.st_mode & S_IRGRP) ? "r" : "-");
printf( (fileStat.st_mode & S_IWGRP) ? "w" : "-");
printf( (fileStat.st_mode & S_IXGRP) ? "x" : "-");
printf( (fileStat.st_mode & S_IROTH) ? "r" : "-");
printf( (fileStat.st_mode & S_IWOTH) ? "w" : "-");
printf( (fileStat.st_mode & S_IXOTH) ? "x" : "-");

raw=fileStat.st_mtime; // raw time from each file
localtime_r(&raw, &timeinfo);

pwd=getpwuid(fileStat.st_uid);
grp=getgrgid(fileStat.st_gid);

strftime(buffer, 80, "%b %d %I:%M", &timeinfo); //time formatter from rawtime

// ls -h 
if (h!=1) { // print normally
printf(" %-10s %-10s %10lu %10s %s\n", pwd->pw_name, grp->gr_name, itemsize[i], buffer, items[i]);
}

else { // print K, MB, GB
	printf(" %-10s %-10s", pwd->pw_name, grp->gr_name);

	if (itemsize[i]<=999) {
		printf("%5lu", itemsize[i]);
	}
	if (itemsize[i]>=1000 && itemsize[i]<=999999) {
		printf("%4.1fK", (float)itemsize[i]/1024);
	}
	if (itemsize[i]>=1000000 && itemsize[i]<=999999999) {
		printf("%4.1fM", (float)itemsize[i]/1024/1024);
	}
	if (itemsize[i]>=1000000000) {
		printf("%4.1fG", (float)itemsize[i]/1024/1024/1024);
	}
	printf(" %10s %s\n", buffer, items[i]);
}

} 

// if no flags are used just print filenames
else { 
	printf("%s\n", items[i]); 
}

}

closedir(pDir);
x=0; // required to reset counter and array when more than 1 directory
}
}	

else {
printf("Err! cannot access %s. No permissions or not exist\n", somearg[zz]);
}

} // loop ends here

exit(0);

} // function ends here

static int flag_help;

static struct option const long_options[] = 
{
    {"ls-l", no_argument, 0, 'l'},
    {"ls-a", no_argument, 0, 'a'},
    {"ls-h", no_argument, 0, 'h'},	
    {"help", no_argument, &flag_help, 1},
    {0, 0, 0, 0}			
};

usage() {
printf("usage: 'ls -[l][a][h]'\n");
exit(1);
}

main (int argc, char* argv[]) {
int index;
int optc;

while ((optc = getopt_long (argc, argv, "lah", long_options, (int *) 0)) !=EOF) {
	switch (optc) {
	    case 0:
	        break;
	    case 'l':
	      	l=1;	
	        break;
	    case 'a':
		a=1;	        
	        break;	
   	    case 'h':
		h=1;	        
	        break;	 	
	    case '?':
		printf("try ls --help for more info.\n");
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

// run ls without args

if (argc==1) {
	argv[0]="./";
	noargs=1;
	leftover=1;
	thefunc2(argc, argv);
}

else if (argc>1) {

//NON OPTION ARGUMENTS
for (index = optind; index < argc; index++) {	
//printf("index:%d non option argument %s\n", index, argv[index]);
	//thefunc(argv[2]);
	}

leftover=index-optind;

if (leftover==0) {
	argv[0]="./";
	leftover=1;
	thefunc2(argc, argv);
}

if (leftover>=1) {
	argc -= optind;
	argv += optind;
	thefunc2(argc, argv);
}

}

}
