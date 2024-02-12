// Opencv
#include <opencv2/opencv.hpp>

// Arguments parsing
#include <cxxopts.hpp>

// socket related
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

// standard
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <vector>

// Project related
#include <CameraStreamer.hpp>

#define BUFFER_SIZE 64

using namespace cv;

NetworkConfig networkConfig;
CameraConfig cameraConfig;

int sock_fd;
sockaddr_in server_addr;
socklen_t addr_size;
char buffer[BUFFER_SIZE];

void process_args(int c, char **v) {
  cxxopts::Options options("CameraStreamer", "Arguments paerser");
  options.add_options()(
      "d,device", "camera device",
      cxxopts::value<std::string>()->default_value("/dev/video0"))(
      "w,width", "frame width",
      cxxopts::value<int>())("h,height", "frame height", cxxopts::value<int>())(
      "q,quality", "compression quality",
      cxxopts::value<int>())("p,port", "network port", cxxopts::value<int>())(
      "a,address", "network address",
      cxxopts::value<std::string>()->default_value("127.0.0.1"));
  auto result = options.parse(c, v);
  if (result.count("help")) {
    fprintf(stdout, "help\n");
    exit(0);
  }
  if (result.count("device")) {
    strcpy(cameraConfig.device, result["device"].as<std::string>().c_str());
  } else {
    exit(-1);
  }
  if (result.count("width")) {
    cameraConfig.resolution[0] = result["width"].as<int>();
  }
  if (result.count("height")) {
    cameraConfig.resolution[1] = result["height"].as<int>();
  }
  if (result.count("quality")) {
    cameraConfig.compression_quality = result["quality"].as<int>();
  }
  if (result.count("port")) {
    networkConfig.port = result["port"].as<int>();
  } else {
    exit(-1);
  }
  if (result.count("address")) {
    strcpy(networkConfig.address, result["address"].as<std::string>().c_str());
  } else {
    exit(-1);
  }
}

int main(int argc, char **argv) {
  process_args(argc, argv);
  fprintf(stdout, "Camera Streamer v0.1\n");

  sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
  if (sock_fd < 0) {
    fprintf(stdout, "Failed to create socket\n");
    return -1;
  }

  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(networkConfig.port);
  server_addr.sin_addr.s_addr = inet_addr(networkConfig.address);
  memset(server_addr.sin_zero, '\0', sizeof(server_addr.sin_zero));

  if (bind(sock_fd, (struct sockaddr *)&server_addr, sizeof(server_addr))) {
    fprintf(stdout, "Failed to bind server\n");
    return -1;
  }

  addr_size = sizeof(server_addr);

  VideoCapture source(cameraConfig.device);

  if (!source.isOpened()) {
    fprintf(stderr, "failed to open camera device %s\n", cameraConfig.device);
    return -1;
  }

  Mat frame, resized_frame;
  Size frame_size;
  std::vector<uchar> encoded_frame;

  fprintf(stdout, "default resolution:\n\twidth: %f\theight: %f\n",
          source.get(CAP_PROP_FRAME_WIDTH), source.get(CAP_PROP_FRAME_HEIGHT));

  char key = '\0';

  while (true) {
    recvfrom(sock_fd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&server_addr,
             &addr_size);
    if (strcmp(buffer, "data") == 0) {
      source.read(frame);
      if (frame.empty()) {
        fprintf(stderr, "empty frame\n");
        continue;
      }

      imencode(".jpg", frame, encoded_frame);
      // imshow("frame", frame);

      int encoded_frame_size = encoded_frame.size();
      sendto(sock_fd, &encoded_frame_size, sizeof(encoded_frame_size), 0,
             (struct sockaddr *)&server_addr, addr_size);

      sendto(sock_fd, encoded_frame.data(), encoded_frame_size, 0,
             (struct sockaddr *)&server_addr, addr_size);
    }
    memset(buffer, 0, BUFFER_SIZE);
    key = waitKey(1);
    if (key == 'q') {
      break;
    }
  }
  close(sock_fd);
  source.release();
  destroyAllWindows();

  return 0;
}