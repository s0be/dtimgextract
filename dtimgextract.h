#ifndef dtimgextract_h
#define dtimgextract_h

typedef struct {
    uint8_t hd_magic[8];
    uint32_t k_size;
    uint32_t k_addr;
    uint32_t r_size;
    uint32_t r_addr;
    uint32_t s_size;
    uint32_t s_addr;
    uint32_t t_addr;
    uint32_t p_size;
    uint32_t unused1;
    uint32_t unused2;
    uint8_t pname[16];
    uint8_t cmdline[512];
    uint32_t id;
} boot_head;

typedef struct {
    uint8_t qc_magic[4];
    uint32_t version;
    uint32_t num;
} qca_head;

typedef struct {
    void (*dt_file_dumper)(FILE *fd, qca_head header, uint32_t headerat);
    uint32_t version;
    bool extended;
} dt_parser;

void add_dt_parser(dt_parser *dtp);

#endif
