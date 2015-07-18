#include<stdio.h>
#include<getopt.h>
#include<stdlib.h>
#include<grp.h>
#include<pwd.h>
#include<string.h>
#include<sys/types.h>
#include<unistd.h>
#include<fcntl.h>
#include<sys/stat.h>
#include<dirent.h>

// chown clone 0.1.1. matthew wilson. june, july 2015. 
// based on chown from GNU Coreutils 8.23. 
// License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>

int leftover=0;
int r=0;

thefunc(int argcc, char*argvv[]) {

struct stat fileStat;

DIR *pDir;
struct dirent *pDirent;
char* items[999];
char* newitems[999];
char* diritems[999];
char* concatitems[999];
char* uname;
char* grname;
char* delim=":";
char* token;
char* delimsearch;
struct passwd *pw;
struct group *gr;
int x;
int i;
int j=0;
int grouper=0;
int nontokenuser=0;
int y=0;

// start loop at 1 to skip over root:staff item and get leftovers
// files now in items[] array

for (i=1; i<leftover; i++) {
	items[j]=malloc(strlen(argvv[i])+1);
	strcpy(items[j], argvv[i]);
	j++;
}

if (j == 0) {
	printf("chown: missing operand after '%s'\n", argvv[0]);
	usage();
}

// look at argvv[0]. see if in format- username: or :group

if (argvv[0][strlen(argvv[0])-1] == ':') {
	// now check username
	int len=strlen(argvv[0]);
	argvv[0][len-1]='\0';
	uname=argvv[0];
	if ((pw=getpwnam(uname)) == NULL) {
		printf("invalid user '%s'\n", uname);
		exit(1);
	}
	nontokenuser=1;
}

else if (argvv[0][0] == ':') {
	// now check groupname
	grname=&argvv[0][1];
	if ((gr=getgrnam(grname)) == NULL) {
		printf("invalid group '%s'\n", grname);
		exit(1);
	}
	grouper=1;
}

// OR check for tokens in argvv[0]. see if argument is delimited or not

else {
	delimsearch=strchr(argvv[0], ':'); //search string
	if (delimsearch == NULL) {
		if ((pw=getpwnam(argvv[0])) == NULL) {
			printf("invalid user '%s'\n", argvv[0]);	
			exit(1);
		}
	}
	else {
		//tokenize the user:group
		uname=strtok(argvv[0], delim);
		grname=strtok(NULL, delim);
			if ((pw=getpwnam(uname)) == NULL) {
				printf("invalid user '%s'\n", uname);
				exit(1);
			}
			if ((gr=getgrnam(grname)) == NULL) {
				printf("invalid group '%s'\n", grname);
				exit(1);
			}
	}
} 

// now test access and write perms of files and move all existing and
// writable items to newitems[counter] 

int counter=0;

for (x=0; x<j; x++) {
	if (access(items[x], F_OK|R_OK)!=-1) {
		if (access(items[x], W_OK)==-1) {
			printf("chown: changing ownership of '%s': ", items[x]);
			printf("operation not permitted\n");
		}
		else {
			newitems[counter]=malloc(strlen(items[x])+1);
			strcpy(newitems[counter], items[x]);
			counter++;
		}
	}
	else {
		printf("chown: cannot access '%s': ", items[x]);	
		printf("No such file or directory\n");
	}
}

// loop through filenames and differentiate between files and dirs
// if dirs and if r=1, go into dirs and chown each file inside

for (x=0; x<counter; x++) {
	stat(newitems[x], &fileStat);

	// using format of :group
	if ((delimsearch == NULL && grouper == 1) && (S_ISREG (fileStat.st_mode))) {
		chown(newitems[x], -1, gr->gr_gid);
	}
	else if ((delimsearch == NULL && grouper == 1) && (S_ISDIR (fileStat.st_mode))) {
		if (r == 1) {
			strcat(newitems[x], "/");
			pDir=opendir(newitems[x]);
			while ((pDirent=readdir(pDir)) != NULL) {
				if (strcmp(pDirent->d_name, ".") == 0 || 
				    strcmp(pDirent->d_name, "..") == 0) { 
					continue; 
				}	
				diritems[y]=malloc(strlen(pDirent->d_name)+1);
				strcpy(diritems[y], pDirent->d_name);
				concatitems[y]=malloc(1+strlen(newitems[x])+strlen(diritems[y]));
				strcpy(concatitems[y], newitems[x]);
				strcat(concatitems[y], diritems[y]);
				chown(concatitems[y], -1, gr->gr_gid);
				y++;
			}
			chown(newitems[x], -1, gr->gr_gid);
			closedir(pDir);	
		}
		else {
			chown(newitems[x], -1, gr->gr_gid);
		}
	}

	// using format of user: or user
	else if (((delimsearch == NULL && nontokenuser == 1 || delimsearch == NULL )) && 
		 (S_ISREG (fileStat.st_mode))) {
		chown(newitems[x], pw->pw_uid, -1);
	}

	else if (((delimsearch == NULL && nontokenuser == 1 || delimsearch == NULL )) && 
		 (S_ISDIR (fileStat.st_mode))) {
		if (r == 1) {
			strcat(newitems[x], "/");
			pDir=opendir(newitems[x]);
			while ((pDirent=readdir(pDir)) != NULL) {
				if (strcmp(pDirent->d_name, ".") == 0 || 
				     strcmp(pDirent->d_name, "..") == 0) { 
					continue; 
				}
				diritems[y]=malloc(strlen(pDirent->d_name)+1);
				strcpy(diritems[y], pDirent->d_name);
				concatitems[y]=malloc(1+strlen(newitems[x])+strlen(diritems[y]));
				strcpy(concatitems[y], newitems[x]);
				strcat(concatitems[y], diritems[y]);
				chown(concatitems[y], pw->pw_uid, -1);
				y++;
			}
			chown(newitems[x], pw->pw_uid, -1);
			closedir(pDir);
		}
		else {
			chown(newitems[x], pw->pw_uid, -1);
		}
	}

	// using format of user:group
	if (delimsearch != NULL && (S_ISREG (fileStat.st_mode))) {
		chown(newitems[x], pw->pw_uid, gr->gr_gid);
	}
	else if (delimsearch != NULL && (S_ISDIR (fileStat.st_mode))) {
		if (r == 1) {
			strcat(newitems[x], "/");
			pDir=opendir(newitems[x]);
			while ((pDirent=readdir(pDir)) != NULL) {
				if (strcmp(pDirent->d_name, ".") == 0 || 
		    		     strcmp(pDirent->d_name, "..") == 0) { 
					continue; 
				}
				diritems[y]=malloc(strlen(pDirent->d_name)+1);
				strcpy(diritems[y], pDirent->d_name);
				concatitems[y]=malloc(1+strlen(newitems[x])+strlen(diritems[y]));
				strcpy(concatitems[y], newitems[x]);
				strcat(concatitems[y], diritems[y]);
				chown(concatitems[y], pw->pw_uid, gr->gr_gid);
				y++;
			}
			chown(newitems[x], pw->pw_uid, gr->gr_gid);
			closedir(pDir);
		}
		else {
			chown(newitems[x], pw->pw_uid, gr->gr_gid);
		}
	}
} // for loop ends here

exit(0);
}

static struct option const long_options[] = {
	{"recursive", no_argument, 0, 'R'}, 
    	{0, 0, 0, 0}			
};

usage() {
printf("usage: 'chown [-R] root:staff /somefile'\n");
exit(1);
}

main (int argc, char* argv[]) {
int optc;
int index;

while ((optc = getopt_long (argc, argv, "R", long_options, (int *) 0)) !=EOF) {
	switch (optc) {
	   case 0:
		break; 
	   case 'R':
	        r=1;
		break;
	   case '?':
		usage();
		break;
	   default:	
		usage();
		break;
	}
}

if (argc==1) {
	printf("chown: missing operand\n");
	usage();
}

else if (argc>1) {
	for (index=optind; index<argc; index++) {
		;
	}
	
	argc -= optind;
	argv += optind;
	leftover=index-optind;

	if (leftover==0) {
		printf("chown: missing operand\n");
		usage();
	}

	else {
	thefunc(argc, argv);
	}

}

}
