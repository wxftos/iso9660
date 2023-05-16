#ifndef __FS_H__
#define __FS_H__

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

// note: shouldn't it be 255 - 32?
#define DIR_RECORD_ID_MAX_SIZE 255
#define SYSTEM_AREA_SIZE 32768
#define VOLUME_DESCRIPTOR_SIZE 2048

typedef char char_a;
typedef char char_d;

typedef int16_t int16_LSB;
typedef int16_t int16_MSB;
typedef int16_t int16_LSB_MSB[2];

typedef uint16_t uint16_LSB;
typedef uint16_t uint16_MSB;
typedef uint16_t uint16_LSB_MSB[2];

typedef int32_t int32_LSB;
typedef int32_t int32_MSB;
typedef int32_t int32_LSB_MSB[2];

typedef uint32_t uint32_LSB;
typedef uint32_t uint32_MSB;
typedef uint32_t uint32_LSB_MSB[2];

struct datetime_dec {
  char_d year[4];
  char_d month[2];
  char_d day[2];
  char_d hour[2];
  char_d minute[2];
  char_d second[2];
  char_d second_hundredths[2];
  uint8_t time_zone;
};

struct datetime_num {
  uint8_t year;
  uint8_t month;
  uint8_t day;
  uint8_t hour;
  uint8_t minute;
  uint8_t second;
  int8_t offset;
};

enum dir_file_flags {
  DIR_RECORD_HIDDEN = (1u << 0),
  DIR_RECORD_DIR = (1u << 1),
  DIR_RECORD_ASSOCIATED = (1u << 2),
  DIR_RECORD_ATTRIBUTE_FORMAT = (1u << 3),
  DIR_RECORD_ATTRIBUTE_OWNER = (1u << 4),
  DIR_RECORD_NOT_FINAL = (1u << 7),
};

struct dir_record {
  uint8_t length;
  uint8_t extended_attribute_length;
  uint32_LSB_MSB extent_location;
  uint32_LSB_MSB extent_size;
  struct datetime_num datetime;
  uint8_t file_flags;
  uint8_t file_unit_size;
  uint8_t file_interleave_gap_size;
  uint16_LSB_MSB volume_sequence_num;
  uint8_t id_length;
  char_d id[DIR_RECORD_ID_MAX_SIZE];
  int8_t padding;
  int8_t data[DIR_RECORD_ID_MAX_SIZE];
};

struct volume_descriptor {
  uint8_t type;
  char_a id[5];
  uint8_t version;
  uint8_t data[2041];
};

enum volume_descriptor_type {
  VOLUME_DESCRIPTOR_BOOT_RECORD = 0,
  VOLUME_DESCRIPTOR_PRIMARY_RECORD = 1,
  VOLUME_DESCRIPTOR_SUPPLEMENTARY_RECORD = 2,
  VOLUME_DESCRIPTOR_PARTITION_RECORD = 3,
  VOLUME_DESCRIPTOR_SET_TERMINATOR = 255,
};

struct __attribute__((__packed__)) volume_descriptor_primary {
  uint8_t type;
  char_a id[5];
  uint8_t version;
  int8_t unused0;
  char_a system_id[32];
  char_d volume_id[32];
  int8_t unused1[8];
  uint32_LSB_MSB volume_space_size;
  int8_t unused2[32];
  uint16_LSB_MSB volume_set_size;
  uint16_LSB_MSB volume_sequence_num;
  uint16_LSB_MSB logical_block_size;
  uint32_LSB_MSB path_table_size;
  uint32_LSB path_table_type_l_location;
  uint32_LSB path_table_type_l_opt_location;
  uint32_MSB path_table_type_m_location;
  uint32_MSB path_table_type_m_opt_location;
  struct dir_record root_entry;
  char_d volume_set_id[128];
  char_a publisher_id[128];
  char_a preparer_id[128];
  char_a application_id[128];
  char_d copyright_file_id[37];
  char_d abstract_file_id[37];
  char_d bibliographic_file_id[37];
  struct datetime_dec volume_created_at;
  struct datetime_dec volume_modified_at;
  struct datetime_dec volume_expiration_at;
  struct datetime_dec volume_effective_at;
  uint8_t file_structure_version;
  int8_t unused4;
  int8_t data[512];
  int8_t unused5[653];
};

struct __attribute__((__packed__)) volume_descriptor_boot_record {
  uint8_t type;
  char_a id[5];
  uint8_t version;
  char_a boot_system_id[32];
  char_a boot_id[32];
  int8_t data[1977];
};

struct volume_descriptor_set_terminator {
  uint8_t type;
  char_a id[5];
  uint8_t version;
};

struct path_table {
  uint8_t length;
  uint8_t extended_attribute_length;
  uint32_t extent_location;
  uint16_t num;
  char_d id[DIR_RECORD_ID_MAX_SIZE];
  int8_t padding;
};

struct volume_descriptor_list {
  struct volume_descriptor data;
  struct volume_descriptor_list *next;
};

struct record_list {
  struct dir_record data;
  struct record_list *next;
};

struct path_list {
  struct path_table data;
  struct path_list *next;
};

struct iso9660 {
  int8_t system_area[SYSTEM_AREA_SIZE];
  struct volume_descriptor_list *volumes;
  struct path_list *paths;
  struct record_list *dirs;
};

ssize_t iso9660_open(struct iso9660 *fs, FILE *file);
ssize_t iso9660_deinit(struct iso9660 *fs);
ssize_t iso9660_read(struct iso9660 *fs, const void *buf, size_t size);
ssize_t iso9660_write(struct iso9660 *fs, const char *path, const void *buf,
                      size_t size, size_t count);
ssize_t iso9660_insert(struct iso9660 *fs, const char *path, FILE *file);
ssize_t iso9660_dump(struct iso9660 *fs, FILE *file);

#endif
