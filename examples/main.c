#include "../fs.h"
#include <stdio.h>

void __print(const struct iso9660 *fs) {
  printf("------------volumes--------------\n");

  for (struct volume_descriptor_list *ptr = fs->volumes; ptr != NULL;
       ptr = ptr->next) {
    printf("volume=%.*s type=%d\n", 5, ptr->data.id, ptr->data.type);

    switch (ptr->data.type) {
    case VOLUME_DESCRIPTOR_BOOT_RECORD: {
      struct volume_descriptor_boot_record *record =
          (struct volume_descriptor_boot_record *)&ptr->data;
      printf("---- id=%.*s system=%.*s\n", 32, record->boot_id, 32,
             record->boot_system_id);
      break;
    }
    case VOLUME_DESCRIPTOR_PRIMARY_RECORD: {
      struct volume_descriptor_primary *record =
          (struct volume_descriptor_primary *)&ptr->data;
      printf("---->> id=%c id_length=%d system=%.*s\n", record->root_entry.id,
             record->root_entry.id_length, 128, record->volume_set_id);
      printf("---->> id=%.*s system=%.*s\n", 32, record->system_id, 128,
             record->application_id);
      printf("---->> id=%.*s system=%.*s\n", 128, record->publisher_id, 128,
             record->preparer_id);
      printf("---->> %d %d %d\n", record->root_entry.extended_attribute_length,
             record->root_entry.file_flags, record->root_entry.id_length);
      printf("---->> %d %d\n", record->root_entry.extent_location[0],
             record->root_entry.extent_location[0]);
      printf("---->> %d %d\n", record->path_table_location_l,
             record->path_table_location_opt_l);
      printf("---->> %d %d\n", record->path_table_location_m,
             record->path_table_location_opt_m);
      printf("---->> %d %d\n", record->path_table_size[0],
             record->path_table_size[1]);
      printf("---->> %d %d\n", record->volume_set_size[0],
             record->volume_set_size[1]);
      printf("---->> %d %d\n", record->volume_space_size[0],
             record->volume_space_size[1]);
      break;
    }
    default:
      break;
    }
  }

  printf("------------path table--------------\n");

  for (struct path_table_list *p = fs->dirs; p != NULL; p = p->next) {
    printf(
        "extent_loc=%d attribute_length=%d parent=%d id_length=%d id='%.*s'\n",
        p->data.extent_location, p->data.extended_attribute_length, p->data.num,
        p->data.length, p->data.length, p->data.id);
  }

  printf("------------records--------------\n");

  for (struct record_list *p = fs->records; p != NULL; p = p->next) {
    printf("id_length=%d id='%.*s', extent_loc=%d extent_size=%d flags=%d\n",
           p->data.length, p->data.length, p->data.id,
           p->data.extent_location[0], p->data.extent_location[0],
           p->data.file_flags);
    printf("id='%d %d %d'\n", p->data.id[0], p->data.id[1], p->data.id[2]);

    if ((p->data.file_flags & RECORD_DIR) == 0) {
       printf("-------------\n");

       for (struct file_list *file = fs->files; file != NULL; file = file->next) {
           if (file->pos[0] == p->data.extent_location[0] && file->pos[1] == p->data.extent_location[1]) {
              printf("content=%.*s\n", (int)file->size, file->data);
              for (int i = 0; i < (int)file->size; i++) {
                  printf("%c", *(file->data + i));
              }

              printf("\n");
           }
       }
    }
  }
}

int main() {
  struct iso9660 fs;
  int ok = iso9660_open(
      &fs, "/home/maxim/projects/kexploration/bkernel/build/startup.iso");
  if (ok < 0) {
    printf("Error build a iso9660\n");
    return -2;
  }

  __print(&fs);

  iso9660_deinit(&fs);

  return 0;
}