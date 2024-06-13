#include <gmock/gmock.h>
#include <Poco/Path.h>
#include <Poco/File.h>
#include <Poco/FileStream.h>
#include "FileReaderImpl.hpp"
#include <fstream>


using namespace testing;
using filesysteminput::FileReaderImpl;
using Poco::File;
using Poco::Path;

const std::string CONTENT{ "Example File.\nHello World!" };
const std::string FILENAME{"FileReaderImplFile.txt"};

TEST(FileReaderImplTest, ReadReturnsContentOfFile) {
	File exampleFile{ Path{Path::tempHome(), FILENAME} };

	Poco::FileOutputStream fos{ exampleFile.path() };
	fos << CONTENT;
	fos.close();


	const filesysteminput::FileReaderPtr fileReader = std::make_shared<FileReaderImpl>();
	const auto expectedContent = fileReader->read(exampleFile.path());

	ASSERT_THAT(expectedContent, Eq(CONTENT));

	exampleFile.remove();
}

TEST(FileReaderImplTest, ThrowsExceptionIfFileCouldNotRead) {
	File exampleFile{ Path{Path::tempHome(), FILENAME} };
	auto fHandle = CreateFileA(exampleFile.path().c_str(), GENERIC_READ, 0, NULL, CREATE_ALWAYS, 0, NULL);

	const filesysteminput::FileReaderPtr fileReader = std::make_shared<FileReaderImpl>();
	try {
		const auto expectedContent = fileReader->read(exampleFile.path());
		CloseHandle(fHandle);
		exampleFile.remove();
		FAIL();
	} catch(const Poco::FileAccessDeniedException&) {
		CloseHandle(fHandle);
		exampleFile.remove();
		SUCCEED();
	}
}