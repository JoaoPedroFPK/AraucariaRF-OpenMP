# Raw Datasets

This directory contains the original datasets used for testing the Random Forest implementation.

## Dataset Requirements

For comprehensive testing, include datasets with varying characteristics:

### Small Dataset (< 1MB)
- Number of samples: 1,000 - 10,000
- Features: 10-50
- Purpose: Quick testing and debugging

### Medium Dataset (1-50MB)
- Number of samples: 10,000 - 100,000
- Features: 50-200
- Purpose: Performance scaling analysis

### Large Dataset (> 50MB)
- Number of samples: 100,000+
- Features: 200+
- Purpose: Stress testing and maximum performance evaluation

## Suggested Datasets

1. **UCI Machine Learning Repository datasets**
2. **Kaggle competition datasets**
3. **Synthetic datasets** (can be generated using scripts)

## Data Format

Datasets should be in CSV format with:
- First row: column headers
- Last column: target variable (class labels for classification)
- Missing values: handled by preprocessing scripts

## Usage

Place your datasets in this directory and update the dataset paths in the configuration files.
