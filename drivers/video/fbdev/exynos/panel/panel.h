/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (c) Samsung Electronics Co., Ltd.
 * Gwanghui Lee <gwanghui.lee@samsung.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef __PANEL_H__
#define __PANEL_H__

#include <linux/lcd.h>
#include <linux/backlight.h>
#include <linux/spi/spi.h>
#include <linux/sysfs.h>
#include <linux/panel_modes.h>
#include "panel_debug.h"
#include "panel_bl.h"
#include "dimming.h"
#include "panel_dimming.h"
#include "mdnie.h"
#include "copr.h"
#include "panel_vrr.h"

#ifdef CONFIG_PANEL_NOTIFY
#include <linux/panel_notify.h>
#endif

#ifdef CONFIG_EXTEND_LIVE_CLOCK
#include "./aod/aod_drv.h"
#endif
enum {
	MIPI_DSI_WR_UNKNOWN = 0,
	MIPI_DSI_WR_GEN_CMD,
	MIPI_DSI_WR_CMD_NO_WAKE,
	MIPI_DSI_WR_DSC_CMD,
	MIPI_DSI_WR_PPS_CMD,
	MIPI_DSI_WR_GRAM_CMD,
	MIPI_DSI_WR_SRAM_CMD,
	MIPI_DSI_WR_SR_FAST_CMD,
	MAX_MIPI_DSI_CMD_TYPE,
};

#define STRINGFY(x)	(#x)

#define CONFIG_LCD_HBM_60_STEP
#define MIPI_DCS_WRITE_GRAM_START			(0x2C)
#define MIPI_DCS_WRITE_GRAM_CONTINUE		(0x3C)
#define MIPI_DCS_WRITE_SIDE_RAM_START		(0x4C)
#define MIPI_DCS_WRITE_SIDE_RAM_CONTINUE	(0x5C)

#ifdef CONFIG_SEC_FACTORY
#define CONFIG_SUPPORT_PANEL_SWAP
#define CONFIG_SUPPORT_XTALK_MODE
#define CONFIG_SUPPORT_ISC_DEFECT
#define CONFIG_SELFMASK_FACTORY
#endif

#define CONFIG_SUPPORT_MST
#define CONFIG_SUPPORT_GRAYSPOT_TEST

#define CONFIG_SUPPORT_SPI_IF_SEL

#define to_panel_device(_m_)	container_of(_m_, struct panel_device, _m_)

/* DUMMY PANEL NAME */
#define __pn_name__	xx_xx
#define __PN_NAME__	XX_XX

#define PN_CONCAT(a, b)  _PN_CONCAT(a, b)
#define _PN_CONCAT(a, b) a ## _ ## b

#define PANEL_ID_REG		(0x04)
#define PANEL_ID_LEN		(3)
#define PANEL_OCTA_ID_LEN	(20)
#define PANEL_POC_CHKSUM_LEN	(5)
#define PANEL_POC_CTRL_LEN	(4)
#define PANEL_CODE_LEN		(5)
#define PANEL_COORD_LEN		(4)
#define PANEL_DATE_LEN		(7)
#define PANEL_RDDPM_LEN		(3)
#define PANEL_RDDSM_LEN		(3)

#define NORMAL_TEMPERATURE			(25)
#define POWER_IS_ON(pwr)		(pwr <= FB_BLANK_NORMAL)
#define UNDER_MINUS_15(temperature)	(temperature <= -15)
#define UNDER_0(temperature)	(temperature <= 0)
#define CAPS_IS_ON(level)	(level >= 41)
#define PANEL_WAIT_VSYNC_TIMEOUT_MSEC	(50)

struct maptbl;
struct mdnie_info;

/*
 * [command types]
 * 1. delay
 * 2. external pin control
 * 3. packet write
 */

/* common cmdinfo */
struct cmdinfo {
	u32 type;
	char *name;
};

/* delay command */
struct delayinfo {
	u32 type;
	char *name;
	u32 usec;
	u32 nframe;
	ktime_t s_time;
};

struct timer_delay_begin_info {
	u32 type;
	char *name;
	struct delayinfo *delay;
};

#define DLYINFO(_name_) (_name_)

#define TIMER_DLYINFO_BEGIN(_name_) PN_CONCAT(begin, _name_)
#define TIMER_DLYINFO(_name_) (_name_)

#define DEFINE_PANEL_UDELAY_NO_SLEEP(_name_, _usec_)	\
struct delayinfo DLYINFO(_name_) =		\
{										\
	.name = #_name_,					\
	.type = CMD_TYPE_DELAY_NO_SLEEP,	\
	.usec = (_usec_),					\
}

#define DEFINE_PANEL_MDELAY_NO_SLEEP(_name_, _msec_)	\
struct delayinfo DLYINFO(_name_) =		\
{										\
	.name = #_name_,					\
	.type = CMD_TYPE_DELAY_NO_SLEEP,	\
	.usec = (_msec_) * 1000,			\
}

#define DEFINE_PANEL_UDELAY(_name_, _usec_)	\
struct delayinfo DLYINFO(_name_) =		\
{										\
	.name = #_name_,					\
	.type = CMD_TYPE_DELAY,				\
	.usec = (_usec_),					\
}

#define DEFINE_PANEL_MDELAY(_name_, _msec_)	\
struct delayinfo DLYINFO(_name_) =		\
{										\
	.name = #_name_,					\
	.type = CMD_TYPE_DELAY,				\
	.usec = (_msec_) * 1000,			\
}

#define DEFINE_PANEL_FRAME_DELAY(_name_, _nframe_)	\
struct delayinfo DLYINFO(_name_) =		\
{										\
	.name = #_name_,					\
	.type = CMD_TYPE_FRAME_DELAY,		\
	.nframe = (_nframe_),				\
}

#define DEFINE_PANEL_VSYNC_DELAY(_name_, _nvsync_)	\
struct delayinfo DLYINFO(_name_) =		\
{										\
	.name = #_name_,					\
	.type = CMD_TYPE_VSYNC_DELAY,		\
	.nframe = (_nvsync_),				\
}

#define DEFINE_PANEL_TIMER_BEGIN(_name_, _timer_delay_)		\
struct timer_delay_begin_info TIMER_DLYINFO_BEGIN(_name_) =	\
{										\
	.name = #_name_,					\
	.type = CMD_TYPE_TIMER_DELAY_BEGIN,	\
	.delay = (_timer_delay_),			\
}

#define DEFINE_PANEL_TIMER_UDELAY(_name_, _usec_)	\
struct delayinfo TIMER_DLYINFO(_name_) =		\
{										\
	.name = #_name_,					\
	.type = CMD_TYPE_TIMER_DELAY,		\
	.usec = (_msec_),					\
	.s_time = (0ULL),					\
}

#define DEFINE_PANEL_TIMER_MDELAY(_name_, _msec_)	\
struct delayinfo TIMER_DLYINFO(_name_) =		\
{										\
	.name = #_name_,					\
	.type = CMD_TYPE_TIMER_DELAY,		\
	.usec = (_msec_) * 1000,			\
	.s_time = (0ULL),					\
}

/* external pin control command */
struct pininfo {
	u32 type;
	char *name;
	int pin;
	int onoff;
};

/* packet command */
enum {
	CMD_TYPE_NONE,
	CMD_TYPE_DELAY,
	CMD_TYPE_DELAY_NO_SLEEP,
	CMD_TYPE_FRAME_DELAY,
	CMD_TYPE_VSYNC_DELAY,
	CMD_TYPE_TIMER_DELAY_BEGIN,
	CMD_TYPE_TIMER_DELAY,
	CMD_TYPE_PINCTL,
	CMD_TYPE_TX_PKT_START,
	CMD_PKT_TYPE_NONE = CMD_TYPE_TX_PKT_START,
	DSI_PKT_TYPE_WR,
	DSI_PKT_TYPE_WR_NO_WAKE,
	DSI_PKT_TYPE_COMP,
	DSI_PKT_TYPE_PPS,
	/* Command to write ddi side ram */
	DSI_PKT_TYPE_WR_SR,
	/*write command to side ram with busy wait*/
	DSI_PKT_TYPE_SR_FAST,
	DSI_PKT_TYPE_WR_MEM,
	CMD_TYPE_TX_PKT_END = DSI_PKT_TYPE_WR_MEM,
	CMD_TYPE_RX_PKT_START,
	SPI_PKT_TYPE_RD = CMD_TYPE_RX_PKT_START,
	DSI_PKT_TYPE_RD_POC,
	DSI_PKT_TYPE_RD,
	CMD_TYPE_RX_PKT_END = DSI_PKT_TYPE_RD,
	SPI_PKT_TYPE_WR,
	SPI_PKT_TYPE_SETPARAM,
	CMD_TYPE_RES,
	CMD_TYPE_SEQ,
	CMD_TYPE_KEY,
	CMD_TYPE_MAP,
	CMD_TYPE_DMP,
	CMD_TYPE_COND_START,
	CMD_TYPE_COND_END,
	MAX_CMD_TYPE,
};

#define IS_CMD_TYPE_TX_MEM_PKT(_type_) \
	(DSI_PKT_TYPE_WR_SR <= (_type_) && (_type_) <= DSI_PKT_TYPE_WR_MEM)

#define IS_CMD_TYPE_TX_PKT(_type_) \
	((CMD_TYPE_TX_PKT_START <= (_type_) && (_type_) <= CMD_TYPE_TX_PKT_END) ||\
	 (_type_) == CMD_TYPE_KEY)

#define IS_CMD_TYPE_RX_PKT(_type_) \
	(CMD_TYPE_RX_PKT_START <= (_type_) && (_type_) <= CMD_TYPE_RX_PKT_END)

#define IS_CMD_TYPE_DELAY(_type_) \
	((_type_) >= CMD_TYPE_DELAY && (_type_) <= CMD_TYPE_FRAME_DELAY)

#define IS_CMD_TYPE_TIMER_DELAY(_type_) \
	((_type_) == CMD_TYPE_TIMER_DELAY_BEGIN || \
	 (_type_) == CMD_TYPE_TIMER_DELAY)


enum {
	RES_UNINITIALIZED,
	RES_INITIALIZED,
};

enum {
	KEY_NONE,
	KEY_DISABLE,
	KEY_ENABLE,
};

enum cmd_level {
	CMD_LEVEL_NONE,
	CMD_LEVEL_1,
	CMD_LEVEL_2,
	CMD_LEVEL_3,
	MAX_CMD_LEVEL,
};

/* key level command */
struct keyinfo {
	u32 type;
	char *name;
	enum cmd_level level;
	u32 en;
	struct pktinfo *packet;
};

#define KEYINFO(_name_) PN_CONCAT(key, _name_)
#define KEYINFO_INIT(_name_, _lv_, _en_, _pkt_)	\
{									\
	.name = #_name_,				\
	.type = (CMD_TYPE_KEY),			\
	.level = (_lv_),				\
	.en = (_en_),				\
	.packet = (_pkt_),				\
}

#define DEFINE_PANEL_KEY(_name_, _lv_, _en_, _pkt_)	\
struct keyinfo KEYINFO(_name_) = KEYINFO_INIT(_name_, _lv_, _en_, _pkt_)

enum cmd_mode {
	CMD_MODE_NONE,
	CMD_MODE_RO,
	CMD_MODE_WO,
	CMD_MODE_RW,
	MAX_CMD_MODE,
};

struct ldi_reg_desc {
	u8 addr;
	char *name;
	enum cmd_mode mode;
	enum cmd_level level;
	bool dirty;
};

#define MAX_LDI_REG		(0x100)
#define LDI_REG_DESC(_addr_, _name_, _mode_, _level_)	\
[(_addr_)] = {											\
	.addr = (_addr_),									\
	.name = (_name_),									\
	.mode = (_mode_),									\
	.level = (_level_),									\
	.dirty = (0),										\
}

enum {
	NDARR_1D,
	NDARR_2D,
	NDARR_3D,
	NDARR_4D,
	NDARR_5D,
	NDARR_6D,
	NDARR_7D,
	NDARR_8D,
	MAX_NDARR,
};

/*
 * indices of n-dimension
 */
struct maptbl_pos {
	unsigned int index[MAX_NDARR];
};

/*
 * shape of maptbl
 */
struct maptbl_shape {
	unsigned int ndim;	/* n-dimension */
	unsigned int sz_dim[MAX_NDARR];
};

struct pkt_update_info {
	u32 offset;
	struct maptbl *maptbl;
	u32 nr_maptbl;
	int (*getidx)(struct pkt_update_info *pktui);
	void *pdata;
};

enum {
	PKT_OPTION_NONE = 0,
	PKT_OPTION_CHECK_TX_DONE = (1 << 0),
	PKT_OPTION_SR_ALIGN_12 = (1 << 1),
	PKT_OPTION_SR_ALIGN_16 = (1 << 2),
};

struct pktinfo {
	u32 type;
	char *name;
	u32 addr;
	u8 *data;
	u8 offset;
	u32 dlen;
	struct pkt_update_info *pktui;
	u32 nr_pktui;
	u32 option;
};

struct rdinfo {
	u32 type;
	char *name;
	u32 addr;
	u32 offset;
	u32 len;
	u8 *data;
};

struct res_update_info {
	u32 offset;
	struct rdinfo *rditbl;
};

struct resinfo {
	u32 type;
	char *name;
	int state;
	u8 *data;
	u32 dlen;
	struct res_update_info *resui;
	u32 nr_resui;
};

struct dumpinfo {
	u32 type;
	char *name;
	struct resinfo *res;
	void (*callback)(struct dumpinfo *info);
};

struct condinfo {
	u32 type;
	char *name;
	bool (*cond)(struct panel_device *);
};

struct condinfo_cont {
	struct condinfo start;
	struct condinfo end;
};

#define DUMPINFO_INIT(_name_, _res_, _callback_)	\
{											\
	.name = #_name_,						\
	.type = (CMD_TYPE_DMP),					\
	.res = (_res_),							\
	.callback = (_callback_),				\
}

#define RESUI(_name_) PN_CONCAT(resui, _name_)
#define RESINFO(_name_) PN_CONCAT(res, _name_)
#define RDINFO(_name_) PN_CONCAT(rd, _name_)

#define RDINFO_INIT(_name_, _type_, _addr_, _offset_, _len_)	\
{											\
	.name = #_name_,						\
	.type = (_type_),						\
	.addr = (_addr_),						\
	.offset = (_offset_),					\
	.len = (_len_),							\
}

#define DEFINE_RDINFO(_name_, _type_, _addr_, _offset_, _len_)	\
struct rdinfo RDINFO(_name_) = RDINFO_INIT(_name_, _type_, _addr_, _offset_, _len_)

#define RESUI_INIT(_rditbl_, _offset_)\
{												\
	.offset = (_offset_),						\
	.rditbl = (_rditbl_),						\
}

#define DEFINE_RESUI(_name_, _rditbl_, _offset_)	\
struct res_update_info RESUI(_name_)[] =			\
{													\
	RESUI_INIT(_rditbl_, _offset_),		\
}

#define RESINFO_INIT(_name_, _arr_, _resui_)	\
{												\
	.name = #_name_,							\
	.type = (CMD_TYPE_RES),						\
	.state = (RES_UNINITIALIZED),				\
	.data = (_arr_),							\
	.dlen = ARRAY_SIZE((_arr_)),				\
	.resui = (_resui_),							\
	.nr_resui = (ARRAY_SIZE(_resui_)),			\
}

#define DEFINE_RESOURCE(_name_, _arr_, _resui_)	\
struct resinfo RESINFO(_name_) = RESINFO_INIT(_name_, _arr_, _resui_)

#define PKTUI(_name_) PN_CONCAT(pktui, _name_)
#define PKTINFO(_name_) PN_CONCAT(pkt, _name_)

#define DECLARE_PKTUI(_name_) \
struct pkt_update_info PKTUI(_name_)[]

#define DEFINE_PKTUI(_name_, _maptbl_, _update_offset_)	\
struct pkt_update_info PKTUI(_name_)[] =			\
{													\
	{												\
		.offset = (_update_offset_),				\
		.maptbl = (_maptbl_),						\
		.nr_maptbl = (1),							\
		.getidx = (NULL),							\
	},												\
}

#define PKTINFO_INIT(_name_, _type_, _arr_, _ofs_, _option_, _pktui_, _nr_pktui_)	\
{												\
	.name = (#_name_),							\
	.type = (_type_),							\
	.data = (_arr_),							\
	.offset = (_ofs_),							\
	.dlen = ARRAY_SIZE((_arr_)),				\
	.pktui = (_pktui_),							\
	.nr_pktui = (_nr_pktui_),					\
	.option = (_option_),							\
}

#define DEFINE_PACKET(_name_, _type_, _arr_, _ofs_, _option_, _pktui_, _nr_pktui_)	\
struct pktinfo PKTINFO(_name_) = PKTINFO_INIT(_name_, _type_, _arr_, _ofs_, _option_, _pktui_, _nr_pktui_)

#define DEFINE_VARIABLE_PACKET(_name_, _type_, _arr_, _ofs_)				\
DEFINE_PACKET(_name_, _type_, _arr_, _ofs_, 0, PKTUI(_name_), ARRAY_SIZE(PKTUI(_name_)))

#define DEFINE_VARIABLE_PACKET_WITH_OPTION(_name_, _type_, _arr_, _ofs_, _option_)				\
DEFINE_PACKET(_name_, _type_, _arr_, _ofs_, _option_, PKTUI(_name_), ARRAY_SIZE(PKTUI(_name_)))

#define DEFINE_STATIC_PACKET(_name_, _type_, _arr_, _ofs_)		\
DEFINE_PACKET(_name_, _type_, _arr_, _ofs_, 0, (NULL), (0))

#define DEFINE_STATIC_PACKET_WITH_OPTION(_name_, _type_, _arr_, _ofs_, _option_)		\
DEFINE_PACKET(_name_, _type_, _arr_, _ofs_, _option_, (NULL), (0))

#define PN_MAP_DATA_TABLE(_name_)	PN_CONCAT(PN_CONCAT(__pn_name__, _name_), table)
#define PN_CMD_DATA_TABLE(_name_)	PN_CONCAT(__PN_NAME__, _name_)

#define DEFINE_PN_STATIC_PACKET(_name_, _type_, _arr_)			\
DEFINE_PACKET(PN_CONCAT(__pn_name__, _name_), _type_,	\
		PN_CONCAT(__PN_NAME__, _arr_), _ofs_, 0, NULL, 0)

#define CONDINFO_S(_name_) (PN_CONCAT(cond_info, _name_)).start
#define CONDINFO_E(_name_) (PN_CONCAT(cond_info, _name_)).end

#define DEFINE_COND(_name_, _func_)						\
struct condinfo_cont PN_CONCAT(cond_info, _name_) = {	\
	.start = {											\
		.name = (#_name_),								\
		.type = (CMD_TYPE_COND_START),					\
		.cond = (void *) _func_,						\
	},													\
	.end = { 											\
		.name = (#_name_),								\
		.type = (CMD_TYPE_COND_END),					\
		.cond = NULL,									\
	},													\
};

/* structure of mapping table */
struct maptbl_ops {
	int (*init)(struct maptbl *tbl);
	int (*getidx)(struct maptbl *tbl);
	void (*copy)(struct maptbl *tbl, u8 *dst);
};

struct maptbl {
	u32 type;
	char *name;
	int nbox;
	int nlayer;
	int nrow;
	int ncol;
	int sz_copy;
	u8 *arr;
	void *pdata;
	bool initialized;
	struct maptbl_ops ops;
};

#define MAPINFO(_name_) (_name_)

#define sizeof_maptbl(tbl) \
	((tbl)->nbox * (tbl)->nlayer * (tbl)->nrow * (tbl)->ncol)

#define sizeof_box(tbl) \
	((tbl)->nlayer * (tbl)->nrow * (tbl)->ncol)

#define sizeof_layer(tbl) \
	((tbl)->nrow * (tbl)->ncol)

#define sizeof_row(tbl) \
	((tbl)->ncol)

#define for_each_maptbl(tbl, __pos__)		\
	for ((__pos__) = 0;						\
		(__pos__) < (sizeof_maptbl(tbl));	\
		(__pos__)++)

#define for_each_box(tbl, __pos__)		\
	for ((__pos__) = 0;						\
		(__pos__) < ((tbl)->nbox);		\
		(__pos__)++)

#define for_each_layer(tbl, __pos__)		\
	for ((__pos__) = 0;						\
		(__pos__) < ((tbl)->nlayer);		\
		(__pos__)++)

#define for_each_row(tbl, __pos__)			\
	for ((__pos__) = 0;						\
		(__pos__) < ((tbl)->nrow);			\
		(__pos__)++)

#define for_each_col(tbl, __pos__)			\
	for ((__pos__) = 0;						\
		(__pos__) < ((tbl)->ncol);			\
		(__pos__)++)

#define DEFINE_MAPTBL(_name_, _arr_, _nlayer_, _nrow_, _ncol_, _sz_copy_, f_init, f_getidx, f_copy)  \
{                                       \
	.type = (CMD_TYPE_MAP),				\
	.name = _name_,                     \
	.arr = (u8 *)(_arr_),               \
	.nbox = 1,                          \
	.nlayer = (_nlayer_),               \
	.nrow = (_nrow_),                   \
	.ncol = (_ncol_),                   \
	.sz_copy = (_sz_copy_),             \
	.pdata = NULL,                      \
	.ops = {                            \
		.init = (f_init),               \
		.getidx = (f_getidx),           \
		.copy = (f_copy),               \
	},                                  \
}

#define DEFINE_0D_MAPTBL(_name_, f_init, f_getidx, f_copy)  \
{                                       \
	.type = (CMD_TYPE_MAP),				\
	.name = #_name_,                    \
	.arr = NULL,                        \
	.nbox = 0,                          \
	.nlayer = 0,                        \
	.nrow = 0,                          \
	.ncol = 0,                          \
	.sz_copy = 0,                       \
	.pdata = NULL,                      \
	.ops = {                            \
		.init = (f_init),               \
		.getidx = (f_getidx),           \
		.copy = (f_copy),               \
	},                                  \
}

#define DEFINE_1D_MAPTBL(_name_, _size_, f_init, f_getidx, f_copy)  \
{                                       \
	.type = (CMD_TYPE_MAP),				\
	.name = #_name_,                    \
	.arr = (u8 *)(_name_),              \
	.nlayer = 1,                        \
	.nrow = ARRAY_SIZE((_name_)),       \
	.ncol = (_size_),                   \
	.sz_copy = (_size_),                \
	.pdata = NULL,                      \
	.ops = {                            \
		.init = (f_init),               \
		.getidx = (f_getidx),           \
		.copy = (f_copy),               \
	},                                  \
}

#define DEFINE_2D_MAPTBL(_name_, f_init, f_getidx, f_copy)  \
{                                       \
	.type = (CMD_TYPE_MAP),				\
	.name = #_name_,                    \
	.arr = (u8 *)(_name_),              \
	.nbox = 1,                          \
	.nlayer = 1,                        \
	.nrow = ARRAY_SIZE(_name_),         \
	.ncol = ARRAY_SIZE((_name_)[0]),    \
	.sz_copy = ARRAY_SIZE((_name_)[0]), \
	.pdata = NULL,                      \
	.ops = {                            \
		.init = f_init,                 \
		.getidx = f_getidx,             \
		.copy = f_copy,                 \
	},                                  \
}

#define DEFINE_3D_MAPTBL(_name_, f_init, f_getidx, f_copy)  \
{                                       \
	.type = (CMD_TYPE_MAP),				\
	.name = #_name_,                    \
	.arr = (u8 *)(_name_),              \
	.nbox = 1,                          \
	.nlayer = ARRAY_SIZE(_name_),       \
	.nrow = ARRAY_SIZE((_name_)[0]),      \
	.ncol = ARRAY_SIZE((_name_)[0][0]),   \
	.sz_copy = ARRAY_SIZE((_name_)[0][0]),\
	.pdata = NULL,                      \
	.ops = {                            \
		.init = f_init,                 \
		.getidx = f_getidx,             \
		.copy = f_copy,                 \
	},                                  \
}

#define DEFINE_4D_MAPTBL(_name_, f_init, f_getidx, f_copy)  \
{                                       \
	.type = (CMD_TYPE_MAP),				\
	.name = #_name_,                    \
	.arr = (u8 *)(_name_),              \
	.nbox = ARRAY_SIZE(_name_),       \
	.nlayer = ARRAY_SIZE((_name_)[0]),       \
	.nrow = ARRAY_SIZE((_name_)[0][0]),      \
	.ncol = ARRAY_SIZE((_name_)[0][0][0]),   \
	.sz_copy = ARRAY_SIZE((_name_)[0][0][0]),\
	.pdata = NULL,                      \
	.ops = {                            \
		.init = f_init,                 \
		.getidx = f_getidx,             \
		.copy = f_copy,                 \
	},                                  \
}

u32 maptbl_index(struct maptbl *tbl, int layer, int row, int col);
u32 maptbl_4d_index(struct maptbl *tbl, int box, int layer, int row, int col);
int maptbl_init(struct maptbl *tbl);
int maptbl_getidx(struct maptbl *tbl);
void maptbl_copy(struct maptbl *tbl, u8 *dst);
void maptbl_memcpy(struct maptbl *dst, struct maptbl *src);
u8 *maptbl_getptr(struct maptbl *tbl);
int maptbl_fill(struct maptbl *tbl, struct maptbl_pos *pos, u8 *src, size_t n);

enum PANEL_SEQ {
	PANEL_INIT_SEQ,
	PANEL_EXIT_SEQ,
	PANEL_RES_INIT_SEQ,
#ifdef CONFIG_SUPPORT_DIM_FLASH
	PANEL_DIM_FLASH_RES_INIT_SEQ,
#endif
#ifdef CONFIG_SUPPORT_GM2_FLASH
	PANEL_GM2_FLASH_RES_INIT_SEQ,
#endif
	PANEL_MAP_INIT_SEQ,
	PANEL_DISPLAY_ON_SEQ,
	PANEL_DISPLAY_OFF_SEQ,
	PANEL_ALPM_ENTER_SEQ,
	PANEL_ALPM_DELAY_SEQ,
	PANEL_ALPM_EXIT_SEQ,
	PANEL_SET_BL_SEQ,
#ifdef CONFIG_SUPPORT_HMD
	PANEL_HMD_ON_SEQ,
	PANEL_HMD_OFF_SEQ,
	PANEL_HMD_BL_SEQ,
#endif
	PANEL_HBM_ON_SEQ,
	PANEL_HBM_OFF_SEQ,
#if defined(CONFIG_PANEL_DISPLAY_MODE)
	PANEL_DISPLAY_MODE_SEQ,
#endif
#ifdef CONFIG_SUPPORT_DSU
	PANEL_DSU_SEQ,
#endif
	PANEL_FPS_SEQ,
	PANEL_BLACK_AND_FPS_SEQ,
	PANEL_MCD_ON_SEQ,
	PANEL_MCD_OFF_SEQ,
	PANEL_MCD_RS_ON_SEQ,
	PANEL_MCD_RS_OFF_SEQ,
	PANEL_MCD_RS_READ_SEQ,
#ifdef CONFIG_SUPPORT_MST
	PANEL_MST_ON_SEQ,
	PANEL_MST_OFF_SEQ,
#endif
#ifdef CONFIG_SUPPORT_GRAM_CHECKSUM
	PANEL_GCT_ENTER_SEQ,
	PANEL_GCT_VDDM_SEQ,
	PANEL_GCT_IMG_UPDATE_SEQ,
	PANEL_GCT_IMG_0_UPDATE_SEQ,
	PANEL_GCT_IMG_1_UPDATE_SEQ,
	PANEL_GCT_EXIT_SEQ,
#endif
#if defined(CONFIG_SEC_FACTORY) && defined(CONFIG_SUPPORT_FAST_DISCHARGE)
	PANEL_FD_SEQ,
#endif
#ifdef CONFIG_SUPPORT_GRAYSPOT_TEST
	PANEL_GRAYSPOT_ON_SEQ,
	PANEL_GRAYSPOT_OFF_SEQ,
#endif
#ifdef CONFIG_SUPPORT_TDMB_TUNE
	PANEL_TDMB_TUNE_SEQ,
#endif
#ifdef CONFIG_SUPPORT_ISC_DEFECT
	PANEL_CHECK_ISC_DEFECT_SEQ,
#endif
#ifdef CONFIG_SUPPORT_SPI_IF_SEL
	PANEL_SPI_IF_ON_SEQ,
	PANEL_SPI_IF_OFF_SEQ,
#endif
#ifdef CONFIG_SUPPORT_CCD_TEST
	PANEL_CCD_TEST_SEQ,
#endif
#ifdef CONFIG_SUPPORT_DYNAMIC_HLPM
	PANEL_DYNAMIC_HLPM_ON_SEQ,
	PANEL_DYNAMIC_HLPM_OFF_SEQ,
#endif
#ifdef CONFIG_SUPPORT_ISC_TUNE_TEST
	PANEL_ISC_THRESHOLD_SEQ,
	PANEL_STM_TUNE_SEQ,
#endif

#ifdef CONFIG_DYNAMIC_FREQ
	PANEL_DYNAMIC_FFC_SEQ,
	PANEL_COMP_LTPS_SEQ,
#endif
#ifdef CONFIG_SUPPORT_MAFPC
	PANEL_MAFPC_IMG_SEQ,
	PANEL_MAFPC_ON_SEQ,
	PANEL_MAFPC_OFF_SEQ,
	PANEL_MAFPC_FAC_CHECKSUM,
	PANEL_MAFPC_SCALE_FACTOR,
#endif
	PANEL_GAMMA_INTER_CONTROL_SEQ,
	PANEL_PARTIAL_DISP_ON_SEQ,
	PANEL_PARTIAL_DISP_OFF_SEQ,
	PANEL_DUMP_SEQ,
#ifdef CONFIG_SUPPORT_DDI_CMDLOG
	PANEL_CMDLOG_DUMP_SEQ,
#endif
	PANEL_CHECK_CONDITION_SEQ,
	PANEL_DIA_ONOFF_SEQ,
#ifdef CONFIG_SUPPORT_MASK_LAYER
	PANEL_MASK_LAYER_BEFORE_SEQ,
	PANEL_MASK_LAYER_AFTER_SEQ,
#endif
	PANEL_DUMMY_SEQ,
	MAX_PANEL_SEQ,
};

/* structure of sequence table */
struct seqinfo {
	u32 type;
	char *name;
	void **cmdtbl;
	u32 size;
};

#define SEQINFO(_name_)	(_name_)
#define SEQINFO_INIT(_name_, _cmdtbl_)	\
{										\
	.type = (CMD_TYPE_SEQ),				\
	.name = (_name_),					\
	.cmdtbl = (_cmdtbl_),				\
	.size = ARRAY_SIZE((_cmdtbl_)),		\
}

#define DEFINE_SEQINFO(_name_, _cmdtbl_) \
struct seqinfo SEQINFO(_name_) = SEQINFO_INIT((#_name_), (_cmdtbl_))

struct brt_map {
	int brt;
	int lum;
};

#define DDI_SUPPORT_WRITE_GPARA	(1U << 0)
#define DDI_SUPPORT_READ_GPARA	(1U << 1)
#define DDI_SUPPORT_POINT_GPARA	(1U << 2)

enum {
	PN_COMP_TYPE_NONE,
	PN_COMP_TYPE_MIC,
	PN_COMP_TYPE_DSC,
	MAX_PN_COMP_TYPE,
};

struct panel_dsc {
	u32 slice_w;
	u32 slice_h;
};

enum vrr_mode {
	VRR_NORMAL_MODE,
	VRR_HS_MODE,
	MAX_VRR_MODE,
};

enum {
	VRR_AID_2_CYCLE,
	VRR_AID_4_CYCLE,
};

#define TE_SKIP_TO_DIV(_sw_skip, _hw_skip) \
	((_sw_skip + 1) * (_hw_skip + 1))

struct panel_vrr {
	u32 fps;
	u32 base_fps;
	u32 base_vactive;
	u32 base_vfp;
	u32 base_vbp;
	int te_sel;
	int te_v_st;
	int te_v_ed;
	u32 te_sw_skip_count;
	u32 te_hw_skip_count;
	u32 mode;
	int aid_cycle;
};

#ifdef CONFIG_PANEL_VRR_BRIDGE
struct vrr_bridge_step {
	int frame_delay;
	struct panel_vrr *vrr;
};

struct panel_vrr_bridge {
	int nr_fps_table;
	int origin_fps;
	int origin_mode;
	int origin_aid_cycle;
	int target_fps;
	int target_mode;
	int target_aid_cycle;
	int min_actual_brt;
	int max_actual_brt;
	struct vrr_bridge_step *step;
	int nr_step;
};
#endif

struct panel_resol {
	unsigned int w;
	unsigned int h;
	unsigned int comp_type;
	union {
		struct panel_dsc dsc;
	} comp_param;
	struct panel_vrr **available_vrr;
	unsigned int nr_available_vrr;
#ifdef CONFIG_PANEL_VRR_BRIDGE
	struct panel_vrr_bridge *bridge_rr;
	unsigned int nr_bridge_rr;
#endif
};

struct panel_mres {
	u32 nr_resol;
	struct panel_resol *resol;
};

#ifdef CONFIG_SUPPORT_MASK_LAYER
enum {
	MASK_LAYER_TRIGGER_BEFORE,
	MASK_LAYER_TRIGGER_AFTER,
	MASK_LAYER_TRIGGER_MAX,
};

enum {
	MASK_LAYER_OFF,
	MASK_LAYER_ON,
	MASK_LAYER_MAX,
};

enum {
	MASK_LAYER_HOOK_OFF,
	MASK_LAYER_HOOK_ON,
	MASK_LAYER_HOOK_MAX,
};

struct mask_layer_data {
	u32 trigger_time;
	u32 req_mask_layer;
};
#endif

#if defined(CONFIG_PANEL_DISPLAY_MODE)
struct common_panel_display_mode {
	char name[PANEL_DISPLAY_MODE_NAME_LEN];
	struct panel_resol *resol;
	struct panel_vrr *vrr;
};

struct common_panel_display_modes {
	unsigned int num_modes;
	struct common_panel_display_mode **modes;
};
#endif

struct ddi_properties {
	u32 gpara;
	bool support_partial_disp;
	//D2: avoid abnormal screen issue
	bool ssd_off_lpm_to_normal;
	u32 cmd_fifo_size;
	u32 img_fifo_size;
	bool err_fg_recovery;
	bool err_fg_powerdown;
	bool support_vrr;
	bool support_vrr_lfd;
};

struct common_panel_info {
	char *ldi_name;
	char *name;
	char *model;
	char *vendor;
	u32 id;
	u32 rev;
	struct ddi_properties ddi_props;
	struct panel_mres mres;
	struct panel_vrr **vrrtbl;
	int nr_vrrtbl;
	struct maptbl *maptbl;
	int nr_maptbl;
	struct seqinfo *seqtbl;
	int nr_seqtbl;
	struct rdinfo *rditbl;
	int nr_rditbl;
	struct resinfo *restbl;
	int nr_restbl;
	struct dumpinfo *dumpinfo;
	int nr_dumpinfo;
	struct mdnie_tune *mdnie_tune;
	struct panel_dimming_info *panel_dim_info[MAX_PANEL_BL_SUBDEV];
#ifdef CONFIG_EXYNOS_DECON_LCD_COPR
	struct panel_copr_data *copr_data;
#endif
#ifdef CONFIG_EXTEND_LIVE_CLOCK
	struct aod_tune *aod_tune;
#endif
#ifdef CONFIG_SUPPORT_DDI_FLASH
	struct panel_poc_data *poc_data;
#endif
#ifdef CONFIG_SUPPORT_POC_SPI
	struct spi_data *spi_data;
	struct spi_data **spi_data_tbl;
	int nr_spi_data_tbl;
#endif
#ifdef CONFIG_DYNAMIC_FREQ
	struct df_freq_tbl_info *df_freq_tbl;
#endif
#ifdef CONFIG_SUPPORT_DISPLAY_PROFILER
	struct profiler_tune *profile_tune;
#endif
#ifdef CONFIG_SUPPORT_MAFPC
	struct mafpc_info *mafpc_info;
#endif
#if defined(CONFIG_PANEL_DISPLAY_MODE)
	struct common_panel_display_modes *common_panel_modes;
#endif
};

enum {
	OVER_ZERO,
	UNDER_ZERO,
	UNDER_MINUS_FIFTEEN,
	DIM_OVER_ZERO,
	DIM_UNDER_ZERO,
	DIM_UNDER_MINUS_FIFTEEN,
	TEMP_MAX,
};

enum {
	POC_ONOFF_OFF,
	POC_ONOFF_ON,
	POC_ONOFF_MAX
};

enum {
	ACL_PWRSAVE_OFF,
	ACL_PWRSAVE_ON,
	MAX_ACL_PWRSAVE,
};

enum {
	SMOOTH_TRANS_OFF,
	SMOOTH_TRANS_ON,
	SMOOTH_TRANS_MAX,
};

enum {
	ACL_OPR_OFF,
	ACL_OPR_03P,
	ACL_OPR_06P,
	ACL_OPR_08P,
	ACL_OPR_12P,
	ACL_OPR_15P,
	ACL_OPR_MAX
};

enum {
	ALPM_OFF = 0,
	ALPM_LOW_BR,
	HLPM_LOW_BR,
	ALPM_HIGH_BR,
	HLPM_HIGH_BR,
};

enum {
	PANEL_HBM_OFF,
	PANEL_HBM_ON,
	MAX_PANEL_HBM,
};

#ifdef CONFIG_SUPPORT_XTALK_MODE
enum {
	XTALK_OFF,
	XTALK_ON,
};
#endif

#ifdef CONFIG_SUPPORT_GRAM_CHECKSUM
enum {
	GRAM_CHKSUM_OFF,
	GRAM_CHKSUM_LV_TEST_1,
	GRAM_CHKSUM_LV_TEST_2,
	GRAM_CHKSUM_HV_TEST_1,
	GRAM_CHKSUM_HV_TEST_2,
	MAX_GRAM_CHKSUM,
};

enum {
	GCT_PATTERN_NONE,
	GCT_PATTERN_1,
	GCT_PATTERN_2,
	MAX_GCT_PATTERN,
};

enum {
	VDDM_ORIG,
	VDDM_LV,
	VDDM_HV,
	MAX_VDDM,
};

enum {
	GRAM_TEST_OFF,
	GRAM_TEST_ON,
	GRAM_TEST_SKIPPED,
};
#endif

#ifdef CONFIG_SUPPORT_ISC_TUNE_TEST
enum stm_field_num {
	STM_CTRL_EN = 0,
	STM_MAX_OPT,
	STM_DEFAULT_OPT,
	STM_DIM_STEP,
	STM_FRAME_PERIOD,
	STM_MIN_SECT,
	STM_PIXEL_PERIOD,
	STM_LINE_PERIOD,
	STM_MIN_MOVE,
	STM_M_THRES,
	STM_V_THRES,
	STM_FIELD_MAX
};
#endif

enum {
	MCD_RS_1_RIGHT,
	MCD_RS_1_LEFT,
	MCD_RS_2_RIGHT,
	MCD_RS_2_LEFT,
	MAX_MCD_RS,
};

enum {
	DIM_TYPE_AID_DIMMING,
	DIM_TYPE_DIM_FLASH,
	MAX_DIM_TYPE,
};

#define MAX_PANEL (32)
#define MAX_PANEL_DDI (8)
#define MAX_PANEL_LUT (128)

enum {
	PANEL_ID,
	PANEL_MASK,
	PANEL_INDEX,
	PANEL_DDI_INDEX,
};

struct panel_lut {
	u32 id;
	u32 mask;
	u32 index;
	u32 ddi_index;
};

struct panel_lut_info {
	const char *names[MAX_PANEL];
	int nr_panel;
	struct device_node *ddi_node[MAX_PANEL_DDI];
	int nr_panel_ddi;
	struct device_node *panel_modes_node[MAX_PANEL_DDI];
	int nr_panel_modes;
	struct panel_lut lut[MAX_PANEL_LUT];
	int nr_lut;
};

struct panel_properties {
	s32 temperature;
	u32 siop_enable;
	u32 adaptive_control;
	u32 alpm_mode;
	u32 cur_alpm_mode;
	s32 lpm_brightness;
	u32 lpm_opr;
	u32 lpm_fps;
	u32 cur_lpm_opr;
	u32 mcd_on;
	u32 mcd_resistance;
	int mcd_rs_range[MAX_MCD_RS][2];
	int mcd_rs_flash_range[MAX_MCD_RS][2];
#ifdef CONFIG_SUPPORT_MST
	u32 mst_on;
#endif
	u32 hmd_on;
	u32 hmd_brightness;
	u32 lux;
#ifdef CONFIG_SUPPORT_XTALK_MODE
	u32 xtalk_mode;
#endif
#ifdef CONFIG_SUPPORT_DDI_FLASH
	u32 poc_op;
#endif
#ifdef CONFIG_SUPPORT_GRAM_CHECKSUM
	u32 gct_on;
	u32 gct_vddm;
	u32 gct_pattern;
	u8 gct_valid_chksum[4];
#endif
#ifdef CONFIG_SUPPORT_GRAYSPOT_TEST
	u32 grayspot;
#endif
	u32 poc_onoff;
#ifdef CONFIG_SUPPORT_TDMB_TUNE
	u32 tdmb_on;
	u32 cur_tdmb_on;
#endif
	u32 key[MAX_CMD_LEVEL];
	u32 irc_mode;
#ifdef CONFIG_SUPPORT_DIM_FLASH
	bool dim_flash_on;
	u32 dim_flash_state;	/* success or fail */
	u32 cur_dim_type;	/* AID DIMMING or DIM FLASH */
#endif
#ifdef CONFIG_SUPPORT_DYNAMIC_HLPM
	u32 dynamic_hlpm;
#endif
#ifdef CONFIG_SUPPORT_ISC_TUNE_TEST
	u8 isc_threshold;
	u8 stm_field_info[STM_FIELD_MAX];
#endif
	u8 *gamma_control_buf;
	u32 panel_partial_disp;
	u32 panel_mode;
	/* resolution */
	bool mres_updated;
	u32 mres_mode;
	u32 xres;
	u32 yres;
	/* variable refresh rate */
	u32 vrr_fps;
	u32 vrr_mode;
	u32 vrr_aid_cycle;
	u32 vrr_idx;
	u32 vrr_origin_fps;
	u32 vrr_origin_mode;
	u32 vrr_origin_aid_cycle;
	u32 vrr_origin_idx;
#ifdef CONFIG_PANEL_VRR_BRIDGE
	bool vrr_bridge_enable;
	u32 vrr_target_fps;
	u32 vrr_target_mode;
	u32 vrr_target_aid_cycle;
	struct panel_vrr_bridge *bridge;
#endif
	bool vrr_updated;
	struct vrr_lfd_info vrr_lfd_info;
	u32 dia_mode;
	u32 ub_con_cnt;
	u32 conn_det_enable;
#if defined(CONFIG_SEC_FACTORY) && defined(CONFIG_SUPPORT_FAST_DISCHARGE)
	u32 enable_fd;
#endif
};

struct panel_info {
	const char *ldi_name;
	unsigned char id[PANEL_ID_LEN];
	unsigned char date[PANEL_DATE_LEN];
	unsigned char coordinate[PANEL_COORD_LEN];
	struct panel_properties props;
	struct ddi_properties ddi_props;
	struct panel_mres mres;

	/* platform dependent data - ex> exynos : dsim_device */
	void *pdata;
	void *dim_info[MAX_PANEL_BL_SUBDEV];
	void *dim_flash_info[MAX_PANEL_BL_SUBDEV];
	struct panel_vrr **vrrtbl;
	int nr_vrrtbl;
	struct maptbl *maptbl;
	int nr_maptbl;
	struct seqinfo *seqtbl;
	int nr_seqtbl;
	struct rdinfo *rditbl;
	int nr_rditbl;
	struct dumpinfo *dumpinfo;
	int nr_dumpinfo;
	struct resinfo *restbl;
	int nr_restbl;
	struct panel_dimming_info *panel_dim_info[MAX_PANEL_BL_SUBDEV];
	int nr_panel_dim_info;
	struct panel_lut_info lut_info;
#if defined(CONFIG_PANEL_DISPLAY_MODE)
	struct common_panel_display_modes *common_panel_modes;
#endif
};

#define __PANEL_ATTR_RO(_name, _mode) __ATTR(_name, _mode,		\
			 PN_CONCAT(_name, show), NULL)

#define __PANEL_ATTR_WO(_name, _mode) __ATTR(_name, _mode,		\
			 NULL, PN_CONCAT(_name, store))

#define __PANEL_ATTR_RW(_name, _mode) __ATTR(_name, _mode,		\
			 PN_CONCAT(_name, show), PN_CONCAT(_name, store))

enum sysfs_arg_type {
	SYSFS_ARG_TYPE_NONE,
	SYSFS_ARG_TYPE_S32,
	SYSFS_ARG_TYPE_U32,
	SYSFS_ARG_TYPE_STR,
	MAX_SYSFS_ARG_TYPE,
};

#define MAX_SYSFS_ARG_NUM		(6)
#define MAX_SYSFS_ARG_STR_LEN	(32)

struct sysfs_arg {
	const char *name;
	unsigned int nargs;
	enum sysfs_arg_type type;
};

struct sysfs_arg_out {
	union {
		s32 val_s32;
		u32 val_u32;
		char val_str[MAX_SYSFS_ARG_STR_LEN];
	} d[MAX_SYSFS_ARG_NUM];
	unsigned int nargs;
};

static inline int search_table_u32(u32 *tbl, u32 sz_tbl, u32 value)
{
	int i;

	if (!tbl)
		return -EINVAL;

	for (i = 0; i < sz_tbl; i++)
		if (tbl[i] == value)
			return i;

	return -1;
}

static inline int search_table(void *tbl, int itemsize, u32 sz_tbl, void *value)
{
	int i;

	if (!tbl)
		return -EINVAL;

	for (i = 0; i < sz_tbl; i++)
		if (!memcmp(tbl + (i * itemsize),
					value, itemsize))
			return i;

	return -1;
}

#define disp_div_round(n, m) ((((n) * 10 / (m)) + 5) / 10)

void print_data(char *data, int size);
void print_maptbl(struct maptbl *tbl);
int register_common_panel(struct common_panel_info *info);
struct common_panel_info *find_panel(struct panel_device *panel, u32 id);
struct device_node *find_panel_ddi_node(struct panel_device *panel, u32 id);
struct device_node *find_panel_modes_node(struct panel_device *panel, u32 id);
void print_panel_lut(struct panel_lut_info *lut_info);
int check_seqtbl_exist(struct panel_info *panel_data, u32 index);
struct seqinfo *find_panel_seqtbl(struct panel_info *panel_data, char *name);
struct seqinfo *find_index_seqtbl(struct panel_info *panel_data, u32 index);
struct pktinfo *find_packet(struct seqinfo *seqtbl, char *name);
struct pktinfo *find_packet_suffix(struct seqinfo *seqtbl, char *name);
struct maptbl *find_panel_maptbl_by_index(struct panel_info *panel, int index);
struct maptbl *find_panel_maptbl_by_name(struct panel_info *panel_data, char *name);
struct panel_dimming_info *find_panel_dimming(struct panel_info *panel_data, char *name);
int excute_seqtbl_nolock(struct panel_device *panel, struct seqinfo *seqtbl, int index);
int panel_do_seqtbl(struct panel_device *panel, struct seqinfo *seqtbl);
int panel_do_seqtbl_by_index_nolock(struct panel_device *panel, int index);
int panel_do_seqtbl_by_index(struct panel_device *panel, int index);
struct resinfo *find_panel_resource(struct panel_info *panel, char *name);
bool panel_resource_initialized(struct panel_info *panel_data, char *name);
int rescpy(u8 *dst, struct resinfo *res, u32 offset, u32 len);
int rescpy_by_name(struct panel_info *panel, u8 *dst, char *name, u32 offset, u32 len);
int resource_clear(struct resinfo *res);
int resource_clear_by_name(struct panel_info *panel_data, char *name);
int resource_copy(u8 *dst, struct resinfo *res);
int resource_copy_by_name(struct panel_info *panel_data, u8 *dst, char *name);
int resource_copy_n_clear_by_name(struct panel_info *panel_data, u8 *dst, char *name);
int get_resource_size_by_name(struct panel_info *panel_data, char *name);
int get_panel_resource_size(struct resinfo *res);
int panel_resource_update(struct panel_device *panel, struct resinfo *res);
int panel_resource_update_by_name(struct panel_device *panel, char *name);
int panel_dumpinfo_update(struct panel_device *panel, struct dumpinfo *info);
int panel_rx_nbytes(struct panel_device *panel, u32 type, u8 *buf, u8 addr, u8 pos, u32 len);
int panel_tx_nbytes(struct panel_device *panel,	u32 type, u8 *buf, u8 addr, u8 pos, u32 len);
u16 calc_checksum_16bit(u8 *arr, int size);

int panel_verify_tx_packet(struct panel_device *panel, u8 *src, u8 ofs, u8 len);

int panel_set_key(struct panel_device *panel, int level, bool on);
struct pktinfo *alloc_static_packet(char *name, u32 type, u8 *data, u32 dlen);
int check_panel_active(struct panel_device *panel, const char *caller);
#if defined(CONFIG_PANEL_DISPLAY_MODE)
bool panel_display_mode_is_supported(struct panel_device *panel);
#endif
int read_panel_id(struct panel_device *panel, u8 *buf);
int panel_resource_prepare(struct panel_device *panel);
void print_panel_resource(struct panel_device *panel);
int find_sysfs_arg_by_name(struct sysfs_arg *arglist, int nr_arglist, char *s);
int parse_sysfs_arg(int nargs, enum sysfs_arg_type type,
		char *s, struct sysfs_arg_out *out);
#ifdef CONFIG_EXYNOS_DECON_LCD_SYSFS
int panel_sysfs_probe(struct panel_device *panel);
#else
static inline int panel_sysfs_probe(struct panel_device *panel) { return 0; }
#endif
#define IS_PANEL_ACTIVE(_panel) check_panel_active(_panel, __func__)
#endif /* __PANEL_H__ */
