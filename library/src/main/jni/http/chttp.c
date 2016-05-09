/*
 * File        : chttp.c
 * Author      : Vincent Cheung
 * Date        : Jan. 21, 2015
 * Description : This is file contains some http method.
 *
 * Copyright (C) Vincent Chueng<coolingfall@gmail.com>
 *
 */

#include <curl/curl.h>

#include "chttp.h"
#include "common.h"

#define LOG_TAG     "CHttp"

/** write callback of curl, we can get response body here */
static size_t write_callback(void *contents, size_t size, size_t nmemb, void *userp)
{
	LOGD(LOG_TAG, "response contents: %s", (char *) contents);
	return size * nmemb;
}

/**
 * C http request with get method.
 */
void chttp_get(char *url, struct curl_slist *headers)
{
	CURL *curl;
	CURLcode res;
	curl = curl_easy_init();

	LOGD(LOG_TAG, "request url: %s", url);

	if(curl)
	{
		curl_easy_setopt(curl, CURLOPT_URL, url);
        if (headers) {
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        }
		/* follow redirection */
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);

		LOGD(LOG_TAG, "before request url");
		/* perform the request, res will get the return code */
		res = curl_easy_perform(curl);
		LOGD(LOG_TAG, "after request url");
		/* Check for errors */
		if(res != CURLE_OK)
		{
			LOGE(LOG_TAG, "curl perform failed: %s", curl_easy_strerror(res));
		}

		/* always cleanup */
		curl_easy_cleanup(curl);

		return;
	}

	LOGD(LOG_TAG, "curl init error");
}

/**
 * C http request with post method.
 */
void chttp_post(char *url, struct curl_slist *headers, char* post_fields)
{
    CURL *curl;
    CURLcode res;
    curl = curl_easy_init();

    LOGD(LOG_TAG, "request url: %s", url);

    if(curl)
    {
        curl_easy_setopt(curl, CURLOPT_URL, url);
        if (headers) {
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        }
        if (post_fields)
        {
            LOGD(LOG_TAG, "Now specify the POST data : %s", post_fields);
            /* Now specify the POST data */
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_fields);
        }
        /* follow redirection */
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);

        LOGD(LOG_TAG, "before post url");
        /* perform the request, res will get the return code */
        res = curl_easy_perform(curl);
        LOGD(LOG_TAG, "after post url");
        /* Check for errors */
        if(res != CURLE_OK)
        {
            LOGE(LOG_TAG, "curl perform failed: %s", curl_easy_strerror(res));
        }

        /* always cleanup */
        curl_easy_cleanup(curl);

        return;
    }

    LOGD(LOG_TAG, "curl init error");
}