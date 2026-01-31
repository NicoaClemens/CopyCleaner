// main.cpp
// Entry point for CopyCleaner interpreter

#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>

#include "lexer.h"
#include "parser.h"
#include "runtime.h"

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <script.ccl>" << std::endl;
        std::cerr << "  Executes a CopyCleaner script file (.ccl)" << std::endl;
        return 1;
    }

    std::string filename = argv[1];

    // Read the script file
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file '" << filename << "'" << std::endl;
        return 1;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string source = buffer.str();
    file.close();

    lexer::Lexer lexer(source);
    parser::Parser parser(lexer);
    auto parse_result = parser.parse();

    if (is_err(parse_result)) {
        std::cerr << "Parse error: " << parse_result.error()->what() << std::endl;
        if (parse_result.error()->span().has_value()) {
            auto& span = parse_result.error()->span().value();
            std::cerr << "  at line " << span.p1.line << ", column " << span.p1.column 
                      << std::endl;
        }
        return 2;
    }

    // Execution
    auto statements = std::move(parse_result).value();
    Interpreter interpreter;
    auto global_env = std::make_shared<Environment>(nullptr);
    auto exec_result = interpreter.eval_statements(statements, *global_env);

    if (is_err(exec_result)) {
        auto& error = exec_result.error();
        std::cerr << "Runtime error: " << error->what() << std::endl;
        if (error->span().has_value()) {
            auto& span = error->span().value();
            std::cerr << "  at line " << span.p1.line << ", column " << span.p1.column << std::endl;
        }

        // Check if this is a graceful exit
        if (error->kind() == ErrorKind::Exit) {
            return 0;  // Program requested exit - not an error
        }
        return 3;
    }

    return 0;
}
