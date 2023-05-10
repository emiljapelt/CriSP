import pandas as pd
import matplotlib.pyplot as plt
import matplotlib.ticker as ticker
import sys
import os

laptopDir = sys.argv[1]
serverDir = sys.argv[2]
metrics_list = os.listdir(laptopDir + "/averages")

if not os.path.exists(laptopDir + '/comparative_charts'):
    os.makedirs(laptopDir + '/comparative_charts')

# Loop through all files in the directory
for metric in metrics_list:
    data = {}
    if not os.path.isfile(laptopDir + "/averages/" + metric):
        continue
    
    # Read CSV file into a Pandas DataFrame
    df1 = pd.read_csv(laptopDir + "/averages/" + metric, index_col=0)
    df2 = pd.read_csv(serverDir + "/averages/" + metric, index_col=0)

    if metric.__contains__("ms-elapsed"):
        df1.iloc[:, 1:] = 10000000 / (df1.iloc[:, 1:]/1000)
        df2.iloc[:, 1:] = 10000000 / (df2.iloc[:, 1:]/1000)

    # Add the right-most column to the data dictionary with the metric name as key
    metric = metric[:-4]  # Remove '.csv' from file name
    data["laptop"] = df1.iloc[:, -4]
    data["server"] = df2.iloc[:, -4]

    # Create a new DataFrame from the data dictionary
    df = pd.DataFrame(data)

    ax = df.plot(kind='line', marker='o', legend=True)
    plt.legend(title="Method")
    if metric.__contains__("ms-elapsed"):
        ax.set_ylabel("Sorted items per second")
        ax.set_title("Sorted items per second comparison, gcc-default")
    else:
        ax.set_ylabel(metric)
        ax.set_title(metric+ " comparison, gcc-default")
    ax.grid(True)
    ax.set_xticks(range(0, 6))
    ax.set_xticklabels(range(0, 6))
    ax.set_xlabel('Depth')
    ax.set_ylim(ymin=0)
    plt.savefig(laptopDir + "/comparative_charts/" + metric + ".png", bbox_inches='tight')