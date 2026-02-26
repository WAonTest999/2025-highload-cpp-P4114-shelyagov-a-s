import matplotlib.pyplot as plt

file = open("./results/floats_stupid.txt", "r")
float_stupid = list(map(int, file.read().split('\n')[:-1]))
file.close()

file = open("./results/floats_simd.txt", "r")
float_simd = list(map(int, file.read().split('\n')[:-1]))
file.close()

file = open("./results/floats_thread.txt", "r")
float_thread = list(map(int, file.read().split('\n')[:-1]))
file.close()

file = open("./results/floats_thread2.txt", "r")
float_thread2 = list(map(int, file.read().split('\n')[:-1]))
file.close()

plt.figure()

x = range(1, len(float_stupid) + 1)
y1 = float_stupid
y2 = float_simd
y3 = float_thread
y4 = float_thread2

plt.plot(x, y1, linestyle='-', label='stupid', linewidth = 1)
plt.plot(x, y2, linestyle='-', label='simd', linewidth = 1)
plt.plot(x, y3, linestyle='-', label='pure thread', linewidth = 1)
plt.plot(x, y4, linestyle='-', label='thread + prepearing', linewidth = 1)

plt.xlabel('Количество вкодных данных, шт')
plt.ylabel('Время, мкс')

plt.title('Фильтрация float-pointer значений, больших заданного')
plt.legend()
plt.grid(True)

plt.savefig('img/floats.png', dpi=300, bbox_inches='tight')

plt.close()


