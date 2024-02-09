#pragma once

#include <string>
#include <vector>
#include "ZipArchive.h"
#include "tiny_obj_loader.h"

struct ExporterOptions {
  std::string objFile;
  std::string zipFile;

  bool direct = false;
  std::vector<float> vertices;
  std::vector<unsigned int> indices;
};

class Exporter {
public:
  explicit Exporter(const ExporterOptions& options);

  void LoadObj(const std::string& obj);
  void ExportDirect(const ExporterOptions& options);

  void WriteFile(void* data, size_t length, const std::string& filename);
  void ExportShape(tinyobj::shape_t);
private:
  std::string zipFileName;
  ZipArchive::Ptr zipFile;
  bool compress = false;
  std::string password;

  tinyobj::attrib_t attrib;
  std::vector<tinyobj::shape_t> shapes;
};