#include <unistd.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "led8x8.h"
#include "oled.h"
#include "font.h"

static unsigned short faces[][8] = {
	{0x18, 0x18, 0x00, 0x18, 0x04, 0x04, 0x24, 0x18},	// question mark
	{0x00, 0x42, 0x3c, 0x00, 0x00, 0xa5, 0x42, 0x00},	// Sad face
	{0x00, 0x3c, 0x42, 0x00, 0x00, 0xa5, 0x42, 0x00},	// Happy face
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}
};

void led8x8Face(int i)
{
	if (i<sizeof faces / sizeof faces[0]) {
		//led8x8Draw((char*)faces[i]);
		for (int j=0; j<8; j++) {
			for (int k=0; k<8; k++) {
				led8x8Point(k,j, faces[i][j]&(1<<k));
			}
		}
	}
}

static void showFont16Center(char *str)
{
	int len = strlen(str)*8;
	int offx = 0;
	if (len < 96) offx = (96-len)/2;
	fontShow16(offx, 8, str, oledPoint);
}

int main(int argc, char **argv)
{
	// TODO:
	char buf[32];


	led8x8Init();
	oledInit();
	led8x8Clear();
	oledClear();

	if (argc != 2)
	{
		printf("error: parameter count no match\n");
		return -1;
	}
	if (!strcmp(argv[1], "0"))	// question mark
	{
		led8x8Face(0);
		led8x8Flush();
		oledClear();
		snprintf(buf, sizeof buf, "unknown");
		showFont16Center(buf);
		oledFlush();
	}
	else if (!strcmp(argv[1], "1")) // sad face
	{
		led8x8Face(1);
		led8x8Flush();
		oledClear();
		snprintf(buf, sizeof buf, "no mask");
		showFont16Center(buf);
		oledFlush();
	}
	else if (!strcmp(argv[1], "2")) // happy face
	{
		led8x8Face(2);
		led8x8Flush();
		oledClear();
		snprintf(buf, sizeof buf, "has mask");
		showFont16Center(buf);
		oledFlush();
	}
	else
	{
		printf("error: invalid parameter input, only 0 or 1 or 2 is allowed\n");
		printf("0 for question mark, 1 for sad face, 2 for happy face\n");
		printf("Example: command below will show a sad face\n");
		printf("display 1\n\n");
	}

	return 0;
}

