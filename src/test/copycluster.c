/* -*- Mode: C -*-
  ======================================================================
  FILE: copycluster.c
  CREATOR: eric 15 January 2000
  
  $Id: copycluster.c,v 1.18 2008-02-03 16:10:46 dothebart Exp $
  $Locker:  $
    
 (C) COPYRIGHT 2000 Eric Busboom
 http://www.softwarestudio.org

 The contents of this file are subject to the Mozilla Public License
 Version 1.0 (the "License"); you may not use this file except in
 compliance with the License. You may obtain a copy of the License at
 http://www.mozilla.org/MPL/
 
 Software distributed under the License is distributed on an "AS IS"
 basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See
 the License for the specific language governing rights and
 limitations under the License.
 
 The Original Code is eric. The Initial Developer of the Original
 Code is Eric Busboom


 ======================================================================*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h> /* for printf */
#include <errno.h>
#include <string.h> /* For strerror */
#include <signal.h> /* for signal */
#if defined(HAVE_UNISTD_H)
#include <unistd.h> /* for alarm */
#endif
#include <stdlib.h> /* for exit */

#include <libical/ical.h>
#include <libicalss/icalss.h>

#ifdef SIGALRM

static void sig_alrm(int i){
    (void)i;/*unused*/
    fprintf(stderr,"Could not get lock on file\n");
    exit(1);
}

#endif

/* This program copies a file that holds iCal components to an other file. */


void usage(char* arg0) {
    printf("usage: %s cluster-file1 cluster-file2\n",arg0);
}

int main(int c, char *argv[]){

    icalset *clusterin, *clusterout = NULL;
    icalcomponent *itr;
    int count=0;
    int tostdout = 0;

    if(c < 2 || c > 3){
	usage(argv[0]);
	exit(1);
    }

    if (c == 2){
	tostdout = 1;
    }


    /*icalerror_set_error_state(ICAL_PARSE_ERROR, ICAL_ERROR_NONFATAL);*/

#ifdef SIGALRM
    signal(SIGALRM,sig_alrm);
    alarm(10);
#endif
    clusterin = icalfileset_new(argv[1]);
#ifdef SIGALRM
    alarm(0);
#endif
    if (clusterin == 0){
	printf("Could not open input cluster \"%s\"\n",argv[1]);
	if(icalerrno!= ICAL_NO_ERROR){
          printf("Error: %s\n",icalerror_strerror(icalerrno));
        }
	exit(1);
    }

    if (!tostdout){
#ifdef SIGALRM
        alarm(10);
#endif
	clusterout = icalfileset_new(argv[2]);
#ifdef SIGALRM
	alarm(0);
#endif
	if (clusterout == 0){
	    printf("Could not open output cluster \"%s\"\n",argv[2]);
	    exit(1);
	}
    }


    for (itr = icalset_get_first_component(clusterin);
	 itr != 0;
	 itr = icalset_get_next_component(clusterin)){

        icalerror_set_error_state(ICAL_BADARG_ERROR, ICAL_ERROR_NONFATAL);
	icalrestriction_check(itr);
        icalerror_set_error_state(ICAL_BADARG_ERROR, ICAL_ERROR_DEFAULT);

	if(tostdout){
	    printf("--------------\n%s\n",icalcomponent_as_ical_string(itr));
	} else {
	    icalfileset_add_component(clusterout, icalcomponent_new_clone(itr));
	}
	count++;
    }


    printf("Transfered %d components\n",count);

    icalset_free(clusterin);

    if (!tostdout){
	icalfileset_mark(clusterout);
	icalset_free(clusterout);
    }

     return 0;
}


