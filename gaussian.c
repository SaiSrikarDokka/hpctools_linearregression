#include "gaussian.h"
#include <stdlib.h>
#include <math.h>

/* -------------------------------------------------------------------------
 * TODO (STUDENT): gaussian_elimination_solve
 *
 * Solve the p x p system:
 *
 *   XtX * beta = Xty
 *
 * using Gaussian elimination with partial pivoting, followed by back
 * substitution:
 *
 *   1. Build an augmented p x (p+1) matrix [XtX | Xty] (work on a local
 *      copy — do not modify XtX/Xty in place, you may want to keep them
 *      for the report).
 *   2. Forward elimination: for each pivot column k = 0..p-1,
 *        a. partial pivoting: find the row r >= k with the largest
 *           absolute value in column k, and swap rows k and r if r != k
 *           (this avoids dividing by a very small/zero pivot).
 *        b. eliminate column k from all rows below k by subtracting an
 *           appropriate multiple of row k.
 *   3. Back substitution: once the augmented matrix is in upper
 *      triangular form, solve for beta[p-1], beta[p-2], ..., beta[0]
 *      from the bottom row upward.
 *
 * XtX  : p x p, row-major (read-only)
 * Xty  : p (right-hand side, read-only)
 * beta : p (output, caller-allocated)
 * ---------------------------------------------------------------------- */
void gaussian_elimination_solve(const double *XtX, const double *Xty,
                                double *beta, int p) {
    double *aug = malloc((size_t)p * (p + 1) * sizeof(double));

    if (aug == NULL) {
        return;
    }

    /* Copy XtX and Xty into augmented matrix. */
    for (int i = 0; i < p; i++) {

        for (int j = 0; j < p; j++) {
            aug[i * (p + 1) + j] =
                XtX[i * p + j];
        }

        aug[i * (p + 1) + p] = Xty[i];
    }

    /* Forward elimination with partial pivoting. */
    for (int k = 0; k < p - 1; k++) {

        /* Find row with largest absolute value in pivot column. */
        int pivot = k;

        for (int i = k + 1; i < p; i++) {
            if (fabs(aug[i * (p + 1) + k]) >
                fabs(aug[pivot * (p + 1) + k])) {
                pivot = i;
            }
        }

        /* Swap rows if necessary. */
        if (pivot != k) {
            for (int j = 0; j <= p; j++) {
                double temp = aug[k * (p + 1) + j];

                aug[k * (p + 1) + j] =
                    aug[pivot * (p + 1) + j];

                aug[pivot * (p + 1) + j] = temp;
            }
        }

        /* Eliminate entries below the pivot. */
        for (int i = k + 1; i < p; i++) {

            double factor =
                aug[i * (p + 1) + k] /
                aug[k * (p + 1) + k];

            for (int j = k; j <= p; j++) {
                aug[i * (p + 1) + j] -=
                    factor * aug[k * (p + 1) + j];
            }
        }
    }

    /* Back substitution. */
    for (int i = p - 1; i >= 0; i--) {

        beta[i] = aug[i * (p + 1) + p];

        for (int j = i + 1; j < p; j++) {
            beta[i] -=
                aug[i * (p + 1) + j] * beta[j];
        }

        beta[i] /= aug[i * (p + 1) + i];
    }

    /* Free temporary augmented matrix. */
    free(aug);

}
