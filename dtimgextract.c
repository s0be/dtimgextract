#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

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
    uint32_t platform_id;
    uint32_t variant_id;
    uint32_t sec_rev;
    uint32_t offset;
    uint32_t len;
} dtb_entry_v1;

typedef struct {
    uint32_t platform_id;
    uint32_t variant_id;
    uint32_t sec_rev;
    uint32_t unknown;
    uint32_t offset;
    uint32_t len;
} dtb_entry_v2;

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

typedef struct {
    uint32_t platform_id;
    uint32_t variant_id;
    uint32_t sec_rev;
    uint32_t msm_id2;
    uint32_t pmic1;
    uint32_t pmic2;
    uint32_t pmic3;
    uint32_t pmic4;
    uint32_t unknown1;
    char unknown2[8];
    uint32_t offset;
    uint32_t len;
} dtb_entry_v3_coolpad;

void dump_files_v1(FILE *fd, qca_head header, uint32_t headerat) {
    int i;
    dtb_entry_v1 *images = malloc(header.num * sizeof(dtb_entry_v1));

    printf("\nPid\tVid\tSrev\tOffset\tlen\n");
    for ( i = 0; i < header.num ; i++ ){
        fread(&images[i], sizeof(dtb_entry_v1), 1, fd);
        printf("%x\t%x\t%x\t%x\t%x\n", images[i].platform_id, images[i].variant_id,
                                           images[i].sec_rev,
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

void dump_files_v3_coolpad(FILE *fd, qca_head header, uint32_t headerat) {
    int i;
    dtb_entry_v3_coolpad *images = malloc(header.num * sizeof(dtb_entry_v3_coolpad));

    printf("\nPid\tVid\tSrev\tmsm_id2\tpmic1\tpmic2\tpmic3\tpmic4\tunk1\tunk2\tunk3\toffset\tlen\n");
    for ( i = 0; i < header.num ; i++ ){
        fread(&images[i], sizeof(dtb_entry_v3_coolpad), 1, fd);
        printf("%x\t%x\t"
	       "%x\t%x\t"
	       "%x\t%x\t"
	       "%x\t%x\t"
	       "%x\t%s\t"
	       "%x\t%x\n",
               images[i].platform_id, images[i].variant_id,
               images[i].sec_rev, images[i].msm_id2,
               images[i].pmic1, images[i].pmic2,
               images[i].pmic3, images[i].pmic4,
               images[i].unknown1, images[i].unknown2,
               images[i].offset, images[i].len);
     //  printf("  qcom,msm-id=<0x%x 0x%x>;\n",images[i].platform_id, images[i].msm_id2);
     //  printf("  qcom,pmic-id=<0x%x 0x%x 0x%x 0x%x>;\n", images[i].pmic1, images[i].pmic2, images[i].pmic3, images[i].pmic4);
     //  printf("  qcom,board-id=<0x%x 0x%x>;\n", images[i].variant_id, images[i].sec_rev);
    }
    printf("\n");
    fseek(fd, headerat, SEEK_SET);
    for ( i = 0; i < header.num; i++ ){
        char dtbname[256];
        char *dtb;
        FILE *out_fd = NULL;
        sprintf(dtbname, "%x_%x_%x_%x_%x.dtb", images[i].platform_id, images[i].variant_id,
                                         images[i].sec_rev, images[i].msm_id2, images[i].unknown1);
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

void splitFile(char *file, int offs, int usealt){

    FILE *fd = NULL;
    int i=0,imagecount=0;
    int headerat = offs;
    qca_head header;
    dtb_entry_v2 *images;

    if ( (fd=fopen(file,"rb")) == NULL ) {
        printf ( "Extract dt.img file, open %s failure!\n", file );
        exit(1);
    }
    fseek(fd, headerat, SEEK_SET);
retry:
    fread(&header, sizeof(qca_head), 1, fd);

    if(strncmp("QCDT", header.qc_magic, 4))
    {
        boot_head bheader;
        printf("Not a dt.img, processing as boot.img(got %.4s, wanted %s)\n", header.qc_magic, "QCDT");
        fseek(fd, headerat, SEEK_SET);
        fread(&bheader, sizeof(boot_head), 1, fd);
        if(strncmp("ANDROID!", bheader.hd_magic, 8)) {
            printf("Not a boot.img, aborting (got %.8s, wanted %s)\n", bheader.hd_magic, "ANDROID!");
            return;
        } // 0x1037800/0x106A800 0x33000
        printf("hd_magic: %.8s\n", bheader.hd_magic);
        printf("k_size: 0x%x\n", bheader.k_size);
        printf("k_addr: 0x%x\n", bheader.k_addr);
        printf("r_size: 0x%x\n", bheader.r_size);
        printf("r_addr: 0x%x\n", bheader.r_addr);
        printf("s_size: 0x%x\n", bheader.s_size);
        printf("s_addr: 0x%x\n", bheader.s_addr);
        printf("t_addr: 0x%x\n", bheader.t_addr);
        printf("p_size: 0x%x\n", bheader.p_size);
        printf("unused: 0x%x\n", bheader.unused1);
        printf("unused: 0x%x\n", bheader.unused2);
        printf("pname: %.16s\n", bheader.pname);
        printf("cmdline: %.512s\n", bheader.cmdline);
        printf("id: %u\n", bheader.id);
        fseek (fd, 0, SEEK_END);
        headerat = ftell(fd) - bheader.unused1;
	printf("Trying header at 0x%x\n", headerat);
        if(fseek(fd, headerat , SEEK_SET)) return;
        goto retry;
        return;
    }

    printf("qc_magic: %.4s\n", header.qc_magic);
    printf("version: %u\n", header.version);
    printf("count: %u\n", header.num);

    switch(header.version) {
        case 1: dump_files_v1(fd, header, headerat); break;
        case 2: dump_files_v2(fd, header, headerat); break;
        case 3:
	    if(usealt)
	        dump_files_v3_coolpad(fd, header, headerat);
	    else
	        dump_files_v3(fd, header, headerat);

            break;
        default: printf("header v%u format not implemented\n", header.version); return;
    }

    fclose(fd);
}

int main ( int argc, char *argv[] )
{
    if (argc==1) {
        printf("usage:%s dt.img [offset]\n", argv[0]);
        exit(0);
    }

    char *dtb;
    int offset = 0;
    int usealt = 0;
    if(argc > 2) {
      offset = atoi(argv[2]);
    }
    if(argc > 3) {
      // todo, use getopt
      usealt = 1;
    }
    dtb=argv[1];
    splitFile(dtb, offset, usealt);

    return EXIT_SUCCESS;
}
