path = "/home/lucaspujia/Escritorio/Facultad/SSOO/TP Memoria/testC.txt"

free_list = []

try:
    with open(path, "r", encoding="utf-8") as file:
        for line in file:
            if ("Free List" in line):
                free_list.append(line.rstrip())
            
except FileNotFoundError:
    print("No se encontró el archivo:", path)

size = 0
count = 0
for entry in free_list:
    if ("Size" in entry): 
        # print(entry)
        size += int(entry.split("Size ")[1].split(" ]: ")[0])
        count += 1
print("Average Free List Size:", size / count)

max_free_list = 0
max_index = 0
for index, entry in enumerate(free_list):
    fr_lst: list = entry.split("]: ")[1]
    count = fr_lst.count("[")
    if count > max_free_list:
        # print(index, count)
        # print(entry)
        max_free_list = count
        max_index = index

# print("Max Free List Length:", max_free_list, max_index)