#include <optional>
#include <iostream>
#include <fstream>
#include <cstring>

std::optional<std::string> file_read(std::string filename){
    if(std::ifstream is{filename, std::ios::binary | std::ios::ate}){
        auto size = is.tellg();
        std::string str(size, '\0');
        is.seekg(0);
        if(is.read(&str[0], size)){
            return str;
        }
    }
    return {};
}

void replace_all(std::string& data, std::string search, std::string replacement){
    size_t pos = data.find(search);
    while(pos != std::string::npos){
        data.replace(pos, search.size(), replacement);
        pos = data.find(search, pos + replacement.size());
    }
}

int main(int argc, char const *argv[]){
	if(argc < 3){
		std::cout << "usage: resource2c outputfile inputfile(s)" << std::endl;
		
	}
	std::ofstream out{std::string(argv[1])};
	out << "#include <string>\n"
		<< "#include <map>\n"
		<< "\n"
		<< "extern const std::map<std::string, std::string> resource2c = {\n";
	for(int i = 2; i < argc; i++){
		std::string filename = argv[i];
		std::cout << "converting " << filename << std::endl;
		const std::optional<std::string> source = file_read(filename);
	    if(!source){
	        std::cerr << "Error opening " << filename << ": " << strerror(errno) << std::endl;
	        return -1;
	    }
	    std::string content = source.value();
	    replace_all(content, "\n", "\\n\"\n        \"");

	    out << "    {\"" << filename << "\",\n"
	    << "        \"" << content << ((i == argc - 1) ? "\"}\n" : "\"},\n");
	}
	out << "};\n";
	return 0;
}