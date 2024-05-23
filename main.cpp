#include <iostream>

/* std */

#include <cstdint>
#include <filesystem>
#include <fstream>

using i32 = std::int32_t;

#include <vector>

template<class T>
using Vector = std::vector <T>;

#include <string>

using String = std::string;

#include <regex>

using Regex = std::regex;

/* jp */

enum class Verbosity {
    Quiet,
    Error,
    Warning,
    Normal,
    Debug,
    Development
};

constexpr auto default_verbosity = Verbosity::Normal;

enum class InputType {
    FileName,
    DestinationDirectoryName,
    TemplateType
};

constexpr auto default_input_type = InputType::FileName;

constexpr auto default_file_name = String("main.cpp");
constexpr auto default_directory_name = String(".");

enum class TemplateType {
    CPPHelloWorld,
    CPPMain
};

constexpr auto default_template_type = TemplateType::CPPHelloWorld;

struct CLI {
    Verbosity verbosity;
    String file_name;
    String destination_directory_name;
    TemplateType template_type;
};

auto create_argument_list(i32 argc, char** argv) -> Vector <String>;
auto print_help_message() -> void;
auto parse_cli(const Vector <String>& argument_list) -> CLI;

auto verbosity_to_string(Verbosity verbosity) -> String;
auto template_type_to_string(TemplateType template_type) -> String;
auto template_type_to_short_string(TemplateType template_type) -> String;

int main(const i32 argc, char** argv) {
    auto argument_list = create_argument_list(argc, argv);

    for (const auto& argument : argument_list) {
        if (argument == "-h" || argument == "--help") {
            print_help_message(); /* noreturn */
        }
    }

    auto cli = parse_cli(argument_list);

    if (cli.verbosity >= Verbosity::Debug) {
        std::cout << "[EXTRA] CLI:" << std::endl;
        std::cout << "  verbosity: " << verbosity_to_string(cli.verbosity) << std::endl;
        std::cout << "  file_name: " << cli.file_name.c_str() << std::endl;
        std::cout << "  destination_directory_name: " << cli.destination_directory_name.c_str() << std::endl;
        std::cout << "  template_type: " << template_type_to_string(cli.template_type) << std::endl;
    }

    if (cli.file_name.empty() && cli.verbosity >= Verbosity::Error) {
        std::cout << "[ERROR] Empty file name!" << std::endl;
        std::exit(1);
    }

    try {
        auto cwd = std::filesystem::current_path();
        if (cli.verbosity >= Verbosity::Development) {
            std::cout << "[DEV] cwd: " << cwd.c_str() << std::endl;
        }

        auto file_path = cwd.append(cli.destination_directory_name).append(cli.file_name);
        if (cli.verbosity >= Verbosity::Development) {
            std::cout << "[DEV] file_path: " << file_path.c_str() << std::endl;
        }

        auto output_file = std::ofstream(file_path);
        if (output_file.is_open()) {
            switch (cli.template_type) {
                case TemplateType::CPPHelloWorld:
                    output_file << "#include <iostream>" << std::endl;
                    output_file << std::endl;
                    output_file << "int main() {" << std::endl;
                    output_file << "  std::cout << \"Hello, World!\" << std::endl;" << std::endl;
                    output_file << "  return 0;" << std::endl;
                    output_file << "}" << std::endl;
                    break;
                case TemplateType::CPPMain:
                    output_file << "int main(const int argc, char** argv) {" << std::endl;
                    output_file << "  return 0;" << std::endl;
                    output_file << "}" << std::endl;
                    break;
            }

            if (cli.verbosity >= Verbosity::Normal) {
                std::cout << "[INFO] Created file: " << file_path.c_str() << " with template: " << template_type_to_string(cli.template_type) << std::endl;
            }
        } else {
            if (cli.verbosity >= Verbosity::Error) {
                std::cout << "[ERROR] Failed to open or create file: " << file_path.c_str() << std::endl;
            }
            std::exit(1);
        }

        output_file.close();
    } catch (const std::exception& e) {
        if (cli.verbosity >= Verbosity::Error) {
            std::cout << "[ERROR] " << e.what() << std::endl;
        }
        std::exit(1);
    }

    return 0;
}

auto create_argument_list(i32 argc, char** argv) -> Vector <String> {
    auto argument_list = Vector <String>();

    for (auto index = 0u; index < argc; ++index) {
        argument_list.emplace_back(argv[index]);
    }

    return argument_list;
}

auto print_help_message() -> void {
    std::cout << "[INFO] Help Message:" << std::endl;

    std::cout << "  --- FLAGS ---" << std::endl;
    std::cout << "    -h, --help" << std::endl;
    std::cout << "      Print this help message." << std::endl;
    std::cout << "    -q, --quiet" << std::endl;
    std::cout << "      Suppress all output." << std::endl;
    std::cout << "    -e, --error" << std::endl;
    std::cout << "      Only print error messages." << std::endl;
    std::cout << "    -w, --warning" << std::endl;
    std::cout << "      Only print error and warning messages." << std::endl;
    std::cout << "    -n, --normal (default)" << std::endl;
    std::cout << "      Print error, warning and info messages." << std::endl;
    std::cout << "    -b, --debug" << std::endl;
    std::cout << "      Print debug messages as well." << std::endl;
    std::cout << "    -v, --development" << std::endl;
    std::cout << "      Print development messages as well." << std::endl;

    std::cout << "  --- OPTIONS ---" << std::endl;
    std::cout << "    -f, --file <file_name>" << std::endl;
    std::cout << "      Set file name." << std::endl;
    std::cout << "      Default: main.cpp" << std::endl;
    std::cout << "    -d, --directory <directory_name>" << std::endl;
    std::cout << "      Set directory name." << std::endl;
    std::cout << "      Default: ." << std::endl;
    std::cout << "    -t, --template <template_type>" << std::endl;
    std::cout << "      Set template type." << std::endl;
    std::cout << "      Default: CPPHelloWorld" << std::endl;
    std::cout << "        Available template types: " << std::endl;
    std::cout << "          CPPHelloWorld, cpphw" << std::endl;
    std::cout << "          CPPMain, cppm" << std::endl;

    std::exit(0);
}

auto parse_cli(const Vector <String>& argument_list) -> CLI {
    auto verbosity = default_verbosity;
    auto file_name = default_file_name;
    auto destination_directory_name = default_directory_name;
    auto template_type = default_template_type;

    auto verbosity_set_once = false;
    auto file_name_set_once = false;
    auto destination_directory_name_set_once = false;
    auto template_type_set_once = false;

    auto input_type = default_input_type;
    for (auto index = 1u; index < argument_list.size(); ++index) {
        const auto& argument = argument_list.at(index);

        if (argument == "-q" || argument == "--quiet") {
            if (!verbosity_set_once) {
                verbosity_set_once = true;
            } else {
                if (verbosity >= Verbosity::Error) {
                    std::cout << "[ERROR] More than two verbosity flags specified!" << std::endl;
                }
                std::exit(1);
            }
            verbosity = Verbosity::Quiet;
            continue;
        }
        if (argument == "-e" || argument == "--error") {
            if (!verbosity_set_once) {
                verbosity_set_once = true;
            } else {
                if (verbosity >= Verbosity::Error) {
                    std::cout << "[ERROR] More than two verbosity flags specified!" << std::endl;
                }
                std::exit(1);
            }
            verbosity = Verbosity::Error;
            continue;
        }
        if (argument == "-w" || argument == "--warning") {
            if (!verbosity_set_once) {
                verbosity_set_once = true;
            } else {
                if (verbosity >= Verbosity::Error) {
                    std::cout << "[ERROR] More than two verbosity flags specified!" << std::endl;
                }
                std::exit(1);
            }
            verbosity = Verbosity::Warning;
            continue;
        }
        if (argument == "-n" || argument == "--normal") {
            if (!verbosity_set_once) {
                verbosity_set_once = true;
            } else {
                if (verbosity >= Verbosity::Error) {
                    std::cout << "[ERROR] More than two verbosity flags specified!" << std::endl;
                }
                std::exit(1);
            }
            verbosity = Verbosity::Normal;
            continue;
        }
        if (argument == "-b" || argument == "--debug") {
            if (!verbosity_set_once) {
                verbosity_set_once = true;
            } else {
                if (verbosity >= Verbosity::Error) {
                    std::cout << "[ERROR] More than two verbosity flags specified!" << std::endl;
                }
                std::exit(1);
            }
            verbosity = Verbosity::Debug;
            continue;
        }
        if (argument == "-v" || argument == "--development") {
            if (!verbosity_set_once) {
                verbosity_set_once = true;
            } else {
                if (verbosity >= Verbosity::Error) {
                    std::cout << "[ERROR] More than two verbosity flags specified!" << std::endl;
                }
                std::exit(1);
            }
            verbosity = Verbosity::Development;
            continue;
        }

        /* input type flags */
        if (argument == "-f" || argument == "--file") {
            input_type = InputType::FileName;
            continue;
        }
        if (argument == "-d" || argument == "--directory") {
            input_type = InputType::DestinationDirectoryName;
            continue;
        }
        if (argument == "-t" || argument == "--template-type") {
            input_type = InputType::TemplateType;
            continue;
        }

        switch (input_type) {
            case InputType::FileName:
                if (file_name_set_once) {
                    if (verbosity >= Verbosity::Error) {
                        std::cout << "[ERROR] More than two file names specified!" << std::endl;
                        std::exit(1);
                    }
                    std::exit(1);
                }
                file_name_set_once = true;
                file_name = argument;
                break;
            case InputType::DestinationDirectoryName:
                if (destination_directory_name_set_once) {
                    if (verbosity >= Verbosity::Error) {
                        std::cout << "[ERROR] More than two destination directory names specified!" << std::endl;
                        std::exit(1);
                    }
                    std::exit(1);
                }
                destination_directory_name_set_once = true;
                destination_directory_name = argument;
                break;
            case InputType::TemplateType:
                if (template_type_set_once) {
                    if (verbosity >= Verbosity::Error) {
                        std::cout << "[ERROR] More than two template types specified!" << std::endl;
                        std::exit(1);
                    }
                    std::exit(1);
                }
                template_type_set_once = true;

                if (argument == template_type_to_short_string(TemplateType::CPPHelloWorld) || argument == template_type_to_string(TemplateType::CPPHelloWorld)) {
                    template_type = TemplateType::CPPHelloWorld;
                    break;
                }
                if (argument == template_type_to_short_string(TemplateType::CPPMain) || argument == template_type_to_string(TemplateType::CPPMain)) {
                    template_type = TemplateType::CPPMain;
                    break;
                }

                std::cout << "[ERROR] Unknown template type: " << argument.c_str() << std::endl;
                std::exit(1);
        }
    }

    return CLI {
        .verbosity = verbosity,
        .file_name = file_name,
        .destination_directory_name = destination_directory_name,
        .template_type = template_type
    };
}

auto verbosity_to_string(Verbosity verbosity) -> String {
    switch (verbosity) {
        case Verbosity::Quiet:
            return "Quite";
        case Verbosity::Error:
            return "Error";
        case Verbosity::Warning:
            return "Warning";
        case Verbosity::Normal:
            return "Normal";
        case Verbosity::Debug:
            return "Debug";
        case Verbosity::Development:
            return "Development";
    }

    return "Unknown";
}

auto template_type_to_string(TemplateType template_type) -> String {
    switch (template_type) {
        case TemplateType::CPPHelloWorld:
            return "CPPHelloWorld";
        case TemplateType::CPPMain:
            return "CPPMain";
    }

    return "Unknown";
}

auto template_type_to_short_string(TemplateType template_type) -> String {
    switch (template_type) {
        case TemplateType::CPPHelloWorld:
            return "cpphw";
        case TemplateType::CPPMain:
            return "cppm";
    }

    return "Unknown";
}
