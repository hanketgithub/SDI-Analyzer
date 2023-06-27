

typedef enum {
    SAV_TYPE_NON_ACTIVE,
    SAV_TYPE_ACTIVE,
} SAV_TYPE_E;

typedef enum {
    EAV_TYPE_NON_ACTIVE,
    EAV_TYPE_ACTIVE,
} EAV_TYPE_E;


typedef struct {
    uint64_t offset;
    uint64_t distance;
    SAV_TYPE_E type;
} SAV_T;

typedef struct {
    uint64_t offset;
    uint64_t distance;
    EAV_TYPE_E type;
} EAV_T;


int parse(uint8_t *sdi_buf, size_t sdi_buf_size, std::vector< SAV_T > &savs, std::vector< EAV_T > &eavs);

