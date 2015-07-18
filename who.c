#include<stdio.h>
#include<getopt.h>
#include<stdlib.h>
#include<ctype.h>
#include<time.h>
#include<utmp.h> 
#include<sys/types.h>
#include<string.h>

// who clone 0.1.1. matthew wilson. june, july 2015. 
// who clone from spec as used in GNU coreutils 8.23. No warranty. 
// License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>

/*
ut_user = login user name
ut_id = id from rootdir/etc/inittab
ut_line = login terminal name
ut_pid = process id
ut_type = entry type - #7 is a logged in user 
ut_host = host user logged in from
*/

int leftover=0;
int a=0, b=0, q=0, r=0, u=0;

thefunc(int argcc, char*argvv[]) {

int usercount=0;
struct utmp *ii;
time_t logintime_raw;
time_t boottime_raw;

setutent();

// number of logged in users. -q flag runs alone, thus following logic
if (q==1) {
	while ((ii=getutent()) !=NULL) {
		if (ii->ut_type==7) {
			printf("%s ", ii->ut_user);
			usercount++;
		}
	}

printf("\n# users=%d\n", usercount);
endutent();
exit(0);
}

if (a==1) {
        while ((ii=getutent()) !=NULL) {
		char tempbuf[99]; // used to remove newline from ctime
		if (ii->ut_type==RUN_LVL) {
			logintime_raw=ii->ut_time;
			printf("           ");
                   	printf("run-level %c %s", ii->ut_pid, ctime(&logintime_raw));
		}
    		else if (ii->ut_type==BOOT_TIME) {
                	boottime_raw=ii->ut_time;
                        printf("           ");
			printf("system boot %s", ctime(&boottime_raw));
         	}
		else {
			if (ii->ut_pid==0) {
				printf("%16s     ", ii->ut_line);
			}
			else {
				printf("%-10s %-10s", ii->ut_user, ii->ut_line);
			}
                	logintime_raw=ii->ut_time;
                	strcpy(tempbuf, ctime(&logintime_raw));
                	tempbuf[strlen(tempbuf)-1]='\0';
                	printf("  %s %5d (%s)\n", tempbuf, ii->ut_pid, ii->ut_host);
		}
	}

endutent();
exit(0);
}

else {
	while ((ii=getutent()) !=NULL) {
		// runlevel
		if (r==1) {
			if (ii->ut_type==RUN_LVL) {
				logintime_raw=ii->ut_time;	
				printf("run-level %c %s", ii->ut_pid, ctime(&logintime_raw));
			}
		}
		// boot time
		if (b==1) {
			if (ii->ut_type==BOOT_TIME) {
				boottime_raw=ii->ut_time;	
				printf("system boot %s", ctime(&boottime_raw));
			}
		}
		// no args reg command
		if (argcc==1 || u==1)  {
			if (ii->ut_type==7) {
				char tempbuf[99]; // used to remove newline from ctime
				printf("%-10s %-10s", ii->ut_user, ii->ut_line);
				logintime_raw=ii->ut_time;
				strcpy(tempbuf, ctime(&logintime_raw));
				tempbuf[strlen(tempbuf)-1]='\0';
				
				if (u==1) {
					printf("%s %5d (%s)\n", tempbuf, ii->ut_pid, ii->ut_host);
				}
				else {
					printf("%s (%s)\n", tempbuf, ii->ut_host);
				}
			}
		}
	}

endutent();
}

exit(0);
}

static struct option const long_options[] = 
{
	{"all", no_argument, 0, 'a'}, 
    	{"boottime", no_argument, 0, 'b'}, 
    	{"allandnumusers", no_argument, 0, 'q'}, 
    	{"runlevel", no_argument, 0, 'r'}, 
    	{"usersloggedin", no_argument, 0, 'u'}, 
    	{0, 0, 0, 0}			
};

usage() {
printf("usage: 'who [-abqru]'\n");
exit(1);
}

main (int argc, char* argv[]) {
int optc;
int index;

while ((optc = getopt_long (argc, argv, "abqru", long_options, (int *) 0)) !=EOF) {
	switch (optc) {
	    case 0:
		break; 
	    case 'a':
	        a=1;
		break;
	    case 'b':
	        b=1;
		break;
	    case 'q':
	        q=1;
		break;
	    case 'r':
	        r=1;
		break;
	    case 'u':
	        u=1;
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
	thefunc(argc, argv);
}

else if (argc>1) {
	for (index=optind; index<argc; index++) {
		;
	}
	
	argc -= optind;
	argv += optind;
	leftover=index-optind;
	
	if (leftover>=1) {
		usage();
	}
	if (leftover==0) {
		thefunc(argc, argv);
	}

}

}
