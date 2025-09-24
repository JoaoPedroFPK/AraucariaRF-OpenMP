#include "random_forest.h"

void print_usage(const char* program_name) {
    printf("Usage: %s <dataset_path> [options]\n", program_name);
    printf("Options:\n");
    printf("  -t <num_trees>     Number of trees (default: 100)\n");
    printf("  -d <max_depth>     Maximum tree depth (default: 10)\n");
    printf("  -s <min_samples>   Minimum samples to split (default: 2)\n");
    printf("  -f <num_features>  Features per tree (default: sqrt(total_features))\n");
    printf("  -r <train_ratio>   Training set ratio (default: 0.8)\n");
    printf("  -h                 Show this help\n");
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        print_usage(argv[0]);
        return 1;
    }
    
    // Default parameters
    char* dataset_path = argv[1];
    int n_trees = DEFAULT_N_TREES;
    int max_depth = MAX_TREE_DEPTH;
    int min_samples_split = MIN_SAMPLES_SPLIT;
    int n_features_per_tree = -1; // Will be calculated as sqrt(total_features)
    double train_ratio = 0.8;
    
    // Parse command line arguments
    for (int i = 2; i < argc; i++) {
        if (strcmp(argv[i], "-t") == 0 && i + 1 < argc) {
            n_trees = atoi(argv[++i]);
        } else if (strcmp(argv[i], "-d") == 0 && i + 1 < argc) {
            max_depth = atoi(argv[++i]);
        } else if (strcmp(argv[i], "-s") == 0 && i + 1 < argc) {
            min_samples_split = atoi(argv[++i]);
        } else if (strcmp(argv[i], "-f") == 0 && i + 1 < argc) {
            n_features_per_tree = atoi(argv[++i]);
        } else if (strcmp(argv[i], "-r") == 0 && i + 1 < argc) {
            train_ratio = atof(argv[++i]);
        } else if (strcmp(argv[i], "-h") == 0) {
            print_usage(argv[0]);
            return 0;
        }
    }
    
    // Initialize random seed
    srand(time(NULL));
    
    printf("=== Sequential Random Forest Implementation ===\n");
    printf("Dataset: %s\n", dataset_path);
    printf("Parameters:\n");
    printf("  Trees: %d\n", n_trees);
    printf("  Max depth: %d\n", max_depth);
    printf("  Min samples split: %d\n", min_samples_split);
    printf("  Train ratio: %.2f\n", train_ratio);
    printf("---\n");
    
    // Load dataset
    struct timeval start_time, end_time;
    gettimeofday(&start_time, NULL);
    
    Dataset* dataset = load_dataset(dataset_path);
    if (!dataset) {
        fprintf(stderr, "Failed to load dataset\n");
        return 1;
    }
    
    print_dataset_info(dataset);
    
    // Shuffle dataset for random train/test split
    shuffle_dataset(dataset);
    
    // Split into training and testing sets
    int train_size = (int)(dataset->n_samples * train_ratio);
    int test_size = dataset->n_samples - train_size;
    
    printf("Train/Test split: %d/%d samples\n", train_size, test_size);
    
    // Create training dataset
    Dataset* train_data = malloc(sizeof(Dataset));
    train_data->n_samples = train_size;
    train_data->n_features = dataset->n_features;
    train_data->features = dataset->features; // Point to first part
    train_data->labels = dataset->labels;
    
    // Create test dataset
    Dataset* test_data = malloc(sizeof(Dataset));
    test_data->n_samples = test_size;
    test_data->n_features = dataset->n_features;
    test_data->features = &dataset->features[train_size]; // Point to second part
    test_data->labels = &dataset->labels[train_size];
    
    // Calculate features per tree if not specified
    if (n_features_per_tree <= 0) {
        n_features_per_tree = (int)sqrt(dataset->n_features);
        if (n_features_per_tree < 1) n_features_per_tree = 1;
    }
    printf("  Features per tree: %d\n", n_features_per_tree);
    printf("---\n");
    
    // Create and train Random Forest
    gettimeofday(&start_time, NULL);
    
    RandomForest* rf = create_random_forest(n_trees, max_depth, min_samples_split, n_features_per_tree);
    train_random_forest(rf, train_data);
    
    gettimeofday(&end_time, NULL);
    double training_time = get_time_diff(start_time, end_time);
    
    printf("Training completed in %.4f seconds\n", training_time);
    printf("---\n");
    
    // Evaluate model
    gettimeofday(&start_time, NULL);
    
    double accuracy = evaluate_accuracy(rf, test_data);
    
    gettimeofday(&end_time, NULL);
    double prediction_time = get_time_diff(start_time, end_time);
    
    printf("Prediction completed in %.4f seconds\n", prediction_time);
    printf("---\n");
    
    // Print final results in CSV format for benchmarking
    printf("RESULT,%s,1,1,%.4f,%.4f\n", 
           dataset_path, training_time + prediction_time, accuracy);
    
    // Performance metrics
    PerformanceMetrics metrics;
    metrics.execution_time = training_time + prediction_time;
    metrics.accuracy = accuracy;
    metrics.n_trees_used = n_trees;
    metrics.n_threads_used = 1;
    
    print_performance_metrics(&metrics, dataset_path);
    
    // Cleanup
    free_random_forest(rf);
    
    // Only free the wrapper structs, not the shared data
    free(train_data);
    free(test_data);
    
    // Free the original dataset which owns all the memory
    free_dataset(dataset);
    
    return 0;
}
