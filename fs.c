#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "fs.h"

ssize_t iso9660_open(struct iso9660 *fs, FILE *fp) {
  size_t size = fread(fs->system_area, sizeof(int8_t), SYSTEM_AREA_SIZE, fp);
  if (size != SYSTEM_AREA_SIZE) {
    if (feof(fp) || ferror(fp)) {
      return -1;
    }
  }

  struct volume_descriptor_list *volumes = NULL;
  struct volume_descriptor_list *volumes_ptr = NULL;
  struct volume_descriptor descriptor;

  while (1) {
    size_t size =
        fread((void *)&descriptor, sizeof(int8_t), VOLUME_DESCRIPTOR_SIZE, fp);
    if (size != VOLUME_DESCRIPTOR_SIZE) {
      if (feof(fp) || ferror(fp)) {
        return -1;
      }
    }

    struct volume_descriptor_list *new_volume = malloc(sizeof(struct volume_descriptor_list));
    new_volume->data = descriptor;
    new_volume->next = NULL;

    if (volumes == NULL) {
      volumes = new_volume;
      volumes_ptr = new_volume;
    } else {
      volumes_ptr->next = new_volume;
      volumes_ptr = new_volume;
    }

    if (descriptor.type == VOLUME_DESCRIPTOR_SET_TERMINATOR) {
      break;
    }
  }

  fs->volumes = volumes;

  return 0;
}

ssize_t iso9660_deinit(struct iso9660 *fs) {
  struct volume_descriptor_list* p = fs->volumes;
  while (p != NULL) {
    struct volume_descriptor_list* tmp = p->next;
    free(p);
    p = tmp;
  }

  return 0;
}

ssize_t iso9660_read(struct iso9660 *fs, const void *buf, size_t size) {
  return 0;
}

ssize_t iso9660_write(struct iso9660 *fs, const char *path, const void *buf,
                      size_t size, size_t count) {
  return 0;
}

ssize_t iso9660_insert(struct iso9660 *fs, const char *path, FILE *file) {
  return 0;
}

ssize_t iso9660_dump(struct iso9660 *fs, FILE *file) { return 0; }
