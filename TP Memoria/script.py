path = "/home/lucaspujia/Escritorio/Facultad/SSOO/TP Memoria/errorsC.txt"

ptrs = []
allocs = []

try:
    with open(path, "r", encoding="utf-8") as file:
        for line in file:
            ptrs.append(int(line.rstrip().split("ptr[")[1].split("]")[0]))
            allocs.append(int(line.rstrip().split("Alloc(")[1].split(")")[0]))
            
except FileNotFoundError:
    print("No se encontró el archivo:", path)

ptrs_100 = [0,0,0,0,0, 0]
for i in range(len(ptrs)):
    index = (ptrs[i] // 100)
    if 0 <= index < 6:
        ptrs_100[index] += 1

print(ptrs_100)

allocs_10 = [0,0,0,0,0,0,0,0,0,0,0]
for i in range(len(allocs)):
    allocs_10[allocs[i]] += 1

print(allocs_10)