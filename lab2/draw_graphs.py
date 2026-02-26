import matplotlib.pyplot as plt

file = open("./results/floats_stupid.txt", "r")
float_stupid = list(map(int, file.read().split('\n')[:-1]))
file.close()

file = open("./results/floats_simd.txt", "r")
float_simd = list(map(int, file.read().split('\n')[:-1]))
file.close()

file = open("./results/img_stupid.txt", "r")
img_stupid = list(map(int, file.read().split('\n')[:-1]))
file.close()

file = open("./results/img_simd.txt", "r")
img_simd = list(map(int, file.read().split('\n')[:-1]))
file.close()


file = open("./results/sequence_stupid.txt", "r")
sequence_stupid = list(map(int, file.read().split('\n')[:-1]))
file.close()

file = open("./results/sequence_simd.txt", "r")
sequence_simd = list(map(int, file.read().split('\n')[:-1]))
file.close()



plt.figure()

x = range(1, len(float_stupid) + 1)
y1 = float_stupid
y2 = float_simd

plt.plot(x, y1, linestyle='-', label='stupid', linewidth = 1)
plt.plot(x, y2, linestyle='-', label='simd', linewidth = 1)

plt.xlabel('Количество вкодных данных, шт')
plt.ylabel('Время, мкс')

plt.title('Фильтрация float-pointer значений, больших заданного')
plt.legend()
plt.grid(True)

plt.savefig('img/floats.png', dpi=300, bbox_inches='tight')

plt.close()


plt.figure()

x = range(1, len(img_stupid) + 1)
y1 = img_stupid
y2 = img_simd

plt.plot(x, y1, linestyle='-', label='stupid', linewidth = 1)
plt.plot(x, y2, linestyle='-', label='simd', linewidth = 1)

plt.xlabel('Количество вкодных данных, шт')
plt.ylabel('Время, мкс')

plt.title('Нахождение абсолютной разности двух картинок')
plt.legend()
plt.grid(True)

plt.savefig('img/img.png', dpi=300, bbox_inches='tight')

plt.close()

plt.figure()

x = range(1, len(sequence_stupid) + 1)
y1 = sequence_stupid
y2 = sequence_simd

plt.plot(x, y1, linestyle='-', label='stupid', linewidth = 1)
plt.plot(x, y2, linestyle='-', label='simd', linewidth = 1)

plt.xlabel('Длинна скобочной последовательности')
plt.ylabel('Время, мкс')

plt.title('Определение валидности скобочной последовательности')
plt.legend()
plt.grid(True)

plt.savefig('img/sequence.png', dpi=300, bbox_inches='tight')

plt.close()


