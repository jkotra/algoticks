#include <stdio.h>
#include <check.h>                  
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include "../include/dtypes.h"

#include "../include/debug.h"
#include "../include/misc.h"
#include "../include/timeutils.h"
#include "../include/csvutils.h"
#include "../include/parser.h"
#include "../include/dashboard.h"

/*

Tests for:



from misc.c:
void write_simresult_to_csv(algoticks_simresult simresult);
algoticks_config filter_boundaries(algoticks_config config, int is_short);
int is_target_hit(algoticks_dashboard dashboard, float target);
int is_stoploss_hit(algoticks_dashboard dashboard, float stoploss);
void convert_to_lowercase(char *str);
void remove_quotes(char *str);
void chomp(char *s);

from dashboard.c:
float getPnL(algoticks_dashboard dashboard)

from timeutils.c:
int is_date_over_or_eq_intraday(char *date, int intraday_hour, int intraday_min);
int is_date_after(char *date_a, char *date_b);
int get_time_with_sscanf_from_string(char* date, struct tm *time_struct);

from csvutls.c:

int is_quoted(char *str);
*/


START_TEST
(test_it_works_2_plus_2) {

  ck_assert_int_eq((2+2), 4);

}
END_TEST


START_TEST
(__misc_c__write_simresult_to_csv) {

  algoticks_simresult sr = {0};
  write_simresult_to_csv(&sr);
  int res = is_file_exists("results.csv");

  ck_assert_int_eq(res, true);
}
END_TEST


START_TEST
(__misc_c__filter_boundaries) {
  
  algoticks_config x = parse_config_from_json("config.json");

  x.target = 15;
  x.stoploss = 20;

  filter_boundaries(&x, false);
  ck_assert_int_eq(x.target, 15);
  ck_assert_int_eq(x.stoploss, -20);

  filter_boundaries(&x,true);
  ck_assert_int_eq(x.target, -15);
  ck_assert_int_eq(x.stoploss, 20);

}
END_TEST

START_TEST
(__misc_c__is_target_hit) {
   
  algoticks_dashboard d;

  d.a = 10;
  d.b = 15;
  d.is_short = false;

  // NOTE: target and stoploss are abs. vals.

  ck_assert_int_eq(is_target_hit(d, 3), true);
  ck_assert_int_eq(is_target_hit(d, 7), false);


  d.a = 10;
  d.b = 7;
  d.is_short = true;

  ck_assert_int_eq(is_target_hit(d, -2), true);
  ck_assert_int_eq(is_target_hit(d, -8), false);

}
END_TEST

START_TEST
(__misc_c__is_stoploss_hit) {

  algoticks_dashboard d;

  d.a = 10;
  d.b = 15;
  d.is_short = false;

  // NOTE: target and stoploss are abs. vals.

  ck_assert_int_eq(is_stoploss_hit(d, 3), false);
  ck_assert_int_eq(is_stoploss_hit(d, 7), true);


  d.a = 10;
  d.b = 7;
  d.is_short = true;

  ck_assert_int_eq(is_stoploss_hit(d, -2), false);
  ck_assert_int_eq(is_stoploss_hit(d, -8), true);

}
END_TEST



START_TEST
(__misc_c__convert_to_lowercase) {

  char str[12] = "TeStStRiNg";
  convert_to_lowercase(str);

  ck_assert_str_eq(str, "teststring");

}
END_TEST


START_TEST
(__misc_c__remove_quotes) {
   
  char str[7] = "\"test\"";
  remove_quotes(str);
  ck_assert_str_eq(str, "test");

}
END_TEST



START_TEST
(__misc_c__chomp) {
  
  char str[7] = "test\n";
  chomp(str);
  ck_assert_str_eq(str, "test");

}
END_TEST


START_TEST
(__dashboard_c__getPnL) {
  
  algoticks_dashboard d;
  d.a = 10;
  d.b = 15;
  d.q = 100;
  d.is_short = false;
  
  ck_assert_float_eq(getPnL(d), (5*100));

  d.a = 10;
  d.b = 8;
  d.q = 100;
  d.is_short = false;

  ck_assert_float_eq(getPnL(d), (-2*100));

  d.a = 10;
  d.b = 8;
  d.q = 100;
  d.is_short = true;

  ck_assert_float_eq(getPnL(d), (2*100));

  d.a = 10;
  d.b = 12;
  d.q = 100;
  d.is_short = true;

  ck_assert_float_eq(getPnL(d), (-2*100));

}
END_TEST

START_TEST
(__timeutils_c__is_date_over_or_eq_intraday) {
  
  ck_assert_int_eq(is_date_over_or_eq_intraday("2015-02-02 09:25:00", 15, 15), false);
  ck_assert_int_eq(is_date_over_or_eq_intraday("2015-02-02 15:29:00", 15, 15), true);

}
END_TEST


START_TEST
(__timeutils_c__is_date_after) {
  
  //is a > b
  ck_assert_int_eq(is_date_after("2019-02-02 09:25:00", "2015-02-02 09:20:00"), true);
  ck_assert_int_eq(is_date_after("2014-02-02 09:25:00", "2015-02-02 09:20:00"), false);

  ck_assert_int_eq(is_date_before("2014-01-01 09:25:00", "2015-01-01 09:00:00"), true);
  ck_assert_int_eq(is_date_before("2013-01-01 09:25:00", "2010-01-01 09:00:00"), false);

}
END_TEST



START_TEST
(__timeutils_c__get_time_with_sscanf_from_string) {
  
  struct tm time_test;

  get_time_with_sscanf_from_string("2015-02-02 09:25:51", &time_test);

  ck_assert_int_eq(time_test.tm_year, 2015 - 1900);
  ck_assert_int_eq(time_test.tm_mon, 2 - 1);
  ck_assert_int_eq(time_test.tm_mday, 2);

  ck_assert_int_eq(time_test.tm_hour, 9);
  ck_assert_int_eq(time_test.tm_min, 25);
  ck_assert_int_eq(time_test.tm_sec, 51);

}
END_TEST


START_TEST
(__csvutils_c__is_quoted) {

  char str[10] = "\"test";
  char str2[10] = "test";

  ck_assert_int_eq(is_quoted(str), true); 
  ck_assert_int_eq(is_quoted(str2), false); 

}
END_TEST

START_TEST(__csvutils_c__parse_header_DOHLC)
{

  algoticks_config config = parse_config_from_json("config.json");
  algoticks_settings settings = parse_settings_from_json("settings.json");
  config.skip_header = false;
  reset_header_skip();

  algoticks_row row = {0};

  FILE *f = fopen("example_DOHLC.csv", "r");

  read_csv(&settings, &config, f, "example_DOHLC.csv", &row, 0);
  ck_assert_int_eq(is_mapped(0, "date"), true);
  ck_assert_int_eq(is_mapped(1, "open"), true);
  ck_assert_int_eq(is_mapped(2, "high"), true);
  ck_assert_int_eq(is_mapped(3, "low"), true);
  ck_assert_int_eq(is_mapped(4, "close"), true);
}
END_TEST

START_TEST(__csvutils_c__parse_header_OCDHL)
{

  algoticks_config config = parse_config_from_json("config.json");
  algoticks_settings settings = parse_settings_from_json("settings.json");
  config.skip_header = false;
  reset_header_skip();

  algoticks_row row = {0};

  FILE *f = fopen("example_OCDHL.csv", "r");

  read_csv(&settings, &config, f, "example_OCDHL.csv", &row, 0);
  ck_assert_int_eq(is_mapped(0, "open"), true);
  ck_assert_int_eq(is_mapped(1, "close"), true);
  ck_assert_int_eq(is_mapped(2, "date"), true);
  ck_assert_int_eq(is_mapped(3, "high"), true);
  ck_assert_int_eq(is_mapped(4, "low"), true);
}
END_TEST

START_TEST(__csvutils_c__parse_header_ODHLC)
{

  algoticks_config config = parse_config_from_json("config.json");
  algoticks_settings settings = parse_settings_from_json("settings.json");
  config.skip_header = false;
  reset_header_skip();

  algoticks_row row = {0};

  FILE *f = fopen("example_ODHLC.csv", "r");

  read_csv(&settings, &config, f, "example_ODHLC.csv", &row, 0);
  ck_assert_int_eq(is_mapped(0, "open"), true);
  ck_assert_int_eq(is_mapped(1, "date"), true);
  ck_assert_int_eq(is_mapped(2, "high"), true);
  ck_assert_int_eq(is_mapped(3, "low"), true);
  ck_assert_int_eq(is_mapped(4, "close"), true);
}
END_TEST

Suite *algoticks_suite(void) {
  Suite *s;
  TCase *tc_core;

  s = suite_create("algoticks");
  tc_core = tcase_create("Core");

  tcase_add_test(tc_core, test_it_works_2_plus_2);
  tcase_add_test(tc_core, __misc_c__write_simresult_to_csv);
  tcase_add_test(tc_core, __misc_c__filter_boundaries);
  tcase_add_test(tc_core, __misc_c__is_target_hit);
  tcase_add_test(tc_core, __misc_c__is_stoploss_hit);
  tcase_add_test(tc_core, __misc_c__convert_to_lowercase);
  tcase_add_test(tc_core, __misc_c__remove_quotes);
  tcase_add_test(tc_core, __misc_c__chomp);
  tcase_add_test(tc_core, __dashboard_c__getPnL);

  tcase_add_test(tc_core, __timeutils_c__is_date_after);
  tcase_add_test(tc_core, __timeutils_c__is_date_over_or_eq_intraday);
  tcase_add_test(tc_core, __timeutils_c__get_time_with_sscanf_from_string);

  tcase_add_test(tc_core, __csvutils_c__is_quoted);
  tcase_add_test(tc_core, __csvutils_c__parse_header_DOHLC);
  tcase_add_test(tc_core, __csvutils_c__parse_header_OCDHL);
  tcase_add_test(tc_core, __csvutils_c__parse_header_ODHLC);

  suite_add_tcase(s, tc_core);

  return s;
}

int main(void) {
  int no_failed = 0;                   
  Suite *s;                            
  SRunner *runner;                     

  s = algoticks_suite();                   
  runner = srunner_create(s);          

  srunner_run_all(runner, CK_NORMAL);  
  no_failed = srunner_ntests_failed(runner); 
  srunner_free(runner);                      
  return (no_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;  
}