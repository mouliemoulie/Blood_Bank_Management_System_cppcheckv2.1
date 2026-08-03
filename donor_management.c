/** @file donor_management.c @brief Donor management implementation. */
#include "donor_management.h"
#include "common_validation.h"
#include "utility.h"

#include "file_management.h"
#include "file_names.h"
#include <stdlib.h>
#include <string.h>

static bool Match(const void*d,const void*k){return((const BmsDonor_t*)d)->donorId==*(const BmsDonorId_t*)k;}
static int32_t CompareName(const void*a,const void*b){return strcmp(((const BmsDonor_t*)a)->name,((const BmsDonor_t*)b)->name);}
typedef struct { BmsDonorVisitor_t visitor; void *context; } DonorVisitAdapter_t;
static BmsStatus_t CallVisitor(void *data, void *context)
{
    DonorVisitAdapter_t *adapter = (DonorVisitAdapter_t *)context;
    return adapter->visitor((const BmsDonor_t *)data, adapter->context);
}
BmsStatus_t DonorManagementInitialize(BmsDonorContext_t*c){if(c==NULL)return BMS_STATUS_INVALID_ARGUMENT;(void)memset(c,0,sizeof(*c));if(LinkedListInitialize(&c->donors,sizeof(BmsDonor_t))!=BMS_STATUS_OK)return BMS_STATUS_MEMORY_ERROR;if(HashTableInitialize(&c->donorIdIndex,BMS_HASH_BUCKET_COUNT,sizeof(BmsDonor_t*),BMS_HASH_KEY_UINT32)!=BMS_STATUS_OK)return BMS_STATUS_MEMORY_ERROR;c->initialized=true;return BMS_STATUS_OK;}
BmsStatus_t DonorManagementLoad(BmsDonorContext_t *context)
{
    bool exists = false;
    uint32_t count = 0U;
    BmsDonor_t *records = NULL;
    BmsStatus_t status;

    if ((context == NULL) || (!context->initialized))
    {
        return BMS_STATUS_NOT_INITIALIZED;
    }
    (void)FileManagementFileExists(BMS_DONORS_FILE, &exists);
    if (!exists)
    {
        return BMS_STATUS_OK;
    }
    status = FileManagementGetRecordCount(BMS_DONORS_FILE,
                                          sizeof(BmsDonor_t), &count);
    if ((status != BMS_STATUS_OK) || (count == 0U))
    {
        return status;
    }
    records = (BmsDonor_t *)calloc(count, sizeof(*records));
    if (records == NULL)
    {
        return BMS_STATUS_MEMORY_ERROR;
    }
    status = FileManagementReadRecords(BMS_DONORS_FILE, records, count,
                                       &count, sizeof(*records));
    for (uint32_t index = 0U;
         (status == BMS_STATUS_OK) && (index < count); ++index)
    {
        status = DonorManagementAdd(context, &records[index]);
    }
    free(records);
    return status;
}
BmsStatus_t DonorManagementSave(const BmsDonorContext_t *context)
{
    BmsDonor_t *records = NULL;
    BmsLinkedListNode_t *node = NULL;
    uint32_t index = 0U;
    BmsStatus_t status;

    if ((context == NULL) || (!context->initialized))
    {
        return BMS_STATUS_NOT_INITIALIZED;
    }

    if (context->donors.count == 0U)
    {
        return FileManagementWriteRecords(BMS_DONORS_FILE, NULL, 0U,
                                          sizeof(BmsDonor_t));
    }

    records = (BmsDonor_t *)calloc(context->donors.count, sizeof(*records));
    if (records == NULL)
    {
        return BMS_STATUS_MEMORY_ERROR;
    }

    for (node = context->donors.head; node != NULL; node = node->next)
    {
        if (node->data == NULL)
        {
            free(records);
            return BMS_STATUS_INVALID_DATA;
        }
        records[index] = *(const BmsDonor_t *)node->data;
        ++index;
    }

    status = FileManagementWriteRecords(BMS_DONORS_FILE, records, index,
                                        sizeof(*records));
    free(records);
    return status;
}
BmsStatus_t DonorManagementAdd(BmsDonorContext_t*c,const BmsDonor_t*d){void*found;if((c==NULL)||(d==NULL))return BMS_STATUS_INVALID_ARGUMENT;if(!ValidateDonorId(d->donorId)||!ValidateName(d->name)||!ValidateBloodGroupValue(d->bloodGroup))return BMS_STATUS_INVALID_DATA;if(LinkedListFind(&c->donors,Match,&d->donorId,&found)==BMS_STATUS_OK)return BMS_STATUS_ALREADY_EXISTS;return LinkedListInsertBack(&c->donors,d);}
BmsStatus_t DonorManagementSearchById(const BmsDonorContext_t*c,BmsDonorId_t id,BmsDonor_t*d){void*f=NULL;BmsStatus_t s;if((c==NULL)||(d==NULL))return BMS_STATUS_INVALID_ARGUMENT;s=LinkedListFind(&c->donors,Match,&id,&f);if(s==BMS_STATUS_OK)*d=*(BmsDonor_t*)f;return s;}
BmsStatus_t DonorManagementUpdate(BmsDonorContext_t*c,const BmsDonor_t*d){if((c==NULL)||(d==NULL))return BMS_STATUS_INVALID_ARGUMENT;return LinkedListUpdate(&c->donors,Match,&d->donorId,d);}
BmsStatus_t DonorManagementDelete(BmsDonorContext_t*c,BmsDonorId_t id){return(c==NULL)?BMS_STATUS_INVALID_ARGUMENT:LinkedListDelete(&c->donors,Match,&id);}
BmsStatus_t DonorManagementCheckEligibility(BmsDonor_t*d,const BmsDate_t*now){if((d==NULL)||(now==NULL))return BMS_STATUS_INVALID_ARGUMENT;d->isEligible=ValidateAge(d->age)&&ValidateWeight(d->weightKg)&&(UtilityCompareDates(&d->lastDonationDate,now)<=0);return BMS_STATUS_OK;}
BmsStatus_t DonorManagementFindByBloodGroup(BmsDonorContext_t*c,BmsBloodGroup_t g,BmsDonorVisitor_t v,void*ctx){BmsLinkedListNode_t*n;if((c==NULL)||(v==NULL))return BMS_STATUS_INVALID_ARGUMENT;for(n=c->donors.head;n!=NULL;n=n->next)if(((BmsDonor_t*)n->data)->bloodGroup==g){BmsStatus_t s=v((const BmsDonor_t*)n->data,ctx);if(s!=BMS_STATUS_OK)return s;}return BMS_STATUS_OK;}
BmsStatus_t DonorManagementTraverse(BmsDonorContext_t *c, BmsDonorVisitor_t v, void *ctx)
{
    DonorVisitAdapter_t adapter;
    if ((c == NULL) || (v == NULL)) { return BMS_STATUS_INVALID_ARGUMENT; }
    adapter.visitor = v;
    adapter.context = ctx;
    return LinkedListTraverse(&c->donors, CallVisitor, &adapter);
}
BmsStatus_t DonorManagementSortByName(BmsDonorContext_t*c){return(c==NULL)?BMS_STATUS_INVALID_ARGUMENT:LinkedListSort(&c->donors,CompareName);}
void DonorManagementDeinitialize(BmsDonorContext_t*c){if(c==NULL)return;LinkedListClear(&c->donors);HashTableDeinitialize(&c->donorIdIndex);(void)memset(c,0,sizeof(*c));}
