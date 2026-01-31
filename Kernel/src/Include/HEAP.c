
#include "HEAP.h"
#include "PMM.h"
#include "VMM.h"
#include "config.h"


void heap_expand(uint32 pages) {
    for(uint32 i = 0; i < pages; i++) {
        
        uint32 phys = alloc_page();
        if(!phys) {
            kprintf("%s %s ERROR: Out of memory for heap expansion!\r\n",
                    LOG_ERROR, LOG_HEAP);
            return;
        }
        
        
        uint32 virt = TEST_HEAP_VIRT + heap_total;
        vmm_map_page(virt, phys, PG_PRESENT | PG_WRITABLE);
        
        
        heap_total += PAGE_SIZE;
        
        
        if(heap_start == 0) {
            heap_start = (block_t*)virt;
            heap_start->size = PAGE_SIZE - sizeof(block_t);
            heap_start->used = 0;
            heap_start->next = 0;
            heap_start->prev = 0;
            free_list = heap_start;
            heap_blocks = 1;
        } else {
            
            block_t* new_block = (block_t*)virt;
            new_block->size = PAGE_SIZE - sizeof(block_t);
            new_block->used = 0;
            new_block->next = 0;
            new_block->prev = 0;
            
            
            block_t* last = free_list;
            while(last && last->next) last = last->next;
            
            if(last) {
                last->next = new_block;
                new_block->prev = last;
            } else {
                free_list = new_block;
            }
            heap_blocks++;
        }
    }
    
    kprintf("%s %s Expanded by %d pages, total: %d bytes\r\n",
            LOG_INFO, LOG_HEAP, pages, heap_total);
}

void heap_init(void) {
    kprintf("%s %s Initializing at %x...\r\n", 
            LOG_INFO, LOG_HEAP, TEST_HEAP_VIRT);
    
    
    heap_expand(HEAP_INIT_PAGES);
    kprintf("%s %s DEBUG: TEST_HEAP_VIRT = 0x%x\r\n", 
            LOG_DEBUG, LOG_HEAP, TEST_HEAP_VIRT);
    kprintf("%s %s DEBUG: heap_start will be at 0x%x\r\n",
            LOG_DEBUG, LOG_HEAP, TEST_HEAP_VIRT);
    kprintf("%s %s Ready (%d bytes)\r\n", 
            LOG_INFO, LOG_HEAP, heap_total);
}

static void split_block(block_t* block, uint32 size) {
    
    uint32 remaining = block->size - size;
    if(remaining >= sizeof(block_t) + 16) {
        
        block_t* new_block = (block_t*)((uint8*)block + sizeof(block_t) + size);
        new_block->size = remaining - sizeof(block_t);
        new_block->used = 0;
        new_block->next = block->next;
        new_block->prev = block;
        
        if(block->next) {
            block->next->prev = new_block;
        }
        block->next = new_block;
        block->size = size;
        
        
        if(free_list == block) {
            free_list = new_block;
        }
        
        heap_blocks++;
        
        kprintf("%s %s Split block: %d -> %d + %d\r\n",
                LOG_DEBUG, LOG_HEAP, 
                size + remaining, size, new_block->size);
    }
}

static void coalesce_blocks(void) {
    block_t* current = free_list;
    
    while(current && current->next) {
        
        uint8* current_end = (uint8*)current + sizeof(block_t) + current->size;
        
        if((uint8*)current->next == current_end && 
           !current->used && !current->next->used) {
            
            current->size += sizeof(block_t) + current->next->size;
            current->next = current->next->next;
            
            if(current->next) {
                current->next->prev = current;
            }
            
            heap_blocks--;
            
            kprintf("%s %s Coalesced blocks, new size: %d\r\n",
                    LOG_DEBUG, LOG_HEAP, current->size);
        } else {
            current = current->next;
        }
    }
}

void* kmalloc(uint32 size) {
    if(size == 0) return 0;
    
    
    size = (size + 7) & ~7;
    
    kprintf("%s %s Request: %d bytes\r\n", LOG_DEBUG, LOG_HEAP, size);
    
    
    block_t* current = free_list;
    while(current) {
        if(!current->used && current->size >= size) {
            
            current->used = 1;
            heap_used += current->size;
            
            
            split_block(current, size);
            
            
            if(current->prev) {
                current->prev->next = current->next;
            } else {
                free_list = current->next;
            }
            if(current->next) {
                current->next->prev = current->prev;
            }
            
            kprintf("%s %s Allocated %d bytes at %x\r\n",
                    LOG_INFO, LOG_HEAP, size, 
                    (uint32)current + sizeof(block_t));
            
            return (void*)((uint8*)current + sizeof(block_t));
        }
        current = current->next;
    }
    
    
    uint32 needed = size + sizeof(block_t);
    uint32 pages_needed = (needed + PAGE_SIZE - 1) / PAGE_SIZE;
    
    kprintf("%s %s No free block, expanding by %d pages...\r\n",
            LOG_DEBUG, LOG_HEAP, pages_needed);
    
    heap_expand(pages_needed);
    
    
    return kmalloc(size);
}

void kfree(void* ptr) {
    if(!ptr) return;
    
    block_t* block = (block_t*)((uint8*)ptr - sizeof(block_t));
    
    if(!block->used) {
        kprintf("%s %s ERROR: Double free at %x!\r\n",
                LOG_ERROR, LOG_HEAP, ptr);
        return;
    }
    
    block->used = 0;
    heap_used -= block->size;
    
    
    block_t* current = free_list;
    block_t* prev = 0;
    
    while(current && current < block) {
        prev = current;
        current = current->next;
    }
    
    block->prev = prev;
    block->next = current;
    
    if(prev) {
        prev->next = block;
    } else {
        free_list = block;
    }
    
    if(current) {
        current->prev = block;
    }
    
    
    coalesce_blocks();
    
    kprintf("%s %s Freed %d bytes at %x\r\n",
            LOG_INFO, LOG_HEAP, block->size, ptr);
}

void heap_print_stats(void) {
    kprintf("%s %s Stats: total=%d, used=%d, free=%d, blocks=%d\r\n",
            LOG_INFO, LOG_HEAP, 
            heap_total, heap_used, heap_total - heap_used, heap_blocks);
    
    #ifdef HEAP_DEBUG
    block_t* current = heap_start;
    uint32 offset = 0;
    
    kprintf("%s %s Block dump:\r\n", LOG_DEBUG, LOG_HEAP);
    while(offset < heap_total) {
        kprintf("  %x: size=%d, used=%d\r\n",
                (uint32)current, current->size, current->used);
        
        offset += sizeof(block_t) + current->size;
        current = (block_t*)((uint8*)current + sizeof(block_t) + current->size);
    }
    #endif
}