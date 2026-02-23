import matplotlib.pyplot as plt

file = open("./results/insertion_same.txt", "r")
insert_same = list(map(int, file.read().split('\n')[:-1]))
file.close()

file = open("./results/deletion_same.txt", "r")
delete_same = list(map(int, file.read().split('\n')[:-1]))
file.close()

file = open("./results/find_same.txt", "r")
find_same = list(map(int, file.read().split('\n')[:-1]))
file.close()


file = open("./results/insertion_rand.txt", "r")
insert_rand = list(map(int, file.read().split('\n')[:-1]))
file.close()

file = open("./results/deletion_rand.txt", "r")
delete_rand = list(map(int, file.read().split('\n')[:-1]))
file.close()

file = open("./results/find_rand.txt", "r")
find_rand = list(map(int, file.read().split('\n')[:-1]))
file.close()



x = range(1, len(insert_same) + 1)
y1 = insert_same
y2 = delete_same
y3 = find_same

y4 = insert_rand
y5 = delete_rand
y6 = find_rand



plt.plot(x, y1, linestyle='-', label='insert same hash', linewidth = 1)
plt.plot(x, y2, linestyle='-', label='delete same hash', linewidth = 1)
plt.plot(x, y3, linestyle='-', label='find same hash', linewidth = 1)

plt.plot(x, y4, linestyle='-', label='insert rand hash', linewidth = 1)
plt.plot(x, y5, linestyle='-', label='delete rand hash', linewidth = 1)
plt.plot(x, y6, linestyle='-', label='find rand hash', linewidth = 1)



plt.xlabel('Количество эллементов, шт')
plt.ylabel('Время, мкс')

plt.title('График с тремя наборами данных')
plt.legend()
plt.grid(True)

plt.savefig('benchmark.png', dpi=300, bbox_inches='tight')

