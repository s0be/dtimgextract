#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#include "dtimgextract.h"

typedef struct {
    uint32_t platform_id;
    uint32_t variant_id;
    uint32_t sec_rev;
    uint32_t unknown;
    uint32_t offset;
    uint32_t len;
} dtb_entry_v2;

void dump_files_v2(FILE *fd, qca_head header, uint32_t headerat) {
    int i;
    dtb_entry_v2 *images = malloc(header.num * sizeof(dtb_entry_v2));

    printf("\nPid\tVid\tSrev\tUnknown\tOffset\tlen\n");
    for ( i = 0; i < header.num ; i++ ){
        fread(&images[i], sizeof(dtb_entry_v2), 1, fd);
        printf("%x\t%x\t%x\t%x\t%x\t%x\n", images[i].platform_id, images[i].variant_id,
                                           images[i].sec_rev, images[i].unknown,
                                           images[i].offset, images[i].len);
    }
    printf("\n");
    fseek(fd, headerat, SEEK_SET);
    for ( i = 0; i < header.num; i++ ){
        char dtbname[256];
        char *dtb;
        FILE *out_fd = NULL;
        sprintf(dtbname, "%x_%x_%x.dtb", images[i].platform_id, images[i].variant_id,
                                         images[i].sec_rev);
        printf("Writing %s(%x bytes)\n", dtbname, images[i].len);
        dtb = malloc(images[i].len);
        fseek(fd, images[i].offset + headerat, SEEK_SET);
        fread(dtb, images[i].len, 1, fd);
        out_fd = fopen(dtbname, "wb");
        fwrite(dtb, images[i].len, 1, out_fd);
        free(dtb);
        fclose(out_fd);
    }
    free(images);
}

dt_parser v2_parser = {
   .dt_file_dumper = &dump_files_v2,
   .version = 2,
   .extended = 0,
};

int __attribute__((constructor)) register_v2_parser(void) {
    add_dt_parser(&v2_parser);
    return 0;
}
