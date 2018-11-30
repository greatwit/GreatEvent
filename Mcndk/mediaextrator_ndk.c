
#include <jni.h>
#include <dlfcn.h>
#include <stdint.h>
#include <assert.h>
#include <sys/types.h>

#ifndef __cplusplus
#include <stdbool.h>
#endif

#include "basedef.h"
#include "mediacodec.h"
#include "mediaextrator.h"


//static struct symext symext;

struct members
{
    const char *name;
    int offset;
    bool critical;
};
static struct members members[] =
{
	#define OFF(x) offsetof(struct symext, AMediaExtractor.x)
		{ "AMediaExtractor_new", OFF(newext), true },
		{ "AMediaExtractor_delete", OFF(deleteext), true },
		{ "AMediaExtractor_setDataSource", OFF(setDataSource), true },
		{ "AMediaExtractor_getTrackCount", OFF(getTrackCount), true },
		{ "AMediaExtractor_getTrackFormat", OFF(getTrackFormat), true },
		{ "AMediaExtractor_selectTrack", OFF(selectTrack), true },
		{ "AMediaExtractor_unselectTrack", OFF(unselectTrack), true },
		{ "AMediaExtractor_advance", OFF(advance), true },
		{ "AMediaExtractor_readSampleData", OFF(readSampleData), true },
		{ "AMediaExtractor_getSampleTime", OFF(getSampleTime), true },
		{ "AMediaExtractor_seekTo", OFF(seekTo), true },
	#undef OFF

	#define OFF(x) offsetof(struct symext, AMediaFormat.x)
		{ "AMediaFormat_new", OFF(newfmt), true },
		{ "AMediaFormat_delete", OFF(deletefmt), true },
		{ "AMediaFormat_setString", OFF(setString), true },
		{ "AMediaFormat_setInt32", OFF(setInt32), true },
		{ "AMediaFormat_getInt32", OFF(getInt32), true },
		{ "AMediaFormat_getString", OFF(getString), true },
	#undef OFF
		{ NULL, 0, false }
};
#undef OFF


/* Initialize all symbols.
 * Done only one time during the first initialisation */
bool InitExtratorSymbols(struct symext *symbols)//mc_api *api
{
    //static vlc_mutex_t lock = VLC_STATIC_MUTEX;
    static int i_init_state = -1;
    bool ret;

    //vlc_mutex_lock(&lock);

    if (i_init_state != -1) {
		GLOGE("i_init_state != -1.");
        goto end;
	}

    i_init_state = 0;

    void *ndk_handle = dlopen("libmediandk.so", RTLD_NOW);
    if (!ndk_handle) {
		GLOGE("dlopen failed.");
        goto end;
	}
	int i = 0;
    for (; members[i].name; i++)
    {
        void *sym = dlsym(ndk_handle, members[i].name);
        if (!sym && members[i].critical)
        {
            dlclose(ndk_handle);
			GLOGE("dlextsym total:%d", i);
            goto end;
        }
        *(void **)((uint8_t*)symbols + members[i].offset) = sym;
    }
	GLOGE("jump for i:%d", i);
    i_init_state = 1;
end:
    ret = i_init_state == 1;
    if (!ret)
    	GLOGE("MediaExtractor NDK init failed");

    //vlc_mutex_unlock(&lock);
    return ret;
}

