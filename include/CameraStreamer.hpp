#ifndef CAMERASTREAMER_HPP
#define CAMERASTREAMER_HPP

struct NetworkConfig
{
    int port = 9999;
    char address[16] = "127.0.0.1";
};

struct CameraConfig
{
    char device[64] = "/dev/video0";
    int resolution[2] = {640, 480};
    int compression_quality = 45;
};

#endif