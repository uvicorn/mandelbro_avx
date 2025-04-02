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


delta = []
for i in range(len(avx_data)):
    xtimes = Decimal(slow_data[i]) / Decimal(avx_data[i])
    delta.append(xtimes)

print('\n\n')
print("delta array:", ', '.join("%.3f" % a for a in delta))

delta.sort()

print("Median: ", delta[len(delta)//2])
print("Avg: ", sum(delta) / len(delta))

plt.figure(figsize=(15, 10))

plt.plot(xs, avx_data, 'r', label='AVX2')
plt.plot(xs, slow_data, 'b', label='Slow')

plt.xlabel('scale = (1/2)^x')
plt.ylabel('CPU clocks cycles')

plt.legend(loc='upper center')
plt.grid()

plt.title("100 random benchmarks with increasing scale")

plt.savefig('img/graph2.png')
