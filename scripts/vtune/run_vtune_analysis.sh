#!/bin/bash

# Intel VTune Profiler analysis script for Random Forest implementations
# This script runs the three recommended analysis types: performance-snapshot, hotspots, and hpc-performance

set -e

PROJECT_ROOT="$(dirname "$(dirname "$(dirname "$(realpath "$0")")")")"
cd "$PROJECT_ROOT"

# Configuration
RESULTS_DIR="$PROJECT_ROOT/results/profiling"
PARALLEL_BIN="$PROJECT_ROOT/bin/rf_parallel"
DATA_DIR="$PROJECT_ROOT/data/processed"

# VTune analysis types
ANALYSIS_TYPES=("performance-snapshot" "hotspots" "hpc-performance")

# Thread count for profiling (use optimal from performance tests)
PROFILE_THREADS=20

# Create results directory
mkdir -p "$RESULTS_DIR"

echo "=== Intel VTune Profiler Analysis ==="
echo "Results will be saved to: $RESULTS_DIR"

# Function to check VTune availability
check_vtune() {
    if command -v vtune &> /dev/null; then
        echo "✓ VTune command found"
        return 0
    else
        echo "VTune not found. Loading environment..."
        # Try to source VTune environment (adjust path as needed)
        if [[ -f "/home/intel/oneapi/vtune/2021.1.1/vtune-vars.sh" ]]; then
            source /home/intel/oneapi/vtune/2021.1.1/vtune-vars.sh
            echo "✓ VTune environment loaded"
        else
            echo "✗ Error: VTune not found and environment script not available"
            echo "Please ensure VTune is installed and accessible"
            exit 1
        fi
    fi
}

# Function to run VTune analysis
run_vtune_analysis() {
    local analysis_type=$1
    local dataset=$2
    local dataset_name=$(basename "$dataset" .csv)
    local result_dir="$RESULTS_DIR/${dataset_name}_${analysis_type}"
    
    echo "Running $analysis_type analysis on $dataset_name..."
    
    # Set OpenMP thread count
    export OMP_NUM_THREADS=$PROFILE_THREADS
    
    # Run VTune analysis
    vtune -collect "$analysis_type" \
          -result-dir "$result_dir" \
          -app-working-dir "$PROJECT_ROOT" \
          -- "$PARALLEL_BIN" "$dataset"
    
    # Generate report
    local report_file="$RESULTS_DIR/${dataset_name}_${analysis_type}_report.txt"
    vtune -report summary -result-dir "$result_dir" > "$report_file"
    
    echo "✓ $analysis_type analysis completed"
    echo "  Result directory: $result_dir"
    echo "  Report file: $report_file"
}

# Function to analyze dataset with all VTune analysis types
analyze_dataset() {
    local dataset=$1
    local dataset_name=$(basename "$dataset" .csv)
    
    echo "--- Analyzing dataset: $dataset_name ---"
    
    for analysis_type in "${ANALYSIS_TYPES[@]}"; do
        run_vtune_analysis "$analysis_type" "$dataset"
    done
}

# Function to generate summary report
generate_summary() {
    local summary_file="$RESULTS_DIR/vtune_summary.md"
    
    echo "# Intel VTune Profiler Analysis Summary" > "$summary_file"
    echo "" >> "$summary_file"
    echo "Generated on: $(date)" >> "$summary_file"
    echo "Thread count used: $PROFILE_THREADS" >> "$summary_file"
    echo "" >> "$summary_file"
    
    echo "## Analysis Results" >> "$summary_file"
    echo "" >> "$summary_file"
    
    # List all result directories
    for result_dir in "$RESULTS_DIR"/*; do
        if [[ -d "$result_dir" && "$result_dir" != *"summary"* ]]; then
            local dir_name=$(basename "$result_dir")
            echo "- $dir_name" >> "$summary_file"
        fi
    done
    
    echo "" >> "$summary_file"
    echo "## Key Metrics to Analyze" >> "$summary_file"
    echo "" >> "$summary_file"
    echo "### Performance Snapshot" >> "$summary_file"
    echo "- Overall CPU utilization" >> "$summary_file"
    echo "- Memory usage patterns" >> "$summary_file"
    echo "- Threading efficiency" >> "$summary_file"
    echo "" >> "$summary_file"
    echo "### Hotspots" >> "$summary_file"
    echo "- Functions consuming most CPU time" >> "$summary_file"
    echo "- Call stack analysis" >> "$summary_file"
    echo "- Optimization opportunities" >> "$summary_file"
    echo "" >> "$summary_file"
    echo "### HPC Performance" >> "$summary_file"
    echo "- Parallel efficiency metrics" >> "$summary_file"
    echo "- Load balancing analysis" >> "$summary_file"
    echo "- Scaling bottlenecks" >> "$summary_file"
    
    echo "Summary report generated: $summary_file"
}

# Main execution
main() {
    # Check VTune availability
    check_vtune
    
    # Check if parallel binary exists
    if [[ ! -f "$PARALLEL_BIN" ]]; then
        echo "Error: Parallel binary not found. Please compile first."
        echo "Run: ./scripts/build/compile.sh parallel"
        exit 1
    fi
    
    # Find datasets to analyze
    local datasets=()
    if [[ -d "$DATA_DIR" ]]; then
        while IFS= read -r -d '' dataset; do
            datasets+=("$dataset")
        done < <(find "$DATA_DIR" -name "*.csv" -type f -print0)
    fi
    
    if [[ ${#datasets[@]} -eq 0 ]]; then
        echo "Warning: No datasets found in $DATA_DIR"
        echo "Please add datasets to analyze"
        exit 1
    fi
    
    # Analyze datasets (limit to first dataset for detailed profiling)
    local primary_dataset="${datasets[0]}"
    echo "Profiling primary dataset: $(basename "$primary_dataset")"
    analyze_dataset "$primary_dataset"
    
    # Generate summary
    generate_summary
    
    echo ""
    echo "VTune profiling completed!"
    echo "Results directory: $RESULTS_DIR"
    echo ""
    echo "To view detailed results:"
    echo "  vtune-gui $RESULTS_DIR/[result_directory]"
    echo ""
    echo "Or view text reports in: $RESULTS_DIR/*_report.txt"
}

# Run main function
main "$@"
