/* Convert ym5 file to raw, uncompressed register dump */

/* ym5 files are usually set up so each register is in a row, then LHA */
/* compressed. */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <math.h>
#include <sys/resource.h>

#include "stats.h"
#include "ym_lib.h"

#define VERSION "0.7"

static void print_help(int just_version, char *exec_name) {

	printf("\nym5_to_raw version %s by Vince Weaver <vince@deater.net>\n\n",VERSION);
	if (just_version) exit(0);

	printf("This converts ym5 files to a raw form\n\n");

	printf("Usage:\n");
	printf("\t%s [-h] [-v] [-d] [-i] [-r] filename\n\n",
		exec_name);
	printf("\t-h: this help message\n");
	printf("\t-v: version info\n");
	printf("\t-d: print debug messages\n");
	printf("\t-i: interleaved file\n");
	printf("\t-r: raw uncompressed data\n");

	exit(0);
}


static int dump_song_raw_interleaved(char *filename, int debug, int size,
		char *outfile) {

	int result;

	int x,y;

	struct ym_song_t ym_song;

	unsigned char *interleaved_data;
	unsigned char frame[YM5_FRAME_SIZE];

	fprintf(stderr, "\nDumping song %s\n",filename);

	result=load_ym_song(filename,&ym_song);
	if (result<0) {
		return -1;
	}

	/**********************/
	/* Print song summary */
	/**********************/
#if 0
	int length_seconds;

	printf("\tYM%d",ym_song.type);
	printf("\tSong attributes (%d) : ",ym_song.attributes);
	printf("Interleaved=%s\n",ym_song.interleaved?"yes":"no");
	if (ym_song.num_digidrum>0) {
		printf("Num digidrum samples: %d\n",ym_song.num_digidrum);
	}
	printf("\tFrames: %d, ",ym_song.num_frames);
	printf("Chip clock: %d Hz, ",ym_song.master_clock);
	printf("Frame rate: %d Hz, ",ym_song.frame_rate);
	if (ym_song.frame_rate!=50) {
		fprintf(stderr,"FIX ME framerate %d\n",ym_song.frame_rate);
		exit(1);
	}
	length_seconds=ym_song.num_frames/ym_song.frame_rate;
	printf("Length=%d:%02d\n",length_seconds/60,length_seconds%60);
	printf("\tLoop frame: %d, ",ym_song.loop_frame);
	printf("Extra data size: %d\n",ym_song.extra_data);
	printf("\tSong name: %s\n",ym_song.song_name);
	printf("\tAuthor name: %s\n",ym_song.author);
	printf("\tComment: %s\n",ym_song.comment);
#endif
	/******************/
	/* Play the song! */
	/******************/

	interleaved_data=calloc(ym_song.num_frames,sizeof(char)*14);
	if (interleaved_data==NULL) {
		fprintf(stderr,"Error allocating memory!\n");
		return -1;
	}

	for(y=0;y<14;y++) {
		for(x=0;x<ym_song.num_frames;x++) {

			ym_return_frame(&ym_song,x,frame,NULL);
			interleaved_data[(y*ym_song.num_frames)+x]=
				frame[y];
		}
	}

	int num_chunks;
	FILE *fff;
	char outname[BUFSIZ];
	int j;
	int frames_per_chunk;

	num_chunks=ym_song.num_frames/(1024);
	fprintf(stderr,"%d frames %d chunks total\n",
			ym_song.num_frames,num_chunks);

	for(j=0;j<num_chunks;j++) {

		sprintf(outname,"%s.%d",outfile,j);
		fff=fopen(outname,"w");
		if (fff==NULL) {
			fprintf(stderr,"Error opening %s\n",outname);
			return -1;
		}

		for(y=0;y<14;y++) {
			for(x=0;x<1024;x++) {
				fprintf(fff,"%c",
				interleaved_data[x+(y*ym_song.num_frames)]);
			}
		}
		fclose(fff);
	}

	fprintf(stderr,"; Total size = %d bytes\n",ym_song.num_frames*14);

	free(interleaved_data);

	/* Free the ym file */
	free(ym_song.file_data);

	return 0;
}


int main(int argc, char **argv) {

	char filename[BUFSIZ]="intro2.ym";
	char outfile[BUFSIZ]="out";

	int c,debug=0;
	int first_song;

	int size=4096;

	/* Parse command line arguments */
	while ((c = getopt(argc, argv, "dhv"))!=-1) {
		switch (c) {
			case 'd':
				/* Debug messages */
				printf("Debug enabled\n");
				debug=1;
				break;
			case 'h':
				/* help */
				print_help(0,argv[0]);
				break;
			case 'v':
				/* version */
				print_help(1,argv[0]);
				break;
			default:
				print_help(0,argv[0]);
				break;
		}
	}

	first_song=optind;

	if (argv[first_song]!=NULL) {
		strcpy(filename,argv[first_song]);
	}

	/* Dump the song */
	dump_song_raw_interleaved(filename,debug,size,outfile);

	return 0;
}
