#include "test_common.h"
#include "common_validation.h"
static void test_name(void){CU_ASSERT_TRUE(ValidateName("Arun Kumar"));CU_ASSERT_FALSE(ValidateName("A1"));}
static void test_username(void){CU_ASSERT_TRUE(ValidateUsername("surya_01"));CU_ASSERT_FALSE(ValidateUsername("ab"));}
static void test_password(void){CU_ASSERT_TRUE(ValidatePassword("Strong@123"));CU_ASSERT_FALSE(ValidatePassword("password"));}
static void test_phone(void){CU_ASSERT_TRUE(ValidatePhoneNumber("9876543210"));CU_ASSERT_FALSE(ValidatePhoneNumber("98765"));CU_ASSERT_FALSE(ValidatePhoneNumber("98A6543210"));}
static void test_email(void){CU_ASSERT_TRUE(ValidateEmail("user@test.com"));CU_ASSERT_FALSE(ValidateEmail("user@"));CU_ASSERT_FALSE(ValidateEmail("@test.com"));}
static void test_blood_group(void){CU_ASSERT_TRUE(ValidateBloodGroup("A+"));CU_ASSERT_TRUE(ValidateBloodGroup("A Rh Positive"));CU_ASSERT_TRUE(ValidateBloodGroup("A1+"));CU_ASSERT_FALSE(ValidateBloodGroup("X+"));}
static void test_date(void){CU_ASSERT_TRUE(ValidateDate("2024-02-29"));CU_ASSERT_FALSE(ValidateDate("2023-02-29"));}
static void test_age_weight_units(void){CU_ASSERT_TRUE(ValidateAge(25U));CU_ASSERT_FALSE(ValidateAge(10U));CU_ASSERT_TRUE(ValidateWeight(60U));CU_ASSERT_TRUE(ValidateUnits(1U));CU_ASSERT_FALSE(ValidateUnits(0U));}
void RegisterValidationTests(void){CU_pSuite s=CU_add_suite("Validation",NULL,NULL);CU_add_test(s,"name",test_name);CU_add_test(s,"username",test_username);CU_add_test(s,"password",test_password);CU_add_test(s,"phone",test_phone);CU_add_test(s,"email",test_email);CU_add_test(s,"blood group",test_blood_group);CU_add_test(s,"date",test_date);CU_add_test(s,"age weight units",test_age_weight_units);}
