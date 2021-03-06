#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "lpd8806.h"
#include "colors.h"
#include "effects.h"

static int help(char *executable) {

	printf("%s effect options\n",executable);
	printf("\tbargraph color1 color2 -- alternate two colors\n");
	printf("\t\tbargraph_manual color1 color2 -- keyboard controlled\n");
	printf("\tdisable -- set display to black\n");
	printf("\tfalling color1 -- falling dots\n");
	printf("\tfish num -- moving fish-like dots\n");
	printf("\tgradient color -- moving gradient\n");
	printf("\tnoise -- random_static\n");
	printf("\tpulsar color -- wide band of color scroll\n");
	printf("\trainbow -- rainbow pattern \n");
	printf("\tscanner color -- colored bar that bounces back and forth\n");
	printf("\t\tscanner_blinky -- blinky scanner\n");
	printf("\t\tscanner_dual color1 color2 -- two color scanner\n");
	printf("\t\tscanner_random -- scanner with random color changes\n");
	printf("\tstars speed brightness -- star pattern\n");
	printf("\ttwo_color_scroll color1 color2 direction\n");
	printf("\t\tred_green -- two_color_scroll with red/green default\n");
	printf("\t\tblue_yellow -- two_color_scroll with blue/yellow default\n");
	printf("\n");

	return 0;
}

int main(int argc, char **argv) {

	int spi_fd;
	int effect=EFFECT_RANDOM;

	if (argc>1) {
		if ((!strncmp(argv[1],"-h",2)) || (!strncmp(argv[1],"help",4))) {
			help(argv[0]);
			return 0;
		}

		if (!strncmp(argv[1],"bargraph_mannual",15)) {
			effect=EFFECT_BARGRAPH_MANUAL;
		}

		if (!strncmp(argv[1],"bargraph",8)) {
			effect=EFFECT_BARGRAPH;
		}

		if (!strncmp(argv[1],"disable",7)) {
			effect=EFFECT_DISABLE;
		}

		if (!strncmp(argv[1],"falling",7)) {
			effect=EFFECT_FALLING;
		}


		if (!strncmp(argv[1],"fish",4)) {
			effect=EFFECT_FISH;
		}

		if (!strncmp(argv[1],"gradient",8)) {
			effect=EFFECT_GRADIENT;
		}

		if (!strncmp(argv[1],"noise",5)) {
			effect=EFFECT_NOISE;
		}

		if (!strncmp(argv[1],"pulsar",6)) {
			effect=EFFECT_PULSAR;
		}

		if (!strncmp(argv[1],"rainbow",7)) {
			effect=EFFECT_RAINBOW;
		}

		if (!strncmp(argv[1],"stars",5)) {
			effect=EFFECT_STARS;
		}

		if (!strncmp(argv[1],"red_green",9)) {
			effect=EFFECT_RED_GREEN;
		}

		if (!strncmp(argv[1],"blue_yellow",11)) {
			effect=EFFECT_BLUE_YELLOW;
		}

		if (!strncmp(argv[1],"two_color_scroll",15)) {
			effect=EFFECT_TWO_COLOR_SCROLL;
		}

		if (!strncmp(argv[1],"scanner_blinky",14)) {
			effect=EFFECT_SCANNER_BLINKY;
		} else

		if (!strncmp(argv[1],"scanner_dual",12)) {
			effect=EFFECT_SCANNER_DUAL;
		} else

		if (!strncmp(argv[1],"scanner_random",14)) {
			effect=EFFECT_SCANNER_RANDOM;
		} else

		if (!strncmp(argv[1],"scanner",7)) {
			effect=EFFECT_SCANNER;
		}


	}

	spi_fd=lpd8806_init();
	if (spi_fd<0) {
		exit(-1);
	}

	switch(effect) {
		case EFFECT_BARGRAPH:
			bargraph(spi_fd,argc>2?argv[2]:NULL,
					argc>3?argv[3]:NULL);
			break;

		case EFFECT_BARGRAPH_MANUAL:
			bargraph_manual(spi_fd,argc>2?argv[2]:NULL,
					argc>3?argv[3]:NULL);
			break;

		case EFFECT_DISABLE:
			disable(spi_fd);
			break;

		case EFFECT_FALLING:
			falling(spi_fd,
				argc>2?argv[2]:NULL,
				argc>2?argv[3]:NULL);
			break;

		case EFFECT_FISH:
			fish(spi_fd,argc>2?argv[2]:NULL);
			break;

		case EFFECT_GRADIENT:
			gradient(spi_fd,argc>2?argv[2]:NULL);
			break;

		case EFFECT_NOISE:
			noise(spi_fd);
			break;

		case EFFECT_PULSAR:
			pulsar(spi_fd,argc>2?argv[2]:NULL);
			break;

		case EFFECT_RAINBOW:
			rainbow(spi_fd);
			break;

		case EFFECT_STARS:
			stars(spi_fd,argc>2?argv[2]:NULL,
					argc>3?argv[3]:NULL);
			break;
		case EFFECT_TWO_COLOR_SCROLL:
			two_color_scroll(spi_fd,
					argc>2?argv[2]:NULL,
					argc>3?argv[3]:NULL,
					argc>4?argv[4]:NULL);
			break;

		case EFFECT_RED_GREEN:
			two_color_scroll(spi_fd,
					"red",
					"green",
					"1");
			break;
		case EFFECT_BLUE_YELLOW:
			two_color_scroll(spi_fd,
					"blue",
					"yellow",
					"0");
			break;

		case EFFECT_SCANNER:
			scanner(spi_fd,argc>2?argv[2]:NULL);
			break;

		case EFFECT_SCANNER_BLINKY:
			scanner_blinky(spi_fd);
			break;

		case EFFECT_SCANNER_DUAL:
			scanner_dual(spi_fd,
				argc>2?argv[2]:NULL,
				argc>3?argv[3]:NULL);
			break;

		case EFFECT_SCANNER_RANDOM:
			scanner_random(spi_fd);
			break;


	}

	lpd8806_close(spi_fd);

	return 0;
}
