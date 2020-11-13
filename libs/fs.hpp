#ifndef FS_HEADER
#define FS_HEADER

#include <bits/stdc++.h>
#include "unistd.h"
#include <dirent.h>
#include <sys/stat.h>

using namespace std;

namespace fs {
	vector<string> read_directory(string path)
	{
		vector<string> file_names;

		DIR *directory = opendir(path.c_str());

		if (directory == NULL) {
			throw strerror(errno);
		}

		struct dirent *directory_entry;

		while (true) {
			directory_entry = readdir(directory);

			if (directory_entry == NULL) break;

			string file_name = directory_entry->d_name;

			if (file_name != "." && file_name != "..") {
				file_names.push_back(file_name);
			}
		}

		int res = closedir(directory);

		if (res == -1) {
			throw strerror(errno);
		}

		return file_names;
	}

	struct FileStats {
		off_t size;
		time_t last_access_time;
		time_t last_modification_time;
		time_t last_change_time;
	};

	FileStats get_stats(string path)
	{
		struct stat file_stats;
		int res = stat(path.c_str(), &file_stats);

		if (res != 0) {
			throw strerror(errno);
		}

		FileStats stats = {
			.size = file_stats.st_size,
			.last_access_time = file_stats.st_atime,
			.last_modification_time = file_stats.st_mtime,
			.last_change_time = file_stats.st_ctime
		};

		return stats;
	}

	bool exists(string path)
	{
		return access(path.c_str(), F_OK) == 0;
	}

	class File {
		public:
			FILE *file;
			size_t offset = 0;

			File(string file_path, const char *modes)
			{
				file = fopen(file_path.c_str(), modes);
			}

			bool exists()
			{
				return file != NULL;
			}

			void seek(size_t new_offset)
			{
				int res = fseek(file, new_offset, SEEK_SET);

				if (res != 0) {
					throw strerror(errno);
				}

				offset = new_offset;
			}

			size_t size()
			{
				// Go to the end of the file and get the size

				int res = fseek(file, 0, SEEK_END);

				if (res != 0) {
					throw strerror(errno);
				}

				size_t file_size = ftell(file);

				// Go back to the previous offset

				res = fseek(file, offset, SEEK_SET);

				if (res != 0) {
					throw strerror(errno);
				}

				return file_size;
			}

			size_t read(char *buffer, size_t number_of_bytes)
			{
				return fread(buffer, 1, number_of_bytes, file);
			}

			size_t write(char *buffer, size_t number_of_bytes)
			{
				return fwrite(buffer, 1, number_of_bytes, file);
			}

			int close()
			{
				if (file != NULL) return fclose(file);
				return -1;
			}
	};
};

#endif