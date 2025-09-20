#include "random_forest.h"

Dataset* load_dataset(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Error: Cannot open file %s\n", filename);
        return NULL;
    }
    
    Dataset* dataset = malloc(sizeof(Dataset));
    if (!dataset) {
        fprintf(stderr, "Error: Memory allocation failed\n");
        fclose(file);
        return NULL;
    }
    
    // First pass: count lines and features
    char line[4096];
    int n_samples = 0;
    int n_features = 0;
    
    // Skip header line and count features
    if (fgets(line, sizeof(line), file)) {
        char* token = strtok(line, ",");
        while (token) {
            n_features++;
            token = strtok(NULL, ",");
        }
        n_features--; // Last column is label
    }
    
    // Count data lines
    while (fgets(line, sizeof(line), file)) {
        n_samples++;
    }
    
    dataset->n_samples = n_samples;
    dataset->n_features = n_features;
    
    // Allocate memory
    dataset->features = malloc(n_samples * sizeof(double*));
    dataset->labels = malloc(n_samples * sizeof(int));
    
    for (int i = 0; i < n_samples; i++) {
        dataset->features[i] = malloc(n_features * sizeof(double));
    }
    
    // Second pass: read data
    rewind(file);
    fgets(line, sizeof(line), file); // Skip header
    
    int sample_idx = 0;
    while (fgets(line, sizeof(line), file) && sample_idx < n_samples) {
        char* token = strtok(line, ",");
        int feature_idx = 0;
        
        // Read features
        while (token && feature_idx < n_features) {
            dataset->features[sample_idx][feature_idx] = atof(token);
            token = strtok(NULL, ",");
            feature_idx++;
        }
        
        // Read label (last column)
        if (token) {
            dataset->labels[sample_idx] = atoi(token);
        }
        
        sample_idx++;
    }
    
    fclose(file);
    printf("Loaded dataset: %d samples, %d features\n", n_samples, n_features);
    return dataset;
}

void free_dataset(Dataset* dataset) {
    if (!dataset) return;
    
    if (dataset->features) {
        for (int i = 0; i < dataset->n_samples; i++) {
            free(dataset->features[i]);
        }
        free(dataset->features);
    }
    
    free(dataset->labels);
    free(dataset);
}

void shuffle_dataset(Dataset* dataset) {
    for (int i = dataset->n_samples - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        
        // Swap features
        double* temp_features = dataset->features[i];
        dataset->features[i] = dataset->features[j];
        dataset->features[j] = temp_features;
        
        // Swap labels
        int temp_label = dataset->labels[i];
        dataset->labels[i] = dataset->labels[j];
        dataset->labels[j] = temp_label;
    }
}

Dataset* bootstrap_sample(Dataset* original, int sample_size) {
    Dataset* sample = malloc(sizeof(Dataset));
    sample->n_samples = sample_size;
    sample->n_features = original->n_features;
    
    sample->features = malloc(sample_size * sizeof(double*));
    sample->labels = malloc(sample_size * sizeof(int));
    
    for (int i = 0; i < sample_size; i++) {
        sample->features[i] = malloc(original->n_features * sizeof(double));
        
        // Random sampling with replacement
        int random_idx = rand() % original->n_samples;
        
        for (int j = 0; j < original->n_features; j++) {
            sample->features[i][j] = original->features[random_idx][j];
        }
        sample->labels[i] = original->labels[random_idx];
    }
    
    return sample;
}

void print_dataset_info(Dataset* dataset) {
    if (!dataset) {
        printf("Dataset is NULL\n");
        return;
    }
    
    printf("Dataset Information:\n");
    printf("  Samples: %d\n", dataset->n_samples);
    printf("  Features: %d\n", dataset->n_features);
    
    // Count unique labels
    int max_label = 0;
    for (int i = 0; i < dataset->n_samples; i++) {
        if (dataset->labels[i] > max_label) {
            max_label = dataset->labels[i];
        }
    }
    
    int* label_counts = calloc(max_label + 1, sizeof(int));
    for (int i = 0; i < dataset->n_samples; i++) {
        label_counts[dataset->labels[i]]++;
    }
    
    printf("  Label distribution:\n");
    for (int i = 0; i <= max_label; i++) {
        if (label_counts[i] > 0) {
            printf("    Class %d: %d samples (%.1f%%)\n", 
                   i, label_counts[i], 
                   100.0 * label_counts[i] / dataset->n_samples);
        }
    }
    
    free(label_counts);
}
