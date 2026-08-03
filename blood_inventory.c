/** @file blood_inventory.c @brief Blood inventory implementation. */
#include "blood_inventory.h"
#include "file_management.h"
#include "file_names.h"
#include "utility.h"
#include <stdlib.h>
#include <string.h>
static bool Match(const void*d,const void*k){return((const BmsBloodInventory_t*)d)->bloodId==*(const BmsBloodId_t*)k;}
BmsStatus_t BloodInventoryInitialize(BmsInventoryContext_t*c){if(c==NULL)return BMS_STATUS_INVALID_ARGUMENT;(void)memset(c,0,sizeof(*c));LinkedListInitialize(&c->inventory,sizeof(BmsBloodInventory_t));HashTableInitialize(&c->bloodIdIndex,BMS_HASH_BUCKET_COUNT,sizeof(BmsBloodInventory_t*),BMS_HASH_KEY_UINT32);c->initialized=true;return BMS_STATUS_OK;}
BmsStatus_t BloodInventoryLoad(BmsInventoryContext_t*c){bool exists=false;uint32_t count=0U,i=0U;BmsBloodInventory_t*a=NULL;BmsStatus_t st;if((c==NULL)||!c->initialized)return BMS_STATUS_NOT_INITIALIZED;(void)FileManagementFileExists(BMS_INVENTORY_FILE,&exists);if(!exists)return BMS_STATUS_OK;st=FileManagementGetRecordCount(BMS_INVENTORY_FILE,sizeof(BmsBloodInventory_t),&count);if(st!=BMS_STATUS_OK)return st;if(count==0U)return BMS_STATUS_OK;a=(BmsBloodInventory_t*)calloc(count,sizeof(*a));if(a==NULL)return BMS_STATUS_MEMORY_ERROR;st=FileManagementReadRecords(BMS_INVENTORY_FILE,a,count,&count,sizeof(*a));for(i=0U;(st==BMS_STATUS_OK)&&(i<count);++i)st=BloodInventoryAddStock(c,&a[i]);free(a);return st;}
BmsStatus_t BloodInventorySave(const BmsInventoryContext_t *context)
{
    BmsBloodInventory_t *records = NULL;
    BmsLinkedListNode_t *node = NULL;
    uint32_t index = 0U;
    BmsStatus_t status;

    if ((context == NULL) || (!context->initialized))
    {
        return BMS_STATUS_NOT_INITIALIZED;
    }

    if (context->inventory.count == 0U)
    {
        return FileManagementWriteRecords(BMS_INVENTORY_FILE, NULL, 0U,
                                          sizeof(BmsBloodInventory_t));
    }

    records = (BmsBloodInventory_t *)calloc(context->inventory.count, sizeof(*records));
    if (records == NULL)
    {
        return BMS_STATUS_MEMORY_ERROR;
    }

    for (node = context->inventory.head; node != NULL; node = node->next)
    {
        if (node->data == NULL)
        {
            free(records);
            return BMS_STATUS_INVALID_DATA;
        }
        records[index] = *(const BmsBloodInventory_t *)node->data;
        ++index;
    }

    status = FileManagementWriteRecords(BMS_INVENTORY_FILE, records, index,
                                        sizeof(*records));
    free(records);
    return status;
}
BmsStatus_t BloodInventoryAddStock(BmsInventoryContext_t*c,const BmsBloodInventory_t*r){void*f;if((c==NULL)||(r==NULL))return BMS_STATUS_INVALID_ARGUMENT;if(LinkedListFind(&c->inventory,Match,&r->bloodId,&f)==BMS_STATUS_OK)return BMS_STATUS_ALREADY_EXISTS;return LinkedListInsertBack(&c->inventory,r);}
BmsStatus_t BloodInventoryUpdateStock(BmsInventoryContext_t*c,BmsBloodId_t id,uint32_t u){void*f=NULL;BmsStatus_t s=LinkedListFind(&c->inventory,Match,&id,&f);if(s==BMS_STATUS_OK){((BmsBloodInventory_t*)f)->units=u;((BmsBloodInventory_t*)f)->isAvailable=(u>0U);}return s;}
BmsStatus_t BloodInventoryRemoveStock(BmsInventoryContext_t*c,BmsBloodId_t id,uint32_t u){void*f=NULL;BmsStatus_t s;if(c==NULL)return BMS_STATUS_INVALID_ARGUMENT;s=LinkedListFind(&c->inventory,Match,&id,&f);if(s!=BMS_STATUS_OK)return s;if(((BmsBloodInventory_t*)f)->units<u)return BMS_STATUS_INSUFFICIENT_STOCK;((BmsBloodInventory_t*)f)->units-=u;((BmsBloodInventory_t*)f)->isAvailable=(((BmsBloodInventory_t*)f)->units>0U);return BMS_STATUS_OK;}
BmsStatus_t BloodInventorySearchById(const BmsInventoryContext_t*c,BmsBloodId_t id,BmsBloodInventory_t*r){void*f=NULL;BmsStatus_t s;if((c==NULL)||(r==NULL))return BMS_STATUS_INVALID_ARGUMENT;s=LinkedListFind(&c->inventory,Match,&id,&f);if(s==BMS_STATUS_OK)*r=*(BmsBloodInventory_t*)f;return s;}
BmsStatus_t BloodInventoryGetAvailableUnits(BmsInventoryContext_t*c,BmsBloodGroup_t g,uint32_t*u){BmsLinkedListNode_t*n;uint32_t total=0U;if((c==NULL)||(u==NULL))return BMS_STATUS_INVALID_ARGUMENT;for(n=c->inventory.head;n;n=n->next){BmsBloodInventory_t*r=(BmsBloodInventory_t*)n->data;if(r->bloodGroup==g&&r->isAvailable)total+=r->units;}*u=total;return BMS_STATUS_OK;}
BmsStatus_t BloodInventoryRemoveExpired(BmsInventoryContext_t*c,const BmsDate_t*now,uint32_t*removed){BmsLinkedListNode_t*n;if((c==NULL)||(now==NULL)||(removed==NULL))return BMS_STATUS_INVALID_ARGUMENT;*removed=0U;for(n=c->inventory.head;n;n=n->next){BmsBloodInventory_t*r=(BmsBloodInventory_t*)n->data;if(UtilityCompareDates(&r->expiryDate,now)<0){r->units=0U;r->isAvailable=false;++*removed;}}return BMS_STATUS_OK;}
BmsStatus_t BloodInventoryDetectLowStock(BmsInventoryContext_t*c,uint32_t t,BmsInventoryVisitor_t v,void*x){BmsLinkedListNode_t*n;if((c==NULL)||(v==NULL))return BMS_STATUS_INVALID_ARGUMENT;for(n=c->inventory.head;n;n=n->next){BmsBloodInventory_t*r=(BmsBloodInventory_t*)n->data;if(r->units<=t){BmsStatus_t s=v(r,x);if(s!=BMS_STATUS_OK)return s;}}return BMS_STATUS_OK;}
BmsStatus_t BloodInventoryTraverse(BmsInventoryContext_t*c,BmsInventoryVisitor_t v,void*x){BmsLinkedListNode_t*n;if((c==NULL)||(v==NULL))return BMS_STATUS_INVALID_ARGUMENT;for(n=c->inventory.head;n;n=n->next){BmsStatus_t s=v((BmsBloodInventory_t*)n->data,x);if(s!=BMS_STATUS_OK)return s;}return BMS_STATUS_OK;}
void BloodInventoryDeinitialize(BmsInventoryContext_t*c){if(c==NULL)return;LinkedListClear(&c->inventory);HashTableDeinitialize(&c->bloodIdIndex);(void)memset(c,0,sizeof(*c));}
