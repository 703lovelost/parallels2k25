import numpy as np
import matplotlib.pyplot as plt

def load_matrix(filename):
    return np.loadtxt(filename, delimiter=',')

if __name__ == '__main__':
    matrix = load_matrix('matrix_10x10.csv')
    plt.imshow(matrix)
    plt.colorbar()
    plt.title('Heatmap of 10×10 Matrix')
    plt.xlabel('Column Index')
    plt.ylabel('Row Index')

    output_path = 'matrix_10x10_heatmap.png'
    plt.savefig(output_path)
    print(f"Heatmap saved to {output_path}")
