#include "random_forest.h"
#include <omp.h>

DecisionTree* create_decision_tree(int max_depth, int min_samples_split) {
    DecisionTree* tree = malloc(sizeof(DecisionTree));
    tree->capacity = 1000; // Initial capacity
    tree->nodes = malloc(tree->capacity * sizeof(TreeNode));
    tree->n_nodes = 0;
    return tree;
}

void free_decision_tree(DecisionTree* tree) {
    if (!tree) return;
    free(tree->nodes);
    free(tree);
}

double calculate_gini_impurity(int* labels, int n_samples) {
    if (n_samples == 0) return 0.0;
    
    // Count label frequencies
    int max_label = 0;
    for (int i = 0; i < n_samples; i++) {
        if (labels[i] > max_label) max_label = labels[i];
    }
    
    int* counts = calloc(max_label + 1, sizeof(int));
    for (int i = 0; i < n_samples; i++) {
        counts[labels[i]]++;
    }
    
    // Calculate Gini impurity
    double gini = 1.0;
    for (int i = 0; i <= max_label; i++) {
        if (counts[i] > 0) {
            double prob = (double)counts[i] / n_samples;
            gini -= prob * prob;
        }
    }
    
    free(counts);
    return gini;
}

int find_best_split(Dataset* data, int* indices, int n_samples, int* feature_indices, 
                   int n_features, int* best_feature, double* best_threshold) {
    
    if (n_samples < 2) return 0;
    
    double best_gini = 1.0;
    *best_feature = -1;
    *best_threshold = 0.0;
    
    // Calculate current gini impurity
    int* current_labels = malloc(n_samples * sizeof(int));
    for (int i = 0; i < n_samples; i++) {
        current_labels[i] = data->labels[indices[i]];
    }
    double current_gini = calculate_gini_impurity(current_labels, n_samples);
    
    // Try each feature
    for (int f = 0; f < n_features; f++) {
        int feature_idx = feature_indices[f];
        
        // Create array of feature values for current samples
        double* values = malloc(n_samples * sizeof(double));
        for (int i = 0; i < n_samples; i++) {
            values[i] = data->features[indices[i]][feature_idx];
        }
        
        // Sort values to find potential split points
        merge_sort(values, n_samples); // <-- paralelizar? não (vetores pequenos demais)
        
        // Try each potential split point
        #pragma omp parallel
        {
            double local_best_gini = 1.0;
            double local_best_threshold = 0.0;

            #pragma omp for nowait schedule(static)
            for (int i = 0; i < n_samples - 1; i++) { // <- paralelizar, sim!
                if (values[i] == values[i + 1]) continue; // Skip identical values
                
                double threshold = (values[i] + values[i + 1]) / 2.0;
                
                // Split samples
                int* left_labels = malloc(n_samples * sizeof(int));
                int* right_labels = malloc(n_samples * sizeof(int));
                int left_count = 0, right_count = 0;
                
                for (int j = 0; j < n_samples; j++) {
                    if (data->features[indices[j]][feature_idx] <= threshold) {
                        left_labels[left_count++] = data->labels[indices[j]];
                    } else {
                        right_labels[right_count++] = data->labels[indices[j]];
                    }
                }
                
                if (left_count == 0 || right_count == 0) {
                    free(left_labels);
                    free(right_labels);
                    continue;
                }
                
                // Calculate weighted gini impurity
                double left_gini = calculate_gini_impurity(left_labels, left_count);
                double right_gini = calculate_gini_impurity(right_labels, right_count);
                double weighted_gini = (left_count * left_gini + right_count * right_gini) / n_samples;
                
                if (weighted_gini < local_best_gini) {
                    local_best_gini = weighted_gini;
                    local_best_threshold = threshold;
                }
                
                free(left_labels);
                free(right_labels);
            }

            #pragma omp critical
            {
                if (local_best_gini < best_gini) {
                    best_gini = local_best_gini;
                    *best_feature = feature_idx;
                    *best_threshold = local_best_threshold;
                }
            }
        }
        
        free(values);
    }
    
    free(current_labels);
    
    // Return 1 if we found a valid split that improves gini
    return (*best_feature != -1 && best_gini < current_gini);
}

void build_tree_recursive(DecisionTree* tree, Dataset* data, int* indices, int n_samples,
                         int* feature_indices, int n_features, int depth, int max_depth, 
                         int min_samples_split, int node_idx) {
    
    // Expand tree capacity if needed
    if (node_idx >= tree->capacity) {
        tree->capacity *= 2;
        tree->nodes = realloc(tree->nodes, tree->capacity * sizeof(TreeNode));
    }
    
    if (node_idx >= tree->n_nodes) {
        tree->n_nodes = node_idx + 1;
    }
    
    TreeNode* node = &tree->nodes[node_idx];
    node->left_child = -1;
    node->right_child = -1;
    node->is_leaf = 0;
    
    // Create label array for current samples
    int* labels = malloc(n_samples * sizeof(int));
    for (int i = 0; i < n_samples; i++) {
        labels[i] = data->labels[indices[i]];
    }
    
    // Check stopping criteria
    if (depth >= max_depth || n_samples < min_samples_split || 
        calculate_gini_impurity(labels, n_samples) == 0.0) {
        
        // Create leaf node
        node->is_leaf = 1;
        node->prediction = get_majority_class(labels, n_samples);
        free(labels);
        return;
    }
    
    // Find best split
    int best_feature;
    double best_threshold;
    if (!find_best_split(data, indices, n_samples, feature_indices, n_features, 
                        &best_feature, &best_threshold)) {
        // No good split found, create leaf
        node->is_leaf = 1;
        node->prediction = get_majority_class(labels, n_samples);
        free(labels);
        return;
    }
    
    node->feature_index = best_feature;
    node->threshold = best_threshold;
    
    // Split samples
    int* left_indices = malloc(n_samples * sizeof(int));
    int* right_indices = malloc(n_samples * sizeof(int));
    int left_count = 0, right_count = 0;
    
    for (int i = 0; i < n_samples; i++) {
        if (data->features[indices[i]][best_feature] <= best_threshold) {
            left_indices[left_count++] = indices[i];
        } else {
            right_indices[right_count++] = indices[i];
        }
    }
    
    // Create child nodes
    int left_child_idx = tree->n_nodes;
    int right_child_idx = tree->n_nodes + 1;
    
    node->left_child = left_child_idx;
    node->right_child = right_child_idx;
    
    // Recursively build children
    build_tree_recursive(tree, data, left_indices, left_count, feature_indices, 
                        n_features, depth + 1, max_depth, min_samples_split, left_child_idx);
    
    build_tree_recursive(tree, data, right_indices, right_count, feature_indices, 
                        n_features, depth + 1, max_depth, min_samples_split, right_child_idx);
    
    free(labels);
    free(left_indices);
    free(right_indices);
}

void train_decision_tree(DecisionTree* tree, Dataset* data, int* feature_indices, int n_features) {
    // Create array of all sample indices
    int* all_indices = malloc(data->n_samples * sizeof(int));
    for (int i = 0; i < data->n_samples; i++) {
        all_indices[i] = i;
    }
    
    // Build tree starting from root
    build_tree_recursive(tree, data, all_indices, data->n_samples, feature_indices, 
                        n_features, 0, MAX_TREE_DEPTH, MIN_SAMPLES_SPLIT, 0);
    
    free(all_indices);
}

int predict_tree(DecisionTree* tree, double* sample) {
    if (tree->n_nodes == 0) return 0;
    
    int current_node = 0;
    
    while (current_node >= 0 && current_node < tree->n_nodes) {
        TreeNode* node = &tree->nodes[current_node];
        
        if (node->is_leaf) {
            return node->prediction;
        }
        
        if (sample[node->feature_index] <= node->threshold) {
            current_node = node->left_child;
        } else {
            current_node = node->right_child;
        }
    }
    
    return 0; // Default prediction if something goes wrong
}
