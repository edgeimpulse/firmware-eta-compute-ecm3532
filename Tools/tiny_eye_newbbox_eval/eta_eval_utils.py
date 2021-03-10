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

import json
import copy

#eta_testset_person.json is a dictionary with image file names as keys
#and their associated image size, labels and ground truth boxes .
#
#Dictionary format is 
#{"image_name0": [image size, 
#                 {"label_name_0": [ [xmin0, ymin0, w0, h0], 
#                                    [xmin1, ymin1, w1, h1],
#                                  ...
#                                  ],
#                  "label_name_1": ...
#                 }
#                ]
# "image_name1": ...
#}
#                 
#Original images are of different sizes.
#Ground truth boxes are wth respect to the original image size.
#Box coordinates are [xmin, ymin, w, h] as in the Coco format,
#where (xmin, ymin) is the upper left corner of the bounding box

#Functions for getting image names, labels and bounding boxes
def load_eta_testset(json_file):
#json_file is the json file containing the groundtruth
    with open(json_file) as fid:
        eta_testset = json.load(fid)
    return eta_testset

def load_prediction_json(prediction_file):
    with open(prediction_file) as fid:
        return json.load(fid)

def num_images(gt_dict):
    return len(gt_dict)

def list_all_images(gt_dict):
    return list(gt_dict)

def size_image(gt_dict, image_name):
#Returns the size of the image [W, H]. 
#Note that ground truth boxes are with respect to this image size.
    if image_name in gt_dict:
        size = list(gt_dict[image_name][0])
    else:
        size = [0, 0]
    return size

def labels_image(gt_dict, image_name):
#Returns all labels for a given image
    if image_name in gt_dict:
        labels = list(gt_dict[image_name][1])
    else:
        labels = []
    return labels

def gtboxes_image(gt_dict, image_name, desired_label, resize=[]):
#Returns all ground truth boxes associated with a label for an image
#gtbox coordinates = [xmin, ymin, w, h]; COCO format
#Coordinates of gtboxes can be resized if resize parameter is specified.
#resize = [W, H]
    bboxes = []
    if image_name in gt_dict.keys():
        #bboxes = gt_dict[image_name][1][desired_label]
        bboxes_dict = gt_dict[image_name][1]
        # if desired_label in foo:
        if desired_label in bboxes_dict.keys():
            bboxes = copy.deepcopy(bboxes_dict[desired_label])
    #resize boxes 
    assert len(resize) == 2
    img_size = size_image(gt_dict, image_name)
    for bbox in bboxes:
        bbox[0] = (bbox[0] / img_size[0]) * resize[0]
        bbox[1] = (bbox[1] / img_size[1]) * resize[1]
        bbox[2] = (bbox[2] / img_size[0]) * resize[0]
        bbox[3] = (bbox[3] / img_size[1]) * resize[1]

    return bboxes

#Functions for calculating metrics
def areaBox( box ):
#box = [xmin, ymin, w, h]; COCO format
    return (box[2] * box[3])

def areaIntersection( box1, box2 ):
#box coordinates [ xmin, ymin, w, h] ; COCO format
    xmin = max( box1[0], box2[0] )
    ymin = max( box1[1], box2[1] )
    xmax = min( box1[0]+box1[2], box2[0]+box2[2] ) #xmin+w
    ymax = min( box1[1]+box1[3], box2[1]+box2[3] ) #ymin+h
    if ymin < ymax  and  xmin < xmax:
        aa = (xmax - xmin)*(ymax-ymin)
    else:
        aa = 0
    return aa

def calculate_iou(box1, box2):
#box coordinates [ xmin, ymin, w, h] ; COCO format
    area1 = areaBox(box1)
    area2 = areaBox(box2)
    intersect = areaIntersection(box1, box2)
    iou = intersect / ( area1 + area2 - intersect )
    if iou > 1 or iou < 0:
        print("ERROR IN IOU")
    return iou

def calculate_metrics(infered_boxes_list, ground_truth_boxes_list, iou_th = 0):
    len_gt = len(ground_truth_boxes_list)
    len_pred = len(infered_boxes_list)
    TP = 0 #true positives
    FP = 0 #false postives
    FN = 0 #false negatives
    TN = 0 #true negative
    # gtbox_list = copy.deepcopy(ground_truth_boxes_list)
    visited = [0] * len(ground_truth_boxes_list)
    for pred_i, pred_box in enumerate(infered_boxes_list):
        max_iou = 0
        det_i = -1
        visited_i = -1
        # y = -1
        for gt_i, gt_box in enumerate(ground_truth_boxes_list):
            # y += 1
            iou = calculate_iou(pred_box, gt_box)
            if iou > max_iou:
                # y_max = y 
                max_iou = iou
                det_i = pred_i # eqv to y_max = y, but what is the use?
                visited_i = gt_i
                # max_box = gt
        # keep an indicater array which identifies the used up gt box
        if max_iou > iou_th and visited[visited_i] == 0:
            TP += 1
            # gtbox_list.remove(max_box)
            visited[visited_i] = 1
    FP = max([len_pred - TP, 0])
    FN = max([len_gt - TP, 0])
    #if there are no ground truth boxes and no boxes are predicted, 
    #then it is declared as a true negative
    if (len_gt == 0) and (TP + FP == 0):
        TN = 1
    return TP, FP, FN, TN


'''
#test code
eta_testset =  load_eta_testset('/home/hari/test_images_source/scripts/')
print('num images = ', num_images(eta_testset))
images = list_all_images(eta_testset)
#print('all images = ', images)
N = 23
image = images[N] #pick an image
print('Nth image name ', N, image)
print('image size ', size_image(eta_testset, image))
print('labels ', labels_image(eta_testset, image)) 
labels = labels_image(eta_testset, image)
for label in labels:
    print('label ', label)
    boxes = gtboxes_image(eta_testset, image, label)
    print('bboxes =', boxes)

if 'person' in labels:
    boxes = gtboxes_image(eta_testset, image, 'person')
print('bboxes for person = ', boxes)
'''
