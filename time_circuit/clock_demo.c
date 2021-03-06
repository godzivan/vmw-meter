/* Makes a clock */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <string.h>

#include "i2c_lib.h"

/* globals */
static struct tcircuit destination_time;

static struct tcircuit yellow_time;
static struct tcircuit green_time;
static struct tcircuit new_time;

static int display_red=1;
static int display_green=1;
static int display_yellow=1;

#define SEG_A 0x0001
#define SEG_B 0x0002
#define SEG_C 0x0004
#define SEG_D 0x0008
#define SEG_E 0x0010
#define SEG_F 0x0020
#define SEG_G 0x0040
#define SEG7_DP 0x0080
#define SEG_H 0x0080
#define SEG_K 0x0100
#define SEG_M 0x0200
#define SEG_N 0x0400
#define SEG_P 0x0800
#define SEG_R 0x2000 /* Hooked up R and S backwards? */
#define SEG_S 0x1000 /* the data sheets list those two out of order, odd */
#define SEG_T 0x4000
#define SEG_U 0x8000

unsigned short font_7seg[10] = {
	SEG_A|SEG_B|SEG_C|SEG_D|SEG_E|SEG_F,	// 0
	SEG_B|SEG_C,				// 1
	SEG_A|SEG_B|SEG_G|SEG_E|SEG_D,		// 2
	SEG_A|SEG_B|SEG_C|SEG_D|SEG_G,		// 3
	SEG_B|SEG_C|SEG_F|SEG_G,		// 4
	SEG_A|SEG_C|SEG_D|SEG_F|SEG_G,		// 5
	SEG_A|SEG_C|SEG_D|SEG_E|SEG_F|SEG_G,	// 6
	SEG_A|SEG_B|SEG_C,			// 7
	SEG_A|SEG_B|SEG_C|SEG_D|SEG_E|SEG_F|SEG_G,	// 8
	SEG_A|SEG_B|SEG_C|SEG_F|SEG_G,		// 9
};

unsigned short font_16seg[128] = {
	0x0000,	// 0x00
	0x0000,	// 0x01
	0x0000,	// 0x02
	0x0000,	// 0x03
	0x0000,	// 0x04
	0x0000,	// 0x05
	0x0000,	// 0x06
	0x0000,	// 0x07
	0x0000,	// 0x08
	0x0000,	// 0x09
	0x0000,	// 0x0a
	0x0000,	// 0x0b
	0x0000,	// 0x0c
	0x0000,	// 0x0d
	0x0000,	// 0x0e
	0x0000,	// 0x0f
	0x0000,	// 0x10
	0x0000,	// 0x11
	0x0000,	// 0x12
	0x0000,	// 0x13
	0x0000,	// 0x14
	0x0000,	// 0x15
	0x0000,	// 0x16
	0x0000,	// 0x17
	0x0000,	// 0x18
	0x0000,	// 0x19
	0x0000,	// 0x1a
	0x0000,	// 0x1b
	0x0000,	// 0x1c
	0x0000,	// 0x1d
	0x0000,	// 0x1e
	0x0000,	// 0x1f
	0x0000,	// 0x20 ' '
	0x0000,	// 0x21
	0x0000,	// 0x22
	0x0000,	// 0x23
	0x0000,	// 0x24
	0x0000,	// 0x25
	0x0000,	// 0x26
	0x0000,	// 0x27
	0x0000,	// 0x28
	0x0000,	// 0x29
	0x0000,	// 0x2a
	0x0000,	// 0x2b
	0x0000,	// 0x2c
	0x0000,	// 0x2d
	0x0000,	// 0x2e
	0x0000,	// 0x2f
	0x0000,	// 0x30
	0x0000,	// 0x31
	0x0000,	// 0x32
	0x0000,	// 0x33
	0x0000,	// 0x34
	0x0000,	// 0x35
	0x0000,	// 0x36
	0x0000,	// 0x37
	0x0000,	// 0x38
	0x0000,	// 0x39
	0x0000,	// 0x3a
	0x0000,	// 0x3b
	0x0000,	// 0x3c
	0x0000,	// 0x3d
	0x0000,	// 0x3e
	0x0000,	// 0x3f
	0x0000,	// 0x40 '@'
	SEG_A|SEG_B|SEG_C|SEG_D|SEG_G|SEG_H|SEG_U|SEG_P,	// 0x41 'A'
	SEG_A|SEG_B|SEG_E|SEG_F|SEG_G|SEG_H|SEG_N|SEG_R|SEG_U,	// 0x42 'B'
	SEG_A|SEG_B|SEG_E|SEG_F|SEG_G|SEG_H,			// 0x43 'C'
	SEG_A|SEG_B|SEG_C|SEG_D|SEG_E|SEG_F|SEG_M|SEG_S,	// 0x44 'D'
	SEG_A|SEG_B|SEG_E|SEG_F|SEG_G|SEG_U|SEG_H,		// 0x45 'E'
	SEG_A|SEG_B|SEG_G|SEG_H|SEG_U|SEG_P,			// 0x46 'F'
	SEG_A|SEG_B|SEG_D|SEG_E|SEG_F|SEG_G|SEG_H|SEG_P,	// 0x47 'G'
	SEG_C|SEG_D|SEG_G|SEG_H|SEG_U|SEG_P,			// 0x48 'H'
	SEG_A|SEG_B|SEG_E|SEG_F|SEG_M|SEG_S,			// 0x49 'I'
	SEG_C|SEG_D|SEG_E|SEG_F|SEG_G,				// 0x4a 'J'
	SEG_N|SEG_R|SEG_H|SEG_U|SEG_G,				// 0x4b 'K'
	SEG_E|SEG_F|SEG_G|SEG_H,				// 0x4c 'L'
	SEG_C|SEG_D|SEG_G|SEG_H|SEG_K|SEG_N,			// 0x4d 'M'
	SEG_C|SEG_D|SEG_G|SEG_H|SEG_K|SEG_R,			// 0x4e 'N'
	SEG_A|SEG_B|SEG_C|SEG_D|SEG_E|SEG_F|SEG_G|SEG_H,	// 0x4f 'O'
	SEG_A|SEG_B|SEG_C|SEG_G|SEG_H|SEG_U|SEG_P,		// 0x50 'P'
	SEG_A|SEG_B|SEG_C|SEG_D|SEG_E|SEG_F|SEG_G|SEG_H|SEG_R,	// 0x51 'Q'
	SEG_A|SEG_B|SEG_C|SEG_G|SEG_H|SEG_P|SEG_R|SEG_U,	// 0x52 'R'
	SEG_A|SEG_B|SEG_D|SEG_E|SEG_F|SEG_H|SEG_U|SEG_P,	// 0x53 'S'
	SEG_A|SEG_B|SEG_M|SEG_S,				// 0x54 'T'
	SEG_C|SEG_D|SEG_E|SEG_F|SEG_G|SEG_H,			// 0x55 'U'
	SEG_H|SEG_G|SEG_T|SEG_M,				// 0x56 'V'
	SEG_C|SEG_D|SEG_H|SEG_G|SEG_T|SEG_R,			// 0x57 'W'
	SEG_K|SEG_N|SEG_R|SEG_T,				// 0x58 'X'
	SEG_S|SEG_K|SEG_N,					// 0x59 'Y'
	SEG_A|SEG_B|SEG_F|SEG_E|SEG_N|SEG_T,			// 0x5a 'Z'
	0x0000,	// 0x5b
	0x0000,	// 0x5c
	0x0000,	// 0x5d
	0x0000,	// 0x5e
	0x0000,	// 0x5f
	0x0000,	// 0x60
	0x0000,	// 0x61
	0x0000,	// 0x62
	0x0000,	// 0x63
	0x0000,	// 0x64
	0x0000,	// 0x65
	0x0000,	// 0x66
	0x0000,	// 0x67
	0x0000,	// 0x68
	0x0000,	// 0x69
	0x0000,	// 0x6a
	0x0000,	// 0x6b
	0x0000,	// 0x6c
	0x0000,	// 0x6d
	0x0000,	// 0x6e
	0x0000,	// 0x6f
	0x0000,	// 0x70
	0x0000,	// 0x71
	0x0000,	// 0x72
	0x0000,	// 0x73
	0x0000,	// 0x74
	0x0000,	// 0x75
	0x0000,	// 0x76
	0x0000,	// 0x77
	0x0000,	// 0x78
	0x0000,	// 0x79
	0x0000,	// 0x7a
	0x0000,	// 0x7b
	0x0000,	// 0x7c
	0x0000,	// 0x7d
	0x0000,	// 0x7e
	0x0000,	// 0x7f
};

#define KEY_STAR	10
#define KEY_POUND	11
#define KEY_TOP_RED	12
#define KEY_TOP_YELLOW	13
#define KEY_TOP_GREEN	14
#define KEY_BOT_YELLOW	15
#define KEY_WHITE	16

int decode_key(long long keycode) {

	int key=0;

	switch(keycode) {
		case 0x000001:	key=0;	break;
		case 0x000002:	key=1;	break;
		case 0x000004:	key=2;	break;
		case 0x000008:	key=3;	break;
		case 0x000010:	key=4;	break;
		case 0x000020:	key=5;	break;
		case 0x000040:	key=6;	break;
		case 0x000080:	key=7;  break;
		case 0x000100:	key=8;	break;
		case 0x000200:	key=9;	break;
		case 0x000400:	key=KEY_STAR; break;
		case 0x000800:	key=KEY_POUND;	break;
		case 0x010000:	key=KEY_TOP_RED; break;
		case 0x020000:	key=KEY_TOP_YELLOW; break;
		case 0x040000:	key=KEY_TOP_GREEN; break;
		case 0x080000:	key=KEY_BOT_YELLOW; break;
		case 0x100000:	key=KEY_WHITE; break;
		default:	printf("Unknown keycode %llx!\n",keycode);
				key=-1;	break;
	}
	return key;
}

#define FIELD_START	-1
#define FIELD_MONTH_1	0
#define FIELD_MONTH_2	1
#define FIELD_DATE_1	2
#define FIELD_DATE_2	3
#define FIELD_YEAR_1	4
#define FIELD_YEAR_2	5
#define FIELD_YEAR_3	6
#define FIELD_YEAR_4	7
#define FIELD_HOUR_1	8
#define FIELD_HOUR_2	9
#define FIELD_MIN_1	10
#define FIELD_MIN_2	11
#define FIELD_DONE    12

static int top_red=1,top_yellow=1,top_green=1,bot_yellow=1,white=0;

struct tcircuit {
	int month;
	int day;
	int year;
	int hour;
	int minutes;
	int seconds;
};

static char month_names[12][4]={
	"JAN", "FEB", "MAR", "APR", "MAY", "JUN",
	"JUL", "AUG", "SEP", "OCT", "NOV", "DEC",

};

static void convert_time(struct tm *ltime, struct tcircuit *tctime) {
	tctime->month=ltime->tm_mon;
	tctime->day=ltime->tm_mday;
	tctime->year=ltime->tm_year+1900;
	tctime->hour=ltime->tm_hour;
	tctime->minutes=ltime->tm_min;
	tctime->seconds=ltime->tm_sec;
}

static void reset_default_times(void) {

	time_t current_time=0;

	/* Destination */
	/* Special Time */
	destination_time.year=1978;
	destination_time.month=1;
	destination_time.day=13;
	destination_time.hour=14;
	destination_time.minutes=40;
	destination_time.seconds=0;

	/* Green is Current time */
	current_time=time(NULL);
	convert_time(localtime(&current_time),&green_time);

	/* Last time departed */
	/* End of UNIX time */
	yellow_time.year=2038;
	yellow_time.month=0;
	yellow_time.day=18;
	yellow_time.hour=22;
	yellow_time.minutes=14;
	yellow_time.seconds=7;

}

static int time_travel_count=0,current_speed=0,current_power=0;

static int which_field=FIELD_MONTH_1;
static int entering_time=0;
static unsigned short enter_buffer[8];

/* Can't handle multiple keys at once? */
static void handle_keypad(long long keypad_in) {

	int keypresses=0;
	int which_key=0;
	time_t current_time;

	which_key=decode_key(keypad_in);
	printf("Keypad: %d %d\n",keypresses,which_key);

	if (which_key<0) {
		printf("Invalid key!\n");
		return;
	}

	/* reset time if * pressed */
	if (which_key==KEY_STAR) {
		current_time=time(NULL);
		convert_time(localtime(&current_time),&green_time);
		return;
	}
#if 0
	/* time travel if # pressed */
	if (which_key==KEY_POUND) {
		going_88mph=1;
		return;
	}
#endif

	if (which_key==KEY_TOP_RED) {
		top_red=!top_red;
		if (top_red==1) {
			display_red=1;
			display_green=1;
			display_yellow=1;
		} else {
			/* Turn off display */
			display_red=0;
			display_green=0;
			display_yellow=0;
		}

		return;
	}

	if (which_key==KEY_TOP_YELLOW) {
		/* Reset to default times */

		reset_default_times();

		display_red=1;
		display_yellow=1;
		display_green=1;

		//top_yellow=!top_yellow;
		return;
	}

	if (which_key==KEY_TOP_GREEN) {

		/* Put last time as Destination Time */
		display_red=1;
		display_yellow=1;

		memcpy(&destination_time,&yellow_time,sizeof(struct tcircuit));

		top_green=!top_green;
		return;
	}

	if (which_key==KEY_BOT_YELLOW) {
		/* Cancel */

		display_red=0xff;
		entering_time=0;
		which_field=FIELD_MONTH_1;
		bot_yellow=1;
		//bot_yellow=!bot_yellow;
		return;
	}

	if (which_key==KEY_WHITE) {
		//white=!white;
		memcpy(&destination_time,&new_time,sizeof(struct tcircuit));
		display_red=1;
		entering_time=0;
		which_field=FIELD_MONTH_1;
		bot_yellow=1;
		return;
	}

	entering_time=1;
	bot_yellow=0;
	memset(enter_buffer,0,sizeof(enter_buffer));

	switch(which_field) {
		case FIELD_MONTH_1:
			display_red=0;	/* Turn off display */

			if (which_key>1) {
				new_time.month=which_key;
				printf("Month now %d\n",new_time.month);

				which_field=FIELD_DATE_1;
				enter_buffer[0]=SEG_D;

			}
			else {
				new_time.month=which_key*10;
				new_time.month--;

				which_field=FIELD_MONTH_2;
				enter_buffer[5]=SEG_E | SEG_F;
				enter_buffer[6]=SEG_E | SEG_F;
				enter_buffer[7]=SEG_E | SEG_F;

			}


			break;
		case FIELD_MONTH_2:
			new_time.month+=which_key;
			printf("Month now %d\n",new_time.month);

			which_field=FIELD_DATE_1;
			enter_buffer[0]=SEG_D;

			break;
		case FIELD_DATE_1:
			new_time.day=which_key*10;
			which_field=FIELD_DATE_2;
			enter_buffer[1]=SEG_D;

			break;
		case FIELD_DATE_2:
			new_time.day+=which_key;
			printf("Day now %d\n",new_time.day);

			which_field=FIELD_YEAR_1;
			enter_buffer[2]=SEG_D;


			break;
		case FIELD_YEAR_1:
			new_time.year=which_key*1000;

			which_field=FIELD_YEAR_2;
			enter_buffer[3]=SEG_D;

			break;
		case FIELD_YEAR_2:
			new_time.year+=which_key*100;

			which_field=FIELD_YEAR_3;
			enter_buffer[4]=SEG_D;

			break;
		case FIELD_YEAR_3:
			new_time.year+=which_key*10;

			which_field=FIELD_YEAR_4;
			enter_buffer[4]=SEG_D<<8;

			break;
		case FIELD_YEAR_4:
			new_time.year+=which_key;
			printf("Year now %d\n",new_time.year);

			which_field=FIELD_HOUR_1;
			enter_buffer[3]=SEG_D<<8;

			break;
		case FIELD_HOUR_1:
			new_time.hour=10*which_key;

			which_field=FIELD_HOUR_2;
			enter_buffer[2]=SEG_D<<8;


			break;
		case FIELD_HOUR_2:
			new_time.hour+=which_key;
			printf("Hour now %d\n",new_time.hour);

			which_field=FIELD_MIN_1;
			enter_buffer[1]=SEG_D<<8;


			break;
		case FIELD_MIN_1:
			new_time.minutes=10*which_key;

			which_field=FIELD_MIN_2;
			enter_buffer[0]=SEG_D<<8;

			break;
		case FIELD_MIN_2:
			new_time.minutes+=which_key;
			printf("Minutes now %d\n",new_time.minutes);
			which_field=FIELD_MONTH_1;

			enter_buffer[0]=(SEG_D<<8)|SEG_D;
			enter_buffer[1]=(SEG_D<<8)|SEG_D;
			enter_buffer[2]=(SEG_D<<8)|SEG_D;
			enter_buffer[3]=(SEG_D<<8)|SEG_D;
			enter_buffer[4]=(SEG_D<<8)|SEG_D;
			enter_buffer[5]=SEG_E | SEG_F;
			enter_buffer[6]=SEG_E | SEG_F;
			enter_buffer[7]=SEG_E | SEG_F;

			break;
	}

	return;
}


static void handle_flux_buttons(int i2c_fd,long long keypad_in) {

	int keypresses=0;
	int which_key=0;

	which_key=decode_key(keypad_in);
	printf("Flux: %d %d\n",keypresses,which_key);

	if (which_key==0) {
		printf("Activating power\n");
		current_power=1;
	}


//	if (which_key==1) {
//		printf("Accelerating\n");
//		current_speed++;
//		if (current_speed>=99) current_speed=99;
//	}


	if (which_key==2) {
		printf("Brake\n");
		current_speed=0;
	}

	return;
}


static void print_time(struct tcircuit *tctime, int blink, int blank) {


	if (blank) {
		printf("\n");
		return;
	}

	printf("%s ",month_names[tctime->month]);
	printf("%02d %04d",tctime->day,tctime->year);
	if (tctime->hour>11) {
		printf("\'");
	}
	else {
		printf(".");
	}
	printf("%02d",tctime->hour);
	if (blink) {
		printf(":");
	}
	else {
		printf(" ");
	}
	printf("%02d\n",tctime->minutes);
}


/* FIXME: use BCD? */
/* That will avoid lots of division */
static void convert_for_display(unsigned short *buffer,
				struct tcircuit *tctime, int blink,
				int display_count) {

	int day_tens,day_ones;
	int year,year_thousands, year_hundreds, year_tens, year_ones;
	int hour_tens, hour_ones;
	int minute_tens,minute_ones;

	/* Clear buffer */
	memset(buffer,0,8*sizeof(short));

	if (display_count==0) {
		return;
	}

	if (display_count>16) {
		/* Month */
		buffer[5]=font_16seg[(int)month_names[tctime->month][0]];
		buffer[6]=font_16seg[(int)month_names[tctime->month][1]];
		buffer[7]=font_16seg[(int)month_names[tctime->month][2]];
	}

	if (display_count>12) {
		/* Date */
		day_tens=tctime->day/10;
		day_ones=tctime->day%10;
		if (day_tens!=0) {
			buffer[0]=font_7seg[day_tens];
		}
		buffer[1]=font_7seg[day_ones];
	}

	if (display_count>8) {
		/* Year */
		year=tctime->year;
		year_thousands=year/1000;
		year=year-(year_thousands*1000);

		year_hundreds=year/100;
		year=year-(year_hundreds*100);

		year_tens=year/10;
		year_ones=year%10;

		if (year_thousands>9) {
			year_thousands=0;
			year_hundreds=0;
			year_tens=0;
			year_ones=0;
		}

		buffer[2]=font_7seg[year_thousands];
		buffer[3]=font_7seg[year_hundreds];
		buffer[4]=font_7seg[year_tens];
		buffer[4]|=(font_7seg[year_ones])<<8;
	}

	/* hours */
	if (display_count>4) {
		hour_tens=tctime->hour/10;
		hour_ones=tctime->hour%10;

		buffer[3]|=(font_7seg[hour_tens])<<8;
		buffer[2]|=(font_7seg[hour_ones])<<8;

		/* AM/PM */
		if (tctime->hour > 11) {
			buffer[3]|=0x8000;
		}
		else {
			buffer[2]|=0x8000;
		}
	}


	if (display_count>0) {
		/* minutes */
		minute_tens=tctime->minutes/10;
		minute_ones=tctime->minutes%10;
		buffer[1]|=(font_7seg[minute_tens])<<8;
		buffer[0]|=(font_7seg[minute_ones])<<8;

		/* Blink */
		if (blink) {
			buffer[0]|=0x8000;
			buffer[1]|=0x8000;
		}
	}

}

#define MONTH_JAN	0
#define MONTH_FEB	1
#define MONTH_MAR	2
#define MONTH_APR	3
#define MONTH_MAY	4
#define MONTH_JUN	5
#define MONTH_JUL	6
#define MONTH_AUG	7
#define MONTH_SEP	8
#define MONTH_OCT	9
#define MONTH_NOV	10
#define MONTH_DEC	11


static void time_travel(struct tcircuit *dest,
			struct tcircuit *curr,
			struct tcircuit *last) {


	/* Move Current to Last Departed */
	memcpy(last,curr,sizeof(struct tcircuit));

	/* Move Destination to Current */
	memcpy(curr,dest,sizeof(struct tcircuit));


}

static void adjust_current_time(struct tcircuit *tctime, int seconds) {

	tctime->seconds+=seconds;

	if (tctime->seconds>=60) {
		tctime->minutes+=(tctime->seconds/60);
		tctime->seconds%=60;
	}

	if (tctime->minutes>=60) {
		tctime->hour+=(tctime->minutes/60);
		tctime->minutes%=60;
	}

	if (tctime->hour>23) {
		tctime->day+=(tctime->hour/24);
		tctime->hour%=24;
	}

	/* OK this gets harder when a month overflows */

	switch (tctime->month) {

		case MONTH_JAN:
		case MONTH_MAR:
		case MONTH_MAY:
		case MONTH_JUL:
		case MONTH_AUG:
		case MONTH_OCT:
		case MONTH_DEC:
			if (tctime->day>31) {
				tctime->month+=1;
				tctime->day=1;
			}
			break;

		case MONTH_SEP:
		case MONTH_APR:
		case MONTH_JUN:
		case MONTH_NOV:
			if (tctime->day>30) {
				tctime->month+=1;
				tctime->day=1;
			}
			break;

		case MONTH_FEB:
			/* Ugh leap year */
			if (((tctime->year%4==0) && (tctime->year%100!=0)) ||
				(tctime->year%400==0)) {

				if (tctime->day>29) {
					tctime->month+=1;
					tctime->day=1;
				}
			} else {
				if (tctime->day>28) {
					tctime->month+=1;
					tctime->day=1;
				}

			}
			break;

	}

	if (tctime->month>MONTH_DEC) {
		tctime->year+=1;
		tctime->month=MONTH_JAN;
	}

	/* How to handle overflow? */
	if (tctime->year>9999) {
		tctime->year=0;
	}

}

int main(int argc, char **argv) {

 	int result,blink=0,count=0;
	int display_missing=0;
	int red_missing=0,green_missing=0,yellow_missing=0,flux_missing=0;
	int d2a_missing=0;
	int flux_count=0;

 	unsigned short red_buffer[8];
 	unsigned short green_buffer[8];
 	unsigned short yellow_buffer[8];
	unsigned short flux_buffer[8];

	long long old_keypad=0,old_fluxkey=0;
	long long keypad_result=0,keypad_change;
	long long fluxkey_result=0,fluxkey_change;
	int i2c_fd;
	int old_dac=0;

	time_t current_time=0,previous_time=0;

	/* open i2c bus */
	i2c_fd=init_i2c("/dev/i2c-1");
	if (i2c_fd < 0) {
		fprintf(stderr,"Error opening device!\n");
		display_missing=1;
	}

	if (display_missing) {

	}
	else {

		/* Init Red Display (top) */
		if (init_display(i2c_fd,HT16K33_ADDRESS4,13)) {
			fprintf(stderr,"Error opening red display\n");
			red_missing=1;
		}

		/* Init Green Display (middle) */
		if (init_display(i2c_fd,HT16K33_ADDRESS5,13)) {
			fprintf(stderr,"Error opening green display\n");
			green_missing=1;
		}

		/* Init Yellow Display (bottom) */
		if (init_display(i2c_fd,HT16K33_ADDRESS6,13)) {
			fprintf(stderr,"Error opening yellow display\n");
			yellow_missing=1;
		}

		/* Init Flux Capacitor */
		if (init_display(i2c_fd,HT16K33_ADDRESS0,13)) {
			fprintf(stderr,"Error opening flux display\n");
			flux_missing=1;
		}

		/* Init d2a Converter */
		if (init_dac(i2c_fd,MCP4725_ADDRESS1,0)) {
			fprintf(stderr,"Error opening DAC\n");
			d2a_missing=1;
		}

	}

/*
	red_buffer[0]=font_16seg['A'];
	red_buffer[1]=font_16seg['U'];
	red_buffer[2]=font_16seg['G'];
	red_buffer[3]=font_7seg[2]|(font_7seg[8]<<8);
	red_buffer[4]=font_7seg[1]|(font_7seg[5]<<8);
	red_buffer[5]=font_7seg[2]|(font_7seg[4]<<8);
	red_buffer[6]=font_7seg[0]|(font_7seg[1]<<8);
	red_buffer[7]=font_7seg[1]|(font_7seg[3]<<8);
*/
/*

	red_buffer[7]=0xffff;
	red_buffer[6]=0xffff;
	red_buffer[5]=0xffff;
	red_buffer[4]=0xffff;
	red_buffer[3]=0xffff;
	red_buffer[2]=0xffff;
	red_buffer[1]=0xffff;
	red_buffer[0]=0xffff;
        update_display(i2c_fd,HT16K33_ADDRESS1,red_buffer);
	return 0;
*/

#if 0
	flux_buffer[7]=0xffff;
	flux_buffer[6]=0xffff;
	flux_buffer[5]=0xffff;
	flux_buffer[4]=0xffff;
	flux_buffer[3]=font_16seg['W'];	/* W */
	flux_buffer[2]=font_16seg['G'];	/* G */
	flux_buffer[1]= font_7seg[2] | font_7seg[1]<<8;
	flux_buffer[0]= ( (font_7seg[1] | SEG7_DP)<<8) | 0xff;
#endif
	memset(flux_buffer,0,sizeof(flux_buffer));

	reset_default_times();
	previous_time=time(NULL);

	/* Time Loop */
	while(1) {

		current_time=time(NULL);

		/* Update displays if a second or more has passed */
		if (current_time!=previous_time) {

			adjust_current_time(&green_time,current_time-previous_time);

			previous_time=current_time;
		}

		if (display_missing) {

			/* clear screen */
			printf("\x1b[2J\x1b[0;0H");

			/* red */
			printf("\x1b[31;1m");
			print_time(&destination_time,blink,display_red);

			/* green */
			printf("\x1b[32;1m");
			print_time(&green_time,blink,display_green);

			/* yellow */
			printf("\x1b[33;1m");
			print_time(&yellow_time,blink,display_yellow);
		}

		else {
			if (!entering_time) {
				convert_for_display(red_buffer,&destination_time,blink,display_red);
				if (!red_missing) {
					update_display(i2c_fd,HT16K33_ADDRESS4,red_buffer);

				}
			}
			else {
				if (!red_missing) {
					update_display(i2c_fd,HT16K33_ADDRESS4,enter_buffer);
				}
			}



			convert_for_display(green_buffer,&green_time,blink,display_green);

			convert_for_display(yellow_buffer,&yellow_time,blink,display_yellow);

			/* buttons */
			if (top_red) yellow_buffer[1]|=0x0080;
			if (top_yellow) yellow_buffer[2]|=0x0080;
			if (top_green) yellow_buffer[3]|=0x0080;
			if (bot_yellow) yellow_buffer[4]|=0x0080;
			if (white) yellow_buffer[4]|=0x8000;


			/* Read main keypad */
			if (!yellow_missing) {
				keypad_result=read_keypad(i2c_fd,HT16K33_ADDRESS6);
				/* Detect key release */
				keypad_change=old_keypad&~keypad_result;
				if (keypad_change) {
					printf("new %lld old %lld\n",keypad_result,old_keypad);
					handle_keypad(keypad_change);
				}
				old_keypad=keypad_result;

			}

			/* Read flux keys */
			if (!flux_missing) {
				fluxkey_result=read_keypad(i2c_fd,HT16K33_ADDRESS0);

				/* Handle accelerator */
				/* No debounce  */
				if (fluxkey_result&0x2) {
					current_speed++;
					if (current_speed>=99) {
						current_speed=99;
					}
				}

				fluxkey_change=old_fluxkey&~fluxkey_result;
				if (fluxkey_change) {
					printf("Old %lld New %lld\n",
						fluxkey_result,old_fluxkey);
					handle_flux_buttons(i2c_fd,fluxkey_change);
				}
				old_fluxkey=fluxkey_result;
			}

			/* Update displays */

			if (!green_missing) {
				update_display(i2c_fd,HT16K33_ADDRESS5,green_buffer);
			}
			if (!yellow_missing) {
				update_display(i2c_fd,HT16K33_ADDRESS6,yellow_buffer);
			}
			if (!flux_missing) {
				update_display(i2c_fd,HT16K33_ADDRESS0,flux_buffer);
			}
		}

		/* Faster than 20000 and the keypad can't keep up */
		usleep(25000);
		count++;
		if (count==20) {
			blink=1;
		}
		if (count==40) {
			count=0;
			blink=0;
		}

		/* Time Travel */
		if (!time_travel_count) {
			if ((current_speed>=88) && (current_power>=121)) {
				time_travel_count=1;
			}
		}

		if (time_travel_count) {

			current_speed=88;

			if (time_travel_count==1) {
				display_red=0;
				display_green=0;
				display_yellow=0;
			}


			if (time_travel_count==100) {
				time_travel(&destination_time,&green_time,&yellow_time);
				time_travel_count=0;
				display_red=1;
				display_green=1;
				display_yellow=1;
				current_speed=0;
				current_power=0;
			} else {
				time_travel_count++;
			}

		}

		if (display_red==1) {
			system("aplay ./sounds/time_circuit.wav");
		}

		if ((display_red>0) && (display_red<60)) display_red++;
		if ((display_green>0) && (display_green<60)) display_green++;
		if ((display_yellow>0) && (display_yellow<60)) display_yellow++;


		/* Update flux display */


		/* Digit 1 of power plus Digit 2 of speed */
		flux_buffer[0]= ( (font_7seg[current_power/100] | SEG7_DP)<<8) |
				font_7seg[current_speed%10];

		/* Digit 2 and 3 of power */
		flux_buffer[1]= font_7seg[(current_power/10)%10] |
			(font_7seg[current_power%10]&0xff)<<8;

		/* Alphanum of power */
		flux_buffer[2]=font_16seg['G'];	/* G */
		flux_buffer[3]=font_16seg['W'];	/* W */

		/* Digit 1 of speed */
		flux_buffer[4]=font_7seg[current_speed/10];


		/* Meters */
		flux_buffer[6]=1 << (current_power/8);
		flux_buffer[7]= (1<<(current_power/8))-1;

		/* DAC */
		if (!d2a_missing) {
			if ((current_power*33)!=old_dac) {
				set_dac(i2c_fd,MCP4725_ADDRESS1,0,current_power*33);
				//printf("Setting dac: %d\n",current_power*33);
				old_dac=current_power*33;
			}
		}

		if ((current_power>0) && (current_power<121)) {
			current_power+=1;
		}

		/* Make flux blinking proportional to velocity */
		flux_count+=5+(current_speed/2);
		if (flux_count>=500) flux_count%=500;

		switch(flux_count/100) {
			case 0:	flux_buffer[5]=0x1 | 0x20 | 0x400;
				break;
			case 1:	flux_buffer[5]=0x2 | 0x40 | 0x800;
				break;
			case 2: flux_buffer[5]=0x4 | 0x80 | 0x1000;
				break;
			case 3: flux_buffer[5]=0x8 | 0x100 | 0x2000;
				break;
			case 4: flux_buffer[5]=0x10 | 0x200 | 0x4000;
				break;
			default:
				break;
		}


	}

	return result;
}

