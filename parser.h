

typedef enum {
    SAV_TYPE_NON_ACTIVE,
    SAV_TYPE_ACTIVE,
} SAV_TYPE_E;

typedef enum {
    EAV_TYPE_NON_ACTIVE,
    EAV_TYPE_ACTIVE,
} EAV_TYPE_E;

int parse(uint8_t *sdi_buf, size_t sdi_buf_size);

