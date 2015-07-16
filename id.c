#include<stdio.h>
#include<getopt.h>
#include<stdlib.h>
#include<string.h>
#include<grp.h>
#include<pwd.h>
#include<unistd.h>
#include<sys/types.h>
#include<ctype.h>

// id clone with enhancements*. matthew wilson. july 2015. 
// based on id from GNU Coreutils 8.2. 
// *will display user info if username entered OR UID.
// License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>

int leftover=0;
int g=0, capg=0, u=0, n=0, r=0;

struct group *grp;
struct passwd *pwd;

thefunc(int argcc, char*argvv[]) {

char *thearg=argvv[0];

// two levels of verification:
// check if argvv[0] is in the db, if not:
// see if argvv[0] is a UID number and if in db

if (leftover==1) {
	if (getpwnam(argvv[0])==NULL) { 
		int q;
		uid_t qq;
		q=atoi(argvv[0]);
		qq = q;
		
		while (*argvv[0]) {
			if (!isdigit(*argvv[0])) {
				printf("id: %s no such user\n", thearg);
				exit(1);
			}
			else {
				++argvv[0];
			}
		}	
		if (getpwuid(qq)==NULL) {
			printf("id: %s no such user\n", thearg);
			exit(1);
		}
		else {
			pwd=getpwuid(qq);
			grp=getgrgid(qq);
		}
			
	}
	else {
		// pwd and group = user input from argvv[0]
		pwd=getpwnam(argvv[0]);
		grp=getgrnam(argvv[0]);
	}	
}
else {  // or use current user if argvv[0] empty
	pwd=getpwuid(getuid());
	grp=getgrgid(getgid());
}

int x, ng=0;
gid_t *groups=NULL;

// control printout based on the flag. 
// have to work: -G, -Gn, -Gr, -Gnr not work: -Gu, -Gg, -Gun, -Gunr
// have to work: -g, -gr, -gn not work: -gu, gG

if ( (n==1 && g==0 && capg==0 && u==0 && r==0) || 
     (n==0 && g==0 && capg==0 && u==0 && r==1) || 
     (r==1 && n==1 && capg==0 && u==0 && g==0) ) {
	printf("id: cannot print only names or real ids in def. format\n");
	exit(1);
}

if ( (g==1 && u==1) ||
     (capg==1 && u==1) ||
     (capg==1 && g==1) ||
     (capg==1 && u==1 && n==1) ||
     (capg==1 && u==1 && n==1 && r==1) ) {
	printf("id: cannot print only of more than one choice\n");
	exit(1);
}

if (u==0 && g==0 && capg==0 && n==0 && r==0) {
	printf("uid=%d(%s) ", pwd->pw_uid, pwd->pw_name); 
	printf("gid=%d(%s) ", grp->gr_gid, grp->gr_name);
	printf("groups=");
}

if (u==1) {
	if (n==1) {
		printf("%s", pwd->pw_name);	
	}
	else{
		printf("%d", pwd->pw_uid);
	}
printf("\n");
exit(0);
}

if (g==1) {
	if (n==1) {
		printf("%s", grp->gr_name);
	}
	else {
		printf("%d", grp->gr_gid);
	}
printf("\n");
exit(0);
}

// prints out supplimentary groups. 

if (getgrouplist(pwd->pw_name, pwd->pw_gid, NULL, &ng) < 0 ) {
	groups=(gid_t *)malloc(ng * sizeof (gid_t));
	getgrouplist(pwd->pw_name, pwd->pw_gid, groups, &ng);
}

int cntr=0;

for (x=0; x<ng; x++) {
	if ( (capg==1 && n!=1) || (u==0 && g==0 && capg==0 && n==0 && r==0) ) {
		printf("%d ", groups[x]);
	}
	grp=getgrgid(groups[x]);
	if (grp!=NULL) { 
		if (capg==1 && n==1) {
			printf("%s ", grp->gr_name);	
		}
		if (capg!=1) {
			printf("(%s)", grp->gr_name);
		}
		if (cntr<ng-1 && capg!=1) { // print comma until last grp
			printf(", ");
		}
		if (cntr+1==ng) { // exit on last iteration
			printf("\n");
			exit(0);
		}
		cntr++;
	}
}

printf("\n");
exit(0);
}

static int flag_help;

static struct option const long_options[] = 
{
	{"g", no_argument, 0, 'g'}, 
    	{"G", no_argument, 0, 'G'}, 
    	{"u", no_argument, 0, 'u'},
    	{"n", no_argument, 0, 'n'},
    	{"r", no_argument, 0, 'r'},
	{"help", no_argument, &flag_help, 1}, 
   	{0, 0, 0, 0}			
};

usage() {
printf("usage: id [username] [-gGunr]'\n");
exit(1);
}

main (int argc, char* argv[]) {
int optc;
int index;

while ((optc = getopt_long (argc, argv, "gGunr", long_options, (int *) 0)) !=EOF) {
	switch (optc) {
	   case 0:
		break; 
	   case 'g':
	        g=1;
		break;
	    case 'G':
	        capg=1;
		break;
	    case 'u':
	        u=1;
		break;
 	    case 'n':
	        n=1;
		break;
	    case 'r':
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

if (flag_help) {
	printf("-g print group ID\n-G print all group IDs\n");
	printf("-u print user ID\n-n print name\n-r print real ID\n");
	usage();
}

if (argc==1) {
	thefunc(argc, argv);
}

else if (argc>1) {
	for (index=optind; index<argc; index++) {
		;
	}

	argc -= optind;
	argv += optind;
	leftover=index-optind;

	if (leftover==1||leftover==0) {
		thefunc(argc, argv);
	}

	if (leftover>1) {
		printf("id: extra operand '%s'\n", argv[1]);
		printf("try id --help\n");
		exit(1);	
	}
}

}
