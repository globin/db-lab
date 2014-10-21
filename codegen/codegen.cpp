#include <string>
#include <fstream>
#include <algorithm>
#include <iostream>
#include "Parser.hpp"

using namespace std;


void generate_header(ofstream &cpp_gen_tables) {
    cpp_gen_tables << "#pragma once" << endl << endl;
    cpp_gen_tables << "#include \"types.hpp\"" << endl;
    cpp_gen_tables << "#include \"table.hpp\"" << endl << endl;
}

int generate_header_file(Schema &schema) {

    ofstream cpp_gen_tables("src/tables_gen.hpp", ios::trunc);

    generate_header(cpp_gen_tables);
    cpp_gen_tables << schema.toCppHeader();

    return 0;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        cerr << "usage: " << argv[0] << " <schema file>" << endl;
        return -1;
    }

    Parser p(argv[1]);
    try {
        Schema schema = *p.parse();
        generate_header_file(schema);
    } catch (ParserError &e) {
        cerr << e.what() << endl;
    }
    return 0;
}
