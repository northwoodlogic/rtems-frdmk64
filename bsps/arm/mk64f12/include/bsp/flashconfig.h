#ifndef MYFLASH_CONFIG
#define MYFLASH_CONFIG

struct FlashConfig {
    unsigned int word1;
    unsigned int word2;
    unsigned int word3;
    unsigned int word4;
};

extern const struct FlashConfig FlashConfig;

#endif

