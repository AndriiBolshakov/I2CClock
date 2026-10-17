#include "font.h"

// -----------------------------------------------------------------------------
// Minimal 5x7 font
// Each byte represents one vertical column; bit 0 is the top pixel.
// -----------------------------------------------------------------------------

const uint8_t *font5x7(char character)
{
    static const uint8_t space[5] = {
        0x00, 0x00, 0x00, 0x00, 0x00
    };

    static const uint8_t colon[5] = {
        0x00, 0x36, 0x36, 0x00, 0x00
    };

    static const uint8_t period[5] = {
        0x00, 0x60, 0x60, 0x00, 0x00
    };

    // -------------------------------------------------------------------------
    // Digits
    // -------------------------------------------------------------------------

    static const uint8_t digits[10][5] = {
        {0x3E, 0x51, 0x49, 0x45, 0x3E}, // 0
        {0x00, 0x42, 0x7F, 0x40, 0x00}, // 1
        {0x42, 0x61, 0x51, 0x49, 0x46}, // 2
        {0x21, 0x41, 0x45, 0x4B, 0x31}, // 3
        {0x18, 0x14, 0x12, 0x7F, 0x10}, // 4
        {0x27, 0x45, 0x45, 0x45, 0x39}, // 5
        {0x3C, 0x4A, 0x49, 0x49, 0x30}, // 6
        {0x01, 0x71, 0x09, 0x05, 0x03}, // 7
        {0x36, 0x49, 0x49, 0x49, 0x36}, // 8
        {0x06, 0x49, 0x49, 0x29, 0x1E}, // 9
    };

    // -------------------------------------------------------------------------
    // Uppercase letters
    // -------------------------------------------------------------------------

    static const uint8_t A[5] = {
        0x7E, 0x11, 0x11, 0x11, 0x7E
    };

    static const uint8_t B[5] = {
        0x7F, 0x49, 0x49, 0x49, 0x36
    };

    static const uint8_t C[5] = {
        0x3E, 0x41, 0x41, 0x41, 0x22
    };

    static const uint8_t D[5] = {
        0x7F, 0x41, 0x41, 0x22, 0x1C
    };

    static const uint8_t E[5] = {
        0x7F, 0x49, 0x49, 0x49, 0x41
    };

    static const uint8_t F[5] = {
        0x7F, 0x09, 0x09, 0x09, 0x01
    };

    static const uint8_t G[5] = {
        0x3E, 0x41, 0x49, 0x49, 0x7A
    };

    static const uint8_t H[5] = {
        0x7F, 0x08, 0x08, 0x08, 0x7F
    };

    static const uint8_t I[5] = {
        0x00, 0x41, 0x7F, 0x41, 0x00
    };

    static const uint8_t J[5] = {
        0x20, 0x40, 0x41, 0x3F, 0x01
    };

    static const uint8_t L[5] = {
        0x7F, 0x40, 0x40, 0x40, 0x40
    };

    static const uint8_t M[5] = {
        0x7F, 0x02, 0x0C, 0x02, 0x7F
    };

    static const uint8_t N[5] = {
        0x7F, 0x04, 0x08, 0x10, 0x7F
    };

    static const uint8_t O[5] = {
        0x3E, 0x41, 0x41, 0x41, 0x3E
    };

    static const uint8_t P[5] = {
        0x7F, 0x09, 0x09, 0x09, 0x06
    };

    static const uint8_t R[5] = {
        0x7F, 0x09, 0x19, 0x29, 0x46
    };

    static const uint8_t S[5] = {
        0x46, 0x49, 0x49, 0x49, 0x31
    };

    static const uint8_t T[5] = {
        0x01, 0x01, 0x7F, 0x01, 0x01
    };

    static const uint8_t U[5] = {
        0x3F, 0x40, 0x40, 0x40, 0x3F
    };

    static const uint8_t V[5] = {
        0x1F, 0x20, 0x40, 0x20, 0x1F
    };

    static const uint8_t Y[5] = {
        0x07, 0x08, 0x70, 0x08, 0x07
    };

    // -------------------------------------------------------------------------
    // Lowercase letters
    // -------------------------------------------------------------------------

    static const uint8_t a[5] = {
        0x20, 0x54, 0x54, 0x54, 0x78
    };

    static const uint8_t b[5] = {
        0x7F, 0x48, 0x44, 0x44, 0x38
    };

    static const uint8_t c[5] = {
        0x38, 0x44, 0x44, 0x44, 0x28
    };

    static const uint8_t d[5] = {
        0x38, 0x44, 0x44, 0x48, 0x7F
    };

    static const uint8_t e[5] = {
        0x38, 0x54, 0x54, 0x54, 0x18
    };

    static const uint8_t f[5] = {
        0x08, 0x7E, 0x09, 0x01, 0x02
    };

    static const uint8_t g[5] = {
        0x0C, 0x52, 0x52, 0x52, 0x3E
    };

    static const uint8_t h[5] = {
        0x7F, 0x08, 0x04, 0x04, 0x78
    };

    static const uint8_t i[5] = {
        0x00, 0x44, 0x7D, 0x40, 0x00
    };

    static const uint8_t j[5] = {
        0x20, 0x40, 0x44, 0x3D, 0x00
    };

    static const uint8_t k[5] = {
        0x7F, 0x10, 0x28, 0x44, 0x00
    };

    static const uint8_t l[5] = {
        0x00, 0x41, 0x7F, 0x40, 0x00
    };

    static const uint8_t m[5] = {
        0x7C, 0x04, 0x18, 0x04, 0x78
    };

    static const uint8_t n[5] = {
        0x7C, 0x08, 0x04, 0x04, 0x78
    };

    static const uint8_t o[5] = {
        0x38, 0x44, 0x44, 0x44, 0x38
    };

    static const uint8_t p[5] = {
        0x7C, 0x14, 0x14, 0x14, 0x08
    };

    static const uint8_t q[5] = {
        0x08, 0x14, 0x14, 0x18, 0x7C
    };

    static const uint8_t r[5] = {
        0x7C, 0x08, 0x04, 0x04, 0x08
    };

    static const uint8_t s[5] = {
        0x48, 0x54, 0x54, 0x54, 0x24
    };

    static const uint8_t t[5] = {
        0x04, 0x3F, 0x44, 0x40, 0x20
    };

    static const uint8_t u[5] = {
        0x3C, 0x40, 0x40, 0x20, 0x7C
    };

    static const uint8_t v[5] = {
        0x1C, 0x20, 0x40, 0x20, 0x1C
    };

    static const uint8_t w[5] = {
        0x3C, 0x40, 0x30, 0x40, 0x3C
    };

    static const uint8_t x[5] = {
        0x44, 0x28, 0x10, 0x28, 0x44
    };

    static const uint8_t y[5] = {
        0x0C, 0x50, 0x50, 0x50, 0x3C
    };

    static const uint8_t z[5] = {
        0x44, 0x64, 0x54, 0x4C, 0x44
    };

    // -------------------------------------------------------------------------
    // Digits
    // -------------------------------------------------------------------------

    if (character >= '0' && character <= '9') {
        return digits[character - '0'];
    }

    // -------------------------------------------------------------------------
    // Characters
    // -------------------------------------------------------------------------

    switch (character) {
        case ' ': return space;
        case ':': return colon;
        case '.': return period;

        // Uppercase
        case 'A': return A;
        case 'B': return B;
        case 'C': return C;
        case 'D': return D;
        case 'E': return E;
        case 'F': return F;
        case 'G': return G;
        case 'H': return H;
        case 'I': return I;
        case 'J': return J;
        case 'L': return L;
        case 'M': return M;
        case 'N': return N;
        case 'O': return O;
        case 'P': return P;
        case 'R': return R;
        case 'S': return S;
        case 'T': return T;
        case 'U': return U;
        case 'V': return V;
        case 'Y': return Y;

        // Lowercase
        case 'a': return a;
        case 'b': return b;
        case 'c': return c;
        case 'd': return d;
        case 'e': return e;
        case 'f': return f;
        case 'g': return g;
        case 'h': return h;
        case 'i': return i;
        case 'j': return j;
        case 'k': return k;
        case 'l': return l;
        case 'm': return m;
        case 'n': return n;
        case 'o': return o;
        case 'p': return p;
        case 'q': return q;
        case 'r': return r;
        case 's': return s;
        case 't': return t;
        case 'u': return u;
        case 'v': return v;
        case 'w': return w;
        case 'x': return x;
        case 'y': return y;
        case 'z': return z;

        default:
            return space;
    }
}