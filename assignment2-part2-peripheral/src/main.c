#include <stdbool.h>
#include <zephyr/types.h>
#include <drivers/sensor.h>
#include <stddef.h>
#include <string.h>
#include <errno.h>
#include <sys/printk.h>
#include <sys/byteorder.h>
#include <kernel.h>

#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/conn.h>
#include <bluetooth/uuid.h>
#include <bluetooth/gatt.h>

// from button project
#include <device.h>
#include <drivers/gpio.h>


#define ASSIGNMENT2_SERVICE_UUID BT_UUID_128_ENCODE(0xBDFC9792, 0x8234, 0x405E, 0xAE02, 0x35EF3274B299)
// ------ FROM BUTTON PROJECT ------
#define SW0_NODE	DT_ALIAS(sw0)
#define SW1_NODE	DT_ALIAS(sw1)
#define SW2_NODE	DT_ALIAS(sw2)
#define SW3_NODE	DT_ALIAS(sw3)

static const struct gpio_dt_spec button1 = GPIO_DT_SPEC_GET_OR(SW0_NODE, gpios, {0});
static const struct gpio_dt_spec button2 = GPIO_DT_SPEC_GET_OR(SW1_NODE, gpios, {0});
static const struct gpio_dt_spec button3 = GPIO_DT_SPEC_GET_OR(SW2_NODE, gpios, {0});
static const struct gpio_dt_spec button4 = GPIO_DT_SPEC_GET_OR(SW3_NODE, gpios, {0});

static struct gpio_callback button1_cb_data;
static struct gpio_callback button2_cb_data;
static struct gpio_callback button3_cb_data;
static struct gpio_callback button4_cb_data;

// We added these; ints because gpio_pin_get_dt returns int
uint32_t button1val;
uint32_t button2val;
uint32_t button3val;
uint32_t button4val;
// -------------

void button1_pressed(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
	button1val = (uint32_t) gpio_pin_get_dt(&button1);
}

void button2_pressed(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
	button2val = (uint32_t) gpio_pin_get_dt(&button2);
}

void button3_pressed(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
	button3val = (uint32_t) gpio_pin_get_dt(&button3);
}

void button4_pressed(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
	button4val = (uint32_t) gpio_pin_get_dt(&button4);
}


void init_button(const struct gpio_dt_spec* button,
                 struct gpio_callback* button_cb_data,
				 gpio_callback_handler_t cb) {
	int ret;

	if (!device_is_ready(button->port)) {
		printk("Error: button device %s is not ready\n",
		       button->port->name);
		return;
	}

	ret = gpio_pin_configure_dt(button, GPIO_INPUT);
	if (ret != 0) {
		printk("Error %d: failed to configure %s pin %d\n",
		       ret, button->port->name, button->pin);
		return;
	}

	ret = gpio_pin_interrupt_configure_dt(button, GPIO_INT_EDGE_BOTH);
	if (ret != 0) {
		printk("Error %d: failed to configure interrupt on %s pin %d\n",
			ret, button->port->name, button->pin);
		return;
	}

	gpio_init_callback(button_cb_data, cb, BIT(button->pin));
	gpio_add_callback(button->port, button_cb_data);
	printk("Set up button at %s pin %d\n", button->port->name, button->pin);
}
// -------------

static ssize_t characteristic_read(struct bt_conn *conn, const struct bt_gatt_attr *attr, void *buf, uint16_t len, uint16_t offset);

// Set up the advertisement data.
#define DEVICE_NAME "Peripheral_14"
#define DEVICE_NAME_LEN (sizeof(DEVICE_NAME) - 1)

static const struct bt_data ad[] = {
	BT_DATA(BT_DATA_NAME_COMPLETE, DEVICE_NAME, DEVICE_NAME_LEN),
	BT_DATA_BYTES(BT_DATA_UUID128_ALL, ASSIGNMENT2_SERVICE_UUID)
};

// Setup the the service and characteristics.
BT_GATT_SERVICE_DEFINE(assignment2_service,
	BT_GATT_PRIMARY_SERVICE(
		BT_UUID_DECLARE_128(ASSIGNMENT2_SERVICE_UUID)
	),
	BT_GATT_CHARACTERISTIC(BT_UUID_DECLARE_16(0x0001), BT_GATT_CHRC_READ,
			       BT_GATT_PERM_READ, characteristic_read, NULL, &button1val),
	BT_GATT_CHARACTERISTIC(BT_UUID_DECLARE_16(0x0002), BT_GATT_CHRC_READ,
			       BT_GATT_PERM_READ, characteristic_read, NULL, &button2val),
	BT_GATT_CHARACTERISTIC(BT_UUID_DECLARE_16(0x0003), BT_GATT_CHRC_READ,
			       BT_GATT_PERM_READ, characteristic_read, NULL, &button3val),
	BT_GATT_CHARACTERISTIC(BT_UUID_DECLARE_16(0x0004), BT_GATT_CHRC_READ,
			       BT_GATT_PERM_READ, characteristic_read, NULL, &button4val),				
);


// Callback when a client reads the characteristic.
//
// Documented under name "bt_gatt_attr_read_chrc()"
static ssize_t characteristic_read(struct bt_conn *conn,
			                       const struct bt_gatt_attr *attr,
								   void *buf,
			                       uint16_t len,
								   uint16_t offset)
{
	// The `user_data` corresponds to the pointer provided as the last "argument"
	// to the `BT_GATT_CHARACTERISTIC` macro.
	uint32_t *value = attr->user_data; // this reassignment may be unnecessary

	// Need to encode data into a buffer to send to client.
	uint8_t out_buffer[4] = {0};

	out_buffer[0]=(*value >> 24) & 0xFF;
	out_buffer[1]=(*value >> 16) & 0xFF;
	out_buffer[2]=(*value >> 8)  & 0xFF;
	out_buffer[3]=(*value >> 0)  & 0xFF;

	// User helper function to encode the output data to send to
	// the client.
	return bt_gatt_attr_read(conn, attr, buf, len, offset, out_buffer, 4);
}


// Setup callbacks when devices connect and disconnect.
static void connected(struct bt_conn *conn, uint8_t err)
{
	if (err) {
		printk("Connection failed (err 0x%02x)\n", err);
	} else {
		printk("Connected\n");
	}
}

static void disconnected(struct bt_conn *conn, uint8_t reason)
{
	printk("Disconnected (reason 0x%02x)\n", reason);
}

BT_CONN_CB_DEFINE(conn_callbacks) = {
	.connected = connected,
	.disconnected = disconnected,
};

static void bt_ready(int err)
{
	if (err) {
		printk("Bluetooth init failed (err %d)\n", err);
		return;
	}

	printk("Bluetooth initialized\n");

	err = bt_le_adv_start(BT_LE_ADV_CONN, ad, ARRAY_SIZE(ad), NULL, 0);
	if (err) {
		printk("Advertising failed to start (err %d)\n", err);
		return;
	}

	printk("Advertising successfully started\n");
}

void main(void)
{
	int err;

	init_button(&button1, &button1_cb_data, button1_pressed);
	init_button(&button2, &button2_cb_data, button2_pressed);
	init_button(&button3, &button3_cb_data, button3_pressed);
	init_button(&button4, &button4_cb_data, button4_pressed);

	err = bt_enable(bt_ready);
	if (err) {
		printk("Bluetooth init failed (err %d)\n", err);
		return;
	}
}
