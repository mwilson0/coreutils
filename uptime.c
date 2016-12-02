#include<stdio.h>
#include<getopt.h>
#include<stdlib.h>
#include<ctype.h>
#include<time.h>
#include<utmp.h> // for logged in users
#include<sys/types.h>
#include<string.h>

// uptime clone. 0.2. matthew wilson. 2015-16. 
// uptime clone from spec as used in procps 3.2.8 by Greenfield and Johnson. No warranty. License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>

int uptimefunc();
int filechecker(FILE** fp, FILE** fp2);
void usage();
int v = 0, p = 0;
char* program_name;
char* version = "0.2";

int filechecker(FILE** fp, FILE** fp2) {
	static int fail = 0;
	char* uptimefile = "/proc/uptime";
	char* loadavgfile = "/proc/loadavg";

	if (! (*fp = fopen(uptimefile, "r"))) {
		fprintf(stderr, "%s read error\n", uptimefile);
		fail = 1;
	}

	if (! (*fp2 = fopen(loadavgfile, "r"))) {
		fprintf(stderr, "%s read error\n", loadavgfile);
		fail = 1;
	}

	if (fail == 1)
		return(1);
	else 
		return(0);
}

int uptimefunc() {
	FILE* fp; 
	FILE* fp2;

	// first check the files we'll be reading
	if (filechecker(&fp, &fp2) != 0) {
		return(1);
	}

	struct utmp *ii;
	int numusers = 0;

	// first scan utmp file for number of users
	// ut_type determines a login "class" of a user.
	// ut_type number 7 is logged in user

	setutent();
	
	while ((ii = getutent()) != NULL) {
		if (ii->ut_type == 7) {
			numusers++;
		}	
	}
	
	endutent();

	// now time display

	struct tm *info; // struct tm has time info
	time_t curtime;
	curtime = time(NULL);
	info = localtime(&curtime);

	// -p pretty option
	if (p != 1) {
		printf("%d:%02d:%02d ", info->tm_hour, info->tm_min, info->tm_sec);
	}
	// now the uptime

	float nums[2];

	fscanf(fp, "%g %g", &nums[0], &nums[1]);
	fclose(fp);

	if (nums[0] < 3600) {
		printf("up %d minutes", (int)(nums[0] / 60));
	}

	else if (nums[0] >= 3600 && nums[0] < 86400 ) {
		printf("up %i hours, ", (int)(nums[0] / (60 * 60)));
		printf("%d minutes", ((int)nums[0] % 3600) / 60);
	}

	else if (nums[0] >= 86400) { 
		int daystotal = ((int)nums[0] / (60 * 60)) / 24;   // num of days
		int hourstotal = ((int)nums[0] / (60 * 60));     // num of hours
		int hoursinxdays = daystotal * 24;		   	
		int leftoverhours = hourstotal - hoursinxdays;
		int mins = ((int)nums[0] % 3600) / 60;
	
		printf("up %d days, %d:%02d", daystotal, leftoverhours, mins);
		
		if (p == 1) {
			printf("\n");
		}
	}

	// finally the load averages
	
	float numz[3];

	fscanf(fp2, "%g %g %g", &numz[0], &numz[1], &numz[2]);
	fclose(fp2);

	// print the rest
	char userphrase[7] = "user";

	if (numusers > 1) {
		userphrase[strlen(userphrase)] = 's';
	}

	if (p != 1) {
		printf(", %d %s, load average: %.2f %.2f %.2f\n", numusers, userphrase, numz[0], numz[1], numz[2]);
	}

	return(0);
}

static struct option const long_options[] = 
{
    {"version", no_argument, 0, 'V'}, 
    {"pretty", no_argument, 0, 'p'},
    {0, 0, 0, 0}			
};

void usage() {
	printf("usage: '%s [-p | --pretty] [-V | --version]'\n", program_name);
	exit(EXIT_FAILURE);
}

int main (int argc, char* argv[]) {
	program_name = argv[0];
	int index;
	int optc;
	
	while ((optc = getopt_long (argc, argv, "Vp", long_options, (int *) 0)) != EOF) {
		switch (optc) {
	        	case 'V':
	        	    v = 1;
			    break;
			case 'p':
			    p = 1;
			    break;
	    		case '?':
	    		    usage();
			    break;
			default:	
			    usage();
			    break;
		}
	}

	argc -= optind;
	argv += optind;

	for (index = optind; index < argc; index++) {
		;
	}
	
	if (argc > 0) {
		usage();
	}

	if (argc == 0) {
		if (v == 1) {
			printf("version: %s\n", version);
		}
		else {	
			if (uptimefunc() != 0) {
				exit(EXIT_FAILURE);
			}
		}
		exit(EXIT_SUCCESS);
	}
}
