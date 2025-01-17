# SEEREP

![unstable](https://img.shields.io/badge/stability-unstable-orange)
[![License](https://img.shields.io/badge/license-BSD_3-brightgreen)](./LICENSE)
[![catkin build workflow](https://github.com/agri-gaia/seerep/actions/workflows/main.yml/badge.svg)](https://github.com/agri-gaia/seerep/actions)
[![pre-commit](https://img.shields.io/badge/pre--commit-enabled-brightgreen?logo=pre-commit&logoColor=white)](./.pre-commit-config.yaml)
[![Docker](https://img.shields.io/badge/Docker-enabled-blue?logo=docker)](./docker)
 <!-- ![Architecture](https://img.shields.io/badge/Architecture-x86-blue) -->

## Table of Contents

- [Maintainer](#maintainer)
- [General](#general)
- [Quick Start](#quick-start)
- [Documentation](#documentation)

<!-- - [Related Publications](#related-publications) -->

## Maintainer

[Mark Höllmann](https://github.com/mhoellmann)\
[mark.hoellmann@dfki.de](mailto:mark.hoellmann@dfki.de)

[German Research Center for Artificial Intelligence\
DFKI Niedersachsen\
Plan-Based Robot Control](https://www.dfki.de/en/web/research/research-departments/plan-based-robot-control)

## General

The large amount of temporally and spatially high resolution sensor data of autonomous mobile robots that have to be
collected in today’s systems require a structured and, above all, efficient management and storage already during the
robot mission. We present SEEREP: A _Spatio-TemporalSemantic Environment Representation for Autonomous Mobile Robots_.
SEEREP deals with spatial, temporal and semantic linked data at once and provides an efficient query interface for all
three modalities that can be combined for high-level analyses. It supports the most popular robotic sensor data such as
images and point clouds, as well as sensor and robot coordinate frames changing over time. Furthermore, SEEREP provides
an efficient HDF5-based storage system running on the robot during operation compatible with ROS and the corresponding
sensor message definitions. The compressed HDF5 data backend can be transferred efficiently to an application server
with a running SEEREP query server providing gRPC interfaces with Protobuf and Flattbuffer message types. Partially
unstructured environments that changes over time, as for example agricultural environments, can be understood based on
high-level planning and reasoning systems using the SEEREP query server.

## Quick Start

To deploy the server locally the following `docker run` command or the following `docker-compose.yml` can be used.

```
docker run \
  --volume=seerep-data:/mnt/seerep-data \
  --publish=9090 \
  --name=seerep_server \
  --tty \
  ghcr.io/agri-gaia/seerep_server:latest \
  --data-folder=/mnt/seerep-data
```

Example docker-compose.yml:

```
version: "3.9"
services:
  seerep:
    image: ghcr.io/agri-gaia/seerep_server:latest
    tty: true
    container_name: seerep_server
    command:
      # define data-dir for seerep-server
      - "--data-folder=/mnt/seerep-data"
    ports:
      # the gRPC port
      - 9090
    volumes:
      # persist the data folder
      - seerep-data:/mnt/seerep-data
volumes:
  seerep-data:
```

## Documentation

The general [MkDocs](https://www.mkdocs.org/) based documentation of this project is available
[here](https://agri-gaia.github.io/seerep/mkdocs). There the general architecture is described, installation
instructions are given and basic tutorials are given.

The [Doxygen](https://doxygen.nl/) based code documentation can be found [here](https://agri-gaia.github.io/seerep/doxygen).

<!-- ## Related Publications -->
