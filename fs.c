#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fs.h"

static int64_t read_file(const char *path, void **buf) {
  FILE *f;
  int64_t fsize;
  uint64_t rsize;
  int32_t err;

  f = fopen(path, "rb");
  if (f == NULL) {
    return (-1);
  }

  err = fseek(f, 0, SEEK_END);
  if (err != 0) {
    return (-1);
  }

  fsize = ftell(f);

  err = fseek(f, 0, SEEK_SET);
  if (err != 0) {
    return (-1);
  }

  *buf = malloc(fsize);
  if (*buf == NULL) {
    return (-1);
  }

  rsize = fread(*buf, sizeof(char), fsize, f);
  if (rsize != fsize) {
    return (-1);
  }

  err = fclose(f);
  if (err != 0) {
    return (-1);
  }

  return (fsize);
}

static int parse_descriptors(const void *buf, size_t size,
                             struct volume_descriptor_list **volumes) {
  size_t pos = 0;
  struct volume_descriptor_list *volumes_ptr;
  struct volume_descriptor *descriptor;

  if (size < SYSTEM_AREA_SIZE) {
    return (-1);
  }

  pos += SYSTEM_AREA_SIZE;

  while (1) {
    if (pos + VOLUME_DESCRIPTOR_SIZE > size) {
      return (-1);
    }

    if (*volumes == NULL) {
      *volumes = malloc(VOLUME_DESCRIPTOR_SIZE);
      volumes_ptr = *volumes;
    } else {
      volumes_ptr->next = malloc(VOLUME_DESCRIPTOR_SIZE);
      volumes_ptr = volumes_ptr->next;
    }

    descriptor = (struct volume_descriptor *)(buf + pos);
    pos += VOLUME_DESCRIPTOR_SIZE;

    memcpy((void *)&volumes_ptr->data, descriptor,
           sizeof(struct volume_descriptor));

    if (descriptor->type == VOLUME_DESCRIPTOR_SET_TERMINATOR) {
      break;
    }
  }

  return (0);
}

static int parse_path_table(const void *buf, size_t size, size_t start,
                            size_t count_bytes, struct path_table_list **dirs) {
  struct path_table *table;
  struct path_table_head *table_head;
  struct path_table_list *ptr;
  size_t pos, i = 0;
  char *ident;
  size_t ident_size;

  while (1) {
    if (*dirs == NULL) {
      ptr = malloc(sizeof(struct path_table_list));
      *dirs = ptr;
    } else {
      ptr->next = malloc(sizeof(struct path_table_list));
      ptr = ptr->next;
    }

    pos = start + i;

    table_head = (struct path_table_head *)(buf + pos);

    ident = (char *)(buf + pos + sizeof(struct path_table_head));
    ident_size = table_head->length;

    memcpy((void *)&ptr->data, table_head, sizeof(struct path_table_head));
    memcpy((void *)ptr->data.id, ident, ident_size);

    i += sizeof(struct path_table_head) + ident_size +
         ((ident_size % 2 == 0) ? 0 : 1);

    if (i == count_bytes) {
      break;
    } else if (i > count_bytes) {
      // Not compatible structure;
      // todo: free resource?;
      return (-1);
    }
  }

  return (0);
}

static int parse_records(const void *buf, size_t size, size_t start,
                         size_t count, size_t max,
                         struct record_list **records) {
  struct record *rec;
  struct record_head *head;
  struct record_list *list;
  size_t pos, i, id_size;

  pos = start;

  while (1) {
    head = (struct record_head *)(buf + pos);
    pos += head->length;
    if (head->length == 0) {
      break;
    }

    if (*records == NULL) {
      *records = malloc(sizeof(struct record_list));
      list = *records;
    } else {
      list->next = malloc(sizeof(struct record_list));
      list = list->next;
    }

    rec = (struct record *)(head);

    memcpy((void *)&list->data, head, sizeof(struct record_head));
    memcpy((void *)list->data.id, rec->id, head->id_length);
  }

  // todo: recursion
  //
  // for (list = *records; list != NULL; list = list->next) {

  // }

  return (0);
}

static int parse_volume(const void *buf, size_t size,
                        const struct volume_descriptor *volume,
                        struct path_table_list **dirs,
                        struct record_list **records) {
  struct volume_descriptor_primary *pvd;
  size_t volume_size;
  int err;

  switch (volume->type) {
  case VOLUME_DESCRIPTOR_PRIMARY_RECORD: {
    pvd = (struct volume_descriptor_primary *)volume;
    volume_size = pvd->volume_space_size[0] * SECTOR_SIZE -
                  sizeof(struct volume_descriptor_primary) -
                  pvd->path_table_size[0] * SECTOR_SIZE;
    err = parse_path_table(buf, size, pvd->path_table_location_l * SECTOR_SIZE,
                           pvd->path_table_size[0], dirs);
    if (err != 0) {
      return (-1);
    }

    err = parse_records(buf, size,
                        pvd->root_entry.extent_location[0] * SECTOR_SIZE,
                        pvd->root_entry.extent_size[0], volume_size, records);
    if (err != 0) {
      return (-1);
    }

    break;
  }
  default:
    break;
  }

  return (0);
}

ssize_t iso9660_open(struct iso9660 *fs, const char *path) {
  void *buf;
  int err;
  int64_t size;
  uint64_t pos;
  struct volume_descriptor_list *volumes = NULL;
  struct volume_descriptor_list *volume = NULL;
  struct path_table_list *dirs = NULL;
  struct record_list *records = NULL;

  size = read_file(path, &buf);
  if (size < 0) {
    return (-1);
  }

  err = parse_descriptors(buf, size, &volumes);
  if (err != 0) {
    return (-1);
  }

  for (volume = volumes; volume != NULL; volume = volume->next) {
    err = parse_volume(buf, size, &volume->data, &dirs, &records);
    if (err != 0) {
      return (-1);
    }
  }

  fs->volumes = volumes;
  fs->dirs = dirs;
  fs->records = records;

  return (0);
}

ssize_t iso9660_deinit(struct iso9660 *fs) {
  struct volume_descriptor_list *volume = fs->volumes;
  struct path_table_list *dir = fs->dirs;

  while (volume != NULL) {
    struct volume_descriptor_list *tmp = volume->next;
    free(volume);
    volume = tmp;
  }

  while (dir != NULL) {
    struct path_table_list *tmp = dir->next;
    free(dir);
    dir = tmp;
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
