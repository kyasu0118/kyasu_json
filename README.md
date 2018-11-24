# kyasu_json
"include" 1 file. C++ json parser.

# example
```
#include <iostream>
#include "kyasu_json.h"

int main(int argc, const char * argv[]) {

    kyasu::json json;
    
    json[0]["順位"] = 1;
    json[0]["名前"] = "kyasu";
    json[0]["年齢"] = 12;
    
    std::cout << json << std::endl;
    
    return 0;
}
```
# result
```
[{"名前":"kyasu","年齢":12,"順位":1}]
```
