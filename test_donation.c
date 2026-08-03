#include "test_common.h"
#include "donation_management.h"
static void test_donation_initialize_search_missing(void){BmsDonationContext_t c;BmsDonation_t d;ASSERT_STATUS(BMS_STATUS_OK,DonationManagementInitialize(&c));ASSERT_STATUS(BMS_STATUS_NOT_FOUND,DonationManagementSearchById(&c,1U,&d));DonationManagementDeinitialize(&c);}
void RegisterDonationTests(void){CU_pSuite s=CU_add_suite("Donation",NULL,NULL);CU_add_test(s,"empty search",test_donation_initialize_search_missing);}
