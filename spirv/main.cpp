#include <cstdlib>
#include <sstream>
#include <iostream>
#include <filesystem>

int main(int argc, char* argv[])
{
  std::string solutionDir = argv[1];
  std::string projectDir = argv[2];

  std::filesystem::path const solutionPath = solutionDir;
  std::filesystem::path const projectPath = projectDir;

  if (std::filesystem::exists(projectPath / "compiled"))
    std::filesystem::remove_all(projectPath / "compiled");

  std::filesystem::create_directory(projectPath / "compiled");

  for (auto const& file : std::filesystem::recursive_directory_iterator{ projectPath / "shaders" })
  {
    if (file.is_directory())
      continue;

    std::string const outputDir = projectDir + "compiled";

    if (!std::filesystem::exists(outputDir))
      std::filesystem::create_directories(outputDir);

    std::ostringstream oss;

    oss << solutionDir << "glslangValidator.exe -G100 "
        << file.path().string()
        << " -o " << outputDir << "\\" << file.path().filename().string();

    std::system(oss.str().c_str());
  }

  return 0;
}