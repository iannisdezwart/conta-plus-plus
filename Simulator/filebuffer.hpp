#ifndef FILEBUFFER_HEADER
#define FILEBUFFER_HEADER

#include <bits/stdc++.h>

using namespace std;

class FileBuffer {
	private:
		vector<char> buffer;

	public:
		void write(string str)
		{
			for (int i = 0; i < str.length(); i++) {
				buffer.push_back(str[i]);
			}
		}

		void write(char num)
		{
			buffer.push_back(num);
		}

		void write(uint8_t num)
		{
			buffer.push_back((char) num);
		}

		void write(int8_t num)
		{
			buffer.push_back((char) num);
		}

		void write(uint16_t num)
		{
			char byte_1 = (num & 0xFF00) >> 8;
			char byte_2 = num & 0x00FF;
			buffer.push_back(byte_1);
			buffer.push_back(byte_2);
		}

		void write(int16_t num)
		{
			char byte_1 = (num & 0xFF00) >> 8;
			char byte_2 = num & 0x00FF;
			buffer.push_back(byte_1);
			buffer.push_back(byte_2);
		}

		void write(uint32_t num)
		{
			char byte_1 = (num & 0xFF000000) >> 24;
			char byte_2 = (num & 0x00FF0000) >> 16;
			char byte_3 = (num & 0x0000FF00) >> 8;
			char byte_4 = num & 0x000000FF;
			buffer.push_back(byte_1);
			buffer.push_back(byte_2);
			buffer.push_back(byte_3);
			buffer.push_back(byte_4);
		}

		void write(int32_t num)
		{
			char byte_1 = (num & 0xFF000000) >> 24;
			char byte_2 = (num & 0x00FF0000) >> 16;
			char byte_3 = (num & 0x0000FF00) >> 8;
			char byte_4 = num & 0x000000FF;
			buffer.push_back(byte_1);
			buffer.push_back(byte_2);
			buffer.push_back(byte_3);
			buffer.push_back(byte_4);
		}

		void write(uint64_t num)
		{
			char byte_1 = (num & 0xFF00000000000000) >> 56;
			char byte_2 = (num & 0x00FF000000000000) >> 48;
			char byte_3 = (num & 0x0000FF0000000000) >> 40;
			char byte_4 = (num & 0x000000FF00000000) >> 32;
			char byte_5 = (num & 0x00000000FF000000) >> 24;
			char byte_6 = (num & 0x0000000000FF0000) >> 16;
			char byte_7 = (num & 0x000000000000FF00) >> 8;
			char byte_8 = num & 0x00000000000000FF;
			buffer.push_back(byte_1);
			buffer.push_back(byte_2);
			buffer.push_back(byte_3);
			buffer.push_back(byte_4);
			buffer.push_back(byte_5);
			buffer.push_back(byte_6);
			buffer.push_back(byte_7);
			buffer.push_back(byte_8);
		}

		void write(int64_t num)
		{
			char byte_1 = (num & 0xFF00000000000000) >> 56;
			char byte_2 = (num & 0x00FF000000000000) >> 48;
			char byte_3 = (num & 0x0000FF0000000000) >> 40;
			char byte_4 = (num & 0x000000FF00000000) >> 32;
			char byte_5 = (num & 0x00000000FF000000) >> 24;
			char byte_6 = (num & 0x0000000000FF0000) >> 16;
			char byte_7 = (num & 0x000000000000FF00) >> 8;
			char byte_8 = num & 0x00000000000000FF;
			buffer.push_back(byte_1);
			buffer.push_back(byte_2);
			buffer.push_back(byte_3);
			buffer.push_back(byte_4);
			buffer.push_back(byte_5);
			buffer.push_back(byte_6);
			buffer.push_back(byte_7);
			buffer.push_back(byte_8);
		}

		void write(FileBuffer other_file_buffer)
		{
			char *bytes = other_file_buffer.data();

			for (int i = 0; i < other_file_buffer.size(); i++) {
				buffer.push_back(bytes[i]);
			}
		}

		char *data()
		{
			return buffer.data();
		}

		int size()
		{
			return buffer.size();
		}
};

#endif