#include "scraper.h"
#include <curl/curl.h>
#include <gumbo.h>
#include <vector>
#include <string>
#include <cstring>
#include <iostream>
#include <fstream>

size_t Scraper::WriteCallback(void *contents, size_t size, size_t nmemb, std::string *output)
{
    output->append((char *)contents, size * nmemb);
    return size * nmemb;
}

// Method to handle downloading a URL
std::string Scraper::getHTML(const std::string &url)
{
    CURL *curl;
    CURLcode res;
    std::string htmlResponse;

    // Initialize curl
    curl = curl_easy_init();
    if (curl)
    {
        // Set the URL
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

        // Set the callback function to store the response
        // struct curl_slist *headers = NULL;
        // std::vector<std::string> headerList;
        // loadRequestHeaders("default.headers", headerList);
        // for(const auto header: headerList){
        //     headers = curl_slist_append(headers, header.c_str());
        // }
        // std::string referer = "Referer: " + url;
        // headers = curl_slist_append(headers, referer.c_str());

        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &htmlResponse);
        // curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        // Perform the GET request
        res = curl_easy_perform(curl);

        // Check for errors
        if (res != CURLE_OK)
        {
            // QMessageBox::critical(nullptr, "Curl Initialization Error", "Failed to initialize cURL");
            return "";
        }

        // Cleanup
        curl_easy_cleanup(curl);
        std::cout << "dewkop" << htmlResponse << std::endl;
        return htmlResponse;
    }
    else
    {
        // QMessageBox::critical(nullptr, "Curl Initialization Error", "Failed to initialize cURL");
        return "";
    }
}
void Scraper::getMediaLinks(GumboNode *node, std::vector<std::string> &media_links)
{
    if (node->type != GUMBO_NODE_ELEMENT)
        return;

    GumboElement *element = &node->v.element;

    // Check for <img> tags and extract "src" attributes
    if (element->tag == GUMBO_TAG_IMG)
    {
        for (int i = 0; i < element->attributes.length; ++i)
        {
            GumboAttribute *attr = (GumboAttribute *)element->attributes.data[i];
            if (strcmp(attr->name, "src") == 0)
            {
                if (strncmp(attr->value, "data:image/", 11) != 0)
                {
                    media_links.push_back(attr->value);
                }
            }
        }
    }

    // Check for <audio> tags and extract "src" attributes
    if (element->tag == GUMBO_TAG_AUDIO)
    {
        for (int i = 0; i < element->attributes.length; ++i)
        {
            GumboAttribute *attr = (GumboAttribute *)element->attributes.data[i];
            if (strcmp(attr->name, "src") == 0)
            {
                media_links.push_back(attr->value);
            }
        }
    }

    // Check for <video> tags and extract "src" attributes
    if (element->tag == GUMBO_TAG_VIDEO)
    {
        for (int i = 0; i < element->attributes.length; ++i)
        {
            GumboAttribute *attr = (GumboAttribute *)element->attributes.data[i];
            if (strcmp(attr->name, "src") == 0)
            {
                if (strncmp(attr->value, "data:image/", 11) != 0)
                {
                    media_links.push_back(attr->value);
                }
            }
        }
    }

    // Recursively check child nodes
    for (int i = 0; i < element->children.length; ++i)
    {
        getMediaLinks((GumboNode *)element->children.data[i], media_links);
    }
}
Scraper::Scraper(const std::string &url)
{
    htmlContent = getHTML(url);
    if (!htmlContent.empty())
    {
        GumboOutput *output = gumbo_parse(htmlContent.c_str());
        getMediaLinks(output->root, urlList);
        gumbo_destroy_output(&kGumboDefaultOptions, output);
    }
}

const std::vector<std::string> &Scraper::getUrls() const
{
    return urlList;
}

void Scraper::loadRequestHeaders(const std::string fileName, std::vector<std::string> &headerVector){
    std::ifstream file(fileName);
    if(file.is_open()){
        std::string header;
        while(std::getline(file, header)){
            headerVector.push_back(header);
        }
    }
}