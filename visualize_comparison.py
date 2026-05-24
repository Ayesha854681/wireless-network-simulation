"""
Network Comparison Visualization Script
Shows before and after XTC side-by-side with paths
"""
import matplotlib.pyplot as plt
import json
import sys

def load_comparison_data(filename):
    """Load comparison data from JSON file"""
    with open(filename, 'r') as f:
        return json.load(f)

def visualize_comparison(data):
    """Create side-by-side comparison visualization"""
    node_x_positions = data['nodes_x']
    node_y_positions = data['nodes_y']
    edges_before_xtc = data['edges_before_xtc']
    edges_after_xtc = data['edges_after_xtc']
    path_before_xtc = data.get('path_before_xtc', [])
    path_after_xtc = data.get('path_after_xtc', [])
    title = data.get('title', 'Network Comparison')
    grid_size = data.get('grid_size', 7.0)
    
    fig, axes = plt.subplots(1, 2, figsize=(14, 6))
    fig.patch.set_facecolor('#111111')
    axes[0].set_title(f'Before: {title}', color='white')
    axes[1].set_title(f'After: {title}', color='white')
    
    for ax in axes:
        ax.set_facecolor('black')
        ax.set_xlim(0, grid_size)
        ax.set_ylim(0, grid_size)
        ax.axis('off')
    
    # Draw edges before XTC
    for i, j in edges_before_xtc:
        axes[0].plot([node_x_positions[i], node_x_positions[j]], 
                    [node_y_positions[i], node_y_positions[j]],
                    color='#555555', linewidth=0.5, alpha=0.35)
    
    # Draw edges after XTC
    for i, j in edges_after_xtc:
        axes[1].plot([node_x_positions[i], node_x_positions[j]], 
                    [node_y_positions[i], node_y_positions[j]],
                    color='#555555', linewidth=0.5, alpha=0.35)
    
    # Draw nodes on both plots
    axes[0].scatter(node_x_positions, node_y_positions, c='red', s=10, zorder=5)
    axes[1].scatter(node_x_positions, node_y_positions, c='red', s=10, zorder=5)
    
    def draw_path(ax, path, color, z=10):
        if not path:
            return
        xs = [node_x_positions[i] for i in path]
        ys = [node_y_positions[i] for i in path]
        ax.plot(xs, ys, color=color, linewidth=2.2, zorder=z)
        ax.scatter(xs, ys, color=color, s=30, zorder=z+1)
    
    # Draw paths
    draw_path(axes[0], path_before_xtc, 'yellow')
    draw_path(axes[1], path_after_xtc, 'cyan')
    
    plt.tight_layout()
    plt.show()

if __name__ == '__main__':
    if len(sys.argv) < 2:
        print("Usage: python visualize_comparison.py <data_file.json>")
        sys.exit(1)
    
    filename = sys.argv[1]
    data = load_comparison_data(filename)
    visualize_comparison(data)
