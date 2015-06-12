#include<stdio.h>
#include<getopt.h>
#include<stdlib.h>
#include<ctype.h>
#include<time.h>
#include<utmp.h> // for logged in users
#include<sys/types.h>

// uptime clone. 0.1. matthew wilson. june 2015. 
// uptime clone from spec as used in procps 3.2.8 by Greenfield and Johnson. No warranty. License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>

uptimefunc() {
// first scan utmp file for number of users
struct utmp *ii;
int numusers=0;

setutent();

while ((ii=getutent()) !=NULL) {
	if (ii->ut_type==7) {
		numusers++;
	}	
}
endutent();

// ut_type determines a login "class" of a user.
// ut_type number 7 is logged in user

// now time display

struct tm *info; // struct tm has time info
time_t curtime;
curtime=time(NULL);
info=localtime(&curtime);

printf("%d:%02d:%02d ", info->tm_hour, info->tm_min, info->tm_sec);

// now the uptime

FILE* fp;
char* procfilename="/proc/uptime";

if (!(fp=fopen(procfilename, "r"))) {
	printf("uptime read error\n");
	exit(1);
}

float nums[2];

fscanf(fp, "%g %g", &nums[0], &nums[1]);
fclose(fp);

if (nums[0] < 3600) {
	printf("up %d minutes", (int)(nums[0]/60));
}

else if (nums[0] >= 3600 && nums[0] < 86400 ) {
	printf("up %i hours, ", (int)(nums[0]/(60*60)));
	printf("%d minutes", ((int)nums[0]%3600)/60);
}

else if (nums[0] >=86400) { 
	int daystotal=((int)nums[0]/(60*60))/24;   // num of days
	int hourstotal=((int)nums[0]/(60*60));     // num of hours
	int hoursinxdays=daystotal*24;		   	
	int leftoverhours=hourstotal-hoursinxdays;
	int mins=((int)nums[0]%3600)/60;
	printf("up %d days, %d:%d", daystotal, leftoverhours, mins);
}

// finally the load averages

FILE* fp2;
char* loadfile="/proc/loadavg";

if (!(fp2=fopen(loadfile, "r"))) {
	printf("load avg read error\n");
	exit(1);
}

float numz[3];

fscanf(fp2, "%g %g %g", &numz[0], &numz[1], &numz[2]);
fclose(fp2);

// print the rest

char* usercount;
if (numusers==1) {
	usercount="user";
}
else if (numusers>1) {
	usercount="users";
}

printf(", %d %s, load average: %.2f %.2f %.2f\n", numusers, usercount, numz[0], numz[1], numz[2]);

exit(0);
}

static struct option const long_options[] = 
{
    {"version", no_argument, 0, 'V'}, 
    {0, 0, 0, 0}			
};

usage() {
printf("usage: 'uptime [-V]'\n\t -V   display version\n");
exit(1);
}

main (int argc, char* argv[]) {

int leftover=0;
int optc;
int v = 0;
int index;
opterr=0; // disables built-in getopt error messages ie - 'invalid option'

while ((optc = getopt_long (argc, argv, "V", long_options, (int *) 0)) !=EOF) {
	switch (optc) {
	    case 'V':
	        v=1;
		break;
	    case '?':
	    default:	
		usage();
		break;
	}
}

if (argc==1) {
	uptimefunc();
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
	usage();
}

if (leftover==0) {
	if (v==1) {
        	printf("ver 0.1\n"); 
	}

}

}

}
