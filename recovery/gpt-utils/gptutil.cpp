#include <iostream>
#include "gpt-utils.h"

using namespace std;

enum part_attr_type {
        ATTR_SLOT_ACTIVE = 0,
        ATTR_BOOT_SUCCESSFUL,
        ATTR_UNBOOTABLE,
};

struct gpt_disk* boot_ctl_get_disk_info(char *partition) {
   struct gpt_disk *disk = NULL;
    if (!partition)
        return NULL;
    disk = gpt_disk_alloc();
    if (!disk) {
        cout << __func__ << ": Failed to alloc disk" << endl;
        goto error;
    }
    if (gpt_disk_get_disk_info(partition, disk)) {
        cout << "failed to get disk info for " << partition << endl;
        goto error;
    }
    return disk;
error:
    if (disk)
        gpt_disk_free(disk);
    return NULL;
}

//Get the value of one of the attribute fields for a partition.
int get_partition_attribute(char *partname,
        enum part_attr_type part_attr)
{
    struct gpt_disk *disk = NULL;
    uint8_t *pentry = NULL;
    int retval = -1;
    uint8_t *attr = NULL;
    if (!partname) {
        cout << "No partname!" << endl;
        goto error;
    }
    disk = gpt_disk_alloc();
    if (!disk) {
        cout << "Failed to alloc disk struct" << endl;
        goto error;
    }
    if (gpt_disk_get_disk_info(partname, disk)) {
        cout << "Failed to get disk info" << endl;
        goto error;
    }
    pentry = gpt_disk_get_pentry(disk, partname, PRIMARY_GPT);
    if (!pentry) {
        cout << "pentry does not exist in disk struct" << endl;
        goto error;
    }
    attr = pentry + AB_FLAG_OFFSET;
    if (part_attr == ATTR_SLOT_ACTIVE) {
        retval = !!(*attr & AB_PARTITION_ATTR_SLOT_ACTIVE);
        cout << "part_attr == ATTR_SLOT_ACTIVE for " << partname << " retval is: " << retval << endl;
    } else if (part_attr == ATTR_BOOT_SUCCESSFUL) {
        retval = !!(*attr & AB_PARTITION_ATTR_BOOT_SUCCESSFUL);
        cout << "part_attr == ATTR_BOOT_SUCCESSFUL for " << partname << endl;
    } else if (part_attr == ATTR_UNBOOTABLE) {
        retval = !!(*attr & AB_PARTITION_ATTR_UNBOOTABLE);
        cout << "part_attr == ATTR_UNBOOTABLE for " << partname << endl;
    } else {
        cout << "FACK! " << partname << endl;
        retval = -1;
    }
    gpt_disk_free(disk);
    return retval;
error:
    if (disk)
        gpt_disk_free(disk);
    return retval;
}

#define SLOT_ACTIVE 1
#define SLOT_INACTIVE 2

#define UPDATE_SLOT(pentry, guid, slot_state) ({ \
        memcpy(pentry, guid, TYPE_GUID_SIZE); \
        if (slot_state == SLOT_ACTIVE)\
            *(pentry + AB_FLAG_OFFSET) = AB_SLOT_ACTIVE_VAL; \
        else if (slot_state == SLOT_INACTIVE) \
        *(pentry + AB_FLAG_OFFSET)  = (*(pentry + AB_FLAG_OFFSET)& \
            ~AB_PARTITION_ATTR_SLOT_ACTIVE); \
        })

const char *slot_suffix_arr[] = {
    AB_SLOT_A_SUFFIX,
    AB_SLOT_B_SUFFIX,
    NULL};

int doIt(char *this_part) {
    gpt_disk* diskA = NULL;
    gpt_disk* diskB = NULL;

    unsigned slot = NULL;

    uint8_t *pentryA = NULL;
    uint8_t *pentryA_bak = NULL;
    uint8_t *pentryB = NULL;
    uint8_t *pentryB_bak = NULL;

    char slotA[MAX_GPT_NAME_SIZE + 1] = {0};
    char slotB[MAX_GPT_NAME_SIZE + 1] = {0};

    char active_guid[TYPE_GUID_SIZE + 1] = {0};
    char inactive_guid[TYPE_GUID_SIZE + 1] = {0};

    cout << "==== " << this_part << " ====" << endl;

    memset(slotA, 0, sizeof(slotA));
    memset(slotB, 0, sizeof(slotB));
    snprintf(slotA, sizeof(slotA) - 1, "%s%s", this_part, AB_SLOT_A_SUFFIX);
    snprintf(slotB, sizeof(slotB) - 1, "%s%s", this_part, AB_SLOT_B_SUFFIX);

    cout << "getting disk info for slotA" << endl;
    diskA = boot_ctl_get_disk_info(slotA);
    cout << "getting disk info for slotB" << endl;
    diskB = boot_ctl_get_disk_info(slotB);

    pentryA = gpt_disk_get_pentry(diskA, slotA, PRIMARY_GPT);
    pentryA_bak = gpt_disk_get_pentry(diskA, slotA, SECONDARY_GPT);
    pentryB = gpt_disk_get_pentry(diskB, slotB, PRIMARY_GPT);
    pentryB_bak = gpt_disk_get_pentry(diskB, slotB, SECONDARY_GPT);

    if (pentryA != NULL)
        cout << "pentryA is good!" << endl;
    else
        cout << "pentryA is bad!" << endl;
    if (pentryA_bak != NULL)
        cout << "pentryA_bak is good!" << endl;
    else
        cout << "pentryA_bak is bad!" << endl;
    if (pentryB != NULL)
        cout << "pentryB is good!" << endl;
    else
        cout << "pentryB is bad!" << endl;
    if (pentryB_bak != NULL)
        cout << "pentryB_bak is good!" << endl;
    else
        cout << "pentryB_bak is bad!" << endl;

    memset(active_guid, '\0', sizeof(active_guid));
    memset(inactive_guid, '\0', sizeof(inactive_guid));

    int attribA = get_partition_attribute(slotA, ATTR_SLOT_ACTIVE);
    int attribB = get_partition_attribute(slotB, ATTR_SLOT_ACTIVE);

    if (attribA == 1) {
        slot = 1;
        //A is the current active slot
        memcpy((void*)active_guid, (const void*)pentryA, TYPE_GUID_SIZE);
        memcpy((void*)inactive_guid,(const void*)pentryB, TYPE_GUID_SIZE);
    } else if (attribB == 1) {
        slot = 0;
        //B is the current active slot
        memcpy((void*)active_guid, (const void*)pentryB, TYPE_GUID_SIZE);
        memcpy((void*)inactive_guid, (const void*)pentryA, TYPE_GUID_SIZE);
    } else {
        cout << "Both A & B are inactive..Aborting" << endl;
        goto error;
    }

    cout << "active_guid: " << &active_guid << endl;
    cout << "inactive_guid: " << &inactive_guid << endl;

    if (!strncmp(slot_suffix_arr[slot], AB_SLOT_A_SUFFIX, strlen(AB_SLOT_A_SUFFIX))){
        //Mark A as active in primary table
        UPDATE_SLOT(pentryA, active_guid, SLOT_ACTIVE);
        //Mark A as active in backup table
        UPDATE_SLOT(pentryA_bak, active_guid, SLOT_ACTIVE);
        //Mark B as inactive in primary table
        UPDATE_SLOT(pentryB, inactive_guid, SLOT_INACTIVE);
        //Mark B as inactive in backup table
        UPDATE_SLOT(pentryB_bak, inactive_guid, SLOT_INACTIVE);
    } else if (!strncmp(slot_suffix_arr[slot], AB_SLOT_B_SUFFIX, strlen(AB_SLOT_B_SUFFIX))){
        //Mark B as active in primary table
        UPDATE_SLOT(pentryB, active_guid, SLOT_ACTIVE);
        //Mark B as active in backup table
        UPDATE_SLOT(pentryB_bak, active_guid, SLOT_ACTIVE);
        //Mark A as inavtive in primary table
        UPDATE_SLOT(pentryA, inactive_guid, SLOT_INACTIVE);
        //Mark A as inactive in backup table
        UPDATE_SLOT(pentryA_bak, inactive_guid, SLOT_INACTIVE);
    } else {
        //Something has gone terribly terribly wrong
        cout << "Unknown slot suffix!" << endl;
        goto error;
    }

    if (gpt_disk_update_crc(diskA)) cout << "gpt_disk_update_crc failed for diskA" << endl;
    if (gpt_disk_update_crc(diskB)) cout << "gpt_disk_update_crc failed for diskB" << endl;
    if (gpt_disk_commit(diskA)) cout << "gpt_disk_commit failed for diskA" << endl;
    if (gpt_disk_commit(diskB)) cout << "gpt_disk_commit failed for diskB" << endl;

    cout << endl << endl;

    return slot;

    error:
        return -1;
}

int main() {
    int slot = -1;
    doIt("rpm");
    doIt("tz");
    doIt("hyp");
    doIt("storsec");
    doIt("devcfg");
    doIt("keymaster");
    doIt("cmnlib");
    doIt("cmnlib64");
    doIt("prov");
    doIt("pmic");
    doIt("abl");
    doIt("fsg");
    doIt("modem");
    doIt("dsp");
    doIt("bluetooth");
    doIt("logo");
    doIt("boot");
    doIt("system");
    doIt("oem");
    slot = doIt("dto");

    if (slot == 0) {
        gpt_utils_set_xbl_boot_partition(NORMAL_BOOT);
        cout << "Set xbl slot to NORMAL_BOOT!" << endl;
    } else if (slot == 1) {
        gpt_utils_set_xbl_boot_partition(BACKUP_BOOT);
        cout << "Set xbl slot to BACKUP_BOOT!" << endl;
    } else {
        cout << "FAILED TO SET xbl SLOT!" << endl;
    }
}
