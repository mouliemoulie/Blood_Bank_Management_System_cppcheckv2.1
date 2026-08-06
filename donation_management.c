/** @file donation_management.c @brief Donation management implementation. */
#include "donation_management.h"
#include "file_management.h"
#include "file_names.h"
#include <stdlib.h>
#include <string.h>

static bool DonationIsLeapYear(uint32_t year)
{
    if ((year % 400U) == 0U) { return true; }
    if ((year % 100U) == 0U) { return false; }
    return ((year % 4U) == 0U);
}

static uint32_t DonationDaysInMonth(uint32_t year, uint32_t month)
{
    static const uint32_t days[12] =
    {
        31U, 28U, 31U, 30U, 31U, 30U,
        31U, 31U, 30U, 31U, 30U, 31U
    };
    uint32_t result = 0U;
    if ((month >= 1U) && (month <= 12U))
    {
        result = days[month - 1U];
        if ((month == 2U) && DonationIsLeapYear(year)) { result = 29U; }
    }
    return result;
}

static bool DonationAddDaysToDate(const BmsDate_t *sourceDate,
                                  uint32_t daysToAdd,
                                  BmsDate_t *resultDate)
{
    BmsDate_t date;
    uint32_t daysInMonth;

    if ((sourceDate == NULL) || (resultDate == NULL)) { return false; }
    daysInMonth = DonationDaysInMonth(sourceDate->year, sourceDate->month);
    if ((sourceDate->year == 0U) || (daysInMonth == 0U) ||
        (sourceDate->day == 0U) || (sourceDate->day > daysInMonth))
    {
        return false;
    }

    date = *sourceDate;
    while (daysToAdd > 0U)
    {
        daysInMonth = DonationDaysInMonth(date.year, date.month);
        if (date.day < daysInMonth) { ++date.day; }
        else
        {
            date.day = 1U;
            if (date.month < 12U) { ++date.month; }
            else
            {
                if (date.year == UINT32_MAX) { return false; }
                date.month = 1U;
                ++date.year;
            }
        }
        --daysToAdd;
    }
    *resultDate = date;
    return true;
}

static bool Match(const void*d,const void*k){return((const BmsDonation_t*)d)->donationId==*(const BmsDonationId_t*)k;}
BmsStatus_t DonationManagementInitialize(BmsDonationContext_t*c){if(c==NULL)return BMS_STATUS_INVALID_ARGUMENT;(void)memset(c,0,sizeof(*c));LinkedListInitialize(&c->donations,sizeof(BmsDonation_t));HashTableInitialize(&c->donationIdIndex,BMS_HASH_BUCKET_COUNT,sizeof(BmsDonation_t*),BMS_HASH_KEY_UINT32);c->initialized=true;return BMS_STATUS_OK;}
BmsStatus_t DonationManagementLoad(BmsDonationContext_t*c){bool exists=false;uint32_t count=0U,i=0U;BmsDonation_t*a=NULL;BmsStatus_t st;if((c==NULL)||!c->initialized)return BMS_STATUS_NOT_INITIALIZED;(void)FileManagementFileExists(BMS_DONATIONS_FILE,&exists);if(!exists)return BMS_STATUS_OK;st=FileManagementGetRecordCount(BMS_DONATIONS_FILE,sizeof(BmsDonation_t),&count);if(st!=BMS_STATUS_OK)return st;if(count==0U)return BMS_STATUS_OK;a=(BmsDonation_t*)calloc(count,sizeof(*a));if(a==NULL)return BMS_STATUS_MEMORY_ERROR;st=FileManagementReadRecords(BMS_DONATIONS_FILE,a,count,&count,sizeof(*a));for(i=0U;(st==BMS_STATUS_OK)&&(i<count);++i)st=LinkedListInsertBack(&c->donations,&a[i]);free(a);return st;}
BmsStatus_t DonationManagementSave(const BmsDonationContext_t *context)
{
    BmsDonation_t *records = NULL;
    BmsLinkedListNode_t *node = NULL;
    uint32_t index = 0U;
    BmsStatus_t status;

    if ((context == NULL) || (!context->initialized))
    {
        return BMS_STATUS_NOT_INITIALIZED;
    }

    if (context->donations.count == 0U)
    {
        return FileManagementWriteRecords(BMS_DONATIONS_FILE, NULL, 0U,
                                          sizeof(BmsDonation_t));
    }

    records = (BmsDonation_t *)calloc(context->donations.count, sizeof(*records));
    if (records == NULL)
    {
        return BMS_STATUS_MEMORY_ERROR;
    }

    for (node = context->donations.head; node != NULL; node = node->next)
    {
        if (node->data == NULL)
        {
            free(records);
            return BMS_STATUS_INVALID_DATA;
        }
        records[index] = *(const BmsDonation_t *)node->data;
        ++index;
    }

    status = FileManagementWriteRecords(BMS_DONATIONS_FILE, records, index,
                                        sizeof(*records));
    free(records);
    return status;
}
BmsStatus_t DonationManagementRecord(
    BmsDonationContext_t *context,
    BmsDonorContext_t *donorContext,
    BmsInventoryContext_t *inventoryContext,
    const BmsDonation_t *donation)
{
    BmsBloodInventory_t stock;
    BmsStatus_t status;

    (void)donorContext;

    if ((context == NULL) ||
        (inventoryContext == NULL) ||
        (donation == NULL))
    {
        return BMS_STATUS_INVALID_ARGUMENT;
    }

    (void)memset(&stock, 0, sizeof(stock));
    stock.bloodId = donation->donationId;
    stock.bloodGroup = donation->bloodGroup;
    stock.units = donation->units;
    stock.collectionDate = donation->donationDate;
    stock.isAvailable = true;

    if (!DonationAddDaysToDate(&stock.collectionDate,
                               45U,
                               &stock.expiryDate))
    {
        return BMS_STATUS_INVALID_DATA;
    }

    status = BloodInventoryAddStock(inventoryContext, &stock);
    if (status != BMS_STATUS_OK)
    {
        return status;
    }

    status = LinkedListInsertBack(&context->donations, donation);
    return status;
}
BmsStatus_t DonationManagementSearchById(const BmsDonationContext_t*c,BmsDonationId_t id,BmsDonation_t*d){void*f=NULL;BmsStatus_t s;if((c==NULL)||(d==NULL))return BMS_STATUS_INVALID_ARGUMENT;s=LinkedListFind(&c->donations,Match,&id,&f);if(s==BMS_STATUS_OK)*d=*(BmsDonation_t*)f;return s;}
BmsStatus_t DonationManagementFindByDonor(BmsDonationContext_t*c,BmsDonorId_t id,BmsDonationVisitor_t v,void*x){BmsLinkedListNode_t*n;if((c==NULL)||(v==NULL))return BMS_STATUS_INVALID_ARGUMENT;for(n=c->donations.head;n;n=n->next)if(((BmsDonation_t*)n->data)->donorId==id){BmsStatus_t s=v(n->data,x);if(s!=BMS_STATUS_OK)return s;}return BMS_STATUS_OK;}
BmsStatus_t DonationManagementTraverse(BmsDonationContext_t*c,BmsDonationVisitor_t v,void*x){BmsLinkedListNode_t*n;if((c==NULL)||(v==NULL))return BMS_STATUS_INVALID_ARGUMENT;for(n=c->donations.head;n;n=n->next){BmsStatus_t s=v(n->data,x);if(s!=BMS_STATUS_OK)return s;}return BMS_STATUS_OK;}
void DonationManagementDeinitialize(BmsDonationContext_t*c){if(c==NULL)return;LinkedListClear(&c->donations);HashTableDeinitialize(&c->donationIdIndex);(void)memset(c,0,sizeof(*c));}
