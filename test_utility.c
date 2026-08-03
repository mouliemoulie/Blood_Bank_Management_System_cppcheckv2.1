#include "test_common.h"
#include "utility.h"
static void test_safe_copy_ok(void){char b[16]; ASSERT_STATUS(BMS_STATUS_OK,UtilitySafeStringCopy(b,sizeof b,"BMS")); CU_ASSERT_STRING_EQUAL(b,"BMS");}
static void test_safe_copy_small(void){char b[3]; ASSERT_STATUS(BMS_STATUS_BUFFER_TOO_SMALL,UtilitySafeStringCopy(b,sizeof b,"BMS")); CU_ASSERT_STRING_EQUAL(b,"");}
static void test_parse_uint32_ok(void){uint32_t v=0; ASSERT_STATUS(BMS_STATUS_OK,UtilityParseUint32("123",&v)); CU_ASSERT_EQUAL(v,123U);}
static void test_parse_uint32_bad(void){uint32_t v=0; ASSERT_STATUS(BMS_STATUS_INVALID_DATA,UtilityParseUint32("12x",&v));}
static void test_trim(void){char s[]="  Blood Bank  ";UtilityTrimWhitespace(s);CU_ASSERT_STRING_EQUAL(s,"Blood Bank");}
static void test_leap_year(void){CU_ASSERT_TRUE(UtilityIsLeapYear(2024U));CU_ASSERT_FALSE(UtilityIsLeapYear(2100U));}
static void test_days_month(void){CU_ASSERT_EQUAL(UtilityDaysInMonth(2024U,2U),29U);CU_ASSERT_EQUAL(UtilityDaysInMonth(2023U,2U),28U);}
static void test_compare_dates(void){BmsDate_t a={2026,1,1},b={2026,2,1};CU_ASSERT(UtilityCompareDates(&a,&b)<0);CU_ASSERT_EQUAL(UtilityCompareDates(&a,&a),0);}
static void test_checksum(void){const char s[]="BloodBank";CU_ASSERT_NOT_EQUAL(UtilityCalculateChecksum(s,strlen(s)),0U);CU_ASSERT_EQUAL(UtilityCalculateChecksum(NULL,5U),0U);}
void RegisterUtilityTests(void){CU_pSuite s=CU_add_suite("Utility",NULL,NULL);CU_add_test(s,"safe copy",test_safe_copy_ok);CU_add_test(s,"small buffer",test_safe_copy_small);CU_add_test(s,"parse integer",test_parse_uint32_ok);CU_add_test(s,"reject bad integer",test_parse_uint32_bad);CU_add_test(s,"trim",test_trim);CU_add_test(s,"leap year",test_leap_year);CU_add_test(s,"days in month",test_days_month);CU_add_test(s,"compare dates",test_compare_dates);CU_add_test(s,"checksum",test_checksum);}
