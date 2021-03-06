#define EFFECT_RANDOM		0
#define EFFECT_BARGRAPH		1
#define EFFECT_RAINBOW		2
#define EFFECT_STARS		3
#define EFFECT_TWO_COLOR_SCROLL	4
#define EFFECT_RED_GREEN	5
#define EFFECT_BLUE_YELLOW	6
#define EFFECT_SCANNER		7
#define EFFECT_SCANNER_BLINKY	8
#define EFFECT_SCANNER_DUAL	9
#define EFFECT_SCANNER_RANDOM	10
#define EFFECT_DISABLE		11
#define EFFECT_FISH		12
#define EFFECT_PULSAR		13
#define EFFECT_NOISE		14
#define EFFECT_GRADIENT		15
#define EFFECT_FALLING		16
#define EFFECT_BARGRAPH_MANUAL	17

int bargraph(int spi_fd, char *left_color, char *right_color);
int bargraph_manual(int spi_fd, char *left_color, char *right_color);
int disable(int spi_fd);
int falling(int spi_fd, char *color1, char *color2);
int fish(int spi_fd, char *nfish);
int gradient(int spi_fd, char *color);
int noise(int spi_fd);
int pulsar(int spi_fd, char *color);
int rainbow(int spi_fd);
int stars(int spi_fd, char *speed_s, char *dimming_s);
int two_color_scroll(int spi_fd, char *col1, char *col2, char *dir);
int scanner(int spid_fd, char *color);
int scanner_blinky(int spi_fd);
int scanner_dual(int spi_fd, char *color1, char *color2);
int scanner_random(int spi_fd);

