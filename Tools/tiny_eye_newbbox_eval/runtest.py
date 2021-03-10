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

''' 
Driver file to run analysis using analysis class
Replace PATH/TO/ at the appropriate places to run this script
'''

from analyzewithetakernels import AnalyzeWithEtaKernels

def main():
    # jsonFile conatins the ground truth data
    jsonFile = '/PATH/TO/JSON_FILE'

    # image dir is the image directory
    imageDir = '/PATH/TO/IMAGES_DIR'
    
    # Enter the sub-directories name in list that you want to 
    # include in your run. This is useful when you have multiple 
    # sub directories. Empty list -> [] means all subdirectories
    includeDirs = [] # all subdirectories
    
    # If you want to run specific images, create a list of specific images
    # else make this parameter as [] so that all images can be run.
    # filename needs to include entire path under imageDir
    specificImages = [] # will run all images in directory
    
    # Tensorflow lite model - needed to quantize image to Qm.n
    tflitemodel = '/PATH/TO/TFLITE_FILE'
    
    # Path to save the results
    saveDir = '/PATH/TO/RESULTS_DIR'
    
    # File for output results
    outFile = 'result.txt'
    
    # This is the dec value you get from the anchors box in tflite model 
    # with NMS. Convert the max value of anchor (should be 0 -> max) to Qm.n
    # and use the value of here. E.G. in our case max value was 1.343 which
    # results in m = 1 and n = 6
    # Node Name : TFLite_Detection_PostProcess
    bboxScale = 6
    
    # This is the IOU number used when assigning inferred boxes to 
    # ground truth boxes, between (0, 1). Very low values should not 
    # be very critical as what we do not care about exact overlap
    # unlike object detection where localization is important
    postNMSIOU = 0.1
    
    # Application directory where the code is set to run on device
    appDir = '/PATH/TO/platform_sw/TFSW/Applications/executor_tiny_eye_256_newbbox/'
    
    # Target image size, right now fixed to 256x256
    img_height = 256
    img_width = 256
    
    # Createthe analyzer object
    myAnalyzer = AnalyzeWithEtaKernels(tflitemodel, jsonFile, bboxScale, 
                                       img_height, img_width, onDevice=True, 
                                       port='/dev/ttyACM0')
    # Get the image list
    myAnalyzer.createImageList(imageDir, includeDirs, specificImages)
    # Calculate and report the metrics
    myAnalyzer.runMetrics(outFile, showImage=False, saveImage=False, 
        saveDir=saveDir, postNMSIOU=postNMSIOU, appDir=appDir)

if __name__ == "__main__":
    main()
