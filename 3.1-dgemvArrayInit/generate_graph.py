import matplotlib.pyplot as plt

threads = [1, 2, 4, 7, 8, 16, 20, 40]
s_20000 = [1.0, 4.37, 9.41, 9.21, 20.61, 18.82, 17.32, 48.11]
s_40000 = [1.0, 5.35, 10.32, 19.02, 21.43, 39.37, 42.32, 45.75]

plt.plot(threads, s_20000, label="Matrix 20000", marker='o')
plt.plot(threads, s_40000, label="Matrix 40000", marker='s')
plt.xlabel("Threads (P)")
plt.ylabel("Speedup (S)")
plt.title("Speedup vs Thread Count")
plt.grid(True)
plt.legend()

plt.savefig('speedup_plot.png', dpi=300, bbox_inches='tight')