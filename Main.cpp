#include <iostream>
#include <vector>
#include <string>
#include <array>

#include <Windows.h>

#include "Log.h"

struct Options 
{
	std::vector<std::string> workPaths;
	bool sizeSuffixes = false;
	bool silent = false;

	std::array<char, 5> suffixes = { 'B', 'K', 'M', 'G', 'T' };
};

struct FileSize 
{
	double value;
	char suffix;
};

auto retrieveArgs(int argc, const char* argv[])
{
	std::vector<std::string> args(argc);
	for (auto i = 0; i < argc; i++)
	{
		args[i] = std::string(argv[i]);
	}
	return args;
}

auto parseArgs(std::vector<std::string>& args) {
	Options opt;
	for (auto i = 1; i < args.size(); i++) 
	{
		const auto& arg = args[i];

		if (arg.empty()) continue;
		if (arg.front() == '-')
		{
			if (arg == "-h")
			{
				opt.sizeSuffixes = true;
				continue;
			}
			else if (arg == "-s")
			{
				opt.silent = true;
			}
			else
			{
				if (opt.silent)
				{
					exit(2);
				}
				Log::warning() << "Unknown parameter " << arg << "\n";
			}
		}
		else
		{
			opt.workPaths.push_back(arg);
		}
	}
	return opt;
}

std::string getWildcard(const std::string& path)
{
	if (path.empty())
		return "*";
	if (path.back() == '/' || path.back() == '\\')
		return path + "*";
	return path + "/*";
}

std::string joinPaths(const std::string& path, const std::string& otherPath)
{
	if (path.empty())
		return otherPath;
	if (otherPath.empty())
		return path;
	if (path.back() == '/' || path.back() == '\\')
	{
		if (otherPath.front() == '/' || otherPath.front() == '\\')
			return path + otherPath.substr(1);
		return path + otherPath;
	}
	else 
	{
		if (otherPath.front() == '/' || otherPath.front() == '\\')
			return path + otherPath;
		return path + "/" + otherPath;
	}
}

FileSize getFileSize(const Options& opts, uint64_t iValue)
{
	if (!opts.sizeSuffixes)
		return { double(iValue), 'B' };

	double value = iValue;
	for (auto i = 0; i < opts.suffixes.size(); i++) 
	{
		if (value < 1024)
			return { value, opts.suffixes[i] };
		value /= 1024.0;
	}

	return { value, opts.suffixes.back() };
}

uint64_t handlePath(const Options& opts, const std::string& path, const std::string &relativePath = "./")
{
	WIN32_FIND_DATAA selfData;
	auto selfFind = FindFirstFileA(path.c_str(), &selfData);

	if (selfFind == INVALID_HANDLE_VALUE)
	{
		if (!opts.silent)
		{
			Log::error() << "Path " << path << " cannot be found\n";
		}
		return 0;
	}

	if (!(selfData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
	{
		return selfData.nFileSizeLow | (selfData.nFileSizeHigh << (sizeof(DWORD) * 8));
	}

	FindClose(selfFind);


	WIN32_FIND_DATAA data;
	auto find = FindFirstFileA(getWildcard(path).c_str(), &data);

	if (find == INVALID_HANDLE_VALUE)
	{
		if (!opts.silent)
			Log::error() << "Cannot find first file for directory " << path << "\n";
		return 0;
	}


	uint64_t size = 0;
	do 
	{
		auto fileName = std::string(data.cFileName);
		if (fileName == "." || fileName == "..")
			continue;
		auto relativeFileName = joinPaths(relativePath, fileName);

		uint64_t fileSize = 0;
		if (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			fileSize = handlePath(opts, joinPaths(path, fileName), relativeFileName);
		}
		else
		{
			fileSize = data.nFileSizeLow | (data.nFileSizeHigh << (sizeof(DWORD) * 8));
		}
		size += fileSize;

		if (!opts.silent)
		{
			auto fz = getFileSize(opts, fileSize);
			Log::fileSize(fz.value) << fz.suffix << " " << relativeFileName << "\n";
		}
	} 
	while (FindNextFileA(find, &data));

	FindClose(find);
	return size;
}

int main(int argc, const char* argv[]) 
{
	auto args = retrieveArgs(argc, argv);
	auto opts = parseArgs(args);

	if (opts.workPaths.empty())
	{
		if (!opts.silent)
		{
			Log::error() << "No work paths were specified.\n";
		}
		return 1;
	}
	else if (!opts.silent)
	{
		Log::info() << "Working with " << opts.workPaths.size() << " work paths\n";
	}

	std::vector<std::pair<std::string, FileSize>> summary;

	for (const auto& path : opts.workPaths)
	{
		auto fz = getFileSize(opts, handlePath(opts, path));
		summary.emplace_back(path, fz);

		if (!opts.silent)
			Log::fileSize(fz.value) << fz.suffix << " " << path << "\n";
	}

	if (!opts.silent)
	{
		Log::info() << "Summary: \n";
	}

	for (const auto& entry : summary)
	{
		Log::fileSize(entry.second.value) 
			<< entry.second.suffix 
			<< " " 
			<< entry.first << "\n";
	}

	return 0;
}