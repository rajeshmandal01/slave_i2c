#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/i2c.h"

#define I2C_SLAVE_SCL_IO 47       // I2C slave clock pin
#define I2C_SLAVE_SDA_IO 48       // I2C slave data pin
#define I2C_SLAVE_NUM I2C_NUM_0   // I2C port number
#define I2C_SLAVE_ADDR 0x28       // I2C address of the slave device

// GPIO pins for LEDs on ESP32-S3
//int led_pins[24] = {GPIO_NUM_4, GPIO_NUM_5, GPIO_NUM_6, GPIO_NUM_7, GPIO_NUM_15, GPIO_NUM_16, GPIO_NUM_17, GPIO_NUM_18, GPIO_NUM_8, GPIO_NUM_3, GPIO_NUM_46, GPIO_NUM_9, GPIO_NUM_10, GPIO_NUM_11, GPIO_NUM_12, GPIO_NUM_13, GPIO_NUM_14, GPIO_NUM_1, GPIO_NUM_2,  GPIO_NUM_37, GPIO_NUM_36, GPIO_NUM_35, GPIO_NUM_0,GPIO_NUM_45};
int led_pins[24] = {4, 5, 6,7, 15, 16, 17, 18, 8, 3, 46, 9, 10, 11, 12, 13, 14, 1, 2, 37, 36,35, 0,45};

// Function to initialize I2C slave
void init_i2c_slave() {
    i2c_config_t config = {
        .mode = I2C_MODE_SLAVE,
        .sda_io_num = I2C_SLAVE_SDA_IO,
        .scl_io_num = I2C_SLAVE_SCL_IO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .slave = {.addr_10bit_en = 0, .slave_addr = I2C_SLAVE_ADDR},
    };
    i2c_param_config(I2C_SLAVE_NUM, &config);
    i2c_driver_install(I2C_SLAVE_NUM, config.mode, 1024, 1024, 0);
}

// Function to initialize LED pins
void init_leds() {
    for (int i = 0; i < 24; i++) {
        gpio_set_direction(led_pins[i], GPIO_MODE_OUTPUT);

        // Set default states:
        // Active High LEDs (2,8,14 and 20) are off at level 0
        // Active Low LEDs (others) are off at level 1
        if (i == 1 || i == 7 || i == 13 || i == 19) {  // LEDs 6, 12, 18, 24
            gpio_set_level(led_pins[i], 0);  // Turn off active high LEDs
        } else {
            gpio_set_level(led_pins[i], 1);  // Turn off active low LEDs
        }
    }
}

// Function to turn on the correct LED based on index
void turn_on_led(int led_index) {
	if(led_index<=23) {
	    for (int i = 0; i < 24; i++) {
	        if (i == led_index) {
	               // Active High LEDs (2,8,14 and 20) are off at level 0): ON when set to 1
	            // Active low LEDs (others): ON when set to 0
	            if (i == 1 || i == 7 || i == 13 || i == 19) {
	                gpio_set_level(led_pins[i], 1);  // Turn on active high LED
	            } else {
	                gpio_set_level(led_pins[i], 0);  // Turn on active low LED
	            }
	        } else {
	            // Turn off LEDs:
	            // Active high LEDs are OFF when set to 0
	            // Active low LEDs are OFF when set to 1
	            if (i == 1 || i == 7 || i == 13 || i == 19) {
	                gpio_set_level(led_pins[i], 0);  // Turn off active high LED
	            } else {
	                gpio_set_level(led_pins[i], 1);  // Turn off active low LED
	            }
	        }
	    }
    }
    else {
		
		 for (int i = 0; i < 24; i++){
	            // Turn off LEDs:
	            // Active high LEDs are OFF when set to 0
	            // Active low LEDs are OFF when set to 1
	            if (i == 1 || i == 7 || i == 13 || i == 19) {
	                gpio_set_level(led_pins[i], 0);  // Turn off active high LED
	            } else {
	                gpio_set_level(led_pins[i], 1);  // Turn off active low LED
	            }
	            }
	            
	        }
}


// Function to send acknowledgment to master
void send_ack_to_master() {
    uint8_t ack = 0xAA;  // Acknowledgment byte
    i2c_slave_write_buffer(I2C_SLAVE_NUM, &ack, 1, pdMS_TO_TICKS(1000));
}

// Main function
void app_main() {
    init_i2c_slave();
    init_leds();

    uint8_t data;
    while (1) {
        int len = i2c_slave_read_buffer(I2C_SLAVE_NUM, &data, 1, pdMS_TO_TICKS(1000));

        // Check if data was received and is in the range 1 to 24
        if (len > 0 && data >= 1 && data <= 25) {
            turn_on_led(data - 1);   // Turn on the LED corresponding to the received value
            send_ack_to_master();    // Send acknowledgment to the master
        }
    }
}
