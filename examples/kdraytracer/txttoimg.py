import numpy as np
from PIL import Image

f = open("colors.txt", "r")

a = f.readline()
a = a.split(" ")
a = [int(a[0]), int(a[1])]

arr = np.zeros((a[0], a[1], 3),dtype='uint8')


i = -1
for l in f:
    if(i == -1) :
        i += 1
        continue
    b = l.strip().split(" ")
    for j in range(a[1]):
        arr[i][j][0] = float(b[4*j])
        arr[i][j][1] = float(b[4*j + 1])
        arr[i][j][2] = float(b[4*j+2])

    i += 1


print("done")
img = Image.fromarray(arr, 'RGB')
img.save("temp.jpg")


    
