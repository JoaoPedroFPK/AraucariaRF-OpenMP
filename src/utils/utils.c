#include "random_forest.h"

double get_time_diff(struct timeval start, struct timeval end) {
    return (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000000.0;
}

void print_performance_metrics(PerformanceMetrics* metrics, const char* dataset_name) {
    printf("Performance Results for %s:\n", dataset_name);
    printf("  Execution Time: %.4f seconds\n", metrics->execution_time);
    printf("  Accuracy: %.2f%%\n", metrics->accuracy * 100.0);
    printf("  Trees Used: %d\n", metrics->n_trees_used);
    printf("  Threads Used: %d\n", metrics->n_threads_used);
    printf("---\n");
}

int* generate_random_features(int n_total_features, int n_selected_features) {
    if (n_selected_features > n_total_features) {
        n_selected_features = n_total_features;
    }
    
    int* selected_features = malloc(n_selected_features * sizeof(int));
    int* available_features = malloc(n_total_features * sizeof(int));
    
    // Initialize available features
    for (int i = 0; i < n_total_features; i++) {
        available_features[i] = i;
    }
    
    // Randomly select features without replacement
    for (int i = 0; i < n_selected_features; i++) {
        int random_idx = rand() % (n_total_features - i);
        selected_features[i] = available_features[random_idx];
        
        // Move selected feature to end and reduce available count
        available_features[random_idx] = available_features[n_total_features - i - 1];
    }
    
    free(available_features);
    return selected_features;
}

int get_majority_class(int* predictions, int n_predictions) {
    if (n_predictions == 0) return 0;
    
    // Find maximum label to allocate counts array
    int max_label = 0;
    for (int i = 0; i < n_predictions; i++) {
        if (predictions[i] > max_label) {
            max_label = predictions[i];
        }
    }
    
    int* counts = calloc(max_label + 1, sizeof(int));
    
    // Count occurrences
    for (int i = 0; i < n_predictions; i++) {
        counts[predictions[i]]++;
    }
    
    // Find majority class
    int majority_class = 0;
    int max_count = counts[0];
    
    for (int i = 1; i <= max_label; i++) {
        if (counts[i] > max_count) {
            max_count = counts[i];
            majority_class = i;
        }
    }
    
    free(counts);
    return majority_class;
}
