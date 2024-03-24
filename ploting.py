import csv
import matplotlib.pyplot as plt

# 打开 CSV 文件并读取指定列的数据
with open('30651214.csv', 'r') as file:
    reader = csv.reader(file)
    
    #next(reader)  // 跳过标题行
    column_data1 = [float(row[1]) for row in reader]
    file.seek(0)
    reader = csv.reader(file)
    column_data2 = [float(row[2]) for row in reader]

# 绘制图表
plt.plot(column_data1,label="ff1")
plt.plot(column_data2,label="ff2")
plt.xlabel('time')
plt.ylabel('fuel flow/pph')
plt.legend()
plt.show()