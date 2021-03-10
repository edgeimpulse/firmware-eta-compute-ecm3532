
/*******************************************************************************
 *
 * @file eta_nn_tables.h
 *
 * Copyright (C) 2019 Eta Compute, Inc
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
 ******************************************************************************/

#ifndef _ETA_NN_TABLES_H
#define _ETA_NN_TABLES_H

/**
* @file eta_nn_tables.h 
* Look-up tables for sigmoid and tanh activation functions
*/

/**
 * @brief Table for q7_t sigmoid funtion.
 */
extern const q7_t sigmoidTable_q7[256];
/**
 * @brief Table for q15_t sigmoid funtion.
 */
extern const q15_t sigmoidTable_q15[256];
/**
 * @brief Table for q7_t tanh funtion.
 */
extern const q7_t tanhTable_q7[256];
/**
 * @brief Table for q15_t tanh funtion.
 */
extern const q15_t tanhTable_q15[256];

#endif