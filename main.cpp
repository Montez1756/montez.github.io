#include "scraper.h"
#include <iostream>

int main(){

    Scraper s("https://www.youtube.com/");
    std::cout << s.getUrls().size() << std::endl;
    for(const auto url: s.getUrls()){
        std::cout << url << std::endl;
    }

    return 0;
}