#include<time.h>
#include<stdio.h>
#include "../include/timeutils.h"

int is_date_after(char *date_a, char *date_b, char *format){

    struct tm date_a_ts;
    struct tm date_b_ts;

    time_t a_mktime;
    time_t b_mktime;

    double diff;


    strptime(date_a, format, &date_a_ts);
    strptime(date_b, format, &date_b_ts);

    date_a_ts.tm_year += 1900;
    date_b_ts.tm_year += 1900;

    a_mktime = mktime(&date_a_ts);
    b_mktime = mktime(&date_b_ts);

    diff = difftime(a_mktime, b_mktime);

    if (diff > 0){ return 1;}

    return 0;

}