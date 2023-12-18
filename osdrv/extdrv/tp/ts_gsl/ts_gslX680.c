/*
 * drivers/input/touchscreen/ts_gslX680.c
 *
 *  Copyright (C) 2018 Anyka(Guangzhou) Microelectronics Technology Co., Ltd.
 *	Zhipeng Zhang <zhang_zhipeng@anyka.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 2 as
 *  published by the Free Software Foundation.
 */


#include <linux/module.h>
#include <linux/delay.h>
#include <linux/hrtimer.h>
#include <linux/i2c.h>
#include <linux/input.h>
#include <linux/interrupt.h>
#include <linux/io.h>
#include <linux/platform_device.h>
#include <linux/async.h>


#include <linux/slab.h>
#include <linux/of_gpio.h>
#include <asm-generic/gpio.h>
#include <linux/gpio.h>

//#include <mach/map.h>
#include <linux/irq.h>
//#include <mach/irqs.h>

#include <linux/workqueue.h>
#include <linux/proc_fs.h>
#include <linux/input/mt.h>
#include <linux/version.h>

#include "ts_gsl3692.h"


//#define REPORT_DATA_ANDROID_4_0
#define DRIVER_VERSION "1.0.2"
//#define HAVE_TOUCH_KEY

#define GSLX680_I2C_NAME 	"ts_gslX680"
#define GSLX680_I2C_ADDR 	0x40

/*AK3760D MIPI BOARD. */
//#define IRQ_PORT			78
//#define REST_PORT			79

/*********************** GSL3692 register *********************************/
#define GSL_DATA_REG		0x80
/* contains the number of touches in the screen. */
#define GSL_TOUCH_NR	0x80

/* 
** contains the coordinates for the first touch,second touch,
** third touch,fourth,and so on. Up to ten.
** four bytes for each touch.
**/
#define GSL_FIRST_COORDINATES  0x84
#define GSL_SECOND_COORDINATES 0x88
#define GSL_THIRD_COORDINATES  0x8c
#define GSL_FOURTH_COORDINATES 0x90
#define GSL_FIVE_COORDINATES   0x94
#define GSL_SIXTH_COORDINATES  0x98
#define GSL_SEVEN_COORDINATES  0x9c
#define GSL_EIGHT_COORDINATES  0xa0
#define GSL_NINE_COORDINATES   0xa4
#define GSL_TEN_COORDINATES    0xa8
/* touches max 10 */
#define GSL_TS_DATA_LEN	44

/* indicate the chip status when the chip work. */
#define GSL_CHIP_STATUS        0xb0
#define GSL_STATUS_OK	       0x5A5A5A5A
/* count the interrupts when the chip work. */
#define GSL_IRQ_REG	           0xb4
/* indicate the chip status when the chip suddenly power off. */
#define GSL_POWER_REG	       0xbc
#define GSL_CMD_START	       0x00

/*for reset chip and startup chip  */
#define GSL_STATUS_REG		   0xe0
#define GSL_CMD_RESET	       0x88

/* some kind of control registers. Needed for uploading the firmware.
** and soft resetting the chip.
**/
#define GSL_CLOCK_REG		   0xe4
#define GSL_CLOCK		       0x04
/* 
** page register. Contains the memory page number 
** currently mapped in the 0x0--0x7f registers.
**/
#define GSL_PAGE_REG		   0xf0
/* the chip id. */
#define GSL_ID_REG		       0xfc



#define PRESS_MAX    		255
#define MAX_FINGERS 		10
#define MAX_CONTACTS 		10
#define DMA_TRANS_LEN		0x20

#define I2C_SPEED           300*1000

#define GPIO_LOW 0
#define GPIO_HIGH 1

static u32 chip_id = 0;

#ifdef HAVE_TOUCH_KEY
static u16 key = 0;
static int key_state_flag = 0;
struct key_data {
	u16 key;
	u16 x_min;
	u16 x_max;
	u16 y_min;
	u16 y_max;	
};

const u16 key_array[]={
                                      KEY_BACK,
                                      KEY_HOME,
                                      KEY_MENU,
                                      KEY_SEARCH,
                                     }; 
#define MAX_KEY_NUM     (sizeof(key_array)/sizeof(key_array[0]))

struct key_data gsl_key_data[MAX_KEY_NUM] = {
	{KEY_BACK, 2048, 2048, 2048, 2048},
	{KEY_HOME, 2048, 2048, 2048, 2048},	
	{KEY_MENU, 2048, 2048, 2048, 2048},
	{KEY_SEARCH, 2048, 2048, 2048, 2048},
};
#endif

struct gsl_ts_data {
	u8 x_index;
	u8 y_index;
	u8 z_index;
	u8 id_index;
	u8 touch_index;
	u8 data_reg;
	u8 status_reg;
	u8 data_size;
	u8 touch_bytes;
	u8 update_data;
	u8 touch_meta_data;
	u8 finger_size;
};

static struct gsl_ts_data devices[] = {
	{
        /*X coordinates starting address offset. */
		.x_index = 6,
		/*Y coordinates starting address offset. */
		.y_index = 4,
		.z_index = 5,
		/*Finger identifier starting address offset. */
		.id_index = 7,
		.data_reg = GSL_DATA_REG,
		.status_reg = GSL_STATUS_REG,
		.update_data = 0x4,
		.touch_bytes = 4,
		.touch_meta_data = 4,
		.finger_size = 70,
	},
};

struct gsl_ts {
	struct i2c_client *client;
	struct input_dev *input;
	struct work_struct work;
	struct workqueue_struct *wq;
	struct gsl_ts_data *dd;
	u8 *touch_data;
	u8 device_id;
	u8 prev_touches;
	bool is_suspended;
	bool int_pending;
	struct mutex sus_lock;

	int 	irq;
	int		reset_gpio;
	int		irq_gpio;
#if defined(CONFIG_HAS_EARLYSUSPEND)
	struct early_suspend early_suspend;
#endif
#ifdef GSL_TIMER
	struct timer_list gsl_timer;
#endif

};


static	 struct gsl_ts *ts;
static   int  ts_global_reset_pin;           

static u32 id_sign[MAX_CONTACTS+1] = {0};
static u8 id_state_flag[MAX_CONTACTS+1] = {0};
static u8 id_state_old_flag[MAX_CONTACTS+1] = {0};
static u16 x_old[MAX_CONTACTS+1] = {0};
static u16 y_old[MAX_CONTACTS+1] = {0};
static u16 x_new = 0;
static u16 y_new = 0;



/*******************************************************
 *                  DEBUG ROUTINE
 * user can dump gsl3692  registers by /proc/gsl
 *******************************************************/
#define PROC_GSL_REGISTERS	1
#define GSL_MAX			1 /* sufficient for now */

/* register type */
struct reg {
	char addr;		/* virtual address of register */
	const char *name;
};

/* register list of ak37d gsl chip */
static struct reg ak37d_gsl_reg_list[] = {
	{GSL_TOUCH_NR		,	"Number of touches in the screen"},
	{GSL_FIRST_COORDINATES,	"Coordinates for the first touch"},
	{GSL_SECOND_COORDINATES,"Coordinates for the second touch"},
	{GSL_THIRD_COORDINATES, "Coordinates for the third touch"},
	{GSL_FOURTH_COORDINATES,"Coordinates for the fourth touch"},
	{GSL_FIVE_COORDINATES,  "Coordinates for the five touch"},
	{GSL_SIXTH_COORDINATES, "Coordinates for the sixth touch"},
	{GSL_SEVEN_COORDINATES, "Coordinates for the seven touch"},
	{GSL_EIGHT_COORDINATES, "Coordinates for the eight touch"},
	{GSL_NINE_COORDINATES,  "Coordinates for the nine touch"},
	{GSL_TEN_COORDINATES,   "Coordinates for the ten touch"},
	{GSL_CHIP_STATUS,       "Indicate the chip status"},
	{GSL_IRQ_REG,           "Count the interrupts "},
	{GSL_POWER_REG,         "Power status register"},
	{GSL_STATUS_REG,        "Reset and startup chip"},
	{GSL_CLOCK_REG,         "Clock control registers"},
	{GSL_PAGE_REG,          "Page number register"},
	{GSL_ID_REG,            "Chip id"},

};

static int gslX680_shutdown_low(void)
{
	int reset_value;

	gpio_direction_output(ts_global_reset_pin, 1);
	reset_value = gpio_get_value(ts_global_reset_pin);
	gpio_set_value(ts_global_reset_pin,GPIO_LOW);
	reset_value = gpio_get_value(ts_global_reset_pin);

	return 0;
}

static int gslX680_shutdown_high(void)
{
	int reset_value;

	gpio_direction_output(ts_global_reset_pin, 1);
	reset_value = gpio_get_value(ts_global_reset_pin);
	gpio_set_value(ts_global_reset_pin,GPIO_HIGH);
	reset_value = gpio_get_value(ts_global_reset_pin);

	return 0;
}

static inline u16 join_bytes(u8 a, u8 b)
{
	u16 ab = 0;
	ab = ab | a;
	ab = ab << 8 | b;
	return ab;
}



static __inline__ void fw2buf(u8 *buf, const u32 *fw)
{
	u32 *u32_buf = (int *)buf;
	*u32_buf = *fw;
}

static int gsl_ts_write(struct i2c_client *client, u8 addr, u8 *pdata, int datalen)
{
	int ret = 0;
	u8 tmp_buf[128];
	unsigned int bytelen = 0;
	if (datalen > 125)
	{
		printk("%s too big datalen = %d!\n", __func__, datalen);
		return -1;
	}

	tmp_buf[0] = addr;
	bytelen++;

	if (datalen != 0 && pdata != NULL)
	{
		memcpy(&tmp_buf[bytelen], pdata, datalen);
		bytelen += datalen;
	}

	ret = i2c_master_send(client, tmp_buf, bytelen);
	return ret;
}

static int gsl_ts_read(struct i2c_client *client, u8 addr, u8 *pdata, unsigned int datalen)
{
	int ret = 0;

	if (datalen > 126)
	{
		printk("%s too big datalen = %d!\n", __func__, datalen);
		return -1;
	}

	ret = gsl_ts_write(client, addr, NULL, 0);
	if (ret < 0)
	{
		printk("%s set data address fail!\n", __func__);
		return ret;
	}
	
	return i2c_master_recv(client, pdata, datalen);
}


static void ak_load_fw(struct i2c_client *client)
{
	u8 buf[DMA_TRANS_LEN*4 + 1] = {0};
    u8 page_num;
    u32 tmp;
    u8 buf1[4] = {0};
	u32 source_line = 0;
	u32 source_len = ARRAY_SIZE(GSL1680E_FW);

	pr_info("gsl loading fireware...\n");

	for (source_line = 0; source_line < source_len; source_line++) 
	{
		/* init page trans, set the page val */
		if (GSL_PAGE_REG == GSL1680E_FW[source_line].offset)
		{
			page_num = (char)GSL1680E_FW[source_line].val;
            gsl_ts_write(client,GSL_PAGE_REG, &page_num, sizeof(page_num));
		}
		else 
		{

	    	buf[0] = (u8)GSL1680E_FW[source_line].offset;
            tmp = GSL1680E_FW[source_line].val;
            memcpy(buf1,&tmp,sizeof(tmp));
            /*write 4 bytes data one time.*/
            gsl_ts_write(client, buf[0], buf1, 4);
		}
	}

	pr_info("gsl finish\n");

}


static u32 gsl_ts_get_status(struct i2c_client *client)
{
	u32 tmp;
    u8 status[4] = {0};

	int ret;
	ret = gsl_ts_read(client, GSL_CHIP_STATUS, status, sizeof(status) );
	if  (ret  < 0)  
	{
		pr_info("I2C transfer error!\n");
	}
	else
	{
	    memcpy(&tmp,status,sizeof(status));
		pr_info("chip status is %x\n", tmp);
	}
	msleep(10);

    return ret;
}

static int gsl_ts_get_id(struct i2c_client *client)
{
	int tmp = 0;
    u8 read_buf[4] = {0};

	int ret;
	ret = gsl_ts_read( client, GSL_ID_REG, read_buf, sizeof(read_buf) );
	if  (ret  < 0)  
	{
		pr_info("I2C transfer error!\n");
	}
	else
	{
	    memcpy(&tmp,read_buf,sizeof(read_buf));
	    chip_id = be32_to_cpu(tmp);
		pr_info("chip id is %x\n", chip_id);
	}
	msleep(10);

	return 0;
}


static void startup_chip(struct i2c_client *client)
{
	u8 tmp = 0x00;
	gsl_ts_write(client, 0xe0, &tmp, 1);

#ifdef GSL_NOID_VERSION
	//gsl_DataInit(gsl_config_data_id);
#endif
	msleep(10);	
}

static void reset_chip(struct i2c_client *client)
{
	u8 buf[4] = {0x00};

    /*Status register: 0xE0, set it to 0x88 */
	u8 tmp = 0x88;
	gsl_ts_write(client, 0xe0, &tmp, sizeof(tmp));
	msleep(10);

    /*Control register: 0xE4, set it to 0x04 */
	tmp = 0x04;
	gsl_ts_write(client, 0xe4, &tmp, sizeof(tmp));
	msleep(10);

    /*Control register: 0xBC-0xBF, set them to 0x0 */
	gsl_ts_write(client, 0xbc, buf, sizeof(buf));
	msleep(10);
}

/* chip initialization. */
static void init_chip(struct i2c_client *client)
{
	/*gslX680_shutdown_low();
	msleep(100); 
	gslX680_shutdown_high();
	msleep(50);
	*/
	reset_chip(client);

    /*
    * Upload the firmware.
    * This chip needs a firemware to be loaded before it
    * would be able to work.
    */
	//gsl_load_fw(client);
    ak_load_fw(client);
    
    /*Status register: 0xE0, set it to 0x0 */
	startup_chip(client);
    
	reset_chip(client);
	#if 1
    /* disable the chip and wait 50 ms*/
	gslX680_shutdown_low();	
	msleep(50); 
    
    /* enable the chip and wait 30 ms*/  
	gslX680_shutdown_high();	
	msleep(30); 		

    /* disable the chip and wait 5 ms*/
	gslX680_shutdown_low();	
	msleep(5); 	

    /* enable the chip and wait 20 ms*/  
	gslX680_shutdown_high();	
	msleep(20); 	
    
	reset_chip(client);
	#endif
    /*
    * Finally,start up the chip. After this procedure,the chip will
    * start to read touches and sending events.
    */
	startup_chip(client);
}


/* check the data which store in the chip's memory if correct. */
static void check_mem_data(struct i2c_client *client)
{
	char write_buf;
	char read_buf[4]  = {0};
	
	msleep(30);

	write_buf = 0x00;
    /* check the data which store in the chip's memory if correct. */
	gsl_ts_write(client,0xf0, &write_buf, sizeof(write_buf));
    /* read page0's data */
	gsl_ts_read(client,0x00, read_buf, sizeof(read_buf));
    /* read page0's data again */
	gsl_ts_read(client,0x00, read_buf, sizeof(read_buf));

	if (read_buf[3] != 0x1 || read_buf[2] != 0 || read_buf[1] != 0 || read_buf[0] != 0)
	{
	    /*if it's wrong data,then we need to try initing chip aganin. */
		pr_debug("!!!!!!!!!!!page: %x offset: %x val: %x %x %x %x\n",0x0, 0x0, read_buf[3], read_buf[2], read_buf[1], read_buf[0]);
		init_chip(client);
	}

}

/* record the coordinates changes */
static void record_point(u16 x, u16 y , u8 id)
{
	u16 x_err =0;
	u16 y_err =0;

	id_sign[id]=id_sign[id]+1;

    /*if it's the first touch. */
	if(id_sign[id]==1){
		x_old[id]=x;
		y_old[id]=y;
	}

	x = (x_old[id] + x)/2;
	y = (y_old[id] + y)/2;
		
	if(x>x_old[id]){
		x_err=x -x_old[id];
	}
	else{
		x_err=x_old[id]-x;
	}

	if(y>y_old[id]){
		y_err=y -y_old[id];
	}
	else{
		y_err=y_old[id]-y;
	}

    /*update the coordinates when touch action have changed. */
	if( (x_err > 3 && y_err > 1) || (x_err > 1 && y_err > 3) ){
		x_new = x;     x_old[id] = x;
		y_new = y;     y_old[id] = y;
	}
	else{
		if(x_err > 3){
			x_new = x;     x_old[id] = x;
		}
		else
			x_new = x_old[id];
		if(y_err> 3){
			y_new = y;     y_old[id] = y;
		}
		else
			y_new = y_old[id];
	}
    
    /*the coordinates are new when it's the first touch. */
	if(id_sign[id]==1){
		x_new= x_old[id];
		y_new= y_old[id];
	}
	
}

#ifdef HAVE_TOUCH_KEY
static void report_key(struct gsl_ts *ts, u16 x, u16 y)
{
	u16 i = 0;

	for(i = 0; i < MAX_KEY_NUM; i++) 
	{
		if((gsl_key_data[i].x_min < x) && (x < gsl_key_data[i].x_max)&&(gsl_key_data[i].y_min < y) && (y < gsl_key_data[i].y_max))
		{
			key = gsl_key_data[i].key;	
			input_report_key(ts->input, key, 1);
			input_sync(ts->input); 		
			key_state_flag = 1;
			break;
		}
	}
}
#endif



/*  
* report_data by input subsystem.
*/
static void report_data(struct gsl_ts *ts, u16 x, u16 y, u8 pressure, u8 id)
{
	if(x>=SCREEN_MAX_X||y>=SCREEN_MAX_Y)
	{
	#ifdef HAVE_TOUCH_KEY
		report_key(ts,x,y);
	#endif
		return;
	}
	
	pr_debug("****finally report_data  X= %x Y= %x  ID= %x\n",x, y,id);	
	
#ifdef REPORT_DATA_ANDROID_4_0
	input_mt_slot(ts->input, id);		
	input_report_abs(ts->input, ABS_MT_TRACKING_ID, id);
	input_report_abs(ts->input, ABS_MT_TOUCH_MAJOR, pressure);
	input_report_abs(ts->input, ABS_MT_POSITION_X, x);
	input_report_abs(ts->input, ABS_MT_POSITION_Y, y);	
	input_report_abs(ts->input, ABS_MT_WIDTH_MAJOR, 1);
#else
	input_report_abs(ts->input, ABS_MT_TRACKING_ID, id);
	input_report_abs(ts->input, ABS_MT_TOUCH_MAJOR, pressure);
	input_report_abs(ts->input, ABS_MT_POSITION_X,x);
	input_report_abs(ts->input, ABS_MT_POSITION_Y, y);
	input_report_abs(ts->input, ABS_MT_WIDTH_MAJOR, 1);
	input_mt_sync(ts->input);
#endif
}



/*  
* processing the coordinates data.
*
* The four bytes of each group of coordinates contains the X and Y values,
* and also the finger.
*
* The first two bytes contains,in little endian format, the X coordinate in
* the 12 lower bits. The other two bytes contains,in little endian format,
* the Y coordinate in the lower bits. The 4 uppper bits in the Y coordinate
* contains the finger identifier that did the touch.
*/
static void process_gslX680_data(struct gsl_ts *ts)
{
	u8 id, touches;
	u16 x, y;
	int i = 0;

	touches = cinfo.finger_num;
	
	for(i=1;i<=MAX_CONTACTS;i++)
	{
		if(touches == 0)
			id_sign[i] = 0;	
		id_state_flag[i] = 0;
	}
	for(i= 0;i < (touches > MAX_FINGERS ? MAX_FINGERS : touches);i ++)
	{
		#ifdef GSL_NOID_VERSION
		id = cinfo.id[i];
		x =  cinfo.x[i];
		y =  cinfo.y[i];
		#else
	    /* obtain X coordinates. */
		x = join_bytes( ( ts->touch_data[ts->dd->x_index  + 4 * i + 1] & 0xf),
				ts->touch_data[ts->dd->x_index + 4 * i]);
        /* obtain Y coordinates. */
		y = join_bytes(ts->touch_data[ts->dd->y_index + 4 * i + 1],
				ts->touch_data[ts->dd->y_index + 4 * i ]);
        /* obtain finger identifier. */
		id = ((ts->touch_data[ts->dd->id_index  + 4 * i ] & 0xf0)>>4);

		#endif
	    //pr_debug("process_gslx680_data  X= %x Y= %x  ID= %x\n",x, y,id);	

		if(1 <=id && id <= MAX_CONTACTS)
		{
			record_point(x, y , id);
			report_data(ts, x_new, y_new, 10, id);		
			id_state_flag[id] = 1;
		}
	}
	for(i=1;i<=MAX_CONTACTS;i++)
	{	
		if( (0 == touches) || ((0 != id_state_old_flag[i]) && (0 == id_state_flag[i])) )
		{
		#ifdef REPORT_DATA_ANDROID_4_0
			input_mt_slot(ts->input, i);
			input_report_abs(ts->input, ABS_MT_TRACKING_ID, -1);
			input_mt_report_slot_state(ts->input, MT_TOOL_FINGER, false);
		#endif
			id_sign[i]=0;
		}
		id_state_old_flag[i] = id_state_flag[i];
	}
#ifndef REPORT_DATA_ANDROID_4_0
	if(0 == touches)
	{	
		input_mt_sync(ts->input);
	#ifdef HAVE_TOUCH_KEY
		if(key_state_flag)
		{
        		input_report_key(ts->input, key, 0);
			input_sync(ts->input);
			key_state_flag = 0;
		}
	#endif			
	}
#endif
	input_sync(ts->input);
	ts->prev_touches = touches;
}


/*
* get touch coordinates from the chip as a worker.
*/
static void gsl_ts_xy_worker(struct work_struct *work)
{
	int rc = 0;
	u8  read_buf[4] = {0};
	u8 buf[4] = {0};
	unsigned int  temp_a, temp_b, i;
	u8 touch_data[MAX_FINGERS * 4 + 4] = {0};
	char point_num = 0;
	int tmp1 = 0;

	struct gsl_ts *ts = container_of(work, struct gsl_ts,work);			 

	if (ts->is_suspended == true) {
		dev_dbg(&ts->client->dev, "TS is supended\n");
		ts->int_pending = true;
		goto schedule;
	}

	i2c_smbus_read_i2c_block_data(ts->client, 0x80, 4, &touch_data[0]);
	point_num = touch_data[0];

	if(point_num > 0)
		i2c_smbus_read_i2c_block_data(ts->client, 0x84, 8, &touch_data[4]);
	if(point_num > 2)
		i2c_smbus_read_i2c_block_data(ts->client, 0x8c, 8, &touch_data[12]);
	if(point_num > 4)
		i2c_smbus_read_i2c_block_data(ts->client, 0x94, 8, &touch_data[20]);
	if(point_num > 6)
		i2c_smbus_read_i2c_block_data(ts->client, 0x9c, 8, &touch_data[28]);
	if(point_num > 8)
		i2c_smbus_read_i2c_block_data(ts->client, 0xa4, 8, &touch_data[36]);


	if (rc < 0)
	{
		dev_err(&ts->client->dev, "read failed\n");
		goto schedule;
	}

#ifdef GSL_NOID_VERSION
		cinfo.finger_num = point_num;
		for(i = 0; i < (point_num < MAX_CONTACTS ? point_num : MAX_CONTACTS); i ++)
		{
			temp_a = touch_data[(i + 1) * 4 + 3] & 0x0f;
			temp_b = touch_data[(i + 1) * 4 + 2];
			cinfo.x[i] = temp_a << 8 |temp_b;
			temp_a = touch_data[(i + 1) * 4 + 1];
			temp_b = touch_data[(i + 1) * 4 + 0];
			cinfo.y[i] = temp_a << 8 |temp_b;
			cinfo.id[i] = ((touch_data[(i + 1) * 4 + 3] & 0xf0)>>4);
		}
		cinfo.finger_num = (touch_data[3]<<24)|(touch_data[2]<<16)|
			(touch_data[1]<<8)|touch_data[0];
		//gsl_alg_id_main(&cinfo);
		//tmp1=gsl_mask_tiaoping();

		if(tmp1>0&&tmp1<0xffffffff)
		{
			buf[0]=0xa;buf[1]=0;buf[2]=0;buf[3]=0;
			gsl_ts_write(ts->client, buf[0], buf, 4);
			buf[0]=(u8)(tmp1 & 0xff);
			buf[1]=(u8)((tmp1>>8) & 0xff);
			buf[2]=(u8)((tmp1>>16) & 0xff);
			buf[3]=(u8)((tmp1>>24) & 0xff);
			gsl_ts_write(ts->client, buf[0], buf, 4);
		}
		point_num = cinfo.finger_num;
#endif

#if 0
	if (touch_data[ts->dd->touch_index] == 0xff) {
		goto schedule;
	}
#endif
	rc = gsl_ts_read( ts->client, 0xbc, read_buf, sizeof(read_buf));
	if (rc < 0)
	{
		dev_err(&ts->client->dev, "read 0xbc failed\n");
		goto schedule;
	}

	if (read_buf[3] == 0 && read_buf[2] == 0 && read_buf[1] == 0 && read_buf[0] == 0)
	{
		process_gslX680_data(ts);
	}
	else
	{
		reset_chip(ts->client);
		startup_chip(ts->client);
	}

schedule:
	enable_irq(ts->irq);

}

/*
** when there is data available(a press) or in other case,
** interrupt pin gets active to signal this to the CPU.
*/
static irqreturn_t gsl_ts_irq(int irq, void *dev_id)
{	
	struct gsl_ts *ts = dev_id;

	if (ts->is_suspended == true) 
		return IRQ_HANDLED;		

	disable_irq_nosync(ts->irq);

    /* schedule and do a work. */
	if (!work_pending(&ts->work)) 
	{
		queue_work(ts->wq, &ts->work);
	}
	
	return IRQ_HANDLED;

}

#ifdef GSL_TIMER
static void gsl_timer_handle(unsigned long data)
{
	struct gsl_ts *ts = (struct gsl_ts *)data;

#ifdef GSL_DEBUG	
	printk("----------------gsl_timer_handle-----------------\n");	
#endif

	disable_irq_nosync(ts->irq);	
	check_mem_data(ts->client);
	ts->gsl_timer.expires = jiffies + 3 * HZ;
	add_timer(&ts->gsl_timer);
	enable_irq(ts->irq);
	
}
#endif

static int gsl_ts_init_ts(struct i2c_client *client, struct gsl_ts *ts)
{
	struct input_dev *input_device;
	int  rc = 0;
	
	pr_info("[GSLX680] Enter %s\n", __func__);

	
	ts->dd = &devices[ts->device_id];

	if (ts->device_id == 0) {
        /*define coordinates data size,max 10 finger. */
		ts->dd->data_size = MAX_FINGERS * ts->dd->touch_bytes + ts->dd->touch_meta_data;
		ts->dd->touch_index = 0;
	}

	ts->touch_data = kzalloc(ts->dd->data_size, GFP_KERNEL);
	if (!ts->touch_data) {
		pr_err("%s: Unable to allocate memory\n", __func__);
		return -ENOMEM;
	}

	ts->prev_touches = 0;

	input_device = input_allocate_device();
	if (!input_device) {
		rc = -ENOMEM;
		goto error_alloc_dev;
	}

	ts->input = input_device;
	input_device->name = GSLX680_I2C_NAME;
	input_device->id.bustype = BUS_I2C;
	input_device->dev.parent = &client->dev;
	input_set_drvdata(input_device, ts);

#ifdef REPORT_DATA_ANDROID_4_0
	__set_bit(EV_ABS, input_device->evbit);
	__set_bit(EV_KEY, input_device->evbit);
	__set_bit(EV_REP, input_device->evbit);
	__set_bit(INPUT_PROP_DIRECT, input_device->propbit);
	input_mt_init_slots(input_device, (MAX_CONTACTS+1),INPUT_MT_POINTER);
#else
	input_set_abs_params(input_device,ABS_MT_TRACKING_ID, 0, (MAX_CONTACTS+1), 0, 0);
	set_bit(EV_ABS, input_device->evbit);
	set_bit(EV_KEY, input_device->evbit);
#endif

#ifdef HAVE_TOUCH_KEY
	input_device->evbit[0] = BIT_MASK(EV_KEY);
	for (i = 0; i < MAX_KEY_NUM; i++)
		set_bit(key_array[i], input_device->keybit);
#endif

	set_bit(ABS_MT_POSITION_X, input_device->absbit);
	set_bit(ABS_MT_POSITION_Y, input_device->absbit);
	set_bit(ABS_MT_TOUCH_MAJOR, input_device->absbit);
	set_bit(ABS_MT_WIDTH_MAJOR, input_device->absbit);

	input_set_abs_params(input_device,ABS_MT_POSITION_X, 0, SCREEN_MAX_X, 0, 0);
	input_set_abs_params(input_device,ABS_MT_POSITION_Y, 0, SCREEN_MAX_Y, 0, 0);
	input_set_abs_params(input_device,ABS_MT_TOUCH_MAJOR, 0, PRESS_MAX, 0, 0);
	input_set_abs_params(input_device,ABS_MT_WIDTH_MAJOR, 0, 200, 0, 0);

	client->irq = ts->irq;
	//ts->irq = client->irq;

    /* create a single work queue and create a kthread. */
	ts->wq = create_singlethread_workqueue("kworkqueue_ts");
	if (!ts->wq) {
		dev_err(&client->dev, "Could not create workqueue\n");
		goto error_wq_create;
	}
	flush_workqueue(ts->wq);	
    
    /* create a work to handle with GSL data. */   
	INIT_WORK(&ts->work, gsl_ts_xy_worker);

    /*register linux input framework. */
	rc = input_register_device(input_device);
	if (rc)
		goto error_unreg_device;

	return 0;

error_unreg_device:
	destroy_workqueue(ts->wq);
error_wq_create:
	input_free_device(input_device);
error_alloc_dev:
	kfree(ts->touch_data);
	return rc;
}


#ifdef CONFIG_HAS_EARLYSUSPEND
static void gsl_ts_early_suspend(struct early_suspend *h)
{
	struct gsl_ts *ts = container_of(h, struct gsl_ts, early_suspend);
	printk("[GSL1680] Enter %s\n", __func__);
	gsl_ts_suspend(&ts->client->dev);
}

static void gsl_ts_late_resume(struct early_suspend *h)
{
	struct gsl_ts *ts = container_of(h, struct gsl_ts, early_suspend);
	printk("[GSL1680] Enter %s\n", __func__);
	gsl_ts_resume(&ts->client->dev);
}
#endif

static int  gsl_ts_probe(struct i2c_client *client,
			const struct i2c_device_id *id)
{
	int rc;
	/*reset and irq */
	/* struct device_node* np = client->dev.of_node; */

	pr_info("GSLX680 Enter %s\n", __func__);
	if (!i2c_check_functionality(client->adapter, I2C_FUNC_I2C)) {
		dev_err(&client->dev, "I2C functionality not supported\n");
		return -ENODEV;
	}
	ts = kzalloc(sizeof(*ts), GFP_KERNEL);
	if (!ts)
		return -ENOMEM;

	ts->client = client;

	client->addr = GSLX680_I2C_ADDR;
	ts->is_suspended = false;
	ts->int_pending = false;
	/*reset and irq */
	/*
	ts->reset_gpio  = of_get_named_gpio(np, "reset-gpio", 0);
	ts->irq_gpio  = of_get_named_gpio(np, "irq-gpio", 0);
	ts->irq = gpio_to_irq(ts->irq_gpio);
	*/
	ts->reset_gpio  = GSL_RST_PORT;
	ts->irq_gpio  = GSL_INT_PORT;
	gpio_request(ts->reset_gpio, "GSL RST PORT");

	gpio_request(ts->irq_gpio, "GSL INT PORT");

	gpio_direction_output(ts->reset_gpio, 1);

	ts->irq = gpio_to_irq(ts->irq_gpio);


	//dev_err(&client->dev, "ts->reset_gpio=%d, ts->irq_gpio=%d, ts->irq=%d\n",ts->reset_gpio, ts->irq_gpio , ts->irq);
	mutex_init(&ts->sus_lock);

    ts_global_reset_pin=ts->reset_gpio;

	i2c_set_clientdata(client, ts);

	rc = gsl_ts_init_ts(client, ts);
	if (rc < 0) {
		dev_err(&client->dev, "GSLX680 init failed\n");
		goto error_mutex_destroy;
	}	
	dev_err(&client->dev, "gsl_probe: init_chip()\n");
	init_chip(ts->client);
	check_mem_data(ts->client);

    gpio_direction_input(ts->irq_gpio);

	//rc=  request_irq(ts->irq, gsl_ts_irq, IRQF_TRIGGER_LOW | IRQF_ONESHOT, "gslx680", ts);
	rc=  request_irq(ts->irq, gsl_ts_irq, IRQF_TRIGGER_RISING, "gslx680", ts);

	if (rc < 0) {
		printk( "gsl_probe: request irq failed\n");
		goto error_req_irq_fail;
	}

    gsl_ts_get_status(client);
    gsl_ts_get_id(client);

#ifdef GSL_TIMER
	printk( "gsl_ts_probe () : add gsl_timer\n");

	init_timer(&ts->gsl_timer);
	ts->gsl_timer.expires = jiffies + 3 * HZ;	//��ʱ3  ����
	ts->gsl_timer.function = &gsl_timer_handle;
	ts->gsl_timer.data = (unsigned long)ts;
	add_timer(&ts->gsl_timer);
#endif

	/* create debug attribute */
	//rc = device_create_file(&ts->input->dev, &dev_attr_debug_enable);

#ifdef CONFIG_HAS_EARLYSUSPEND
	ts->early_suspend.level = EARLY_SUSPEND_LEVEL_BLANK_SCREEN + 1;
	ts->early_suspend.suspend = gsl_ts_early_suspend;
	ts->early_suspend.resume = gsl_ts_late_resume;
	register_early_suspend(&ts->early_suspend);
#endif

	pr_info("[GSLX680] End %s\n", __func__);

	return 0;


error_req_irq_fail:
    free_irq(ts->irq, ts);	

error_mutex_destroy:
	mutex_destroy(&ts->sus_lock);
	input_free_device(ts->input);
	kfree(ts);
	return rc;
}

static int  gsl_ts_remove(struct i2c_client *client)
{
	struct gsl_ts *ts = i2c_get_clientdata(client);
	pr_info("gsl_ts_remove...\n");

#ifdef CONFIG_HAS_EARLYSUSPEND
	unregister_early_suspend(&ts->early_suspend);
#endif
	device_init_wakeup(&client->dev, 0);
	free_irq(ts->irq, ts);
	cancel_work_sync(&ts->work);
	destroy_workqueue(ts->wq);
	input_unregister_device(ts->input);
	mutex_destroy(&ts->sus_lock);
	kfree(ts->touch_data);
	kfree(ts);

	return 0;
}



/******************************************
 * /proc/gsl
 ******************************************/
#ifdef PROC_GSL_REGISTERS
static void *ak_gsl_seq_start(struct seq_file *m, loff_t *pos)
{
	return (*pos < GSL_MAX) ? pos : NULL;
}

static void *ak_gsl_seq_next(struct seq_file *m, void *v, loff_t *pos)
{
	(*pos)++;
	return (*pos < GSL_MAX) ? pos : NULL;
}

static void ak_gsl_seq_stop(struct seq_file *m, void *v)
{

}


static int ak_gsl_seq_show(struct seq_file *m, void *v)
{
	int j=0;

    u32 tmp = 0;
    u8  tmp_reg[4] = {0};
    u8  ret = 0;
    u8  touches = 0;

	/* read data from DATA_REG */
	ret = gsl_ts_read(ts->client, 0x80, &touches, sizeof(touches));
	for(j=0;j<ARRAY_SIZE(ak37d_gsl_reg_list);j++)
	{ 
	    if(ak37d_gsl_reg_list[j].addr == GSL_TOUCH_NR){
	        ret = gsl_ts_read(ts->client, ak37d_gsl_reg_list[j].addr, &touches, sizeof(touches));
        	seq_printf(m,"%-50s=0x%x\n",ak37d_gsl_reg_list[j].name, touches);        
        }
        else{
	        /* read register data from gsl */
	        ret = gsl_ts_read(ts->client, ak37d_gsl_reg_list[j].addr, tmp_reg, sizeof(tmp_reg));
	        if(ret  < 0)  
	        {
		        pr_info("I2C transfer error,read register data from gsl fail !\n");
	        }
            memcpy(&tmp,tmp_reg,sizeof(tmp_reg));
		    seq_printf(m,"%-50s=0x%08x\n",ak37d_gsl_reg_list[j].name, tmp);
        }
    }
	
	return 0;
}

static const struct seq_operations proc_ak_gsl_seq_ops = {
	.start	= ak_gsl_seq_start,
	.next	= ak_gsl_seq_next,
	.stop	= ak_gsl_seq_stop,
	.show	= ak_gsl_seq_show,
};

static int proc_ak_gsl_open(struct inode *inode, struct file *file)
{
	return seq_open(file, &proc_ak_gsl_seq_ops);
}

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 10, 0))
static const struct proc_ops ak37d_gsl_proc_fops = {
	.proc_open = proc_ak_gsl_open,
	.proc_read = seq_read,
	.proc_lseek = seq_lseek,
	.proc_release = seq_release,
};
#else
static const struct file_operations ak37d_gsl_proc_fops = {
	.owner		= THIS_MODULE,
	.open		= proc_ak_gsl_open,
	.read		= seq_read,
	.llseek		= seq_lseek,
	.release	= seq_release,
};
#endif
#endif


static const struct i2c_device_id gsl_ts_id[] = {
	{GSLX680_I2C_NAME, 0},
	{}
};
MODULE_DEVICE_TABLE(i2c, gsl_ts_id);

static const struct of_device_id cvitek_touchscreen_of_ids[] = {
    { .compatible = "cvitek,i2c-touchscreen-gslX680" },
	{},
};
MODULE_DEVICE_TABLE(of, cvitek_touchscreen_of_ids);


static struct i2c_driver gsl_ts_driver = {
	.driver = {
		.name = GSLX680_I2C_NAME,
        .of_match_table = of_match_ptr(cvitek_touchscreen_of_ids),
		.owner = THIS_MODULE,
	},

	.probe		= gsl_ts_probe,
	.remove		= (gsl_ts_remove),
	.id_table	= gsl_ts_id,
};

static int __init gsl_ts_init(void)
{
    int ret;

#ifdef PROC_GSL_REGISTERS
	proc_create("gsl", 0, NULL, &ak37d_gsl_proc_fops);
#endif

	pr_info("gsl_ts_init..\n");
	ret = i2c_add_driver(&gsl_ts_driver);
	return ret;
}
static void __exit gsl_ts_exit(void)
{

#ifdef PROC_GSL_REGISTERS
	remove_proc_entry("gsl", NULL);
#endif

	pr_info("gsl_ts_exit...\n");
	i2c_del_driver(&gsl_ts_driver);
	return;
}

module_init(gsl_ts_init);
module_exit(gsl_ts_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("GSLX680 touchscreen controller driver");
MODULE_AUTHOR("zhangzhipeng, <zhang_zhipeng@anyka.com>");
MODULE_VERSION(DRIVER_VERSION);
MODULE_ALIAS("gsl_ts");
