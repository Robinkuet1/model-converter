#include "Exporter.h"
#include "ZipFile.h"
#include "streams/memstream.h"

static void printProgress(double percentage) {
  const int val = (int) (percentage * 100);
  const int lpad = (int) (percentage * 60);
  const int rpad = 60 - lpad;
  printf("\r%3d%% [%.*s%*s]", val, lpad, "||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||", rpad, "");
  fflush(stdout);
}

Exporter::Exporter(const ExporterOptions& options) : zipFileName(options.zipFile) {
  zipFile = ZipFile::Open(options.zipFile);
  if (!zipFile) {
    std::cerr << "Failed to open zip file: " << options.zipFile << std::endl;
  }

  if(!options.objFile.empty()) {
    LoadObj(options.objFile);
  }

  if(options.direct) {
    ExportDirect(options);
  }
}

void Exporter::WriteFile(void *data, size_t length, const std::string &filename) {
  ZipArchiveEntry::Ptr shapeEntry = zipFile->CreateEntry(filename);
  if (!shapeEntry) {
    std::cerr << "Failed to create zip entry for shape data" << std::endl;
  }

  imemstream dataStream(reinterpret_cast<char *>(data), length);

  //if(compress)
  shapeEntry->SetCompressionStream(dataStream); // Set the binary stream as the compression stream
  ZipFile::SaveAndClose(zipFile, zipFileName);
  zipFile = ZipFile::Open(zipFileName);
}

void Exporter::ExportShape(tinyobj::shape_t shape) {
  static int num = 0;
  printProgress((double) (num+1) / (double) shapes.size());
  std::vector<float> vertexData;

  size_t index_offset = 0;
  for (size_t f = 0; f < shape.mesh.num_face_vertices.size(); f++) {
    auto fv = size_t(shape.mesh.num_face_vertices[f]);

    for (size_t v = 0; v < fv; v++) {
      tinyobj::index_t idx = shape.mesh.indices[index_offset + v];
      tinyobj::real_t vx = attrib.vertices[3 * size_t(idx.vertex_index) + 0];
      tinyobj::real_t vy = attrib.vertices[3 * size_t(idx.vertex_index) + 1];
      tinyobj::real_t vz = attrib.vertices[3 * size_t(idx.vertex_index) + 2];

      vertexData.push_back(vx);
      vertexData.push_back(vy);
      vertexData.push_back(vz);

      // Store other data if needed
    }
    index_offset += fv;

    // per-face material
    //shape.mesh.material_ids[f];
  }
  std::string verticesName = "vertices_" + std::to_string(num) + ".bin";
  WriteFile(vertexData.data(), vertexData.size() * sizeof(float), verticesName);

  std::string indicesName = "indices_" + std::to_string(num) + ".bin";
  std::vector<unsigned int> indices;
  indices.reserve(shape.mesh.indices.size());
  for(auto & i : shape.mesh.indices) {
    indices.push_back(static_cast<unsigned int>(i.vertex_index));
  }

  WriteFile(indices.data(), indices.size() * sizeof(int), indicesName);
  num++;
}

void Exporter::LoadObj(const std::string &obj) {
  tinyobj::ObjReaderConfig reader_config;
  reader_config.mtl_search_path = obj.substr(0, obj.find_last_of('/'));

  tinyobj::ObjReader reader;

  if (!reader.ParseFromFile(obj, reader_config)) {
    if (!reader.Error().empty()) {
      std::cerr << "TinyObjReader: " << reader.Error();
    }
    exit(1);
  }

  if (!reader.Warning().empty()) {
    std::cout << "TinyObjReader: " << reader.Warning();
  }

  attrib = reader.GetAttrib();
  shapes = reader.GetShapes();
  auto &materials = reader.GetMaterials();

  std::cout << "Trying to load obj with:\n";
  std::cout << shapes.size() << " Shapes and " << materials.size() << " Materials\n";

  for (const auto &shape: shapes) {
    ExportShape(shape);
  }
}

void Exporter::ExportDirect(const ExporterOptions &options) {
  WriteFile((void*)options.vertices.data(), options.vertices.size() * sizeof(float),"vertices_0.bin");
  WriteFile((void*)options.indices.data(), options.indices.size() * sizeof(int),"indices_0.bin");
}
