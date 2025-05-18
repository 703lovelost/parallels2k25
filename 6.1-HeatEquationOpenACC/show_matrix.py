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
    plt.show()
