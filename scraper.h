#ifndef SCRAPER_H
#define SCRAPER_H

#include <vector>
#include <string>
// #include <QMessageBox>
#include <curl/curl.h>
#include <iostream>
#include <gumbo.h>
class Scraper
{
private:
    std::vector<std::string> urlList;
    std::string currentUrl;
    std::string htmlContent;

    // Callback function to handle the response data
    static size_t WriteCallback(void *contents, size_t size, size_t nmemb, std::string *output);
    // Returns HTML content of a URL
    std::string getHTML(const std::string &url);
    // Parses HTML page for Media links (mp4,mv4,mov,png,jpg,svg) etc.
    void getMediaLinks(GumboNode* node, std::vector<std::string>& media_links);
    // Loads headers from a file
    void loadRequestHeaders(const std::string fileName, std::vector<std::string> &headerVector);
public:
    Scraper(const std::string &url);
    
    const std::vector<std::string> &getUrls() const;
};

#endif
