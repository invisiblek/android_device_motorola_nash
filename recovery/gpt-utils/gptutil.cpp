#include <iostream>
#include "gpt-utils.h"

using namespace std;

static struct gpt_disk* boot_ctl_get_disk_info(char *partition) {
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

int main() {
    gpt_disk* diskA = NULL;
    gpt_disk* diskB = NULL;

    uint8_t *pentryA = NULL;
    uint8_t *pentryA_bak = NULL;
    uint8_t *pentryB = NULL;
    uint8_t *pentryB_bak = NULL;

    char slotA[MAX_GPT_NAME_SIZE + 1] = {0};
    char slotB[MAX_GPT_NAME_SIZE + 1] = {0};

    memset(slotA, 0, sizeof(slotA));
    memset(slotB, 0, sizeof(slotB));
    snprintf(slotA, sizeof(slotA) - 1, "%s%s", "rpm", AB_SLOT_A_SUFFIX);
    snprintf(slotB, sizeof(slotB) - 1, "%s%s", "rpm", AB_SLOT_B_SUFFIX);

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

    return 0;
}
