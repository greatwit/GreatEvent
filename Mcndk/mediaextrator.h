#ifndef MEDIA_EXTRATOR_H
#define MEDIA_EXTRATOR_H


typedef enum {
    AMEDIAEXTRACTOR_SEEK_PREVIOUS_SYNC,
    AMEDIAEXTRACTOR_SEEK_NEXT_SYNC,
    AMEDIAEXTRACTOR_SEEK_CLOSEST_SYNC
} SeekMode;


struct AMediaExtractor;
typedef struct AMediaExtractor AMediaExtractor;
struct AMediaFormat;
typedef struct AMediaFormat AMediaFormat;

/////////////////////AMediaExtractor
typedef AMediaExtractor* (*pf_AMediaExtractor_new)();
typedef media_status_t   (*pf_AMediaExtractor_delete)(AMediaExtractor*);
typedef media_status_t   (*pf_AMediaExtractor_setDataSource)(AMediaExtractor*, const char *location);
typedef size_t   		 (*pf_AMediaExtractor_getTrackCount)(AMediaExtractor*);
typedef AMediaFormat*    (*pf_AMediaExtractor_getTrackFormat)(AMediaExtractor*, size_t idx);
typedef media_status_t   (*pf_AMediaExtractor_selectTrack)(AMediaExtractor*, size_t idx);
typedef media_status_t   (*pf_AMediaExtractor_unselectTrack)(AMediaExtractor*, size_t idx);
typedef bool   			 (*pf_AMediaExtractor_advance)(AMediaExtractor*);
typedef ssize_t   		 (*pf_AMediaExtractor_readSampleData)(AMediaExtractor*, uint8_t *buffer, size_t capacity);
typedef int64_t   		 (*pf_AMediaExtractor_getSampleTime)(AMediaExtractor*);
typedef media_status_t   (*pf_AMediaExtractor_seekTo)(AMediaExtractor*, int64_t seekPosUs, SeekMode mode);

////////////////AMediaFormat
typedef AMediaFormat *(*pf_AMediaFormat_new)();
typedef media_status_t (*pf_AMediaFormat_delete)(AMediaFormat*);

typedef void (*pf_AMediaFormat_setString)(AMediaFormat*,
        const char* name, const char* value);

typedef void (*pf_AMediaFormat_setInt32)(AMediaFormat*,
        const char* name, int32_t value);

typedef bool (*pf_AMediaFormat_getInt32)(AMediaFormat*,
        const char *name, int32_t *out);

typedef bool (*pf_AMediaFormat_getString)(AMediaFormat*,
		const char *name, const char **out);


struct symext
{
    struct {
    	pf_AMediaExtractor_new newext;
    	pf_AMediaExtractor_delete deleteext;//(const char *mime_type)
    	pf_AMediaExtractor_setDataSource setDataSource;//(const char *name)
    	pf_AMediaExtractor_getTrackCount getTrackCount;
    	pf_AMediaExtractor_getTrackFormat getTrackFormat;
    	pf_AMediaExtractor_selectTrack selectTrack;
    	pf_AMediaExtractor_unselectTrack unselectTrack;
    	pf_AMediaExtractor_advance advance;
    	pf_AMediaExtractor_readSampleData readSampleData;
    	pf_AMediaExtractor_getSampleTime getSampleTime;
    	pf_AMediaExtractor_seekTo seekTo;
    	pf_AMediaFormat_getString getString;
    } AMediaExtractor;
    struct {
        pf_AMediaFormat_new newfmt;
        pf_AMediaFormat_delete deletefmt;
        pf_AMediaFormat_setString setString;
        pf_AMediaFormat_setInt32 setInt32;
        pf_AMediaFormat_getInt32 getInt32;
        pf_AMediaFormat_getString getString;
    } AMediaFormat;
};


#ifdef __cplusplus
extern "C"
{
#endif

//extern struct symext;
bool InitExtratorSymbols(struct symext *symbols);

#ifdef __cplusplus
};
#endif

#endif
