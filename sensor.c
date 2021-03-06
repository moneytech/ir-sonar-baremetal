#include "sensor.h"
#include "gpio.h"

#define RPI_INT_ENABLE_1             0x2000B210
#define RPI_INT_ENABLE_2             0x2000B214
#define RPI_INT_DISABLE_1            0x2000B21C
#define RPI_INT_DISABLE_2            0x2000B220

extern int ir_sensors[3];
extern int ir_sensor_reads[3];
extern int detection_flag;


void sensor_init(void) {

    for (int i = 0; i < 8; i++) {
    	int sensor = ir_sensors[i];
    	gpio_set_input(sensor);
        gpio_set_pullup(sensor);
        gpio_detect_rising_edge(sensor);
    }

    // Make sure everything is cleared
    PUT32(RPI_INT_DISABLE_1, 0xFFFFFFFF);
    PUT32(RPI_INT_DISABLE_2, 0xFFFFFFFF);

    // Enable GPIO interrupts
    PUT32(RPI_INT_ENABLE_1, 0x00100000);
    PUT32(RPI_INT_ENABLE_2, 0x00100000);

    // Enable interrupts
    system_enable_interrupts();
}

void sensor_int_handler(void) {
	system_disable_interrupts();

	//check and clear events
	for (int i = 0; i < 8; i++) {
		int sensor = ir_sensors[i];
		int s = gpio_check_and_clear_event(sensor);
        if (s == 1) {
            detection_flag = 1;
            printf("Event triggered on sensor %d\n", i);
        }
	}
	//check which interrupt was triggered
    for (int i = 0; i < 8; i++) {
		int sensor = ir_sensors[i];
		if (gpio_read(sensor) == 1) {
            printf("Detection on sensor %d\n", i);
			ir_sensor_reads[i] = 1;
		}
	}
    system_enable_interrupts();
}