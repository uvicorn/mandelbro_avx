import matplotlib.pyplot as plt
import sys
import matplotlib
from decimal import Decimal

# set font size
matplotlib.rcParams.update({'font.size': 16})

def read_data(filename):
    with open(filename, 'rb') as f:
        data = f.read()

    ys = []
    # clocks
    for i in range(0, len(data), 8):
        clock = (int.from_bytes(data[i:i+8], 'little'))
        ys.append(clock)
    return ys


avx_data = read_data("benchmarks/avx.bin")
slow_data = read_data("benchmarks/slow.bin")

assert len(avx_data) == len(slow_data)

print("AVX: ", avx_data)
print("Slow: ", slow_data)

avx_ys = [avx_data[0]]
slow_ys = [slow_data[0]]

for i in range(1, len(avx_data)):
    avx_ys.append(avx_data[i] + avx_ys[i-1])
    slow_ys.append(slow_data[i] + slow_ys[i-1])

xs = [i for i in range(len(avx_data))]

plt.figure(figsize=(15, 10))

# plt.stem(xs, avx_data, color='b')
# plt.bar(list(range(1, len(avx_data), 2)), avx_data, 1/(2*len(avx_data)), color="b")
# plt.bar(list(range(2, len(avx_data), 2)), slow_data, 1/(2*len(avx_data)), color="r")
# plt.bar([1, 2, 3], [1, 2, 3], 0.5, label="label 1", color="r")

plt.plot(xs, avx_data, 'r', label='AVX2')
plt.plot(xs, slow_data, 'b', label='Slow')

plt.xlabel('test number')
plt.ylabel('clocks')

plt.legend(loc='upper center')
plt.grid()

plt.title("100 random benchmarks")

plt.savefig('img/graph1.png')
