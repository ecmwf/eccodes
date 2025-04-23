#define ROT(x, c) ((x << c) | (x >> (32 - c)))

#define ECC_F(x, y, z) ((x & y) | ((~x) & z))
#define ECC_G(x, y, z) ((x & z) | (y & (~z)))
#define ECC_H(x, y, z) (x ^ y ^ z)
#define ECC_I(x, y, z) (y ^ (x | (~z)))

#define F_(A, B, C, D, g, i)        \
    A += ECC_F(B, C, D) + w[g] + k[i]; \
    A &= 0xffffffff;                \
    A = ROT(A, r[i]);               \
    A += B;
#define G_(A, B, C, D, g, i)        \
    A += ECC_G(B, C, D) + w[g] + k[i]; \
    A &= 0xffffffff;                \
    A = ROT(A, r[i]);               \
    A += B;
#define H_(A, B, C, D, g, i)        \
    A += ECC_H(B, C, D) + w[g] + k[i]; \
    A &= 0xffffffff;                \
    A = ROT(A, r[i]);               \
    A += B;
#define I_(A, B, C, D, g, i)        \
    A += ECC_I(B, C, D) + w[g] + k[i]; \
    A &= 0xffffffff;                \
    A = ROT(A, r[i]);               \
    A += B;

static void grib_md5_flush(grib_md5_state* s)
{
#if 1
    unsigned long a  = s->h0;
    unsigned long b  = s->h1;
    unsigned long c  = s->h2;
    unsigned long d  = s->h3;
    unsigned long* w = &s->words[0];

    F_(a, b, c, d, 0, 0);
    F_(d, a, b, c, 1, 1);
    F_(c, d, a, b, 2, 2);
    F_(b, c, d, a, 3, 3);

    F_(a, b, c, d, 4, 4);
    F_(d, a, b, c, 5, 5);
    F_(c, d, a, b, 6, 6);
    F_(b, c, d, a, 7, 7);

    F_(a, b, c, d, 8, 8);
    F_(d, a, b, c, 9, 9);
    F_(c, d, a, b, 10, 10);
    F_(b, c, d, a, 11, 11);

    F_(a, b, c, d, 12, 12);
    F_(d, a, b, c, 13, 13);
    F_(c, d, a, b, 14, 14);
    F_(b, c, d, a, 15, 15);

    G_(a, b, c, d, 1, 16);
    G_(d, a, b, c, 6, 17);
    G_(c, d, a, b, 11, 18);
    G_(b, c, d, a, 0, 19);

    G_(a, b, c, d, 5, 20);
    G_(d, a, b, c, 10, 21);
    G_(c, d, a, b, 15, 22);
    G_(b, c, d, a, 4, 23);

    G_(a, b, c, d, 9, 24);
    G_(d, a, b, c, 14, 25);
    G_(c, d, a, b, 3, 26);
    G_(b, c, d, a, 8, 27);

    G_(a, b, c, d, 13, 28);
    G_(d, a, b, c, 2, 29);
    G_(c, d, a, b, 7, 30);
    G_(b, c, d, a, 12, 31);

    H_(a, b, c, d, 5, 32);
    H_(d, a, b, c, 8, 33);
    H_(c, d, a, b, 11, 34);
    H_(b, c, d, a, 14, 35);

    H_(a, b, c, d, 1, 36);
    H_(d, a, b, c, 4, 37);
    H_(c, d, a, b, 7, 38);
    H_(b, c, d, a, 10, 39);

    H_(a, b, c, d, 13, 40);
    H_(d, a, b, c, 0, 41);
    H_(c, d, a, b, 3, 42);
    H_(b, c, d, a, 6, 43);

    H_(a, b, c, d, 9, 44);
    H_(d, a, b, c, 12, 45);
    H_(c, d, a, b, 15, 46);
    H_(b, c, d, a, 2, 47);

    I_(a, b, c, d, 0, 48);
    I_(d, a, b, c, 7, 49);
    I_(c, d, a, b, 14, 50);
    I_(b, c, d, a, 5, 51);

    I_(a, b, c, d, 12, 52);
    I_(d, a, b, c, 3, 53);
    I_(c, d, a, b, 10, 54);
    I_(b, c, d, a, 1, 55);

    I_(a, b, c, d, 8, 56);
    I_(d, a, b, c, 15, 57);
    I_(c, d, a, b, 6, 58);
    I_(b, c, d, a, 13, 59);

    I_(a, b, c, d, 4, 60);
    I_(d, a, b, c, 11, 61);
    I_(c, d, a, b, 2, 62);
    I_(b, c, d, a, 9, 63);

#else
    unsigned long i, g;
    unsigned long a = s->h0;
    unsigned long b = s->h1;
    unsigned long c = s->h2;
    unsigned long d = s->h3;
    unsigned long f;
    unsigned long temp;
    unsigned long* w = &s->words[0];
    unsigned long h;
    for (i = 0; i < 16; i++) {
        f    = F(b, c, d);
        g    = i;
        temp = d;
        d    = c;
        c    = b;
        h    = a + f + k[i] + w[g];
        b    = b + rotate(h, r[i]);
        a    = temp;
    }

    for (i = 16; i < 32; i++) {
        f    = G(b, c, d);
        g    = (5 * i + 1) % 16;
        temp = d;
        d    = c;
        c    = b;
        h    = a + f + k[i] + w[g];
        b    = b + rotate(h, r[i]);
        a    = temp;
    }
    for (i = 32; i < 48; i++) {
        f    = H(b, c, d);
        g    = (3 * i + 5) % 16;
        temp = d;
        d    = c;
        c    = b;
        h    = a + f + k[i] + w[g];
        b    = b + rotate(h, r[i]);
        a    = temp;
    }
    for (i = 48; i < 64; i++) {
        f    = I(b, c, d);
        g    = (7 * i) % 16;
        temp = d;
        d    = c;
        c    = b;
        h    = a + f + k[i] + w[g];
        b    = b + rotate(h, r[i]);
        a    = temp;
    }

#endif

    s->h0 += a;
    s->h1 += b;
    s->h2 += c;
    s->h3 += d;

    s->word_count = 0;
}
