#include "Exporter.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>
#include <filesystem>

#include <args.hxx>

template<typename T>
std::vector<T> parseStringToVector(const std::string& inputString) {
  std::vector<T> resultVector;

  std::string trimmedString = inputString.substr(1, inputString.length() - 2);

  std::istringstream iss(trimmedString);
  std::string element;
  while (std::getline(iss, element, ',')) {
    T value = static_cast<T>(std::stof(element));
    resultVector.push_back(value);
  }

  return resultVector;
}

int main(int argc, char **argv)
{
  std::this_thread::sleep_for(std::chrono::seconds(10));

  args::ArgumentParser parser("This is a test program.", "This goes after the options.");
  args::HelpFlag help(parser, "help", "Display this help menu", {'h', "help"});

  args::ValueFlag<std::string> vertices(parser, "vertices", "Vertices for Direct pass example \"[1.0,0.3,0.7]\"", {'v'});
  args::ValueFlag<std::string> indices(parser, "indices", "Indices for Direct pass example \"[1,2,3]\"", {'i'});
  args::ValueFlag<std::string> objFile(parser, "obj file", "File to read the data from", {'f'});
  args::ValueFlag<std::string> outFile(parser, "out", "File to write the data to", {'o'});
  args::Flag direct(parser, "direct", "Pass data direct with --v and --i flags", {'d'});

  try
  {
    parser.ParseCLI(argc, argv);
  }
  catch (args::Help)
  {
    std::cout << parser;
    return 0;
  }
  catch (args::ParseError e)
  {
    std::cerr << e.what() << std::endl;
    std::cerr << parser;
    return 1;
  }
  catch (args::ValidationError e)
  {
    std::cerr << e.what() << std::endl;
    std::cerr << parser;
    return 1;
  }
  ExporterOptions options{};
  options.direct = args::get(direct);

  if (!outFile) {
    options.zipFile = "output.zip";
  }else {
    options.zipFile = args::get(outFile);
  }

  if (direct) {
    if(!vertices || !indices) {
      std::cout << "Vertices and Indices have to be provided with --v and --i flags\n";
      return -1;
    }
    options.vertices = parseStringToVector<float>(std::string(args::get(vertices).begin(), args::get(vertices).end()));
    options.indices = parseStringToVector<unsigned int>(std::string(args::get(indices).begin(), args::get(indices).end()));
  }else {
    if(!objFile) {
      std::cout << "If data isn't passed directly you need to provide an *.obj file\n";
      return -1;
    }
    options.objFile = std::filesystem::absolute(args::get(objFile));
  }

  Exporter exporter(options);
  return 0;
}
