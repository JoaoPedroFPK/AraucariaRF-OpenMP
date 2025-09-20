#ifndef RANDOM_FOREST_H
#define RANDOM_FOREST_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <sys/time.h>

// Data structures
typedef struct {
    double **features;
    int *labels;
    int n_samples;
    int n_features;
} Dataset;

typedef struct {
    int feature_index;
    double threshold;
    int left_child;
    int right_child;
    int prediction;  // For leaf nodes
    int is_leaf;
} TreeNode;

typedef struct {
    TreeNode *nodes;
    int n_nodes;
    int capacity;
} DecisionTree;

typedef struct {
    DecisionTree *trees;
    int n_trees;
    int max_depth;
    int min_samples_split;
    int n_features_per_tree;
} RandomForest;

typedef struct {
    double execution_time;
    double accuracy;
    int n_trees_used;
    int n_threads_used;
} PerformanceMetrics;

// Function declarations

// Dataset operations
Dataset* load_dataset(const char* filename);
void free_dataset(Dataset* dataset);
void shuffle_dataset(Dataset* dataset);
Dataset* bootstrap_sample(Dataset* original, int sample_size);
void print_dataset_info(Dataset* dataset);

// Decision Tree operations
DecisionTree* create_decision_tree(int max_depth, int min_samples_split);
void free_decision_tree(DecisionTree* tree);
void train_decision_tree(DecisionTree* tree, Dataset* data, int* feature_indices, int n_features);
int predict_tree(DecisionTree* tree, double* sample);
double calculate_gini_impurity(int* labels, int n_samples);
int find_best_split(Dataset* data, int* indices, int n_samples, int* feature_indices, 
                   int n_features, int* best_feature, double* best_threshold);

// Random Forest operations
RandomForest* create_random_forest(int n_trees, int max_depth, int min_samples_split, int n_features_per_tree);
void free_random_forest(RandomForest* rf);
void train_random_forest(RandomForest* rf, Dataset* training_data);
int predict_random_forest(RandomForest* rf, double* sample);
double evaluate_accuracy(RandomForest* rf, Dataset* test_data);

// Utility functions
double get_time_diff(struct timeval start, struct timeval end);
void print_performance_metrics(PerformanceMetrics* metrics, const char* dataset_name);
int* generate_random_features(int n_total_features, int n_selected_features);
int get_majority_class(int* predictions, int n_predictions);

// Constants
#define MAX_TREE_DEPTH 10
#define MIN_SAMPLES_SPLIT 2
#define DEFAULT_N_TREES 100
#define DEFAULT_N_FEATURES_RATIO 0.33  // sqrt(n_features) / n_features approximation

#endif // RANDOM_FOREST_H
