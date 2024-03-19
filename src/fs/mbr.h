#ifndef MBR_H
#define MBR_H

typedef struct mbr_entry {
    uint8_t status;
    uint8_t s_head;
    uint8_t s_sect_hi;
    uint8_t s_lo;
    uint8_t part_type;
    uint8_t e_head;
    uint8_t e_sect_hi;
    uint8_t e_lo;

    uint32_t first_lba;
    uint32_t sector_ct;
} __attribute__((packed)) mbr_entry;

typedef struct mbr_table {
    mbr_entry pe_1;
    mbr_entry pe_2;
    mbr_entry pe_3;
    mbr_entry pe_4;
    uint8_t sig_1;
    uint8_t sig_2;
} __attribute__((packed)) mbr_table;

/* 
reads the mbr and returns a malloc'd struct
containing its contents.
*/
mbr_table *read_mbr();

/* print out the values from the mbr */
void print_mbr(mbr_table *mbr);

#endif