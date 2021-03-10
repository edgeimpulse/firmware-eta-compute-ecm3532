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
"""Class definition to process images and calculate metrics"""

import os
import sys
import subprocess
import re
import time
import psutil
import serial
import numpy as np
from pathlib import Path
from PIL import Image, ImageFont, ImageDraw, ImageEnhance

from create_input_quantized import read_infer_quantize_for_compiler
from eta_eval_utils import load_eta_testset, gtboxes_image,calculate_metrics



class AnalyzeWithEtaKernels:
    def __init__(self, tflitemodel, dataSetDir, bboxScale, imHeight=320, imWidth=320, onDevice=False, port=None):
        """Top level class for inferencing and evaluating metrics
        Args:
            tflitemode: TFLite model file - used to quantize input data
            dataSetDir: Location of the ground truth files
            bboxScale: n in the Qm.n for anchor boxes defined in TFLite file
            imHeight, imWidth: Model requirements for image height and width
            onDevice: boolean, If true then use board else renode
            port: string path to the port, for e.g. /dev/ttyACM0

        Returns: Nothing but defines
            self.bboxScale: bboxScale for anchor boxes from TFLite 
            self.modelFile: TFLite File
            self.renodeLogFile: Location of where you want to put the renode log file
            self.imHeight: Height of images used by TFLite Model
            self.imWidth: Width of images used by TFLite Model
            self.imageCount: Number of images in a particular run
            self.imageList: List of images used for this run
            self.onDevice: boolean, If true then use board else renode
        """
        self.d1 = load_eta_testset(dataSetDir)
        self.bboxScale = bboxScale
        self.modelFile = tflitemodel
        self.onDevice = onDevice
        if onDevice: # use serial port
            assert port != None, 'Must specify the port on which the device is connected.'
            self.serialPort = serial.Serial()
            self.serialPort.port = port
            self.serialPort.baudrate = 115200
            self.serialPort.timeout = 3
        else:
            # if you change this location, you need to edit the location in
            # the file Eta_Travis.resc located in renode_emulator directory.
            self.renodeLogFile = '/tmp/renode_log.txt'
        self.imHeight = imHeight
        self.imWidth = imWidth
        self.imageCount = 0
        self.imageList = []

    def createImageList(self, imagedir, includeDirs, imageList = [] ):
        """Create a list of images on which inference will be run
        Args:
            imagedir: Directory containing images that need to processed
            includeDirs : If [], all files and subdirectories in imagedir are processed. Else only the subdirectories
            listed in includeDirs are procecssed
            single_image: If [], all images in directory are processed. Else only the images in this list are processed 
        Returns: Nothing but updates the following class variables
            self.imageList : List of all images that will be processed
            self.imageCount : Number of images that will be processed
        """
        if not imageList:
            for subdir, _, files in os.walk(imagedir):
                if includeDirs :
                    if os.path.basename(subdir) in includeDirs:
                        for file in files:
                            if file.endswith('.jpg') or file.endswith('.bmp') or file.endswith('.jpeg'):
                                self.imageList.append(os.path.join(subdir, file))
                                self.imageCount += 1
                else:
                        for file in files:
                            if file.endswith('.jpg') or file.endswith('.bmp') or file.endswith('.jpeg'):
                                self.imageList.append(os.path.join(subdir, file))
                                self.imageCount += 1                    
        else:
            #if you want to process a specific image, put it in here
            self.imageList = [os.path.join(imagedir, file) for file in imageList]
            self.imageCount = len(imageList)
        print(f'Total Number of images : {self.imageCount}')
 
    def __convertToCocoFormat(self, inBBox):
        """Convert bbox from inference output to COCO format for bboxes 
        They come in as ymin,xmin,ymax, xmax and get converted to xmin, ymin, W, H
        format. Additionally the vertical boxes that have been clipped are resized.
        Args:
            inBBox: Array of BBoxes coming from the NMS routine in C
        Returns:
            outBBox : Array of inBBoxes that have been converted to COCO format.
        """
        scale = 2**self.bboxScale
        # there are some corner cases that may be slightly negative like -1. round them to 0
        if np.any(inBBox < 0):
            # this should not be zero technically but may result in -1 because of quantization
            # and rounding. 
            assert not np.any(inBBox < -2), "BBox error output from C is very negative"
            inBBox[inBBox < 0] = 0
        # scale x and y axis differently
        outBBox = inBBox * [self.imHeight/scale, self.imWidth/scale,self.imHeight/scale, self.imWidth/scale]
        # swap to get order as xmin, ymin, xmax, ymax
        # create matrix to subtract two columns to get W and H
        subtractmatrix = np.array([[1,0,-1,0],[0,1,0,-1],[0,0,1,0],[0,0,0,1]], dtype=np.float32)
        outBBox = outBBox[:, [1,0,3,2]]
        outBBox = np.matmul(outBBox, subtractmatrix)
        # order is xmin, ymin, w, h if H > 2W, H = 2W
        # for row in outBBox:
        #     row[3] = 2 * row[2] if row[3] > 2 * row[2] else row[3]
        # all numbers should be positive
        assert outBBox.all() >= 0, 'CoCo format BBox has negative numbers'
        return outBBox

    def __runSingleImageOnRenode(self, inFile):
        """This function is specific to c kernels running on renode.
        The input image is quantized using Qm.n format based on the input ranges
        defined in the TFLite mode and saved in input_data.h. This file is included
        in the c program at gen_code/src/main.c. After make, renode is run and 
        the output is stored in the file /tmp/renode_log.txt. This file is deleted
        after parsing.
        
        Args:
            inFile : A single image file
        Returns:
            None. But produces a log file of UART output
        """
        print(f'Image file being processed: {os.path.basename(inFile)}')
        # generate input_data.h
        read_infer_quantize_for_compiler(inFile, self.modelFile, 'input_data.h')
        # copy input_data.h to ./gen_code/inc
        subprocess.run(['mv','input_data.h','./gen_code/inc'])
        # make, should we do a make clean once ?
        subprocess.run(['make'], cwd='gen_code', capture_output = True)
        # run renode and produce renode_log.txt in /tmp directory
        subprocess.run(['renode','Eta_Travis.resc'], cwd='renode_emulator', capture_output = True)

    def __runSingleImageOnBoard(self, inFile, appDir):
        """This function is specific to the application running on board.
        The input image is quantized using Qm.n format based on the input ranges
        defined in the TFLite mode and saved in input_data.h. This file is 
        included in the c program at appDir/src/infer_multi.c. 
        After make, renode is run and the output is stored in the file /tmp/infer_log.txt. This file is deleted after parsing.
        
        Args:
            inFile: A single image file
            appDir: Path to the application running the inference code
        Returns:
            None. But produces a log file on UART output
        """
        print(f'Image file being processed: {os.path.basename(inFile)}')
        # generate input_data.h
        read_infer_quantize_for_compiler(inFile, self.modelFile, 'input_data.h',
                                         self.onDevice)
        # copy input_data.h to appDir/incude
        subprocess.run(['mv','input_data.h',os.path.join(appDir, 'include')])
        # make, should we do a make clean once ?
        buildDir = os.path.join(appDir, 'build')
        subprocess.run(['make', '-j8'], cwd=buildDir, 
                       shell=True, capture_output=True)
        # erase the board
        subprocess.run(['make erase'], cwd=buildDir, 
                       shell=True, capture_output=True)
        # flash the board with new input
        subprocess.run(['make flash'], cwd=buildDir, 
                       shell=True, capture_output=True)
        # capture the output in the logs
        self.serialPort.open()
        self.deviceLogs = list(
            map(lambda line: line.decode("utf-8").strip(), 
                self.serialPort.readlines()))
        self.serialPort.close()

    def __processDeviceLog(self):
        """This function is used to parse the device logs captured from the 
        serial port and produce bboxes and probabilites
        from the C program
        Args:
            None. The renode_log.txt file is used implicitly.
        Returns:
            numPeople: Output of the NMS program indicating the number of   
                persons detected
            postNmsProbs: Numpy array of probabilities corresponding to each 
                person detected. Note that this will have only numPeople
                bounding boxes.
            postNMSBbox: Numpy array of bounding boxes corresponding to each 
                person detected. Note that this will have only 
                numPeople bounding boxes.
        """
                # pattern match regexes
        # theoretically probs should not have a negative number but it does due to quantization
        # typically -1. Have not seen a larger number
        CONF_REGEX = r".*=[ \[\]]*(?P<vals>[-,0-9 ]*)"
        PEOPLE_REGEX = r".*detected:[ ]*(?P<num>\d+)"
        # initialize defaults. this is what we will send out it no person is detected
        numPeople, postNmsProbs, postNMSBbox  = 0, [], []    
        
        confList = [x for x in self.deviceLogs if 'conf_list' in x]
        bboxList = [x for x in self.deviceLogs if 'bbox_list' in x]
        numPeopleList = [x for x in self.deviceLogs if 'Total people' in x]
        # conf_list is a list. so pick the first string
        # this is to generate ssd_head output for processing by python separately
        # pick ssd bounding boxes
        # there is an extra comma at the end so picks up an extra value
        # pick ssd probabilities
        # there is an extra comma at the end so picks up an extra value
        # check to see if the number of bboxes is identical for both. Else we have a problem
        # this is the result of postprocessing by NMS in c code in the processor. 
        matches = re.search(PEOPLE_REGEX, numPeopleList[0], re.MULTILINE)
        numPeople = int(matches.group('num'))
        print(f'Number of people detected: {numPeople}')
        # if no person detected there will no bounding boxes so cannot reshape
        # so default values will be sent out
        if not numPeople == 0:
            matches = re.search(CONF_REGEX, confList[0], re.MULTILINE)
            postNmsProbs = np.fromstring(matches.group('vals'), 
                                         dtype=np.int8, 
                                         sep=',')
            # bbox shaped as :,4
            matches = re.search(CONF_REGEX, bboxList[0], re.MULTILINE)
            bboxVals = np.fromstring(matches.group('vals'), 
                                     dtype=np.int8, 
                                     sep=',').reshape((-1,4))
            postNMSBbox = self.__convertToCocoFormat(bboxVals)

        return numPeople, postNmsProbs, postNMSBbox

    def __processLogFile(self):
        """This function is used to parse the renode output log file and produce bboxes and probabilites
        from the C program
        Args:
            None. The renode_log.txt file is used implicitly.
        Returns:
            ssdHeadBbox: Numpy array containing BBoxes at the output of SSD Head. Note that these are relative to anchor
            boxes and need decoding before they can be related to image coordinates. Currently we don't use this. They are
            there for experimentation purposes only.
            ssdHeadProbs: Numpy array containing probabilities at the output of SSD Head. These are valid numbers in the
            form [background prob, person prob] and coded as Q0.7. To convert to floating point, divide by 2^7. The number
            of rows equals the number of anchor boxes in the network. Currently we don't use this. They are
            there for experimentation purposes only.
            numPeople: Output of the NMS program indicating the number of persons detected
            postNmsProbs: Numpy array of probabilities corresponding to each person detected. Note that this will have only numPeople
            bounding boxes.
            postNMSBbox: Numpy array of bounding boxes corresponding to each person detected. Note that this will have only 
            numPeople bounding boxes.
        """  
        # pattern match regexes
        # theoretically probs should not have a negative number but it does due to quantization
        # typically -1. Have not seen a larger number
        CONF_REGEX = r".*=[ \[\]]*(?P<vals>[-,0-9 ]*)"
        PEOPLE_REGEX = r".*detected:[ ]*(?P<num>\d+)"
        SSD_HEAD_BBOX_REGEX = r".*ssd head bbox:[ ]*(?P<bbox>[-, \d]*)"
        SSD_HEAD_PROBS_REGEX = r".*ssd head probs:[ ]*(?P<probs>[-, \d]*)"
        # initialize defaults. this is what we will send out it no person is detected
        ssdHeadBbox, ssdHeadProbs, numPeople,postNmsProbs, postNMSBbox  = [], [], 0, [], []    
        with open(self.renodeLogFile) as f:
            allstrings = f.readlines()
            confList = [x for x in allstrings if 'conf_list' in x]
            bboxList = [x for x in allstrings if 'bbox_list' in x]
            numPeopleList = [x for x in allstrings if 'Total people' in x]
            ssdHeadBboxString = [x for x in allstrings if 'ssd head bbox' in x]
            ssdHeadProbsString = [x for x in allstrings if 'ssd head prob' in x]
        # conf_list is a list. so pick the first string
        # this is to generate ssd_head output for processing by python separately
        # pick ssd bounding boxes
        matches = re.search(SSD_HEAD_BBOX_REGEX, ssdHeadBboxString[0], re.MULTILINE)
        # there is an extra comma at the end so picks up an extra value
        ssdHeadBbox = np.fromstring(matches.group('bbox'),dtype=np.int8, sep=',')[:-1].reshape((-1,4))
        # pick ssd probabilities
        matches = re.search(SSD_HEAD_PROBS_REGEX, ssdHeadProbsString[0], re.MULTILINE)
        # there is an extra comma at the end so picks up an extra value
        ssdHeadProbs = np.fromstring(matches.group('probs'),dtype=np.int8, sep=',')[:-1].reshape((-1,2))
        # check to see if the number of bboxes is identical for both. Else we have a problem
        assert ssdHeadBbox.shape[0] == ssdHeadProbs.shape[0]
        # this is the result of postprocessing by NMS in c code in the processor. 
        matches = re.search(PEOPLE_REGEX, numPeopleList[0], re.MULTILINE)
        numPeople = int(matches.group('num'))
        print(f'Number of people detected: {numPeople}')
        # if no person detected there will no bounding boxes so cannot reshape
        # so default values will be sent out
        if not numPeople == 0:
            matches = re.search(CONF_REGEX, confList[0], re.MULTILINE)
            postNmsProbs = np.fromstring(matches.group('vals'), dtype=np.int8, sep=',')
            # bbox shaped as :,4
            matches = re.search(CONF_REGEX, bboxList[0], re.MULTILINE)
            bboxVals = np.fromstring(matches.group('vals'), dtype=np.int8, sep=',').reshape((-1,4))
            postNMSBbox = self.__convertToCocoFormat(bboxVals)
        # remove renode log fle
        subprocess.run(['rm', self.renodeLogFile], capture_output = True)

        return ssdHeadBbox, ssdHeadProbs, numPeople,postNmsProbs, postNMSBbox

    def runMetrics(self, outFile, showImage=False, saveImage=False, 
                   saveDir='junk', postNMSIOU=0.3, appDir=None):
        """This is the main loop that processes each image, calculates metrics and displays and logs image data.
        Finally,it aggregates metrics over the entire run. It uses __plotSaveImage and __calcualateMetrics functions
        to do most of the work.
        Args:
            outFile: Output log file for the entire run. This includes data per image as well as total data.
            showImage: If true, this shows each image with inferred image bboxes in red and probabilities and ground truth
            bboxes in green (if available)
            saveImage: This saves the annotated image (image + ground truth bboxes (if available) + inferred bboxes + inferred
            probs) as a jpg file
            saveDir: Directory to save annotated images
            postNMSIOU: This is the number used to see if the predicted bbox and ground truth bboxes correspond to each other.
            Large number would require prediction to match ground truth bboxes better before they get counted as a true 
            positive
        Returns:
            No return.  
        """  
        tpTotal, fpTotal, fnTotal, tnTotal = 0, 0, 0, 0
        maeold, maenew = 0, 0
        if self.onDevice:
            assert appDir is not None 
        for inFile in self.imageList:
            #get ground truth boxes for the image
            gtBoxes = gtboxes_image(self.d1, os.path.basename(inFile), \
                'person',[self.imWidth, self.imHeight])
            if self.onDevice:
                self.__runSingleImageOnBoard(inFile, appDir)
                numPeople,postNMSProbs, postNMSBbox = self.__processDeviceLog()
            else:
                self.__runSingleImageOnRenode(inFile)
                _, _, numPeople,postNMSProbs, postNMSBbox \
                = self.__processLogFile()
            if numPeople == 0:
                tp, fp, fn, tn = calculate_metrics(postNMSBbox, gtBoxes, postNMSIOU)
                if showImage or saveImage:
                    self.__plotSaveImage(inFile, gtBoxes, postNMSBbox, postNMSProbs, showImage, saveImage, saveDir)
            else:
                tp, fp, fn, tn = calculate_metrics(postNMSBbox.tolist(), gtBoxes, postNMSIOU)
                if showImage or saveImage:
                    self.__plotSaveImage(inFile, gtBoxes, postNMSBbox.tolist(), postNMSProbs,showImage, saveImage, saveDir)
            tpTotal += tp
            fpTotal += fp
            fnTotal += fn
            tnTotal += tn
            maeold += abs(numPeople - tp -fn)
            maenew += fp + fn
            print(f'tp: {tp}, fp: {fp}, fn: {fn}, tn: {tn}')
            with open(outFile,'a') as f:
                f.write(f'{os.path.basename(inFile)} -->Number:') 
                f.write(f'{numPeople} tp: {tp}, fp: {fp}, fn: {fn} tn: {tn}' + '\n')
        #precision, recall, accuracy, f1, MAEold, MAEnew = \
        self.__calculateMetrics(tpTotal, fpTotal, fnTotal, tnTotal, maeold, maenew, outFile)
    
    def __calculateMetrics(self, tpTotal, fpTotal, fnTotal, tnTotal, MAEOld, MAENew, outFile ):
        """This function caclulates the aggregate metrics and writes it out to the logfile
        Args:
            tpTotal: Aggregated True Positives
            fpTotal: Aggregated False Positives
            fnTotal: Aggregated False Negatives
            tnTotal: Aggregated True Negative
            MAEOld: Mean Average Error Old Style. This is INCORRECT and should not be used. For debugging only.
            MAENew:  Mean Average Error - correct number. This will be larger than MAEOld
            outFile: Output log file for the entire run. This includes data per image as well as total data.
        Returns:
            No return.  
        """ 
        print(f'Total tp: {tpTotal}, Total tn: {fpTotal}, Total fn: {fnTotal}, Total tn: {tnTotal}') 
        # to avoid division by zero in primarily single image processing
        eps = 1e-6
        precision =  tpTotal/(tpTotal + fpTotal + eps)
        recall =  tpTotal/(tpTotal + fnTotal + eps)
        accuracy = (tpTotal + tnTotal)/(tpTotal + fnTotal + fpTotal + tnTotal + eps)
        # add 1e-6 to avoid division by zero
        f1 = 2. * precision * recall /(precision + recall + eps)
        maeold = MAEOld/self.imageCount
        maenew = MAENew/self.imageCount
        print(f'precision: {precision:.3f}, recall: {recall:.3f}, accuracy: {accuracy:.3f}, f1: {f1:.3f}')
        print(f'MAEOld: {maeold}, MAEnew: {maenew}')       
        with open(outFile,'a') as f:
            f.write(f'Total tp: {tpTotal}, Total fp: {fpTotal}, Total fn: {fnTotal}, Total tn: {tnTotal}') 
            f.write('\n')
            f.write(f'precision: {precision:.3f}, recall: {recall:.3f},')
            f.write(f' accuracy: {accuracy:.3f}, f1: {f1:.3f}')
            f.write('\n')
            f.write(f'MAEOld: {maeold:.3f}  MAENew:  {maenew:.3f}')
            f.write('\n')       

    def __plotSaveImage(self, inImage, gtBBox, predBBox, postNMSProbs, showImage, saveImage, saveDir):
        """This function plots the image, bboxes and probabilities and displays or saves the integrate
        image to a directory.
        Args:
            inImage: Original image
            gtBBox: Ground truth BBoxes if available
            predBBox: NMS output predicted BBoxes 
            postNMSProbs: NMS output probabilities corresponding to predBBox
            showImage: If true, will annotated display image for 3 seconds
            saveImage: If true, will save annotated image to saveDir
            saveDir: Directory to store images in.
        Returns:
            No return.  
        """         
        fnt = ImageFont.truetype('/usr/share/fonts/truetype/liberation2/LiberationSerif-Regular.ttf',20) 
        image = Image.open( inImage ).convert('RGB')
        image = image.resize( (self.imWidth, self.imHeight), Image.BILINEAR)
        # note that image is x,y,w,h need to convert to x, y, x+w, y+h
        draw = ImageDraw.Draw(image)
        for box in gtBBox:
            box[2] += box[0]
            box[3] += box[1]
            draw.rectangle(box, outline = 'green')
      
        for i, box in enumerate(predBBox):
            box[2] += box[0]
            box[3] += box[1]
            draw.rectangle(box, outline = 'red')
            # output is width, height
            textBoxSize = draw.textsize(f'{postNMSProbs[i]/127:.2f}', font=fnt)
            # if texttop is negative, it will not be seen in the image if it is on top of bounding box
            # in this case put the text inside the bounding box not on top
            if box[1]-textBoxSize[1] > 0 :
                textTop = box[1]-textBoxSize[1] 
                textBottom = box[1]
            else:
                textTop = 0
                textBottom = box[1]+textBoxSize[1]
            draw.rectangle([box[0], textTop, box[0]+textBoxSize[0], textBottom], fill = 'black')               
            draw.text((box[0],textTop), f'{postNMSProbs[i]/127:.2f}', font=fnt, fill='white')
        if showImage:
            image.show()
            time.sleep(3)
            # funky way to kill separate process spawned by image.show
            # use other techniques
            for proc in psutil.process_iter():
                if proc.name() == "display":
                    proc.kill()
        if saveImage:
            outFile = os.path.join(saveDir, Path(inImage).stem + '.jpg' )
            image.save(outFile, "JPEG")
