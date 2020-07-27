#ifndef OSDEV_PAGING_H
#define OSDEV_PAGING_H
#include <stdint.h>
#include <interrupts.h>
#include <stdbool.h>


// Reference https://software.intel.com/sites/default/files/managed/39/c5/325462-sdm-vol-1-2abcd-3abcd.pdf page 2898

typedef struct page_table_entry
{
	uint32_t present        : 1;   ///< Does page exist
	uint32_t rw             : 1;   ///< Can page be written to
	uint32_t user           : 1;   ///< Can be accessed from user mode
	uint32_t write_though   : 1;   ///< Should write though caching be enabled
	uint32_t cache_disabled : 1;   ///< Should cache be disabled
	uint32_t accessed       : 1;   ///< Has paged been accessed
	uint32_t dirty          : 1;   ///< Has page been written to
	uint32_t pat            : 1;   ///< Page attribute table
	uint32_t global         : 1;   ///< Stops TLB from updating cache when CR3 is reset and PGE flag is set in cr4
	uint32_t ignored        : 3;   ///< Does nothing
	uint32_t address        : 20;  ///< Page address >> 12
} page_t;

typedef struct page_directory_entry
{
	uint32_t present        : 1;   ///< Does page table exist
	uint32_t rw             : 1;   ///< Can page table region be written to
	uint32_t user           : 1;   ///< Can be accessed from user mode
	uint32_t write_though   : 1;   ///< Should write though caching be enabled
	uint32_t cache_disabled : 1;   ///< Should cache be disabled
	uint32_t accessed       : 1;   ///< Has table been used for translation
	uint32_t dirty          : 1;   ///< Has page been written to, only used with 4 MiB pages
	uint32_t page_size      : 1;   ///< Sets page size. 0 for 4 KiB. 1 for 4 MiB
	uint32_t global         : 1;   ///< Stops TLB from updating cache when CR3 is reset and PGE flag is set in cr4
	uint32_t ignored        : 2;   ///< Does nothing
	uint32_t pat            : 1;   ///< Page attribute table, only used for 4 MiB pages
	uint32_t address        : 20;  ///< Page table address >> 12 (with 4KiB pages)
} page_directory_entry_t;


typedef struct page_table
{
	page_t pages[1024];
} page_table_t;

typedef struct page_directory
{
	page_directory_entry_t (*page_tables_physical)[1024]; ///< Physical location of page tables

	page_table_t* (*page_tables)[1024]; ///< Page tables

	uint32_t page_directory_physical_address; ///< Physical location of page directory;
} page_directory_t;

typedef struct pagefault_error_code
{
	uint32_t present    : 1;   ///< Was fault caused by present page
	uint32_t rw         : 1;   ///< Was fault caused by read or write
	uint32_t user       : 1;   ///< Did fault occur in user mode
	uint32_t rsvd       : 1;   ///< Was fault caused by reserved bit
	uint32_t id         : 1;   ///< Was fault caused by instruction fetch
	uint32_t pk         : 1;   ///< Was fault caused by protection key violation
	uint32_t ss         : 1;   ///< Was fault caused by shadow stack access
	uint32_t reserved   : 8;   ///< Not currently used
	uint32_t sgx        : 1;   ///< Is fault related to SGX (Software Guard Extensions)
	uint32_t reserved2  : 16;  ///< Not currently used
} pagefault_error_code_t;

#define PAGE_SIZE 0x1000u
#define PAGE_TABLE_NUM_PAGES 1024u
#define NUM_PAGE_TABLES 1024u
#define KHEAP_SIZE 32u * 1024u * 1024u // Todo: allow kheap to grow in size dynamically at runtime
#define KERNEL_SPACE 0xC0000000u
#define KERNEL_PAGE_TABLES_START 768u


/**
 * Switches page directory by loading value into CR3
 * Expects physical address
 * @param [int] new physical address of page directory
 */
void switch_page_directory(uint32_t new);
void initialise_paging(void);

page_table_t* get_page_table(uint32_t address, page_directory_t *dir, bool make);
page_t* get_page(uint32_t address, page_directory_t *dir, bool make);

void page_fault(registers_t* regs);
void alloc_page(page_t *page, int is_kernel, int is_writeable);

/**
 *
 * @param [in,out] page page to modify
 * @param [in] is_kernel should the page only be accesible for kernel mode
 * @param [in] is_writeable can the page be written to
 * @param [in] addr physical address page should map to
 */
void force_page_phys(page_t *page, int is_kernel, int is_writeable, uint32_t addr);
void free_page_entry(page_t* page);
void free_page(uint32_t page_addr);
void clone_page_directory(page_directory_t* src, page_directory_t* dst);
void* get_free_page(uint32_t* phys);

#endif //OSDEV_PAGING_H
