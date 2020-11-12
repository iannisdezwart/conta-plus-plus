#include <bits/stdc++.h>
#include "libs/cpp-httplib.hpp"
#include "libs/fs.hpp"

#include "Simulator/simulator.hpp"
#include "Simulator/simulation_settings.hpp"

using namespace std;
using namespace httplib;

struct FileEntry {
	string name;
	string mime_type;
};

unordered_map<string, FileEntry> files = {
	{ "/", { .name = "index.html", .mime_type = "text/html" } },
	{ "/style", { .name = "style.css", .mime_type = "text/css" } },
	{ "/file-buffer", { .name = "file-buffer.js", .mime_type = "text/javascript" } },
	{ "/list-runs", { .name = "list-runs.html", .mime_type = "text/html" } },
	{ "/list-runs-script", { .name = "list-runs.js", .mime_type = "text/javascript" } },
	{ "/perform-run", { .name = "perform-run.html", .mime_type = "text/html" } },
	{ "/perform-run-script", { .name = "perform-run.js", .mime_type = "text/javascript" } },
	{ "/view-run", { .name = "view-run.html", .mime_type = "text/html" } },
	{ "/view-run-script", { .name = "view-run.js", .mime_type = "text/javascript" } }
};

void stream_file(string file_path, string mime_type, Response& res)
{
	fs::File file(file_path, "r");
	char buffer[64 * 1024];

	res.set_content_provider(
		file.size(),
		mime_type.c_str(),
		[&file, &buffer](size_t offset, size_t length, DataSink& sink) {
			// Put the next 64kB (or remaining bytes) into the buffer

			int bytes_to_stream = min((uint64_t) 64 * 1024, length);
			file.read(buffer, bytes_to_stream);

			// Write the buffer to the sink

			sink.write(buffer, bytes_to_stream);

			return true;
		},
		[&file]() {
			file.close();
		}
	);
}

void send_file(const Request& req, Response& res)
{
	if (!files.count(req.path)) {
		// The file does not exist, send 404

		res.status = 404;
		res.set_content("404: Not Found", "text/plain");
		return;
	}

	// Send the file

	FileEntry file_entry = files[req.path];
	string file_path = "WebApp/" + file_entry.name;

	if (!fs::exists(file_path)) {
		// File does not exist, but is defined in the files map

		res.status = 500;
		res.set_content("500: Internal Server Error", "text/plain");
		return;
	}

	// Stream the file to the response

	stream_file(file_path, file_entry.mime_type, res);
}

void run_simulator(const Request& req, Response& res)
{
	// Todo: make

	// SimulationSettings settings = {
		
	// };

	// thread simulation_thread = thread(simulate, settings);
	// simulation_thread.join();
}

void list_runs(const Request& req, Response& res)
{
	// Get all files

	vector<pair<string, fs::FileStats>> files;
	vector<string> file_names = fs::read_directory("Simulator/output");

	for (int i = 0; i < file_names.size(); i++) {
		fs::FileStats stats = fs::get_stats(file_names[i]);
		pair<string, fs::FileStats> entry(file_names[i], stats);
		files.push_back(entry);
	}

	// Sort the files by creation time

	sort(files.begin(), files.end(), [](
		pair<string, fs::FileStats> f1,
		pair<string, fs::FileStats> f2
	) {
		return f2.second.last_change_time - f1.second.last_change_time;
	});

	// Send the file names to the client

	string output = "";

	for (int i = 0; i < files.size(); i++) {
		time_t timestamp = time(&files[i].second.last_change_time);
		output += files[i].first + '\t' + to_string(timestamp) + '\n';
	}

	res.set_content(output, "text/plain");
}

string sanitise_hex_string(string input)
{
	string output;

	for (int i = 0; i < input.size(); i++) {
		// Convert to uppercase

		if (input[i] >= 'a' && input[i] <= 'z') {
			input[i] -= 32;
		}

		// If the char is [0-9] or [A-F], add it to the output

		if (input[i] >= '0' && input[i] <= '9' || input[i] >= 'A' && input[i] <= 'F') {
			output += input[i];
		}
	}

	return output;
}

void get_run_output(const Request& req, Response& res)
{
	string run_id = sanitise_hex_string(req.get_header_value("run-id"));
	string file_path = "Simulator/output/" + run_id + ".conta";
	fs::File *file = new fs::File(file_path, "r");

	if (!file->exists()) {
		// The file does not exist, send 404

		res.status = 404;
		res.set_content("404: Not Found", "text/plain");
		return;
	}

	// Stream the run output to the client

	size_t file_size = file->size();

	res.set_chunked_content_provider(
		"application/conta",
		[=](size_t offset, DataSink& sink) {
			size_t bytes_written = 0;
			char buffer[64 * 1024];

			while (bytes_written < file_size) {
				size_t bytes_to_write = min((size_t) 64 * 1024, file_size - bytes_written);
				bytes_written += bytes_to_write;

				file->read(buffer, bytes_to_write);
				sink.write(buffer, bytes_to_write);
			}

			sink.done();
			return true;
		},
		[=]() { delete file; }
	);
}

int main()
{
	Server server;

	server.set_logger([&](const Request& req, const Response& res) {
		cout << "[ Incoming Request ]: " << req.path << '\n';
	});

	server.Get("/.*", send_file);

	server.Post("/run", run_simulator);
	server.Post("/list-runs", list_runs);
	server.Post("/get-run-output", get_run_output);

	server.listen("0.0.0.0", 1337);
}
