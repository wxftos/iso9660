
#include "eltorito.h"
#include "fs.h"
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

int eltorito_parse(void *buf, size_t size, uint32_LSB sector,
                   struct eltorito *fs) {
  struct eltorito_catalog_entry_section *entry_section;
  struct eltorito_catalog_entry_validation *entry_validation;
  struct eltorito_catalog_entry_initial *entry_default;
  struct eltorito_catalog *catalog;
  struct file_list *files;
  size_t pos;
  char *buf_ptr;
  int is_empty;

  fs->catalog = malloc(sizeof(struct eltorito_catalog));
  fs->files = malloc(sizeof(struct file_list));

  catalog = fs->catalog;

  entry_validation = malloc(sizeof(struct eltorito_catalog_entry_validation));
  entry_default = malloc(sizeof(struct eltorito_catalog_entry_initial));

  memcpy(entry_validation, buf + sector * SECTOR_SIZE,
         sizeof(struct eltorito_catalog_entry_validation));
  memcpy(entry_default,
         buf + sector * SECTOR_SIZE +
             sizeof(struct eltorito_catalog_entry_validation),
         sizeof(struct eltorito_catalog_entry_initial));

  catalog->data = entry_validation;
  catalog->kind = ELTORITO_CATALOG_ENTRY_KIND_VALIDATION;
  catalog->next = malloc(sizeof(struct eltorito_catalog));

  catalog = catalog->next;
  catalog->data = entry_default;
  catalog->kind = ELTORITO_CATALOG_ENTRY_KIND_INITIAL;
  catalog->next = NULL;

  files = fs->files;
  files->data = malloc(SECTOR_SIZE);
  files->size = SECTOR_SIZE;
  files->next = NULL;

  memcpy(files->data, buf + entry_default->load_rba * SECTOR_SIZE,
         sizeof(struct eltorito_catalog_entry_initial));

  pos = sector * SECTOR_SIZE +
        sizeof(struct eltorito_catalog_entry_validation) +
        sizeof(struct eltorito_catalog_entry_initial);
  while (1) {
    buf_ptr = buf + pos;

    is_empty = 0;
    for (int i = 0; i < sizeof(struct eltorito_catalog_entry_section); i++) {
      if (*buf_ptr != 0) {
        is_empty = 1;
        break;
      }
    }

    if (is_empty == 0) {
      break;
    }

    entry_section = malloc(sizeof(struct eltorito_catalog_entry_section));
    memcpy(entry_validation, buf_ptr,
           sizeof(struct eltorito_catalog_entry_section));

    catalog->next = malloc(sizeof(struct eltorito_catalog));
    catalog = catalog->next;
    catalog->data = entry_section;
    catalog->kind = ELTORITO_CATALOG_ENTRY_KIND_SECTION;

    files->next = malloc(sizeof(struct file_list));
    files = files->next;
    files->data = malloc(SECTOR_SIZE);
    files->size = SECTOR_SIZE;

    memcpy(files->data, buf + entry_section->load_rba * SECTOR_SIZE,
           sizeof(struct eltorito_catalog_entry_section));
  }

  return (0);
}