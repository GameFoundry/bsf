//************************************ bs::framework - Copyright 2018 Marko
//Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms.
//This notice is not to be removed. ***********//

#include "Testing/BsTestSuite.h"

#include "Debug/BsDebug.h"
#include "Error/BsException.h"
#include "FileSystem/BsFileSystem.h"

#include <algorithm>
#include <fstream>

namespace bs {
const String testDirectoryName = "FileSystemTestDirectory/";

class FileSystemTestSuite : public bs::Test {
public:
  void SetUp() override {
    Test::SetUp();

    mTestDirectory = getTestDirectory();
  }

  static Path getTestDirectory() {
    return FileSystem::getWorkingDirectoryPath() + testDirectoryName;;
  }

  static void SetUpTestSuite() {
    // call parent test suite setup first.
    Test::SetUpTestSuite();

    auto mTestDirectory = getTestDirectory();
    if (FileSystem::exists(mTestDirectory)) {
      BS_EXCEPT(
          InternalErrorException,
          String("Directory '") + testDirectoryName +
              "' should not already exist; you should remove it manually.");
    } else {
      FileSystem::createDir(mTestDirectory);
      if (!FileSystem::exists(mTestDirectory)) {
        BS_EXCEPT(
          InternalErrorException,
          "FileSystemTestSuite::startUp(): test directory creation failed");
      }
    }
  }

  static void TearDownTestSuite() {
    auto mTestDirectory = getTestDirectory();
    FileSystem::remove(mTestDirectory, true);
    if (FileSystem::exists(mTestDirectory)) {
      LOGERR("FileSystemTestSuite failed to delete '" +
             mTestDirectory.toString() + "', you should remove it manually.");
    }

    // call parent test suite teardown last.
    Test::TearDownTestSuite();
  }
public:
  Path mTestDirectory;
};

  void createFile(Path path, String content)
  {
  std::ofstream fs;
  fs.open(path.toPlatformString().c_str());
  fs << content;
  fs.close();
}

void createEmptyFile(Path path) { createFile(path, ""); }

String readFile(Path path) {
  String content;
  std::ifstream fs;
  fs.open(path.toPlatformString().c_str());
  fs >> content;
  fs.close();
  return content;
}

TEST_F(FileSystemTestSuite, testExists_yes_file) {
  Path path = mTestDirectory + "plop";
  createEmptyFile(path);
  ASSERT_TRUE(FileSystem::exists(path));
  FileSystem::remove(path);
}

TEST_F(FileSystemTestSuite, testExists_yes_dir) {
  Path path = mTestDirectory + "plop/";
  FileSystem::createDir(path);
  ASSERT_TRUE(FileSystem::exists(path));
  FileSystem::remove(path);
}

TEST_F(FileSystemTestSuite, testExists_no) {
  ASSERT_TRUE(!FileSystem::exists(Path("this-file-does-not-exist")));
}

TEST_F(FileSystemTestSuite, testGetFileSize_zero) {
  Path path = mTestDirectory + "file-size-test-1";
  createEmptyFile(path);
  ASSERT_TRUE(FileSystem::getFileSize(path) == 0);
  FileSystem::remove(path);
}

TEST_F(FileSystemTestSuite, testGetFileSize_not_zero) {
  Path path = mTestDirectory + "file-size-test-2";
  createFile(path, "0123456789");
  ASSERT_TRUE(FileSystem::getFileSize(path) == 10);
  FileSystem::remove(path);
}

TEST_F(FileSystemTestSuite, testIsFile_yes) {
  Path path = mTestDirectory + "some-file-1";
  createEmptyFile(path);
  ASSERT_TRUE(FileSystem::isFile(path));
}

TEST_F(FileSystemTestSuite, testIsFile_no) {
  Path path = mTestDirectory + "some-directory-1/";
  FileSystem::createDir(path);
  ASSERT_TRUE(!FileSystem::isFile(path));
}

TEST_F(FileSystemTestSuite, testIsDirectory_yes) {
  Path path = mTestDirectory + "some-directory-2/";
  FileSystem::createDir(path);
  ASSERT_TRUE(FileSystem::isDirectory(path));
}

TEST_F(FileSystemTestSuite, testIsDirectory_no) {
  Path path = mTestDirectory + "some-file-2";
  createEmptyFile(path);
  ASSERT_TRUE(!FileSystem::isDirectory(path));
}

TEST_F(FileSystemTestSuite, testRemove_file) {
  Path path = mTestDirectory + "file-to-remove";
  createEmptyFile(path);
  ASSERT_TRUE(FileSystem::exists(path));
  FileSystem::remove(path);
  ASSERT_TRUE(!FileSystem::exists(path));
}

TEST_F(FileSystemTestSuite, testRemove_directory) {
  Path path = mTestDirectory + "directory-to-remove/";
  FileSystem::createDir(path);
  ASSERT_TRUE(FileSystem::exists(path));
  FileSystem::remove(path, true);
  ASSERT_TRUE(!FileSystem::exists(path));
}

TEST_F(FileSystemTestSuite, testMove) {
  Path source = mTestDirectory + "move-source-1";
  Path destination = mTestDirectory + "move-destination-1";
  createFile(source, "move-data-source-1");
  ASSERT_TRUE(FileSystem::exists(source));
  ASSERT_TRUE(!FileSystem::exists(destination));
  FileSystem::move(source, destination);
  ASSERT_TRUE(!FileSystem::exists(source));
  ASSERT_TRUE(FileSystem::exists(destination));
  ASSERT_TRUE(readFile(destination) == "move-data-source-1");
}

TEST_F(FileSystemTestSuite, testMove_overwrite_existing) {
  Path source = mTestDirectory + "move-source-2";
  Path destination = mTestDirectory + "move-destination-2";
  createFile(source, "move-data-source-2");
  createFile(destination, "move-data-destination-2");
  ASSERT_TRUE(FileSystem::exists(source));
  ASSERT_TRUE(FileSystem::exists(destination));
  FileSystem::move(source, destination, true);
  ASSERT_TRUE(!FileSystem::exists(source));
  ASSERT_TRUE(FileSystem::exists(destination));
  ASSERT_TRUE(readFile(destination) == "move-data-source-2");
}

TEST_F(FileSystemTestSuite, testMove_no_overwrite_existing) {
  Path source = mTestDirectory + "move-source-3";
  Path destination = mTestDirectory + "move-destination-3";
  createFile(source, "move-data-source-3");
  createFile(destination, "move-data-destination-3");
  ASSERT_TRUE(FileSystem::exists(source));
  ASSERT_TRUE(FileSystem::exists(destination));
  FileSystem::move(source, destination, false);
  ASSERT_TRUE(FileSystem::exists(source));
  ASSERT_TRUE(FileSystem::exists(destination));
  ASSERT_TRUE(readFile(destination) == "move-data-destination-3");
}

TEST_F(FileSystemTestSuite, testCopy) {
  Path source = mTestDirectory + "copy-source-1";
  Path destination = mTestDirectory + "copy-destination-1";
  createFile(source, "copy-data-source-1");
  ASSERT_TRUE(FileSystem::exists(source));
  ASSERT_TRUE(!FileSystem::exists(destination));
  FileSystem::copy(source, destination);
  ASSERT_TRUE(FileSystem::exists(source));
  ASSERT_TRUE(FileSystem::exists(destination));
  ASSERT_TRUE(readFile(source) == "copy-data-source-1");
  ASSERT_TRUE(readFile(destination) == "copy-data-source-1");
}

TEST_F(FileSystemTestSuite, testCopy_overwrite_existing) {
  Path source = mTestDirectory + "copy-source-2";
  Path destination = mTestDirectory + "copy-destination-2";
  createFile(source, "copy-data-source-2");
  createFile(destination, "copy-data-destination-2");
  ASSERT_TRUE(FileSystem::exists(source));
  ASSERT_TRUE(FileSystem::exists(destination));
  FileSystem::copy(source, destination, true);
  ASSERT_TRUE(FileSystem::exists(source));
  ASSERT_TRUE(FileSystem::exists(destination));
  ASSERT_TRUE(readFile(source) == "copy-data-source-2");
  ASSERT_TRUE(readFile(destination) == "copy-data-source-2");
}

TEST_F(FileSystemTestSuite, testCopy_no_overwrite_existing) {
  Path source = mTestDirectory + "copy-source-3";
  Path destination = mTestDirectory + "copy-destination-3";
  createFile(source, "copy-data-source-3");
  createFile(destination, "copy-data-destination-3");
  ASSERT_TRUE(FileSystem::exists(source));
  ASSERT_TRUE(FileSystem::exists(destination));
  FileSystem::copy(source, destination, false);
  ASSERT_TRUE(FileSystem::exists(source));
  ASSERT_TRUE(FileSystem::exists(destination));
  ASSERT_TRUE(readFile(source) == "copy-data-source-3");
  ASSERT_TRUE(readFile(destination) == "copy-data-destination-3");
}

#define CONTAINS(v, e) (std::find(v.begin(), v.end(), e) != v.end())

TEST_F(FileSystemTestSuite, testGetChildren) {
  Path path = mTestDirectory + "get-children-test/";
  FileSystem::createDir(path);
  FileSystem::createDir(path + "foo/");
  FileSystem::createDir(path + "bar/");
  FileSystem::createDir(path + "baz/");
  createEmptyFile(path + "ga");
  createEmptyFile(path + "bu");
  createEmptyFile(path + "zo");
  createEmptyFile(path + "meu");
  Vector<Path> files, directories;
  FileSystem::getChildren(path, files, directories);
  ASSERT_TRUE(files.size() == 4);
  ASSERT_TRUE(CONTAINS(files, path + "ga"));
  ASSERT_TRUE(CONTAINS(files, path + "bu"));
  ASSERT_TRUE(CONTAINS(files, path + "zo"));
  ASSERT_TRUE(CONTAINS(files, path + "meu"));
  ASSERT_TRUE(directories.size() == 3);
  ASSERT_TRUE(CONTAINS(directories, path + "foo"));
  ASSERT_TRUE(CONTAINS(directories, path + "bar"));
  ASSERT_TRUE(CONTAINS(directories, path + "baz"));
}

TEST_F(FileSystemTestSuite, testGetLastModifiedTime) {
  std::time_t beforeTime;
  time(&beforeTime);

  Path path = mTestDirectory + "blah1234";
  createFile(path, "blah");
  std::time_t mtime = FileSystem::getLastModifiedTime(path);
  ASSERT_TRUE(mtime >= beforeTime);
  ASSERT_TRUE(mtime <= beforeTime + 10);
}

TEST_F(FileSystemTestSuite, testGetTempDirectoryPath) {
  Path path = FileSystem::getTempDirectoryPath();
  /* No judging. */
  ASSERT_TRUE(!path.toString().empty());
}
}  // namespace bs
