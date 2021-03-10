/**
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

#include "config.h"
#include "ecm3532_pinconfig.h"
#include "reg_eta_ecm3532_m3.h"
#include "helper_func.h"
#include "errno.h"

#define PIN_CTRL_BASE REG_SOCCTRL_PMUXA_ADDR

struct pin_ctrl {
    /* registet offset */
    uint8_t reg_off;
    /* gpio control bit position in 32 bit*/
    uint8_t bpos;
    /* no of bits*/
    uint8_t bit_cnt;
};

struct pinMux
{
    uint8_t pinNo;
    uint8_t pinMux;
};

struct pin_ctrl gp_ctrl[PIN_CNT] = {
    [PIN0] = {0, 0, 3},
    [PIN1] = {0, 4, 3},
    [PIN2] = {0, 8, 3},
    [PIN3] = {0, 12, 3},
    [PIN4] = {0, 16, 3},
    [PIN5] = {0, 20, 3},
    [PIN6] = {0, 24, 2},
    [PIN7] = {0, 28, 2},

    [PIN8] = {4, 0, 2},
    [PIN9] = {4, 4, 2},
    [PIN10] = {4, 8, 2},
    [PIN11] = {4, 12, 3},

    [PIN12] = {4, 16, 1},
    [PIN13] = {4, 20, 1},
    [PIN14] = {4, 24, 2},
    [PIN15] = {4, 28, 2},

    [PIN16] = {8, 0, 1},
    [PIN17] = {8, 4, 2},
    [PIN18] = {8, 8, 2},
    [PIN19] = {8, 12, 2},
    [PIN20] = {8, 16, 2},
    [PIN21] = {8, 20, 2},
    [PIN22] = {8, 24, 2},
    [PIN23] = {8, 28, 2},

    [PIN24] = {12, 0, 1},
    [PIN25] = {12, 4, 1},
    [PIN26] = {12, 8, 2},
    [PIN27] = {12, 12, 3},
    [PIN28] = {12, 16, 3},
    [PIN29] = {12, 20, 3},
    [PIN30] = {12, 24, 3},
    [PIN31] = {12, 28, 2},

    [PIN_SPI0CLK] = {16, 0, 2},
    [PIN_SPI0MOSI] = {16, 4, 2},
    [PIN_SPI0MISO] = {16, 8, 1},
};

const struct pinMux pinMuxArray [] __attribute__((section(".initSection"))) =
{
#ifdef CONFIG_PIN0_MUX
    {
        .pinNo = PIN0,
        .pinMux = CONFIG_PIN0_MUX,
    },
#endif

#ifdef CONFIG_PIN1_MUX
    {
        .pinNo = PIN1,
        .pinMux = CONFIG_PIN1_MUX,
    },
#endif

#ifdef CONFIG_PIN2_MUX
    {
        .pinNo = PIN2,
        .pinMux = CONFIG_PIN2_MUX,
    },
#endif

#ifdef CONFIG_PIN3_MUX
    {
        .pinNo = PIN3,
        .pinMux = CONFIG_PIN3_MUX,
    },
#endif

#ifdef CONFIG_PIN4_MUX
    {
        .pinNo = PIN4,
        .pinMux = CONFIG_PIN4_MUX,
    },
#endif

#ifdef CONFIG_PIN5_MUX
    {
        .pinNo = PIN5,
        .pinMux = CONFIG_PIN5_MUX,
    },
#endif

#ifdef CONFIG_PIN6_MUX
    {
        .pinNo = PIN6,
        .pinMux = CONFIG_PIN6_MUX,
    },
#endif

#ifdef CONFIG_PIN7_MUX
    {
        .pinNo = PIN7,
        .pinMux = CONFIG_PIN7_MUX,
    },
#endif
#ifdef CONFIG_PIN8_MUX
    {
        .pinNo = PIN8,
        .pinMux = CONFIG_PIN8_MUX,
    },
#endif
#ifdef CONFIG_PIN9_MUX
    {
        .pinNo = PIN9,
        .pinMux = CONFIG_PIN9_MUX,
    },
#endif
#ifdef CONFIG_PIN10_MUX
    {
        .pinNo = PIN10,
        .pinMux = CONFIG_PIN10_MUX,
    },
#endif
#ifdef CONFIG_PIN11_MUX
    {
        .pinNo = PIN11,
        .pinMux = CONFIG_PIN11_MUX,
    },
#endif
#ifdef CONFIG_PIN12_MUX
    {
        .pinNo = PIN12,
        .pinMux = CONFIG_PIN12_MUX,
    },
#endif
#ifdef CONFIG_PIN13_MUX
    {
        .pinNo = PIN13,
        .pinMux = CONFIG_PIN13_MUX,
    },
#endif
#ifdef CONFIG_PIN14_MUX
    {
        .pinNo = PIN14,
        .pinMux = CONFIG_PIN14_MUX,
    },
#endif
#ifdef CONFIG_PIN15_MUX
    {
        .pinNo = PIN15,
        .pinMux = CONFIG_PIN15_MUX,
    },
#endif
#ifdef CONFIG_PIN16_MUX
    {
        .pinNo = PIN16,
        .pinMux = CONFIG_PIN16_MUX,
    },
#endif
#ifdef CONFIG_PIN17_MUX
    {
        .pinNo = PIN17,
        .pinMux = CONFIG_PIN17_MUX,
    },
#endif
#ifdef CONFIG_PIN18_MUX
    {
        .pinNo = PIN18,
        .pinMux = CONFIG_PIN18_MUX,
    },
#endif
#ifdef CONFIG_PIN19_MUX
    {
        .pinNo = PIN19,
        .pinMux = CONFIG_PIN19_MUX,
    },
#endif
#ifdef CONFIG_PIN20_MUX
    {
        .pinNo = PIN20,
        .pinMux = CONFIG_PIN20_MUX,
    },
#endif
#ifdef CONFIG_PIN21_MUX
    {
        .pinNo = PIN21,
        .pinMux = CONFIG_PIN21_MUX,
    },
#endif
#ifdef CONFIG_PIN22_MUX
    {
        .pinNo = PIN22,
        .pinMux = CONFIG_PIN22_MUX,
    },
#endif
#ifdef CONFIG_PIN23_MUX
    {
        .pinNo = PIN23,
        .pinMux = CONFIG_PIN23_MUX,
    },
#endif
#ifdef CONFIG_PIN24_MUX
    {
        .pinNo = PIN24,
        .pinMux = CONFIG_PIN24_MUX,
    },
#endif
#ifdef CONFIG_PIN25_MUX
    {
        .pinNo = PIN25,
        .pinMux = CONFIG_PIN25_MUX,
    },
#endif
 #ifdef CONFIG_PIN25_MUX
    {
        .pinNo = PIN25,
        .pinMux = CONFIG_PIN25_MUX,
    },
#endif
#ifdef CONFIG_PIN26_MUX
    {
        .pinNo = PIN26,
        .pinMux = CONFIG_PIN26_MUX,
    },
#endif
#ifdef CONFIG_PIN27_MUX
    {
        .pinNo = PIN27,
        .pinMux = CONFIG_PIN27_MUX,
    },
#endif
#ifdef CONFIG_PIN28_MUX
    {
        .pinNo = PIN28,
        .pinMux = CONFIG_PIN28_MUX,
    },
#endif
#ifdef CONFIG_PIN29_MUX
    {
        .pinNo = PIN29,
        .pinMux = CONFIG_PIN29_MUX,
    },
#endif
#ifdef CONFIG_PIN30_MUX
    {
        .pinNo = PIN30,
        .pinMux = CONFIG_PIN30_MUX,
    },
#endif
#ifdef CONFIG_PIN31_MUX
    {
        .pinNo = PIN31,
        .pinMux = CONFIG_PIN31_MUX,
    },
#endif
#ifdef CONFIG_PIN_SPI0CLK
    {
        .pinNo = PIN_SPI0CLK,
        .pinMux = CONFIG_PIN_SPI0CLK,
    },
#endif
#ifdef CONFIG_PIN_SPI0MOSI
    {
        .pinNo = PIN_SPI0MOSI,
        .pinMux = CONFIG_PIN_SPI0MOSI,
    },
#endif
#ifdef CONFIG_PIN_SPI0MISO
    {
        .pinNo = PIN_SPI0MISO,
        .pinMux = CONFIG_PIN_SPI0MISO,
    },
#endif


};

/*
 * pin config
 *
 * select pin function
 *
 * @param pin number
 *
 * @param pin function
 *
 * @return status
 */
int
ecm3532_pin_config(uint8_t pin_no, uint8_t pin_func)
{
    volatile int *reg;
    int val, mask;
    struct pin_ctrl pctrl;

    if (pin_no >= PIN_CNT)
        return -EINVAL;

    pctrl = gp_ctrl[pin_no];

    if (pin_func >= (1 << pctrl.bit_cnt))
        return -EINVAL;

    reg = (int *)((int)PIN_CTRL_BASE +
                    pctrl.reg_off);
    val = *reg;

    mask = (((1 << pctrl.bit_cnt) - 1) << pctrl.bpos);

    val = val & ~(mask);
    val |= (pin_func << pctrl.bpos);
    *reg = val;

    return 0;
}

void ecm3532_set_pinmux(void)
{
    int i;

    for (i = 0;  i < ARRAY_SIZE(pinMuxArray); i++)
        ecm3532_pin_config(pinMuxArray[i].pinNo, pinMuxArray[i].pinMux);
}


