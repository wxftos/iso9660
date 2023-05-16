#include "../fs.h"
#include <stdio.h>

void __print(const struct iso9660 *fs) {
  for (struct volume_descriptor_list *ptr = fs->volumes; ptr != NULL;
       ptr = ptr->next) {
    printf("volume=%.*s type=%d\n", 5, ptr->data.id, ptr->data.type);

    switch (ptr->data.type) {
    case VOLUME_DESCRIPTOR_BOOT_RECORD: {
      struct volume_descriptor_boot_record *record = (struct volume_descriptor_boot_record*)&ptr->data;
      printf("---- id=%.*s system=%.*s %ul\n", 32, record->boot_id, 32, record->boot_system_id, sizeof(struct volume_descriptor_boot_record));
      break;
    }
    case VOLUME_DESCRIPTOR_PRIMARY_RECORD:
      break;
    default:
      break;
    }
  }
}

int main() {
  printf("Hello World\n");

  FILE *file = fopen(
      "/home/maxim/projects/kexploration/bkernel/build/startup.iso", "rb");
  if (file == NULL) {
    printf("Error! Could not open file\n");
    return -1;
  }

  struct iso9660 fs;
  int ok = iso9660_open(&fs, file);
  if (ok < 0) {
    printf("Error build a iso9660\n");
    return -2;
  }

  __print(&fs);

  iso9660_deinit(&fs);

  return 0;
}