#include <stdio.h>
#include <string>

#include <dser/json_parser.h>
#include <dser/file.h>

int main() {
//  std::string json_string =
//  	"
//  	  \"key 1\": \"value 1\","
//  	  \"key 2\": \"value 2\","
//  	  \"key 3\": 3.141592,"
//  	  \"key 4\": { \"key 5\": [ 2.71, 1.4141 ] },"
//  	  \"key 6\": Null"
//  	";

    dser::fs::file file;
    file.open("../assets/json/1.json");
 
    dser::json_parser json_parser;
    int err = json_parser.parse(file.data(), file.size());
    if (err != dser::json_parser_error::ERR_NO_ERROR) {
        printf("\n[E] Error parsing json:\n");
        printf("%s\n\n", dser::stringify_json_parser_error(err));
    
        printf("Error context:\n");
        printf("line: %d, column: %d\n",
                json_parser.context().line,
                json_parser.context().column);
        printf("depth: %d\n", json_parser.context().depth);
    } else {
        printf("\n[S] No errors parsing json\n");
    }

    return 0;
}
