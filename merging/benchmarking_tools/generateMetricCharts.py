import pandas as pd
import matplotlib.pyplot as plt
import matplotlib.ticker as ticker
import sys
import os

# directory should be the path to the folder of either CONCURRENT_OUTPUT or COUNT_THEN_MOVE
directory = sys.argv[1] + "/averages"
file_list = os.listdir(directory)
# print(file_list)

if not os.path.exists(directory + '/charts'):
    os.makedirs(directory + '/charts')

for file in file_list:
    if not os.path.isfile(directory + "/" + file):
        continue
    
    metric = file[:-4]
    df = pd.read_csv(directory + "/" + file, names=['Category', "tcc", "clang-default", "clang-O2", "gcc-default", "gcc-O2", "gcc-O3", "gcc-Os"], skiprows=1)
    if file.__contains__("ms-elapsed"):
        df.iloc[:, 1:] = 10000000 / (df.iloc[:, 1:]/1000)

    ax = df.plot(kind='line', x='Category', y=["tcc", "clang-default", "clang-O2", "gcc-default", "gcc-O2", "gcc-O3", "gcc-Os"], marker='o', legend=True)
    plt.legend(title="Compiler")
    
    if file.__contains__("ms-elapsed"):
        ax.set_ylabel("Sorted items per second")
        ax.set_title("Sorted items per second")
    else:
        ax.set_ylabel(metric)
        ax.set_title(metric)

    ax.set_ylim(ymin=0)

    ax.grid(True)
    ax.set_xticks(range(0, 6))
    ax.set_xticklabels(range(0, 6))
    ax.set_xlabel('Depth')



    plt.savefig(directory + "/charts/" + metric + ".png", bbox_inches='tight')
