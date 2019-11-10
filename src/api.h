#pragma once

#include "OrihimeRequest.h"

void sources_GET(OrihimeRequest& request);
void sources_HEAD(OrihimeRequest& request);
void sources_POST(OrihimeRequest& request);
void sources_id_GET(OrihimeRequest& request);
void sources_id_HEAD(OrihimeRequest& request);
void texts_GET(OrihimeRequest& request);
void texts_HEAD(OrihimeRequest& request);
void texts_id_GET(OrihimeRequest& request);
void texts_id_HEAD(OrihimeRequest& request);
void words_GET(OrihimeRequest& request);
void words_HEAD(OrihimeRequest& request);
void words_id_GET(OrihimeRequest& request);
void words_id_HEAD(OrihimeRequest& request);
void users_id_texts_GET(OrihimeRequest& request);
void users_id_texts_HEAD(OrihimeRequest& request);
void users_id_texts_POST(OrihimeRequest& request);
void users_id_texts_id_GET(OrihimeRequest& request);
void users_id_texts_id_GET_JSON(OrihimeRequest& request);
void users_id_texts_id_HEAD(OrihimeRequest& request);
void users_id_texts_id_PUT(OrihimeRequest& request);
void users_id_texts_id_POST(OrihimeRequest& request);
void users_id_words_GET(OrihimeRequest& request);
void users_id_words_HEAD(OrihimeRequest& request);
void users_id_words_POST(OrihimeRequest& request);
void users_id_words_id_GET(OrihimeRequest& request);
void users_id_words_id_HEAD(OrihimeRequest& request);
void users_id_words_id_PUT(OrihimeRequest& request);
