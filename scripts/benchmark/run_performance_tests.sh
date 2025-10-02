#!/bin/bash

# Performance testing script for Random Forest implementations
# This script runs comprehensive performance tests with varying thread counts and dataset sizes

set -e

PROJECT_ROOT="$(dirname "$(dirname "$(dirname "$(realpath "$0")")")")"
cd "$PROJECT_ROOT"

# Configuration
RESULTS_DIR="$PROJECT_ROOT/results/performance"
PARALLEL_BIN="$PROJECT_ROOT/bin/rf_parallel"
DATA_DIR="$PROJECT_ROOT/data/processed"

# Thread counts to test
THREAD_COUNTS=(1 2 4 8 12 16 20 24)

# Number of test iterations for statistical significance
ITERATIONS=3

# Create results directory
mkdir -p "$RESULTS_DIR"

echo "=== Random Forest Performance Testing ==="
echo "Results will be saved to: $RESULTS_DIR"

# Function to run performance test for a specific configuration
run_test() {
    local binary=$1
    local dataset=$2
    local threads=$3
    local output_file=$4
    
    echo "Testing: $(basename "$binary") with $threads threads on $(basename "$dataset")"
    
    # Set OpenMP thread count
    export OMP_NUM_THREADS=$threads
    
    # Run multiple iterations and collect timing data
    for i in $(seq 1 $ITERATIONS); do
        echo "  Iteration $i/$ITERATIONS"
        
        # Run the test and capture timing output
        # Format: dataset,threads,iteration,time_seconds,accuracy
        # Only extract lines starting with "RESULT," and filter out the "RESULT," prefix
        if timeout 240 "$binary" "$dataset" -r 0.6 2>&1 | grep "^RESULT," | sed 's/^RESULT,//' >> "$output_file"; then
            echo "    ✓ Completed"
        else
            echo "    ✗ Failed or timeout"
            # Ensure a CSV entry is still recorded for failures/timeouts
            # Format: dataset,threads,iteration,time_seconds,accuracy
            echo "${dataset},${threads},${i},-1,-1" >> "$output_file"
        fi
    done
}

# Function to test all configurations for a dataset
test_dataset() {
    local dataset=$1
    local dataset_name=$(basename "$dataset" .csv)
    
    echo "--- Testing dataset: $dataset_name ---"
    
    # Results file
    local par_results="$RESULTS_DIR/${dataset_name}_parallel.csv"
    
    # Clear previous results
    echo "dataset,threads,iteration,time_seconds,accuracy" > "$par_results"
    
    # Test parallel version with different thread counts
    echo "Testing parallel implementation with varying thread counts..."
    for threads in "${THREAD_COUNTS[@]}"; do
        run_test "$PARALLEL_BIN" "$dataset" "$threads" "$par_results"
    done
}

# Main execution
main() {
    # Check if parallel binary exists
    if [[ ! -f "$PARALLEL_BIN" ]]; then
        echo "Error: Parallel binary not found. Please compile first."
        echo "Run: make parallel"
        exit 1
    fi
    
    # Find all processed datasets
    local datasets=()
    if [[ -d "$DATA_DIR" ]]; then
        while IFS= read -r -d '' dataset; do
            datasets+=("$dataset")
        done < <(find "$DATA_DIR" -name "*.csv" -type f -print0)
    fi
    
    if [[ ${#datasets[@]} -eq 0 ]]; then
        echo "Warning: No datasets found in $DATA_DIR"
        echo "Please add datasets to test with"
        exit 1
    fi
    
    # Test each dataset
    for dataset in "${datasets[@]}"; do
        test_dataset "$dataset"
    done
    
    echo "Performance testing completed!"
    echo "Results saved in: $RESULTS_DIR"
    echo "Next steps:"
    echo "  1. Run analysis script to generate graphs"
    echo "  2. Run VTune profiling with: ./scripts/vtune/run_vtune_analysis.sh"
}

# Run main function
main "$@"
