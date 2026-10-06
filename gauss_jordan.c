#include "gauss_jordan.h"
#include <math.h>
#include <stdlib.h>

void gauss_jordan_solve(const double *XtX, const double *Xty,
                        double *beta, int p)
{
    /* Augmented matrix [XtX | Xty] */
    double *aug = malloc((size_t)p * (p + 1) * sizeof(double));

    if (aug == NULL) {
        return;
    }

    /* Copy XtX and Xty into augmented matrix */
    for (int i = 0; i < p; i++) {
        for (int j = 0; j < p; j++) {
            aug[i * (p + 1) + j] = XtX[i * p + j];
        }

        aug[i * (p + 1) + p] = Xty[i];
    }

    /*
     * Gauss-Jordan elimination
     * Transform [A | b] into [I | beta]
     */
    for (int k = 0; k < p; k++) {

        /* Find pivot row */
        int pivot = k;

        for (int i = k + 1; i < p; i++) {
            if (fabs(aug[i * (p + 1) + k]) >
                fabs(aug[pivot * (p + 1) + k])) {
                pivot = i;
            }
        }

        /* Swap pivot row with current row */
        if (pivot != k) {
            for (int j = 0; j <= p; j++) {
                double temp = aug[k * (p + 1) + j];

                aug[k * (p + 1) + j] =
                    aug[pivot * (p + 1) + j];

                aug[pivot * (p + 1) + j] = temp;
            }
        }

        /* Normalize pivot row so pivot becomes 1 */
        double pivot_value = aug[k * (p + 1) + k];

        for (int j = 0; j <= p; j++) {
            aug[k * (p + 1) + j] /= pivot_value;
        }

        /*
         * Eliminate the current column
         * from every other row.
         */
        for (int i = 0; i < p; i++) {
            if (i == k) {
                continue;
            }

            double factor = aug[i * (p + 1) + k];

            for (int j = 0; j <= p; j++) {
                aug[i * (p + 1) + j] -=
                    factor * aug[k * (p + 1) + j];
            }
        }
    }

    /* The right-hand side now contains beta */
    for (int i = 0; i < p; i++) {
        beta[i] = aug[i * (p + 1) + p];
    }
    free(aug);
}