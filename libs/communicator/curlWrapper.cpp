/*

  Safety Framework for Component-based Robotics

  Created on: August 31, 2012

  Copyright (C) 2012 Min Yang Jung, Peter Kazanzides

  Distributed under the Boost Software License, Version 1.0.
  (See accompanying file LICENSE_1_0.txt or copy at
  http://www.boost.org/LICENSE_1_0.txt)

*/

#include "curlWrapper.h"

#include <curl/curl.h>
#include <curl/types.h>
#include <curl/easy.h>

#include <iostream>
#include <string.h>

namespace SF {

// callback function to receive the data (called by Curl)
size_t static Callback(void *buffer, size_t size, size_t nmemb, void *userp)
{
    char **response_ptr =  (char**)userp;

    // assume that response is of type string
    *response_ptr = strndup((const char*)buffer, (size_t)(size *nmemb));

    return 0;
}

Curl::Curl(const std::string & baseURL)
    : BaseURL(baseURL)
{
}

Curl::~Curl()
{
}

const std::string & Curl::GetBaseURL(void) {
    return BaseURL;
}

void Curl::SetBaseURL(const std::string & baseURL) {
    BaseURL = baseURL;
}

char * Curl::CallAPI(const std::string & arg)
{
    // keeps the handle to the curl object
    CURL *curl_handle = 0;
    // to keep the response
    char *response = 0;

    // initialize curl and set the url
    curl_handle = curl_easy_init();
    std::string url(BaseURL);
    url += "/";
    url += arg;
    curl_easy_setopt(curl_handle, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl_handle, CURLOPT_HTTPGET, 1);

    // follow locations specified by the response header
    curl_easy_setopt(curl_handle, CURLOPT_FOLLOWLOCATION, 1);

    // set callback function to receive the data returned
    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, Callback);

    // pass the pointer to the response as the callback parameter
    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, &response);

    // perform the request
    curl_easy_perform(curl_handle);

    // clean up
    curl_easy_cleanup(curl_handle);

    return response;
}

}
