import pandas as pd
import matplotlib.pyplot as plt
import matplotlib.ticker as ticker
import sys
import os

directory = sys.argv[1]
file_list = os.listdir(directory)

if not os.path.exists(directory + '/charts'):
    os.makedirs(directory + '/charts')

for file in file_list:
    if not os.path.isfile(directory + "/" + file):
        continue
    
    metric = file[:-4]
    df = pd.read_csv(directory + "/" + file, names=['Category', 1, 2, 4, 8, 16, 32], skiprows=1)

    if file.__contains__("timing"):
        df.iloc[:, 1:] = 16777216 / (df.iloc[:, 1:]/1000)

    ax = df.plot(kind='line', x='Category', y=[1, 2, 4, 8, 16, 32], marker='o', legend=True)
    plt.legend(title="Threads")
    
    if file.__contains__("timing"):
        ax.set_ylabel('Million tuples per second')
        formatter = ticker.FuncFormatter(lambda x, pos: '{:.0f}'.format(round(x/1000000)))
        ax.yaxis.set_major_formatter(formatter)
    else:
        ax.set_ylabel('Count')

    ax.set_ylim(ymin=0)

    ax.set_title(metric)
    ax.grid(True)
    ax.set_xticks(range(1, 19))
    ax.set_xticklabels(range(1, 19))
    ax.set_xlabel('Hash bits')



    plt.savefig(directory + "/charts/" + metric + ".png", bbox_inches='tight')
