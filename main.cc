#define CROW_USE_BOOST 1
#include <crow_all.h>
#include <stdio.h>

#include <boost/program_options.hpp>
#include <cctype>
#include <ctime>
#include <iostream>
#include <memory>
#include <optional>
#include <pqxx/pqxx>
#include <regex>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

#include "api_namespace.h"
#include "functions.h"
using namespace api;
namespace po = boost::program_options;
class InvalidArguments : std::exception {};
using namespace std;
auto get_args(int argc, char** argv) -> po::variables_map {
  po::options_description desc("Allowed options");
  desc.add_options()("help", "show help")("pgdsn", po::value<std::string>(),
                                          "PostgreSQL DSN");
  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, desc), vm);
  if (vm.count("help")) {
    std::cout << desc << std::endl;
    exit(0);
  }
  if (!vm.count("pgdsn")) {
    throw InvalidArguments();
  }
  return vm;
}

auto establish_pgconnection(std::string_view dsn) -> pqxx::connection {
  return pqxx::connection(dsn.data());
}

int main(int argc, char** argv) {
  auto vm = get_args(argc, argv);
  auto dsn = vm["pgdsn"].as<std::string>();

  DB postgres("postgres", dsn);

  crow::App<crow::CORSHandler> app;
  auto& cors = app.get_middleware<crow::CORSHandler>();
  try {
    API new_api(app, "postgres", dsn);
    new_api.run();

  } catch (const std::exception& e) {
    std::cerr << "Error: " << e.what() << std::endl;
  }

  return 0;
}