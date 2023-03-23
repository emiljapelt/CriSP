import pandas as pd
import matplotlib.pyplot as plt
import sys
import os

directory = sys.argv[1]
file_list = os.listdir(directory + "/CONCURRENT_OUTPUT")
if not os.path.exists(directory + '/comparative_charts'):
    os.makedirs(directory + '/comparative_charts')

for file in file_list:
    if not file.__contains__(".csv"):
        continue

    metric = file[:-4]
    df1 = pd.read_csv(directory + "/CONCURRENT_OUTPUT/" + file)
    df2 = pd.read_csv(directory + "/COUNT_THEN_MOVE/" + file)


    # extract the desired column from each dataframe
    
    if file.__contains__("timing"):
        col1 = df1['32'].apply(lambda x: 16777216 / (x/1000))
        col2 = df2['32'].apply(lambda x: 16777216 / (x/1000))
    else:
        col1 = df1['32']
        col2 = df2['32']
    # plot the two columns using matplotlib
    plt.xlim(1, 18)
    plt.plot(col1, label='concurrent output', marker='o')
    plt.plot(col2, label='count then move', marker='o')
    plt.legend()
    plt.savefig(directory + "/comparative_charts/" + metric + ".png", bbox_inches='tight')
    plt.clf()
    # plt.show()