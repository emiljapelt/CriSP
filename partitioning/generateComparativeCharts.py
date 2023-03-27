import pandas as pd
import matplotlib.pyplot as plt
import matplotlib.ticker as ticker
import sys
import os

# directory should be the path to the folder containing the two folders: CONCURRENT_OUTPUT and COUNT_THEN_MOVE
directory = sys.argv[1]
metrics_list = os.listdir(directory + "/CONCURRENT_OUTPUT")

if not os.path.exists(directory + '/comparative_charts'):
    os.makedirs(directory + '/comparative_charts')

# Loop through all files in the directory
for metric in metrics_list:
    data = {}
    if not os.path.isfile(directory + "/CONCURRENT_OUTPUT/" + metric):
        continue
    
    # Read CSV file into a Pandas DataFrame
    df1 = pd.read_csv(directory + "/CONCURRENT_OUTPUT/" + metric, index_col=0)
    df2 = pd.read_csv(directory + "/COUNT_THEN_MOVE/" + metric, index_col=0)

    if metric.__contains__("timing"):
        df1.iloc[:, 1:] = 16777216 / (df1.iloc[:, 1:]/1000)
        df2.iloc[:, 1:] = 16777216 / (df2.iloc[:, 1:]/1000)

    # Add the right-most column to the data dictionary with the metric name as key
    metric = metric[:-4]  # Remove '.csv' from file name
    data["concurrent output"] = df1.iloc[:, -1]
    data["count then move"] = df2.iloc[:, -1]

    # Create a new DataFrame from the data dictionary
    df = pd.DataFrame(data)

    # Plot the values in the '32' column of the DataFrame
    ax = df.plot(kind='line', marker='o', legend=True)
    plt.legend(title="Method")
    if metric.__contains__("timing"):
        ax.set_ylabel('Million tuples per second')
        ax.set_title("throughput comparison, 32 threads")
        formatter = ticker.FuncFormatter(lambda x, pos: '{:.0f}'.format(round(x/1000000)))
        ax.yaxis.set_major_formatter(formatter)
    else:
        ax.set_ylabel(metric)
        ax.set_title(metric+ " comparison, 32 threads")
    ax.grid(True)
    ax.set_xticks(range(1, 19))
    ax.set_xticklabels(range(1, 19))
    ax.set_xlabel('Hash bits')
    ax.set_ylim(ymin=0)
    plt.savefig(directory + "/comparative_charts/" + metric + ".png", bbox_inches='tight')