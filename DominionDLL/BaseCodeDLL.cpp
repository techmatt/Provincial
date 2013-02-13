#include "Main.h"

BASECODEDLL_API void* __stdcall BCInit()
{
    App *app = new App;
    app->Init();
    return app;
}

BASECODEDLL_API UINT32 __stdcall BCProcessCommand(void *context, const char *s)
{
    if(context == NULL) return 1;
    App &app = *(App*)context;
    UINT32 result = app.ProcessCommand(String(s));
    return result;
}

BASECODEDLL_API const char* __stdcall BCQueryStringByName(void *context, const char *s)
{
    if(context == NULL) return NULL;
    App &app = *(App*)context;
    return app.QueryStringByName(s);
}

BASECODEDLL_API int __stdcall BCQueryIntegerByName(void *context, const char *s)
{
    if(context == NULL) return 0;
    App &app = *(App*)context;
    return app.QueryIntegerByName(s);
}

BASECODEDLL_API double __stdcall BCQueryDoubleByName(void *context, const char *s)
{
    if(context == NULL) return 0;
    App &app = *(App*)context;
    return app.QueryDoubleByName(s);
}