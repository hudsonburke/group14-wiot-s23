#include <zephyr/types.h>
#include <stddef.h>
#include <sys/printk.h>
#include <sys/util.h>

#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>

//in the project directory run "pio device monitor > /data/packets.csv"

static void scan_cb(const bt_addr_le_t *addr, int8_t rssi, uint8_t adv_type,
		    struct net_buf_simple *buf)
{
	char src_addr[18];
	
    // Convert address to typical MAC address format.
	bt_addr_le_to_str(addr, src_addr, 18);
    
    char advertiser_data[37];
    memcpy(advertiser_data, buf->data, buf->len);
    printk("%s %d", src_addr, adv_type);
    // csv line structure: src_addr, adv_type, payload{[LEN, TYPE, DATA], ...}
    uint8_t ad_len = 0;
    for (int i = 0; i < buf->len; i++){
        ad_len = advertiser_data[i];
        // AD Length, AD Type
        printk(" 0x%02x 0x%02x 0x", ad_len, advertiser_data[i+1]); 
        // AD Data
        for (int j = i+2; j < i + ad_len; j++){
            printk("%02x", advertiser_data[j]);
        }
        i+=ad_len;
    }
    printk("\n");
}

void main(void) 
{
	struct bt_le_scan_param scan_param = {
		.type       = BT_HCI_LE_SCAN_ACTIVE,
		.options    = BT_LE_SCAN_OPT_FILTER_DUPLICATE, // previously BT_LE_SCAN_OPT_NONE,
		.interval   = 0x0010,
		.window     = 0x0010,
	};
	int err;

	printk("Starting Scanner\n");

	// Initialize the Bluetooth Subsystem
	err = bt_enable(NULL);
	if (err) {
		printk("Bluetooth init failed (err %d)\n", err);
		return;
	}

	printk("Bluetooth initialized\n");
	err = bt_le_scan_start(&scan_param, scan_cb);
	if (err) {
		printk("Starting scanning failed (err %d)\n", err);
		return;
	}

}