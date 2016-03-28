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
    uint32_t msm_id2;
    uint32_t pmic1;
    uint32_t pmic2;
    uint32_t pmic3;
    uint32_t pmic4;
    uint32_t offset;
    uint32_t len;
} dtb_entry_v3;

void dump_files_v3(FILE *fd, qca_head header, uint32_t headerat) {
    int i;
    dtb_entry_v3 *images = malloc(header.num * sizeof(dtb_entry_v3));

    printf("\nPid\tVid\tSrev\tmsm_id2\tpmic1\tpmic2\tpmic3\tpmic4\toffset\tlen\n");
    for ( i = 0; i < header.num ; i++ ){
        fread(&images[i], sizeof(dtb_entry_v3), 1, fd);
        printf("%x\t%x\t%x\t%x\t%x\t%x\t%x\t%x\t%x\t%x\n",
               images[i].platform_id, images[i].variant_id,
               images[i].sec_rev, images[i].msm_id2,
               images[i].pmic1, images[i].pmic2,
               images[i].pmic3, images[i].pmic4,
               images[i].offset, images[i].len);
        printf("  qcom,msm-id=<0x%x 0x%x>;\n",images[i].platform_id, images[i].msm_id2);
        printf("  qcom,pmic-id=<0x%x 0x%x 0x%x 0x%x>;\n", images[i].pmic1, images[i].pmic2, images[i].pmic3, images[i].pmic4);
        printf("  qcom,board-id=<0x%x 0x%x>;\n", images[i].variant_id, images[i].sec_rev);
    }
    printf("\n");
    fseek(fd, headerat, SEEK_SET);
    for ( i = 0; i < header.num; i++ ){
        char dtbname[256];
        char *dtb;
        FILE *out_fd = NULL;
        sprintf(dtbname, "%x_%x_%x_%x.dtb", images[i].platform_id, images[i].variant_id,
                                         images[i].sec_rev, images[i].msm_id2);
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

dt_parser v3_parser = {
   .dt_file_dumper = &dump_files_v3,
   .version = 3,
   .extended = 0,
};

int __attribute__((constructor)) register_v3_parser(void) {
    add_dt_parser(&v3_parser);
    return 0;
}
