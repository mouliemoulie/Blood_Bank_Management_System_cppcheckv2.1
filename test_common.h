#ifndef BMS_TEST_COMMON_H
#define BMS_TEST_COMMON_H
#include <CUnit/CUnit.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "common.h"

#define ASSERT_STATUS(expected, expression) CU_ASSERT_EQUAL((expression), (expected))

static inline BmsHospital_t TestHospital(BmsHospitalId_t id, const char *name)
{
    BmsHospital_t h;
    (void)memset(&h, 0, sizeof(h));
    h.hospitalId = id;
    (void)snprintf(h.name, sizeof(h.name), "%s", name);
    (void)snprintf(h.location, sizeof(h.location), "Chennai");
    (void)snprintf(h.address, sizeof(h.address), "1 Main Road, Chennai");
    (void)snprintf(h.contactNumber, sizeof(h.contactNumber), "9876543210");
    (void)snprintf(h.email, sizeof(h.email), "hospital%u@test.com", id);
    h.isActive = true;
    return h;
}

static inline BmsDonor_t TestDonor(BmsDonorId_t id, const char *name, BmsBloodGroup_t group)
{
    BmsDonor_t d;
    (void)memset(&d, 0, sizeof(d));
    d.donorId=id; (void)snprintf(d.name,sizeof(d.name),"%s",name);
    d.age=25U; d.weightKg=65U; d.bloodGroup=group;
    (void)snprintf(d.phone,sizeof(d.phone),"9876543210");
    (void)snprintf(d.email,sizeof(d.email),"donor%u@test.com",id);
    (void)snprintf(d.address,sizeof(d.address),"2 Main Road, Chennai");
    d.lastDonationDate=(BmsDate_t){2025U,1U,1U}; d.isEligible=true; d.isActive=true;
    return d;
}
#endif
