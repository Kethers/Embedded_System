
CC := ../../gw3399-linux/prebuilts/gcc/linux-x86/aarch64/gcc-linaro-6.3.1-2017.05-x86_64_aarch64-linux-gnu/bin/aarch64-linux-gnu-gcc

CFLAGS := -O2  -I. #-std=c99
#CFLAGS += -D_POSIX_C_SOURCE
LDFLAGS := -Llib
LDFLAGS += -lm -lpthread -lasound

OBJ_DEMO = main.o \
	demo.o \
	utils.o	\
	font.o \
	fft.o  spectrometer.o\
	adcTest.o adc.o \
	key.o keyTest.o \
	led8x8.o led8x8Test.o \
	leds.o ledsTest.o	\
	oled.o oledTest.o \
	pwmLed.o pwmLedTest.o \

OBJ_DISPLAY = display.o \
				led8x8.o \
				oled.o \
				font.o \

OBJ_UDPFW = udpfw.o	

OUT_ELF = demo udpfw display

all: $(OUT_ELF)
 
udpfw: $(OBJ_UDPFW)
	$(CC)  $(LDFLAGS)   -o udpfw $^
	
demo: $(OBJ_DEMO)
	$(CC)  $(LDFLAGS)   -o demo $^

display: $(OBJ_DISPLAY)
	$(CC)  $(LDFLAGS)   -o display $^

clean:
	@rm -vf  $(OUT_ELF) *.o *~
