#!/user/bin/env python3

import os
import sys
import time

import flatbuffers
import numpy as np
from fb import Header, LabelWithInstance, PointCloud2, PointField, Timestamp
from fb import point_cloud_service_grpc_fb as pointCloudService

script_dir = os.path.dirname(__file__)
util_dir = os.path.join(script_dir, '..')
sys.path.append(util_dir)
import util
import util_fb


def createTimeStamp(builder, time):
    '''Create a flatbuffers time stamp'''
    Timestamp.Start(builder)
    Timestamp.AddSeconds(builder, time)
    Timestamp.AddNanos(builder, 0)
    return Timestamp.End(builder)


# ToDo add doc for input parameter
def createPointField(builder, name, offset, datatype, count):
    '''Creates point field to describe a channel in the point cloud'''
    nameStr = builder.CreateString(name)
    PointField.Start(builder)
    PointField.AddName(builder, nameStr)
    PointField.AddOffset(builder, offset)
    PointField.AddDatatype(builder, datatype)
    PointField.AddCount(builder, count)
    return PointField.End(builder)


def createPointFields(builder, fields="xyz", datatype=7, count=1):
    '''Creates fields for all channels of the point cloud'''
    fieldsList = []
    offset = 0
    for field in fields:
        fieldsList.append(createPointField(builder, field, offset, datatype, count))
        offset += 4
    return fieldsList


def listToPointFieldVector(builder, pointFieldList):
    '''Creates fields vector from a list of point fields'''
    PointCloud2.StartFieldsVector(builder, len(pointFieldList))
    for pointField in reversed(pointFieldList):
        builder.PrependUOffsetTRelative(pointField)
    return builder.EndVector()


def createHeader(builder, time, frame, projectUuid):
    '''Creates a flatbuffers header'''
    frameIdStr = builder.CreateString(frame)
    projectUuidStr = builder.CreateString(projectUuid)
    timeStamp = createTimeStamp(builder, time)
    Header.Start(builder)
    Header.AddFrameId(builder, frameIdStr)
    Header.AddStamp(builder, timeStamp)
    Header.AddUuidProject(builder, projectUuidStr)
    return Header.End(builder)


def createGeneralLabel(builder, label="testlabel", instanceUuid="testUuid"):
    '''Creates a label with an instance uuid'''
    labelStr = builder.CreateString(label)
    instanceUuidStr = builder.CreateString(instanceUuid)
    LabelWithInstance.Start(builder)
    LabelWithInstance.AddLabel(builder, labelStr)
    LabelWithInstance.AddInstanceUuid(builder, instanceUuidStr)
    return LabelWithInstance.End(builder)


def createGeneralLabels(builder, numOf=10):
    '''Creates 'numOf' general labels'''
    labelsGeneral = []
    for i in range(numOf):
        labelsGeneral.append(createGeneralLabel(builder, "testlabel" + str(i), "testUuid" + str(i)))
    return labelsGeneral


def listToGeneralLabelsVector(builder, generalLabelList):
    '''Creates labels general vector from a list of labels'''
    PointCloud2.StartLabelsGeneralVector(builder, len(generalLabelList))
    for label in reversed(generalLabelList):
        builder.PrependUOffsetTRelative(label)
    return builder.EndVector()


def createPointCloud(builder, header, height=960, width=1280):
    '''Creates the point cloud message'''
    pointFields = createPointFields(builder)
    pointFieldVector = listToPointFieldVector(builder, pointFields)

    labelsGeneral = createGeneralLabels(builder)
    labelsGeneralVector = listToGeneralLabelsVector(builder, labelsGeneral)

    # generate ordered point cloud with dim (height, width, 3)
    points = np.random.randn(height, width, 3).astype(np.float32)
    pointsVector = builder.CreateByteVector(points.tobytes())

    PointCloud2.Start(builder)
    PointCloud2.AddHeader(builder, header)
    PointCloud2.AddHeight(builder, points.shape[0])
    PointCloud2.AddWidth(builder, points.shape[1])
    PointCloud2.AddIsBigendian(builder, False)
    PointCloud2.AddPointStep(builder, 12)
    PointCloud2.AddRowStep(builder, points.shape[0] * points.shape[1])
    PointCloud2.AddFields(builder, pointFieldVector)
    PointCloud2.AddData(builder, pointsVector)
    PointCloud2.AddLabelsGeneral(builder, labelsGeneralVector)
    return PointCloud2.End(builder)


def createPointClouds(projectUuid, numOf=10):
    '''Creates 'numOf' point cloud messages'''
    theTime = int(time.time())
    for i in range(numOf):
        print(f"Send point cloud: {str(i)}")
        builder = flatbuffers.Builder(1024)

        header = createHeader(builder, theTime + i, "camera", projectUuid)
        pointCloudMsg = createPointCloud(builder, header)
        builder.Finish(pointCloudMsg)
        yield bytes(builder.Output())


channel = util.get_gRPC_channel()
stub = pointCloudService.PointCloudServiceStub(channel)

projectUuid = util_fb.get_or_create_project(channel, "testproject", True)

responseBuf = stub.TransferPointCloud2(createPointClouds(projectUuid))
