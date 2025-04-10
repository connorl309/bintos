#pragma once
// Limine requests kept in this file
#include "../limine.h"

// Limine request list
__attribute__((used, section(".limine_requests_start")))
static volatile LIMINE_REQUESTS_START_MARKER;

__attribute__((used, section(".limine_requests")))
static volatile LIMINE_BASE_REVISION(3);

// Framebuffer
__attribute__((used, section(".limine_requests")))
static volatile struct limine_framebuffer_request framebuffer_request = {
    .id = LIMINE_FRAMEBUFFER_REQUEST,
    .revision = 0
};

// HHDM for memory mapping later
__attribute__((used, section(".limine_requests")))
static volatile struct limine_hhdm_request hhdm_request = {
    .id = LIMINE_HHDM_REQUEST,
    .revision = 0
};

// Memory map
__attribute__((used, section(".limine_requests")))
static volatile struct limine_memmap_request memmap_request = {
    .id = LIMINE_MEMMAP_REQUEST,
    .revision = 0
};

// File information
__attribute__((used, section(".limine_requests")))
static volatile struct limine_kernel_file_request kernel_request = {
    .id = LIMINE_KERNEL_FILE_REQUEST,
    .revision = 0
};

// Virtual and physical base addresses for the kernel
__attribute__((used, section(".limine_requests")))
static volatile struct limine_kernel_address_request addr_request = {
    .id = LIMINE_KERNEL_ADDRESS_REQUEST,
    .revision = 0
};

// Device tree information
// https://github.com/devicetree-org/devicetree-specification/releases/tag/v0.4
__attribute__((used, section(".limine_requests")))
static volatile struct limine_dtb_request devtree_request = {
    .id = LIMINE_DTB_REQUEST,
    .revision = 0
};

__attribute__((used, section(".limine_requests_end")))
static volatile LIMINE_REQUESTS_END_MARKER;// Please don't modify where it is relative to kmain : )