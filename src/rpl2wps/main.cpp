#include <fmt/format.h>
#include <excmd.h>
#include <fstream>
#include <vector>
#include <string>

static bool convertRplToWps(const std::string &src, const std::string &dst)
{
    std::ifstream in(src, std::ios::binary | std::ios::ate);
    if (!in.is_open()) {
        fmt::print("Error: Could not open source file {} for reading\n", src);
        return false;
    }

    std::streamsize size = in.tellg();
    if (size < 11) {
        fmt::print("Error: Source file {} is too small to be a valid RPL/RPX\n", src);
        return false;
    }

    in.seekg(0, std::ios::beg);
    std::vector<char> buffer(static_cast<size_t>(size));
    if (!in.read(buffer.data(), size)) {
        fmt::print("Error: Could not read data from {}\n", src);
        return false;
    }
    in.close();

    buffer[9] = 'P';
    buffer[10] = 'L';

    std::ofstream out(dst, std::ios::binary);
    if (!out.is_open()) {
        fmt::print("Error: Could not open destination file {} for writing\n", dst);
        return false;
    }

    out.write(buffer.data(), static_cast<std::streamsize>(buffer.size()));
    out.close();

    return true;
}

int main(int argc, char **argv)
{
    excmd::parser parser;
    excmd::option_state options;
    using excmd::description;
    using excmd::value;

    try {
        parser.global_options()
           .add_option("H,help",
                       description { "Show help." });

        parser.default_command()
           .add_argument("src",
                         description { "Path to input rpl file" },
                         value<std::string> {})
           .add_argument("dst",
                         description { "Path to output wps file" },
                         value<std::string> {});

        options = parser.parse(argc, argv);
    } catch (const excmd::exception &ex) {
        fmt::print("Error parsing options: {}\n", ex.what());
        return -1;
    }

    if (options.empty()
        || options.has("help")
        || !options.has("src")
        || !options.has("dst")) {
        fmt::print("{} <options> src dst\n", argv[0]);
        fmt::print("{}\n", parser.format_help(argv[0]));
        return 0;
    }

    auto src = options.get<std::string>("src");
    auto dst = options.get<std::string>("dst");

    if (!convertRplToWps(src, dst)) {
        return -1;
    }

    return 0;
}