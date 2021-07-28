#include <common.h>
#include <command.h>
#include <errno.h>
#include <dm.h>
#include <asm/gpio.h>

static int do_ledtest(cmd_tbl_t *cmdtp, int flag, int argc, char *const argv[])
{
	unsigned int gpio;
	const char *led_action, *led_name;
	int ret;

	if (argc < 3)
		return -1;

	led_action = argv[1];
	led_name = argv[2];

#if defined(CONFIG_DM_GPIO)
	if (!strncmp(led_name, "D2", sizeof("D2")))
	{
		ret = gpio_lookup_name("C9", NULL, NULL, &gpio);
	}
	else if (!strncmp(led_name, "D3", sizeof("D3")))
	{
		ret = gpio_lookup_name("C11", NULL, NULL, &gpio);
	}
	else if (!strncmp(led_name, "D4", sizeof("D4")))
	{
		ret = gpio_lookup_name("C12", NULL, NULL, &gpio);
	}
	else
	{
		ret = 1;
		gpio = -1;
	}
	if (ret)
	{
		printf("GPIO: '%s' not found\n", led_name);
		return -1;
	}
#else
	// convert input LED name on board to its number in hardware-level
	if (!strncmp(led_name, "D2", sizeof("D2")))
	{
		gpio = name_to_gpio("C9");
	}
	else if (!strncmp(led_name, "D3", sizeof("D3")))
	{
		gpio = name_to_gpio("C11");
	}
	else if (!strncmp(led_name, "D4", sizeof("D4")))
	{
		gpio = name_to_gpio("C12");
	}
	else
	{
		gpio = -1;
	}
	if (gpio < 0)
	{
		printf("GPIO: '%s' not found\n", led_name);
		return -1;
	}
#endif

	ret = gpio_request(gpio, "led_control");
	if (ret && ret != -EBUSY)
	{
		printf("gpio: requesting pin %u failed\n", gpio);
		return -1;
	}

	// set the action of LED command
	if (!strncmp(led_action, "on", sizeof("on")))
	{
		gpio_direction_output(gpio, 0);
	}
	else if (!strncmp(led_action, "off", sizeof("off")))
	{
		gpio_direction_output(gpio, 1);
	}
	else if (!strncmp(led_action, "toggle", sizeof("toggle")))
	{
		gpio_direction_output(gpio, !gpio_get_value(gpio));
	}

	gpio_free(gpio);
	return 0;
}

U_BOOT_CMD(ledtest, 3, 0, do_ledtest,
		   "",
		   "");
