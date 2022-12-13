#ifndef METADEFENDER_H
#define METADEFENDER_H

#include <phdk.h>
#include <phappresource.h>
#include <settings.h>
#include <http.h>
#include <json.h>
#include <commonutil.h>
#include <workqueue.h>
#include <mapimg.h>


////////////Goes in onlnchk.h file//////////////////

typedef struct _METADEFENDER_FILE_REPORT
{
    INT64 ResponseCode;
    PPH_STRING StatusMessage;
    PPH_STRING PermaLink;
    PPH_STRING ScanId;

    PPH_STRING ScanDate;
    PPH_STRING Positives;
    PPH_STRING Total;
    PPH_LIST ScanResults;
} METADEFENDER_FILE_REPORT, * PMETADEFENDER_FILE_REPORT;
//////////////////////////////////////////////////////
#endif 
