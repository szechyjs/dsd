/*
 * Copyright (C) 2010 DSD Author
 * GPG Key ID: 0x3F1D7FD0 (74EF 430D F7F2 0A48 FCE6  F630 FAA2 635D 3F1D 7FD0)
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND ISC DISCLAIMS ALL WARRANTIES WITH
 * REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS.  IN NO EVENT SHALL ISC BE LIABLE FOR ANY SPECIAL, DIRECT,
 * INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
 * LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
 * OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 */

/*
 *  dstar interleave experiments 
 */

#ifndef _MAIN

extern const int dW[72];
extern const int dX[72];

#else

/*
 *  4x18
 */
/*
const int dW[72] = {
0,0,1,2,
0,0,1,2,
0,0,1,2,
0,0,1,2,
0,0,1,3,
0,0,1,3,
0,1,1,3,
0,1,1,3,
0,1,1,3,
0,1,1,3,
0,1,1,3,
0,1,2,3,
0,1,2,3,
0,1,2,3,
0,1,2,3,
0,1,2,3,
0,1,2,3,  
0,1,2,3
};

const int dX[72] = {
23,5,10,3,
22,4,9,2,
21,3,8,1,
20,2,7,0,
19,1,6,13,
18,0,5,12,
17,22,4,11,
16,21,3,10,
15,20,2,9,
14,19,1,8,
13,18,0,7,
12,17,10,6,
11,16,9,5,
10,15,8,4,
9,14,7,3,
8,13,6,2,
7,12,5,1,
6,11,4,0
};
*/

/*
 * 3x24 alt
 */
const int dW[72] = {
  1, 0, 3,
  1, 0, 3,
  1, 0, 3,
  1, 0, 3,
  1, 0, 3,
  1, 0, 3,
  1, 0, 3,
  1, 0, 3,
  1, 0, 3,
  1, 0, 3,
  1, 0, 3,
  1, 0, 3,
  1, 0, 3,
  1, 0, 2,
  1, 0, 2,
  1, 0, 2,
  1, 0, 2,
  1, 0, 2,
  1, 0, 2,
  1, 0, 2,
  1, 0, 2,
  1, 0, 2,
  1, 0, 2,
  0, 3, 2,
};

const int dX[72] = {
  22, 22, 12,
  21, 21, 11,
  20, 20, 10,
  19, 19, 9,
  18, 18, 8,
  17, 17, 7,
  16, 16, 6,
  15, 15, 5,
  14, 14, 4,
  13, 13, 3,
  12, 12, 2,
  11, 11, 1,
  10, 10, 0,
  9, 9, 10,
  8, 8, 9,
  7, 7, 8,
  6, 6, 7,
  5, 5, 6,
  4, 4, 5,
  3, 3, 4,
  2, 2, 3,
  1, 1, 2,
  0, 0, 1,
  23, 13, 0
};

/*
 * 3x24
 */
/*
const int dW[72] = {
0,1,2,
0,1,2,
0,1,2,
0,1,2,
0,1,2,
0,1,2,
0,1,2,
0,1,2,
0,1,2,
0,1,2,
0,1,3,
0,1,3,
0,1,3,
0,1,3,
0,1,3,
0,1,3,
0,1,3,
0,1,3,
0,1,3,
0,1,3,
0,1,3,
0,1,3,
0,1,3,
0,2,3,
};

const int dX[72] = {
23,22,9,
22,21,8,
21,20,7,
20,19,6,
19,18,5,
18,17,4,
17,16,3,
16,15,2,
15,14,1,
14,13,0,
13,12,13,
12,11,12,
11,10,11,
10,9,10,
9,8,9,
8,7,8,
7,6,7,
6,5,6,
5,4,5,
4,3,4,
3,2,3,
2,1,2,
1,0,1,
0,10,0
};
*/


/*
 * 2x36
 */
/*
const int dW[72] = {
0,1,
0,1,
0,1,
0,1,
0,1,
0,1,
0,1,
0,1,
0,1,
0,1,
0,1,
0,2,
0,2,
0,2,
0,2,
0,2,
0,2,
0,2,
0,2,
0,2,
0,2,
0,2,
0,3,
0,3,
1,3,
1,3,
1,3,
1,3,
1,3,
1,3,
1,3,
1,3,
1,3,
1,3,
1,3,
1,3
};

const int dX[72] = {
23,10,
22,9,
21,8,
20,7,
19,6,
18,5,
17,4,
16,3,
15,2,
14,1,
13,0,
12,10,
11,9,
10,8,
9,7,
8,6,
7,5,
6,4,
5,3,
4,2,
3,1,
2,0,
1,13,
0,12,
22,11,
21,10,
20,9,
19,8,
18,7,
17,6,
16,5,
15,4,
14,3,
13,2,
12,1,
11,0
};
*/

/*
 * non-interleaved
 */
/*
const int dW[72] = {
0,0,0,0,0,0,
0,0,0,0,0,0,
0,0,0,0,0,0,
0,0,0,0,0,0,
1,1,1,1,1,1,
1,1,1,1,1,1,
1,1,1,1,1,1,
1,1,1,1,1,2,
2,2,2,2,2,2,
2,2,2,2,3,3,
3,3,3,3,3,3,
3,3,3,3,3,3
};

const int dX[72] = {
23,22,21,20,19,18,
17,16,15,14,13,12,
11,10,9,8,7,6,
5,4,3,2,1,0,
22,21,20,19,18,17,
16,15,14,13,12,11,
10,9,8,7,6,5,
4,3,2,1,0,10,
9,8,7,6,5,4,
3,2,1,0,13,12,
11,10,9,8,7,6,
5,4,3,2,1,0
};
*/

#endif
