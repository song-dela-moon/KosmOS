/**
 * @file fat.hpp
 *
 * This file contains programs for operating the FAT filesystem.
 */

#pragma once

#include <cstdint>

namespace fat {

struct BPB {
  uint8_t jump_boot[3];
  char oem_name[8];
  uint16_t bytes_per_sector;
  uint8_t sectors_per_cluster;
  uint16_t reserved_sector_count;
  uint8_t num_fats;
  uint16_t root_entry_count;
  uint16_t total_sectors_16;
  uint8_t media;
  uint16_t fat_size_16;
  uint16_t sectors_per_track;
  uint16_t num_heads;
  uint32_t hidden_sectors;
  uint32_t total_sectors_32;
  uint32_t fat_size_32;
  uint16_t ext_flags;
  uint16_t fs_version;
  uint32_t root_cluster;
  uint16_t fs_info;
  uint16_t backup_boot_sector;
  uint8_t reserved[12];
  uint8_t drive_number;
  uint8_t reserved1;
  uint8_t boot_signature;
  uint32_t volume_id;
  char volume_label[11];
  char fs_type[8];
} __attribute__((packed));

enum class Attribute : uint8_t {
  kReadOnly  = 0x01,
  kHidden    = 0x02,
  kSystem    = 0x04,
  kVolumeID  = 0x08,
  kDirectory = 0x10,
  kArchive   = 0x20,
  kLongName  = 0x0f,
};

struct DirectoryEntry {
  unsigned char name[11];
  Attribute attr;
  uint8_t ntres;
  uint8_t create_time_tenth;
  uint16_t create_time;
  uint16_t create_date;
  uint16_t last_access_date;
  uint16_t first_cluster_high;
  uint16_t write_time;
  uint16_t write_date;
  uint16_t first_cluster_low;
  uint32_t file_size;

  uint32_t FirstCluster() const {
    return first_cluster_low |
      (static_cast<uint32_t>(first_cluster_high) << 16);
  }
} __attribute__((packed));

extern BPB* boot_volume_image;
extern unsigned long bytes_per_cluster;
void Initialize(void* volume_image);

/** @brief Returns the memory address where the first sector of the specified cluster is located.
 *
 * @param cluster Cluster number (starting from 2)
 * @return The address of the memory area where the first sector of the cluster is located.
 */
uintptr_t GetClusterAddr(unsigned long cluster);

/** @brief Returns the memory area where the first sector of the specified cluster is located.
 *
 * @param cluster Cluster number (starting from 2)
 * @return Pointer to the memory area where the first sector of the cluster is located.
 */
template <class T>
T* GetSectorByCluster(unsigned long cluster) {
  return reinterpret_cast<T*>(GetClusterAddr(cluster));
}

/** @brief Splits the short name of a directory entry into the base name and extension name.
 * Padded blank characters (0x20) are removed and null-terminated.
 *
 * @param entry The directory entry to get the filename from.
 * @param base File name excluding extension (array of 9 bytes or more).
 * @param ext Extension (array of 4 bytes or more).
 */
void ReadName(const DirectoryEntry& entry, char* base, char* ext);

static const unsigned long kEndOfClusterchain = 0x0ffffffflu;

/** @brief Returns the next cluster number of the specified cluster.
 *
 * @param cluster  Cluster number
 * @return Next cluster number (kEndOfClusterchain if not found)
 */
unsigned long NextCluster(unsigned long cluster);

/** @brief Searches for a file in the specified directory.
 *
 * @param name  8.3 format filename (case-insensitive)
 * @param directory_cluster  Start cluster of the directory (searches from root directory if omitted)
 * @return Entry representing the file, or nullptr if not found.
 */
DirectoryEntry* FindFile(const char* name, unsigned long directory_cluster = 0);

bool NameIsEqual(const DirectoryEntry& entry, const char* name);

} // namespace fat
