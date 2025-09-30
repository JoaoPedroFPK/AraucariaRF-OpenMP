import pandas as pd
import matplotlib.pyplot as plt
import os

def load_data(file_path):
    return pd.read_csv(file_path)

def main():
    performance_dir = "results/performance"
    csv_files = [os.path.join(performance_dir, f) for f in os.listdir(performance_dir) if f.endswith('.csv')]

    all_data = []
    for file in csv_files:
        df = load_data(file)
        all_data.append(df)

    thread_counts = [1, 2, 4, 8, 12, 16, 20, 24]
    
    # Create results/graphs directory if it doesn't exist
    output_dir = "results/graphs"
    os.makedirs(output_dir, exist_ok=True)
    
    # Colors for different datasets
    colors = ['blue', 'red', 'green', 'orange', 'purple', 'brown', 'pink', 'gray']
    
    # Create aggregated execution time plot
    plt.figure(figsize=(12, 8))
    
    for i, df in enumerate(all_data):
        dataset_name = os.path.basename(df['dataset'].iloc[0]).replace('.csv', '')
        
        # Group by threads and get the average time, handling missing thread counts
        grouped_data = df.groupby('threads')['time_seconds'].mean().reindex(thread_counts)
        available_data = grouped_data.dropna()
        
        plt.plot(available_data.index, available_data.values, 
                marker='o', linestyle='-', color=colors[i % len(colors)], 
                label=dataset_name, linewidth=2, markersize=6)
    
    plt.title('Execution Time vs. Number of Threads (All Datasets)', fontsize=16)
    plt.xlabel('Number of Threads', fontsize=12)
    plt.ylabel('Execution Time (seconds)', fontsize=12)
    plt.yscale('linear')
    plt.xticks(thread_counts)
    plt.grid(True, alpha=0.3)
    plt.legend(bbox_to_anchor=(1.05, 1), loc='upper left')
    plt.tight_layout()
    
    # Save the aggregated execution time plot
    plt.savefig(os.path.join(output_dir, 'all_datasets_execution_time.png'), dpi=300, bbox_inches='tight')
    plt.close()
    
    # Create aggregated speedup plot
    plt.figure(figsize=(12, 8))
    
    for i, df in enumerate(all_data):
        dataset_name = os.path.basename(df['dataset'].iloc[0]).replace('.csv', '')
        
        # Group by threads and get the average time, handling missing thread counts
        grouped_data = df.groupby('threads')['time_seconds'].mean().reindex(thread_counts)
        available_data = grouped_data.dropna()
        
        if len(available_data) > 0:
            # Try to use 1 thread as baseline, otherwise use the smallest thread count available
            if 1 in available_data.index:
                baseline_threads = 1
                baseline_time = available_data.loc[1]
                baseline_label = ""
            else:
                baseline_threads = available_data.index.min()
                baseline_time = available_data.loc[baseline_threads]
                baseline_label = f" (baseline: {baseline_threads} threads)"
            
            # Calculate speedup: baseline_time / t_n
            speedup = baseline_time / available_data
            
            plt.plot(speedup.index, speedup.values, 
                    marker='o', linestyle='-', color=colors[i % len(colors)], 
                    label=f'{dataset_name}{baseline_label}', linewidth=2, markersize=6)
    
    plt.title('Speedup vs. Number of Threads (All Datasets)', fontsize=16)
    plt.xlabel('Number of Threads', fontsize=12)
    plt.ylabel('Speedup', fontsize=12)
    plt.xticks(thread_counts)
    plt.grid(True, alpha=0.3)
    plt.legend(bbox_to_anchor=(1.05, 1), loc='upper left')
    plt.tight_layout()
    
    # Save the aggregated speedup plot
    plt.savefig(os.path.join(output_dir, 'all_datasets_speedup.png'), dpi=300, bbox_inches='tight')
    plt.close()

if __name__ == "__main__":
    main()