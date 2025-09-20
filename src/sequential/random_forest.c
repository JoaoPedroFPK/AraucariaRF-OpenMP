#include "random_forest.h"

RandomForest* create_random_forest(int n_trees, int max_depth, int min_samples_split, int n_features_per_tree) {
    RandomForest* rf = malloc(sizeof(RandomForest));
    rf->n_trees = n_trees;
    rf->max_depth = max_depth;
    rf->min_samples_split = min_samples_split;
    rf->n_features_per_tree = n_features_per_tree;
    
    rf->trees = malloc(n_trees * sizeof(DecisionTree));
    
    for (int i = 0; i < n_trees; i++) {
        rf->trees[i].nodes = NULL;
        rf->trees[i].n_nodes = 0;
        rf->trees[i].capacity = 0;
    }
    
    return rf;
}

void free_random_forest(RandomForest* rf) {
    if (!rf) return;
    
    if (rf->trees) {
        for (int i = 0; i < rf->n_trees; i++) {
            if (rf->trees[i].nodes) {
                free(rf->trees[i].nodes);
            }
        }
        free(rf->trees);
    }
    
    free(rf);
}

void train_random_forest(RandomForest* rf, Dataset* training_data) {
    printf("Training Random Forest with %d trees...\n", rf->n_trees);
    
    // Calculate number of features per tree if not set
    if (rf->n_features_per_tree <= 0) {
        rf->n_features_per_tree = (int)sqrt(training_data->n_features);
        if (rf->n_features_per_tree < 1) rf->n_features_per_tree = 1;
    }
    
    printf("Using %d features per tree\n", rf->n_features_per_tree);
    
    for (int tree_idx = 0; tree_idx < rf->n_trees; tree_idx++) {
        if (tree_idx % 10 == 0) {
            printf("Training tree %d/%d\n", tree_idx + 1, rf->n_trees);
        }
        
        // Create bootstrap sample
        Dataset* bootstrap_data = bootstrap_sample(training_data, training_data->n_samples);
        
        // Select random features for this tree
        int* feature_indices = generate_random_features(training_data->n_features, rf->n_features_per_tree);
        
        // Initialize decision tree
        DecisionTree* tree = &rf->trees[tree_idx];
        tree->capacity = 1000;
        tree->nodes = malloc(tree->capacity * sizeof(TreeNode));
        tree->n_nodes = 0;
        
        // Train the tree
        train_decision_tree(tree, bootstrap_data, feature_indices, rf->n_features_per_tree);
        
        // Clean up
        free_dataset(bootstrap_data);
        free(feature_indices);
    }
    
    printf("Random Forest training completed!\n");
}

int predict_random_forest(RandomForest* rf, double* sample) {
    int* predictions = malloc(rf->n_trees * sizeof(int));
    
    // Get prediction from each tree
    for (int i = 0; i < rf->n_trees; i++) {
        predictions[i] = predict_tree(&rf->trees[i], sample);
    }
    
    // Return majority vote
    int majority_prediction = get_majority_class(predictions, rf->n_trees);
    
    free(predictions);
    return majority_prediction;
}

double evaluate_accuracy(RandomForest* rf, Dataset* test_data) {
    int correct_predictions = 0;
    
    printf("Evaluating accuracy on %d samples...\n", test_data->n_samples);
    
    for (int i = 0; i < test_data->n_samples; i++) {
        int prediction = predict_random_forest(rf, test_data->features[i]);
        if (prediction == test_data->labels[i]) {
            correct_predictions++;
        }
        
        // Progress indicator for large datasets
        if (test_data->n_samples > 1000 && i % (test_data->n_samples / 10) == 0) {
            printf("  Evaluated %d/%d samples\n", i, test_data->n_samples);
        }
    }
    
    double accuracy = (double)correct_predictions / test_data->n_samples;
    printf("Accuracy: %.2f%% (%d/%d correct)\n", 
           accuracy * 100.0, correct_predictions, test_data->n_samples);
    
    return accuracy;
}
