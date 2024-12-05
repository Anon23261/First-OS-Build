#ifndef MULTIBOOT_H
#define MULTIBOOT_H

#include <stdint.h>

/* Multiboot header magic value */
#define MULTIBOOT_HEADER_MAGIC 0x1BADB002
#define MULTIBOOT_BOOTLOADER_MAGIC 0x2BADB002

/* Multiboot header flags */
#define MULTIBOOT_PAGE_ALIGN (1 << 0)    /* Align modules on page boundaries */
#define MULTIBOOT_MEMORY_INFO (1 << 1)   /* Provide memory map */
#define MULTIBOOT_VIDEO_MODE (1 << 2)    /* Provide video mode */
#define MULTIBOOT_HEADER_FLAGS (MULTIBOOT_PAGE_ALIGN | MULTIBOOT_MEMORY_INFO)

/* Multiboot header checksum */
#define MULTIBOOT_HEADER_CHECKSUM -(MULTIBOOT_HEADER_MAGIC + MULTIBOOT_HEADER_FLAGS)

/* Memory map entry structure */
struct multiboot_mmap_entry {
    uint32_t size;
    uint64_t addr;
    uint64_t len;
    uint32_t type;
} __attribute__((packed));

/* Memory types */
#define MULTIBOOT_MEMORY_AVAILABLE 1
#define MULTIBOOT_MEMORY_RESERVED 2
#define MULTIBOOT_MEMORY_ACPI_RECLAIMABLE 3
#define MULTIBOOT_MEMORY_NVS 4
#define MULTIBOOT_MEMORY_BADRAM 5

/* VBE controller information */
struct multiboot_vbe_controller_info {
    uint8_t signature[4];
    uint16_t version;
    uint32_t oem_string;
    uint32_t capabilities;
    uint32_t video_mode_ptr;
    uint16_t total_memory;
    uint16_t oem_software_rev;
    uint32_t oem_vendor_name;
    uint32_t oem_product_name;
    uint32_t oem_product_rev;
    uint8_t reserved[222];
    uint8_t oem_data[256];
} __attribute__((packed));

/* VBE mode information */
struct multiboot_vbe_mode_info {
    uint16_t attributes;
    uint8_t window_a;
    uint8_t window_b;
    uint16_t granularity;
    uint16_t window_size;
    uint16_t segment_a;
    uint16_t segment_b;
    uint32_t window_func_ptr;
    uint16_t pitch;
    uint16_t width;
    uint16_t height;
    uint8_t char_width;
    uint8_t char_height;
    uint8_t num_planes;
    uint8_t bpp;
    uint8_t num_banks;
    uint8_t memory_model;
    uint8_t bank_size;
    uint8_t num_image_pages;
    uint8_t reserved0;
    uint8_t red_mask_size;
    uint8_t red_field_position;
    uint8_t green_mask_size;
    uint8_t green_field_position;
    uint8_t blue_mask_size;
    uint8_t blue_field_position;
    uint8_t reserved_mask_size;
    uint8_t reserved_field_position;
    uint8_t direct_color_mode_info;
    uint32_t phys_base;
    uint32_t reserved1;
    uint16_t reserved2;
} __attribute__((packed));

/* Multiboot information flags */
#define MULTIBOOT_INFO_MEMORY (1 << 0)
#define MULTIBOOT_INFO_BOOTDEV (1 << 1)
#define MULTIBOOT_INFO_CMDLINE (1 << 2)
#define MULTIBOOT_INFO_MODS (1 << 3)
#define MULTIBOOT_INFO_SYMS_AOUT (1 << 4)
#define MULTIBOOT_INFO_SYMS_ELF (1 << 5)
#define MULTIBOOT_INFO_MMAP (1 << 6)
#define MULTIBOOT_INFO_DRIVES (1 << 7)
#define MULTIBOOT_INFO_CONFIG (1 << 8)
#define MULTIBOOT_INFO_BOOT_LOADER (1 << 9)
#define MULTIBOOT_INFO_APM (1 << 10)
#define MULTIBOOT_INFO_VBE (1 << 11)
#define MULTIBOOT_INFO_FRAMEBUFFER (1 << 12)

/* Multiboot information structure */
struct multiboot_info {
    uint32_t magic;
    uint32_t flags;
    uint32_t mem_lower;
    uint32_t mem_upper;
    uint32_t boot_device;
    uint32_t cmdline;
    uint32_t mods_count;
    uint32_t mods_addr;
    union {
        struct {
            uint32_t num;
            uint32_t size;
            uint32_t addr;
            uint32_t shndx;
        } elf_sec;
        struct {
            uint32_t tabsize;
            uint32_t strsize;
            uint32_t addr;
            uint32_t reserved;
        } aout_sym;
    } syms;
    uint32_t mmap_length;
    uint32_t mmap_addr;
    uint32_t drives_length;
    uint32_t drives_addr;
    uint32_t config_table;
    uint32_t boot_loader_name;
    uint32_t apm_table;
    struct {
        uint32_t control_info;
        uint32_t mode_info;
        uint16_t mode;
        uint16_t interface_seg;
        uint16_t interface_off;
        uint16_t interface_len;
    } vbe;
    struct {
        uint64_t addr;
        uint32_t pitch;
        uint32_t width;
        uint32_t height;
        uint8_t bpp;
        uint8_t type;
        union {
            struct {
                uint32_t palette_addr;
                uint16_t color_count;
            };
            struct {
                uint8_t red_field_position;
                uint8_t red_mask_size;
                uint8_t green_field_position;
                uint8_t green_mask_size;
                uint8_t blue_field_position;
                uint8_t blue_mask_size;
            };
        };
    } framebuffer;
};

typedef struct multiboot_info multiboot_info_t;

/* Module structure */
struct multiboot_mod_list {
    uint32_t mod_start;
    uint32_t mod_end;
    uint32_t cmdline;
    uint32_t pad;
};

#endif
