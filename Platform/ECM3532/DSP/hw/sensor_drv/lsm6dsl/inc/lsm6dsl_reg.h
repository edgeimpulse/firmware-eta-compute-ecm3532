#ifndef H_LSM6DSL_REG_
#define H_LSM6DSL_REG_

#include "reg.h"

#define LSM6DSL_SLAVE_ADDR  0x6B

typedef enum {
    LSM6DSL_XL_ODR_OFF = 0,
    LSM6DSL_XL_ODR_12Hz5 = 1,
    LSM6DSL_XL_ODR_26Hz = 2,
    LSM6DSL_XL_ODR_52Hz = 3,
    LSM6DSL_XL_ODR_104Hz = 4,
    LSM6DSL_XL_ODR_208Hz = 5,
    LSM6DSL_XL_ODR_416Hz = 6,
    LSM6DSL_XL_ODR_833Hz = 7,
    LSM6DSL_XL_ODR_1k66Hz = 8,
    LSM6DSL_XL_ODR_3k33Hz = 9,
    LSM6DSL_XL_ODR_6k66Hz = 10,
    LSM6DSL_XL_ODR_1Hz6 = 11,
    LSM6DSL_XL_ODR_ND = 12,  /* ERROR CODE */
} lsm6dsl_odr_xl_t;

typedef enum {
    LSM6DSL_GY_ODR_OFF = 0,
    LSM6DSL_GY_ODR_12Hz5 = 1,
    LSM6DSL_GY_ODR_26Hz = 2,
    LSM6DSL_GY_ODR_52Hz = 3,
    LSM6DSL_GY_ODR_104Hz = 4,
    LSM6DSL_GY_ODR_208Hz = 5,
    LSM6DSL_GY_ODR_416Hz = 6,
    LSM6DSL_GY_ODR_833Hz = 7,
    LSM6DSL_GY_ODR_1k66Hz = 8,
    LSM6DSL_GY_ODR_3k33Hz = 9,
    LSM6DSL_GY_ODR_6k66Hz = 10,
    LSM6DSL_GY_ODR_ND = 11,    /* ERROR CODE */
} lsm6dsl_odr_g_t;

typedef enum {
    LSM6DSL_FIFO_DS4_DISABLE = 0,
    LSM6DSL_FIFO_DS4_NO_DEC = 1,
    LSM6DSL_FIFO_DS4_DEC_2 = 2,
    LSM6DSL_FIFO_DS4_DEC_3 = 3,
    LSM6DSL_FIFO_DS4_DEC_4 = 4,
    LSM6DSL_FIFO_DS4_DEC_8 = 5,
    LSM6DSL_FIFO_DS4_DEC_16 = 6,
    LSM6DSL_FIFO_DS4_DEC_32 = 7,
    LSM6DSL_FIFO_DS4_DEC_ND = 8,    /* ERROR CODE */
} lsm6dsl_dec_ds4_fifo_t;

typedef enum {
    LSM6DSL_2g = 0,
    LSM6DSL_16g = 1,
    LSM6DSL_4g = 2,
    LSM6DSL_8g = 3,
    LSM6DSL_XL_FS_ND = 4,  /* ERROR CODE */
} lsm6dsl_fs_xl_t;

typedef enum {
    LSM6DSL_250dps = 0,
    LSM6DSL_125dps = 1,
    LSM6DSL_500dps = 2,
    LSM6DSL_1000dps = 4,
    LSM6DSL_2000dps = 6,
    LSM6DSL_GY_FS_ND = 7,    /* ERROR CODE */
} lsm6dsl_fs_g_t;

typedef enum {
    LSM6DSL_BYPASS_MODE = 0,
    LSM6DSL_FIFO_MODE = 1,
    LSM6DSL_STREAM_TO_FIFO_MODE = 3,
    LSM6DSL_BYPASS_TO_STREAM_MODE = 4,
    LSM6DSL_STREAM_MODE = 6,
    LSM6DSL_FIFO_MODE_ND = 8,    /* ERROR CODE */
} lsm6dsl_fifo_mode_t;

typedef enum {
    LSM6DSL_FIFO_DISABLE = 0,
    LSM6DSL_FIFO_12Hz5 = 1,
    LSM6DSL_FIFO_26Hz = 2,
    LSM6DSL_FIFO_52Hz = 3,
    LSM6DSL_FIFO_104Hz = 4,
    LSM6DSL_FIFO_208Hz = 5,
    LSM6DSL_FIFO_416Hz = 6,
    LSM6DSL_FIFO_833Hz = 7,
    LSM6DSL_FIFO_1k66Hz = 8,
    LSM6DSL_FIFO_3k33Hz = 9,
    LSM6DSL_FIFO_6k66Hz = 10,
    LSM6DSL_FIFO_RATE_ND = 11,    /* ERROR CODE */
} lsm6dsl_odr_fifo_t;

typedef enum {
    LSM6DSL_FF_TSH_156mg = 0,
    LSM6DSL_FF_TSH_219mg = 1,
    LSM6DSL_FF_TSH_250mg = 2,
    LSM6DSL_FF_TSH_312mg = 3,
    LSM6DSL_FF_TSH_344mg = 4,
    LSM6DSL_FF_TSH_406mg = 5,
    LSM6DSL_FF_TSH_469mg = 6,
    LSM6DSL_FF_TSH_500mg = 7,
    LSM6DSL_FF_TSH_ND = 8,    /* ERROR CODE */
} lsm6dsl_ff_ths_t;

typedef enum {
    LSM6DSL_FIFO_XL_DISABLE = 0,
    LSM6DSL_FIFO_XL_NO_DEC = 1,
    LSM6DSL_FIFO_XL_DEC_2 = 2,
    LSM6DSL_FIFO_XL_DEC_3 = 3,
    LSM6DSL_FIFO_XL_DEC_4 = 4,
    LSM6DSL_FIFO_XL_DEC_8 = 5,
    LSM6DSL_FIFO_XL_DEC_16 = 6,
    LSM6DSL_FIFO_XL_DEC_32 = 7,
    LSM6DSL_FIFO_XL_DEC_ND = 8,    /* ERROR CODE */
} lsm6dsl_dec_fifo_xl_t;

typedef enum {
    LSM6DSL_FIFO_GY_DISABLE = 0,
    LSM6DSL_FIFO_GY_NO_DEC = 1,
    LSM6DSL_FIFO_GY_DEC_2 = 2,
    LSM6DSL_FIFO_GY_DEC_3 = 3,
    LSM6DSL_FIFO_GY_DEC_4 = 4,
    LSM6DSL_FIFO_GY_DEC_8 = 5,
    LSM6DSL_FIFO_GY_DEC_16 = 6,
    LSM6DSL_FIFO_GY_DEC_32 = 7,
    LSM6DSL_FIFO_GY_DEC_ND = 8,    /* ERROR CODE */
} lsm6dsl_dec_fifo_gyro_t;

typedef enum {
    LSM6DSL_XL_ANA_BW_1k5Hz = 0,
    LSM6DSL_XL_ANA_BW_400Hz = 1,
    LSM6DSL_XL_ANA_BW_ND = 2,    /* ERROR CODE */
} lsm6dsl_bw0_xl_t;

#ifndef BIT
#define BIT(n) (1 << (n))
#endif
#ifndef ARRAY_SIZE
#define ARRAY_SIZE(a) (sizeof(a)/sizeof((a)[0]))
#endif
#define LSM6DSL_REG_FUNC_CFG_ACCESS			0x01
#define LSM6DSL_MASK_FUNC_CFG_EN			BIT(7)
#define LSM6DSL_SHIFT_FUNC_CFG_EN			7
#define LSM6DSL_MASK_FUNC_CFG_EN_B			BIT(5)
#define LSM6DSL_SHIFT_FUNC_CFG_EN_B			5

#define LSM6DSL_REG_SENSOR_SYNC_TIME_FRAME		0x04
#define LSM6DSL_MASK_SENSOR_SYNC_TIME_FRAME_TPH		(BIT(3) | BIT(2) | \
                             BIT(1) | BIT(0))
#define LSM6DSL_SHIFT_SENSOR_SYNC_TIME_FRAME_TPH	0

#define LSM6DSL_REG_SENSOR_SYNC_RES_RATIO		0x05
#define LSM6DSL_MASK_SENSOR_SYNC_RES_RATIO		(BIT(1) | BIT(0))
#define LSM6DSL_SHIFT_SENSOR_SYNC_RES_RATIO		0

typedef struct {
    uint8_t fth : 8;  /* + FIFO_CTRL2(fth) */
} lsm6dsl_fifo_ctrl1_t;

#define LSM6DSL_REG_FIFO_CTRL1				0x06
#define LSM6DSL_MASK_FIFO_CTRL1_FTH			(BIT(7) | BIT(6) | \
                             BIT(5) | BIT(4) | \
                             BIT(3) | BIT(2) | \
                             BIT(1) | BIT(0))
#define LSM6DSL_SHIFT_FIFO_CTRL1_FTH			0

#define LSM6DSL_REG_FIFO_CTRL2				0x07
typedef struct {
    uint8_t fth : 3;  /* + FIFO_CTRL1(fth) */
    uint8_t fifo_temp_en : 1;
    uint8_t not_used_01 : 2;
    uint8_t  timer_pedo_fifo_drdy : 1;
    uint8_t timer_pedo_fifo_en : 1;
} lsm6dsl_fifo_ctrl2_t;
#define LSM6DSL_MASK_FIFO_CTRL2_TIMER_PEDO_FIFO_EN	BIT(7)
#define LSM6DSL_SHIFT_FIFO_CTRL2_TIMER_PEDO_FIFO_EN	7
#define LSM6DSL_MASK_FIFO_CTRL2_TIMER_PEDO_FIFO_DRDY	BIT(6)
#define LSM6DSL_SHIFT_FIFO_CTRL2_TIMER_PEDO_FIFO_DRDY	6
#define LSM6DSL_MASK_FIFO_CTRL2_FIFO_TEMP_EN		BIT(3)
#define LSM6DSL_SHIFT_FIFO_CTRL2_FIFO_TEMP_EN		3
#define LSM6DSL_MASK_FIFO_CTRL2_FTH			(BIT(2) | BIT(1) | \
                             BIT(0))
#define LSM6DSL_SHIFT_FIFO_CTRL2_FTH			0

#define LSM6DSL_REG_FIFO_CTRL3				0x08
typedef struct {
    uint8_t dec_fifo_xl : 3;
    uint8_t dec_fifo_gyro : 3;
    uint8_t not_used_01 : 2;
} lsm6dsl_fifo_ctrl3_t;
#define LSM6DSL_MASK_FIFO_CTRL3_DEC_FIFO_GYRO		(BIT(5) | BIT(4) | \
                             BIT(3))
#define LSM6DSL_SHIFT_FIFO_CTRL3_DEC_FIFO_GYRO		3
#define LSM6DSL_MASK_FIFO_CTRL3_DEC_FIFO_XL		(BIT(2) | BIT(1) | \
                             BIT(0))
#define LSM6DSL_SHIFT_FIFO_CTRL3_DEC_FIFO_XL		0

#define LSM6DSL_REG_FIFO_CTRL4				0x09
typedef struct {
    uint8_t dec_ds3_fifo : 3;
    uint8_t dec_ds4_fifo : 3;
    uint8_t only_high_data : 1;
    uint8_t stop_on_fth : 1;
} lsm6dsl_fifo_ctrl4_t;

#define LSM6DSL_MASK_FIFO_CTRL4_STOP_ON_FTH		BIT(7)
#define LSM6DSL_SHIFT_FIFO_CTRL4_STOP_ON_FTH		7
#define LSM6DSL_MASK_FIFO_CTRL4_ONLY_HIGH_DATA		BIT(6)
#define LSM6DSL_SHIFT_FIFO_CTRL4_ONLY_HIGH_DATA		6
#define LSM6DSL_MASK_FIFO_CTRL4_DEC_DS4_FIFO		(BIT(5) | BIT(4) | \
                             BIT(3))
#define LSM6DSL_SHIFT_FIFO_CTRL4_DEC_DS4_FIFO		3
#define LSM6DSL_MASK_FIFO_CTRL4_DEC_DS3_FIFO		(BIT(2) | BIT(1) | \
                             BIT(0))
#define LSM6DSL_SHIFT_FIFO_CTRL4_DEC_DS3_FIFO		0

#define LSM6DSL_REG_FIFO_CTRL5				0x0A
typedef struct {
    uint8_t fifo_mode : 3;
    uint8_t odr_fifo : 4;
    uint8_t not_used_01 : 1;
} lsm6dsl_fifo_ctrl5_t;
#define LSM6DSL_MASK_FIFO_CTRL5_ODR_FIFO		(BIT(5) | BIT(4) | \
                             BIT(3))
#define LSM6DSL_SHIFT_FIFO_CTRL5_ODR_FIFO		3
#define LSM6DSL_MASK_FIFO_CTRL5_FIFO_MODE		(BIT(2) | BIT(1) | \
                             BIT(0))
#define LSM6DSL_SHIFT_FIFO_CTRL5_FIFO_MODE		0

#define LSM6DSL_REG_DRDY_PULSE_CFG_G			0x0B
#define LSM6DSL_MASK_DRDY_PULSE_CFG_G_DRDY_PULSED	BIT(7)
#define LSM6DSL_SHIFT_DRDY_PULSE_CFG_G_DRDY_PULSED	7
#define LSM6DSL_MASK_DRDY_PULSE_CFG_G_INT2_WRIST_TILT	BIT(0)
#define LSM6DSL_SHIFT_DRDY_PULSE_CFG_G_INT2_WRIST_TILT	0

#define LSM6DSL_REG_INT1_CTRL				0x0D
typedef struct {
    uint8_t int1_drdy_xl : 1;
    uint8_t int1_drdy_g : 1;
    uint8_t int1_boot : 1;
    uint8_t int1_fth : 1;
    uint8_t int1_fifo_ovr : 1;
    uint8_t int1_full_flag : 1;
    uint8_t int1_sign_mot : 1;
    uint8_t int1_step_detector : 1;
} lsm6dsl_int1_ctrl_t;

#define LSM6DSL_MASK_INT1_CTRL_STEP_DETECTOR		BIT(7)
#define LSM6DSL_SHIFT_INT1_CTRL_STEP_DETECTOR		7
#define LSM6DSL_MASK_INT1_CTRL_SIGN_MOT			BIT(6)
#define LSM6DSL_SHIFT_INT1_CTRL_SIGN_MOT		6
#define LSM6DSL_MASK_INT1_CTRL_FULL_FLAG		BIT(5)
#define LSM6DSL_SHIFT_INT1_CTRL_FULL_FLAG		5
#define LSM6DSL_MASK_INT1_CTRL_FIFO_OVR			BIT(4)
#define LSM6DSL_SHIFT_INT1_CTRL_FIFO_OVR		4
#define LSM6DSL_MASK_INT1_FTH				BIT(3)
#define LSM6DSL_SHIFT_INT1_FTH				3
#define LSM6DSL_MASK_INT1_CTRL_BOOT			BIT(2)
#define LSM6DSL_SHIFT_INT1_CTRL_BOOT			2
#define LSM6DSL_MASK_INT1_CTRL_DRDY_G			BIT(1)
#define LSM6DSL_SHIFT_INT1_CTRL_DRDY_G			1
#define LSM6DSL_MASK_INT1_CTRL_DRDY_XL			BIT(0)
#define LSM6DSL_SHIFT_INT1_CTRL_DRDY_XL			0

#define LSM6DSL_REG_INT2_CTRL				0x0E
typedef struct {
    uint8_t int2_drdy_xl : 1;
    uint8_t int2_drdy_g : 1;
    uint8_t int2_drdy_temp : 1;
    uint8_t int2_fth : 1;
    uint8_t int2_fifo_ovr : 1;
    uint8_t int2_full_flag : 1;
    uint8_t int2_step_count_ov : 1;
    uint8_t int2_step_delta : 1;
} lsm6dsl_int2_ctrl_t;

#define LSM6DSL_MASK_INT2_CTRL_STEP_DELTA		BIT(7)
#define LSM6DSL_SHIFT_INT2_CTRL_STEP_DELTA		7
#define LSM6DSL_MASK_INT2_CTRL_STEP_COUNT_OV		BIT(6)
#define LSM6DSL_SHIFT_INT2_CTRL_STEP_COUNT_OV		6
#define LSM6DSL_MASK_INT2_CTRL_FULL_FLAG		BIT(5)
#define LSM6DSL_SHIFT_INT2_CTRL_FULL_FLAG		5
#define LSM6DSL_MASK_INT2_CTRL_FIFO_OVR			BIT(4)
#define LSM6DSL_SHIFT_INT2_CTRL_FIFO_OVR		4
#define LSM6DSL_MASK_INT2_FTH				BIT(3)
#define LSM6DSL_SHIFT_INT2_FTH				3
#define LSM6DSL_MASK_INT2_DRDY_TEMP			BIT(2)
#define LSM6DSL_SHIFT_INT2_DRDY_TEMP			2
#define LSM6DSL_MASK_INT2_CTRL_DRDY_G			BIT(1)
#define LSM6DSL_SHIFT_INT2_CTRL_DRDY_G			1
#define LSM6DSL_MASK_INT2_CTRL_DRDY_XL			BIT(0)
#define LSM6DSL_SHIFT_INT2_CTRL_DRDY_XL			0

#define LSM6DSL_REG_WHO_AM_I				0x0F
#define LSM6DSL_VAL_WHO_AM_I				0x6A

#define LSM6DSL_REG_CTRL1_XL				0x10
#define LSM6DSL_REG_CTRL1_XL_MASK 0x1
typedef struct {
    uint8_t bw0_xl : 1;
    uint8_t lpf1_bw_sel : 1;
    uint8_t fs_xl : 2;
    uint8_t odr_xl : 4;
} lsm6dsl_ctrl1_xl_t;

#define LSM6DSL_MASK_CTRL1_XL_ODR_XL			(BIT(7) | BIT(6) | \
                             BIT(5) | BIT(4))
#define LSM6DSL_SHIFT_CTRL1_XL_ODR_XL			4
#define LSM6DSL_MASK_CTRL1_XL_FS_XL			(BIT(3) | BIT(2))
#define LSM6DSL_SHIFT_CTRL1_XL_FS_XL			2
#define LSM6DSL_MASK_CTRL1_XL_LPF1_BW_SEL		BIT(1)
#define LSM6DSL_SHIFT_CTRL1_XL_LPF1_BW_SEL		1

#define LSM6DSL_REG_CTRL2_G				0x11
typedef struct {
    uint8_t not_used_01 : 1;
    uint8_t fs_g : 3;  /* fs_g + fs_125 */
    uint8_t odr_g : 4;
} lsm6dsl_ctrl2_g_t;

#define LSM6DSL_MASK_CTRL2_G_ODR_G			(BIT(7) | BIT(6) | \
                             BIT(5) | BIT(4))
#define LSM6DSL_SHIFT_CTRL2_G_ODR_G			4
#define LSM6DSL_MASK_CTRL2_G_FS_G			(BIT(3) | BIT(2))
#define LSM6DSL_SHIFT_CTRL2_G_FS_G			2
#define LSM6DSL_MASK_CTRL2_FS125			BIT(1)
#define LSM6DSL_SHIFT_CTRL2_FS125			1

#define LSM6DSL_REG_CTRL3_C				0x12
typedef struct {
    uint8_t sw_reset : 1;
    uint8_t ble : 1;
    uint8_t if_inc : 1;
    uint8_t sim : 1;
    uint8_t pp_od : 1;
    uint8_t h_lactive : 1;
    uint8_t bdu : 1;
    uint8_t boot : 1;
} lsm6dsl_ctrl3_c_t;

#define LSM6DSL_MASK_CTRL3_C_BOOT			BIT(7)
#define LSM6DSL_SHIFT_CTRL3_C_BOOT			7
#define LSM6DSL_MASK_CTRL3_C_BDU			BIT(6)
#define LSM6DSL_SHIFT_CTRL3_C_BDU			6
#define LSM6DSL_MASK_CTRL3_C_H_LACTIVE			BIT(5)
#define LSM6DSL_SHIFT_CTRL3_C_H_LACTIVE			5
#define LSM6DSL_MASK_CTRL3_C_PP_OD			BIT(4)
#define LSM6DSL_SHIFT_CTRL3_C_PP_OD			4
#define LSM6DSL_MASK_CTRL3_C_SIM			BIT(3)
#define LSM6DSL_SHIFT_CTRL3_C_SIM			3
#define LSM6DSL_MASK_CTRL3_C_IF_INC			BIT(2)
#define LSM6DSL_SHIFT_CTRL3_C_IF_INC			2
#define LSM6DSL_MASK_CTRL3_C_BLE			BIT(1)
#define LSM6DSL_SHIFT_CTRL3_C_BLE			1
#define LSM6DSL_MASK_CTRL3_C_SW_RESET			BIT(0)
#define LSM6DSL_SHIFT_CTRL3_C_SW_RESET			0

#define LSM6DSL_REG_CTRL4_C				0x13
typedef struct {
    uint8_t not_used_01 : 1;
    uint8_t lpf1_sel_g : 1;
    uint8_t i2c_disable : 1;
    uint8_t drdy_mask : 1;
    uint8_t den_drdy_int1 : 1;
    uint8_t int2_on_int1 : 1;
    uint8_t sleep : 1;
    uint8_t den_xl_en : 1;
} lsm6dsl_ctrl4_c_t;

#define LSM6DSL_MASK_CTRL4_C_DEN_XL_EN			BIT(7)
#define LSM6DSL_SHIFT_CTRL4_C_DEN_XL_EN			7
#define LSM6DSL_MASK_CTRL4_C_SLEEP			BIT(6)
#define LSM6DSL_SHIFT_CTRL4_C_SLEEP			6
#define LSM6DSL_MASK_CTRL4_C_INT2_ON_INT1		BIT(5)
#define LSM6DSL_SHIFT_CTRL4_C_INT2_ON_INT1		5
#define LSM6DSL_MASK_CTRL4_C_DEN_DRDY_INT1		BIT(4)
#define LSM6DSL_SHIFT_CTRL4_C_DEN_DRDY_INT1		4
#define LSM6DSL_MASK_CTRL4_C_DRDY_MASK			BIT(3)
#define LSM6DSL_SHIFT_CTRL4_C_DRDY_MASK			3
#define LSM6DSL_MASK_CTRL4_C_I2C_DISABLE		BIT(2)
#define LSM6DSL_SHIFT_CTRL4_C_I2C_DISABLE		2
#define LSM6DSL_MASK_CTRL4_C_LPF1_SEL_G			BIT(1)
#define LSM6DSL_SHIFT_CTRL4_C_LPF1_SEL_G		1

#define LSM6DSL_REG_CTRL5_C				0x14
typedef struct {
    uint8_t st_xl : 2;
    uint8_t st_g : 2;
    uint8_t den_lh : 1;
    uint8_t rounding : 3;
} lsm6dsl_ctrl5_c_t;

#define LSM6DSL_MASK_CTRL5_C_ROUNDING			(BIT(7) | BIT(6) \
                             BIT(5))
#define LSM6DSL_SHIFT_CTRL5_C_ROUNDING			5
#define LSM6DSL_MASK_CTRL5_C_DEN_LH			BIT(4)
#define LSM6DSL_SHIFT_CTRL5_C_DEN_LH			4
#define LSM6DSL_MASK_CTRL5_C_ST_G			(BIT(3) | BIT(2))
#define LSM6DSL_SHIFT_CTRL5_C_ST_G			2
#define LSM6DSL_MASK_CTRL5_C_ST_XL			(BIT(1) | BIT(0))
#define LSM6DSL_SHIFT_CTRL5_C_ST_XL			0

#define LSM6DSL_REG_CTRL6_C				0x15
typedef struct {
    uint8_t ftype : 2;
    uint8_t not_used_01 : 1;
    uint8_t usr_off_w : 1;
    uint8_t xl_hm_mode : 1;
    uint8_t den_mode : 3;  /* trig_en + lvl_en + lvl2_en */
} lsm6dsl_ctrl6_c_t;

#define LSM6DSL_MASK_CTRL6_C_TRIG_EN			BIT(7)
#define LSM6DSL_SHIFT_CTRL6_C_TRIG_EN			7
#define LSM6DSL_MASK_CTRL6_C_LVL_EN			BIT(6)
#define LSM6DSL_SHIFT_CTRL6_C_LVL_EN			6
#define LSM6DSL_MASK_CTRL6_C_LVL2_EN			BIT(5)
#define LSM6DSL_SHIFT_CTRL6_C_LVL2_EN			5
#define LSM6DSL_MASK_CTRL6_C_XL_HM_MODE			BIT(4)
#define LSM6DSL_SHIFT_CTRL6_C_XL_HM_MODE		4
#define LSM6DSL_MASK_CTRL6_C_USR_OFF_W			BIT(3)
#define LSM6DSL_SHIFT_CTRL6_C_USR_OFF_W			3
#define LSM6DSL_MASK_CTRL6_C_FTYPE			(BIT(0) | BIT(1))
#define LSM6DSL_SHIFT_CTRL6_C_FTYPE			0

#define LSM6DSL_REG_CTRL7_G				0x16
typedef struct {
    uint8_t not_used_01 : 2;
    uint8_t rounding_status : 1;
    uint8_t not_used_02 : 1;
    uint8_t hpm_g : 2;
    uint8_t hp_en_g : 1;
    uint8_t g_hm_mode : 1;
} lsm6dsl_ctrl7_g_t;

#define LSM6DSL_MASK_CTRL7_G_HM_MODE			BIT(7)
#define LSM6DSL_SHIFT_CTRL7_G_HM_MODE			7
#define LSM6DSL_MASK_CTRL7_HP_EN_G			BIT(6)
#define LSM6DSL_SHIFT_CTRL7_HP_EN_G			6
#define LSM6DSL_MASK_CTRL7_HPM_G			(BIT(5) | BIT(4))
#define LSM6DSL_SHIFT_CTRL7_HPM_G			4
#define LSM6DSL_MASK_CTRL7_ROUNDING_STATUS		BIT(2)
#define LSM6DSL_SHIFT_CTRL7_ROUNDING_STATUS		2

#define LSM6DSL_REG_CTRL8_XL				0x17
typedef struct {
    uint8_t low_pass_on_6d : 1;
    uint8_t not_used_01 : 1;
    uint8_t hp_slope_xl_en : 1;
    uint8_t input_composite : 1;
    uint8_t hp_ref_mode : 1;
    uint8_t hpcf_xl : 2;
    uint8_t lpf2_xl_en : 1;
} lsm6dsl_ctrl8_xl_t;

#define LSM6DSL_MASK_CTRL8_LPF2_XL_EN			BIT(7)
#define LSM6DSL_SHIFT_CTRL8_LPF2_XL_EN			7
#define LSM6DSL_MASK_CTRL8_HPCF_XL			(BIT(6) | BIT(5))
#define LSM6DSL_SHIFT_CTRL8_HPCF_XL			5
#define LSM6DSL_MASK_CTRL8_HP_REF_MODE			BIT(4)
#define LSM6DSL_SHIFT_CTRL8_HP_REF_MODE			4
#define LSM6DSL_MASK_CTRL8_INPUT_COMPOSITE		BIT(3)
#define LSM6DSL_SHIFT_CTRL8_INPUT_COMPOSITE		3
#define LSM6DSL_MASK_CTRL8_HP_SLOPE_XL_EN		BIT(2)
#define LSM6DSL_SHIFT_CTRL8_HP_SLOPE_XL_EN		2
#define LSM6DSL_MASK_CTRL8_LOW_PASS_ON_6D		BIT(0)
#define LSM6DSL_SHIFT_CTRL8_LOW_PASS_ON_6D		0

#define LSM6DSL_REG_CTRL9_XL				0x18
typedef struct {
    uint8_t not_used_01 : 2;
    uint8_t soft_en : 1;
    uint8_t not_used_02 : 1;
    uint8_t den_xl_g : 1;
    uint8_t den_z : 1;
    uint8_t den_y : 1;
    uint8_t den_x : 1;
} lsm6dsl_ctrl9_xl_t;

#define LSM6DSL_MASK_CTRL9_XL_DEN_X			BIT(7)
#define LSM6DSL_SHIFT_CTRL9_XL_DEN_X			7
#define LSM6DSL_MASK_CTRL9_XL_DEN_Y			BIT(6)
#define LSM6DSL_SHIFT_CTRL9_XL_DEN_Y			6
#define LSM6DSL_MASK_CTRL9_XL_DEN_Z			BIT(5)
#define LSM6DSL_SHIFT_CTRL9_XL_DEN_Z			5
#define LSM6DSL_MASK_CTRL9_XL_DEN_G			BIT(4)
#define LSM6DSL_SHIFT_CTRL9_XL_DEN_G			4
#define LSM6DSL_MASK_CTRL9_XL_SOFT_EN			BIT(2)
#define LSM6DSL_SHIFT_CTRL9_XL_SOFT_EN			2

#define LSM6DSL_REG_CTRL10_C				0x19
typedef struct {
    uint8_t sign_motion_en : 1;
    uint8_t pedo_rst_step : 1;
    uint8_t func_en : 1;
    uint8_t tilt_en : 1;
    uint8_t pedo_en : 1;
    uint8_t timer_en : 1;
    uint8_t not_used_01 : 1;
    uint8_t wrist_tilt_en : 1;
} lsm6dsl_ctrl10_c_t;

#define LSM6DSL_MASK_CTRL10_C_WRIST_TILT_EN		BIT(7)
#define LSM6DSL_SHIFT_CTRL10_C_WRIST_TILT_EN		7
#define LSM6DSL_MASK_CTRL10_C_TIMER_EN			BIT(5)
#define LSM6DSL_SHIFT_CTRL10_C_TIMER_EN			5
#define LSM6DSL_MASK_CTRL10_C_PEDO_EN			BIT(4)
#define LSM6DSL_SHIFT_CTRL10_C_PEDO_EN			4
#define LSM6DSL_MASK_CTRL10_C_TILT_EN			BIT(3)
#define LSM6DSL_SHIFT_CTRL10_C_TILT_EN			3
#define LSM6DSL_MASK_CTRL10_C_FUNC_EN			BIT(2)
#define LSM6DSL_SHIFT_CTRL10_C_FUNC_EN			2
#define LSM6DSL_MASK_CTRL10_C_PEDO_RST_STEP		BIT(1)
#define LSM6DSL_SHIFT_CTRL10_C_PEDO_RST_STEP		1
#define LSM6DSL_MASK_CTRL10_C_SIGN_MOTION_EN		BIT(0)
#define LSM6DSL_SHIFT_CTRL10_C_SIGN_MOTION_EN		0

#define LSM6DSL_REG_MASTER_CONFIG			0x1A
typedef struct {
    uint8_t master_on : 1;
    uint8_t iron_en : 1;
    uint8_t pass_through_mode : 1;
    uint8_t pull_up_en : 1;
    uint8_t start_config : 1;
    uint8_t not_used_01 : 1;
    uint8_t  data_valid_sel_fifo : 1;
    uint8_t drdy_on_int1 : 1;
} lsm6dsl_master_config_t;

#define LSM6DSL_MASK_MASTER_CONFIG_DRDY_ON_INT1		BIT(7)
#define LSM6DSL_SHIFT_MASTER_CONFIG_DRDY_ON_INT1	7
#define LSM6DSL_MASK_MASTER_CONFIG_DATA_VALID_SEL_FIFO	BIT(6)
#define LSM6DSL_SHIFT_MASTER_CONFIG_DATA_VALID_SEL_FIFO	6
#define LSM6DSL_MASK_MASTER_CONFIG_START_CONFIG		BIT(4)
#define LSM6DSL_SHIFT_MASTER_CONFIG_START_CONFIG	4
#define LSM6DSL_MASK_MASTER_CONFIG_PULL_UP_EN		BIT(3)
#define LSM6DSL_SHIFT_MASTER_CONFIG_PULL_UP_EN		3
#define LSM6DSL_MASK_MASTER_CONFIG_PASS_THROUGH_MODE	BIT(2)
#define LSM6DSL_SHIFT_MASTER_CONFIG_PASS_THROUGH_MODE	2
#define LSM6DSL_MASK_MASTER_CONFIG_IRON_EN		BIT(1)
#define LSM6DSL_SHIFT_MASTER_CONFIG_IRON_EN		1
#define LSM6DSL_MASK_MASTER_CONFIG_MASTER_ON		BIT(0)
#define LSM6DSL_SHIFT_MASTER_CONFIG_MASTER_ON		0

#define LSM6DSL_REG_WAKE_UP_SRC				0x1B
#define LSM6DSL_MASK_WAKE_UP_SRC_FF_IA			BIT(5)
#define LSM6DSL_SHIFT_WAKE_UP_SRC_FF_IA			5
#define LSM6DSL_MASK_WAKE_UP_SRC_SLEEP_STATE_IA		BIT(4)
#define LSM6DSL_SHIFT_WAKE_UP_SRC_SLEEP_STATE_IA	4
#define LSM6DSL_MASK_WAKE_UP_SRC_WU_IA			BIT(3)
#define LSM6DSL_SHIFT_WAKE_UP_SRC_WU_IA			3
#define LSM6DSL_MASK_WAKE_UP_SRC_X_WU			BIT(2)
#define LSM6DSL_SHIFT_WAKE_UP_SRC_X_WU			2
#define LSM6DSL_MASK_WAKE_UP_SRC_Y_WU			BIT(1)
#define LSM6DSL_SHIFT_WAKE_UP_SRC_Y_WU			1
#define LSM6DSL_MASK_WAKE_UP_SRC_Z_WU			BIT(0)
#define LSM6DSL_SHIFT_WAKE_UP_SRC_Z_WU			0

#define LSM6DSL_REG_TAP_SRC				0x1C
#define LSM6DSL_MASK_TAP_SRC_TAP_IA			BIT(6)
#define LSM6DSL_SHIFT_TAP_SRC_TAP_IA			6
#define LSM6DSL_MASK_TAP_SRC_SINGLE_TAP			BIT(5)
#define LSM6DSL_SHIFT_TAP_SRC_SINGLE_TAP		5
#define LSM6DSL_MASK_TAP_SRC_DOUBLE_TAP			BIT(4)
#define LSM6DSL_SHIFT_TAP_SRC_DOUBLE_TAP		4
#define LSM6DSL_MASK_TAP_SRC_TAP_SIGN			BIT(3)
#define LSM6DSL_SHIFT_TAP_SRC_TAP_SIGN			3
#define LSM6DSL_MASK_TAP_SRC_X_TAP			BIT(2)
#define LSM6DSL_SHIFT_TAP_SRC_X_TAP			2
#define LSM6DSL_MASK_TAP_SRC_Y_TAP			BIT(1)
#define LSM6DSL_SHIFT_TAP_SRC_Y_TAP			1
#define LSM6DSL_MASK_TAP_SRC_Z_TAP			BIT(0)
#define LSM6DSL_SHIFT_TAP_SRC_Z_TAP			0

#define LSM6DSL_REG_D6D_SRC				0x1D
#define LSM6DSL_MASK_D6D_SRC_DEN_DRDY			BIT(7)
#define LSM6DSL_SHIFT_D6D_SRC_DEN_DRDY			7
#define LSM6DSL_MASK_D6D_SRC_D6D_IA			BIT(6)
#define LSM6DSL_SHIFT_D6D_SRC_D6D_IA			6
#define LSM6DSL_MASK_D6D_SRC_ZH				BIT(5)
#define LSM6DSL_SHIFT_D6D_SRC_ZH			5
#define LSM6DSL_MASK_D6D_SRC_ZL				BIT(4)
#define LSM6DSL_SHIFT_D6D_SRC_ZL			4
#define LSM6DSL_MASK_D6D_SRC_YH				BIT(3)
#define LSM6DSL_SHIFT_D6D_SRC_YH			3
#define LSM6DSL_MASK_D6D_SRC_YL				BIT(2)
#define LSM6DSL_SHIFT_D6D_SRC_YL			2
#define LSM6DSL_MASK_D6D_SRC_XH				BIT(1)
#define LSM6DSL_SHIFT_D6D_SRC_XH			1
#define LSM6DSL_MASK_D6D_SRC_XL				BIT(0)
#define LSM6DSL_SHIFT_D6D_SRC_XL			0

#define LSM6DSL_REG_STATUS_REG				0x1E
typedef struct {
    uint8_t xlda : 1;
    uint8_t gda : 1;
    uint8_t tda : 1;
    uint8_t not_used_01 : 5;
} lsm6dsl_status_reg_t;

#define LSM6DSL_MASK_STATUS_REG_TDA			BIT(2)
#define LSM6DSL_SHIFT_STATUS_REG_TDA			2
#define LSM6DSL_MASK_STATUS_REG_GDA			BIT(1)
#define LSM6DSL_SHIFT_STATUS_REG_GDA			1
#define LSM6DSL_MASK_STATUS_REG_XLDA			BIT(0)
#define LSM6DSL_SHIFT_STATUS_REG_XLDA			0

#define LSM6DSL_REG_OUT_TEMP_L				0x20
#define LSM6DSL_REG_OUT_TEMP_H				0x21
#define LSM6DSL_REG_OUTX_L_G				0x22
#define LSM6DSL_REG_OUTX_H_G				0x23
#define LSM6DSL_REG_OUTY_L_G				0x24
#define LSM6DSL_REG_OUTY_H_G				0x25
#define LSM6DSL_REG_OUTZ_L_G				0x26
#define LSM6DSL_REG_OUTZ_H_G				0x27
#define LSM6DSL_REG_OUTX_L_XL				0x28
#define LSM6DSL_REG_OUTX_H_XL				0x29
#define LSM6DSL_REG_OUTY_L_XL				0x2A
#define LSM6DSL_REG_OUTY_H_XL				0x2B
#define LSM6DSL_REG_OUTZ_L_XL				0x2C
#define LSM6DSL_REG_OUTZ_H_XL				0x2D
#define LSM6DSL_REG_SENSORHUB1				0x2E
#define LSM6DSL_REG_SENSORHUB2				0x2F
#define LSM6DSL_REG_SENSORHUB3				0x30
#define LSM6DSL_REG_SENSORHUB4				0x31
#define LSM6DSL_REG_SENSORHUB5				0x32
#define LSM6DSL_REG_SENSORHUB6				0x33
#define LSM6DSL_REG_SENSORHUB7				0x34
#define LSM6DSL_REG_SENSORHUB8				0x35
#define LSM6DSL_REG_SENSORHUB9				0x36
#define LSM6DSL_REG_SENSORHUB10				0x37
#define LSM6DSL_REG_SENSORHUB11				0x38
#define LSM6DSL_REG_SENSORHUB12				0x39


#define LSM6DSL_REG_FIFO_STATUS1			0x3A
typedef struct {
    uint8_t diff_fifo : 8;  /* + FIFO_STATUS2(diff_fifo) */
} lsm6dsl_fifo_status1_t;

#define LSM6DSL_REG_FIFO_STATUS2			0x3B
typedef struct {
    uint8_t diff_fifo : 3;  /* + FIFO_STATUS1(diff_fifo) */
    uint8_t not_used_01 : 1;
    uint8_t fifo_empty : 1;
    uint8_t fifo_full_smart : 1;
    uint8_t over_run : 1;
    uint8_t waterm : 1;
} lsm6dsl_fifo_status2_t;

#define LSM6DSL_MASK_FIFO_STATUS2_WATERM		BIT(7)
#define LSM6DSL_SHIFT_FIFO_STATUS2_WATERM		7
#define LSM6DSL_MASK_FIFO_STATUS2_OVER_RUN		BIT(6)
#define LSM6DSL_SHIFT_FIFO_STATUS2_OVER_RUN		6
#define LSM6DSL_MASK_FIFO_STATUS2_FIFO_FULL_SMART	BIT(5)
#define LSM6DSL_SHIFT_FIFO_STATUS2_FIFO_FULL_SMART	5
#define LSM6DSL_MASK_FIFO_STATUS2_FIFO_EMPTY		BIT(4)
#define LSM6DSL_SHIFT_FIFO_STATUS2_FIFO_EMPTY		4
#define LSM6DSL_MASK_FIFO_STATUS2_DIFF_FIFO		(BIT(2) | BIT(1) | \
                             BIT(0))
#define LSM6DSL_SHIFT_FIFO_STATUS2_DIFF_FIFO		0

#define LSM6DSL_REG_FIFO_STATUS3			0x3C
typedef struct {
    uint8_t fifo_pattern : 8;  /* + FIFO_STATUS4(fifo_pattern) */
} lsm6dsl_fifo_status3_t;

#define LSM6DSL_MASK_FIFO_STATUS3_FIFO_PATTERN		0x0F
#define LSM6DSL_SHIFT_FIFO_STATUS3_FIFO_PATTERN		0

#define LSM6DSL_REG_FIFO_STATUS4			0x3C

typedef struct {
    uint8_t fifo_pattern : 2;  /* + FIFO_STATUS3(fifo_pattern) */
    uint8_t not_used_01 : 6;
} lsm6dsl_fifo_status4_t;

#define LSM6DSL_MASK_FIFO_STATUS4_FIFO_PATTERN		(BIT(1) | BIT(0))
#define LSM6DSL_SHIFT_FIFO_STATUS4_FIFO_PATTERN		0

#define LSM6DSL_REG_FIFO_DATA_OUT_L			0x3E
#define LSM6DSL_REG_FIFO_DATA_OUT_H			0x3F
#define LSM6DSL_REG_TIMESTAMP0				0x40
#define LSM6DSL_REG_TIMESTAMP1				0x41
#define LSM6DSL_REG_TIMESTAMP2				0x42
#define LSM6DSL_REG_STEP_TIMESTAMP_L			0x49
#define LSM6DSL_REG_STEP_TIMESTAMP_H			0x4A
#define LSM6DSL_REG_STEP_COUNTER_L			0x4B
#define LSM6DSL_REG_STEP_COUNTER_H			0x4C
#define LSM6DSL_REG_SENSORHUB13				0x4D
#define LSM6DSL_REG_SENSORHUB14				0x4E
#define LSM6DSL_REG_SENSORHUB15				0x4F
#define LSM6DSL_REG_SENSORHUB16				0x50
#define LSM6DSL_REG_SENSORHUB17				0x51
#define LSM6DSL_REG_SENSORHUB18				0x52

#define LSM6DSL_REG_FUNC_SRC1				0x53
#define LSM6DSL_MASK_FUNC_SRC1_STEP_COUNT_DELTA_IA	BIT(7)
#define LSM6DSL_SHIFT_FUNC_SRC1_STEP_COUNT_DELTA_IA	7
#define LSM6DSL_MASK_FUNC_SRC1_SIGN_MOTION_IA		BIT(6)
#define LSM6DSL_SHIFT_FUNC_SRC1_SIGN_MOTION_IA		6
#define LSM6DSL_MASK_FUNC_SRC1_TILT_IA			BIT(5)
#define LSM6DSL_SHIFT_FUNC_SRC1_TILT_IA			5
#define LSM6DSL_MASK_FUNC_SRC1_STEP_DETECTED		BIT(4)
#define LSM6DSL_SHIFT_FUNC_SRC1_STEP_DETECTED		4
#define LSM6DSL_MASK_FUNC_SRC1_STEP_OVERFLOW		BIT(3)
#define LSM6DSL_SHIFT_FUNC_SRC1_STEP_OVERFLOW		3
#define LSM6DSL_MASK_FUNC_SRC1_HI_FAIL			BIT(2)
#define LSM6DSL_SHIFT_FUNC_SRC1_HI_FAIL			2
#define LSM6DSL_MASK_FUNC_SRC1_SI_SEND_OP		BIT(1)
#define LSM6DSL_SHIFT_FUNC_SRC1_SI_SEND_OP		1
#define LSM6DSL_MASK_FUNC_SRC1_SENSORHUB_END_OP		BIT(0)
#define LSM6DSL_SHIFT_FUNC_SRC1_SENSORHUB_END_OP	0

#define LSM6DSL_REG_FUNC_SRC2				0x54
#define LSM6DSL_MASK_FUNC_SRC2_SLAVE3_NACK		BIT(6)
#define LSM6DSL_SHIFT_FUNC_SRC2_SLAVE3_NACK		6
#define LSM6DSL_MASK_FUNC_SRC2_SLAVE2_NACK		BIT(5)
#define LSM6DSL_SHIFT_FUNC_SRC2_SLAVE2_NACK		5
#define LSM6DSL_MASK_FUNC_SRC2_SLAVE1_NACK		BIT(4)
#define LSM6DSL_SHIFT_FUNC_SRC2_SLAVE1_NACK		4
#define LSM6DSL_MASK_FUNC_SRC2_SLAVE0_NACK		BIT(3)
#define LSM6DSL_SHIFT_FUNC_SRC2_SLAVE0_NACK		3
#define LSM6DSL_MASK_FUNC_SRC2_WRIST_TILT_IA		BIT(0)
#define LSM6DSL_SHIFT_FUNC_SRC2_WRIST_TILT_IA		0

#define LSM6DSL_REG_WRIST_TILT_IA			0x55
#define LSM6DSL_MASK_WRIST_TILT_IA_XPOS			BIT(7)
#define LSM6DSL_SHIFT_WRIST_TILT_IA_XPOS		7
#define LSM6DSL_MASK_WRIST_TILT_IA_XNEG			BIT(6)
#define LSM6DSL_SHIFT_WRIST_TILT_IA_XNEG		6
#define LSM6DSL_MASK_WRIST_TILT_IA_YPOS			BIT(5)
#define LSM6DSL_SHIFT_WRIST_TILT_IA_YPOS		5
#define LSM6DSL_MASK_WRIST_TILT_IA_YNEG			BIT(4)
#define LSM6DSL_SHIFT_WRIST_TILT_IA_YNEG		4
#define LSM6DSL_MASK_WRIST_TILT_IA_ZPOS			BIT(3)
#define LSM6DSL_SHIFT_WRIST_TILT_IA_ZPOS		3
#define LSM6DSL_MASK_WRIST_TILT_IA_ZNEG			BIT(2)
#define LSM6DSL_SHIFT_WRIST_TILT_IA_ZNEG		2

#define LSM6DSL_REG_TAP_CFG				0x58
#define LSM6DSL_MASK_TAP_CFG_INTERRPUTS_ENABLE		BIT(7)
#define LSM6DSL_SHIFT_TAP_CFG_INTERRPUTS_ENABLE		7
#define LSM6DSL_MASK_TAP_CFG_INACT_EN			(BIT(6) | BIT(5))
#define LSM6DSL_SHIFT_TAP_CFG_INACT_EN			5
#define LSM6DSL_MASK_TAP_CFG_SLOPE_FDS			BIT(4)
#define LSM6DSL_SHIFT_TAP_CFG_SLOPE_FDS			4
#define LSM6DSL_MASK_TAP_CFG_X_EN			BIT(3)
#define LSM6DSL_SHIFT_TAP_CFG_X_EN			3
#define LSM6DSL_MASK_TAP_CFG_Y_EN			BIT(2)
#define LSM6DSL_SHIFT_TAP_CFG_Y_EN			2
#define LSM6DSL_MASK_TAP_CFG_Z_EN			BIT(1)
#define LSM6DSL_SHIFT_TAP_CFG_Z_EN			1
#define LSM6DSL_MASK_TAP_CFG_LIR			BIT(0)
#define LSM6DSL_SHIFT_TAP_CFG_LIR			0

#define LSM6DSL_REG_TAP_THS_6D				0x59
#define LSM6DSL_MASK_TAP_THS_6D_D4D_EN			BIT(7)
#define LSM6DSL_SHIFT_TAP_THS_6D_D4D_EN			7
#define LSM6DSL_MASK_TAP_THS_6D_SIXD_THS		(BIT(6) | BIT(5))
#define LSM6DSL_SHIFT_TAP_THS_6D_SIXD_THS		5
#define LSM6DSL_MASK_TAP_THS_6D_TAP_THS			(BIT(4) | BIT(3) | \
                             BIT(2) | BIT(1) | \
                             BIT(0))
#define LSM6DSL_SHIFT_TAP_THS_6D_TAP_THS		0

#define LSM6DSL_REG_INT_DUR2				0x5A
#define LSM6DSL_MASK_INT_DUR2_DUR			(BIT(7) | BIT(6) | \
                            BIT(5) | BIT(4))
#define LSM6DSL_SHIFT_INT_DUR2_DUR			4
#define LSM6DSL_MASK_INT_DUR2_QUIET			(BIT(3) | BIT(2))
#define LSM6DSL_SHIFT_INT_QUIET				2
#define LSM6DSL_MASK_INT_DUR2_SHOCK			(BIT(1) | BIT(0))
#define LSM6DSL_SHIFT_INT_SHOCK				0

#define LSM6DSL_REG_WAKE_UP_THS				0x5B
#define LSM6DSL_MASK_WAKE_UP_THS_SINGLE_DOUBLE_TAP	BIT(7)
#define LSM6DSL_SHIFT_WAKE_UP_THS_SINGLE_DOUBLE_TAP	7
#define LSM6DSL_MASK_WAKE_UP_THS_WK_THS			(BIT(5) | BIT(4) | \
                             BIT(3) | BIT(2) | \
                             BIT(1) | BIT(0))
#define LSM6DSL_SHIFT_WAKE_UP_THS_WK_THS		0

#define LSM6DSL_REG_WAKE_UP_DUR				0x5C
#define LSM6DSL_MASK_WAKE_UP_DUR_FF_DUR5		BIT(7)
#define LSM6DSL_SHIFT_WAKE_UP_DUR_FF_DUR5		7
#define LSM6DSL_MASK_WAKE_UP_DUR_WAKE_DUR		(BIT(6) | BIT(5))
#define LSM6DSL_SHIFT_WAKE_UP_DUAR_WAKE_DUR		5
#define LSM6DSL_MASK_WAKE_UP_DUR_TIMER_HR		BIT(4)
#define LSM6DSL_SHIFT_WAKE_UP_DUR_TIMER_HR		4
#define LSM6DSL_MASK_WAKE_UP_DUR_SLEEP_DUR		(BIT(3) | BIT(2) | \
                             BIT(1) | BIT(0))
#define LSM6DSL_SHIFT_WAKE_UP_DUR_SLEEP_DUR		0

#define LSM6DSL_REG_FREE_FALL				0x5D
#define LSM6DSL_MASK_FREE_FALL_DUR			(BIT(7) | BIT(6) | \
                             BIT(5) | BIT(4) | \
                             BIT(3))
#define LSM6DSL_SHIFT_FREE_FALL_DUR			4
#define LSM6DSL_MASK_FREE_FALL_THS			(BIT(2) | BIT(1) | \
                            BIT(0))
#define LSM6DSL_SHIFT_FREE_FALL_THS			0

#define LSM6DSL_REG_MD1_CFG				0x5E
typedef struct {
    uint8_t int1_timer : 1;
    uint8_t int1_tilt : 1;
    uint8_t int1_6d : 1;
    uint8_t int1_double_tap : 1;
    uint8_t int1_ff : 1;
    uint8_t int1_wu : 1;
    uint8_t int1_single_tap : 1;
    uint8_t int1_inact_state : 1;
} lsm6dsl_md1_cfg_t;

#define LSM6DSL_MASK_MD1_CFG_INT1_INACT_STATE		BIT(7)
#define LSM6DSL_SHIFT_MD1_CFG_INT1_INACT_STATE		7
#define LSM6DSL_MASK_MD1_CFG_INT1_SINGLE_TAP		BIT(6)
#define LSM6DSL_SHIFT_MD1_CFG_INT1_SINGLE_TAP		6
#define LSM6DSL_MASK_MD1_CFG_INT1_WU			BIT(5)
#define LSM6DSL_SHIFT_MD1_CFG_INT1_WU			5
#define LSM6DSL_MASK_MD1_CFG_INT1_FF			BIT(4)
#define LSM6DSL_SHIFT_MD1_CFG_INT1_FF			4
#define LSM6DSL_MASK_MD1_CFG_INT1_DOUBLE_TAP		BIT(3)
#define LSM6DSL_SHIFT_MD1_CFG_INT1_DOUBLE_TAP		3
#define LSM6DSL_MASK_MD1_CFG_INT1_6D			BIT(2)
#define LSM6DSL_SHIFT_MD1_CFG_INT1_6D			2
#define LSM6DSL_MASK_MD1_CFG_INT1_TILT			BIT(1)
#define LSM6DSL_SHIFT_MD1_CFG_INT1_TILT			1
#define LSM6DSL_MASK_MD1_CFG_INT1_TIMER			BIT(0)
#define LSM6DSL_SHIFT_MD1_CFG_INT1_TIMER		0

#define LSM6DSL_REG_MD2_CFG				0x5F
typedef struct {
    uint8_t int2_iron : 1;
    uint8_t int2_tilt : 1;
    uint8_t int2_6d : 1;
    uint8_t int2_double_tap : 1;
    uint8_t int2_ff : 1;
    uint8_t int2_wu : 1;
    uint8_t int2_single_tap : 1;
    uint8_t int2_inact_state : 1;
} lsm6dsl_md2_cfg_t;

#define LSM6DSL_MASK_MD2_CFG_INT2_INACT_STATE		BIT(7)
#define LSM6DSL_SHIFT_MD2_CFG_INT2_INACT_STATE		7
#define LSM6DSL_MASK_MD2_CFG_INT2_SINGLE_TAP		BIT(6)
#define LSM6DSL_SHIFT_MD2_CFG_INT2_SINGLE_TAP		6
#define LSM6DSL_MASK_MD2_CFG_INT2_WU			BIT(5)
#define LSM6DSL_SHIFT_MD2_CFG_INT1_WU			5
#define LSM6DSL_MASK_MD2_CFG_INT2_FF			BIT(4)
#define LSM6DSL_SHIFT_MD2_CFG_INT2_FF			4
#define LSM6DSL_MASK_MD2_CFG_INT2_DOUBLE_TAP		BIT(3)
#define LSM6DSL_SHIFT_MD2_CFG_INT2_DOUBLE_TAP		3
#define LSM6DSL_MASK_MD2_CFG_INT2_6D			BIT(2)
#define LSM6DSL_SHIFT_MD2_CFG_INT2_6D			2
#define LSM6DSL_MASK_MD2_CFG_INT2_TILT			BIT(1)
#define LSM6DSL_SHIFT_MD2_CFG_INT2_TILT			1
#define LSM6DSL_MASK_MD2_CFG_INT2_IRON			BIT(0)
#define LSM6DSL_SHIFT_MD2_CFG_INT2_IRON			0

#define LSM6DSL_REG_MASTER_CMD_CODE			0x60
#define LSM6DSL_REG_SENS_SYNC_SPI_ERROR_CODE		0x61
#define LSM6DSL_REG_OUT_MAG_RAW_X_L			0x66
#define LSM6DSL_REG_OUT_MAG_RAW_X_H			0x67
#define LSM6DSL_REG_OUT_MAG_RAW_Y_L			0x68
#define LSM6DSL_REG_OUT_MAG_RAW_Y_H			0x69
#define LSM6DSL_REG_OUT_MAG_RAW_Z_L			0x6A
#define LSM6DSL_REG_OUT_MAG_RAW_Z_H			0x6B
#define LSM6DSL_REG_X_OFS_USR				0x73
#define LSM6DSL_REG_Y_OFS_USR				0x74
#define LSM6DSL_REG_Z_OFS_USR				0x75


/* Accel sensor sensitivity grain is 0.061 mg/LSB */
#define SENSI_GRAIN_XL				(61LL / 1000.0)

/* Gyro sensor sensitivity grain is 4.375 mdps/LSB */
#define SENSI_GRAIN_G				(4375LL / 1000.0)
#define SENSOR_PI_DOUBLE			(SENSOR_PI / 1000000.0)
#define SENSOR_DEG2RAD_DOUBLE			(SENSOR_PI_DOUBLE / 180)
#define SENSOR_G_DOUBLE				(SENSOR_G / 1000000.0)




















#endif  //H_LSM6DSL_REG_