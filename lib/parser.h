#ifndef PARSER_H
#define PARSER_H

#include <cJSON.h>
#include <tidy.h>
#include <tidybuffio.h>

void parse_sessions_page(TidyDoc doc, TidyNode tnod, cJSON *results);
void parse_user_detail_page(TidyDoc doc, TidyNode tnod, double *sum);

#endif
