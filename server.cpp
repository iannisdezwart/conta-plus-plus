#include <bits/stdc++.h>
#include "libs/cpp-httplib.hpp"
#include "libs/fs.hpp"

// Run simulations in multi threaded mode

// #define MULTI_THREADED

#include "Simulator/simulator.hpp"
#include "Simulator/simulation_settings.hpp"

// Change the below output directory path to your liking

#define OUTPUT_DIRECTORY string("Simulator/output")

using namespace std;
using namespace httplib;

struct FileEntry {
	string name;
	string mime_type;
};

unordered_map<string, FileEntry> files = {
	{ "/", { .name = "index.html", .mime_type = "text/html" } },
	{ "/style", { .name = "style.css", .mime_type = "text/css" } },
	{ "/plotly", { .name = "plotly.js", .mime_type = "text/javascript" } },
	{ "/file-buffer", { .name = "file-buffer.js", .mime_type = "text/javascript" } },
	{ "/list-runs", { .name = "list-runs.html", .mime_type = "text/html" } },
	{ "/list-runs-script", { .name = "list-runs.js", .mime_type = "text/javascript" } },
	{ "/perform-run", { .name = "perform-run.html", .mime_type = "text/html" } },
	{ "/perform-run-script", { .name = "perform-run.js", .mime_type = "text/javascript" } },
	{ "/view-run", { .name = "view-run.html", .mime_type = "text/html" } },
	{ "/view-run-script", { .name = "view-run.js", .mime_type = "text/javascript" } },
	{ "/view-run-graph", { .name = "view-run-graph.js", .mime_type = "text/javascript" } }
};

void stream_file(string file_path, string mime_type, Response& res)
{
	res.set_chunked_content_provider(
		mime_type.c_str(),
		[file_path](size_t offset, DataSink& sink) {
			fs::File file(file_path, "r");

			size_t remaining_bytes = file.size();
			char buffer[64 * 1024];

			while (remaining_bytes != 0) {
				// Put the next 64kB (or remaining bytes) into the buffer

				size_t bytes_to_stream = min((size_t) 64 * 1024, remaining_bytes);
				remaining_bytes -= bytes_to_stream;

				file.read(buffer, bytes_to_stream);
				sink.write(buffer, bytes_to_stream);
			}

			sink.done();
			file.close();
			return true;
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

string random_hex_string()
{
	char byte_chars[3];
	string output;
	RandomIntGenerator random_int(0x00, 0xFF);

	for (int i = 0; i < 4; i++) {
		uint8_t random_byte = random_int.generate();
		sprintf(byte_chars, "%02x", random_byte);
		output.push_back(byte_chars[0]);
		output.push_back(byte_chars[1]);
	}

	return output;
}

void run_simulator(const Request& req, Response& res)
{
	string run_id = random_hex_string();
	res.set_header("run-id", run_id);

	// Collect the settings from the headers

	string POPULATION_SIZE_H = req.get_header_value("population-size");
	string NUMBER_OF_COMMUNITIES_H = req.get_header_value("number-of-communities");
	string HUMAN_MAX_VELOCITY_H = req.get_header_value("human-max-velocity");
	string HUMAN_SPREAD_PROBABILITY_HEADER = req.get_header_value("human-spread-probability");
	string HUMAN_SPREAD_RANGE_H = req.get_header_value("human-spread-range");
	string HUMAN_INFECTION_DURATION_H = req.get_header_value("human-infection-duration");
	string HUMAN_TRAVEL_RATIO_H = req.get_header_value("human-travel-ratio");

	// Convert the header values to the correct data types

	int POPULATION_SIZE = atoi(POPULATION_SIZE_H.c_str());
  int NUMBER_OF_COMMUNITIES = atoi(NUMBER_OF_COMMUNITIES_H.c_str());
  double HUMAN_MAX_VELOCITY = atof(HUMAN_MAX_VELOCITY_H.c_str());
  double HUMAN_SPREAD_PROBABILITY = atof(HUMAN_SPREAD_PROBABILITY_HEADER.c_str());
  int HUMAN_SPREAD_RANGE = atoi(HUMAN_SPREAD_RANGE_H.c_str());
  int HUMAN_INFECTION_DURATION = atoi(HUMAN_INFECTION_DURATION_H.c_str());
	double HUMAN_TRAVEL_RATIO = atof(HUMAN_TRAVEL_RATIO_H.c_str());

	SimulationSettings *settings = new SimulationSettings {
		.POPULATION_SIZE = POPULATION_SIZE,
		.NUMBER_OF_COMMUNITIES = NUMBER_OF_COMMUNITIES,
		.HUMAN_MAX_VELOCITY = HUMAN_MAX_VELOCITY,
		.HUMAN_SPREAD_PROBABILITY = HUMAN_SPREAD_PROBABILITY,
		.HUMAN_SPREAD_RANGE = HUMAN_SPREAD_RANGE,
		.HUMAN_INFECTION_DURATION = HUMAN_INFECTION_DURATION,
		.HUMAN_TRAVEL_RATIO = HUMAN_TRAVEL_RATIO
	};

	printf("Running a simulation with these settings:\n");

	printf("POPULATION_SIZE: %d\n", POPULATION_SIZE);
	printf("NUMBER_OF_COMMUNITIES: %d\n", NUMBER_OF_COMMUNITIES);
	printf("HUMAN_MAX_VELOCITY: %f\n", HUMAN_MAX_VELOCITY);
	printf("HUMAN_SPREAD_PROBABILITY: %f\n", HUMAN_SPREAD_PROBABILITY);
	printf("HUMAN_SPREAD_RANGE: %d\n", HUMAN_SPREAD_RANGE);
	printf("HUMAN_INFECTION_DURATION: %d\n", HUMAN_INFECTION_DURATION);
	printf("HUMAN_TRAVEL_RATIO: %f\n", HUMAN_TRAVEL_RATIO);

	res.set_chunked_content_provider(
		"text/plain",
		[settings, run_id](size_t offset, DataSink& sink) {
			auto start = chrono::high_resolution_clock::now();

			// Start the simulation

			simulate(
				OUTPUT_DIRECTORY + "/" + run_id + ".conta",
				*settings,
				[&sink](int tick_number, Population& population) {
					// Send the tick number

					string output = "Rendered tick " + to_string(tick_number) + '\n';
					sink.write(output.c_str(), output.size());
				}
			);

			auto end = chrono::high_resolution_clock::now();
			int duration = chrono::duration_cast<chrono::milliseconds>(
				end - start).count();

			string output = "Elapsed time " + to_string(duration) + "ms\n";
			sink.write(output.c_str(), output.size());

			sink.done();
			return true;
		},
		[settings] { delete settings; }
	);
}

void list_runs(const Request& req, Response& res)
{
	// Get all files

	vector<pair<string, fs::FileStats>> files;
	vector<string> file_names = fs::read_directory(OUTPUT_DIRECTORY);

	for (int i = 0; i < file_names.size(); i++) {
		fs::FileStats stats = fs::get_stats(OUTPUT_DIRECTORY + "/" + file_names[i]);
		pair<string, fs::FileStats> entry(file_names[i], stats);
		files.push_back(entry);
	}

	// Sort the files by last modification time

	sort(files.begin(), files.end(), [](
		pair<string, fs::FileStats> top_file,
		pair<string, fs::FileStats> bottom_file
	) {
		time_t top_file_time = top_file.second.last_modification_time;
		time_t bottom_file_time = bottom_file.second.last_modification_time;

		return top_file_time > bottom_file_time;
	});

	// Send the file names to the client

	string output = "";

	for (int i = 0; i < files.size(); i++) {
		time_t modification_time = files[i].second.last_modification_time;
		output += files[i].first + '\t' + to_string(modification_time) + '\n';
	}

	res.set_content(output, "text/plain");
}

string sanitise_hex_string(string input)
{
	string output;

	for (int i = 0; i < input.size(); i++) {
		// Convert to lowercase

		if (input[i] >= 'A' && input[i] <= 'F') {
			input[i] += 32;
		}

		// If the char is [0-9] or [A-F], add it to the output

		if (input[i] >= '0' && input[i] <= '9' || input[i] >= 'a' && input[i] <= 'f') {
			output += input[i];
		}
	}

	return output;
}

void get_run_output(const Request& req, Response& res)
{
	string run_id = sanitise_hex_string(req.get_header_value("run-id"));
	string file_path = OUTPUT_DIRECTORY + "/" + run_id + ".conta";
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
		[file, file_size](size_t offset, DataSink& sink) {
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
		[file]() {
			file->close();
			delete file;
		}
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
