#include <sds.h>
#include <cJSON.h>
#include <tidy.h>
#include <tidybuffio.h>

#include "portal.h"

typedef char bool;
#define true 1
#define false 0

static float parse_usage_with_unit(const char *usage)
{
    if (usage == NULL)
        return 0;
    size_t len = strlen(usage);
    if (len == 0)
        return 0;
    char unit = usage[len - 1];
    char number[len];
    memcpy(number, usage, len - 1);
    number[len - 1] = '\0';

    switch (unit)
    {
    case 'B':
        return strtod(number, NULL);
    case 'K':
        return strtod(number, NULL) * 1024;
    case 'M':
        return strtod(number, NULL) * 1024 * 1024;
    case 'G':
        return strtod(number, NULL) * 1024 * 1024 * 1024;
    default:
        return 0;
    }
}

static const char *session_table_head[] = {
    "IP",
    "loginTime",
    "inTraffic",
    "outTraffic",
    "NAS IP",
    "VLAN ID",
    "MAC",
    "balance",
    "downRate",
    "upRate",
    "device",
    "availableUsage",
    "availableTime"};

int sessions_count = 0;

void parse_sessions_page(TidyDoc doc, TidyNode tnod, cJSON *results)
{
    TidyNode child;
    bool isRecursionNecessary = true;

    for (child = tidyGetChild(tnod); child; child = tidyGetNext(child))
    {
        ctmbstr name = tidyNodeGetName(child);
        cJSON *result = NULL;

        if (name)
        {
            TidyAttr attr;
            if (!strncmp(name, "td", 2))
            {
                for (attr = tidyAttrFirst(child); attr; attr = tidyAttrNext(attr))
                {
                    if (!strcmp(tidyAttrName(attr), "width") && !strcmp(tidyAttrValue(attr), "20"))
                    {
                        sessions_count++;
                        isRecursionNecessary = false;

                        if (sessions_count > 1)
                        {
                            result = cJSON_CreateObject();
                            cJSON_AddItemToArray(results, result);
                            TidyNode input = tidyGetChild(child);
                            TidyAttr input_attr = tidyAttrFirst(input);
                            input_attr = tidyAttrNext(input_attr);
                            input_attr = tidyAttrNext(input_attr);
                            cJSON *id = cJSON_CreateString(tidyAttrValue(input_attr));
                            cJSON_AddItemToObject(result, "id", id);
                        }

                        break;
                    }
                }
            }
        }

        if (sessions_count > 1 && !isRecursionNecessary)
        {
            TidyNode td;
            TidyBuffer buf;
            int i = 0;
            cJSON *item;

            for (td = tidyGetNext(child); td; td = tidyGetNext(td))
            {
                tidyBufInit(&buf);
                tidyNodeGetText(doc, tidyGetChild(td), &buf);
                char *bp = (char *)buf.bp;

                if (buf.bp)
                {
                    bp[strlen(bp) - 1] = '\0';
                    item = cJSON_CreateString((char *)buf.bp);
                }
                else
                {
                    item = cJSON_CreateString("");
                }

                cJSON_AddItemToObject(result, session_table_head[i], item);
                tidyBufFree(&buf);
                i++;
            }

            break;
        }

        parse_sessions_page(doc, child, results);
    }
}

void parse_user_detail_page(TidyDoc doc, TidyNode tnod, float *sum)
{
    TidyNode child;

    for (child = tidyGetChild(tnod); child; child = tidyGetNext(child))
    {
        ctmbstr name = tidyNodeGetName(child);
        if (name)
        {
            TidyAttr attr;
            if (!strcmp(name, "tr"))
            {
                attr = tidyAttrFirst(child);
                if (attr && !strcmp(tidyAttrName(attr), "align"))
                {
                    attr = tidyAttrNext(attr);
                    if (attr && !strcmp(tidyAttrName(attr), "onmouseover"))
                    {
                        TidyNode td = tidyGetChild(child);
                        int i;
                        for (i = 0; i < 5; i++)
                        {
                            td = tidyGetNext(td);
                        }

                        TidyBuffer buf;
                        tidyBufInit(&buf);
                        tidyNodeGetText(doc, tidyGetChild(td), &buf);
                        char *bp = (char *)buf.bp;
                        if (bp)
                        {
                            bp[strlen(bp) - 1] = '\0';
                        }
                        (*sum) = (*sum) + parse_usage_with_unit(bp);
                        tidyBufFree(&buf);

                        continue;
                    }
                }
            }
        }

        parse_user_detail_page(doc, child, sum);
    }
}
