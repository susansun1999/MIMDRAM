#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <immintrin.h>
#include <m5op.h>
#include "mimdram.h"

int main(int argc, char **argv) {
    init_ambit();
    srand(121324314);

    int col_length = atoi(argv[1]);
    int num_vals = 1024 << (atoi(argv[2]));

    int total_bits = col_length * num_vals;
    int total_bytes = total_bits / 8;
    int total_ints = total_bits / 32;
    int total_vecs = total_bits / 128;

    int per_col_bits = num_vals;
    int per_col_bytes = num_vals / 8;
    int per_col_ints = num_vals / 32;
    int per_col_vecs = num_vals / 128;
    int per_col_rows = (per_col_bytes + ROW_SIZE - 1) / ROW_SIZE;


    // allocate operands data
    unsigned **vals1 = random_vector_array(per_col_bytes, col_length);
    unsigned **vals2 = random_vector_array(per_col_bytes, col_length);

    // allocate output
    unsigned *output = allocate_vector(per_col_bytes);

	// run some iterations of the algorithm
    for (int iter = 0; iter < 2; iter ++) {
        m5_reset_stats(0,0);

		FOR_ALL_VECTORS {

			AAP_VECTORS (B_T2_T3, C_1)

			for (int j = 0; j < col_length; j ++) {
				unsigned *v1 = VECTOR(vals1[j]);
				unsigned *v2 = VECTOR(vals2[j]);

				AAP_VECTORS (B_DCC0N_T0  , v1)
				AAP_VECTORS (B_DCC1N_T1  , v2)
				AP_VECTOR   (B_DCC0_T1_T2    )
				AP_VECTOR   (B_DCC1_T0_T3    )
			}

			AAP_VECTORS (B_T1,           C_0       )
			AAP_VECTORS (VECTOR(output), B_T0_T1_T2)
		}

    }
    m5_dump_stats(0,0);

    return 0;
}
