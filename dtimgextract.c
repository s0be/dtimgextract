#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

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
    uint32_t unknown;
    uint32_t unk5;
    uint32_t unk4;
    uint32_t unk2;
    uint32_t unk3;
    uint32_t offset;
    uint32_t len;
} dtb_entry_v3;

void dump_files_v1(FILE *fd, qca_head header) {
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
    fseek(fd, 0, SEEK_SET);
    for ( i = 0; i < header.num; i++ ){
	char dtbname[256];
	char *dtb;
        FILE *out_fd = NULL;
        sprintf(dtbname, "%x_%x_%x.dtb", images[i].platform_id, images[i].variant_id,
                                         images[i].sec_rev);
	printf("Writing %s(%x bytes)\n", dtbname, images[i].len);
	dtb = malloc(images[i].len);
	fseek(fd, images[i].offset, SEEK_SET);
	fread(dtb, images[i].len, 1, fd);
	out_fd = fopen(dtbname, "wb");
	fwrite(dtb, images[i].len, 1, out_fd);
	free(dtb);
	fclose(out_fd);
    }
    free(images);
}

void dump_files_v2(FILE *fd, qca_head header) {
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
    fseek(fd, 0, SEEK_SET);
    for ( i = 0; i < header.num; i++ ){
	char dtbname[256];
	char *dtb;
        FILE *out_fd = NULL;
        sprintf(dtbname, "%x_%x_%x.dtb", images[i].platform_id, images[i].variant_id,
                                         images[i].sec_rev);
	printf("Writing %s(%x bytes)\n", dtbname, images[i].len);
	dtb = malloc(images[i].len);
	fseek(fd, images[i].offset, SEEK_SET);
	fread(dtb, images[i].len, 1, fd);
	out_fd = fopen(dtbname, "wb");
	fwrite(dtb, images[i].len, 1, out_fd);
	free(dtb);
	fclose(out_fd);
    }
    free(images);
}

void dump_files_v3(FILE *fd, qca_head header) {
    int i;
    dtb_entry_v3 *images = malloc(header.num * sizeof(dtb_entry_v3));

    printf("\nPid\tVid\tSrev\tUnknown\tOffset\tlen\tunk2\tunk3\tunk4\tunk5\n");
    for ( i = 0; i < header.num ; i++ ){
        fread(&images[i], sizeof(dtb_entry_v3), 1, fd);
        printf("%x\t%x\t%x\t%x\t%x\t%x\t%x\t%x\t%x\t%x\n", images[i].platform_id, images[i].variant_id,
                                           images[i].sec_rev, images[i].unknown,
                                           images[i].offset, images[i].len,
					   images[i].unk2, images[i].unk3,
                                           images[i].unk4, images[i].unk5);
    }
    printf("\n");
    fseek(fd, 0, SEEK_SET);
    for ( i = 0; i < header.num; i++ ){
	char dtbname[256];
	char *dtb;
        FILE *out_fd = NULL;
        sprintf(dtbname, "%x_%x_%x.dtb", images[i].platform_id, images[i].variant_id,
                                         images[i].sec_rev);
	printf("Writing %s(%x bytes)\n", dtbname, images[i].len);
	dtb = malloc(images[i].len);
	fseek(fd, images[i].offset, SEEK_SET);
	fread(dtb, images[i].len, 1, fd);
	out_fd = fopen(dtbname, "wb");
	fwrite(dtb, images[i].len, 1, out_fd);
	free(dtb);
	fclose(out_fd);
    }
    free(images);
}

void splitFile(char *file){

    FILE *fd = NULL;
    FILE *ft = NULL;
    int i=0,imagecount=0;
    qca_head header;
    dtb_entry_v2 *images;

    if ( (fd=fopen(file,"rb")) == NULL ) {
        printf ( "Extract dt.img file, open %s failure!\n", file );
        exit(1);
    }

    fread(&header, sizeof(header), 1, fd);

    printf("qc_magic: %s\n", header.qc_magic);
    printf("version: %u\n", header.version);
    printf("count: %u\n", header.num);

    switch(header.version) {
        case 1: dump_files_v1(fd, header); break;
        case 2: dump_files_v2(fd, header); break; 
        case 3: dump_files_v3(fd, header); break;
        default: printf("header v%u format not implemented\n", header.version); return;
    }
 
    fclose(fd);
}

int main ( int argc, char *argv[] )
{
    if (argc==1) {
        printf("usage:%s dt.img.\n", argv[0]);
        exit(0);
    }
    
    char *dtb;
    dtb=argv[1];
    splitFile(dtb);

    return EXIT_SUCCESS;
}
