
#include "fs.h"
#include <stdint.h>

struct __attribute__((__packed__)) eltorito_volume {
  uint8_t type;
  char_a id[5];
  uint8_t version;
  char_a boot_system_id[32];
  char_a boot_id[32];
  uint32_t sector;
  int8_t unused[1974];
};

struct __attribute__((__packed__)) eltorito_catalog_entry_validation {
  uint8_t header_id;
  uint8_t platform_id;
  int16_t unused0;
  char id[24];
  uint16_t checksum;
  uint8_t key1;
  uint8_t key2;
};

struct __attribute__((__packed__)) eltorito_catalog_entry_initial {
  uint8_t indicator;
  uint8_t media_type;
  uint16_t load_segment;
  uint8_t system_type;
  int8_t unused0;
  uint16_t sector_count;
  uint32_t load_rba;
  int8_t unused1[20];
};

struct __attribute__((__packed__)) eltorito_catalog_entry_header {
  uint8_t indicator;
  uint8_t platform;
  uint16_t count_entries;
  char id[28];
};

struct __attribute__((__packed__)) eltorito_catalog_entry_section {
  uint8_t indicator;
  uint8_t media_type;
  uint16_t load_segment;
  uint8_t segment_type;
  uint8_t unused0;
  uint16_t sector_count;
  uint32_t load_rba;
  uint8_t selection_criteria_type;
  int8_t id[19];
};

struct __attribute__((__packed__)) eltorito_catalog_entry_section_extension {
  uint8_t indicator;
  uint8_t type;
  int8_t id[30];
};

enum eltorito_catalog_entry_kind {
  ELTORITO_CATALOG_ENTRY_KIND_VALIDATION = 0,
  ELTORITO_CATALOG_ENTRY_KIND_INITIAL = 1,
  ELTORITO_CATALOG_ENTRY_KIND_HEADER = 2,
  ELTORITO_CATALOG_ENTRY_KIND_SECTION = 3,
  ELTORITO_CATALOG_ENTRY_KIND_SECTION_EXTENSION = 4,
};

struct __attribute__((__packed__)) eltorito_catalog {
  enum eltorito_catalog_entry_kind kind;
  void *data;
  struct eltorito_catalog *next;
};

struct __attribute__((__packed__)) eltorito {
  struct eltorito_catalog *catalog;
  struct file_list *files;
};

int eltorito_parse(void *buf, size_t size, uint32_LSB sector,
                   struct eltorito *fs);