/*
 * sysutils.c
 *
 *  Created on: 06.09.2014
 *      Author: veni
 */
#include "sysutils.h"


double time_diff(struct timeval *t1,struct timeval *t2) {
  //  time_difference in seconds:
  return  (( (double) t2->tv_sec -(double) t1->tv_sec)*1000000. + ((double)t2->tv_usec - (double)t1->tv_usec))/ 1000000.;

}


