/* patch for differing sizes for longs and ints on Dec Alphas */
#ifndef ALPHA_H
#ifdef ALPHA
#define ALPHA_H
#define long int
#endif
#endif

