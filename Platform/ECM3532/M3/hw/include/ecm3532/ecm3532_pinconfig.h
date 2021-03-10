/*
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
#include <stdint.h>

#define PIN0            			0
#define PIN0_FUNC_GPIO0      		0
#define PIN0_FUNC_PWMCH0     		1
#define PIN0_FUNC_UART0DBGC   		2
#define PIN0_FUNC_SPI2MISO     		3
#define PIN0_FUNC_SPISLV2MOSI  		4

#define PIN1            			1
#define PIN1_FUNC_GPIO1      		0
#define PIN1_FUNC_PDMCH1     		1
#define PIN1_FUNC_UART0DBGB  		2
#define PIN1_FUNC_SPI2MOSI          3
#define PIN1_FUNC_SPISLV2MISO       4

#define PIN2						2
#define PIN2_FUNC_GPIO2				0
#define PIN2_FUNC_PDMCH2			1
#define PIN2_FUNC_UART0DBGA			2
#define PIN2_FUNC_SPI2CLK           3
#define PIN2_FUNC_SPISLV2CLK        4

#define PIN3						3
#define PIN3_FUNC_GPIO3				0
#define PIN3_FUNC_PDMCH3			1
#define PIN3_FUNC_UART1DBGC			2
#define PIN3_FUNC_UART1RX           3

#define PIN4						4
#define PIN4_FUNC_GPIO4				0
#define PIN4_FUNC_PDMCH4			1
#define PIN4_FUNC_UART1DBGB			2
#define PIN4_FUNC_UART1CTS          3

#define PIN5						5
#define PIN5_FUNC_GPIO5				0
#define PIN5_FUNC_PDMCH5			1
#define PIN5_FUNC_UART1DBGA			2

#define PIN6						6
#define PIN6_FUNC_GPIO6				0
#define PIN6_FUNC_PDMCH6			1
#define PIN6_FUNC_FPANEL            2
#define PIN6_FUNC_CLKOUT            3

#define PIN7						7
#define PIN7_FUNC_GPIO7				0
#define PIN7_FUNC_PDMCH7			1
#define PIN7_FUNC_DSP_UART_TX       2
#define PIN7_FUNC_UART1TX           3

#define PIN8						8
#define PIN8_FUNC_GPIO8             0
#define PIN8_FUNC_I2C1SDA           1
#define PIN8_FUNC_PDM0CLK           2
#define PIN8_FUNC_I2C2SDA           3

#define PIN9						9
#define PIN9_FUNC_GPIO9             0
#define PIN9_FUNC_I2C1SCK           1
#define PIN9_FUNC_PDM0DATA          2
#define PIN9_FUNC_I2C2SCK           3

#define PIN10						10
#define PIN10_FUNC_GPIO10           0
#define PIN10_FUNC_SPI2CS2          1

#define PIN11						11
#define PIN11_FUNC_GPIO11			0
#define PIN11_FUNC_SPI2RDY          1
#define PIN11_FUNC_SPI2CS3          2
#define PIN11_FUNC_SPISLV2CS        3

#define PIN12						12
#define PIN12_FUNC_GPIO12			0
#define PIN12_FUNC_I2SACLK			1

#define PIN13						13
#define PIN13_FUNC_GPIO13			0
#define PIN13_FUNC_I2SBCLK			1

#define PIN14						14
#define PIN14_FUNC_GPIO14			0
#define PIN14_FUNC_I2SLRCLK			1
#define PIN14_FUNC_PDM1CLK          2

#define PIN15						15
#define PIN15_FUNC_GPIO15			0
#define PIN15_FUNC_I2SDATA_IN		1
#define PIN15_FUNC_PDM1DATA         2


#define PIN16						16
#define PIN16_FUNC_GPIO16			0
#define PIN16_FUNC_I2SDATA_OUT		1

#define PIN17						17
#define PIN17_FUNC_GPIO17			0
#define PIN17_FUNC_I2C0SDA          1
#define PIN17_FUNC_DSP_JTAG_TMS		2
#define PIN17_FUNC_PDM0CLK          3

#define PIN18						18
#define PIN18_FUNC_GPIO18			0
#define PIN18_FUNC_I2C0SCK          1
#define PIN18_FUNC_DSP_JTAG_TDI		2
#define PIN18_FUNC_PDM0DATA         3

#define PIN19						19
#define PIN19_FUNC_GPIO19			0
#define PIN19_FUNC_UART1RX			1
#define PIN19_FUNC_DSP_JTAG_TRST_N	2

#define PIN20						20
#define PIN20_FUNC_GPIO20			0
#define PIN20_FUNC_UART1TX			1
#define PIN20_FUNC_DSP_UART_TX      2
#define PIN20_FUNC_FPANEL           3

#define PIN21						21
#define PIN21_FUNC_GPIO21			0
#define PIN21_FUNC_UART1CTS			1
#define PIN21_FUNC_DSP_JTAG_TCLK	2

#define PIN22						22
#define PIN22_FUNC_GPIO22			0
#define PIN22_FUNC_UART1RTS			1
#define PIN22_FUNC_DSP_JTAG_TDO		2
#define PIN22_FUNC_CLKOUT           3

#define PIN23						23
#define PIN23_FUNC_GPIO23			0
#define PIN23_FUNC_SPI0CS0			1
#define PIN23_FUNC_SPISLV0CS        2

#define PIN24						24
#define PIN24_FUNC_GPIO24			0
#define PIN24_FUNC_SPI0CS1			1

#define PIN25						25
#define PIN25_FUNC_GPIO25			0
#define PIN25_FUNC_SPI0CS2			1

#define PIN26						26
#define PIN26_FUNC_GPIO26			0
#define PIN26_FUNC_SPI0CS3			1
#define PIN26_FUNC_SPI0RDY			2

#define PIN27						27
#define PIN27_FUNC_GPIO27			0
#define PIN27_FUNC_SPI1MISO			1
#define PIN27_FUNC_SPI2MISO			3
#define PIN27_FUNC_SPISLV1MOSI 		4
#define PIN27_FUNC_SPISLV2MOSI 		5

#define PIN28						28
#define PIN28_FUNC_GPIO28			0
#define PIN28_FUNC_SPI1MOSI			1
#define PIN28_FUNC_I2C1SDA			2
#define PIN28_FUNC_SPI2MOSI			3
#define PIN28_FUNC_SPISLV1MISO      4
#define PIN28_FUNC_SPISLV2MISO      5

#define PIN29						29
#define PIN29_FUNC_GPIO29			0
#define PIN29_FUNC_SPI1CLK			1
#define PIN29_FUNC_I2C1SCK			2
#define PIN29_FUNC_SPI2CLK			3
#define PIN29_FUNC_SPISLV1CLK       4
#define PIN29_FUNC_SPISLV2CLK       5

#define PIN30						30
#define PIN30_FUNC_GPIO30			0
#define PIN30_FUNC_SPI1CS0			1
#define PIN30_FUNC_SPI2RDY			2
#define PIN30_FUNC_SPI2CS0			3
#define PIN30_FUNC_SPISLV1CS 		4
#define PIN30_FUNC_SPISLV2CS 		5

#define PIN31						31
#define PIN31_FUNC_GPIO31			0
#define PIN31_FUNC_SPI1CS1			1
#define PIN31_FUNC_SPI1RDY			2
#define PIN31_FUNC_SPI2CS1			3

#define PIN_SPI0CLK                 32
#define PIN_SPI0CLK_FUNC_SPI0CLK    0
#define PIN_SPI0CLK_FUNC_I2C0CLK    1
#define PIN_SPI0CLK_FUNC_SPISLV0SCK 2

#define PIN_SPI0MOSI                33
#define PIN_SPI0MOSI_FUNC_MOSI      0
#define PIN_SPI0MOSI_FUNC_I2C0SDA   1
#define PIN_SPI0MOSI_FUNC_SPISLV0MISO 2

#define PIN_SPI0MISO                34
#define PIN_SPI0MISO_FUNC_MISO      0
#define PIN_SPI0MISO_FUNC_SPISLV0MOSI   1


#define PIN_CNT						35

int ecm3532_pin_config(uint8_t pin_no, uint8_t pin_func);

void ecm3532_set_pinmux(void);
