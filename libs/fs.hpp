#ifndef FS_HEADER
#define FS_HEADE

#include <bits/stdc++.h>
#include <dirent.h>
#include <sys/stat.h>

using namespace std;

namespace fs {
	vector<string> read_directory(string path)
	{
		vector<string> file_names;

		DIR *directory = opendir(path.c_str());
		struct dirent *directory_entry;

		while (true) {
			directory_entry = readdir(directory);

			if (directory_entry == NULL) break;

			string file_name = directory_entry->d_name;

			if (file_name != "." && file_name != "..") {
				file_names.push_back(file_name);
			}
		}

		closedir(directory);
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
		struct stat buf;
		stat(path.c_str(), &buf);

		FileStats stats = {
			.size = buf.st_size,
			.last_access_time = buf.st_atime,
			.last_modification_time = buf.st_mtime,
			.last_change_time = buf.st_ctime
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
				fseek(file, new_offset, SEEK_SET);
				offset = new_offset;
			}

			size_t size()
			{
				// Go to the end of the file and get the size

				fseek(file, 0, SEEK_END);
				size_t file_size = ftell(file);

				// Go back to the previous offset

				fseek(file, offset, SEEK_SET);
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
				return fclose(file);
			}
	};
};

#endif