#!/bin/bash

#SBATCH --job-name=linreg-bench
#SBATCH --output=benchmark.out
#SBATCH --error=linreg_%j.err
#SBATCH --time=02:00:00
#SBATCH --cpus-per-task=1
#SBATCH --mem=16G

set -e


# ============================================================
# Benchmark configuration
# ============================================================

RUNS=5

SOURCES="linreg.c gemm.c gemv.c gaussian.c gauss_jordan.c rng.c"

BUILD_DIR="benchmark_build"
RESULTS_DIR="benchmark_results"

mkdir -p "$BUILD_DIR"
mkdir -p "$RESULTS_DIR"

SUMMARY="${RESULTS_DIR}/summary.csv"

echo "compiler,optimization,solver,N,p,average_ms,min_ms,max_ms" > "$SUMMARY"


# ============================================================
# Benchmark function
# ============================================================

benchmark()
{
    COMPILER_NAME=$1
    COMPILER=$2
    OPT=$3
    EXTRA_FLAGS=$4

    echo
    echo "############################################################"
    echo "# Compiler       : $COMPILER_NAME"
    echo "# Optimization   : $OPT"
    echo "# Extra flags    : $EXTRA_FLAGS"
    echo "############################################################"

    BINARY="${BUILD_DIR}/linreg_${COMPILER_NAME}_${OPT#-}"

    echo
    echo "Building: $BINARY"

    $COMPILER \
        -std=c11 \
        $OPT \
        $EXTRA_FLAGS \
        -D_GNU_SOURCE \
        -D_POSIX_C_SOURCE=200809L \
        -DM_PI=3.14159265358979323846 \
        $SOURCES \
        -lm \
        -o "$BINARY"

    echo "Build successful."


    # ========================================================
    # Problem configurations
    # ========================================================

    for CONFIG in 1 2 3
    do

        case $CONFIG in

            1)
                N=20000
                P=50
                ;;

            2)
                N=50000
                P=300
                ;;

            3)
                N=2000
                P=2000
                ;;

        esac


        # ====================================================
        # Solvers
        # ====================================================

        for SOLVER in gaussian gauss_jordan
        do

            TIMES=()


            echo
            echo "============================================================"
            echo "Compiler       : $COMPILER_NAME"
            echo "Optimization   : $OPT"
            echo "Solver         : $SOLVER"
            echo "Configuration  : N=$N p=$P"
            echo "============================================================"


            # =================================================
            # Multiple runs
            # =================================================

            for ((RUN=1; RUN<=RUNS; RUN++))
            do

                echo -n "  Run $RUN/$RUNS ... "


                OUTPUT=$(
                    "$BINARY" "$N" "$P" 42 "$SOLVER"
                )


                # Extract total compute time
                TIME=$(echo "$OUTPUT" |
                    grep "Total compute time:" |
                    awk '{print $4}')


                if [ -z "$TIME" ]; then

                    echo "ERROR"

                    echo
                    echo "Program output:"
                    echo "$OUTPUT"

                    exit 1

                fi


                # Convert seconds -> milliseconds
                TIME_MS=$(awk -v t="$TIME" \
                    'BEGIN {printf "%.0f", t * 1000}')


                TIMES+=("$TIME_MS")


                echo "${TIME_MS} ms"

            done


            # =================================================
            # Calculate statistics
            # =================================================

            AVERAGE=$(printf "%s\n" "${TIMES[@]}" |
                awk '
                {
                    sum += $1
                }
                END {
                    printf "%.2f", sum / NR
                }')


            MIN=$(printf "%s\n" "${TIMES[@]}" |
                sort -n |
                head -1)


            MAX=$(printf "%s\n" "${TIMES[@]}" |
                sort -n |
                tail -1)


            # =================================================
            # Display results
            # =================================================

            echo
            echo "  Runs: $RUNS"


            printf "  Times: ["

            for ((i=0; i<${#TIMES[@]}; i++))
            do

                if [ "$i" -gt 0 ]; then
                    printf ", "
                fi

                printf "%s" "${TIMES[$i]}"

            done

            echo "]"


            echo "  Average: ${AVERAGE} ms"
            echo "  Min:     ${MIN} ms"
            echo "  Max:     ${MAX} ms"


            # =================================================
            # Save to CSV
            # =================================================

            echo "$COMPILER_NAME,$OPT,$SOLVER,$N,$P,$AVERAGE,$MIN,$MAX" \
                >> "$SUMMARY"


            echo

        done

    done
}


# ============================================================
# GCC 12.3.0
# ============================================================

module purge

module load cesga/2020
module load gcc/12.3.0


echo
echo "============================================================"
echo "GCC environment"
echo "============================================================"

gcc --version | head -1


# GCC O0
benchmark gcc gcc -O0 ""

# GCC O2
benchmark gcc gcc -O2 "-march=native"

# GCC O3
benchmark gcc gcc -O3 "-march=native"

# GCC Ofast
benchmark gcc gcc -Ofast "-march=native"



# ============================================================
# Intel ICC 2021.3.0 and ICX 2021.3.0
# ============================================================

module purge

module load cesga/2020
module load intel


echo
echo "============================================================"
echo "Intel environment"
echo "============================================================"

echo "ICC:"
icc --version | head -1

echo "ICX:"
icx --version | head -1



# ============================================================
# ICC 2021.3.0
# ============================================================

benchmark icc icc -O0 ""

benchmark icc icc -O2 "-march=native"

benchmark icc icc -O3 "-march=native"

benchmark icc icc -Ofast "-march=native"



# ============================================================
# ICX 2021.3.0
# ============================================================

benchmark icx icx -O0 ""

benchmark icx icx -O2 "-march=native"

benchmark icx icx -O3 "-march=native"

benchmark icx icx -Ofast "-march=native"



# ============================================================
# Final results
# ============================================================

echo
echo "############################################################"
echo "#                    FINAL RESULTS                         #"
echo "############################################################"
echo


if command -v column >/dev/null 2>&1
then

    column -t -s ',' "$SUMMARY"

else

    cat "$SUMMARY"

fi


echo
echo "############################################################"
echo "# Results saved to:"
echo "# $SUMMARY"
echo "############################################################"
echo