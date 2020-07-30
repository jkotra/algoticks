#define _XOPEN_SOURCE
#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <string.h>
#include "../include/dtypes.h"
#include "../include/csvutils.h"
#include "../include/timeutils.h"

const char *strp_format_1 = "%Y-%m-%d %H:%M:%S";
const char *scanf_format_notime = "%4d-%2d-%2d";
const char *scanf_time_format_1 = "%4d-%2d-%2d %2d:%2d:%2d";

const char *strp_format_2 = "%Y/%m/%d %H:%M:%S";
const char *scanf_format2_notime = "%4d/%2d/%2d";
const char *scanf_time_format_2 = "%4d/%2d/%2d %2d:%2d:%2d";

int is_date_over_or_eq_intraday(char *date, int intraday_hour, int intraday_min)
{
    
    struct tm date_ts;
    
    #ifdef _WIN32
    if (!get_time_with_sscanf_from_string(date, &date_ts))
    {
        return -1;
    }

    #else
    if (!strptime(date, strp_format_1, &date_ts))
    {
        if (!strptime(date, strp_format_2, &date_ts))
        {
            return -1;
        }
    }
    #endif


    date_ts.tm_year += 1900;

    if ((date_ts.tm_hour >= intraday_hour) == true && (date_ts.tm_min >= intraday_min) == true)
    {
        return true;
    }
    else
    {
        return false;
    }
}

int is_date_after(char *date_a, char *date_b)
{

    struct tm date_a_ts;
    struct tm date_b_ts;

    time_t x;
    time_t y;

    double diff;

    if(!get_time_with_sscanf_from_string(date_a, &date_a_ts)){
        return -1;
    }

    if(!get_time_with_sscanf_from_string(date_b, &date_b_ts)){
        return -1;
    }

    x = mktime(&date_a_ts);
    y = mktime(&date_b_ts);

    diff = difftime(x, y);

    if (diff >= 0){
        return true;
    }

    return false;
}

int is_date_before(char *date_a, char *date_b)
{

    struct tm date_a_ts;
    struct tm date_b_ts;

    time_t x;
    time_t y;

    if(!get_time_with_sscanf_from_string(date_a, &date_a_ts)){
        return -1;
    }

    if(!get_time_with_sscanf_from_string(date_b, &date_b_ts)){
        return -1;
    }


    x = mktime(&date_a_ts);
    y = mktime(&date_b_ts);

    double diff = difftime(x,y);

    if (diff <= 0){
        return true;
    }

    return false;

}

int get_time_with_sscanf_from_string(char* date, struct tm *time_struct){

    int year = 0, month = 0, day = 0, hour = 0, min = 0, sec = 0;      
   
   if ( sscanf(date, scanf_time_format_1, &year, &month, &day, &hour, &min, &sec) == 6 ) {
       time_struct->tm_year = year;
       time_struct->tm_mon = month;
       time_struct->tm_mday = day;
       time_struct->tm_hour = hour;
       time_struct->tm_min = min;
       time_struct->tm_sec = sec;

       return true;
   }
   else if ( sscanf(date, scanf_time_format_2, &year, &month, &day, &hour, &min, &sec) == 6 ) {

       time_struct->tm_year = year;
       time_struct->tm_mon = month;
       time_struct->tm_mday = day;
       time_struct->tm_hour = hour;
       time_struct->tm_min = min;
       time_struct->tm_sec = sec;

       return true;
   }

    else if ( sscanf(date, scanf_format_notime, &year, &month, &day) == 3 ) {

       time_struct->tm_year = year;
       time_struct->tm_mon = month;
       time_struct->tm_mday = day;
       time_struct->tm_hour = hour;
       time_struct->tm_min = min;
       time_struct->tm_sec = sec;

       return true;
   }

    else if ( sscanf(date, scanf_format2_notime, &year, &month, &day) == 3 ) {

       time_struct->tm_year = year;
       time_struct->tm_mon = month;
       time_struct->tm_mday = day;
       time_struct->tm_hour = hour;
       time_struct->tm_min = min;
       time_struct->tm_sec = sec;

       return true;
   }
   else{
       return false;
   }

}

int sync_curr(algoticks_settings settings, algoticks_config config,  FILE *f, char* fname, char *date, int seek_offset, int debug){

    int curr = seek_offset;
    if (debug) { printf("finding %s in %s\n", date, fname); }

    while(curr != EOF || curr != -1){
        struct Row r;
        curr = read_csv(settings, config, f, fname, &r, curr);
        if (debug){ printf("is %s > %s : %d\n", r.date, date, is_date_after(r.date, date)); }

        if (is_date_after(r.date, date) == true){
            return curr;
        }
        else if (is_date_after(r.date, date) == -1){
            break;
        }

    }

    //if nothing, return null
    return -1;

}