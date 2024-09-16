#include "MediaServer.hh"

int main(int argc, char** argv)
{
  spdlog::set_level(spdlog::level::debug);

  my::MediaServer server;
  return server.run();
}
