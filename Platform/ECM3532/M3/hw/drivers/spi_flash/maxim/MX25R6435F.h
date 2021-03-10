/***************************************************************************//**
 *
 * @file eta_status.h
 *
 * @brief This file contains eta_status module definitions.
 *
 * Copyright (C) 2018 Eta Compute, Inc
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * This is part of revision 1.0.0b1 of the Tensai Chip Support Package.
 *
 ******************************************************************************/

#ifndef H_MX25R6435F_
#define H_MX25R6435F_

#define MAX_OFFSET_SIZE 0x800000

#define WREN_CMD    0x06
#define WRDI_CMD    0x04
#define RDID_CMD    0x9F
#define RDSR_CMD    0x05
#define RDCR_CMD    0x15
#define WRSR_CMD    0x01
#define READ_CMD    0x03
#define FAST_READ_CMD   0x0B
#define SE_CMD      0x20
#define BE32K_CMD   0x52
#define BE_CMD      0xD8
#define CE_CMD      0xC7
#define DP_CMD      0xB9
#define PP_CMD      0x02

#define PP_MAX_LEN  256
#define PP_MAX_LEN_MASK (PP_MAX_LEN - 1)

#define MAX_NUM_BLOCK    128
#define BLOCK_LEN_BYTES  0x10000
#define BLOCK32_LEN_BYTES 0x8000
#define SECTOR_LEN_BYTES 0x1000

#define MAX_SECTOR_NUM  2048

#define MANUFACTURER_ID 0xC2
#define MEM_TYPE        0x28
#define MEM_DENSITY     0x17

#define MAX_WAIT_DELAY  500

#pragma pack(1)
typedef struct _WREN_
{
    uint8_t cmd;
}WREN_t;

typedef struct _WRDI_
{
    uint8_t cmd;
}WRDI_t;

typedef struct _RDID_
{
    uint8_t cmd;
}RDID_t;

typedef struct _RDID_RES_
{
    uint8_t manufacturerId;
    uint8_t memoryType;
    uint8_t memoryDensity;
}RDID_RES_t;

typedef struct _RDSR_
{
    uint8_t cmd;
}RDSR_t;

typedef union _RDSR_RES_
{
    uint8_t status;
    struct
    {
        uint8_t wip : 1;
        uint8_t wel : 1;
        uint8_t bpx : 4;
        uint8_t qe  : 1;
        uint8_t srwd: 1;
    }bits;
}RDSR_RES_t;

typedef struct _RDCR_
{
    uint8_t cmd;
}RDCR_t;

typedef union _RDCR_RES_
{
    struct
    {
        uint8_t config1;
        uint8_t config2;
    }bytes;
    struct
    {
        struct
        {
            uint8_t rsvd0   : 3;
            uint8_t tb      : 1;
            uint8_t rsvd1   : 2;
            uint8_t dc      : 1;
            uint8_t rsvd2   : 1;
        }config1;
        struct
        {
            uint8_t rsvd0   : 1;
            uint8_t lphp    : 1;
            uint8_t rsvd1   : 5;
        }config2;
    }bits;
}RDCR_RES_t;

typedef struct _WRSR_
{
    uint8_t cmd;
    RDSR_RES_t status;
    RDCR_RES_t config;
}WRSR_t;

typedef struct _READ_
{
    uint8_t cmd;
    uint8_t addr2;
    uint8_t addr1;
    uint8_t addr0;
}READ_t;

typedef struct _FAST_READ_
{
    uint8_t cmd;
    uint8_t addr2;
    uint8_t addr1;
    uint8_t addr0;
    uint8_t dummy;
}FAST_READ_t;

typedef struct _SE_
{
    uint8_t cmd;
    uint8_t addr2;
    uint8_t addr1;
    uint8_t addr0;
}SE_t;

typedef struct _BE32K_
{
    uint8_t cmd;
    uint8_t addr2;
    uint8_t addr1;
    uint8_t addr0;
}BE32K_t;

typedef struct _BE_
{
    uint8_t cmd;
    uint8_t addr2;
    uint8_t addr1;
    uint8_t addr0;
}BE_t;

typedef struct _CE_
{
    uint8_t cmd;
}CE_t;

typedef struct _PP_
{
    uint8_t cmd;
    uint8_t addr2;
    uint8_t addr1;
    uint8_t addr0;
}PP_t;

typedef struct _DP_
{
    uint8_t cmd;
}DP_t;
//#pragma pack(pop)

#endif