# ==============================================================================
# Copyright (C) 2020 Eta Compute, Inc
# *
# * Licensed under the Apache License, Version 2.0 (the "License");
# * you may not use this file except in compliance with the License.
# * You may obtain a copy of the License at
# *
# *    http://www.apache.org/licenses/LICENSE-2.0
# *
# * Unless required by applicable law or agreed to in writing, software
# * distributed under the License is distributed on an "AS IS" BASIS,
# * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# * See the License for the specific language governing permissions and
# * limitations under the License.
# *
# * This is part of Tensai Software Development Kit.
# *
# ==============================================================================

"""
This file generates an input data file for a given image
Outputs a C include file for the input and output vectors for this particular image.

"""

import numpy as np
import tensorflow as tf
import os
import math
from PIL import Image
import click
import matplotlib.pyplot as plt


def quantize(input_array, scale, zero, bits):
  # image comes in as float64
  # use model scale and zero to convert to uint8 
  quant = np.round(input_array/scale) + zero
  quant = np.clip(quant, a_min = 0, a_max = 255)
  quant = quant.astype(np.uint8)
  return quant 
  
  
def read_infer_quantize_for_compiler(in_file, model_file, out_file, on_device=False):
    # Load TFLite model and allocate tensors.
    interpreter = tf.compat.v1.lite.Interpreter(model_path=model_file)
    interpreter.allocate_tensors()

    overFactor = 0.015
    # Get input and output tensors.
    input_details = interpreter.get_input_details()
    # input details is list of dictionaries.Since we have only one 1 input
    # we need to look at the first item only [0]
    scale, zero = input_details[0]['quantization']
    
    # find max range to calculate m in qm.n quantization
    input_max = scale * (255-zero)
    input_min = -zero * scale
    maxinput = max(abs(input_max), abs(input_min))
    frac, int_ = math.modf(math.log2(maxinput))
    if frac < overFactor:
        numbits = max(0, int(int_))
    else:
        numbits = max(0, int(1+int_))

    # assumes tensors stored in NHWC format
    _, H, W, C = input_details[0]['shape']
    # read image from files
    image = Image.open(in_file)
    # resize image to W x H
    image = image.resize((W,H))
    # convert image to greyscale
    image = image.convert('L')
    image = np.array(image)
    # normalize image
    mu = np.mean(image)
    sig = np.std(image, ddof=1)
    dec_bits = 8 - numbits -1
    # normalized
    norm_image_float = (image-mu)/sig
    # use numbits to quantize input
    dec_bits = 8 - numbits - 1
    with open(out_file, "w") as f:
        tmpstr = str(numbits)
        f.write("const int8_t input_int_bits = ")
        f.write(tmpstr)
        f.write(";\n")      
        tmpstr = str(dec_bits)
        f.write("const int8_t input_dec_bits = ")
        f.write(tmpstr)
        f.write(";\n")
        # convert uint8 to int before subtraction and multiplication
        # convert norm_image_float to qm.n
        norm_image_qmn = np.round(norm_image_float * (2**dec_bits),0)
        norm_image_qmn = np.clip(norm_image_qmn, a_min=-128, a_max = 127).astype(np.int8)
        tmpstr = str(norm_image_qmn.flatten().tolist())[1:-1]
        if on_device:
            f.write("const q7_t pIn[" + str(H*W*C) + ']__attribute__((section(".binSection"))) = {')
        else: # on renode
            f.write("const q7_t pIn0[" + str(H*W*C) + "] = {")
        f.write(tmpstr)
        f.write(" };")
        f.write("\n\n")

