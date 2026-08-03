/**
 * @file report_management.h
 * @author Suriya Prakash R
 * @version 2.0.0
 * @brief MIS and operational report generation APIs.
 */

#ifndef BMS_REPORT_MANAGEMENT_H
#define BMS_REPORT_MANAGEMENT_H

#include "blood_inventory.h"
#include "blood_request_management.h"
#include "common.h"
#include "donation_management.h"
#include "donor_management.h"
#include "emergency_alert_management.h"
#include "hospital_management.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    uint32_t totalDonors;
    uint32_t eligibleDonors;
    uint32_t totalHospitals;
    uint32_t totalInventoryUnits;
    uint32_t pendingRequests;
    uint32_t fulfilledRequests;
    uint32_t totalDonations;
    uint32_t unresolvedAlerts;
} BmsSummaryReport_t;

BmsStatus_t ReportManagementBuildSummary(
    const BmsDonorContext_t *donorContext,
    const BmsHospitalContext_t *hospitalContext,
    const BmsInventoryContext_t *inventoryContext,
    const BmsBloodRequestContext_t *requestContext,
    const BmsDonationContext_t *donationContext,
    const BmsEmergencyAlertContext_t *alertContext,
    BmsSummaryReport_t *summary);

BmsStatus_t ReportManagementGenerateInventoryReport(
    BmsInventoryContext_t *context,
    const char *outputPath);
BmsStatus_t ReportManagementGenerateDonorReport(
    BmsDonorContext_t *context,
    const char *outputPath);
BmsStatus_t ReportManagementGenerateHospitalReport(
    BmsHospitalContext_t *context,
    const char *outputPath);
BmsStatus_t ReportManagementGenerateBloodRequestReport(
    BmsBloodRequestContext_t *context,
    const char *outputPath);
BmsStatus_t ReportManagementGenerateDonationReport(
    BmsDonationContext_t *context,
    const char *outputPath);
BmsStatus_t ReportManagementGenerateEmergencyAlertReport(
    BmsEmergencyAlertContext_t *context,
    const char *outputPath);
BmsStatus_t ReportManagementPrintSummary(
    const BmsSummaryReport_t *summary);

#ifdef __cplusplus
}
#endif

#endif /* BMS_REPORT_MANAGEMENT_H */
