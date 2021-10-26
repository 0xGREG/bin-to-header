#include <iostream>
#include <fstream>
#include <filesystem>
#include <sstream>

#pragma warning(disable:6385)

int main(int argc, char** argv)
{
    // check for valid argument count

    if (argc != 3) {
        std::cout << "error: invalid argument count" << std::endl;
        return -1;
    }

    // check if source file exists

    if (!std::filesystem::exists(argv[1])) {
        std::cout << "error: source file does not exist" << std::endl;
        return -2;
    }

    // read file into process

    char* source;
    std::streamsize source_size;

    try
    {
        std::ifstream read_stream(argv[1], std::ios::binary);

        if (!read_stream.is_open()) {
            std::cout << "error: failed to open source file" << std::endl;
            return -6;
        }

        read_stream.ignore(std::numeric_limits<std::streamsize>::max());
        source_size = read_stream.gcount();
        read_stream.clear();

        source = new char[static_cast<size_t>(source_size)];

        read_stream.seekg(0, std::ios::beg);
        read_stream.read(source, source_size);

        read_stream.close();
    }
    catch (...)
    {
        std::cout << "error: failed to read source file" << std::endl;
        return -3;
    }

    // convert file to hex

    std::stringstream output{};
    
    output << "#pragma once\n\nnamespace binary {\n\tunsigned char buffer[] = {";

    constexpr size_t line_max = 24;

    for (auto i = 0; i < source_size; i++)
    {
        if (i % line_max == 0)
            output << "\n\t\t";

        output << "0x" << std::hex << std::setw(2) << std::setfill('0') << static_cast<uint32_t>(static_cast<uint8_t>(source[i])) << std::dec << ", ";
    }

    output << "\n\t};\n}";

    // delete source buffer
    delete[] source;

    // write file

    try
    {
        std::ofstream write_stream(argv[2], std::ios::out | std::ios::binary | std::ios::trunc);

        if (!write_stream.is_open()) {
            std::cout << "error: failed to open result file" << std::endl;
            return -4;
        }

        write_stream.write(output.str().c_str(), output.str().length());

        write_stream.close();
    }
    catch (...)
    {
        std::cout << "error: failed to write result file" << std::endl;
        return -5;
    }

    // free output memory
    output.clear();

    // success

    std::cout << "success: file dumped to header file" << std::endl;

    return 0;
}