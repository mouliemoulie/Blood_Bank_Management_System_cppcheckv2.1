/** @file donation_management.c @brief Donation management implementation. */
#include "donation_management.h"
#include "file_management.h"
#include "file_names.h"
#include <stdlib.h>
#include <string.h>
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
BmsStatus_t DonationManagementRecord(BmsDonationContext_t*c,BmsDonorContext_t*d,BmsInventoryContext_t*i,const BmsDonation_t*x){BmsBloodInventory_t stock;(void)d;if((c==NULL)||(i==NULL)||(x==NULL))return BMS_STATUS_INVALID_ARGUMENT;if(LinkedListInsertBack(&c->donations,x)!=BMS_STATUS_OK)return BMS_STATUS_MEMORY_ERROR;(void)memset(&stock,0,sizeof(stock));stock.bloodId=x->donationId;stock.bloodGroup=x->bloodGroup;stock.units=x->units;stock.collectionDate=x->donationDate;stock.expiryDate=x->donationDate;stock.isAvailable=true;return BloodInventoryAddStock(i,&stock);}
BmsStatus_t DonationManagementSearchById(const BmsDonationContext_t*c,BmsDonationId_t id,BmsDonation_t*d){void*f=NULL;BmsStatus_t s;if((c==NULL)||(d==NULL))return BMS_STATUS_INVALID_ARGUMENT;s=LinkedListFind(&c->donations,Match,&id,&f);if(s==BMS_STATUS_OK)*d=*(BmsDonation_t*)f;return s;}
BmsStatus_t DonationManagementFindByDonor(BmsDonationContext_t*c,BmsDonorId_t id,BmsDonationVisitor_t v,void*x){BmsLinkedListNode_t*n;if((c==NULL)||(v==NULL))return BMS_STATUS_INVALID_ARGUMENT;for(n=c->donations.head;n;n=n->next)if(((BmsDonation_t*)n->data)->donorId==id){BmsStatus_t s=v(n->data,x);if(s!=BMS_STATUS_OK)return s;}return BMS_STATUS_OK;}
BmsStatus_t DonationManagementTraverse(BmsDonationContext_t*c,BmsDonationVisitor_t v,void*x){BmsLinkedListNode_t*n;if((c==NULL)||(v==NULL))return BMS_STATUS_INVALID_ARGUMENT;for(n=c->donations.head;n;n=n->next){BmsStatus_t s=v(n->data,x);if(s!=BMS_STATUS_OK)return s;}return BMS_STATUS_OK;}
void DonationManagementDeinitialize(BmsDonationContext_t*c){if(c==NULL)return;LinkedListClear(&c->donations);HashTableDeinitialize(&c->donationIdIndex);(void)memset(c,0,sizeof(*c));}
