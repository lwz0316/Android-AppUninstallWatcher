/*
 * File        : chttp.h
 * Author      : Vincent Cheung
 * Date        : Jan. 21, 2015
 * Description : This is header file of chttp.c.
 *
 * Copyright (C) Vincent Chueng<coolingfall@gmail.com>
 *
 */

#ifndef __CHTTP_H__
#define __CHTTP_H__

void chttp_get(char *url, struct curl_slist *headers);
void chttp_post(char *url, struct curl_slist *headers, char* post_fields);

#endif