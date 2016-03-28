#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <getopt.h>

#include "dtimgextract.h"

dt_parser **dt_parsers;
int dt_parser_cnt = 0;

void add_dt_parser(dt_parser *dtp) {
    if (!dt_parsers)
        dt_parsers = malloc(sizeof(dt_parser *));
    else
        dt_parsers = realloc(dt_parsers, (dt_parser_cnt + 1) * sizeof(dt_parser *));

    if (!dt_parsers) {
        printf("Failed to allocate memory for %i device tree parsers\n",
	       dt_parser_cnt);
	exit(1);
    }
    dt_parsers[dt_parser_cnt] = dtp;
    dt_parser_cnt++;
}

uint32_t align(uint32_t datalen, uint32_t page) {
    uint32_t seekamt;
    seekamt  = datalen / page;
    seekamt += datalen % page ? 1 : 0;
    seekamt *= page;
    return seekamt;
}

void splitFile(char *file, int offs, int usealt){

    FILE *fd = NULL;
    int headerat = offs;
    qca_head header;

    if ( (fd=fopen(file,"rb")) == NULL ) {
        printf ( "Extract dt.img file, open %s failure!\n", file );
        exit(1);
    }
    fseek(fd, headerat, SEEK_SET);
retry:
    fread(&header, sizeof(qca_head), 1, fd);

    if(strncmp("QCDT", (char *)header.qc_magic, 4))
    {
        boot_head bheader;
        printf("Not a dt.img, processing as boot.img(got %.4s, wanted %s)\n", header.qc_magic, "QCDT");
        fseek(fd, headerat, SEEK_SET);
        fread(&bheader, sizeof(boot_head), 1, fd);
        if(strncmp("ANDROID!", (char *)bheader.hd_magic, 8)) {
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

        uint32_t seekamt;
        fseek (fd, headerat, SEEK_SET);

        seekamt = align(sizeof(boot_head), bheader.p_size);
        printf("Skipping android boot header 0x%lx(0x%x)\n", sizeof(boot_head), seekamt);
        fseek (fd, seekamt, SEEK_CUR);

        seekamt = align(bheader.k_size, bheader.p_size);
        printf("Skipping kernel 0x%x(0x%x)\n", bheader.k_size, seekamt);
        fseek (fd, seekamt, SEEK_CUR);

        seekamt = align(bheader.r_size, bheader.p_size);
        printf("Skipping ramdisk 0x%x(0x%x)\n", bheader.r_size, seekamt);
        fseek (fd, seekamt, SEEK_CUR);

        seekamt = align(bheader.s_size, bheader.p_size);
        printf("Skinning secondary image 0x%x(0x%x)\n", bheader.s_size, seekamt);
        fseek (fd, seekamt, SEEK_CUR);

        headerat = ftell(fd);
        printf("Trying header at 0x%x\n", headerat);
        if(fseek(fd, headerat , SEEK_SET)) return;
        goto retry;
        return;
    }

    printf("qc_magic: %.4s\n", header.qc_magic);
    printf("version: %u\n", header.version);
    printf("count: %u\n", header.num);

    bool dumped = 0;

    for (int i = 0; i < dt_parser_cnt; i++) {
        if (dt_parsers[i]->version == header.version &&
	    dt_parsers[i]->extended == usealt) {
            dt_parsers[i]->dt_file_dumper(fd, header, headerat);
	    dumped = 1;
	}
    }

    if (!dumped)
        printf("header v%u format not implemented\n", header.version); return;

    fclose(fd);
}

void usage(char *name) {
    printf("usage:%s [-a] [-o 0xOFFSET] dt.img\n", name);
}

int main ( int argc, char *argv[] )
{
    int offset = 0;
    char *offset_arg = NULL;
    int usealt = 0;
    char *dtb = NULL;
    int c;

    static struct option long_options[] = {
        {"offset", required_argument, NULL, 'o'},
        {"usealt", no_argument, NULL, 'a'},
        {NULL, 0, NULL, 0}
    };

    int option_index = 0;

    while ( (c = getopt_long (argc, argv, "-ao:",
                        long_options, &option_index)) != -1) {
        switch(c) {
            case 'o': offset_arg = optarg; break;
            case 'a': usealt = 1; break;
            case 1:
                if (dtb == NULL) {
                    dtb = optarg;
                } else {
                    usage(argv[0]);
                    exit(1);
                }
                break;
            case '?':
            default:
                printf("usage\n");
                usage(argv[0]);
                exit(1);
                break;
        }
    }

    if (dtb == NULL || argc == 1) {
        usage(argv[0]);
        exit(1);
    }

    if (offset_arg) {
        printf("Trying offset of %s\n", offset_arg);
        sscanf(offset_arg, "%x", &offset);
    }

    splitFile(dtb, offset, usealt);

    return EXIT_SUCCESS;
}
