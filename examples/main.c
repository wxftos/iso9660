#include "../eltorito.h"
#include "../fs.h"
#include <stdio.h>
#include <string.h>

void __print(const struct iso9660 *fs) {
  printf("------------volumes--------------\n");

  for (struct volume_descriptor_list *ptr = fs->volumes; ptr != NULL;
       ptr = ptr->next) {
    switch (ptr->data.type) {
    case VOLUME_DESCRIPTOR_BOOT_RECORD: {
      struct volume_descriptor_boot *record =
          (struct volume_descriptor_boot *)&ptr->data;
      printf("BOOT\n");
      printf("id='%.*s'\n", (int)sizeof(record->id), record->id);
      printf("type=%d\n", record->type);
      printf("system='%.*s'\n", (int)sizeof(record->boot_system_id),
             record->boot_system_id);
      printf("boot='%.*s'\n", (int)sizeof(record->boot_id), record->boot_id);

      if (strcmp(record->boot_system_id, "EL TORITO SPECIFICATION") == 0) {
        struct eltorito_volume *eltorito = (struct eltorito_volume *)record;
        printf("boot_sector=%d\n", eltorito->sector);

        // find basic structure of eltorito.

        char *buf = fs->files->data;
        struct eltorito_catalog_entry_validation *validation_entry =
            (struct eltorito_catalog_entry_validation *)buf;

        printf("platform_id=%d\n", validation_entry->platform_id);
        printf("header_id=%d\n", validation_entry->header_id);
        printf("id='%.*s'\n", (int)sizeof(validation_entry->id),
               validation_entry->id);

        struct eltorito_catalog_entry_initial *default_entry =
            (struct eltorito_catalog_entry_initial
                 *)(buf + sizeof(struct eltorito_catalog_entry_validation));

        printf("media_type=%d\n", default_entry->media_type);
        printf("system_type=%d\n", default_entry->system_type);
        printf("indicator=%d\n", default_entry->indicator);
        printf("sector_count=%d\n", default_entry->sector_count);
        printf("load_rba=%d\n", default_entry->load_rba);
        printf("load_segment=%d\n", default_entry->load_segment);

        struct eltorito_catalog_entry_section *section_entry =
            (struct eltorito_catalog_entry_section
                 *)(buf + sizeof(struct eltorito_catalog_entry_validation) * 2);

        printf("media_type=%d\n", section_entry->media_type);
        printf("segment_type=%d\n", section_entry->segment_type);
        printf("indicator=%d\n", section_entry->indicator);
        printf("selection_criteria_type=%d\n",
               section_entry->selection_criteria_type);
        printf("indicator=%d\n", section_entry->indicator);
        printf("sector_count=%d\n", section_entry->sector_count);
        printf("load_rba=%d\n", section_entry->load_rba);
        printf("load_segment=%d\n", section_entry->load_segment);

        printf("BOOTABLE_FILE\n");

        int i = 0;
        for (struct file_list *file = fs->sectors; file != NULL;
             file = file->next) {
          if (i == default_entry->load_rba) {
            printf("d=%d content=%.*s\n", (int)file->size, (int)file->size,
                   file->data);
            for (int i = 0; i < (int)file->size; i++) {
              printf("%x ", *(file->data + i));
            }

            printf("\n");
            break;
          }

          i++;
        }
      }

      break;
    }
    case VOLUME_DESCRIPTOR_PRIMARY_RECORD: {
      struct volume_descriptor_primary *record =
          (struct volume_descriptor_primary *)&ptr->data;
      printf("PVD\n");
      printf("id='%.*s'\n", (int)sizeof(record->id), record->id);
      printf("type=%d\n", record->type);
      printf("system='%.*s'\n", (int)sizeof(record->system_id),
             record->system_id);
      printf("volume='%.*s'\n", (int)sizeof(record->volume_id),
             record->volume_id);
      printf("volume_set='%.*s'\n", (int)sizeof(record->volume_set_id),
             record->volume_set_id);
      printf("publisher='%.*s'\n", (int)sizeof(record->publisher_id),
             record->publisher_id);
      printf("application='%.*s'\n", (int)sizeof(record->application_id),
             record->application_id);
      printf("preparer='%.*s'\n", (int)sizeof(record->preparer_id),
             record->preparer_id);
      printf("copyright_file='%.*s'\n", (int)sizeof(record->copyright_file_id),
             record->copyright_file_id);
      printf("abstract_file='%.*s'\n", (int)sizeof(record->abstract_file_id),
             record->abstract_file_id);
      printf("bibliographic_file='%.*s'\n",
             (int)sizeof(record->bibliographic_file_id),
             record->bibliographic_file_id);
      printf("structure_version=%d\n", record->file_structure_version);
      printf("volume_space_size=[%d, %d]'\n", record->volume_space_size[0],
             record->volume_space_size[1]);
      printf("volume_set_size=[%d, %d]'\n", record->volume_set_size[0],
             record->volume_set_size[1]);
      printf("volume_sequence_number=[%d, %d]'\n",
             record->volume_sequence_num[0], record->volume_sequence_num[1]);
      printf("block_size=[%d, %d]'\n", record->logical_block_size[0],
             record->logical_block_size[1]);
      printf("path_table_size=[%d, %d]'\n", record->path_table_size[0],
             record->path_table_size[1]);
      printf("path_table_location=[%d, %d]'\n", record->path_table_location_l,
             record->path_table_location_m);
      printf("path_table_optional_location=[%d, %d]'\n",
             record->path_table_location_opt_l,
             record->path_table_location_opt_m);
      printf("created_at=(%.*s.%.*s.%.*s %.*s:%.*s:%.*s:%.*s TZ=%d)'\n",
             (int)sizeof(record->volume_created_at.year),
             record->volume_created_at.year,
             (int)sizeof(record->volume_created_at.month),
             record->volume_created_at.month,
             (int)sizeof(record->volume_created_at.day),
             record->volume_created_at.day,
             (int)sizeof(record->volume_created_at.hour),
             record->volume_created_at.hour,
             (int)sizeof(record->volume_created_at.minute),
             record->volume_created_at.minute,
             (int)sizeof(record->volume_created_at.second),
             record->volume_created_at.second,
             (int)sizeof(record->volume_created_at.second_hundredths),
             record->volume_created_at.second_hundredths,
             record->volume_created_at.time_zone);
      printf("modified_at=(%.*s.%.*s.%.*s %.*s:%.*s:%.*s:%.*s TZ=%d)'\n",
             (int)sizeof(record->volume_modified_at.year),
             record->volume_modified_at.year,
             (int)sizeof(record->volume_modified_at.month),
             record->volume_modified_at.month,
             (int)sizeof(record->volume_modified_at.day),
             record->volume_modified_at.day,
             (int)sizeof(record->volume_modified_at.hour),
             record->volume_modified_at.hour,
             (int)sizeof(record->volume_modified_at.minute),
             record->volume_modified_at.minute,
             (int)sizeof(record->volume_modified_at.second),
             record->volume_modified_at.second,
             (int)sizeof(record->volume_modified_at.second_hundredths),
             record->volume_modified_at.second_hundredths,
             record->volume_modified_at.time_zone);
      printf("expiration_at=(%.*s.%.*s.%.*s %.*s:%.*s:%.*s:%.*s TZ=%d)'\n",
             (int)sizeof(record->volume_expiration_at.year),
             record->volume_expiration_at.year,
             (int)sizeof(record->volume_expiration_at.month),
             record->volume_expiration_at.month,
             (int)sizeof(record->volume_expiration_at.day),
             record->volume_expiration_at.day,
             (int)sizeof(record->volume_expiration_at.hour),
             record->volume_expiration_at.hour,
             (int)sizeof(record->volume_expiration_at.minute),
             record->volume_expiration_at.minute,
             (int)sizeof(record->volume_expiration_at.second),
             record->volume_expiration_at.second,
             (int)sizeof(record->volume_expiration_at.second_hundredths),
             record->volume_expiration_at.second_hundredths,
             record->volume_expiration_at.time_zone);
      printf("effective_at=(%.*s.%.*s.%.*s %.*s:%.*s:%.*s:%.*s TZ=%d)'\n",
             (int)sizeof(record->volume_effective_at.year),
             record->volume_effective_at.year,
             (int)sizeof(record->volume_effective_at.month),
             record->volume_effective_at.month,
             (int)sizeof(record->volume_effective_at.day),
             record->volume_effective_at.day,
             (int)sizeof(record->volume_effective_at.hour),
             record->volume_effective_at.hour,
             (int)sizeof(record->volume_effective_at.minute),
             record->volume_effective_at.minute,
             (int)sizeof(record->volume_effective_at.second),
             record->volume_effective_at.second,
             (int)sizeof(record->volume_effective_at.second_hundredths),
             record->volume_effective_at.second_hundredths,
             record->volume_effective_at.time_zone);
      break;
    }
    default:
      break;
    }
  }

  printf("------------path table--------------\n");

  for (struct path_table_list *p = fs->dirs; p != NULL; p = p->next) {
    printf("extent_loc=%d attribute_length=%d parent=%d id_length=%d "
           "id='%.*s'\n",
           p->data.extent_location, p->data.extended_attribute_length,
           p->data.num, p->data.length, p->data.length, p->data.id);
  }

  printf("------------records--------------\n");

  for (struct record_list *p = fs->records; p != NULL; p = p->next) {
    printf("id_length=%d id='%.*s', extent_loc=%d extent_size=%d flags=%d\n",
           p->data.length, p->data.length, p->data.id,
           p->data.extent_location[0], p->data.extent_location[0],
           p->data.file_flags);
  }

  printf("------------files--------------\n");

  for (struct file_list *p = fs->files; p != NULL; p = p->next) {
    printf("file_pos=[%d %d] size=%ld\n", p->pos[0], p->pos[1], p->size);
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