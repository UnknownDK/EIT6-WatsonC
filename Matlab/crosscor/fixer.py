import csv

filename = "inSignal"
b = []

with open(filename+'.txt') as f:
    lines = f.readlines()

del lines[0]
del lines[-1]


for line in lines:
    line = line.replace('\n',"").split(' ')
    print(line)
    #print(line)
    for i in range(8):
        b.append('0x' + line[i])
    #b.append('0x' + line[0] + line[1])
    #b.append('0x' + line[2] + line[3])
    #b.append('0x' + line[4] + line[5])
    #b.append('0x' + line[6] + line[7])

with open(filename+'.csv', 'w') as file:
    writer = csv.writer(file)
    writer.writerow(b)