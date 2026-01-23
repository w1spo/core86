#include "HEAP.h"
#include "VGA_H.h"
#include "UTILS.h"

static Block* free_list = 0;
static Block* used_list = 0;

void HEAP_INIT(void) {
    // Initialize first free block covering entire heap
    Block* first_block = (Block*)HEAP_START;
    first_block->magic = BLOCK_MAGIC;
    first_block->size = HEAP_SIZE - BLOCK_SIZE;
    first_block->used = 0;
    first_block->next = 0;
    first_block->prev = 0;
    
    free_list = first_block;
    used_list = 0;
    
    vga_print("[HEAP] Initialized with free list allocator\n");
}

// Find first fit free block
static Block* find_free_block(ukint32 size) {
    Block* current = free_list;
    while(current) {
        if(current->size >= size && !current->used) {
            return current;  // First fit
        }
        current = current->next;
    }
    
    return 0;
}

void* KMALLOC(ukint32 size) {
    if(size == 0) return 0;
    
    // Align size to 16 bytes
    size = (size + 15) & ~15;
    
    // Find free block
    Block* block = find_free_block(size);
    
    if(!block) {
        vga_print("[HEAP] OUT OF MEMORY! Requested: ");
        char buffer[12];
        int_to_str(size, buffer);
        vga_print(buffer);
        vga_print(" bytes\n");
        return 0;
    }
    
    // Can we split the block?
    if(block->size >= size + BLOCK_SIZE + MIN_BLOCK_SIZE) {
        // Create new free block after this one
        Block* new_block = (Block*)((ukint_8*)block + BLOCK_SIZE + size);
        new_block->magic = BLOCK_MAGIC;
        new_block->size = block->size - size - BLOCK_SIZE;
        new_block->used = 0;
        new_block->next = block->next;
        new_block->prev = block;
        
        if(block->next) {
            block->next->prev = new_block;
        }
        block->next = new_block;
        block->size = size;
    }
    
    // Mark as used
    block->used = 1;
    
    // Remove from free list
    if(block->prev) {
        block->prev->next = block->next;
    } else {
        free_list = block->next;
    }
    if(block->next) {
        block->next->prev = block->prev;
    }
    
    // Add to used list
    block->prev = 0;
    block->next = used_list;
    if(used_list) {
        used_list->prev = block;
    }
    used_list = block;
    
    // Return pointer to data area (after block header)
    return (void*)((ukint_8*)block + BLOCK_SIZE);
}

void KFREE(void* ptr) {
    if(!ptr) return;
    
    // Get block pointer from data pointer
    Block* block = (Block*)((ukint_8*)ptr - BLOCK_SIZE);
    
    // Validate block
    if(block->magic != BLOCK_MAGIC) {
        vga_print("[HEAP] ERROR: Invalid pointer in KFREE!\n");
        return;
    }
    
    if(!block->used) {
        vga_print("[HEAP] ERROR: Double free detected!\n");
        return;
    }
    
    // Mark as free
    block->used = 0;
    
    // Remove from used list
    if(block->prev) {
        block->prev->next = block->next;
    } else {
        used_list = block->next;
    }
    if(block->next) {
        block->next->prev = block->prev;
    }
    
    // Add to free list at beginning (simple)
    block->prev = 0;
    block->next = free_list;
    if(free_list) {
        free_list->prev = block;
    }
    free_list = block;
    
    // Try to merge with next block if free and contiguous
    if(block->next && !block->next->used && 
       (ukint_8*)block + BLOCK_SIZE + block->size == (ukint_8*)block->next) {
        // Merge with next
        block->size += BLOCK_SIZE + block->next->size;
        block->next = block->next->next;
        if(block->next) {
            block->next->prev = block;
        }
    }
    
    // Try to merge with previous block if free and contiguous
    if(block->prev && !block->prev->used &&
       (ukint_8*)block->prev + BLOCK_SIZE + block->prev->size == (ukint_8*)block) {
        // Merge with previous
        block->prev->size += BLOCK_SIZE + block->size;
        block->prev->next = block->next;
        if(block->next) {
            block->next->prev = block->prev;
        }
        
        // Update block pointer for consistency
        block = block->prev;
    }
}

void HEAP_PRINT_STATUS(void) {
    ukint32 used_bytes = 0;
    ukint32 free_bytes = 0;
    ukint32 used_blocks = 0;
    ukint32 free_blocks = 0;
    
    // Count used blocks
    Block* current = used_list;
    while(current) {
        used_bytes += current->size;
        used_blocks++;
        current = current->next;
    }
    
    // Count free blocks
    current = free_list;
    while(current) {
        free_bytes += current->size;
        free_blocks++;
        current = current->next;
    }
    
    char buffer[12];
    vga_print("\n[HEAP] STATUS\n");
    
    vga_print("[HEAP] Start: (0x");
    char hex_buf[9];
    ukint32 temp = HEAP_START;
    const char* hex_digits = "0123456789ABCDEF";
    for(int i = 7; i >= 0; i--) {
        hex_buf[i] = hex_digits[temp & 0xF];
        temp >>= 4;
    }
    hex_buf[8] = '\0';
    vga_print(hex_buf);
    vga_print(")\n");
    
    vga_print("[HEAP] End: (0x");
    temp = HEAP_END;
    for(int i = 7; i >= 0; i--) {
        hex_buf[i] = hex_digits[temp & 0xF];
        temp >>= 4;
    }
    vga_print(hex_buf);
    vga_print(")\n");
    
    vga_print("[HEAP] Used: ");
    int_to_str(used_bytes, buffer);
    vga_print(buffer);
    vga_print(" bytes (");
    int_to_str(used_blocks, buffer);
    vga_print(buffer);
    vga_print(" blocks)\n");
    
    vga_print("[HEAP] Free: ");
    int_to_str(free_bytes, buffer);
    vga_print(buffer);
    vga_print(" bytes (");
    int_to_str(free_blocks, buffer);
    vga_print(buffer);
    vga_print(" blocks)\n");
}

// Debug function to dump heap
void HEAP_DEBUG_DUMP(void) {
    vga_print("\n[HEAP] DEBUG DUMP:\n");
    
    vga_print("Used list:\n");
    Block* current = used_list;
    while(current) {
        char buffer[12];
        vga_print("  Block at 0x");
        vga_print(hex_to_str((ukint32)current, buffer));
        vga_print(", size: ");
        int_to_str(current->size, buffer);
        vga_print(buffer);
        vga_print("\n");
        current = current->next;
    }
    
    vga_print("Free list:\n");
    current = free_list;
    while(current) {
        char buffer[12];
        vga_print("  Block at 0x");
        vga_print(hex_to_str((ukint32)current, buffer));
        vga_print(", size: ");
        int_to_str(current->size, buffer);
        vga_print(buffer);
        vga_print("\n");
        current = current->next;
    }
}

void HEAP_PRINT_DETAILED(void) {
    ukint32 total_used = 0;
    ukint32 total_free = 0;
    int used_count = 0;
    int free_count = 0;
    
    // Count used blocks
    Block* current = used_list;
    while(current) {
        total_used += current->size;
        used_count++;
        current = current->next;
    }
    
    // Count free blocks
    current = free_list;
    while(current) {
        total_free += current->size;
        free_count++;
        current = current->next;
    }
    
    char buf[12];
    vga_print("\n[HEAP DETAILED]\n");
    vga_print("Used: ");
    int_to_str(total_used, buf);
    vga_print(buf);
    vga_print(" bytes in ");
    int_to_str(used_count, buf);
    vga_print(buf);
    vga_print(" blocks\n");
    
    vga_print("Free: ");
    int_to_str(total_free, buf);
    vga_print(buf);
    vga_print(" bytes in ");
    int_to_str(free_count, buf);
    vga_print(buf);
    vga_print(" blocks\n");
    
    // Largest free block
    ukint32 largest_free = 0;
    current = free_list;
    while(current) {
        if(current->size > largest_free) {
            largest_free = current->size;
        }
        current = current->next;
    }
    
    vga_print("Largest free block: ");
    int_to_str(largest_free, buf);
    vga_print(buf);
    vga_print(" bytes\n");
}

// Reset heap completely
void HEAP_RESET(void) {
    vga_print("[HEAP] Resetting heap...\n");
    HEAP_INIT();
}