#!/usr/bin/env python3

import os
import sys
import time

import boundingbox2d_labeled_pb2 as bb
import image_pb2 as image
import image_service_pb2_grpc as imageService
import label_with_instance_pb2 as labelWithInstance
import meta_operations_pb2_grpc as metaOperations
import numpy as np
import projectCreation_pb2 as projectCreation
import tf_service_pb2_grpc as tfService
import transform_stamped_pb2 as tf

script_dir = os.path.dirname(__file__)
util_dir = os.path.join(script_dir, '..')
sys.path.append(util_dir)
import util

channel = util.get_gRPC_channel("local")

stub = imageService.ImageServiceStub(channel)
stubTf = tfService.TfServiceStub(channel)
stubMeta = metaOperations.MetaOperationsStub(channel)

# create new project
creation = projectCreation.ProjectCreation(name="LabeledImagesInGrid", mapFrameId="map")
projectCreated = stubMeta.CreateProject(creation)
projectname = projectCreated.uuid

# get and save the time
theTime = int(time.time())

# create 9 grid cells
for k in range(9):
    n = 0
    # create an image more per cell (1 image in first cell; 9 images in 9th cell...)
    while n <= k:
        n = n + 1
        theImage = image.Image()

        # create rgb with shifted rgb values in each cell
        rgb = []
        lim = 256
        for i in range(lim):
            for j in range(lim):
                x = float(i) / lim
                y = float(j) / lim
                z = float(j) / lim
                r = np.ubyte((x * 255.0 + k * 100) % 255)
                g = np.ubyte((y * 255.0 + k * 100) % 255)
                b = np.ubyte((z * 255.0 + k * 100) % 255)
                # print(r, g, b)
                rgb.append(r)
                rgb.append(g)
                rgb.append(b)

        # write the header
        theImage.header.frame_id = "camera"
        # increment timestamp for each cell for later matching with tf
        theImage.header.stamp.seconds = theTime + k
        theImage.header.stamp.nanos = 0
        theImage.header.uuid_project = projectname
        theImage.height = lim
        theImage.width = lim
        theImage.encoding = "rgb8"
        theImage.step = 3 * lim
        theImage.data = bytes(rgb)

        # write labeled bounding boxes
        bb1 = bb.BoundingBox2DLabeled()
        for i in range(1, n + 1):
            bb1.labelWithInstance.label = "testlabel" + str(i)
            bb1.boundingBox.point_min.x = 0.01 + i / 10
            bb1.boundingBox.point_min.y = 0.02 + i / 10
            bb1.boundingBox.point_max.x = 0.03 + i / 10
            bb1.boundingBox.point_max.y = 0.04 + i / 10
            theImage.labels_bb.append(bb1)

        # write general labels
        label = labelWithInstance.LabelWithInstance()
        label.label = "testlabelgeneral"
        theImage.labels_general.append(label)

        # transfer image
        uuidImg = stub.TransferImage(theImage)

        print("uuid of transfered img: " + uuidImg.message)

# create tf with data valid for all following tfs
theTf = tf.TransformStamped()
theTf.header.frame_id = "map"
theTf.header.uuid_project = projectname
theTf.child_frame_id = "camera"
theTf.transform.translation.z = 0
theTf.transform.rotation.x = 0
theTf.transform.rotation.y = 0
theTf.transform.rotation.z = 0
theTf.transform.rotation.w = 1

# create tf for a 3x3 grid
# x,y -> [0,2]
for x in range(3):
    for y in range(3):
        # increment time per cell
        theTf.header.stamp.seconds = theTime + (x * 3 + y)
        # write x and y coordinates
        theTf.transform.translation.x = x
        theTf.transform.translation.y = y
        # transfer tf
        stubTf.TransferTransformStamped(theTf)
