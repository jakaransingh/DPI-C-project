#include <stdio.h>
#include <string.h>
#include <stdbool.h>

bool isVerbose = true;
bool isHuge = false;

typedef struct {
    const char* mode;
    bool verbose;
    int H[8];
    int t1;
    int t2;
    int a;
    int b;
    int c;
    int d;
    int e;
    int f;
    int g;
    int h;
    int w;
    int W[16];
    int k;
    int K[64];
} Hash;

void sha256(Hash* hash, const char* mode, bool verbose) {
    if (strcmp(strcmp(mode, "sha256") != 0) {
        printf("Error: Given %s is not a supported mode.\n", mode);
        return;
    }

    hash->mode = mode;
    hash->verbose = verbose;
    memset(hash->H, 0, sizeof(hash->H));
    hash->t1 = 0;hash->t2 = 0;hash->a = 0;
    hash->b = 0; hash->c = 0; hash->d = 0;
    hash->e = 0;hash->f = 0;hash->g = 0;
    hash->h = 0; hash->w = 0;
    memset(hash->W, 0, sizeof(hash->W));
    hash->k = 0;
    memcpy(hash->K, (unsigned[]){
        0x428a2f98, 0x71374491, 
        0xb5c0fbcf, 0xe9b5dba5,
        0x3956c25b, 0x59f111f1, 
        0x923f82a4, 0xab1c5ed5,
        0xd807aa98, 0x12835b01, 
        0x243185be, 0x550c7dc3,
        0x72be5d74, 0x80deb1fe, 
        0x9bdc06a7, 0xc19bf174,
        0xe49b69c1, 0xefbe4786, 
        0x0fc19dc6, 0x240ca1cc,
        0x2de92c6f, 0x4a7484aa, 
        0x5cb0a9dc, 0x76f988da,
        0x983e5152, 0xa831c66d, 
        0xb00327c8, 0xbf597fc7,
        0xc6e00bf3, 0xd5a79147, 
        0x06ca6351, 0x14292967,
        0x27b70a85, 0x2e1b2138, 
        0x4d2c6dfc, 0x53380d13,
        0x650a7354, 0x766a0abb, 
        0x81c2c92e, 0x92722c85,
        0xa2bfe8a1, 0xa81a664b, 
        0xc24b8b70, 0xc76c51a3,
        0xd192e819, 0xd6990624, 
        0xf40e3585, 0x106aa070,
        0x19a4c116, 0x1e376c08,  
        0x2748774c, 0x34b0bcb5,
        0x391c0cb3, 0x4ed8aa4a, 
         0x5b9cca4f, 0x682e6ff3,
        0x748f82ee, 0x78a5636f, 
        0x84c87814, 0x8cc70208,
        0x90befffa, 0xa4506ceb, 
        0xbef9a3f7, 0xc67178f2
    }, sizeof(hash->K));
}

void initHash(Hash* hash) {
    if (strcmp(hash->mode, "sha256") == 0) {
        printf("------>We are in SHA 256<------- \n");
        memcpy(hash->H, (unsigned[]){
            0x6a09e667, 
            0xbb67ae85, 
            0x3c6ef372, 
            0xa54ff53a,
            0x510e527f, 
            0x9b05688c, 
            0x1f83d9ab, 
            0x5be0cd19
        }, sizeof(hash->H));
    } else {
        memcpy(hash->H, (unsigned[]){
            0xc1059ed8, 
            0x367cd507, 
            0x3070dd17, 
            0xf70e5939,
            0xffc00b31, 
            0x68581511, 
            0x64f98fa7, 
            0xbefa4fa4
        }, sizeof(hash->H));
    }
}

void W_schedule(Hash* hash, int* block) {
    for (int i = 0; i < 16; i++) {
        hash->W[i] = block[i];
    }
}

unsigned rotr32(unsigned value, unsigned shift) {
    return ((value >> shift) | (value << (32 - shift))) & 0xffffffff;
}

int shr32(unsigned int value, unsigned int shift) {
    return value >> shift;
}

void copyDigest(Hash* hash) {
    hash->a = hash->H[0];hash->b = hash->H[1];hash->c = hash->H[2];
    hash->d = hash->H[3];hash->e = hash->H[4];hash->f = hash->H[5];
    hash->g = hash->H[6];hash->h = hash->H[7];
}

void printState(Hash* hash, int round) {
    printf("State at round 0x%02x:\n", round);
    printf("t1 = 0x%08x, t2 = 0x%08x\n", hash->t1, hash->t2);
    printf("k  = 0x%08x, w  = 0x%08x\n", hash->k, hash->w);
    printf("a  = 0x%08x, b  = 0x%08x\n", hash->a, hash->b);
    printf("c  = 0x%08x, d  = 0x%08x\n", hash->c, hash->d);
    printf("e  = 0x%08x, f  = 0x%08x\n", hash->e, hash->f);
    printf("g  = 0x%08x, h  = 0x%08x\n\n", hash->g, hash->h);
}

int delta0(int x) {
    return (rotr32(x, 7) ^ rotr32(x, 18) ^ shr32(x, 3));
}

int delta1(int x) {
    return (rotr32(x, 17) ^ rotr32(x, 19) ^ shr32(x, 10));
}

int nextW(Hash* hash, int round) {
    if (round < 16) {
        return hash->W[round];
    } else {
        int tmpW = (delta1(hash->W[14]) +
                     hash->W[9] +
                     delta0(hash->W[1]) +
                     hash->W[0]) & 0xffffffff;
        for (int i = 0; i < 15; i++) {
            hash->W[i] = hash->W[i + 1];
        }
        hash->W[15] = tmpW;
        return tmpW;
    }
}

int Ch(int x, int y, int z) {
    return (x & y) ^ (~x & z);
}

int Maj(int x, int y, int z) {
    return (x & y) ^ (x & z) ^ (y & z);
}

int sigma0(int x) {
    return (rotr32(x, 2) ^ rotr32(x, 13) ^ rotr32(x, 22));
}

int sigma1(int x) {
    return (rotr32(x, 6) ^ rotr32(x, 11) ^ rotr32(x, 25));
}

int _T1(int e, int f, int g, int h, int k, int w) {
    return (h + sigma1(e) + Ch(e, f, g) + k + w) & 0xffffffff;
}

int _T2(int a, int b, int c){
    return (sigma0(a) + Maj(a, b, c)) & 0xffffffff;
}

void sha256Round(Hash* hash, int round) {
    hash->k = hash->K[round];hash->w = nextW(hash, round);
    hash->t1 = _T1(hash->e, hash->f, hash->g, hash->h, hash->k, hash->w);
    hash->t2 = _T2(hash->a, hash->b, hash->c);
    hash->h = hash->g;hash->g = hash->f;hash->f = hash->e;
    hash->e = (hash->d + hash->t1) & 0xffffffff;
    hash->d = hash->c;hash->c = hash->b;hash->b = hash->a;
    hash->a = (hash->t1 + hash->t2) & 0xffffffff;
}

void updateDigest(Hash* hash) {
    hash->H[0] = (hash->H[0] + hash->a) & 0xffffffff;
    hash->H[1] = (hash->H[1] + hash->b) & 0xffffffff;
    hash->H[2] = (hash->H[2] + hash->c) & 0xffffffff;
    hash->H[3] = (hash->H[3] + hash->d) & 0xffffffff;
    hash->H[4] = (hash->H[4] + hash->e) & 0xffffffff;
    hash->H[5] = (hash->H[5] + hash->f) & 0xffffffff;
    hash->H[6] = (hash->H[6] + hash->g) & 0xffffffff;
    hash->H[7] = (hash->H[7] + hash->h) & 0xffffffff;
}

void next(Hash* hash, int* block, int verbose) {
    W_schedule(hash, block);
    copyDigest(hash);
    if (verbose) {
        printf("State after init:\n");
        printState(hash, 0);
    }

    for (int i = 0; i < 64; i++) {
        sha256Round(hash, i);
        if (verbose) {
            printState(hash, i);
        }
    }

    updateDigest(hash);
}
typedef struct {
    const char* mode;
    bool verbose;
    int H[8];int t1;int t2;int a;int b;
    int c;int d;int e;int f;int g;int h;
    int w;int W[16];int k;int K[64];
} Hash;

int* getDigest(Hash* hash) {
    return hash->H;
}

void printDigest(int* digest, int length) {
    printf("0x%08x, 0x%08x, 0x%08x, 0x%08x\n", digest[0], digest[1], digest[2], digest[3]);
    if (length == 8) {
        printf("0x%08x, 0x%08x, 0x%08x, 0x%08x\n", digest[4], digest[5], digest[6], digest[7]);
    } else {
        printf("0x%08x, 0x%08x, 0x%08x\n", digest[4], digest[5], digest[6]);
    }
    printf("\n");
}

void compareDigests(int* digest, int* expected, int length) {
    int correct = 1;
    for (int i = 0; i < length; i++) {
        if (digest[i] != expected[i]) {
            correct = 0;
            break;
        }
    }
    if (!correct) {
        printf("Error:\n");
        printf("Got:\n");
        printDigest(digest, length);
        printf("Expected:\n");
        printDigest(expected, length);
    } else {
        printf("Test case ok.\n");
    }
}

void sha256Tests(int dut_in[], Hash* hash, const char* mode, int dut_res[], bool verbose) {
    printf("Running test cases for SHA256:\n");
    sha256(hash, mode, verbose);
    int TC1_block[16] = {
        0x61626380, 0x00000000, 0x00000000, 0x00000000,
        0x00000000, 0x00000000, 0x00000000, 0x00000000,
        0x00000000, 0x00000000, 0x00000000, 0x00000000,
        0x00000000, 0x00000000, 0x00000000, 0x00000018
    };
    init(hash);
    next(hash, dut_in, verbose);
    int* my_digest = getDigest(hash);
    compareDigests(my_digest, dut_res, 8);
}

void sha256Test2(Hash* hash, const char* mode, int dut_res[], bool verbose) {
    printf("Running test2 cases for SHA256:\n");
    sha256(hash, mode, verbose);

    int TC2_1_block[16] = {
        0x61626364, 0x62636465, 0x63646566, 0x64656667,
        0x65666768, 0x66676869, 0x6768696A, 0x68696A6B,
        0x696A6B6C, 0x6A6B6C6D, 0x6B6C6D6E, 0x6C6D6E6F,
        0x6D6E6F70, 0x6E6F7071, 0x80000000, 0x00000000
    };

    int TC2_1_expected[8] = {
        0x85E655D6, 0x417A1795, 0x3363376A, 0x624CDE5C,
        0x76E09589, 0xCAC5F811, 0xCC4B32C1, 0xF20E533A
    };

    init(hash);
    next(hash, TC2_1_block, verbose);
    int* my_digest = getDigest(hash);
    compareDigests(my_digest, dut_res, 8);
}

void sha256Test3(Hash* hash, const char* mode, int dut_res[], bool verbose) {
    printf("Running test3 cases for SHA256:\n");
    sha256(hash, mode, verbose);

    int TC2_2_block[16] = {
        0x00000000, 0x00000000, 0x00000000, 0x00000000,
        0x00000000, 0x00000000, 0x00000000, 0x00000000,
        0x00000000, 0x00000000, 0x00000000, 0x00000000,
        0x00000000, 0x00000000, 0x00000000, 0x000001C0
    };

    int TC2_2_expected[8] = {
        0x248D6A61, 0xD20638B8, 0xE5C02693, 0x0C3E6039,
        0xA33CE459, 0x64FF2167, 0xF6ECEDD4, 0x19DB06C1
    };

    init(hash);

    next(hash, TC2_2_block, verbose);
    int* my_digest = getDigest(hash);
    compareDigests(my_digest, dut_res, 8);
}

void sha256Test4(Hash* hash, const char* mode, int dut_res[], bool verbose) {
    printf("Running test4 cases for SHA256:\n");
    sha256(hash, mode, verbose);

    int TC2_3_block[16] = {
        0x61000000, 0x62000000, 0x63000000, 0x64000000,
        0x65000000, 0x66000000, 0x67000000, 0x68000000,
        0x69000000, 0x6A000000, 0x6B000000, 0x6C000000,
        0x6D000000, 0x6E000000, 0x6F000000, 0x70000000
    };

    int TC2_3_expected[8] = {
        0x57F57E2D, 0x6DB8986B, 0xC4C3C01B, 0x117B85A4,
        0x03309BEF, 0x48A2CD5D, 0x84E3AA0C, 0xB8959E6F
    };

    init(hash);

    next(hash, TC2_3_block, verbose);
    int* my_digest = getDigest(hash);
    compareDigests(my_digest, dut_res, 8);
}

int main() {
    int dut_in[16] = {
        0x61626364, 0x62636465, 0x63646566, 0x64656667,
        0x65666768, 0x66676869, 0x6768696A, 0x68696A6B,
        0x696A6B6C, 0x6A6B6C6D, 0x6B6C6D6E, 0x6C6D6E6F,
        0x6D6E6F70, 0x6E6F7071, 0x80000000, 0x00000000
    };

    int dut_res[8] = {
        0x1E00C28D, 0x4E34B948, 0xA5263C39, 0xBA1F40D8,
        0x22A5F841, 0xF582A8B6, 0x4E61EFD5, 0x4777C50D
    };

    Hash hash;
    sha256Tests(dut_in, &hash, "sha256", dut_res, true);

    int dut_res_224[7] = {
        0xFD2B34E7, 0xFABE30A5, 0x06C22B5E, 0x4C1C03A3,
        0x60D09A09, 0x482A05A0, 0x2A5F2C16
    };

    sha256Tests(dut_in, &hash, "sha224", dut_res_224, true);

    return 0;
}
