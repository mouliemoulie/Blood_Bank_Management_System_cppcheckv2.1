/**
 * @file multithreading.c
 * @brief POSIX-thread based background inventory monitoring.
 */
#include "multithreading.h"

#include <errno.h>
#include <string.h>
#include <time.h>

static BmsStatus_t CountLowStock(const BmsBloodInventory_t *record,
                                 void *context)
{
    uint32_t *count = (uint32_t *)context;
    if ((record == NULL) || (count == NULL))
    {
        return BMS_STATUS_INVALID_ARGUMENT;
    }
    ++(*count);
    return BMS_STATUS_OK;
}

static bool GetCurrentDate(BmsDate_t *date)
{
    time_t now;
    struct tm localTime;

    if (date == NULL)
    {
        return false;
    }

    now = time(NULL);
    if ((now == (time_t)-1) || (localtime_r(&now, &localTime) == NULL))
    {
        return false;
    }

    date->year = (uint16_t)(localTime.tm_year + 1900);
    date->month = (uint8_t)(localTime.tm_mon + 1);
    date->day = (uint8_t)localTime.tm_mday;
    return true;
}

static void RunInventoryCheck(BmsThreadController_t *controller)
{
    BmsDate_t currentDate = { 0U, 0U, 0U };
    uint32_t expiredCount = 0U;
    uint32_t lowStockCount = 0U;

    if ((controller == NULL) || (controller->inventory == NULL) ||
        (!GetCurrentDate(&currentDate)))
    {
        return;
    }

    if (BloodInventoryRemoveExpired(controller->inventory,
                                    &currentDate,
                                    &expiredCount) == BMS_STATUS_OK)
    {
        controller->lastExpiredCount = expiredCount;
        if (expiredCount > 0U)
        {
            (void)BloodInventorySave(controller->inventory);
        }
    }

    if (BloodInventoryDetectLowStock(controller->inventory,
                                     controller->lowStockThreshold,
                                     CountLowStock,
                                     &lowStockCount) == BMS_STATUS_OK)
    {
        controller->lastLowStockCount = lowStockCount;
    }
}

static void *InventoryMonitorMain(void *argument)
{
    BmsThreadController_t *controller = (BmsThreadController_t *)argument;

    if (controller == NULL)
    {
        return NULL;
    }

    RunInventoryCheck(controller);

    (void)pthread_mutex_lock(&controller->controlMutex);
    while (!controller->stopRequested)
    {
        struct timespec wakeTime;
        int waitResult;

        if (clock_gettime(CLOCK_REALTIME, &wakeTime) != 0)
        {
            break;
        }
        wakeTime.tv_sec += (time_t)controller->intervalSeconds;

        waitResult = pthread_cond_timedwait(&controller->stopCondition,
                                            &controller->controlMutex,
                                            &wakeTime);
        if (controller->stopRequested)
        {
            break;
        }
        if ((waitResult == ETIMEDOUT) || (waitResult == 0))
        {
            (void)pthread_mutex_unlock(&controller->controlMutex);
            RunInventoryCheck(controller);
            (void)pthread_mutex_lock(&controller->controlMutex);
        }
    }
    (void)pthread_mutex_unlock(&controller->controlMutex);
    return NULL;
}

BmsStatus_t BmsThreadControllerStart(BmsThreadController_t *controller,
                                     BmsInventoryContext_t *inventory,
                                     uint32_t intervalSeconds,
                                     uint32_t lowStockThreshold)
{
    int result;

    if ((controller == NULL) || (inventory == NULL) ||
        (intervalSeconds == 0U))
    {
        return BMS_STATUS_INVALID_ARGUMENT;
    }

    (void)memset(controller, 0, sizeof(*controller));
    controller->inventory = inventory;
    controller->intervalSeconds = intervalSeconds;
    controller->lowStockThreshold = lowStockThreshold;

    if (pthread_mutex_init(&controller->controlMutex, NULL) != 0)
    {
        return BMS_STATUS_INTERNAL_ERROR;
    }
    if (pthread_cond_init(&controller->stopCondition, NULL) != 0)
    {
        (void)pthread_mutex_destroy(&controller->controlMutex);
        return BMS_STATUS_INTERNAL_ERROR;
    }

    result = pthread_create(&controller->thread,
                            NULL,
                            InventoryMonitorMain,
                            controller);
    if (result != 0)
    {
        (void)pthread_cond_destroy(&controller->stopCondition);
        (void)pthread_mutex_destroy(&controller->controlMutex);
        return BMS_STATUS_INTERNAL_ERROR;
    }

    controller->started = true;
    return BMS_STATUS_OK;
}

void BmsThreadControllerStop(BmsThreadController_t *controller)
{
    if ((controller != NULL) && controller->started)
    {
        (void)pthread_mutex_lock(&controller->controlMutex);
        controller->stopRequested = true;
        (void)pthread_cond_signal(&controller->stopCondition);
        (void)pthread_mutex_unlock(&controller->controlMutex);
        (void)pthread_join(controller->thread, NULL);
        (void)pthread_cond_destroy(&controller->stopCondition);
        (void)pthread_mutex_destroy(&controller->controlMutex);
        controller->started = false;
    }
}
